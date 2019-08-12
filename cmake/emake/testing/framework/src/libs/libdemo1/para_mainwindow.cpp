#include <para_mainwindow.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QDebug>
#include <para_mainwindow_p.h>

ParaMainWindow::ParaMainWindow()
{
  d = new ParaMainwindow_p();
  d->layout();

  setCentralWidget(d->centralWidget);
}

ParaMainWindow::~ParaMainWindow()
{

}
