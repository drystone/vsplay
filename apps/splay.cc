/* Sound Player

   Copyright (C) 1997 by Woo-jae Jung */

// It's an example of using MPEG Sound library

// Anyone can use MPEG Sound library under GPL

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#ifdef HAVE_LIBID3
#include <id3/tag.h>
#include <id3/misc_support.h>
#endif /* HAVE_LIBID3 */

#include <iomanip>

#include "mpegsound.h"

#include "splay.h"

using namespace std;

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

/***********************/
/* Command line player */
/***********************/
inline void error(int n)
{
  fprintf(stderr,"%s: %s\n",splay_progname,splay_Sounderrors[n-1]);
  return;
}

#ifdef PTHREADEDMPEG
static void playingthread(Mpegfileplayer *player)
{
  if(player->geterrorcode()>0)error(player->geterrorcode());
  else
  {
    player->setforcetomono(splay_forcetomonoflag);
    player->playingwiththread(splay_verbose-1,splay_frameinfo,
			      splay_threadnum, splay_startframe); 
    if(player->geterrorcode()>0)error(player->geterrorcode());
  }
}
#endif

static void playing(Fileplayer *player)
{
  if(player->geterrorcode()>0)error(player->geterrorcode());
  else
  {
    player->setforcetomono(splay_forcetomonoflag);
    player->playing(splay_verbose-1,splay_frameinfo,splay_startframe);
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

  return s;
}

#endif /* HAVE_LIBID3 */

static void play(char *filename)
{
  if( splay_verbose-- )
    cout << filename << ":" << endl;

#ifdef HAVE_LIBID3

  if( splay_verbose>0 )
    {
      //    cerr << getpid() << endl;
      try {
	const ID3_Tag*  mytag = new ID3_Tag(filename);
	if ( mytag->HasV1Tag() || mytag->HasV2Tag()  )
	  cout << mytag << endl;
	delete mytag;
      }
//      catch(ID3_Error &err){
      catch(...){
	cout << "Error found (GetError functions disabled)" << endl;
//	cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
//	     << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
      }
    }

#endif /* HAVE_LIBID3 */        
  
  Mpegfileplayer *player;
  
  player=new Mpegfileplayer;
  if(!player->openfile(filename,splay_devicename))
    {
      error(player->geterrorcode());
      delete player;
      return;
    }
  player->setdownfrequency(splay_downfrequency);
#ifdef PTHREADEDMPEG
  playingthread(player);
#else
  playing(player);
#endif
  delete player;
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


int main(int argc,char *argv[])
{
  int c;

  splay_progname=argv[0];

  while((c=getopt(argc,argv,
		  "VM2mfrsvd:k:l:"
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
      case 'M':
	{
	  Mpegfileplayer player;

	  player.openfile(NULL,splay_devicename);
	  playing(&player);
	  return 0;
	}
	
    case '2':splay_downfrequency  =   1;break;
    case 'f':splay_frameinfo      =true;break;
    case 'm':splay_forcetomonoflag=true;break;
    case 'r':splay_repeatflag     =true;break;
    case 's':splay_shuffleflag    =true;break;
    case 'v':splay_verbose++;           break;
      
    case 'd':splay_devicename=optarg;   break;
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
	   "Usage : splay [-2mrsvMVW] [-d device] [-l listfile] "
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

  do
  {
    if(splay_shuffleflag)shufflelist();

    for(int i=0;i<splay_listsize;i++){
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
	play(splay_list[i]);
	exit(0);
      }
    }
    
  }while(splay_repeatflag);

  return 0;
}
