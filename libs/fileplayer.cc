/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Fileplayer.cc
// It's an example for how to use MPEG Sound library

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <math.h>
#include "mpegsound.h"

// File player superclass
Fileplayer::Fileplayer()
  : player(NULL)
  , _abort_flag(false)
  , __errorcode(SOUND_ERROR_OK)
{
}

Fileplayer::~Fileplayer()
{
}

// Mpegfileplayer
Mpegfileplayer::Mpegfileplayer()
  : Fileplayer()
  , loader(NULL)
  , server(NULL)
{
}

Mpegfileplayer::~Mpegfileplayer()
{
  if(loader)delete loader;
  if(server)delete server;
}

bool Mpegfileplayer::openfile(const char *filename)
{
  if (loader) delete loader;
  if (server) delete server;
  loader = NULL;
  server = NULL;

  int err;
  if((loader=Soundinputstream::hopen(filename,&err))==NULL)
    return seterrorcode(err);

  if((server=new Mpegtoraw(loader,player))==NULL)
    return seterrorcode(SOUND_ERROR_MEMORYNOTENOUGH);

  _abort_flag = false;
  server->initialize();
  return true;
}

void Mpegfileplayer::setforcetomono(bool flag)
{
  server->setforcetomono(flag);
};

void Mpegfileplayer::setdownfrequency(int value)
{
  server->setdownfrequency(value);
};

char * tominsec(double s)
{
  static char buff[15];
  int min = s / 60;
  sprintf(buff, "%d:%2.2f", min, s - (min * 60));
  return buff;
}

bool Mpegfileplayer::playing(int verbose, bool frameinfo, int startframe)
{
  if(!server->run(-1))
  {       // Initialize MPEG Layer 3
      seterrorcode(server->geterrorcode());
      return false;
  }
  if (verbose > 2)
    showverbose(verbose);

  if (startframe) server->setframe(startframe);

  int pcmperframe=server->getpcmperframe();
  int frequency=server->getfrequency();
  int totframes=server->gettotalframe();
  double tottime=1.0*totframes*pcmperframe/frequency;
  if(frameinfo) {
    std::cout << "Totalframes " <<  totframes;
    std::cout << "; Totaltime " << tominsec(tottime)  << std::endl;
  }

  // Playing
  while (server->run(1)) {
    if (_abort_flag)
    {
      player->abort();
      break;
    }
  
    if(frameinfo) {
      int currframe=server-> getcurrentframe();
      double currtime=1.0*currframe*pcmperframe/frequency;
      std::cout << "Frame " << currframe << " [" << totframes-currframe << "]; ";
      std::cout << "Time " << tominsec(currtime) << " [" ;
      std::cout << tominsec(tottime-currtime) << "]" << std::endl ;
    }
  }

  seterrorcode(server->geterrorcode());
  if(seterrorcode(SOUND_ERROR_FINISH))return true;
  return false;
}

void Mpegfileplayer::showverbose(int)
{
  static const char *modestring[4]={"stereo","joint stereo","dual channel","mono"};

  fprintf(stderr,"\tMPEG-%d Layer %d, %s,\n\t%dHz%s, %dkbit/s, ",
	  server->getversion()+1,
	  server->getlayer(),modestring[server->getmode()],
	  server->getfrequency(),server->getdownfrequency()?"//2":"",
	  server->getbitrate());
  fprintf(stderr,server->getcrccheck() 
	  ? "with crc check\n" 
	  : "without crc check\n");
}

