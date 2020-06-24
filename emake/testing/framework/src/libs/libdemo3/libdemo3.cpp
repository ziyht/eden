
#include <QDebug>

#include "libdemo3.h"
#include "add.h"

void ParaNonGui::showInfo()
{
  qDebug("non gui test");

  int c = extdemo1_add(2, 5);
}
