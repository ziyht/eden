#include <plg_non_gui.h>
#include <QDebug>
#include "sub.h"

void PlgNonGui::showInfo()
{
  qDebug("non gui test");

  int c = extdemo1_sub(2, 5);
}
