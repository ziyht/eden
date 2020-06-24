#include <plg_non_gui.h>
#include <QDebug>

#include "add.h"
//#include "sub.h"
#include "mul.h"
//#include "div.h"

void PlgNonGui::showInfo()
{
  qDebug("non gui test");

  int a = extdemo1_add(2, 5);
  //int b = extdemo1_sub(2, 5);
  int c = extdemo1_mul(2, 5);
  //int d = extdemo1_div(2, 5);
}
