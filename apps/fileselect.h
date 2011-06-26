/* Sound Player

   Copyright (C) 1997 by Woo-jae Jung*/

#include "multipleselect.h"

#include <qdialog.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlineedit.h>


class FileSelect : public QDialog
{
  Q_OBJECT
public:
  FileSelect(QWidget *parent=0,const char *name=0);
  ~FileSelect();
  void readdir();

public slots:
  void SelectOk();
  void SelectAll();
  void dirSelected(int index);
  void MSelectclicked(); 
  void filterentered();

private:
  QPushButton *ok_button,*cancel_button,*all_button;
  QListBox *dir_listbox;
  MSelect  *mselect;
  QLabel *selectedfilenumber_label,
         *currentdirectory_label,
         *filter_label;
  QLineEdit *filter_ledit;

  char selectedfilenumber_string[100];

  QDir *dir;
};
