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
#include <libgen.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/param.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>

#ifdef TAGLIB
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#endif

#include "mpegsound.h"

#include "vsplay.h"

char progname[MAXPATHLEN];
int  verbose;

std::vector<std::string> playlist;

const char *vsplay_Sounderrors[SOUND_ERROR_UNKNOWN]=
{ "Failed to open sound device.",
  "Sound device is busy.",
  "Buffersize of sound device is wrong.",
  "Sound device control error.",

  "Failed to open file for reading.",    // 5
  "Failed to read file.",                

  "Unkown proxy.",
  "Unkown host.",
  "Socket error.",
  "Connection failed.",                  // 10
  "Fdopen error.",
  "Http request failed.",
  "Http write failed.",
  "Too many relocation",

  "Memory is not enough.",               // 15
  "Unexpected EOF.",
  "Bad sound file format.",

  "Cannot make thread.",

  "Unknown error.",
};

void usage()
{
  std::cout << "Usage: " << progname 
    << " [-2fmrsvMVW] [-d device] [-l listfile] [-k frame] files ..." << std::endl
    << std::endl
    << std::setw(12) << std::left << "\t-2" << "half frequency" << std::endl
    << std::setw(12) << std::left << "\t-f" << "show frame and time info" << std::endl
    << std::setw(12) << std::left << "\t-m" << "mono" << std::endl
    << std::setw(12) << std::left << "\t-r" << "repeat" << std::endl
    << std::setw(12) << std::left << "\t-s" << "shuffle" << std::endl
    << std::setw(12) << std::left << "\t-v[v[v]]" << "verbose, verboser, verbosest" << std::endl
    << std::setw(12) << std::left << "\t-M" << "play standard input" << std::endl
    << std::setw(12) << std::left << "\t-V" << "show version and exit" << std::endl
    << std::setw(12) << std::left << "\t-k <num>" << "playing from frame <num>" << std::endl
    << std::setw(12) << std::left << "\t-d <dev>" << "use audio device <dev>" << std::endl
    << std::setw(12) << std::left << "\t-l" << "list list file" << std::endl
    << std::endl
    << "For more detail, see man page." << std::endl;
}

typedef enum {devicetype_oss, devicetype_alsa} Devicetype;
bool single_threaded = false;
Mpegfileplayer g_player;

/***********************/
/* Command line player */
/***********************/
void error(int n)
{
  std::cerr << progname << " " << vsplay_Sounderrors[n-1] << std::endl;
}

#ifdef TAGLIB
std::ostream& operator<<(std::ostream& s, const TagLib::Tag& tag )
{
  s.setf(std::ios::left);
  s << "Title : " << tag.title() << std::endl
    << "Artist: " << std::setw(30) << tag.artist()
    << "Album: " << tag.album() << std::endl
    << "Genre : " << std::setw(18) << tag.genre()
    << "Track: " << std::setw(5) << tag.track()
    << "Year: " <<  std::setw(6) << tag.year()
    << std::endl << std::flush; 
  return s;
}
#endif /* TAGLIB */

static void play(const std::string& filename, Soundplayer* device, unsigned int startframe, bool frameinfo)
{
  if( verbose-- )
    std::cout << filename << ":" << std::endl;

#ifdef TAGLIB
  if( verbose>0 && filename != "-") // cant do ID3 stuff on stdin
  {
    std::cout << *TagLib::MPEG::File(filename.c_str()).tag();
  }
#endif /* TAGLIB */        
  
  if(!g_player.openfile(filename.c_str()))
    error(g_player.geterrorcode());
  else
  {
    g_player.playing(verbose - 1, frameinfo, startframe);
    if (g_player.geterrorcode() > 0)
      error(g_player.geterrorcode());
  }
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

  g_player.abort();
}

void readlist(std::istream& s)
{
  char * linebuf = new char[0x10000];
  while (!s.eof())
  {
    s.getline(linebuf, 0x10000);
    playlist.push_back(std::string(linebuf));
  }
  delete linebuf;
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
  Devicetype devicetype = devicetype_alsa;
  char const * devicename = NULL;
  char const * list_file = NULL;
  bool stdin_only = false;
  unsigned int startframe = 0;
  bool shuffle = false;
  bool repeat = false;
  bool frameinfo = false;

  strcpy(progname, basename(argv[0]));

  opterr = 0;
  while (1)
  {
    int c = getopt(argc, argv, "VM2mfrsvd:k:l:");
    if (c == -1)
      break;

    switch (c)
    {
    case 'V':
      std::cout << PACKAGE << VERSION << std::endl;
      return 0;
    case 'M':
      stdin_only = true;
      break;
    case '2':
      g_player.setdownfrequency(true);
      break;
    case 'f':
      frameinfo = true;
      break;
    case 'm':
      g_player.setforcetomono(true);
      break;
    case 'r':
      repeat = true;
      break;
    case 's':
      srandom(time(NULL));
      shuffle = true;
      break;
    case 'v':
      verbose++;
      break;
    case 'd':
      devicename=optarg;
      break;
    case 'k':
      startframe = atoi(optarg);
      break;
    case 'l':
      list_file = optarg;
      break;
    default:
      std::cerr << progname << ": unknown argument -" << char(optopt) << std::endl;
    }
  }

  if (list_file)
  {
    if (!strcmp(list_file, "-"))
      readlist(std::cin);
    else
    {
      std::ifstream s(list_file);
      if (!s.fail())
        readlist(s);
      else
        std::cerr << progname << ": listfile not found" << std::endl;
    }
    if (verbose)
      std::cerr << "List file: " << list_file << std::endl;
  }

  for (optind; optind < argc; ++optind)
    playlist.push_back(std::string(argv[optind]));

  if (!playlist.size())
  {
    usage();
    return 0;
  }

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

  g_player.setoutput(device);
  signal(SIGINT, sigint_capture);
  do
  {
    if (shuffle)
      for (unsigned int i = 0, s = playlist.size(); i < s; i++)
        std::swap(playlist[i], playlist[random() % s]);

    std::vector<std::string>::iterator i;
    for(i = playlist.begin(); i != playlist.end(); i++)
      play(*i, device, startframe, frameinfo);

  } while (repeat);

  delete device;
  
  return 0;
}

