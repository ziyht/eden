
#include <QDebug>

#include "libdemo4.h"
#include "sub.h"

void ParaNonGui::showInfo()
{
  qDebug("non gui test");

  int c = extdemo1_sub(2, 5);
}
