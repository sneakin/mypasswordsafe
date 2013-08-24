#include <QtTest>
#include "passphrasedlg.h"

class TestPassPhraseDlg: public QObject
{
  Q_OBJECT;

  PassPhraseDlg *_dialog;

private slots:
  void init()
  {
    _dialog = new PassPhraseDlg();
    _dialog->show();
  }

  void cleanup()
  {
    delete _dialog;
  }

  void testCloseRejectsDialog()
  {
    _dialog->close();
    QVERIFY(_dialog->result() == PassPhraseDlg::Rejected);
  }

  void testCancelRejectsDialog()
  {
    QTest::mouseClick(_dialog->cancelButton, Qt::LeftButton);
    QVERIFY(_dialog->result() == PassPhraseDlg::Rejected);
  }

  void testOkAccepts()
  {
    QTest::mouseClick(_dialog->okButton, Qt::LeftButton);
    QVERIFY(_dialog->result() == PassPhraseDlg::Accepted);
  }

  void testReturnAccepts()
  {
    QTest::keyClick(_dialog->pwordBox, Qt::Key_Return);
    QVERIFY(_dialog->result() == PassPhraseDlg::Accepted);
  }

  void testTextFieldEchoModeIsPassword()
  {
    QVERIFY(_dialog->pwordBox->echoMode() == QLineEdit::Password);
  }

  void testCancelButtonCanBeHid()
  {
    _dialog->hideCancel(true);
    QVERIFY(_dialog->cancelButton->isHidden() == true);

    _dialog->hideCancel(false);
    QVERIFY(_dialog->cancelButton->isHidden() == false);
  }
};

QTEST_MAIN(TestPassPhraseDlg);
#include "test_passphrasedlg.moc"
