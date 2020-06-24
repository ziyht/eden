#include <QDebug>

#include <plg_mainwindow_p.h>


void PlgMainwindow_p::buttonClickSlot(bool status)
{
  qDebug()<<__FILE__<<__LINE__<<this->pushbutton->text();
}
