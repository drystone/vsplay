/* MPEG Sound library

   (C) 1997 by Woo-jae Jung */

// Binput.cc
// Inputstream from file

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include "mpegsound.h"

void Soundinputstreamfromfile::open(const char *filename)
  throw (Vsplayexception)
{
  if (!strcmp(filename, "-"))
    _fp = stdin;
  else
    _fp = fopen(filename, "r");
  
  if (!_fp) throw Vsplayexception(SOUND_ERROR_FILEOPENFAIL);
}

