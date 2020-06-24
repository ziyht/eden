#include <Plg_non_gui.h>
#include <QDebug>

#include "add.h"
#include "sub.h"

#include "para_mainwindow_p.h"
#include "para_mainwindow.h"
#include "para_non_gui.h"

void PlgNonGui::showInfo()
{
  qDebug("non gui test");

  int a = extdemo1_add(2, 5);
  int b = extdemo1_sub(2, 5);

  ParaMainWindow newWindow;
  ParaNonGui     newNonGui;
}
