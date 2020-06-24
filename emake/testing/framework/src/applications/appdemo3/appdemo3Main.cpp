#include <QCoreApplication>
#include <QDebug>

#include "add.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);


  int c = extdemo1_add(2, 5);

  qDebug("test dome1");
  return a.exec();
}
