/* MPEG Sound library

   (C) 1997 by Woo-jae Jung */

// Binput.cc
// Inputstream from file

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include "mpegsound.h"

bool Soundinputstreamfromfile::open(const char *filename)
{
  if (!strcmp(filename, "-"))
    _fp = stdin;
  else
    _fp = fopen(filename, "r");
  
  return _fp != NULL;
}

