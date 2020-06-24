#include <plg_non_gui.h>
#include <QDebug>
#include "add.h"

void PlgNonGui::showInfo()
{
  qDebug("non gui test");

  int c = extdemo1_add(2, 5);
}
