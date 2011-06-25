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

#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <iomanip>
using namespace std;

#ifdef HAVE_LIBID3
#include <id3/tag.h>
#include <id3/misc_support.h>
#endif /* HAVE_LIBID3 */


#include "mpegsound.h"

#include "splay.h"

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

/***********************/
/* Command line player */
/***********************/
inline void error(int n)
{
  cerr << splay_progname << " " << splay_Sounderrors[n-1] << endl;
}

static void playing(Fileplayer *player)
{
  if(player->geterrorcode()>0)error(player->geterrorcode());
  else
  {
    player->setforcetomono(splay_forcetomonoflag);
#ifdef PTHREADEDMPEG
    if (!single_threaded)
    {
      player->playingwiththread(splay_verbose-1,splay_frameinfo,
  			        splay_threadnum, splay_startframe); 
    }
    else
    {
      player->playing(splay_verbose-1,splay_frameinfo,splay_startframe);
    }
#else
    player->playing(splay_verbose-1,splay_frameinfo,splay_startframe);
#endif  // PTHREADEDMPEG
    if(player->geterrorcode()>0)error(player->geterrorcode());
  }
}

//Wrapper to convert null-string to the empty string
inline const char * nn(const char* str)
{
  return str?str:"";
}

#ifdef HAVE_LIBID3

ostream& operator<<(ostream& s, const ID3_Tag* tag )
{
  /* this gets all frames, but the info is rather difficult to get,
     just assume all is ID3FN_TEXT here and hope we get the most...
  */
  /*
  size_t max = tag->NumFrames();
  // s << "frames: " << max << endl;
  for ( size_t n=0; n<max ; n++){
    if ( ID3_Frame* frame = (*tag)[n] ){
      s << "@@@" << frame->GetDescription() << " " <<
	ID3_GetString(frame,ID3FN_TEXT) << endl;
    }
  }
  */


  // Print just what we want...
  s.setf(ios::left);  // The filled fields get the text to the left
  s << 
    "Title : " << nn(ID3_GetTitle(tag)) << endl;
  s << 
    "Artist: " << setw(30) << nn(ID3_GetArtist(tag)) << 
    "Album: " << nn(ID3_GetAlbum(tag)) << 
    endl ; 
  
  s << 
    "Genre : " << setw(18) << nn(ID3_GetGenre(tag)) << 
    "Track: " << setw(5) << ID3_GetTrackNum(tag) << 
    "Year: " <<  setw(6) << nn(ID3_GetYear(tag)); 
//   if ( ID3_GetComment(tag) ){
//     s << endl << "Comment:" << nn(ID3_GetComment(tag)) <<
//       nn(ID3_GetLyricist(tag)) << nn(ID3_GetLyrics(tag));
//   } 

  return s;
}
#endif /* HAVE_LIBID3 */

static void play(char *filename, Soundplayer* device)
{
  if( splay_verbose-- )
    cout << filename << ":" << endl;
#ifdef HAVE_LIBID3
  if( splay_verbose>0 && strcmp(filename, "-") != 0) // cant do ID3 stuff on stdin
    {
      //    cerr << getpid() << endl;
	  ID3_Tag mytag(filename);
	  if ( mytag.HasV1Tag() || mytag.HasV2Tag()  )
	    cout << &mytag << endl;
    }
#endif /* HAVE_LIBID3 */        
  
  Mpegfileplayer player(device);
  if(!player.openfile(strcmp(filename,"-")==0?NULL:filename))
    {
      error(player.geterrorcode());
      return;
    }
  player.setdownfrequency(splay_downfrequency);
  playing(&player);
}

#include <signal.h>

void mtest(int)
{
  //  cerr << "mtest got nr " << i << endl;
}

void mstop(int)
{
  //  cerr << "mstop got nr " << i << endl;
  exit(1);
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

  splay_progname=argv[0];

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
    case '2':splay_downfrequency  =   1;break;
    case 'f':splay_frameinfo      =true;break;
    case 'm':splay_forcetomonoflag=true;break;
    case 'r':splay_repeatflag     =true;break;
    case 's':splay_shuffleflag    =true;break;
    case 'v':splay_verbose++;           break;
      
    case 'd':devicename=optarg;   break;
    case 'k':splay_startframe=atoi(optarg);   break;
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
    case 'S':single_threaded = true; break;
    default:fprintf(stderr,"Bad argument.\n");
    }
  }

  if(argc<=optind && splay_listsize==0)
  {
    printf("%s %s"
#ifdef PTHREADEDMPEG
	   " with pthread"
#endif
	   "\n"
	   "Usage : splay [-2mrsvMVWS] [-d device] [-l listfile] "
#ifdef PTHREADEDMPEG
	   "[-t number] "
#endif
	   "files ...\n"
	   "\n"
	   "%s"
	   ,PACKAGE,VERSION,help);
    return 0;
  }

  if(splay_listsize==0)    // Make list by arguments
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
    if(splay_shuffleflag)shufflelist();

    for(int i=0;i<splay_listsize;i++){
      if (!single_threaded)
      { // normal mode
        signal(SIGINT,&mtest);
        if ( fork() ) {
  	  //	pid_t pid=getpid();
  	  //	cout << "parent process " << pid << endl;
	  int a;
	  wait(&a);
	  signal(SIGINT,&mstop);
	  //	cerr << "stoppable "<<endl;
	  usleep (500*1000);
	  //	cerr << "not stoppable "<<endl;
	  signal(SIGINT,&mtest);
        }
        else{
	  //	cout << "child process " << getpid() << endl;
	  signal(SIGINT,&mstop);
	  play(splay_list[i], device);
	  exit(0);
        }
      }
      else
      { // single threaded mode
	play(splay_list[i], device);
      }
    }
  }while(splay_repeatflag);

  delete device;
  
  return 0;
}

