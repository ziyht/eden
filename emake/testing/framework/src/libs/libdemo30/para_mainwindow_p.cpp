#include <QDebug>

#include <para_mainwindow_p.h>


void ParaMainwindow_p::buttonClickSlot(bool status)
{
  qDebug()<<__FILE__<<__LINE__<<this->pushbutton->text();
}
