#include <qdom.h>
#include <qstring.h>
#include <qstringlist.h>
#include "safe.hpp"
#include "xmlserializer.hpp"

// register this w/ the factory
XmlSerializer _xml_serializer;


XmlSerializer::XmlSerializer()
  : SafeSerializer("xml", "UNENCRYPTED XML (*.xml)")
{
}

XmlSerializer::~XmlSerializer()
{
}

Safe::Error XmlSerializer::checkPassword(const QString &path, const SecuredString &password)
{
  return Safe::Failed;
}

Safe::Error XmlSerializer::load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &def_user)
{
  return Safe::Failed;
}

Safe::Error XmlSerializer::save(Safe &safe, const QString &path, const QString &def_user)
{
  return Safe::Failed;
}

QDomElement XmlSerializer::safeToXml(QDomDocument &doc, const Safe &safe)
{
  QDomElement e = doc.createElement("safe");
  return e;
}

QDomElement XmlSerializer::safeEntryToXml(QDomDocument &doc, const SafeEntry &entry)
{
  QDomElement e = doc.createElement("item");

  e.appendChild(fieldToXml(doc, "uuid", entry.uuid()));
  e.appendChild(fieldToXml(doc, "name", entry.name()));
  e.appendChild(fieldToXml(doc, "user", entry.user()));
  e.appendChild(fieldToXml(doc, "password",
			   entry.password().get())); // NOTE: password decrypted
  e.appendChild(fieldToXml(doc, "notes", entry.notes(), true));
  e.appendChild(fieldToXml(doc, "created", entry.creationTime()));
  e.appendChild(fieldToXml(doc, "modified", entry.modifiedTime()));
  e.appendChild(fieldToXml(doc, "accessed", entry.accessTime()));
  e.appendChild(fieldToXml(doc, "lifetime", entry.lifetime()));

  // FIXME: xml policy

  return e;
}

QDomElement XmlSerializer::safeGroupToXml(QDomDocument &doc, const SafeGroup &group)
{
  QDomElement e = doc.createElement("group");
  e.setAttribute("name", group.name());

  SafeGroup::Iterator it(&group);
  while(it.current()) {
    SafeItem *item = it.current();
    if(item->rtti() < 0) {
      QDomElement child;
      if(item->rtti() == SafeGroup::RTTI) {
	SafeGroup *g = (SafeGroup *)item;
	child = safeGroupToXml(doc, *g);
      }
      else if(item->rtti() == SafeEntry::RTTI) {
	SafeEntry *i = (SafeEntry *)item;
	child = safeEntryToXml(doc, *i);
      }
      e.appendChild(child);
    }
    ++it;
  }

  return e;
}

bool XmlSerializer::safeFromXml(const QDomDocument &doc, Safe &safe)
{
  return false;
}

bool XmlSerializer::safeEntryFromXml(const QDomElement &root, SafeEntry *entry)
{
  if(root.tagName() == "item") {
    entry->clear();

    QDomNode n = root.firstChild();
    while(!n.isNull()) {
      if(n.isElement()) {
	QDomElement elem(n.toElement());
	QString tagname(elem.tagName());
	QDomNode value(elem.firstChild());

	if(tagname == "uuid") {
	  UUID uuid;
	  uuid.fromString(value.toText().data());
	  entry->setUUID(uuid);
	}
	else if(tagname == "name") {
	  entry->setName(value.toText().data());
	}
	else if(tagname == "user") {
	  entry->setUser(value.toText().data());
	}
	else if(tagname == "password") {
	  entry->setPassword(value.toText().data());
	}
	else if(tagname == "notes") {
	  QString notes;

	  while(!value.isNull()) {
	    if(value.isElement()) {
	      QDomElement line(value.toElement());
	      if(line.tagName() == "line") {
		QDomText text(line.firstChild().toText());
		notes += text.data();
		if(!value.nextSibling().isNull())
		  notes += "\n";
	      }
	    }
	    value = value.nextSibling();
	  }

	  entry->setNotes(notes);
	}
	else if(tagname == "created") {
	  entry->setCreationTime(QDateTime::fromString(value.toText().data(), Qt::ISODate));
	}
	else if(tagname == "modified") {
	  entry->setModifiedTime(QDateTime::fromString(value.toText().data(), Qt::ISODate));
	}
	else if(tagname == "accessed") {
	  entry->setAccessTime(QDateTime::fromString(value.toText().data(), Qt::ISODate));
	}
	else if(tagname == "lifetime") {
	  entry->setLifetime(QTime::fromString(value.toText().data(), Qt::ISODate));
	}
	// quietly ignore unknown elements
#ifdef DEBUG
	else {
	  DBGOUT("Unknown element in item: " << tagname);
	}
#endif
      }
      n = n.nextSibling();
    }

    return true;
  }

  return false;
}

bool XmlSerializer::safeGroupFromXml(const QDomElement &elem, SafeGroup *group)
{
  if(elem.tagName() == "group") {
    QString name = elem.attribute("name");
    if(!name.isEmpty())
      group->setName(name);

    QDomNode n = elem.firstChild();
    while(!n.isNull()) {
      if(n.isElement()) {
	QDomElement item = n.toElement();
	if(item.tagName() == "item") {
	  SafeEntry *entry = new SafeEntry(group);
	  if(!safeEntryFromXml(item, entry)) {
	    delete entry;
	    return false;
	  }
	}
	else if(item.tagName() == "group") {
	  SafeGroup *new_group = new SafeGroup(group);
	  if(!safeGroupFromXml(item, new_group)) {
	    delete group;
	    return false;
	  }
	}
	// quietly ignore unknown items
#ifdef DEBUG
	else {
	  DBGOUT("Unknown element: " << item.tagName());
	}
#endif
      }

      n = n.nextSibling();
    }

    return true;
  }

  return false;
}

QDomElement XmlSerializer::fieldToXml(QDomDocument &doc,
				  const QString &field,
				  const QString &value,
				  bool multiline)
{
  QDomElement xml = doc.createElement(field);

  if(!multiline) {
    xml.appendChild(doc.createTextNode(value));
  }
  else {
    QStringList lines = QStringList::split("\n", value);
    QStringList::Iterator it = lines.begin();
    QStringList::Iterator end = lines.end();

    while(it != end) {
      xml.appendChild(fieldToXml(doc, "line", *it));
      it++;
    }
  }

  return xml;
}

QDomElement XmlSerializer::fieldToXml(QDomDocument &doc,
				  const QString &field,
				  const SecuredString &str)
{
  QDomElement xml = doc.createElement(field);
  xml.appendChild(doc.createTextNode(str.get()));
  return xml;
}

QDomElement XmlSerializer::fieldToXml(QDomDocument &doc,
				  const QString &field,
				  const QDateTime &date)
{
  QDomElement xml = doc.createElement(field);
  xml.appendChild(doc.createTextNode(date.toString(Qt::ISODate)));
  return xml;
}

QDomElement XmlSerializer::fieldToXml(QDomDocument &doc,
				  const QString &field,
				  const QTime &time)
{
  QDomElement xml = doc.createElement(field);
  xml.appendChild(doc.createTextNode(time.toString(Qt::ISODate)));
  return xml;
}

QDomElement XmlSerializer::fieldToXml(QDomDocument &doc,
				  const QString &field,
				  const UUID &uuid)
{
  QDomElement xml = doc.createElement(field);
  xml.appendChild(doc.createTextNode(uuid.toString().c_str()));
  return xml;
}
