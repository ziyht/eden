
#include <QDebug>

#include "libdemo5.h"
#include "add.h"
#include "sub.h"

void ParaNonGui::showInfo()
{
  qDebug("non gui test");

  //int c = extdemo1_add(2, 5);
  int d = extdemo1_sub(2, 5);
}
