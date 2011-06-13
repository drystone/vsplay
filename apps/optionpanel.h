/* Sound Player

   Copyright (C) 1997 by Jung woo-jae */

#include <qcheckbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>

class OptionPanel : public QDialog
{
  Q_OBJECT
public:
  OptionPanel(QWidget *parent=0,char *name=0);
  ~OptionPanel();

public slots:
  void SelectOk();
  void SelectCancel();
  void setthread(int);

private:
  QCheckBox   *shuffle_checkbox,*forcetomono_checkbox,
              *repeat_checkbox,*half_checkbox;
  int         thread_num;
  char        thread_string[20];
  QLabel      *thread_label;
  QSlider     *thread_slider;
  QPushButton *ok_button,*cancel_button;
};
