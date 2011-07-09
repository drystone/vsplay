/* MPEG Sound library

   (C) 1997 by Woo-jae Jung */

// Mpegsound.h
//   This is typeset for functions in MPEG Sound library.
//   Now, it's for only linux-pc-?86

/************************************/
/* Inlcude default library packages */
/************************************/
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#ifdef ALSA
#include <alsa/asoundlib.h>
#endif // ALSA

#ifndef _L__SOUND__
#define _L__SOUND__

/****************/
/* Sound Errors */
/****************/
// General error
#define SOUND_ERROR_OK                0
#define SOUND_ERROR_FINISH           -1

// Device error (for player)
#define SOUND_ERROR_DEVOPENFAIL       1
#define SOUND_ERROR_DEVBUSY           2
#define SOUND_ERROR_DEVWRITEFAIL      3
#define SOUND_ERROR_DEVCTRLERROR      4

// Sound file (for reader)
#define SOUND_ERROR_FILEOPENFAIL      5
#define SOUND_ERROR_FILEREADFAIL      6

// Network
#define SOUND_ERROR_UNKNOWNPROXY      7
#define SOUND_ERROR_UNKNOWNHOST       8
#define SOUND_ERROR_SOCKET            9
#define SOUND_ERROR_CONNECT          10
#define SOUND_ERROR_FDOPEN           11
#define SOUND_ERROR_HTTPFAIL         12
#define SOUND_ERROR_HTTPWRITEFAIL    13
#define SOUND_ERROR_TOOMANYRELOC     14

// Miscellneous (for translater)
#define SOUND_ERROR_MEMORYNOTENOUGH  15
#define SOUND_ERROR_EOF              16
#define SOUND_ERROR_BAD              17

#define SOUND_ERROR_UNKNOWN          19

/**************************/
/* Define values for MPEG */
/**************************/
#define SCALEBLOCK     12
#define CALCBUFFERSIZE 512
#define MAXSUBBAND     32
#define MAXCHANNEL     2
#define MAXTABLE       2
#define SCALE          32768
#define MAXSCALE       (SCALE-1)
#define MINSCALE       (-SCALE)
#define RAWDATASIZE    (2*2*32*SSLIMIT)

#define LS 0
#define RS 1

#define SSLIMIT      18
#define SBLIMIT      32

#define WINDOWSIZE    4096

// Huffmancode
#define HTN 34

#define MODE_MONO   0
#define MODE_STEREO 1

/********************/
/* Type definitions */
/********************/
typedef float REAL;

typedef struct
{
  bool         generalflag;
  unsigned int part2_3_length;
  unsigned int big_values;
  unsigned int global_gain;
  unsigned int scalefac_compress;
  unsigned int window_switching_flag;
  unsigned int block_type;
  unsigned int mixed_block_flag;
  unsigned int table_select[3];
  unsigned int subblock_gain[3];
  unsigned int region0_count;
  unsigned int region1_count;
  unsigned int preflag;
  unsigned int scalefac_scale;
  unsigned int count1table_select;
}layer3grinfo;

typedef struct
{
  unsigned main_data_begin;
  unsigned private_bits;
  struct
  {
    unsigned scfsi[4];
    layer3grinfo gr[2];
  }ch[2];
}layer3sideinfo;

typedef struct
{
  int l[23];            /* [cb] */
  int s[3][13];         /* [window][cb] */
}layer3scalefactor;     /* [ch] */

typedef struct
{
  int tablename;
  unsigned int xlen,ylen;
  unsigned int linbits;
  unsigned int treelen;
  const unsigned int (*val)[2];
}HUFFMANCODETABLE;

struct Vsplayexception
{
  int error;
  Vsplayexception(int e) { error = e; };
};

class Bitstream
{
private:
  unsigned int prefetched_bits, prefetched_count, prefetched_size;
protected:
  unsigned char * buffer;
  uint16_t byteoffset, freeoffset;
  unsigned int bitoffset;
public:
  Bitstream() : buffer(new unsigned char[USHRT_MAX+1]), byteoffset(0), bitoffset(0), freeoffset(0) {};
  inline unsigned int getavailable() const
  {
    return freeoffset >= byteoffset 
           ? freeoffset - byteoffset 
           : USHRT_MAX - byteoffset - freeoffset;
  }
  inline void bytealign(void) { if (bitoffset) { ++byteoffset; bitoffset = 0; } };
  inline unsigned int getalignedbyte() {
    return buffer[byteoffset++];
  };
  unsigned int getbits(unsigned int bits) {
    register int a = buffer[byteoffset] & (0xff >> bitoffset);
    if (bits >= 8)
    {
      a = (a << 8) | buffer[++byteoffset];
      if (bits >= 16)
      {
        a = (a << 8) | buffer[++byteoffset];
        if (bits >= 24)
          a = (a << 8) | buffer[++byteoffset];
      }
    }
    if (bitoffset + (bits & 7) >= 8)
      a = (a << 8 ) | buffer[++byteoffset];

    bitoffset = (bits + bitoffset) & 7;
    return a >> (8-bitoffset);
  };
  inline unsigned int getbit() {
    if (++bitoffset == 8) {
      bitoffset = 0;
      return buffer[byteoffset++] & 1;
    }
    else return (buffer[byteoffset] & (1 << 8-bitoffset)) ? 1 : 0;
  };
  inline bool isbytealigned() { return bitoffset == 0; };
  inline void prefetch(unsigned int count, unsigned int size) {
    prefetched_size = size;
    prefetched_count = count;
    prefetched_bits = getbits(size * count);
  };
  inline unsigned int fetch() {
    return (prefetched_bits >> (--prefetched_count * prefetched_size)) & ~(0xffffffff << prefetched_size);
  };
};

/*********************************/
/* Sound input interface classes */
/*********************************/
// Superclass for Inputbitstream // Yet, Temporary
class Soundinputstream
{
protected:
  FILE *_fp;
public:
  Soundinputstream() : _fp(NULL) {};
  virtual ~Soundinputstream() { if (_fp) fclose(_fp); };

  virtual void open(const char *filename)
    throw (Vsplayexception) = 0;
  bool eof(void)
    { return feof(_fp); };
  size_t read(unsigned char *buffer, size_t size)
    { return fread(buffer, 1, size, _fp); };
  void skip(size_t len) { fseek(_fp, len, SEEK_CUR); };
};

// Inputstream from file
class Soundinputstreamfromfile : public Soundinputstream
{
public:
  void open(const char *filename) throw (Vsplayexception)
  {
    _fp = strcmp(filename, "-") ? fopen(filename, "r") : stdin;
    if (!_fp) throw Vsplayexception(SOUND_ERROR_FILEOPENFAIL);
  };
};

// Inputstream from http
class Soundinputstreamfromhttp : public Soundinputstream
{
public:
  void open(const char *filename) throw (Vsplayexception);
};


/**********************************/
/* Sound player interface classes */
/**********************************/
// Superclass for player
class Soundplayer
{
public:
  virtual ~Soundplayer() {};
  virtual void initialize(const char *filename)
    throw (Vsplayexception) = 0;
  virtual void setsoundtype(int stereo, int samplesize, int speed)
    throw (Vsplayexception) = 0;
  virtual void putblock(void *buffer,int size)
    throw (Vsplayexception) = 0;
  virtual void resetsoundtype(void)
    throw (Vsplayexception) {};
  virtual void abort(void) {};
  virtual void drain(void) {};
};


class Rawtofile : public Soundplayer
{
public:
  ~Rawtofile();
  void initialize(const char *filename)
    throw (Vsplayexception);
  void setsoundtype(int stereo,int samplesize,int speed)
    throw (Vsplayexception);
  void putblock(void *buffer,int size)
    throw (Vsplayexception);

private:
  int filehandle;
  int rawstereo,rawsamplesize,rawspeed;
};

// Class for playing raw data
class Rawplayer : public Soundplayer
{
public:
  ~Rawplayer();

  void initialize(const char *filename)
    throw (Vsplayexception);
  void abort(void);

  void setsoundtype(int stereo,int samplesize,int speed)
    throw (Vsplayexception);
  void resetsoundtype(void)
    throw (Vsplayexception);

  void putblock(void *buffer,int size)
    throw (Vsplayexception);

  int  getblocksize(void);

  static const char *defaultdevice;
  static int  setvolume(int volume);
  void drain(void) {};

private:
  short int rawbuffer[RAWDATASIZE];
  int  rawbuffersize;
  int  audiohandle,audiobuffersize;
  int  rawstereo,rawsamplesize,rawspeed;
  bool forcetomono,forceto8;
};

#ifdef ALSA
// Class for alsa playing raw data 
class Rawplayeralsa : public Soundplayer
{
public:
  Rawplayeralsa();
  ~Rawplayeralsa();

  void initialize(const char *filename)
    throw (Vsplayexception);
  void setsoundtype(int stereo,int samplesize,int speed)
    throw (Vsplayexception);
  void putblock(void *buffer,int size)
    throw (Vsplayexception);
  void abort(void);
  void drain() { snd_pcm_drain(_device_handle); };

private:
  snd_pcm_t *_device_handle;
  snd_pcm_hw_params_t *_hw_params;
  unsigned int _framesize;
  bool _abort_flag;
};
#endif // ALSA

/*********************************/
/* Data format converter classes */
/*********************************/
// Class for Mpeg layer3
class Mpegbitwindow : public Bitstream
{
  unsigned int point;
public:
  Mpegbitwindow() : point(0) {};
  void initialize(void) { byteoffset = 0; bitoffset = 0; point = 0; };
  int  gettotalbit(void) const {return (byteoffset << 3) + bitoffset; };
  void putbyte(int c) { buffer[point & (WINDOWSIZE - 1)] = c; point++; };
  void wrap(void)
  {
    point &= (WINDOWSIZE - 1);
    if (byteoffset >= point)
    {
      for (register int i = 4; i < point; i++)
        buffer[WINDOWSIZE + i]=buffer[i];
    }
    *((int *)(buffer+WINDOWSIZE))=*((int *)buffer);
  }
  void rewind(int bits) { byteoffset -= (bits >> 3); if ((bits & 7) > bitoffset) { bitoffset += 8; --byteoffset; } bitoffset -= (bits & 7); };
  void forward(int bits) { byteoffset += (bits >> 3); bitoffset += (bits & 7); if (bitoffset >= 8) { bitoffset -=8; ++byteoffset; } };
  inline unsigned int getbits9(int bits) { return getbits(bits); };
};

// Class for converting mpeg format to raw format
class Mpegtoraw : Bitstream
{
  /*****************************/
  /* Constant tables for layer */
  /*****************************/
private:
  static const int bitrate[2][3][15],frequencies[2][3];
  static const REAL scalefactorstable[64];
  static const HUFFMANCODETABLE ht[HTN];
  static const REAL filter[512];
  static REAL hcos_64[16],hcos_32[8],hcos_16[4],hcos_8[2],hcos_4;

  /*************************/
  /* MPEG header variables */
  /*************************/
private:
  int layer,protection,bitrateindex,padding,extendedmode;
  enum _mpegversion  {mpeg1,mpeg2}                               version;
  enum _mode    {fullstereo,joint,dual,single}                   mode;
  enum _frequency {frequency44100, frequency48000, frequency32000} frequency;

  /*******************************************/
  /* Functions getting MPEG header variables */
  /*******************************************/
public:
  // General
  int  getversion(void)   const {return version;};
  int  getlayer(void)     const {return layer;};
  bool getcrccheck(void)  const {return (!protection);};
  // Stereo or not
  int  getmode(void)      const {return mode;};
  bool isstereo(void)     const {return (getmode()!=single);};
  // Frequency and bitrate
  int  getfrequency(void) const {return frequencies[version][frequency];};
  int  getbitrate(void)   const {return bitrate[version][layer-1][bitrateindex];};

  /***************************************/
  /* Interface for setting music quality */
  /***************************************/
private:
  bool forcetomonoflag;
  int  downfrequency;

public:
  void setforcetomono(bool flag);
  void setdownfrequency(int value);

  /******************************************/
  /* Functions getting other MPEG variables */
  /******************************************/
public:
  bool getforcetomono(void);
  int  getdownfrequency(void);

  /***************************************/
  /* Variables made by MPEG-Audio header */
  /***************************************/
private:
  int tableindex;
  int stereobound,subbandnumber,inputstereo,outputstereo;
  REAL scalefactor;
  int framesize;

  /*******************/
  /* Mpegtoraw class */
  /*******************/
public:
  Mpegtoraw(Soundinputstream *loader,Soundplayer *player);
  bool run(int frames);
  int  geterrorcode(void) {return __errorcode;};

private:
  int __errorcode;
  bool seterrorcode(int errorno){__errorcode=errorno;return false;};

  /*****************************/
  /* Loading MPEG-Audio stream */
  /*****************************/
private:
  Soundinputstream *loader;   // Interface
  void preload(size_t size);
  void skip(size_t len);

  /********************/
  /* Global variables */
  /********************/
private:
  int lastfrequency,laststereo;

  // for Layer3
  int layer3slots,layer3framestart,layer3part2start;
  REAL prevblck[2][2][SBLIMIT][SSLIMIT];
  int currentprevblock;
  layer3sideinfo sideinfo;
  layer3scalefactor scalefactors[2];

  Mpegbitwindow bitwindow;

  /*************************************/
  /* Decoding functions for each layer */
  /*************************************/
private:
  bool loadframe(void);

  //
  // Subbandsynthesis
  //
  REAL calcbufferL[2][CALCBUFFERSIZE],calcbufferR[2][CALCBUFFERSIZE];
  int  currentcalcbuffer,calcbufferoffset;

  void computebuffer(REAL *fraction,REAL buffer[2][CALCBUFFERSIZE]);
  void generatesingle(void);
  void generate(void);
  void subbandsynthesis(REAL *fractionL,REAL *fractionR);

  void computebuffer_2(REAL *fraction,REAL buffer[2][CALCBUFFERSIZE]);
  void generatesingle_2(void);
  void generate_2(void);
  void subbandsynthesis_2(REAL *fractionL,REAL *fractionR);

  // Extarctor
  void extractlayer1(void);    // MPEG-1
  void extractlayer2(void);
  void extractlayer3(void);
  void extractlayer3_2(void);  // MPEG-2


  // Functions for layer 3
  void layer3initialize(void);
  bool layer3getsideinfo(void);
  bool layer3getsideinfo_2(void);
  void layer3getscalefactors(int ch,int gr);
  void layer3getscalefactors_2(int ch);
  void layer3huffmandecode(int ch,int gr,int out[SBLIMIT][SSLIMIT]);
  REAL layer3twopow2(int scale,int preflag,int pretab_offset,int l);
  REAL layer3twopow2_1(int a,int b,int c);
  void layer3dequantizesample(int ch,int gr,int   in[SBLIMIT][SSLIMIT],
			                    REAL out[SBLIMIT][SSLIMIT]);
  void layer3fixtostereo(int gr,REAL  in[2][SBLIMIT][SSLIMIT]);
  void layer3reorderandantialias(int ch,int gr,REAL  in[SBLIMIT][SSLIMIT],
				               REAL out[SBLIMIT][SSLIMIT]);

  void layer3hybrid(int ch,int gr,REAL in[SBLIMIT][SSLIMIT],
		                  REAL out[SSLIMIT][SBLIMIT]);
  
  void huffmandecoder_1(const HUFFMANCODETABLE *h,int *x,int *y);
  void huffmandecoder_2(const HUFFMANCODETABLE *h,int *x,int *y,int *v,int *w);

  /********************/
  /* Playing raw data */
  /********************/
private:
  Soundplayer *player;       // General playing interface
  int       rawdataoffset;
  short int rawdata[RAWDATASIZE];

  void clearrawdata(void)    {rawdataoffset=0;};
  void putraw(short int pcm) {rawdata[rawdataoffset++]=pcm;};
  void flushrawdata(void);
};



/***********************/
/* File player classes */
/***********************/
// Superclass for playing file
class Fileplayer
{
public:
  Fileplayer();
  virtual ~Fileplayer();

  int geterrorcode(void)        {return __errorcode;};

  void setoutput(Soundplayer * p) { player = p; };
  virtual bool openfile(const char *filename)=0;
  virtual void setforcetomono(bool flag)            =0;
  virtual bool playing(int verbose) = 0;
  void abort() { _abort_flag = true; }

protected:
  bool seterrorcode(int errorno){__errorcode=errorno;return false;};
  Soundplayer *player;
  bool _abort_flag;

private:
  int __errorcode;
};


// Class for playing MPEG file
class Mpegfileplayer : public Fileplayer
{
public:
  Mpegfileplayer();
  ~Mpegfileplayer();

  bool openfile(const char *filename);
  void setforcetomono(bool flag);
  void setdownfrequency(int value);
  bool playing(int verbose);

private:
  Soundinputstream *loader;
  Mpegtoraw *server;

  void showverbose(int verbose);
};

#endif
