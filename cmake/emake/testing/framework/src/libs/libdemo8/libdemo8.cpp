
#include <QDebug>

#include "libdemo8.h"
#include "sub.h"
#include "div.h"
#include "add.h"
#include "mul.h"

void ParaNonGui::showInfo()
{
  qDebug("non gui test");

  int a = extdemo1_sub(2, 5);
  int b = extdemo1_div(2, 5);
  int c = extdemo1_add(2, 5);
  int d = extdemo1_mul(2, 5);
}
