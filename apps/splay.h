/* Sound Player Version

   Copyright (C) 1997 by Jung woo-jae */

// Header files

#define MAXLISTSIZE        1000     // Too many?
#define MAXFILENAMELENGTH   300     // Too long?

extern int  splay_verbose;
extern int splay_startframe;
extern char *splay_progname;
extern const char *splay_devicename;

extern char *splay_list[MAXLISTSIZE];
extern int  splay_listsize;
extern int  splay_downfrequency;
extern bool splay_shuffleflag,
            splay_repeatflag,
            splay_forcetomonoflag,
            splay_frameinfo;

extern const char *splay_Sounderrors[];

#ifdef PTHREADEDMPEG
extern int  splay_threadnum;
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

