#ifndef Plg_MAINWINDOW_H
#define Plg_MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class PlgMainwindow_p;
class Q_DECL_EXPORT  PlgMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  PlgMainWindow();
  ~PlgMainWindow();
private:
  PlgMainwindow_p *d;

};



#endif // Plg_MAINWINDOW_H
