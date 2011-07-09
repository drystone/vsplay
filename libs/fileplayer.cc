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

Mpegfileplayer::~Mpegfileplayer()
{
  if(loader)delete loader;
  if(server)delete server;
}

void Mpegfileplayer::openfile(const char *filename)
  throw (Vsplayexception)
{
  if (loader) delete loader;
  if (server) delete server;
  loader = NULL;
  server = NULL;

  if(strstr(filename,"://"))
    loader = new Soundinputstreamfromhttp;
  else
    loader = new Soundinputstreamfromfile;

  loader->open(filename);

  if((server=new Mpegtoraw(loader,player))==NULL)
    throw Vsplayexception(SOUND_ERROR_MEMORYNOTENOUGH);
}

void Mpegfileplayer::setforcetomono(bool flag)
{
  server->setforcetomono(flag);
};

void Mpegfileplayer::setdownfrequency(int value)
{
  server->setdownfrequency(value);
};

void Mpegfileplayer::playing(int verbose)
  throw (Vsplayexception)
{
  server->run(verbose);
}

