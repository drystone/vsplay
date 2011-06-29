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

#include "vsplay.h"

int  vsplay_verbose=0;
int vsplay_startframe=0;
char *vsplay_progname;
const char *vsplay_devicename=NULL;

char *vsplay_list[MAXLISTSIZE];
int  vsplay_listsize=0;
int  vsplay_downfrequency=0;
bool vsplay_shuffleflag=false,
     vsplay_repeatflag=false,
     vsplay_forcetomonoflag=false,
     vsplay_frameinfo=false;

#ifdef PTHREADEDMPEG
int vsplay_threadnum=50;
#endif

const char *vsplay_Sounderrors[SOUND_ERROR_UNKNOWN]=
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
  for(i=start;(i<argc) && (vsplay_listsize<MAXLISTSIZE); i++)
    vsplay_list[vsplay_listsize++]=argv[i];
}

void killlist(void)
{
  if(vsplay_listsize)
  {
    if(!argsflag)
    {
      for(int i=0;i<vsplay_listsize;i++)free(vsplay_list[i]);
    }
    argsflag=false;
  }
  vsplay_listsize=0;
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
  vsplay_list[vsplay_listsize]=strdup(songfilename);
  vsplay_listsize++;
}

void readlist(char *filename)
{
  char songfile[MAXFILENAMELENGTH];
  Soundinputstream *fp;
  int c;

  killlist();
 
  {
    int err;

    if (strcmp(filename,"-")==0)
    {
      filename = NULL;	// forces Soundinputstream to use stdin
    }

    if((fp=Soundinputstream::hopen(filename,&err))==NULL)
    {
      fprintf(stderr,"%s: No list file found\n",vsplay_progname);
      return;
    }
  }

  for(vsplay_listsize=0;vsplay_listsize<MAXLISTSIZE;)
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
	  vsplay_list[vsplay_listsize]=strdup(songfile);
	  vsplay_listsize++;
	  break;
	}

	c=fp->getbytedirect();
      }
  }

  if(vsplay_verbose>1)
  {
    for(int i=0;i<vsplay_listsize;i++)
      fprintf(stderr,"%d : %s\n",i+1,vsplay_list[i]);
  }

  delete fp;
}

void shufflelist(void)
{
  char *tmp;
  int i,p;
  time_t t;

  srandom(time(&t));

  for(i=vsplay_listsize-1;i>0;i--)
  {
    p=random()%(i+1);

    if(p==i)continue;

    tmp=vsplay_list[p];
    vsplay_list[p]=vsplay_list[i];
    vsplay_list[i]=tmp;
  }
}
