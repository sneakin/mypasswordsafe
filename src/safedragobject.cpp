#include <qstring.h>
#include <qdom.h>
#include "safe.hpp"
#include "safedragobject.hpp"

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
  else {
    if(oneItemAndEntry())
      return true;
  }

  return false;
}

QByteArray SafeDragObject::encodedData(const char *mime_type) const
{
  QByteArray ret;
  // FIXME: implement
  return ret;
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
