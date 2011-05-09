/* Sound Player

   Copyright (C) 1997 by Jung woo-jae */

#include <qlist.h>
#include <qpainter.h>
#include <qtablevw.h>

class MFile
{
public:
  MFile(const char *string)
    {str=strdup(string);selected=false;};
  ~MFile();
  const char *string(void){return str;};
  void Select(int num){selected=num;};
  int  IsSelected(void){return selected;};

private:
  const char *str;
  int  selected;
};

class MSelect : public QTableView
{
  Q_OBJECT
public:
  MSelect(QWidget* parent=0,const char* name=0);
  ~MSelect();
    
  void Clearfilelist();
  void Appendfilelist(const char *str);
  void Donefilelist();
  const char* Cellstring(int fileindex);
  void Select(int fileindex,bool select);
  int  IsSelected(int fileindex);

  int  Getselectedfilenumber() {return selectednumber;};
  int  Gettotalfilenumber()    {return listnumber;};

signals:
  void clicked();
  void doubleclicked();

protected:
  void paintCell(QPainter*,int fileindex,int);
  void mousePressEvent(QMouseEvent* e);
  void mouseDoubleClickEvent(QMouseEvent* e);
  void focusInEvent(QFocusEvent*);
  void focusOutEvent(QFocusEvent*);
    
private:
  QList<MFile> *filelist;
  int currentfileindex;
  int currentselectnum;
  int listnumber,selectednumber;
};
