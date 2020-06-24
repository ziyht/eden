#include <QApplication>
#include <QDebug>

#include "para_mainwindow.h"
#include "para_non_gui.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  ParaMainWindow w;
  w.show();
  qDebug("test dome2");


  ParaNonGui g;
  g.showInfo();

  return a.exec();
}
