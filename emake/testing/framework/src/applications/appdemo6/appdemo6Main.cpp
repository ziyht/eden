#include <QCoreApplication>
#include <QDebug>

#include "add.h"
#include "mul.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  int b = extdemo1_add(2, 5);
  int c = extdemo1_mul(2, 5);

  qDebug("test dome1");
  return a.exec();
}
