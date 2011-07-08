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

  if(strstr(filename,"://"))
    loader = new Soundinputstreamfromhttp;
  else
    loader = new Soundinputstreamfromfile;

  if(!loader->open(filename))
  {
    delete loader;
    loader = NULL;
    return false;
  }

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

bool Mpegfileplayer::playing(int verbose)
{
  if(!server->run(-1))
  {       // Initialize MPEG Layer 3
      seterrorcode(server->geterrorcode());
      return false;
  }
  if (verbose > 2)
    showverbose(verbose);

  int pcmperframe=server->getpcmperframe();
  int frequency=server->getfrequency();

  // Playing
  while (server->run(1)) {
    if (_abort_flag)
    {
      player->abort();
      break;
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

