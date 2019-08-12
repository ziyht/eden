#include <QDebug>

#include "plgdepender.h"

#include "para_mainwindow_p.h"
#include "para_mainwindow.h"
#include "para_non_gui.h"

void PlgDepender::showInfo()
{
    qDebug("non gui test");

    ParaMainWindow newWindow;
    ParaNonGui     newNonGui;
}
