/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Rawtofile.cc
// Output raw data to file.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <unistd.h>

#include "mpegsound.h"

// Rawplayer class
Rawtofile::~Rawtofile()
{
  close(filehandle);
}

void Rawtofile::initialize(const char *filename) throw (Vsplayexception)
{
  if (filename == NULL)
    filehandle = 1;
  else if ((filehandle = creat(filename, 0644)) == -1)
    throw Vsplayexception(SOUND_ERROR_DEVOPENFAIL);
}

void Rawtofile::setsoundtype(int stereo, int samplesize, int speed)
  throw (Vsplayexception)
{
  rawstereo = stereo;
  rawsamplesize = samplesize;
  rawspeed = speed;
}

void Rawtofile::putblock(void *buffer,int size)
  throw (Vsplayexception)
{
  if (write(filehandle, buffer, size) != size)
    throw Vsplayexception(SOUND_ERROR_DEVWRITEFAIL);
};

