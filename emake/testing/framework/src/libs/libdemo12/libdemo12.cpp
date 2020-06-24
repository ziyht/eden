
#include <QDebug>

#include "libdemo12.h"
#include "sub.h"
#include "add.h"

#include "para_mainwindow_p.h"
#include "para_mainwindow.h"

void ParaNonGui::showInfo()
{
  qDebug("non gui test");

  int a = extdemo1_sub(2, 5);
  int c = extdemo1_add(2, 5);

  ParaMainWindow newWindow;
}
