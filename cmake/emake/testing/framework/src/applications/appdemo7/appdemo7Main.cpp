#include <QCoreApplication>
#include <QDebug>

#include "sub.h"
#include "div.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  int b = extdemo1_sub(2, 5);
  int c = extdemo1_div(2, 5);

  qDebug("test dome1");
  return a.exec();
}
