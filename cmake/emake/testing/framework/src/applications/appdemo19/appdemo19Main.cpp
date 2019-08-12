#include <QCoreApplication>
#include <QDebug>

#include "para_mainwindow.h"
#include "para_mainwindow_p.h"
#include "para_non_gui.h"

#include "add.h"
#include "sub.h"
#include "mul.h"
#include "div.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  int e = extdemo1_sub(2, 5);
  int b = extdemo1_add(2, 5);
  int c = extdemo1_div(2, 5);
  int d = extdemo1_mul(2, 5);

  ParaMainWindow newWindow;
  ParaNonGui     newNonGui;

  qDebug("test appdemo19");
  return a.exec();
}
