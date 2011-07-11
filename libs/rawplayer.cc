/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Rawplayer.cc
// Playing raw data with sound type.
// It's for only Linux

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

#include "mpegsound.h"

/* IOCTL */
#ifdef SOUND_VERSION
#define IOCTL(a,b,c)		ioctl(a,b,&c)
#else
#define IOCTL(a,b,c)		(c = ioctl(a,b,c) )
#endif

/*******************/
/* Rawplayer class */
/*******************/
// Rawplayer class
Rawplayer::Rawplayer(const char * filename)
  throw(Vsplayexception)
{
  if ((audiohandle = open(filename, O_WRONLY)) == -1)
    throw Vsplayexception(SOUND_ERROR_DEVOPENFAIL);
}

Rawplayer::~Rawplayer()
{
  close(audiohandle);
}

void Rawplayer::stop(void)
{
  int a;
  IOCTL(audiohandle,SNDCTL_DSP_RESET,a);
}

void Rawplayer::setsoundtype(int stereo, int samplesize, int speed)
  throw (Vsplayexception)
{
  rawstereo = stereo;
  rawsamplesize = samplesize;
  rawspeed = speed;
  forcetomono = forceto8 = false;
  resetsoundtype();
}

void Rawplayer::resetsoundtype(void)
  throw (Vsplayexception)
{
  int tmp;

  if (ioctl(audiohandle, SNDCTL_DSP_SYNC, NULL) < 0)
    throw Vsplayexception(SOUND_ERROR_DEVCTRLERROR);

#ifdef SOUND_VERSION
  if (ioctl(audiohandle, SNDCTL_DSP_STEREO, &rawstereo) < 0)
#else
  if (rawstereo != ioctl(audiohandle, SNDCTL_DSP_STEREO, rawstereo))
#endif
  {
    rawstereo = MODE_MONO;
    forcetomono = true;
  }

  tmp=rawsamplesize;
  // TODO ?? &tmp
  IOCTL(audiohandle,SNDCTL_DSP_SAMPLESIZE,tmp);
  if(tmp!=rawsamplesize)
    if(rawsamplesize==16)
    {
      rawsamplesize=8;
      IOCTL(audiohandle,SNDCTL_DSP_SAMPLESIZE,rawsamplesize);
      if(rawsamplesize!=8)
        throw Vsplayexception(SOUND_ERROR_DEVCTRLERROR);

      forceto8=true;
    }

  if(IOCTL(audiohandle,SNDCTL_DSP_SPEED,rawspeed)<0)
    throw Vsplayexception(SOUND_ERROR_DEVCTRLERROR);
}

void Rawplayer::putblock(void *buffer, int size)
  throw (Vsplayexception)
{
  int modifiedsize = size;

  if(forcetomono || forceto8)
  {
    register unsigned char *source, *dest;
    int increment = 0, c;

    source = dest = (unsigned char *)buffer;

    if (forcetomono) increment++;
    if (forceto8) increment++, source++;

    c = modifiedsize = size >> increment;
    increment <<= 1;

    while(c--)
    {
      *(dest++) = *source;
      source += increment;
    }
  }

  write(audiohandle, buffer, modifiedsize);
}

