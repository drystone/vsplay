/* Sound Player

   Copyright (C) 1997 by Jung woo-jae */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <qpainter.h>

#include "multipleselect.h"

MFile::~MFile()
{
  free((void *)str);
}

MSelect::MSelect(QWidget *parent,const char *name) : QGridView(parent,name)
{
  currentselectnum=1;
  setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  setLineWidth(2);
  setBackgroundColor(white);
  setNumCols(1);
  setNumRows(1);
  setCellWidth(230);
  setCellHeight(20);
//  setTableFlags();
//  setTableFlags(Tbl_vScrollBar |
//		Tbl_clipCellPainting |
//		Tbl_smoothScrolling);
  resize(400,200);

  filelist=new QList<MFile>;
  filelist->setAutoDelete(TRUE);
  Clearfilelist();
  Donefilelist();
}

MSelect::~MSelect()
{
  delete filelist;
}

void MSelect::Clearfilelist()
{
  filelist->clear();
  listnumber=0;
}

void MSelect::Appendfilelist(const char *str)
{
  filelist->append(new MFile(str));
  listnumber++;
}

void MSelect::Donefilelist(void)
{
  setNumRows(listnumber);
  selectednumber=0;
  currentfileindex=0;
}

const char* MSelect::Cellstring(int fileindex)
{
  return ((filelist->at(fileindex))->string());
}

int MSelect::IsSelected(int fileindex)
{
  return ((filelist->at(fileindex))->IsSelected());
}

void MSelect::Select(int fileindex,bool select)
{
  int selectnum=0;

  if(IsSelected(fileindex))selectednumber--;

  if(select)
  {
    selectnum=currentselectnum++;
    selectednumber++;
  }
  (filelist->at(fileindex))->Select(selectnum);
}

void MSelect::paintCell(QPainter* p,int fileindex,int)
{
  int w=cellWidth();
  int h=cellHeight();
  int x2=w-1;
  int y2=h-1;

  if(fileindex==currentfileindex)
  {
    if(hasFocus())p->drawRect(0,0,x2,y2);
    else
    {
      p->setPen(DotLine);
      p->drawRect(0,0,x2,y2);
      p->setPen(SolidLine);
    }
  }

  if(IsSelected(fileindex))
  {
    QBrush brush(black);

    p->fillRect(0+2,0+2,x2-2*2,y2-2*2,brush);
    p->setPen(white);
  }
  else
  {
    p->setPen(white);
    p->drawRect(0+2,0+2,x2-2*2,y2-2*2);
    p->setPen(black);
  }
  p->drawText(0+2,0+2,x2-2*2,y2-2*2,AlignRight,Cellstring(fileindex));
}


void MSelect::mousePressEvent(QMouseEvent* e)
{
  int oldfileindex=currentfileindex;
  QPoint clickedPos=e->pos();

  currentfileindex=rowAt(clickedPos.y());

  if(currentfileindex>=0 && currentfileindex<listnumber)
  {
    Select(currentfileindex,IsSelected(currentfileindex)==0);

    updateCell(oldfileindex,0);
    updateCell(currentfileindex,0);

    emit clicked();
  }
}

void MSelect::mouseDoubleClickEvent(QMouseEvent *e)
{
  int oldfileindex=currentfileindex;
  QPoint clickedPos=e->pos();

  currentfileindex=rowAt(clickedPos.y());

  if(currentfileindex>=0 && currentfileindex<listnumber)
  {
    Select(currentfileindex,true);

    updateCell(oldfileindex,0);
    updateCell(currentfileindex,0);

    emit doubleclicked();
  }
}

void MSelect::focusInEvent(QFocusEvent*)
{
  updateCell(currentfileindex,0);
}    


void MSelect::focusOutEvent(QFocusEvent*)
{
  updateCell(currentfileindex,0);
}    

