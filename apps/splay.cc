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
#include <iostream.h>

#ifdef HAVE_LIBID3
#include <id3/tag.h>
#include <id3/misc_support.h>
#endif /* HAVE_LIBID3 */

#include <iomanip.h>

#include "mpegsound.h"

#include "splay.h"

static char *help=
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

//List of ID3 genre names, copied from xmms.org
const char *ID3_gen_list[255] =
  {
    //0-24
    "Blues", "Classic Rock", "Country", "Dance", "Disco", 
    "Funk", "Grunge", "Hip-Hop", "Jazz", "Metal",
    "New Age", "Oldies", "Other", "Pop", "R&B",
    "Rap", "Reggae", "Rock", "Techno", "Industrial",
    "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
    //25-49
    "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
    "Fusion", "Trance", "Classical", "Instrumental", "Acid",
    "House", "Game", "Sound Clip", "Gospel", "Noise",
    "Alt", "Bass", "Soul", "Punk", "Space",
    "Meditative", "Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic",
    //50-74
    "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",
    "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta Rap",
    "Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American",
    "Cabaret", "New Wave", "Psychedelic", "Rave", "Showtunes",
    "Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz",
    //75-99
    "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock",
    "Folk", "Folk/Rock", "National Folk", "Swing", "Fast-Fusion",
    "Bebob", "Latin", "Revival", "Celtic", "Bluegrass",
    "Avantgarde", "Gothic Rock", "Progressive Rock", "Psychedelic Rock", "Symphonic Rock",
    "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic",
    //100-124
    "Humour", "Speech", "Chanson", "Opera", "Chamber Music",
    "Sonata", "Symphony", "Booty Bass", "Primus", "Porn Groove",
    "Satire", "Slow Jam", "Club", "Tango", "Samba",
    "Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
    "Duet", "Punk Rock", "Drum Solo", "A Cappella", "Euro-House",
    //125-149
    "Dance Hall", "Goa", "Drum & Bass", "Club-House", "Hardcore",
    "Terror", "Indie", "BritPop", "Negerpunk", "Polsk Punk",
    "Beat", "Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover",
    "Contemporary Christian", "Christian Rock", "Merengue", "Salsa", "Thrash Metal",
    "Anime", "JPop", "Synthpop"
};

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
    "Genre : " << setw(18) << nn(ID3_gen_list[ID3_GetGenreNum(tag)]) << 
    "Track: " << setw(5) << ID3_GetTrackNum(tag) << 
    "Year: " <<  setw(6) << nn(ID3_GetYear(tag)); 
//   if ( ID3_GetComment(tag) ){
//     s << endl << "Comment:" << nn(ID3_GetComment(tag)) <<
//       nn(ID3_GetLyricist(tag)) << nn(ID3_GetLyrics(tag));
//   } 

  return s;
}
#endif /* HAVE_LIBID3 */

static void play(char *filename)
{
  if( splay_verbose-- )
    cout << filename << ":" << endl;
  if( splay_verbose>0 )
    {
      //    cerr << getpid() << endl;
#ifdef HAVE_LIBID3
      try {
	const ID3_Tag*  mytag = new ID3_Tag(filename);
	if ( mytag->HasV1Tag() || mytag->HasV2Tag()  )
	  cout << mytag << endl;
	delete mytag;
	
      }
      catch(ID3_Error &err){
	cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
	     << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
      }
#endif /* HAVE_LIBID3 */        
    }
  
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
