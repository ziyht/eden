#include <plg_mainwindow.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QDebug>
#include <plg_mainwindow_p.h>

PlgMainWindow::PlgMainWindow()
{
  d = new PlgMainwindow_p();
  d->layout();

  setCentralWidget(d->centralWidget);
}

PlgMainWindow::~PlgMainWindow()
{

}
