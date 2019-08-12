
#ifndef _CAL_H
#define _CAL_H

#include <QtWidgets/QMainWindow>


class Q_DECL_EXPORT Add :public QObject
{
  Q_OBJECT
public:
    Add(){}
    ~Add(){}

public:
  int add(int a, int b);
};

#endif
