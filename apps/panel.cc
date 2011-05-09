/* Sound Player

   Copyright (C) 1997 by Woo-jae Jung */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>

#include "xsplay.h"
#include "panel.h"

#include <qapp.h>
#include <qbrush.h>
#include <qfiledlg.h>
#include <qkeycode.h>
#include <qmsgbox.h>
#include <qpainter.h>
#include <qpixmap.h>

Panel *panel;
QApplication *mainwidget;
static bool errorflag=false;
static char *errorstring;

#define BOXWIDTH        2
#define FONTSIZE        16
#define LABELHEIGHT     30
#define SONGNAME_X      5
#define SONGNAME_Y      35
#define SONGNAME_WIDTH  200
#define SONGNAME_HEIGHT LABELHEIGHT

#define SONGMUSI_X      SONGNAME_X
#define SONGMUSI_Y      (SONGNAME_Y+SONGNAME_HEIGHT)
#define SONGMUSI_WIDTH  SONGNAME_WIDTH
#define SONGMUSI_HEIGHT SONGNAME_HEIGHT

#define SONGPATH_X      SONGNAME_X
#define SONGPATH_Y      (SONGMUSI_Y+SONGMUSI_HEIGHT)
#define SONGPATH_WIDTH  SONGNAME_WIDTH
#define SONGPATH_HEIGHT SONGNAME_HEIGHT

#define FSTATUS_X       SONGPATH_X
#define FSTATUS_Y       (SONGPATH_Y+FONTSIZE)
#define FSTATUS_WIDTH   160
#define FSTATUS_HEIGHT  FONTSIZE

#define FRAME_X         (FSTATUS_X+FSTATUS_WIDTH+10)
#define FRAME_Y         (FSTATUS_Y+2)
#define FRAME_WIDTH     (VOL_X-FRAME_X)
#define FRAME_HEIGHT    13


#define BUTTON_NUMBER   5
#define BUTTON_WIDTH    30
#define BUTTON_HEIGHT   20
#define BUTTONS_X       5
#define BUTTONS_Y       (FSTATUS_Y+FSTATUS_HEIGHT+5)
#define BUTTONS_WIDTH   (BUTTON_WIDTH*BUTTON_NUMBER)

#define SONGNUM_X       (BUTTONS_X+BUTTONS_WIDTH+10)
#define SONGNUM_Y       BUTTONS_Y
#define SONGNUM_WIDTH   80
#define SONGNUM_HEIGHT  20

#define STATUS_X        (SONGNAME_X+SONGNAME_WIDTH+10)
#define STATUS_Y        SONGNAME_Y
#define STATUS_WIDTH    100
#define STATUS_HEIGHT   40

#define VOL_X           (STATUS_X+STATUS_WIDTH+10)
#define VOL_Y           SONGNAME_Y
#define VOL_WIDTH       15
#define VOL_HEIGHT      (PANEL_HEIGHT-35)

#define PANEL_WIDTH    (VOL_X+VOL_WIDTH+20)
#define PANEL_HEIGHT   (BUTTONS_Y+BUTTON_HEIGHT)

Panel::Panel(QWidget *parent,const char *name) : QWidget(parent,name)
{
  resize(PANEL_WIDTH,PANEL_HEIGHT);
  setFixedSize(PANEL_WIDTH,PANEL_HEIGHT);
  setCaption(PACKAGE " " VERSION);

  {
    menu=new QMenuBar(this,"menu");
    QPopupMenu *file=new QPopupMenu;
    QPopupMenu *option=new QPopupMenu;
    CHECK_PTR(file);
    menu->insertItem("&File",file);

    file->insertItem("Open",     this,SLOT(load()),    ALT+Key_O);
    file->insertItem("Open List",this,SLOT(loadlist()),ALT+Key_L);
    file->insertSeparator();
    file->insertItem("Restart"  ,this,SLOT(restart()),ALT+Key_R);
    file->insertSeparator();
    file->insertItem("Quit",     this,SLOT(quit()),    ALT+Key_Q);

    menu->insertItem("&Option",option);

    option->insertItem("Setting",this,SLOT(option()),  ALT+Key_S);
  }

  {
    currentsong.name.w=new QLabel(this,"name");
    currentsong.name.w->setGeometry(SONGNAME_X,SONGNAME_Y,
				  SONGNAME_WIDTH,SONGNAME_HEIGHT);
    currentsong.name.w->setFrameStyle(QFrame::Box | QFrame::Raised);
    currentsong.name.w->setLineWidth(BOXWIDTH);
    currentsong.name.w->setFont(QFont("Times",16,QFont::Bold));
    currentsong.name.w->setAlignment(AlignLeft);

    currentsong.musican.w=new QLabel(this,"musican");
    currentsong.musican.w->setGeometry(SONGMUSI_X,SONGMUSI_Y,
				  SONGMUSI_WIDTH,SONGMUSI_HEIGHT);
    currentsong.musican.w->setFrameStyle(QFrame::Box | QFrame::Raised);
    currentsong.musican.w->setLineWidth(BOXWIDTH);
    currentsong.musican.w->setFont(QFont("Times",16,QFont::Bold));
    currentsong.musican.w->setAlignment(AlignLeft);

    currentsong.path.w=new QLabel(this,"path");
    currentsong.path.w->setGeometry(SONGPATH_X,SONGPATH_Y,
				  SONGPATH_WIDTH,SONGPATH_HEIGHT);
    currentsong.path.w->setAlignment(AlignLeft);


    currentsong.number.w=new QLabel(this,"number");
    currentsong.number.w->setGeometry(SONGNUM_X,SONGNUM_Y,
				    SONGNUM_WIDTH,SONGNUM_HEIGHT);
    currentsong.number.w->setFont(QFont("Times",16,QFont::Bold));
    currentsong.number.w->setAlignment(AlignLeft);

    currentstatus.w=new QLabel(this,"status");
    currentstatus.w->setGeometry(STATUS_X,STATUS_Y,
			       STATUS_WIDTH,STATUS_HEIGHT);
    currentstatus.w->setFont(QFont("Times",10));
    currentstatus.w->setAlignment(AlignLeft);

    framestatus.w=new QLabel(this,"Framestatus");
    framestatus.w->setGeometry(FSTATUS_X,FSTATUS_Y,
			     FSTATUS_WIDTH,FSTATUS_HEIGHT);
    framestatus.w->setFont(QFont("Times",12));
    framestatus.w->setAlignment(AlignLeft);
  }

  {
    QPainter p;
    QPixmap  t(30,20);
    QBrush   brush(black);
    int i;
    
    t.fill(backgroundColor());
    p.begin(&t);
      for(i=0;i<=5;i++)
      {
	p.drawLine(10,10,25, 5+i);
	p.drawLine(10,10,25,15-i);
      }
      p.fillRect( 8, 3, 2,14,brush);
      p.fillRect( 4, 3, 2,14,brush);
    p.end();
    previousbutton=new QPushButton("Previous",this);
    previousbutton->setGeometry(BUTTONS_X+BUTTON_WIDTH*0,BUTTONS_Y,
				BUTTON_WIDTH,BUTTON_HEIGHT);
    previousbutton->setPixmap(t);
    QObject::connect(previousbutton,SIGNAL(clicked()),this,SLOT(previous()));

    t.fill(backgroundColor());
    p.begin(&t);
    p.fillRect(10,5,10,10,brush);
    p.end();
    stopbutton=new QPushButton("Stop",this);
    stopbutton->setGeometry(BUTTONS_X+BUTTON_WIDTH*1,BUTTONS_Y,
				BUTTON_WIDTH,BUTTON_HEIGHT);
    stopbutton->setPixmap(t);
    QObject::connect(stopbutton,SIGNAL(clicked()),this,SLOT(stop()));

    t.fill(backgroundColor());
    p.begin(&t);
      for(i=0;i<=5;i++)
      {
	p.drawLine(5, 5+i,25,10);
	p.drawLine(5,15-i,25,10);
      }
    p.end();
    playbutton=new QPushButton("Play",this);
    playbutton->setGeometry(BUTTONS_X+BUTTON_WIDTH*2,BUTTONS_Y,
				BUTTON_WIDTH,BUTTON_HEIGHT);
    playbutton->setPixmap(t);
    QObject::connect(playbutton,SIGNAL(clicked()),this,SLOT(play()));

    t.fill(backgroundColor());
    p.begin(&t);
      p.fillRect(10,3,3,14,brush);
      p.fillRect(17,3,3,14,brush);
    p.end();
    pausebutton=new QPushButton("Pause",this);
    pausebutton->setGeometry(BUTTONS_X+BUTTON_WIDTH*3,BUTTONS_Y,
				BUTTON_WIDTH,BUTTON_HEIGHT);
    pausebutton->setPixmap(t);
    QObject::connect(pausebutton,SIGNAL(clicked()),this,SLOT(pause()));

    t.fill(backgroundColor());
    p.begin(&t);
      for(i=0;i<=5;i++)
      {
	p.drawLine(5, 5+i,20,10);
	p.drawLine(5,15-i,20,10);
      }

      p.fillRect(20, 3, 2,14,brush);
      p.fillRect(24, 3, 2,14,brush);
    p.end();
    nextbutton=new QPushButton("Previous",this);
    nextbutton->setGeometry(BUTTONS_X+BUTTON_WIDTH*4,BUTTONS_Y,
				BUTTON_WIDTH,BUTTON_HEIGHT);
    nextbutton->setPixmap(t);
    QObject::connect(nextbutton, SIGNAL(clicked()), this, SLOT(next()) );
  }

  {
    volume_slider=new QSlider(QSlider::Vertical,this,"Slider");
    volume_slider->setTickmarks(QSlider::Below);
    volume_slider->setGeometry(width()-20,32,
			       VOL_WIDTH,VOL_HEIGHT);
    volume_slider->setValue(99-Setvolume(-1));
    connect(volume_slider,SIGNAL(valueChanged(int)),
	    SLOT(Volumechanged(int)) );

    frame_slider.w=new QSlider(0,MAXFRAMESLIDER,0,0,
			     QSlider::Horizontal,this,"FSlider");
    frame_slider.w->setTickmarks(QSlider::NoMarks);
    frame_slider.w->setGeometry(FRAME_X,FRAME_Y,
			      FRAME_WIDTH,FRAME_HEIGHT);
    frame_slider.w->setValue(0);
    connect(frame_slider.w,SIGNAL(sliderMoved(int)),
	    SLOT(Framechanged(int)) );
  }

  initializelabelflag=false;
  initializelabel();

  timer=startTimer(200);
}

inline void refreshlabel(Label *l)
{
  if(l->dirty)
  {
    l->dirty=false;
    l->w->setText(l->str);
  }
}

void Panel::Setframeslider(int frame,int maxframe)
{
  if(maxframe>1)
    frame_slider.w->setValue(getslidernumber(frame,maxframe));
}

void Panel::initializelabel(void)
{
  currentsong.name.str   =
  currentsong.musican.str=
  currentsong.number.str =nullstring;
  currentsong.path.str   =nonestring;
  currentstatus.str      =
  framestatus.str        =stopstring;
  refreshflag=true;
}

void Panel::timerEvent(QTimerEvent *)
{
  if(initializelabelflag)
  {
    initializelabel();
    initializelabelflag=false;
  }

  if(refreshflag)
  {
    refreshflag=false;
    updateflag=true;
    if(errorflag)
    {
      char str[250];
      QMessageBox mb;

      sprintf(str,"xsplay: %s",errorstring);

      mb.setText(str);
      mb.setButtonText(0, "Skip");
      mb.show();

      errorflag=false;
    }
    refreshlabel(&currentsong.path);
    refreshlabel(&currentsong.number);
    refreshlabel(&currentsong.name);
    refreshlabel(&currentsong.musican);
    refreshlabel(&currentstatus);
  }

  if(updateflag)
  {
    updateflag=false;
    refreshlabel(&framestatus);
  }

  if(frame_slider.dirty)
  {
    if(!frame_slider.lock)
    {
      frame_slider.dirty=false;
      Setframeslider(frame_slider.frame,frame_slider.maxframe);
    }
  }
  doneflag=true;
}




void Panel::load()
{
  extern void RunFileSelect(QWidget *widget);

  RunFileSelect(this);
}

void Panel::loadlist()
{
  QString fn = QFileDialog::getOpenFileName();
  if(!fn.isEmpty())
  {
    stop();
    readlist((char *)((const char *)fn));
    music_restart();
  }
}

void Panel::restart()
{
  music_stop();
  music_restart();
}

void Panel::quit()
{
  exit(0);
}

void Panel::option()
{
  extern void RunOptionPanel(QWidget *widget);

  RunOptionPanel(this);
}

void Panel::stop()
{
  music_stop();
}

void Panel::play()
{
  music_play();
}

void Panel::pause()
{
  music_pause();
}

void Panel::previous()
{
  music_previous();
}

void Panel::next()
{
  music_next();
}

void Panel::Volumechanged(int value)
{
  int vol;

  vol=99-Setvolume(99-value);

  volume_slider->setValue(vol);
}

void Panel::Framechanged(int frame)
{
  frame_slider.lock=true;
  Setframe(frame);
}


/*********************/
/* Interface routine */
/*********************/
void Displayerror(char *errstr)
{
  errorstring=errstr;
  errorflag=true;
  panel->refreshflag=true;
}

bool Msgboxdone(void)
{
  return !errorflag;
}

inline void Setlabel(Label *l,const char *name)
{
  l->str=name;
  l->dirty=true;
}

void Setsongname(const char *name)
{
  if(name[0])Setlabel(&(panel->currentsong.name),name);
  panel->refreshflag=true;
}

void Setsongmusican(const char *musican)
{
  Setlabel(&(panel->currentsong.musican),musican);
  panel->refreshflag=true;
}

void Setsongpath(const char *path)
{
  Setlabel(&(panel->currentsong.name),nullstring);
  Setlabel(&(panel->currentsong.path),path);
  Setsongmusican(nullstring);
}

void Setcurrentsongnumber(int crun,int no)
{
  if(crun>=no)
    Setlabel(&(panel->currentsong.number),"END");
  else
  {
    sprintf(panel->cbuffer.number,"%d/%d",crun+1,no);
    Setlabel(&(panel->currentsong.number),panel->cbuffer.number);
  }
  
  panel->refreshflag=true;
}

void Setcurrentmpegstatus(int version,int layer,
			  int frequency,int bitrate,
			  bool crc,int mode,
			  bool forcetomono,int downfreq)
{
  if(music_isstop())
    Setlabel(&(panel->currentstatus),nullstring);
  else
  {
    sprintf(panel->cbuffer.status,
	    "MPEG-%d, Layer %d\n"
            "%5.2fKHz%s, "
	    "%dkbps\n"
	    "%s, "
	    "%s%s",
	    version+1,layer,
	    (float)frequency/1000.,downfreq ? "//2" : "",
	    bitrate,
	    (crc ? "Crc" : "No Crc"),
	    (mode ? "Stereo" : "Mono"),
	    forcetomono ? "(M)" : "");
    Setlabel(&(panel->currentstatus),panel->cbuffer.status);
  }
  panel->refreshflag=true;
}

void Clearcurrentstatus(void)
{
  Setlabel(&(panel->currentstatus),nullstring);
  panel->refreshflag=true;
}

void Setframestatus(int frame,int maxframe,
		    int pcmperframe,int frequency)
{
  if(music_isstop())
    Setlabel(&(panel->framestatus),stopstring);
  else
  {
    int m,s,mm,ms;

    s=frame*pcmperframe/frequency;
    m=s/60;s=s%60;
    if(maxframe==0)
      sprintf(panel->cbuffer.framestatus,"%2d:%2d",m,s);
    else
    {
      ms=maxframe*pcmperframe/frequency;
      mm=ms/60;ms=ms%60;
      sprintf(panel->cbuffer.framestatus,
	      "%2d:%2d(%2d:%2d)",m,s,mm,ms);
    }
    Setlabel(&(panel->framestatus),panel->cbuffer.framestatus);
  }

  if(maxframe)
  {
    panel->frame_slider.frame=frame;
    panel->frame_slider.maxframe=maxframe;
    panel->frame_slider.dirty=true;
  }
  panel->updateflag=true;
}

void Unlockframeslider(void)
{
  panel->frame_slider.lock=false;
}

void Paneldone(void)
{
  panel->doneflag=false;
  panel->initializelabelflag=true;
  while(!panel->doneflag)usleep(100);
  while(panel->initializelabelflag)usleep(100); 
}

int mainwidget_x(void)
{
  return panel->x();
}

int mainwidget_y(void)
{
  return panel->y();
}



void Getxarg(int &argc,char *argv[])
{
  mainwidget=new QApplication(argc,argv);
}

void SetupPanel(void)
{
#ifdef USE_KOREAN
  {
    extern void Koreaninit(void);
    Koreaninit();
  }
#endif
  panel=new Panel;
  mainwidget->setMainWidget(panel);
  panel->show();
  QObject::connect(mainwidget,SIGNAL(lastWindowClosed()),
		   mainwidget,SLOT(quit()) );
}

int RunPanel(void)
{
  return mainwidget->exec();
}
