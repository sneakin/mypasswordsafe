#include <QtTest>
#include <qtest.h>
#include "encryptedstring.hpp"
#include "securedstring.hpp"

class TestEncryptedString_EmptyString: public QObject
{
  Q_OBJECT;

  EncryptedString *_string;

private slots:
  void initTestCase();
  void cleanupTestCase();
  void length();
};

void TestEncryptedString_EmptyString::initTestCase()
{
  _string = new EncryptedString("");
}

void TestEncryptedString_EmptyString::cleanupTestCase()
{
  delete _string;
}

void TestEncryptedString_EmptyString::length()
{
  QVERIFY(_string->length() == 0);
}


QTEST_MAIN(TestEncryptedString_EmptyString);
#include "empty_string.moc"