#include <iostream>
#include <qdom.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include "safe.hpp"
#include "xmlserializer.hpp"
#include "safetest.hpp"

using namespace std;

class SafeEntryTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(SafeEntryTest);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST_SUITE_END();

private:
  Safe *safe;

public:
  void setUp()
  {
    safe = new Safe();
  }

  void tearDown()
  {
    delete safe;
  }

  void testConstruction()
  {
    SafeEntry *e = new SafeEntry(safe);
    CPPUNIT_ASSERT(e->name().isEmpty());
    CPPUNIT_ASSERT(e->user().isEmpty());
    CPPUNIT_ASSERT(e->password().length() == 0);
    CPPUNIT_ASSERT(e->notes().isEmpty());
    CPPUNIT_ASSERT(e->creationTime() == e->modifiedTime());
    CPPUNIT_ASSERT(e->modifiedTime() == e->accessTime());
    CPPUNIT_ASSERT(e->lifetime().isNull());
    delete e;

    e = new SafeEntry(safe, "name", "user", "password", "notes");
    CPPUNIT_ASSERT(e->name() == "name");
    CPPUNIT_ASSERT(e->user() == "user");
    CPPUNIT_ASSERT(e->password() == "password");
    CPPUNIT_ASSERT(e->notes() == "notes");
    CPPUNIT_ASSERT(e->creationTime() == e->modifiedTime());
    CPPUNIT_ASSERT(e->modifiedTime() == e->accessTime());
    CPPUNIT_ASSERT(e->lifetime().isNull());

    SafeEntry *e2 = new SafeEntry(*e);
    CPPUNIT_ASSERT(e->uuid() == e2->uuid());
    CPPUNIT_ASSERT(e->name() == e2->name());
    CPPUNIT_ASSERT(e->user() == e2->user());
    CPPUNIT_ASSERT(e->password() == e2->password());
    CPPUNIT_ASSERT(e->notes() == e2->notes());
    CPPUNIT_ASSERT(e->creationTime() == e2->creationTime());
    CPPUNIT_ASSERT(e->modifiedTime() == e2->modifiedTime());
    CPPUNIT_ASSERT(e->accessTime() == e2->accessTime());
    CPPUNIT_ASSERT(e->lifetime() == e2->lifetime());
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SafeEntryTest);


class SafeGroupTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(SafeGroupTest);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST(testIterator);
  CPPUNIT_TEST(testAdd);
  CPPUNIT_TEST_SUITE_END();

private:
  Safe *safe;

public:
  void setUp()
  {
    safe = new Safe();
  }

  void tearDown()
  {
    delete safe;
  }

  void testConstruction()
  {
    SafeGroup *group = new SafeGroup(safe, "Group");
    CPPUNIT_ASSERT(group->name() == "Group");
    CPPUNIT_ASSERT(group->safe() == safe);
    CPPUNIT_ASSERT(group->parent() == safe);
    CPPUNIT_ASSERT(group->count() == 0);
    CPPUNIT_ASSERT(safe->count() == 1);
    CPPUNIT_ASSERT(safe->at(0) == group);

    SafeGroup *g2 = new SafeGroup(group, "Subgroup");
    CPPUNIT_ASSERT(g2->parent() == group);
    CPPUNIT_ASSERT(g2->safe() == safe);

    CPPUNIT_ASSERT(group->count() == 1);
    group->empty();
    CPPUNIT_ASSERT(group->count() == 0);

    delete group;
    CPPUNIT_ASSERT(safe->count() == 0);
  }

  void testIterator()
  {
    SafeEntry *e1 = new SafeEntry(safe, "name", "user", "password", "notes");
    SafeEntry *e2 = new SafeEntry(safe, "name", "user", "password", "notes");
    SafeEntry *e3 = new SafeEntry(safe, "name", "user", "password", "notes");

    SafeGroup::Iterator it(safe);
    CPPUNIT_ASSERT(it.atFirst());
    CPPUNIT_ASSERT(it.current() == e1);
    CPPUNIT_ASSERT(it.next() == e2);
    CPPUNIT_ASSERT(it.prev() == e1);
    CPPUNIT_ASSERT(++it == e2);
    CPPUNIT_ASSERT(*it == e2);
    CPPUNIT_ASSERT(--it == e1);
    CPPUNIT_ASSERT(it.current() == e1);
    CPPUNIT_ASSERT(it.toLast() == e3);
    CPPUNIT_ASSERT(it.atLast());
    CPPUNIT_ASSERT(it.toFirst() == e1);
    CPPUNIT_ASSERT(it.atFirst());
  }

  void testAdd()
  {
    SafeGroup *g = new SafeGroup(safe, "Group");
    CPPUNIT_ASSERT(safe->at(0)->rtti() == SafeGroup::RTTI);
    g = (SafeGroup *)safe->at(0);
    CPPUNIT_ASSERT(g->name() == "Group");
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SafeGroupTest);

class XmlSerializerTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(XmlSerializerTest);
  CPPUNIT_TEST(testEntry);
  CPPUNIT_TEST_SUITE_END();

private:
  Safe *safe;

public:
  void setUp()
  {
    safe = new Safe();
  }

  void tearDown()
  {
    delete safe;
  }

  void testEntry()
  {
    QDomDocument doc("myps");
    SafeEntry *e1 = new SafeEntry(safe, "name", "user", "password", "notes\nnotes");

    QDomElement elem(XmlSerializer::safeEntryToXml(doc, *e1));
    SafeEntry *e2 = new SafeEntry(safe);
    CPPUNIT_ASSERT(XmlSerializer::safeEntryFromXml(elem, *e2) == true);

    CPPUNIT_ASSERT(e1->uuid() == e2->uuid());
    CPPUNIT_ASSERT(e1->name() == e2->name());
    CPPUNIT_ASSERT(e1->user() == e2->user());
    CPPUNIT_ASSERT(e1->password() == e2->password());
    CPPUNIT_ASSERT(e1->notes() == e2->notes());
    // the times are converted to strings because QTime stores milliseconds which
    // don't get saved into XML
    CPPUNIT_ASSERT(e1->creationTime().toString() == e2->creationTime().toString());
    CPPUNIT_ASSERT(e1->modifiedTime().toString() == e2->modifiedTime().toString());
    CPPUNIT_ASSERT(e1->accessTime().toString() == e2->accessTime().toString());
    CPPUNIT_ASSERT(e1->lifetime().toString() == e2->lifetime().toString());
    // FIXME: check policy
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(XmlSerializerTest);

void SafeTest::setUp()
{
  safe = new Safe();
}

void SafeTest::tearDown()
{
  delete safe;
}

void SafeTest::checkSignalState(bool changed, bool it_changed,
				bool it_added, bool it_deleted,
				bool saved, bool loaded)
{
  CPPUNIT_ASSERT(sig_changed == changed);
  CPPUNIT_ASSERT(sig_item_changed == it_changed);
  CPPUNIT_ASSERT(sig_added == it_added);
  CPPUNIT_ASSERT(sig_deleted == it_deleted);
  CPPUNIT_ASSERT(sig_saved == saved);
  CPPUNIT_ASSERT(sig_loaded == loaded);

  sig_changed = sig_item_changed = sig_added = false;
  sig_deleted = sig_saved = sig_loaded = false;
}

void SafeTest::testSignals()
{
  connect(safe, SIGNAL(changed()), this, SLOT(changed()));
  connect(safe, SIGNAL(itemChanged(SafeItem *)), this, SLOT(itemChanged(SafeItem *)));
  connect(safe, SIGNAL(itemAdded(SafeItem *, SafeGroup *)),
	  this, SLOT(itemAdded(SafeItem *, SafeGroup *)));
  connect(safe, SIGNAL(itemDeleted(SafeItem *, SafeGroup *)),
	  this, SLOT(itemDeleted(SafeItem *, SafeGroup *)));
  connect(safe, SIGNAL(saved()), this, SLOT(saved()));
  connect(safe, SIGNAL(loaded()), this, SLOT(loaded()));

  SafeGroup *root = safe;
  root->setName("safe");
  checkSignalState(true, true, false, false, false, false);

  SafeEntry *e = new SafeEntry(safe, "name", "user", "password", "notes");
  checkSignalState(true, false, true, false, false, false);
  e->setName("hello");
  checkSignalState(true, true, false, false, false, false);
  e->setUser("user");
  checkSignalState(true, true, false, false, false, false);
  e->setPassword("newpass");
  checkSignalState(true, true, false, false, false, false);
  e->setNotes("notes");
  checkSignalState(true, true, false, false, false, false);

  SafeGroup *g = new SafeGroup(safe, "hello");
  checkSignalState(true, false, true, false, false, false);
  g->setName("hello");
  checkSignalState(true, true, false, false, false, false);

  delete g;
  checkSignalState(true, false, false, true, false, false);
}

void SafeTest::changed()
{
  cout << "SafeTest::changed" << endl;
  sig_changed = true;
}

void SafeTest::itemChanged(SafeItem *item)
{
  cout << "SafeTest::itemChanged" << endl;
  if(item->rtti() == SafeGroup::RTTI)
    cout << ((SafeGroup*)item)->name() << endl;
  else if(item->rtti() == SafeGroup::RTTI)
    cout << ((SafeEntry*)item)->name() << endl;
  sig_item_changed = true;
}

void SafeTest::itemAdded(SafeItem *item, SafeGroup *group)
{
  cout << "SafeTest::itemAdded to " << group->name() << endl;
  if(item->rtti() == SafeGroup::RTTI)
    cout << ((SafeGroup*)item)->name() << endl;
  else if(item->rtti() == SafeGroup::RTTI)
    cout << ((SafeEntry*)item)->name() << endl;
  sig_added = true;
}

void SafeTest::itemDeleted(SafeItem *item, SafeGroup *group)
{
  cout << "SafeTest::itemDeleted from " << group->name() << endl;
  if(item->rtti() == SafeGroup::RTTI)
    cout << ((SafeGroup*)item)->name() << endl;
  else if(item->rtti() == SafeGroup::RTTI)
    cout << ((SafeEntry*)item)->name() << endl;
  sig_deleted = true;
}

void SafeTest::saved()
{
  cout << "SafeTest::saved" << endl;
  sig_saved = true;
}

void SafeTest::loaded()
{
  cout << "SafeTest::loaded" << endl;
  sig_loaded = true;
}

CPPUNIT_TEST_SUITE_REGISTRATION(SafeTest);



int main(int argc, char *argv[])
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  return runner.run("", false);
}

