#include <qstring.h>
#include <qdom.h>
#include "safe.hpp"
#include "safedragobject.hpp"
#include "xmlserializer.hpp"

static const QString SafeDragObject_MimeType = "application/mypasswordsafe";
static const QString SafeDragObject_Text = "text/plain";

SafeDragObject::SafeDragObject(QWidget *src)
  : QDragObject(src)
{
}

SafeDragObject::~SafeDragObject()
{
}

void SafeDragObject::addItem(SafeItem *item)
{
  if(item != NULL) {
    m_items.append(item);
  }
}

const char *SafeDragObject::format(int i) const
{
  if(i == 0)
    return SafeDragObject_MimeType;
  else if(i == 1 && oneItemAndEntry()) {
    return SafeDragObject_Text;
  }
  else
    return NULL;
}

bool SafeDragObject::provides(const char *mime_type) const
{
  QString mime(mime_type);
  if(mime == SafeDragObject_MimeType)
    return true;
  else if(mime == SafeDragObject_Text && oneItemAndEntry()) {
    return true;
  }

  return false;
}

QByteArray SafeDragObject::encodedData(const char *mime_type) const
{
  if(mime_type == SafeDragObject_MimeType) {
    QDomDocument doc("safe");
    QPtrListIterator<SafeItem> it(m_items);
    for(; it.current(); ++it) {
      SafeItem *item = it.current();

      if(item->rtti() == SafeGroup::RTTI) {
	doc.appendChild(XmlSerializer::safeGroupToXml(doc, *(SafeGroup *)item));
      }
      else if(item->rtti() == SafeEntry::RTTI) {
	doc.appendChild(XmlSerializer::safeEntryToXml(doc, *(SafeEntry *)item));
      }
    }

    DBGOUT("encodedData: " << endl << doc.toCString());
    return doc.toCString();
  }
  else if(mime_type == SafeDragObject_Text && oneItemAndEntry()) {
    SafeEntry *i = (SafeEntry *)m_items.getFirst();
    const EncryptedString &es(i->password());
    const SecuredString &sec(es.get());
    DBGOUT("encodedData: " << sec.get());
    return QCString(sec.get());
  }

  return QByteArray(0);
}

bool SafeDragObject::canDecode(const QMimeSource *src)
{
  return src->provides(SafeDragObject_MimeType);
}

bool SafeDragObject::decode(const QMimeSource *src, QDomDocument &xml)
{
  if(canDecode(src)) {
    xml.setContent(src->encodedData(SafeDragObject_MimeType));
    return true;
  }

  return false;
}

bool SafeDragObject::oneItemAndEntry() const
{
  if(m_items.count() == 1) {
    const SafeItem *item = m_items.getFirst();
    if(item->rtti() == SafeEntry::RTTI)
      return true;
  }

  return false;
}
