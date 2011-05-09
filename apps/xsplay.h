/* Sound Player (X-interface)

   Copyright (C) 1997 by Woo-jae Jung */

#include "splay.h"

#define MAXFRAMESLIDER 2000

// Strings used frequently
extern char *stopstring,*nonestring,*nullstring;

// Panel.cc
void Displayerror(char *errstr);
bool Msgboxdone(void);

void Setsongname   (const char *name);
void Setsongmusican(const char *musican);
void Setsongpath   (const char *path);
void Setcurrentsongnumber(int crun,int no);
void Setcurrentmpegstatus(int version,int layer,
			  int frequency,int bitrate,
			  bool crc,int mode,
			  bool forcetomono,int downfreq);
void Clearcurrentstatus(void);
void Setframestatus(int frame,int maxframe,
		    int pcmperframe,int frequency);
void Unlockframeslider(void);

void Paneldone(void);

int  mainwidget_x(void);
int  mainwidget_y(void);

void Getxarg(int& argc,char *argv[]);
void SetupPanel(void);
int  RunPanel(void);


//
// Functions.cc
//
extern bool splay_exitwhendone;
// directory environment
char *Getcurrentpath(void);
void Setcurrentpath(char *path);
char *Getcurrentwildcard(void);
void Setcurrentwildcard(char *wildcard);

//
// Xsplay.cc
//
// Control music
bool music_isstop(void);
bool music_ispause(void);
void music_restart(void);
void music_stop(void);
void music_play(void);
void music_pause(void);
void music_previous(void);
void music_next(void);

// frame managemant
void Setframe(int frame);
int  getslidernumber(int frame,int maxframe);

// misc
bool getquotaflag(void);
void setquotaflag(bool flag);

// start up function
void xinit(int& argc,char *argv[]);
int  xmain(void);

