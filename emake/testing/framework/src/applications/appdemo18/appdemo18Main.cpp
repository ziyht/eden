#include <QCoreApplication>
#include <QDebug>

#include "add.h"
#include "sub.h"
#include "libdemo5.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  int b = extdemo1_add(2, 5);
  int c = extdemo1_sub(2, 5);

  ParaNonGui newNonGui;

  qDebug("test dome1");
  return a.exec();
}
