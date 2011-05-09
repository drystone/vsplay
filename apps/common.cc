/* Sound Player

   Copyright (C) 1997 by Woo-jae Jung */

// Functions to support player
// Anyone can use MPEG Sound library under GPL

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mpegsound.h"

#include "splay.h"

int  splay_verbose=0;
int splay_startframe=0;
char *splay_progname;
char *splay_devicename=NULL;

char *splay_list[MAXLISTSIZE];
int  splay_listsize=0;
int  splay_downfrequency=0;
bool splay_shuffleflag=false,
     splay_repeatflag=false,
     splay_forcetomonoflag=false,
     splay_frameinfo=false;

#ifdef PTHREADEDMPEG
int splay_threadnum=50;
#endif

char *splay_Sounderrors[SOUND_ERROR_UNKNOWN]=
{ "Failed to open sound device.",
  "Sound device is busy.",
  "Buffersize of sound device is wrong.",
  "Sound device control error.",

  "Failed to open file for reading.",    // 5
  "Failed to read file.",                

  "Unkown proxy.",
  "Unkown host.",
  "Socket error.",
  "Connection failed.",                  // 10
  "Fdopen error.",
  "Http request failed.",
  "Http write failed.",
  "Too many relocation",

  "Memory is not enough.",               // 15
  "Unexpected EOF.",
  "Bad sound file format.",

  "Cannot make thread.",

  "Unknown error.",
};


int Setvolume(int volume)
{
  return Rawplayer::setvolume(volume);
}

/***************/
/* Manage list */
/***************/
static bool argsflag=false;
void arglist(int argc,char *argv[],int start)
{
  register int i;

  argsflag=true;
  for(i=start;(i<argc) && (splay_listsize<MAXLISTSIZE); i++)
    splay_list[splay_listsize++]=argv[i];
}

void killlist(void)
{
  if(splay_listsize)
  {
    if(!argsflag)
    {
      for(int i=0;i<splay_listsize;i++)free(splay_list[i]);
    }
    argsflag=false;
  }
  splay_listsize=0;
}

void addlist(const char *path,const char *filename)
{
  char songfilename[MAXFILENAMELENGTH*2];
  int p=0;

  if(path)
  {
    for(;path[p];p++)
      songfilename[p]=path[p];
    songfilename[p++]='/';
  }

  for(int i=0;filename[i];i++,p++)
    songfilename[p]=filename[i];

  songfilename[p]=0;
  splay_list[splay_listsize]=strdup(songfilename);
  splay_listsize++;
}

void readlist(char *filename)
{
  char songfile[MAXFILENAMELENGTH];
  Soundinputstream *fp;
  int c;

  killlist();
 
  {
    int err;

    if((fp=Soundinputstream::hopen(filename,&err))==NULL)
    {
      fprintf(stderr,"%s: No list file found\n",splay_progname);
      return;
    }
  }

  for(splay_listsize=0;splay_listsize<MAXLISTSIZE;)
  {
    c=fp->getbytedirect();
    if(c==EOF)break;
    if(c=='#')
      for(;;)
      {
	c=fp->getbytedirect();
	if(c==EOF)break;
	if(c=='\n')break;
      }
    else
      for(;;)
      {
	if((c==EOF) || (c=='\n'))break;
	if(c!=' ')
	{
	  int p;

	  for(p=0;;p++)
	  {
	    songfile[p]=c;
	    c=fp->getbytedirect();
	    if((c==EOF) || (c=='\n'))break;
	  }

	  songfile[++p]='\0';
	  splay_list[splay_listsize]=strdup(songfile);
	  splay_listsize++;
	  break;
	}

	c=fp->getbytedirect();
      }
  }

  if(splay_verbose>1)
  {
    for(int i=0;i<splay_listsize;i++)
      fprintf(stderr,"%d : %s\n",i+1,splay_list[i]);
  }

  delete fp;
}

void shufflelist(void)
{
  char *tmp;
  int i,p;
  time_t t;

  srandom(time(&t));

  for(i=splay_listsize-1;i>0;i--)
  {
    p=random()%i;

    tmp=splay_list[p];
    splay_list[p]=splay_list[i];
    splay_list[i]=tmp;
  }
}
