#ifndef SAFETEST_HPP
#define SAFETEST_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class XmlSerializerTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(XmlSerializerTest);
  CPPUNIT_TEST(testSafeEntry);
  CPPUNIT_TEST(testSafeGroup);
  CPPUNIT_TEST_SUITE_END();

public:
  void testSafeEntry();
  void testSafeGroup();
};

#endif
