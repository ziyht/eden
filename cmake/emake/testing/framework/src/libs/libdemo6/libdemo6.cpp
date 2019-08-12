
#include <QDebug>

#include "libdemo6.h"
#include "add.h"
#include "mul.h"

void ParaNonGui::showInfo()
{
  qDebug("non gui test");

  int c = extdemo1_add(2, 5);
  int d = extdemo1_mul(2, 5);
}
