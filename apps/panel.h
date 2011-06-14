/* Sound Player

   Copyright (C) 1997 by Jung woo-jae */


#include <qlabel.h>
#include <qmenubar.h>
#include <qslider.h>
#include <qpushbutton.h>

class Label
{
public:
  Label() {dirty=false;};

  const char   *str;
  QLabel *w;
  bool   dirty;
};

class Panel : public QWidget
{
  Q_OBJECT
public:
  Panel(QWidget *parent=0,const char *name=0);

  friend void Displayerror(const char *errstr);
  friend void Setsongname   (const char *name);
  friend void Setsongmusican(const char *musican);
  friend void Setsongpath   (const char *path);
  friend void Setcurrentsongnumber(int currentrun,int number);
  friend void Setcurrentmpegstatus(int version,int layer,
				   int frequency,int bitrate,
				   bool crc,int mode,
				   bool forcetomono,int downfreq);
  friend void Clearcurrentstatus(void);
  friend void Setframestatus(int frame,int maxframe,
			     int pcmperframe,int frequency);
  friend void Unlockframeslider(void);
  friend void Paneldone(void);

private:
  void Setframeslider(int frame,int maxframe);
  void initializelabel(void);
  void timerEvent(QTimerEvent *e);

public slots:
  void load();
  void loadlist();
  void option();
  void restart();
  void quit();
  void stop();
  void play();
  void pause();
  void previous();
  void next();

  void Volumechanged(int value);
  void Framechanged(int frame);

private:
  bool updateflag,refreshflag;
  bool doneflag,initializelabelflag;

  int timer;
  QMenuBar *menu;

  QPushButton *playbutton,*pausebutton,*stopbutton;
  QPushButton *previousbutton,*nextbutton;

  struct
  {
    char number[20];
    char status[100];
    char framestatus[20];
  }cbuffer;

  struct
  {
    Label path;
    Label number;
    Label name;
    Label musican;
  }currentsong;
  Label currentstatus,framestatus;

  QSlider *volume_slider;
  struct
  {
    void frame_slider();

    QSlider *w;
    int     frame,maxframe;
    bool    dirty;
    bool    lock;
  }frame_slider;
};
