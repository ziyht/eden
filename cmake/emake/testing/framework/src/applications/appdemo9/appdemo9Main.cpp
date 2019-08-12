#include <QApplication>
#include <QDebug>

#include <para_mainwindow.h>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  ParaMainWindow w;
  w.show();
  qDebug("test dome2");
  return a.exec();
}
