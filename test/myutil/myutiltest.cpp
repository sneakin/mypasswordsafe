#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <qstring.h>
#include <qapplication.h>
#include <qclipboard.h>
#include "safe.hpp"
#include "myutil.hpp"
#include "myutiltest.hpp"

using namespace std;

void MyUtilTest::testCopyToClipboard()
{
  int argc = 1;
  char *argv[] = { "myutil", NULL};

  QApplication app(argc, argv);

  QClipboard *cb = QApplication::clipboard();
  copyToClipboard("Hello");

  CPPUNIT_ASSERT(cb->text(QClipboard::Clipboard) == "Hello");
  if(cb->supportsSelection())
    CPPUNIT_ASSERT(cb->text(QClipboard::Selection) == "Hello");
}

void MyUtilTest::testEscapeGroup()
{
#define TEST(a, b) \
CPPUNIT_ASSERT(escapeGroup(a) == b)

  TEST(QString::null, QString::null);
  TEST("", "");
  TEST("hello", "hello");
  TEST("/", "\\/");
  TEST("\\", "\\\\");
  TEST("Hello/World", "Hello\\/World");

#undef TEST
}

void MyUtilTest::testUnescapeGroup()
{
#define TEST(a, b) \
CPPUNIT_ASSERT(unescapeGroup(a) == b)

  TEST(QString::null, QString::null);
  TEST("", "");
  TEST("hello", "hello");
  TEST("\\/", "/");
  TEST("\\\\", "\\");
  TEST("Hello\\/World", "Hello/World");
  TEST("he\\\\llo", "he\\llo");

#undef TEST
}

void MyUtilTest::testEscapeException()
{
  unescapeGroup("\\h");
  unescapeGroup("hello\\");
}

void MyUtilTest::testThisGroup()
{
#define TEST(full, grp) \
CPPUNIT_ASSERT(thisGroup(full) == grp)

  TEST("group/subgroup", "subgroup");
  TEST("/group/subgroup", "subgroup");
  TEST("group", "group");
  TEST("/group", "group");
  TEST("\\/group", "\\/group");
  TEST("\\\\group", "\\\\group");
  TEST("/group\\//subgroup", "subgroup");
  TEST("group/\\/subgroup", "\\/subgroup");
  CPPUNIT_ASSERT(thisGroup("/hello//").isEmpty());

#undef TEST
}

void MyUtilTest::testParentGroup()
{
#define TEST(full, grp) \
CPPUNIT_ASSERT(parentGroup(full) == grp)

  TEST("group/subgroup", "group");
  TEST("/group/subgroup", "/group");
  CPPUNIT_ASSERT(parentGroup("group").isEmpty());
  CPPUNIT_ASSERT(parentGroup("/group").isEmpty());
  CPPUNIT_ASSERT(parentGroup("\\/group").isEmpty());
  CPPUNIT_ASSERT(parentGroup("\\group").isEmpty());
  CPPUNIT_ASSERT(parentGroup("\\").isEmpty());
  CPPUNIT_ASSERT(parentGroup("/").isEmpty());
  TEST("/group\\//subgroup", "/group\\/");
  TEST("group/\\/subgroup", "group");

#undef TEST
}

void MyUtilTest::testFindGroup()
{
  Safe *safe = new Safe();
  SafeGroup *g1 = new SafeGroup(safe, "he\\llo");
  SafeGroup *g2 = new SafeGroup(g1, "wo/rld");
  SafeGroup *g3 = new SafeGroup(g2, "good");

  CPPUNIT_ASSERT(findGroup(NULL, "no") == NULL);
  CPPUNIT_ASSERT(findGroup(safe, "/he\\\\llo") == g1);
  CPPUNIT_ASSERT(findGroup(safe, "he\\\\llo") == g1);
  CPPUNIT_ASSERT(findGroup(safe, "/he\\\\llo/wo\\/rld") == g2);
  CPPUNIT_ASSERT(findGroup(safe, "wo/rld") == NULL);
  CPPUNIT_ASSERT(findGroup(safe, QString::null) == NULL);
  CPPUNIT_ASSERT(findGroup(safe, "////") == NULL);
  CPPUNIT_ASSERT(findGroup(safe, "") == NULL);
  CPPUNIT_ASSERT(findGroup(safe, "/") == NULL);
  CPPUNIT_ASSERT(findGroup(safe, "/he\\\\llo/wo\\/rld/good") == g3);
  CPPUNIT_ASSERT(findGroup(safe, "////he\\\\llo") == g1);

  delete safe;
}

void MyUtilTest::testFindOrCreateGroup()
{
  Safe *safe = new Safe();
  SafeGroup *g1 = new SafeGroup(safe, "he\\llo");
  SafeGroup *g2 = new SafeGroup(g1, "wo/rld");

  CPPUNIT_ASSERT(findOrCreateGroup(safe, "he\\\\llo") == g1);
  SafeGroup *g3 = findOrCreateGroup(safe, "/he\\\\llo/wo\\/rld/good");
  CPPUNIT_ASSERT(g3 != NULL);
  CPPUNIT_ASSERT(g3->parent() == g2);

  CPPUNIT_ASSERT(findOrCreateGroup(safe, QString::null) == NULL);
  CPPUNIT_ASSERT(findOrCreateGroup(NULL, "hello") == NULL);
  CPPUNIT_ASSERT(findOrCreateGroup(safe, "//he\\\\llo") == g1);

  delete safe;
}

CPPUNIT_TEST_SUITE_REGISTRATION(MyUtilTest);
