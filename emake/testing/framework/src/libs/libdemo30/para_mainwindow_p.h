#ifndef PARA_MAINWINDOW_P_H
#define PARA_MAINWINDOW_P_H
#include <QObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>

class ParaMainwindow_p : public QObject
{
  Q_OBJECT
public:
  ParaMainwindow_p(){}
  ~ParaMainwindow_p(){}
  QHBoxLayout *mainLayout;
  QPushButton *pushbutton;
  QWidget *centralWidget;

  void layout()
  {
    mainLayout = new QHBoxLayout();
    pushbutton = new QPushButton("Test");

    QObject::connect(pushbutton,SIGNAL(clicked(bool)),this,SLOT(buttonClickSlot(bool)));

    mainLayout->addWidget(pushbutton);

    centralWidget = new QWidget();

    centralWidget->setLayout(mainLayout);
  }
private slots:
 void buttonClickSlot(bool status);
};

#endif // PARA_MAINWINDOW_P_H
