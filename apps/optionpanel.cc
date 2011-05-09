/* Sound Player

   Copyright (C) 1997 by Jung woo-jae */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "xsplay.h"
#include "optionpanel.h"

OptionPanel::OptionPanel(QWidget *parent,char *name)
  : QDialog(parent,name,TRUE)
{
  setCaption("Options");
  move(mainwidget_x()+10,mainwidget_y()+10);
  resize(200,190);
  setFixedSize(200,190);

  shuffle_checkbox=new QCheckBox("Shuffle",this);
  shuffle_checkbox->move(10,10);
  shuffle_checkbox->setChecked(splay_shuffleflag);

  half_checkbox=new QCheckBox("Half freq",this);
  half_checkbox->move(10,30);
  half_checkbox->setChecked(splay_downfrequency);

  forcetomono_checkbox=new QCheckBox("Force to mono",this);
  forcetomono_checkbox->move(10,50);
  forcetomono_checkbox->setChecked(splay_forcetomonoflag);

  repeat_checkbox=new QCheckBox("Repeat",this);
  repeat_checkbox->move(10,70);
  repeat_checkbox->setChecked(splay_repeatflag);

  thread_label=new QLabel(this);
  thread_label->setGeometry(10,100,100,25);

  thread_num=splay_threadnum;
  thread_slider=new QSlider(0,999,120,thread_num,QSlider::Horizontal,this);
  thread_slider->setTickmarks(QSlider::Below);
  thread_slider->setGeometry(10,120,180,20);
  connect(thread_slider,SIGNAL(valueChanged(int)),
	  this,SLOT(setthread(int)));
  setthread(thread_num);

  ok_button=new QPushButton("Ok",this);
  ok_button->setGeometry(10,height()-40,50,25);
  connect(ok_button,SIGNAL(clicked()),SLOT(SelectOk()));

  cancel_button=new QPushButton("Cancel",this);
  cancel_button->setGeometry(width()-50-10,height()-40,50,25);
  connect(cancel_button,SIGNAL(clicked()),SLOT(SelectCancel()));
}

OptionPanel::~OptionPanel()
{
  delete shuffle_checkbox;
  delete forcetomono_checkbox;

  delete thread_label;
  delete thread_slider;

  delete ok_button;
  delete cancel_button;
}

void OptionPanel::SelectOk()
{
  splay_shuffleflag    =shuffle_checkbox->isChecked();
  splay_forcetomonoflag=forcetomono_checkbox->isChecked();
  splay_repeatflag     =repeat_checkbox->isChecked();
  splay_threadnum      =thread_num;
  splay_downfrequency  =half_checkbox->isChecked();

  accept();
}

void OptionPanel::SelectCancel()
{
  reject();
}

void OptionPanel::setthread(int value)
{
  thread_num=value;
  sprintf(thread_string,"Buffer: %3d\n",value);
  thread_label->setText(thread_string);
}

void RunOptionPanel(QWidget *widget)
{
  OptionPanel op(widget,"Option Panel");
  op.exec();
}
