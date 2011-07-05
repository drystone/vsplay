/* Sound Player

   Copyright (C) 1997 by Woo-jae Jung */

// It's an example of using MPEG Sound library

// Anyone can use MPEG Sound library under GPL

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <iostream>
#include <iomanip>
using namespace std;

#ifdef TAGLIB
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#endif

#include "mpegsound.h"

#include "vsplay.h"

static const char *help=
"\t-2 : playing with half frequency.\n"
"\t-e : exit when playing is done. (only XSPLAY)\n"
"\t-f : display frame and time info (played and remaining).\n"
"\t-m : force to mono.\n"
"\t-r : repeat forever.\n"
"\t-s : shuffle play.\n"
"\t-v : verbose, Very verbose, Very very verbose.\n"
"\t-M : playing MPEG-Audio file as standard input.\n"
"\t-V : display version.\n"
"\n"
"\t-k num : start playing at frame num.\n"
"\t-d dev  : Set dev as device or file.\n"
"\t-l list : list file.\n"
"\t-t buf  : Set number of buffer. (default : 50)\n"
"\n"
"For detail, see man pages.\n";

typedef enum {devicetype_oss, devicetype_alsa} Devicetype;
bool single_threaded = false;
Mpegfileplayer * g_player = NULL;

/***********************/
/* Command line player */
/***********************/
inline void error(int n)
{
  cerr << vsplay_progname << " " << vsplay_Sounderrors[n-1] << endl;
}

static void playing(Fileplayer *player)
{
  if(player->geterrorcode()>0)error(player->geterrorcode());
  else
  {
    player->setforcetomono(vsplay_forcetomonoflag);
#ifdef PTHREADEDMPEG
    if (!single_threaded)
    {
      player->playingwiththread(vsplay_verbose-1,vsplay_frameinfo,
  			        vsplay_threadnum, vsplay_startframe); 
    }
    else
    {
      player->playing(vsplay_verbose-1,vsplay_frameinfo,vsplay_startframe);
    }
#else
    player->playing(vsplay_verbose-1,vsplay_frameinfo,vsplay_startframe);
#endif  // PTHREADEDMPEG
    if(player->geterrorcode()>0)error(player->geterrorcode());
  }
}

#ifdef TAGLIB
ostream& operator<<(ostream& s, const TagLib::Tag& tag )
{
  s.setf(ios::left);
  s << "Title : " << tag.title() << endl
    << "Artist: " << setw(30) << tag.artist()
    << "Album: " << tag.album() << endl
    << "Genre : " << setw(18) << tag.genre()
    << "Track: " << setw(5) << tag.track()
    << "Year: " <<  setw(6) << tag.year()
    << endl << flush; 
  return s;
}
#endif /* TAGLIB */

static void play(char *filename, Soundplayer* device)
{
  if( vsplay_verbose-- )
    cout << filename << ":" << endl;

#ifdef TAGLIB
  if( vsplay_verbose>0 && strcmp(filename, "-") != 0) // cant do ID3 stuff on stdin
    {
      cout << *TagLib::MPEG::File(filename).tag();
    }
#endif /* TAGLIB */        
  
  g_player = new Mpegfileplayer(device);
  if(!g_player->openfile(strcmp(filename,"-")==0?NULL:filename))
      error(g_player->geterrorcode());
  else
  {
      g_player->setdownfrequency(vsplay_downfrequency);
      playing(g_player);
  }
  delete g_player;
  g_player = NULL;
}

void sigint_abort(int)
{
  // TODO possible problem here if g_player is being deleted
  if (g_player)
    g_player->abort();
}

void sigint_capture(int)
{
  static struct timeval last = {0, 0};

  struct timeval now;
  gettimeofday(&now, NULL);

  if (last.tv_sec)
  {
    long diff = (now.tv_sec - last.tv_sec) * 1000 
              + (now.tv_usec - last.tv_usec) / 1000;
    if (diff < 250)
      exit(0);
  }
  last.tv_sec = now.tv_sec;
  last.tv_usec = now.tv_usec;
}

Soundplayer * open_device(const char * devicename, Devicetype devicetype) throw (int)
{
  Soundplayer * device = NULL;
  switch (devicetype)
  {
  case devicetype_oss:
    if (!devicename)
    {
      const char * const default_devices[] = { "/dev/sound/dsp", "/dev/dsp", 0 };
      for (int i = 0; default_devices[i]; ++i)
      {
        int fd = open(default_devices[i], O_WRONLY|O_NDELAY);
        if (fd != -1)
        {
          close(fd);
          devicename = default_devices[i];
          break;
        }
      }
      if (!devicename)
      {
        throw SOUND_ERROR_DEVOPENFAIL;
      }
    }
    if (devicename[0] == '/')
    { // assume device
      device = new Rawplayer;
    }
    else
    { // file or stdout
      device = new Rawtofile;
    }
    if (devicename[0] == '-') devicename = NULL;  // clear device name for correct stdout initialisation
    break;
    
#ifdef ALSA
  case devicetype_alsa:
    if (!devicename)
      devicename = "default";
    device = new Rawplayeralsa;
    break;
#endif // ALSA
  }

  if (!device)
  { // memory allocation problem
    throw SOUND_ERROR_MEMORYNOTENOUGH;
  }
  else if (!device->initialize(devicename))
  { // initialisation problem
    int error = device->geterrorcode();
    delete device;
    throw error;
  }
  return device;
}
      
int main(int argc,char *argv[])
{
  int c;
  Devicetype devicetype = devicetype_alsa;
  char const * devicename = NULL;
  bool stdin_only = false;

  vsplay_progname=argv[0];

  while((c=getopt(argc,argv,
		  "VM2mfrsvd:k:l:S"
#ifdef PTHREADEDMPEG
		  "t:"
#endif
		  ))>=0)
  {
    switch(c)
    {
      case 'V':printf("%s %s"
#ifdef PTHREADEDMPEG
		      " with pthread"
#endif
		      "\n",PACKAGE,VERSION);
               return 0;
    case 'M':stdin_only           =true;break;
    case '2':vsplay_downfrequency  =   1;break;
    case 'f':vsplay_frameinfo      =true;break;
    case 'm':vsplay_forcetomonoflag=true;break;
    case 'r':vsplay_repeatflag     =true;break;
    case 's':vsplay_shuffleflag    =true;break;
    case 'v':vsplay_verbose++;           break;
      
    case 'd':devicename=optarg;   break;
    case 'k':vsplay_startframe=atoi(optarg);   break;
    case 'l':if(vsplay_verbose)
		fprintf(stderr,"List file : %s\n",optarg);
	       readlist(optarg);
	       break;
#ifdef PTHREADEDMPEG
      case 't':
	{
	  int a;

	  sscanf(optarg,"%d",&a);
	  vsplay_threadnum=a;
	}
	break;
#endif
    case 'S':single_threaded = true; break;
    default:fprintf(stderr,"Bad argument.\n");
    }
  }

  if(argc<=optind && vsplay_listsize==0)
  {
    printf("%s %s"
#ifdef PTHREADEDMPEG
	   " with pthread"
#endif
	   "\n"
	   "Usage : vsplay [-2mrsvMVWS] [-d device] [-l listfile] "
#ifdef PTHREADEDMPEG
	   "[-t number] "
#endif
	   "files ...\n"
	   "\n"
	   "%s"
	   ,PACKAGE,VERSION,help);
    return 0;
  }

  if(vsplay_listsize==0)    // Make list by arguments
    arglist(argc,argv,optind);

  // create the output device
  Soundplayer* device;
  try
  {
    device = open_device(devicename, devicetype);
  }
  catch (int e)
  {
    error(e);
    return 1;
  }

  do
  {
    if(vsplay_shuffleflag)shufflelist();

    for(int i=0;i<vsplay_listsize;i++){
      if (!single_threaded)
      { // normal mode
        if ( fork() )
        {
          signal(SIGINT, sigint_capture);
          wait(NULL);
        }
        else
        {
          signal(SIGINT, sigint_abort);
          play(vsplay_list[i], device);
          exit(0);
        }
      }
      else
      { // single threaded mode
	play(vsplay_list[i], device);
      }
    }
  }while(vsplay_repeatflag);

  delete device;
  
  return 0;
}

