/* Sound player

   Copyright (C) 1997 by Woo-jae Jung */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include "xsplay.h"
#include "fileselect.h"

#include <qregexp.h>
#include <qpushbutton.h>

FileSelect::FileSelect( QWidget *parent,char *name )
        : QDialog(parent,name,TRUE)
{
  setCaption("Open Files");
  move(mainwidget_x()+10,mainwidget_y()+10);
  resize(400,460);
  setFixedSize(400,460);

  filter_label=new QLabel("Filter:",this);
  filter_label->setGeometry(10,10,60,20);

  filter_ledit=new QLineEdit(this,"filterEdit");
  filter_ledit->setGeometry(70,10,width()-70-10,20);
  filter_ledit->setText(Getcurrentwildcard());
  connect(filter_ledit,SIGNAL(returnPressed()),SLOT(filterentered()));

  {
    char *path=Getcurrentpath();
    
    if(path)dir=new QDir(path);
    else dir=new QDir;
  }

  currentdirectory_label=new QLabel(this,"Current directory");
  currentdirectory_label->setGeometry(10,40,380,20);
  currentdirectory_label->setAlignment(AlignRight);
  currentdirectory_label->setText(dir->absPath());

  dir_listbox=new QListBox(this);
  dir_listbox->setGeometry(10,70,100,300);
  connect(dir_listbox,SIGNAL(selected(int)),SLOT(dirSelected(int)) );

  mselect=new MSelect(this);
  mselect->setGeometry(width()-10-250,70,250,300);
  connect(mselect,SIGNAL(clicked()),SLOT(MSelectclicked()) );
  connect(mselect,SIGNAL(doubleclicked()),SLOT(SelectOk()));

  ok_button=new QPushButton("Ok",this);
  ok_button->setGeometry(10,height()-40,50,25 );
  connect(ok_button,SIGNAL(clicked()),SLOT(SelectOk()));

  all_button=new QPushButton( "All", this );
  all_button->setGeometry(width()/2-25-5,height()-40, 50,25 );
  connect(all_button,SIGNAL(clicked()),SLOT(SelectAll()) );

  cancel_button=new QPushButton( "Cancel", this );
  cancel_button->setGeometry(width()-50-10,height()-40, 50,25 );
  connect(cancel_button,SIGNAL(clicked()),SLOT(reject()) );

  selectedfilenumber_label=new QLabel(this,"Selected");
  selectedfilenumber_label->setGeometry(width()-120,height()-40-30,100,20);
  selectedfilenumber_label->setAlignment(AlignRight);


  readdir();
}

FileSelect::~FileSelect()
{
  Setcurrentwildcard((char *)((const char *)filter_ledit->text()));

  delete filter_label;
  delete filter_ledit;
  delete currentdirectory_label;

  delete dir_listbox;
  delete mselect;

  delete selectedfilenumber_label;

  delete ok_button;
  delete all_button;
  delete cancel_button;
}

void FileSelect::SelectOk()
{
  if(mselect->Getselectedfilenumber())
  {
    int e=mselect->Gettotalfilenumber()-1;
    int k,currentmin,nextmin=1;

    music_stop();
    killlist();

    k=mselect->Getselectedfilenumber();
    while(k)
    {
      currentmin=nextmin;
      nextmin=0x7FFFFFFF;
      for(int i=0;i<=e;i++)
      {
	int n=mselect->IsSelected(i);

	if(n==currentmin)
	{
	  addlist((const char *)dir->absPath(),
		  (const char *)mselect->Cellstring(i));
	  k--;
	}
	if(n>currentmin)
	{
	  if(nextmin>n)nextmin=n;
	}
      }
      if(nextmin==0x7FFFFFFF)break;        // Avoid halting
    }

    music_restart();
    if(splay_shuffleflag)shufflelist();
    accept();
  }
}

void FileSelect::SelectAll()
{
  int e=mselect->Gettotalfilenumber();

  for(int i=0;i<e;i++)
    mselect->Select(i,true);
  mselect->update();
  update();
}

void FileSelect::dirSelected(int index)
{
  if (dir->cd(dir_listbox->text(index)) && dir->isReadable())
  {
    readdir();
    currentdirectory_label->setText(dir->absPath());
    Setcurrentpath((char *)((const char *)dir->absPath()));
  } 
  /* else {
    QMessageBox::message( "Sorry", "Cannot open or read directory." );
    d = tmp;
  }*/
}

void FileSelect::MSelectclicked()
{
  sprintf(selectedfilenumber_string,
	  "Selected : %d/%d",
	  mselect->Getselectedfilenumber(),mselect->Gettotalfilenumber());

  selectedfilenumber_label->setText(selectedfilenumber_string);
}

void FileSelect::filterentered()
{
  if(strcmp(filter_ledit->text(),"")==0)
    filter_ledit->setText("*");
  //  d.setNameFilter( filterEdit->text() );
  readdir();
}

void FileSelect::readdir()
{
  const QFileInfoList *filelist=dir->entryInfoList();
  QFileInfoListIterator l(*filelist);
  QFileInfo *inf;
  QRegExp **regexp;
  char **expression;
  int regexpnum=1;

  {
    char buffer[300];
    const char *str;
    int p,j,k;
    
    str=filter_ledit->text();
    if(str[0]==0)filter_ledit->setText("*");
    str=filter_ledit->text();
    for(p=0;str[p];p++)
      if(str[p]==':')regexpnum++;

    regexp=new (QRegExp *)[regexpnum];
    expression=new (char *)[regexpnum];

    for(p=j=k=0;;p++)
    {
      if(str[p]==':' || str[p]==0)
      {
	if(j)
	{
	  buffer[j]=0;
	  expression[k]=strdup(buffer);
	  regexp[k]=new QRegExp(expression[k],TRUE,TRUE);
	  k++;
	  j=0;
	}
	else regexpnum--;

	if(str[p]==0)break;
      }
      else buffer[j++]=str[p];
    }
  }

  dir_listbox->clear();
  mselect->Clearfilelist();
  for(inf=l.current();inf;inf=++l)
  {
    if(inf->isDir())
      dir_listbox->insertItem(inf->fileName().data());
    else
    {
      for(int k=0;k<regexpnum;k++)
      {
	if(regexp[k]->match(inf->fileName().data())>=0)
	  mselect->Appendfilelist((inf->fileName().data()));
      }
    }
  }

  for(int p=0;p<regexpnum;p++)
  {
    delete regexp[p];
    free(expression[p]);
  }

  delete[] regexp;
  delete[] expression;

  mselect->Donefilelist();
  mselect->update();
  MSelectclicked();
  update();
}

void RunFileSelect(QWidget *widget)
{
  FileSelect fs(widget,"File Select");
  fs.exec();
}
