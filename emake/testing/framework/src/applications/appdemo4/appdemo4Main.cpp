#include <QCoreApplication>
#include <QDebug>

#include "sub.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);


  int c = extdemo1_sub(2, 5);

  qDebug("test dome1");
  return a.exec();
}
