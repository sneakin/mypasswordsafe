#include <cppunit/TestFixture.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "securedstring.hpp"

class SecuredStringTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(SecuredStringTest);
  CPPUNIT_TEST(testLength);
  CPPUNIT_TEST(testEquality);
  CPPUNIT_TEST(testSet);
  CPPUNIT_TEST(testClear);
  CPPUNIT_TEST_SUITE_END();

private:
  SecuredString *s1, *s2, *s3;

public:
  void setUp()
  {
    s1 = new SecuredString("Hello");
    s2 = new SecuredString("Hello");
    s3 = new SecuredString("World");
  }

  void tearDown()
  {
    delete s1;
    delete s2;
    delete s3;
  }

  void testEquality()
  {
    CPPUNIT_ASSERT(*s1 == *s2);
    CPPUNIT_ASSERT(*s1 != *s3);
    CPPUNIT_ASSERT(!(*s1 == *s3));
    CPPUNIT_ASSERT(!(*s1 != *s2));
  }

  void testLength()
  {
    s1->set("Goodbye");
    CPPUNIT_ASSERT(s1->length() == 7);
  }

  void testSet()
  {
    s1->set("Hello");
    CPPUNIT_ASSERT(*s1 == *s2);

    s1->set(*s3);
    CPPUNIT_ASSERT(*s1 == *s3);
  }

  void testClear()
  {
    s1->clear();
    CPPUNIT_ASSERT(s1->length() == 0);
    CPPUNIT_ASSERT(s1->get() == NULL);
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SecuredStringTest);

int main(int argc, char *argv[])
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  return runner.run("", false);
}
