#ifndef MYUTILTEST_HPP
#define MYUTILTEST_PP

class EscapeException;

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class MyUtilTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(MyUtilTest);
  CPPUNIT_TEST(testCopyToClipboard);
  CPPUNIT_TEST(testEscapeGroup);
  CPPUNIT_TEST(testUnescapeGroup);
  CPPUNIT_TEST_EXCEPTION(testEscapeException, EscapeException);
  CPPUNIT_TEST(testThisGroup);
  CPPUNIT_TEST(testParentGroup);
  CPPUNIT_TEST(testFindGroup);
  CPPUNIT_TEST(testFindOrCreateGroup);
  CPPUNIT_TEST_SUITE_END();

public:
  void testCopyToClipboard();
  void testEscapeGroup();
  void testUnescapeGroup();
  void testEscapeException();
  void testThisGroup();
  void testParentGroup();
  void testFindGroup();
  void testFindOrCreateGroup();
};

#endif
