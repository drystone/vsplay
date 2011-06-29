/* Sound Player Version

   Copyright (C) 1997 by Jung woo-jae */

// Header files

#define MAXLISTSIZE        1000     // Too many?
#define MAXFILENAMELENGTH   300     // Too long?

extern int  vsplay_verbose;
extern int vsplay_startframe;
extern char *vsplay_progname;
extern const char *vsplay_devicename;

extern char *vsplay_list[MAXLISTSIZE];
extern int  vsplay_listsize;
extern int  vsplay_downfrequency;
extern bool vsplay_shuffleflag,
            vsplay_repeatflag,
            vsplay_forcetomonoflag,
            vsplay_frameinfo;

extern const char *vsplay_Sounderrors[];

#ifdef PTHREADEDMPEG
extern int  vsplay_threadnum;
#endif

/*****************/
/* Sound quality */
/*****************/
int  Setvolume(int volume);

/***************/
/* Manage list */
/***************/
void arglist(int argc,char *argv[],int start);
void killlist(void);
void addlist(const char *path,const char *filename);
void readlist(char *filename);
void shufflelist(void);

