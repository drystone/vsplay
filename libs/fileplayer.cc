/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Fileplayer.cc
// It's an example for how to use MPEG Sound library

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_IOSTREAM
#include <iostream>
using namespace std;
#else
#include <iostream.h>
#endif

#include <math.h>
#include "mpegsound.h"

// File player superclass
Fileplayer::Fileplayer(Soundplayer * player)
    : player(player)
{
  __errorcode=SOUND_ERROR_OK;
};

Fileplayer::~Fileplayer()
{
  delete player;
};

// Mpegfileplayer
Mpegfileplayer::Mpegfileplayer(Soundplayer * player)
    : Fileplayer(player)
{
  loader=NULL;
  server=NULL;
};

Mpegfileplayer::~Mpegfileplayer()
{
  if(loader)delete loader;
  if(server)delete server;
}

bool Mpegfileplayer::openfile(const char *filename)
{
// Loader
  {
    int err;
    if((loader=Soundinputstream::hopen(filename,&err))==NULL)
      return seterrorcode(err);
  }

// Server
  if((server=new Mpegtoraw(loader,player))==NULL)
    return seterrorcode(SOUND_ERROR_MEMORYNOTENOUGH);

// Initialize server
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
  int min=int(floor(s/60));
  double sec=s-60*min;
  char * buff = new char[15];
  sprintf(buff,"%d:%2.2f",min,sec);
  return buff;
}

bool Mpegfileplayer::playing(int verbose, bool frameinfo, int startframe)
{
  if(!server->run(-1))
  {       // Initialize MPEG Layer 3
      seterrorcode(server->geterrorcode());
      return false;
  }
  if(verbose>0)showverbose(verbose);

  if (startframe) server->setframe(startframe);

  int pcmperframe=server->getpcmperframe();
  int frequency=server->getfrequency();
  int totframes=server->gettotalframe();
  double tottime=1.0*totframes*pcmperframe/frequency;
  if(frameinfo) {
    cout << "Totalframes " <<  totframes;
    cout << "; Totaltime " << tominsec(tottime)  << endl;
  }

  // Playing
  while (server->run(1)) {
    if (_abort_flag)
    {
      server->stopthreadedplayer();
      player->abort();
      break;
    }
  
    if(frameinfo) {
      int currframe=server-> getcurrentframe();
      double currtime=1.0*currframe*pcmperframe/frequency;
      cout << "Frame " << currframe << " [" << totframes-currframe << "]; ";
      cout << "Time " << tominsec(currtime) << " [" ;
      cout << tominsec(tottime-currtime) << "]" << endl ;
    }
  }

  seterrorcode(server->geterrorcode());
  if(seterrorcode(SOUND_ERROR_FINISH))return true;
  return false;
}

#ifdef PTHREADEDMPEG
bool Mpegfileplayer::playingwiththread(int verbose,bool frameinfo,
				       int framenumbers, int startframe)
{
  if (framenumbers < 20)
    return playing(verbose, frameinfo, startframe);

  server->makethreadedplayer(framenumbers);
  bool ret = playing(verbose, frameinfo, startframe);
  server->freethreadedplayer();
  return ret;
}
#endif

void Mpegfileplayer::showverbose(int )
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
