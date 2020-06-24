#include <QString>
#include <QtTest>
#include <cal.h>

class ParaCalcTest:public QObject
{
  Q_OBJECT
public:
  ParaCalcTest();
private:
  Add m_add;
private Q_SLOTS:
  void addCase();
  void addCase1();

  void addBenchmark();

};

ParaCalcTest::ParaCalcTest()
{

}

void ParaCalcTest::addCase()
{
    QVERIFY2(m_add.add(1,1) == 2,"1+1");
    QVERIFY2(m_add.add(-1,-1) == -2,"-1 + -1");
    QVERIFY2(m_add.add(0,0) == 0,"0 + 0");
}

void ParaCalcTest::addCase1()
{
    QVERIFY2(m_add.add(1,1) == 2,"1+1");
    QVERIFY2(m_add.add(-1,-1) == -2,"-1 + -1");
    QVERIFY2(m_add.add(0,0) == 0,"0 + 0");
}

void ParaCalcTest::addBenchmark()
{
  QBENCHMARK{
    m_add.add(1,1);
  }
}

//void ParaBigQTest::isBigCase2()
//{
//  QFETCH(QChar,testChar);
//  QFETCH(bool,rult);

//  QCOMPARE(m_big.isBig(testChar),rult);
//}

//void ParaBigQTest::isBigCase2_data()
//{
//  QTest::addColumn<QChar>("testChar");
//  QTest::addColumn<bool>("rult");

//  QTest::newRow("less char test") << QChar('a') << false;
//  QTest::newRow("big char test") << QChar('A') << true;
//  QTest::newRow("? char test") << QChar('?') << false;
//  QTest::newRow("number char test") << QChar('1')<< false;
//}


//void ParaBigQTest::isBigCase()
//{
//  QVERIFY2(m_big.isBig(QChar('a')) == false, "a check");
//  QVERIFY2(m_big.isBig(QChar('A')) == true, "A check");
//  QVERIFY2(m_big.isBig(QChar('?')) == false, "? check");
//  QVERIFY2(m_big.isBig(QChar('1')) == false, "1 check");
//}

//void ParaBigQTest::setCpuCase()
//{
//  m_big.setCpu(-1);
//  QVERIFY2(m_big.checkOwnCpuIdel() <= 100, "set idel -1 check");
//  QVERIFY2(m_big.checkOwnCpuIdel() >= 0, "set idel -1 check");
//  m_big.setCpu(50);
//  QVERIFY2(m_big.checkOwnCpuIdel() <= 100, "set idel 50 check");
//  QVERIFY2(m_big.checkOwnCpuIdel() >= 0, "set idel 50 check");
//  m_big.setCpu(200);
//  QVERIFY2(m_big.checkOwnCpuIdel() <= 100, "set idel 50 check");
//  QVERIFY2(m_big.checkOwnCpuIdel() >= 0, "set idel 50 check");
//}

//void ParaBigQTest::isBigBenchmark()
//{
//  QBENCHMARK{
//    m_big.isBig(QChar('a'));
//  }
//}


//CTEST_PARA_TARGET_MAIN(ParaBigQTest)

int calcTest(int argc, char *argv[])
{
    ParaCalcTest tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "calcTest.moc"
