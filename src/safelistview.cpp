/* $Header: /home/cvsroot/MyPasswordSafe/src/safelistview.cpp,v 1.1.1.1 2004/05/04 22:47:07 nolan Exp $
 */
#include <qpixmap.h>
#include "myutil.hpp"
#include "safelistview.hpp"

SafeListViewItem::SafeListViewItem(SafeListView *parent, SafeItem *item)
  : QListViewItem(parent), m_item(item)
{
  assert(m_item != NULL);

  //setPixmap(0, QPixmap::fromMimeSource("password.png"));
  setGroup("/");
  setDragEnabled(true);
  setDropEnabled(true);
}

SafeListViewItem::SafeListViewItem(SafeListViewGroup *parent,
				   SafeItem *item)
  : QListViewItem(parent), m_item(item)
{
  assert(m_item != NULL);

  //setPixmap(0, QPixmap::fromMimeSource("password.png"));
  setGroup(parent->fullname());
  setDragEnabled(true);
  setDropEnabled(true);
}

SafeListViewItem::~SafeListViewItem()
{
}

void SafeListViewItem::setSelected(bool yes)
{
  setMultiLinesEnabled(yes);
  QListViewItem::setSelected(yes);
  setup();
}

QString SafeListViewItem::text(int col) const
{
  if(col == 0) {
    return getName();
  }
  else if(col == 1) {
    return getUser();
  }
  else if(col == 2) {
    if(isSelected() == false) {
      QString notes(getNotes());
      int pos = notes.find('\n');
      if(pos > 0)
	return notes.left(pos);
    }

    return getNotes();
  }
#ifdef DEBUG
  else if(col == 3) {
    return getGroup();
  }
#endif
  else {
    return QString::null;
  }
}

void SafeListViewItem::setName(const QString &name)
{
  m_item->setName((const char *)name.utf8());
}

QString SafeListViewItem::getName() const
{
  return QString::fromUtf8(m_item->getName());
}

void SafeListViewItem::setUser(const QString &user)
{
  m_item->setUser((const char *)user.utf8());
}

QString SafeListViewItem::getUser() const
{
  return QString::fromUtf8(m_item->getUser());
}

void SafeListViewItem::setPassword(const EncryptedString &pword)
{
  m_item->setPassword(pword);
}

const EncryptedString &SafeListViewItem::getPassword() const
{
  return m_item->getPassword();
}

void SafeListViewItem::setNotes(const QString &notes)
{
  m_item->setNotes((const char *)notes.utf8());
}

QString SafeListViewItem::getNotes() const
{
  return QString::fromUtf8(m_item->getNotes());
}


void SafeListViewItem::setGroup(const QString &group)
{
  m_item->setGroup((const char *)group.utf8());
}


QString SafeListViewItem::getGroup() const
{
  return QString::fromUtf8(m_item->getGroup());
}


SafeListViewGroup::SafeListViewGroup(SafeListView *parent,
				     const QString &name)
  : QListViewItem(parent), m_name(name)
{
  init();
}

SafeListViewGroup::SafeListViewGroup(SafeListViewGroup *parent,
				     const QString &name)
  : QListViewItem(parent), m_name(name)
{
  init();
}


void SafeListViewGroup::init()
{
  setPixmap(0, QPixmap::fromMimeSource("folder_blue.png"));
  //setRenameEnabled(0, true);
  setDragEnabled(true);
  setDropEnabled(true);
}

void SafeListViewGroup::setOpen(bool yes)
{
  if(yes)
    setPixmap(0, QPixmap::fromMimeSource("folder_blue.png"));
  else
    setPixmap(0, QPixmap::fromMimeSource("folder_blue_open.png"));

  QListViewItem::setOpen(yes);
}


void SafeListViewGroup::setName(const QString &name)
{
  m_name = name;
  updateItems();
}


QString SafeListViewGroup::text(int col) const
{
  if(col == 0)
    //return m_fullname.section(SafeItem::GroupSeperator, -1);
    return m_name;
#ifdef DEBUG
  else if(col == 3)
    return fullname();
#endif
  else
    return QString::null;
}


const QString SafeListViewGroup::fullname() const
{
  QListViewItem *p = parent();
  QString fullname = escape(m_name);
  while(p != NULL) {
    if(p->rtti() != RTTI)
      break;

    SafeListViewGroup *parent_group =
      static_cast<SafeListViewGroup *>(p);

    fullname = parent_group->text(0) +
      SafeItem::GroupSeperator + fullname;
    p = p->parent();
  }

  return fullname;
}


QString SafeListViewGroup::escape(const QString &name) const
{
  QString ret(name);
  ret.replace('\\', "\\\\");
  ret.replace('/', "\\/");
  return ret;
}


void SafeListViewGroup::updateItems()
{
  QString name(fullname());
  QListViewItemIterator iter(this);
  while(iter.current()) {
    if(iter.current()->rtti() == SafeListViewItem::RTTI) {
      SafeListViewItem *item = (SafeListViewItem *)iter.current();
      item->setGroup(name);
    }
    iter++;
  }
}


bool SafeListViewGroup::acceptDrop(const QMimeSource *mime) const
{
  return false;
}


void SafeListViewGroup::dropped(QDropEvent *event)
{
  DBGOUT("Drop on group");
}


SafeListView::SafeListView(QWidget *parent, const char *name, Safe *safe)
  : QListView(parent, name)
{
  setShowSortIndicator(true);
  setAllColumnsShowFocus(true);
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
  setRootIsDecorated(true);

  addColumn(tr("Name"));
  addColumn(tr("User"));
  addColumn(tr("Notes"));
#ifdef DEBUG
  addColumn(tr("Group"));
#endif
  setSafe(safe);
}

SafeListView::~SafeListView()
{
}

void SafeListView::setResizePolicy(QListView::WidthMode mode)
{
  QScrollView::setResizePolicy((QScrollView::ResizePolicy)mode);
}

void SafeListView::setSafe(Safe *safe)
{
  m_safe = safe;
  populate();
}

SafeListViewItem *SafeListView::getSelectedItem()
{
  QListViewItem *item = selectedItem();
  if(item->rtti() == SafeListViewItem::RTTI)
    return static_cast<SafeListViewItem *>(item);
  else
    return NULL;
}

SafeListViewItem *SafeListView::addItem(SafeItem *item)
{
  if(m_safe) {
    //SafeItem *item_ptr = m_safe->addItem(item);
    QListViewItem *parent = selectedItem();
    if(parent == NULL) {
      return new SafeListViewItem(this, item);
    }
    else if(parent->rtti() != SafeListViewGroup::RTTI) {
      parent = parent->parent();
      if(parent == NULL) {
	return new SafeListViewItem(this, item);
      }
    }

    parent->setOpen(true);
    return new
      SafeListViewItem(static_cast<SafeListViewGroup *>(parent),
		       item);
  }

  return NULL;
}

void SafeListView::delItem(SafeListViewItem *item)
{
  if(m_safe) {
    m_safe->delItem(item->item());
    delete item;
  }
}


SafeListViewGroup *SafeListView::findGroup(const QString &group_name)
{
  QListViewItemIterator iter(this);
  while(iter.current()) {
    if(iter.current()->rtti() == SafeListViewGroup::RTTI) {
      SafeListViewGroup *group =
	static_cast<SafeListViewGroup *>(iter.current());
	if(group->fullname() == group_name)
	  return group;
    }

    iter++;
  }

  return NULL;
}


SafeListViewGroup *SafeListView::addGroup(const QString &group_name)
{
  // if the group's name is empty,
  if(group_name.isEmpty())
    return NULL;
  // if name has zero seperators or
  else {
    QString group_path(group_name);

    if(group_path.at(0) == '/') {
      for(int i = 0; i < group_path.length(); i++) {
	if(group_path.at(i) != '/') {
	  group_path.remove(0, i);
	  break;
	}
      }
      DBGOUT("Group path: " << group_path);
    }

    SafeListViewGroup *group = findGroup(group_path);
    if(group != NULL) {
      DBGOUT("Found group " << group_path);
      return group;
    }

    DBGOUT("adding group: " << group_path);

    // FIXME: doesn't divide the group's name right if GroupSeperator
    // is escaped
    QString this_group = thisGroup(group_path);
    QString parent_group = parentGroup(group_path);
    DBGOUT("this_group = " << this_group);

    if(parent_group.isEmpty()) {
      return new SafeListViewGroup(this, this_group);
    }
    else {
      DBGOUT("parent = " << parent_group);
      // search for the parent
      // if it doesn't exist add it
      SafeListViewGroup *parent = addGroup(parent_group);
      parent->setOpen(true);
      return new SafeListViewGroup(parent, this_group);
    }
  }

  return NULL;
}


void SafeListView::startDrag()
{
  DBGOUT("Drag started");
}

void SafeListView::populate()
{
  clear();

  if(m_safe) {
    for(Safe::iterator iter = m_safe->firstItem();
	iter != m_safe->lastItem();
	iter++) {
      SafeItem *item = *iter;
      QString group_name(item->getGroup());
      if(!group_name.isEmpty()) {
	SafeListViewGroup *group(addGroup(group_name));
	assert(group != NULL);
	group->setOpen(true);
	(void)new SafeListViewItem(group, item);
      }
      else
	(void)new SafeListViewItem(this, item);
    }
  }
}

QString SafeListView::thisGroup(const QString &group)
{
  unsigned int i;

  DBGOUT("length: " << group.length());

  for(i = (group.length() - 1); i != 0; i--) {
    if(group[i] == SafeItem::GroupSeperator) {
      if(i != 0 && group[i-1] == '\\') {
	continue;
      }
      else {
	break;
      }
    }
  }

  if(group[i] == SafeItem::GroupSeperator)
    i++;

  QString ret;
  for(; i != group.length(); i++) {
    if(group[i] == '\\') {
      i++;
      if(i != group.length()) {
	char c = group[i];
	if(c == '\\')
	  ret += '\\';
	else if(c == '/')
	  ret += '/';
      }
      continue;
    }

    ret += group[i];
  }
  return ret;

  //.section(SafeItem::GroupSeperator,
  //				    -1);
}

QString SafeListView::parentGroup(const QString &group)
{
  unsigned int i;

  for(i = group.length() - 1; i != 0; i--) {
    if(group[i] == SafeItem::GroupSeperator) {
      if(i != 0 && group[i-1] == '\\') {
	continue;
      }
      else {
	break;
      }
    }
  }

  QString ret(group);
  ret.truncate(i);
  return ret;
  //.section(SafeItem::GroupSeperator,
  //				      0, -2)
}
