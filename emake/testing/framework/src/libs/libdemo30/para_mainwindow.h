#ifndef PARA_MAINWINDOW_H
#define PARA_MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class ParaMainwindow_p;
class Q_DECL_EXPORT  ParaMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  ParaMainWindow();
  ~ParaMainWindow();
private:
  ParaMainwindow_p *d;

};



#endif // PARA_MAINWINDOW_H
