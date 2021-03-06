/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Mpegtoraw.cc
// Server which get mpeg format and put raw format.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "mpegsound.h"

#define MY_PI 3.14159265358979323846


Mpegtoraw::Mpegtoraw(Soundinputstream *l, Soundplayer *p)
  : forcetomonoflag(false)
  , downfrequency(0)
  , loader(l)
  , _abort_flag(false)
  , player(p)
{
  register int i;

  scalefactor=SCALE;
  calcbufferoffset=15;
  currentcalcbuffer=0;

  for(i=CALCBUFFERSIZE-1;i>=0;i--)
    calcbufferL[0][i]=calcbufferL[1][i]=
    calcbufferR[0][i]=calcbufferR[1][i]=0.0;

  for(i=0;i<16;i++)hcos_64[i]=1.0/(2.0*cos(MY_PI*double(i*2+1)/64.0));
  for(i=0;i< 8;i++)hcos_32[i]=1.0/(2.0*cos(MY_PI*double(i*2+1)/32.0));
  for(i=0;i< 4;i++)hcos_16[i]=1.0/(2.0*cos(MY_PI*double(i*2+1)/16.0));
  for(i=0;i< 2;i++)hcos_8 [i]=1.0/(2.0*cos(MY_PI*double(i*2+1)/ 8.0));
  hcos_4=1.0/(2.0*cos(MY_PI*1.0/4.0));

  layer3initialize();
}

void Mpegtoraw::setforcetomono(bool flag)
{
  forcetomonoflag=flag;
}

void Mpegtoraw::setdownfrequency(int value)
{
  downfrequency=0;
  if(value)downfrequency=1;
}

bool Mpegtoraw::getforcetomono(void)
{
  return forcetomonoflag;
}

int Mpegtoraw::getdownfrequency(void)
{
  return downfrequency;
}

void Mpegtoraw::preload(size_t len)
{
  if (getavailable() < len)
  {
    size_t bytes = loader->read(&buffer[freeoffset], 4096);
    freeoffset += bytes;
  }
  if (getavailable() < len)
    throw Vsplayexception(SOUND_ERROR_EOF);
}

void Mpegtoraw::skip(size_t len)
{
  unsigned int available = getavailable();
  if (available >= len)
    byteoffset += len;
  else
  {
    byteoffset += available;
    loader->skip(len - available);
  }
}

void Mpegtoraw::loadframe(void)
  throw (Vsplayexception)
{
  bytealign();

  preload(4);
  // find frame start
  unsigned int header = (getalignedbyte() << 24)
                      + (getalignedbyte() << 16)
                      + (getalignedbyte() << 8)
                      + getalignedbyte();

  while ((header & 0xfff00000) != 0xfff00000)
  {
    // skip over if ID3 tag
    if (header >> 24 == 'I'
     && (header >> 16 & 0xff) == 'D'
     && (header >> 8 & 0xff) == '3'
     && (header & 0xff) != 0xff)
    {
      preload(6);
      for (unsigned int i = 0; i < 6; i++)
      {
        header = (header << 8) + getalignedbyte();
        if ((header & 0xff) == 0xff)
          break;
      }
      if ((header & 0xff) == 0xff)
        continue;

      size_t len = ((header >> 3) & (0x7f << 21)) 
                 + ((header >> 2) & (0x7f << 14))
                 + ((header >> 1) & (0x7f << 7))
                 + (header & 0x7f );

      skip(len);
 
      preload(4);
      header = (getalignedbyte() << 24)
             + (getalignedbyte() << 16)
             + (getalignedbyte() << 8)
             + (getalignedbyte());
    }
    else
    {
      preload(1);
      header = (header << 8) + getalignedbyte();
    }
  } 

  // deconstruct header
  version = (header & 0x00080000) ? mpeg1 : mpeg2;
  layer = 4 - ((header >> 17) & 3);
  protection = (header & 0x00010000) ? false : true;
  bitrateindex = (header >> 12) & 0xf;
  if (bitrateindex == 15)
    throw Vsplayexception(SOUND_ERROR_BAD);
  frequency=(_frequency)((header >> 10) & 3);
  padding = (header & 0x00000200) ? true : false;
  mode = (_mode)((header >> 6) & 3);
  extendedmode = (header >> 4) & 3;

  // Making information
  inputstereo = (mode == single) ? 0 : 1;
  outputstereo = forcetomonoflag ? 0 : inputstereo;

  int cbi = bitrateindex; // channel bitrate index
  if (inputstereo)
    cbi = (cbi == 4) ? 1 : cbi - 4;

  tableindex = 1;
  if (cbi == 1 || cbi == 2)
    tableindex = 0;

  if (layer == 1)
    subbandnumber = MAXSUBBAND;
  else
  {
    if (!tableindex)
      subbandnumber = (frequency == frequency32000) ? 12 : 8;
    else
      subbandnumber = 
        (frequency == frequency48000 || (cbi >= 3 && cbi <= 5))
        ? 27 : 30;
  }

  stereobound = subbandnumber;
  if (mode == single)
    stereobound = 0;
  else if (mode == joint)
  {
    stereobound = (extendedmode + 1) << 2;
    if (stereobound > subbandnumber)
      stereobound = subbandnumber;
  }

  // framesize & slots
  if (layer == 1)
  {
    framesize = 12000 * bitrate[version][0][bitrateindex]
                / frequencies[version][frequency];
    if (frequency == frequency44100 && padding)
      framesize++;
    framesize <<= 2;
  }
  else
  {
    framesize = 144000 * bitrate[version][layer-1][bitrateindex]
                / (frequencies[version][frequency] << version);
    if (padding)
      framesize++;
    if (layer == 3)
    {
      layer3slots = framesize - (protection ? 2 : 0) - 4;
      if (version == mpeg2)
	layer3slots -= (mode == single) ? 9 : 17;
      else
	layer3slots -= (mode == single) ? 17 : 32;
    }
  }

  preload(framesize - 4);

  if (protection) skip(2);
}
/*
newrun

while 1
  if (input.space)
    source.read()
  if (outbuf.length)
    device.write()
  while (input.length and output.space)
    decodeframe()
    if (source.blocked and device.blocked)
      select(0)
    if (!source.blocked or !device.blocked)
      break
  if (source.blocked and device.blocked)
    select(-1)
*/

// Convert mpeg to raw
void Mpegtoraw::run(int verbose)
  throw (Vsplayexception)
{
  static const char * modestr[] = {"stereo", "joint stereo", "dual channel", "mono"};
  loadframe();
  player->setsoundtype(outputstereo, 16, frequencies[version][frequency] >> downfrequency);
  if (verbose > 2)
    std::cout << "MPEG-" << (version + 1) << " Layer " << layer << ", " 
              << modestr[mode] << std::endl
              << frequencies[version][frequency] << (downfrequency ? "Hz/2, " : "Hz, ")
              << bitrate[version][layer-1][bitrateindex] << "kbit/s, "
              << (protection ? "with" : "without") << " crc check" << std::endl;
  
  while (!_abort_flag)
  {
    rawdataoffset = 0;
    if (layer == 3) extractlayer3();
    else if (layer == 2) extractlayer2();
    else if (layer == 1) extractlayer1();

    player->putblock((char *)rawdata, rawdataoffset << 1);

    _frequency f = frequency;
    try
    {
      loadframe();
    }
    catch (Vsplayexception &e)
    {
      if (e.error == SOUND_ERROR_EOF)
        break;
      throw e;
    }
    if(frequency != f)
      throw Vsplayexception(SOUND_ERROR_BAD);
  }
  // TODO cant abort if player is draining
  if (!_abort_flag)
    player->drain();
  else
    player->stop();
}

