/* Sound Player Version

   Copyright (C) 1997 by Woo-jae Jung */

// Functions to support player
// Anyone can use MPEG Sound library under GPL

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "xsplay.h"

bool splay_exitwhendone=false;
/*
static bool exitflag=false;
bool Getexitwhendone(void)
{
  return exitflag;
}

void Setexitwhendone(bool flag)
{
  exitflag=flag;
}
*/
static char currentpath[300]={0};
char *Getcurrentpath(void)
{
  if(currentpath[0]==0)return NULL;
  return currentpath;
}

void Setcurrentpath(char *path)
{
  strcpy(currentpath,path);
}

static char currentwildcard[300]={0};
char *Getcurrentwildcard(void)
{
  if(currentwildcard[0]==0)
  {
    currentwildcard[0]='*';
    currentwildcard[1]=0;
  }

  return currentwildcard;
}

void Setcurrentwildcard(char *wildcard)
{
  strcpy(currentwildcard,wildcard);
}
