/* Sound Player

   Copyright (C) 1997 by Woo-jae Jung */

// It's an example of using MPEG Sound library

// This is X interface of splay using QT

// Now, it require pthread.
// But X library and pthread can't work together now...
// Then if you want to use xsplay, you must get patched X-library

// You can get patched X-library from http://adam.kaist.ac.kr/~jwj95/

// If you don't want to use patched X-library,
//   you can get splay which is compiled in statically.
// Anyone can use MPEG Sound library under GPL

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifdef HAVE_LIBID3
#include <id3/tag.h>
#include <id3/misc_support.h>
#endif /* HAVE_LIBID3 */

#include <fcntl.h>
#include <string>
#include <unistd.h>

#include <iostream>
#include <iomanip>

#include "mpegsound.h"
#include "xsplay.h"

static struct
{
  bool pause,quit;
  bool setframeflag;
  int  setframenumber;
}music;

static struct
{
  bool stop;
  bool restart;
  int  move,currentrun;
  pthread_mutex_t movelock;
  bool errorflag;
  int  errorcode;
}musics;

char *nullstring="";
char *stopstring="Stop";
char *nonestring="None";

static pthread_mutex_t startuplock;

/**********************/
/* Setting music flag */
/**********************/
// internal
static void music_move(int value)
{
  pthread_mutex_lock(&musics.movelock);
  musics.move+=value;
  pthread_mutex_unlock(&musics.movelock);
}

inline void music_done(void)
{
  music_move(1);
}

inline void music_term(void)
{
  music.pause=false;
  music.quit=true;
}

// external 
bool music_isstop(void)
{
  return musics.stop;
}

bool music_ispause(void)
{
  return music.pause;
}

void music_restart(void)
{
  musics.restart=true;
}

void music_stop(void)
{
  musics.stop=true;
  music_term();
}

void music_play(void)
{
  music.pause=false;
  music.quit=false;
  musics.stop=false;
}

void music_pause(void)
{
  music.pause=true;
}

void music_previous(void)
{
  music_move(-1);
  music_term();
}

void music_next(void)
{
  music_move(1);
  music_term();
}

/*******************/
/* Frame functions */
/*******************/
int getslidernumber(int framenumber,int maxframe)
{
  register unsigned int a;

  a=MAXFRAMESLIDER*(unsigned int)framenumber;
  a/=(unsigned int)maxframe;

  return (int)a;
}

static int getframenumber(int slidernum,int maxframe)
{
  register unsigned int a;

  a=(unsigned int)maxframe*(unsigned int)slidernum;
  a/=(unsigned int)MAXFRAMESLIDER;

  return (int)a;
}

static int adjustframe(int slider,int maxframe)
{
  int f,s;

  if(maxframe==0)return 0;
  f=getframenumber(slider,maxframe);
  for(;;)
  {
    s=getslidernumber(f,maxframe);
    if(s>=slider)break;
    f++;
  }

  return f;
}



//Wrapper to convert null-string to the empty string
inline const char * nn(const char* str)
{
  return str?str:"";
}

/*************************************/
/* Funcitions playing MPEG file */
/*************************************/
static void seterrorcode(int errcode)
{
  musics.errorcode=errcode;
  musics.errorflag=true;
  music_done();
}

static void xplaympeg(char *filename,Soundinputstream *loader,Rawplayer *player)
{
  Mpegtoraw *server;
  bool threadflag=(splay_threadnum>=20);
  int panelupdate=0;

  int pcmperframe,frequency;
  int updatedelay;

// Server
  if((server=new Mpegtoraw(loader,player))==NULL)
  {
    seterrorcode(SOUND_ERROR_MEMORYNOTENOUGH);
    return;
  }
  server->initialize();
  server->setforcetomono(splay_forcetomonoflag);
  server->setdownfrequency(splay_downfrequency);
  if(threadflag)server->makethreadedplayer(splay_threadnum);

#ifdef HAVE_LIBID3
  {
	ID3_Tag mytag(filename);
	if ( mytag.HasV1Tag() || mytag.HasV2Tag() )
    {
	  Setsongname(nn(ID3_GetTitle(&mytag)));
	  Setsongmusican(nn(ID3_GetArtist(&mytag)));
	}
  }
#endif /* HAVE_LIBID3 */

  music.quit=music.pause=
  music.setframeflag=false;

  if(!server->run(-1))      // Initialize MPEG player
  {
    Setcurrentmpegstatus(0,0,0,0,0,0,0,0);
    if(threadflag)server->freethreadedplayer();
    seterrorcode(server->geterrorcode());
    Paneldone();delete server;
    return;
  }

  pcmperframe=server->getpcmperframe();
  frequency=server->getfrequency();
  updatedelay=frequency/pcmperframe/6;

  if(getquotaflag())
  {
    int q=server->getfrequency()*(server->isstereo()+1);

    if(!splay_forcetomonoflag)q<<=1;

    q>>=1;

    player->setquota(1024);
  }

  Setcurrentmpegstatus(server->getversion(),    server->getlayer(),
		       server->getfrequency(),  server->getbitrate(),
		       server->getcrccheck(),   server->isstereo(),
		       server->getforcetomono(),server->getdownfrequency());
  Unlockframeslider();

  for(;;)
  {
    if(music.setframeflag)
    {
      int a;

      music.setframeflag=false;
      panelupdate=updatedelay;
      a=adjustframe(music.setframenumber,
		    server->gettotalframe());
      server->setframe(a);
      Unlockframeslider();
      Setframestatus(server->getcurrentframe(),
		     0,
		     pcmperframe,frequency);
    }

    if(music.pause)         // When pause set,
    {
      if(threadflag)
      {
	server->pausethreadedplayer();
	server->setframe(server->getcurrentframe());
      }
      server->clearbuffer();
      while(music.pause)
	if(threadflag && (server->getframesaved()<(splay_threadnum-1)))
	{
	  server->run(1);
	  panelupdate=updatedelay;
	}
	else if(music.setframeflag)
	{
	  music.setframeflag=false;
	  server->setframe(adjustframe(music.setframenumber,
				       server->gettotalframe()));
	  Setframestatus(server->getcurrentframe(),
			 0,
			 pcmperframe,frequency);
	  Unlockframeslider();
	}
	else usleep(100);

      if(music.setframeflag)
      {
	panelupdate=-1;
	music.setframeflag=false;
	server->setframe(adjustframe(music.setframenumber,
				     server->gettotalframe()));
      }

      if(threadflag)server->unpausethreadedplayer();
    }

    if(panelupdate<0)
    {
      Setframestatus(server->getcurrentframe(),
		     server->gettotalframe(),
		     pcmperframe,frequency);
      panelupdate=updatedelay;
    }
    else panelupdate--;

    if(music.quit)
    {
      if(threadflag)
      {
	server->stopthreadedplayer();
	while(server->existthread())usleep(10);
      }
      player->abort();player->resetsoundtype();
      break;
    }
    if(!server->run(5))
    {
      if(!threadflag)
      {
	music_done();
	break;
      }
      if(server->getframesaved()==0)
      {
	music_done();
	break;
      }
      usleep(100);
    }
  }

  if(threadflag)
    server->freethreadedplayer();

  Paneldone();delete server;
  return;
}

static char *stripfilename(char *str)
{
  static char songname[30+1];

  char *ss;
  int p=0,s=0;

  for(;str[p];p++)
    if(str[p]=='/')
    {
      p++;
      s=p;
    }

  ss=str+s;
  for(p=0;p<30 && ss[p];p++)songname[p]=ss[p];
  songname[p]=0;

  return songname;
}

#define FRAMESIZE (4096)

static void xplayfile(char *filename)
{
  char *device=splay_devicename;
  Soundinputstream *loader;
  Rawplayer        *player;

// Loader
  {
    int err;

    if((loader=Soundinputstream::hopen(filename,&err))==NULL)
    {
      seterrorcode(err);
      return;
    }
  }

// Player
  if(device==NULL)device=Rawplayer::defaultdevice;
  if(device[0]!='/')device=Rawplayer::defaultdevice;
  player=new Rawplayer;

  if(player==NULL)
  {
    seterrorcode(SOUND_ERROR_MEMORYNOTENOUGH);
    return;
  }
  if(!player->initialize(device))
  {
    seterrorcode(player->geterrorcode());
    return;
  }

  xplaympeg(filename,loader,player);
  
  Clearcurrentstatus();


// Clean up
  delete loader;
  delete player;
}


/**********/
/* Player */
/**********/
static void xplay()
{
  bool updateflag=true;  // Control music_stop and music_move;

  musics.restart=true;
  musics.errorflag=false;
  pthread_mutex_init(&musics.movelock,NULL);


  for(;;)
  {
    if(musics.restart)
    {
      if(splay_listsize)
      {
	if(splay_shuffleflag)
        {
	  Paneldone();
	  shufflelist();
	}
	musics.stop=false;
	musics.restart=false;
	musics.currentrun=0;
	musics.move=0;
      }
      else musics.stop=true;
      updateflag=true;
    }

    if(musics.errorflag)
    {
      musics.errorflag=false;
      Displayerror(splay_Sounderrors[musics.errorcode-1]);
      while(!Msgboxdone())usleep(100);
    }

    if(musics.move!=0)
    {
      pthread_mutex_lock(&musics.movelock);
      musics.currentrun+=musics.move;

      if(musics.currentrun>=splay_listsize)
      {
	if(!musics.stop)
	{
	  if(splay_repeatflag) musics.restart=true;
	}
	musics.currentrun=splay_listsize;
	musics.stop=true;
      }

      if(musics.currentrun<0)musics.currentrun=0;

      musics.move=0;
      Setcurrentsongnumber(musics.currentrun,splay_listsize);
      pthread_mutex_unlock(&musics.movelock);
      updateflag=true;
    }

    if(updateflag)
    {
      char *str;

      if(musics.currentrun<splay_listsize)str=splay_list[musics.currentrun];
      else if(splay_listsize==0)str=nonestring;
      else if(splay_exitwhendone)exit(0);
      else str="End of list";
      Setsongpath(str);
      Setsongname(stripfilename(str));
      Setcurrentsongnumber(musics.currentrun,splay_listsize);
      updateflag=false;
    }

    if(musics.stop)usleep(500);             // Don't play
    else
    {
      if(musics.currentrun<splay_listsize)
      {
	xplayfile(splay_list[musics.currentrun]);
	updateflag=true;
      }
      else
	musics.restart=true;
    }
  }
}

void Setframe(int frame)
{
  music.setframenumber=frame;
  music.setframeflag=true;
}

static bool quotaflag=false;
bool getquotaflag(void)
{
  return quotaflag;
}

void setquotaflag(bool flag)
{
  quotaflag=flag;
}

/*********************************/
/* Initialize and main functions */
/*********************************/
inline void xinit(int& argc,char *argv[])
{
  pthread_mutex_init(&startuplock,NULL);
  pthread_mutex_lock(&startuplock);
  Getxarg(argc,argv);
}

static void *_startup(void *)
{
  pthread_mutex_lock(&startuplock);
  xplay();
  return NULL;
}

inline int xmain(void)
{
  pthread_t th;

  pthread_create(&th,NULL,_startup,NULL);
  SetupPanel();
  pthread_mutex_unlock(&startuplock);

  return RunPanel();
}


/***********************/
/* Command line player */
/***********************/
inline void error(int n)
{
  fprintf(stderr,"%s: %s\n",splay_progname,splay_Sounderrors[n-1]);
  return;
}

int main(int argc,char *argv[])
{
  int c;

  splay_progname=argv[0];

  xinit(argc,argv);

  while((c=getopt(argc,argv,
#ifdef USE_KOREAN
		  "h:"
#endif
		  "V2emrsvd:l:t:"
		  ))>=0)
  {
    switch(c)
    {
#ifdef USE_KOREAN
      case 'h':{
                 extern char *hanfontname;

		 hanfontname=optarg;
               }break;
#endif
      case 'V':printf("x%s %s\n",PACKAGE,VERSION);
               return 0;

      case '2':splay_downfrequency  =   1;break;
      case 'e':splay_exitwhendone   =true;break;
      case 'm':splay_forcetomonoflag=true;break;
      case 'r':splay_repeatflag     =true;break;
      case 's':splay_shuffleflag    =true;break;
      case 'v':splay_verbose++;           break;

      case 'd':splay_devicename=optarg;break;
      case 'l':if(splay_verbose)
		 fprintf(stderr,"List file : %s\n",optarg);
	       readlist(optarg);
	       break;
#ifdef PTHREADEDMPEG
      case 't':
	{
	  int a;

	  sscanf(optarg,"%d",&a);
	  splay_threadnum=a;
	}
	break;
#endif

      default:fprintf(stderr,"Bad argument.\n");
    }
  }

  if(splay_listsize==0)    // Make list by arguments
    arglist(argc,argv,optind);

  return xmain();
}
