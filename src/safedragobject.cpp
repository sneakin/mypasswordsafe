#include <qstring.h>
#include <qdom.h>
#include "safe.hpp"
#include "safedragobject.hpp"
#include "xmlserializer.hpp"

static const QString SafeDragObject_MimeType = "application/mypasswordsafe";
static const QString SafeDragObject_Text = "text/plain";

SafeDragObject::SafeDragObject(QWidget *src)
  : QDragObject(src), m_xml("MyPasswordSafe"), m_state(Nothing)
{
}

SafeDragObject::~SafeDragObject()
{
}

void SafeDragObject::addItem(SafeItem *item)
{
  if(item != NULL) {
    if(m_state == Nothing && item->rtti() == SafeEntry::RTTI) {
      SafeEntry *ent = (SafeEntry *)item;
      m_text = ent->password().get().get();
      m_state = Single;
    }
    else if(m_state == Single) {
      m_state = Multiple;
    }

    if(item->rtti() == SafeGroup::RTTI) {
      m_xml.appendChild(XmlSerializer::safeGroupToXml(m_xml,
						      *(SafeGroup *)item));
    }
    else if(item->rtti() == SafeEntry::RTTI) {
      m_xml.appendChild(XmlSerializer::safeEntryToXml(m_xml,
						      *(SafeEntry *)item));
    }
  }
}

const char *SafeDragObject::format(int i) const
{
  if(i == 0)
    return SafeDragObject_MimeType;
  else if(i == 1 && m_state == Single) {
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
  else if(mime == SafeDragObject_Text && m_state == Single) {
    return true;
  }

  return false;
}

QByteArray SafeDragObject::encodedData(const char *mime_type) const
{
  if(mime_type == SafeDragObject_MimeType) {
    return m_xml.toCString();
  }
  else if(mime_type == SafeDragObject_Text && m_state == Single) {
    return m_text.utf8();
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
