/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Mpeglayer1.cc
// It's for MPEG Layer 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include "mpegsound.h"

// Tables for layer 1
static const REAL factortable[15] = 
{
  0.0, 
  (1.0/2.0)    * (4.0/3.0),         (1.0/4.0)     * (8.0/7.0),
  (1.0/8.0)    * (16.0/15.0),       (1.0/16.0)    * (32.0/31.0),
  (1.0/32.0)   * (64.0/63.0),       (1.0/64.0)    * (128.0/127.0),
  (1.0/128.0)  * (256.0/255.0),     (1.0/256.0)   * (512.0/511.0),
  (1.0/512.0)  * (1024.0/1023.0),   (1.0/1024.0)  * (2048.0/2047.0),
  (1.0/2048.0) * (4096.0/4095.0),   (1.0/4096.0)  * (8192.0/8191.0),
  (1.0/8192.0) * (16384.0/16383.0), (1.0/16384.0) * (32768.0/32767.0)
};

static const REAL offsettable[15] = 
{
  0.0, 
  ((1.0/2.0)-1.0)    * (4.0/3.0),         ((1.0/4.0)-1.0)     * (8.0/7.0), 
  ((1.0/8.0)-1.0)    * (16.0/15.0),       ((1.0/16.0)-1.0)    * (32.0/31.0),
  ((1.0/32.0)-1.0)   * (64.0/63.0),       ((1.0/64.0)-1.0)    * (128.0/127.0),
  ((1.0/128.0)-1.0)  * (256.0/255.0),     ((1.0/256.0)-1.0)   * (512.0/511.0),
  ((1.0/512.0)-1.0)  * (1024.0/1023.0),   ((1.0/1024.0)-1.0)  * (2048.0/2047.0),
  ((1.0/2048.0)-1.0) * (4096.0/4095.0),   ((1.0/4096.0)-1.0)  * (8192.0/8191.0),
  ((1.0/8192.0)-1.0) * (16384.0/16383.0), ((1.0/16384.0)-1.0) * (32768.0/32767.0)
};

// Mpeg layer 1
void Mpegtoraw::extractlayer1(void)
{
  REAL fraction[MAXCHANNEL][MAXSUBBAND];
  REAL scalefactor[MAXCHANNEL][MAXSUBBAND];
  int bitalloc[MAXCHANNEL][MAXSUBBAND];
  register int i,t;

std::cout << "ok" << std::endl;
// Bitalloc
  for (i = 0; i < stereobound; i++)
  {
    prefetch(2, 4);
    bitalloc[LS][i] = fetch();
    bitalloc[RS][i] = fetch();
  }
  for (; i < MAXSUBBAND; i++)
    bitalloc[LS][i] = bitalloc[RS][i] = getbits(4);

// Scale index
  if (inputstereo)
  {
    for (i = 0; i < MAXSUBBAND; i++)
    {
      if (bitalloc[LS][i]) scalefactor[LS][i] = scalefactorstable[getbits(6)];
      if (bitalloc[RS][i]) scalefactor[RS][i] = scalefactorstable[getbits(6)];
    }
  }
  else
    for (i = 0; i < MAXSUBBAND; i++)
      if (bitalloc[LS][i]) scalefactor[LS][i] = scalefactorstable[getbits(6)];

  for(int l = 0; l < SCALEBLOCK; l++)
  {
    for(i=0;i<stereobound;i++)
    {
      t = bitalloc[LS][i];
      fraction[LS][i] = t 
        ? (REAL(getbits(t+1)) * factortable[t] + offsettable[t]) * scalefactor[LS][i]
        : 0.0;
      t = bitalloc[RS][i];
      fraction[RS][i] = t 
        ? (REAL(getbits(t+1)) * factortable[t] + offsettable[t]) * scalefactor[RS][i]
        : 0.0;
    }
    for (; i < MAXSUBBAND; i++)
    {
      if ((t = bitalloc[LS][i]))
      {
        REAL tt = REAL(getbits(t+1)) * factortable[t] + offsettable[t];
        fraction[LS][i] = tt * scalefactor[LS][i];
        fraction[RS][i] = tt * scalefactor[RS][i];
      }
      else fraction[LS][i] = fraction[RS][i] = 0.0;
    }

    subbandsynthesis(fraction[LS],fraction[RS]);
  }
}

