#include <cppunit/TestFixture.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <qstring.h>
#include <qdom.h>
#include "uuid.hpp"
#include "safe.hpp"
#include "xmlserializer.hpp"
#include "xmlserializertest.hpp"

void XmlSerializerTest::testSafeEntry()
{
  Safe *safe = new Safe();
  SafeEntry *entry = new SafeEntry(safe);
  QString xml_text =
    "<item>"
    "<uuid>bd9fde03-caf5-4931-814e-b44d4b6344f1</uuid>"
    "<name>Name</name>"
    "<user>User</user>"
    "<password>Password</password>"
    "<notes>"
    "<line>Hello</line>"
    "<line>World</line>"
    "</notes>"
    "<created>2004-06-24T00:16:22</created>"
    "<modified>2004-06-24T00:16:22</modified>"
    "<accessed>2004-06-24T00:16:22</accessed>"
    "<lifetime>10:00</lifetime>"
    "</item>";
  QDomDocument xml_doc;
  xml_doc.setContent(xml_text);

  XmlSerializer::safeEntryFromXml(xml_doc.documentElement(), entry);
  CPPUNIT_ASSERT(entry->name() == "Name");
  CPPUNIT_ASSERT(entry->user() == "User");
  CPPUNIT_ASSERT(entry->password() == "Password");
  CPPUNIT_ASSERT(entry->notes() == "Hello\nWorld");
  CPPUNIT_ASSERT(entry->creationTime() == QDateTime::fromString("2004-06-24T00:16:22", Qt::ISODate));
  CPPUNIT_ASSERT(entry->modifiedTime() == QDateTime::fromString("2004-06-24T00:16:22", Qt::ISODate));
  CPPUNIT_ASSERT(entry->accessTime() == QDateTime::fromString("2004-06-24T00:16:22", Qt::ISODate));
  CPPUNIT_ASSERT(entry->lifetime() == QTime::fromString("10:00", Qt::ISODate));

  UUID uuid;
  uuid.fromString("bd9fde03-caf5-4931-814e-b44d4b6344f1");
  CPPUNIT_ASSERT(entry->uuid() == uuid);

  delete safe;
}

void XmlSerializerTest::testSafeGroup()
{
  Safe *safe = new Safe();
  QString xml_text =
    "<group name=\"Hello\">"
    "<group name=\"World\"/>"
    "<item>"
    "<uuid>bd9fde03-caf5-4931-814e-b44d4b6344f1</uuid>"
    "<name>Name</name>"
    "<user>User</user>"
    "<password>Password</password>"
    "<notes>"
    "<line>Hello</line>"
    "<line>World</line>"
    "</notes>"
    "<created>2004-06-24T00:16:22</created>"
    "<modified>2004-06-24T00:16:22</modified>"
    "<accessed>2004-06-24T00:16:22</accessed>"
    "<lifetime>10:00</lifetime>"
    "</item>"
    "</group>";
  QDomDocument xml_doc;
  xml_doc.setContent(xml_text);

  XmlSerializer::safeGroupFromXml(xml_doc.documentElement(), safe);
  CPPUNIT_ASSERT(safe->count() == 2);
  CPPUNIT_ASSERT(((SafeGroup *)safe)->name() == "Hello");

  SafeGroup::Iterator iter(safe);
  while(iter.current()) {
    if(iter.current()->rtti() == SafeEntry::RTTI) {
      SafeEntry *e = (SafeEntry *)iter.current();
      CPPUNIT_ASSERT(e->name() == "Name");
    }
    else if(iter.current()->rtti() == SafeGroup::RTTI) {
      SafeGroup *g = (SafeGroup *)iter.current();
      CPPUNIT_ASSERT(g->name() == "World");
    }
    ++iter;
  }

  delete safe;
}

CPPUNIT_TEST_SUITE_REGISTRATION(XmlSerializerTest);
