
#include <QDebug>

#include "libdemo7.h"
#include "sub.h"
#include "div.h"

void ParaNonGui::showInfo()
{
  qDebug("non gui test");

  int c = extdemo1_sub(2, 5);
  int d = extdemo1_div(2, 5);
}
