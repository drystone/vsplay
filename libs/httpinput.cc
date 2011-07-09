/* MPEG Sound library

   (C) 1997 by Woo-jae Jung */

// Httpinputstream.cc
// Inputstream for http

// It's from mpg123

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <unistd.h>

#include "mpegsound.h"

#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

static const char *httpstr="http://";

void writestring(int fd, char *string)
  throw (Vsplayexception)
{
  int result, bytes = strlen(string);

  while(bytes)
  {
    if((result=write(fd,string,bytes))<=0 && errno!=EINTR)
      throw Vsplayexception(SOUND_ERROR_HTTPWRITEFAIL);
    string += result;
    bytes -= result;
  }
}

void readstring(char *string, int maxlen, FILE *f)
  throw (Vsplayexception)
{
  char *result;

  do{
    result=fgets(string,maxlen,f);
  }while(!result  && errno==EINTR);
  if(!result)
    throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
}

static char *strndup(char *src,int num)
{
  char *dst;

  if(!(dst=(char *)malloc(num+1)))return NULL;
  dst[num]='\0';

  return strncpy(dst, src, num);
}

static char *url2hostport(char *url,char **hname,
			  unsigned long *hip,unsigned int *port)
{
  char *cptr;
  struct hostent *myhostent;
  struct in_addr myaddr;
  int isip=1;

  if(!(strncmp(url,httpstr,7)))url+=7;
  cptr=url;
  while(*cptr && *cptr!=':' && *cptr!='/')
  {
    if((*cptr<'0' || *cptr>'9') && *cptr!='.')isip=0;
    cptr++;
  }
  if(!(*hname=strndup(url,cptr-url)))
  {
    *hname=NULL;
    return NULL;
  }
  if(!isip)
  {
    if (!(myhostent=gethostbyname(*hname)))return NULL;
    memcpy(&myaddr,myhostent->h_addr,sizeof(myaddr));
    *hip=myaddr.s_addr;
  }
  else if((*hip=inet_addr(*hname))==INADDR_NONE)return NULL;
  if(!*cptr || *cptr=='/')
  {
    *port=80;
    return cptr;
  }
  *port=atoi(++cptr);
  while(*cptr && *cptr!='/')cptr++;
  return cptr;
}

char *proxyurl=NULL;
unsigned long proxyip=0;
unsigned int proxyport;

void Soundinputstreamfromhttp::open(const char *url)
  throw (Vsplayexception)
{
  char *purl=NULL,*host,*request,*sptr;
  char agent[50];
  int linelength;
  unsigned long myip;
  unsigned int myport;
  int sock;
  int relocate=0,numrelocs=0;
  struct sockaddr_in server;
  FILE *myfile;

  if(!proxyip)
  {
    if(!proxyurl)
      if(!(proxyurl=getenv("MP3_HTTP_PROXY")))
	if(!(proxyurl=getenv("http_proxy")))
	  proxyurl = getenv("HTTP_PROXY");
    if (proxyurl && proxyurl[0] && strcmp(proxyurl, "none"))
    {
      if (!(url2hostport(proxyurl, &host, &proxyip, &proxyport)))
        throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
      if(host)free(host);
    }
    else
      proxyip = INADDR_NONE;
  }
  
  if((linelength=strlen(url)+100)<1024)
    linelength=1024;
  if(!(request=(char *)malloc(linelength)) || !(purl=(char *)malloc(1024))) 
    throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
  strncpy(purl,url,1023);
  purl[1023]='\0';
  do{
    strcpy(request,"GET ");
    if(proxyip!=INADDR_NONE) 
    {
      if(strncmp(url,httpstr,7))
	strcat(request,httpstr);
      strcat(request,purl);
      myport=proxyport;
      myip=proxyip;
    }
    else
    {
      if(!(sptr=url2hostport(purl,&host,&myip,&myport)))
        throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
      if (host)
	free (host);
      strcat (request, sptr);
    }
    sprintf (agent, " HTTP/1.0\r\nUser-Agent: %s/%s\r\n\r\n",
	     "vsplay","0.6");
    strcat (request, agent);
    server.sin_family = AF_INET;
    server.sin_port = htons(myport);
    server.sin_addr.s_addr = myip;
    if((sock=socket(PF_INET,SOCK_STREAM,6))<0)
      throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
    if(connect(sock,(struct sockaddr *)&server,sizeof(server)))
      throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
    writestring(sock,request);
    if(!(_fp=fdopen(sock, "rb")))
      throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
    relocate=false;
    purl[0]='\0';
    readstring(request,linelength-1,_fp);
    if((sptr=strchr(request,' ')))
    {
      switch(sptr[1])
      {
        case '3':relocate=true;
        case '2':break;
        default: throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
      }
    }
    do{
      readstring(request,linelength-1,_fp);
      if(!strncmp(request,"Location:",9))
	strncpy (purl,request+10,1023);
    }while(request[0]!='\r' && request[0]!='n');
  }while(relocate && purl[0] && numrelocs++<5);
  if(relocate)
    throw Vsplayexception(SOUND_ERROR_HTTPFAIL);
  free(purl);
  free(request);
}

