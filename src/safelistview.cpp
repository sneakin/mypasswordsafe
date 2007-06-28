/* $Header: /home/cvsroot/MyPasswordSafe/src/safelistview.cpp,v 1.26 2005/12/17 11:33:21 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <qpixmap.h>
#include <qdatetime.h>
#include <q3valuelist.h>
#include <q3dragobject.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <q3mimefactory.h>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include "myutil.hpp"
#include "safelistview.hpp"
#include "safedragobject.hpp"

SafeListViewItem::SafeListViewItem(SafeListView *parent, SafeItem *i)
  : Q3ListViewItem(parent), m_item(i)
{
  Q_ASSERT(m_item != NULL);

  setDragEnabled(true);
  setDropEnabled(true);
}

SafeListViewItem::SafeListViewItem(SafeListViewGroup *parent, SafeItem *i)
  : Q3ListViewItem(parent), m_item(i)
{
  Q_ASSERT(m_item != NULL);

  setDragEnabled(true);
  setDropEnabled(true);
}

SafeListViewItem::~SafeListViewItem()
{
}

void SafeListViewItem::setItem(SafeItem *i)
{
  m_item = i;
}

bool SafeListViewItem::acceptDrop(const QMimeSource *mime) const
{
  return SafeDragObject::canDecode(mime);
}

void SafeListViewItem::dropped(QDropEvent *event)
{
  SafeListView *view = (SafeListView *)listView();
  view->dropped(event, this);
}



SafeListViewEntry::SafeListViewEntry(SafeListView *parent, SafeEntry *item)
  : SafeListViewItem(parent, item)
{
  setPixmap(0, qPixmapFromMimeSource("file_locked.png"));
}

SafeListViewEntry::SafeListViewEntry(SafeListViewGroup *parent,
				   SafeEntry *item)
  : SafeListViewItem(parent, item)
{
  setPixmap(0, qPixmapFromMimeSource("file_locked.png"));
}

SafeListViewEntry::~SafeListViewEntry()
{
}

void SafeListViewEntry::setSelected(bool yes)
{
  setMultiLinesEnabled(yes);
  Q3ListViewItem::setSelected(yes);
  setup();
}

QString SafeListViewEntry::text(int col) const
{
  if(col == 0) {
    return entry()->name();
  }
  else if(col == 1) {
    return entry()->user();
  }
  else if(col == 2) {
    QString notes(entry()->notes());
    if(isSelected() == false) {
      int pos = notes.find('\n');
      if(pos > 0)
	return notes.left(pos);
    }

    return notes;
  }
  else if(col >= 3 && col < 6) {
    QDateTime time;

    if(col == 3)
      time = entry()->modifiedTime();
    else if(col == 4)
      time = entry()->accessTime();
    else if(col == 5)
      time = entry()->creationTime();

    return time.toString(Qt::LocalDate);
  }
  else if(col == 6) {
    return entry()->lifetime().toString();
  }
  else {
    return QString::null;
  }
}

void SafeListViewEntry::dropped(QDropEvent *event)
{
  SafeListView *view = (SafeListView *)listView();
  view->dropped(event, this);
}



SafeListViewGroup::SafeListViewGroup(SafeListView *parent,
				     SafeGroup *group)
  : SafeListViewItem(parent, group)
{
  init();
}

SafeListViewGroup::SafeListViewGroup(SafeListViewGroup *parent,
				     SafeGroup *group)
  : SafeListViewItem(parent, group)
{
  init();
}


void SafeListViewGroup::init()
{
  setPixmap(0, qPixmapFromMimeSource("folder_blue.png"));
  setRenameEnabled(0, true);
}

void SafeListViewGroup::setOpen(bool yes)
{
  if(yes)
    setPixmap(0, qPixmapFromMimeSource("folder_blue.png"));
  else
    setPixmap(0, qPixmapFromMimeSource("folder_blue_open.png"));

  Q3ListViewItem::setOpen(yes);
}

void SafeListViewGroup::setText(int col, const QString &text)
{
  if(col == 0) {
    group()->setName(text);
    group()->safe()->setChanged(true);
  }

  SafeListViewItem::setText(col, text);
}

QString SafeListViewGroup::text(int col) const
{
  if(col == 0)
    //return m_fullname.section(SafeEntry::GroupSeperator, -1);
    return group()->name();
  else
    return QString::null;
}

void SafeListViewGroup::dropped(QDropEvent *event)
{
  SafeListView *view = (SafeListView *)listView();
  view->dropped(event, this);
}

/*
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
      SafeEntry::GroupSeperator + fullname;
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
*/


/*
void SafeListViewGroup::updateItems()
{
  QString name(fullname());
  QListViewItemIterator iter(this);
  while(iter.current()) {
    if(iter.current()->rtti() == SafeListViewEntry::RTTI) {
      SafeListViewEntry *item = (SafeListViewEntry *)iter.current();
      item->setGroup(name);
    }
    iter++;
  }
}
*/



SafeListView::SafeListView(QWidget *parent, const char *name, Safe *safe)
  : Q3ListView(parent, name), m_target_is_child(false)
{
  setShowSortIndicator(true);
  setAllColumnsShowFocus(true);
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
  setRootIsDecorated(false);

  addColumn(tr("Name"), 150);
  addColumn(tr("User"), 100);
  addColumn(tr("Notes"), 150);
  addColumn(tr("Last Modified"), 100);
  addColumn(tr("Accessed on"), 100);
  addColumn(tr("Created on"), 100);
  addColumn(tr("Lifetime"), 100);
#ifdef DEBUG
  addColumn(tr("Group"));
#endif
  setSafe(safe);

  connect(this, SIGNAL(dropped(QDropEvent *)), SLOT(dropped(QDropEvent *)));
}

SafeListView::~SafeListView()
{
}

void SafeListView::setResizePolicy(Q3ListView::WidthMode mode)
{
  Q3ScrollView::setResizePolicy((Q3ScrollView::ResizePolicy)mode);
}

void SafeListView::setSafe(Safe *safe)
{
  m_safe = safe;
  clear();

  if(m_safe != NULL) {
    populate(m_safe);
  }
}

SafeItem *SafeListView::getSelectedItem()
{
  Q3ListViewItem *item = selectedItem();
  if(item != NULL && (item->rtti() == SafeListViewEntry::RTTI ||
		      item->rtti() == SafeListViewGroup::RTTI))
    return ((SafeListViewItem *)item)->item();

  return NULL;
}

SafeItem *SafeListView::getCurrentItem()
{
  Q3ListViewItem *item = currentItem();
  if(item != NULL && (item->rtti() == SafeListViewEntry::RTTI ||
		      item->rtti() == SafeListViewGroup::RTTI))
    return ((SafeListViewItem *)item)->item();

  return NULL;
}

#if 0
SafeListViewEntry *SafeListView::addItem(SafeEntry *item)
{
  if(m_safe) {
    //SafeEntry *item_ptr = m_safe->addItem(item);
    Q3ListViewItem *parent = selectedItem();
    if(parent == NULL) {
      return new SafeListViewEntry(this, item);
    }
    else if(parent->rtti() != SafeListViewGroup::RTTI) {
      parent = parent->parent();
      if(parent == NULL) {
	return new SafeListViewEntry(this, item);
      }
    }

    parent->setOpen(true);
    return new
      SafeListViewEntry(static_cast<SafeListViewGroup *>(parent),
		       item);
  }

  return NULL;
}

void SafeListView::delItem(SafeListViewEntry *item)
{
  if(m_safe) {
    m_safe->delItem(item->item());
    delete item;
  }
}


SafeListViewGroup *SafeListView::findGroup(const QString &group_name)
{
  Q3ListViewItemIterator iter(this);
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
#endif


void SafeListView::startDrag()
{
  DBGOUT("Drag started (is targe child: " << m_target_is_child << ")");

  SafeDragObject *d = new SafeDragObject(viewport());

  Q3ListViewItemIterator it(this, Q3ListViewItemIterator::Selected |
			   Q3ListViewItemIterator::DragEnabled);
  for(; it.current(); ++it) {
    SafeListViewItem *item = (SafeListViewItem *)it.current();
    d->addItem(item->item());

    // copy the user name to the clipboard
    if(item->rtti() == SafeListViewEntry::RTTI) {
	    SafeEntry *ent = ((SafeListViewEntry *)item)->entry();
	    copyToClipboard(ent->user());
    }
  }

  m_drop_target = NULL;
  m_target_is_child = false;

  bool drag_ret = d->drag();
  DBGOUT("\tdrag() returned " << drag_ret);
  DBGOUT("\tsrc: " << d->source() << "\ttarget: " << d->target());

  // drag was a move
  if(drag_ret) {
    DBGOUT("\tMove");
    // prevent drops from doing anything when they occur on a child
    // of the dragged object
    if(m_target_is_child) {
      QMessageBox::information(this, "Sorry", "Sorry, but items can't be dragged into their children");
    }
    // delete the item if it's dropped onto this
    else if(d->source() == d->target()) {
      // remove the item from the list
      SafeListViewItem *item = (SafeListViewItem *)selectedItem();
      emit deleteItem(item->item());
    }
  }
  // drag was a copy onto a child
  else {
    DBGOUT("\tCopy");
    if(m_target_is_child)
      emit dragObjectDropped(d, m_drop_target);
  }

  DBGOUT("\t" << name() << ": Drag ended");
}

void SafeListView::itemChanged(SafeItem *item)
{
  DBGOUT("Item changed");
  SafeListViewItem *list_item = findItem(item);

  // are both needed? I really don't know.
  list_item->setup();
  list_item->repaint();
}

void SafeListView::itemAdded(SafeItem *item, bool make_current)
{
  DBGOUT("Item added:");
  SafeListViewGroup *parent = (SafeListViewGroup *)findItem(item->parent());
  if(parent) {
    parent->setOpen(true);
  }

  SafeListViewItem *lv_item = NULL;

  if(item->rtti() == SafeEntry::RTTI) {
    DBGOUT("Item name: " << ((SafeEntry *)item)->name());

    if(parent)
      lv_item = new SafeListViewEntry(parent, (SafeEntry *)item);
    else
      lv_item = new SafeListViewEntry(this, (SafeEntry *)item);
  }
  else if(item->rtti() == SafeGroup::RTTI) {
    if(parent)
      lv_item = new SafeListViewGroup(parent, (SafeGroup *)item);
    else
      lv_item = new SafeListViewGroup(this, (SafeGroup *)item);

    populate(static_cast<SafeGroup *>(item),
	     static_cast<SafeListViewGroup *>(lv_item));
  }
  else {
    DBGOUT("Unkown item type: " << item->rtti());
  }

  // set the current item?
  if(make_current) {
    DBGOUT("Selecting new item");
    ensureItemVisible(lv_item); // this scrolls the view
    setCurrentItem(lv_item); // and this makes it more apparent
  }
}

void SafeListView::itemDeleted(SafeItem *item)
{
  DBGOUT("Item deleted");
  SafeListViewItem *list_item = findItem(item);
  delete list_item;
}

void SafeListView::dropped(QDropEvent *event, SafeListViewItem *target)
{
  DBGOUT("Item dropped (is target child: " << m_target_is_child << ")");
  DBGOUT("\tAction: " << event->action());
  DBGOUT("\tsrc: " << event->source() << "\ttarget: " << target);
  DBGOUT("\tAccepted: " << event->isAccepted()
	 << "\t" << event->isActionAccepted());
  if(SafeDragObject::canDecode(event)) {
    DBGOUT("\tSafeDragObject");

    m_drop_target = target;
    m_target_is_child = isTargetChild(event, target);
    DBGOUT("\tTarget is child? " << m_target_is_child);

    if(m_target_is_child) {
      return;
    }

    if(event->action() == QDropEvent::Move)
      event->acceptAction();

    emit dragObjectDropped(event, target);
  }
}

void SafeListView::dropped(QDropEvent *event)
{
  dropped(event, NULL);
}

#if 0
void SafeListView::contentsDragEnterEvent(QDragEnterEvent *e)
{
  DBGOUT("Enter event:");
  DBGOUT("\tAction: " << e->action());

  Q3ListView::contentsDragEnterEvent(e);

  if(e->action() == QDropEvent::Move)
    e->acceptAction();
}

void SafeListView::contentsDragMoveEvent(QDragMoveEvent *e)
{
  DBGOUT("Move event:");
  DBGOUT("\tAction: " << e->action());

  Q3ListView::contentsDragMoveEvent(e);

  if(e->action() == QDropEvent::Move)
    e->acceptAction();
}
#endif

/** Checks if the drop occured on a child of the item
 * that got dragged.
 */
bool SafeListView::isTargetChild(QDropEvent *event, SafeListViewItem *target)
{
  // prevent incest
  if(event->source() == viewport()) {
    DBGOUT("\tWarning!!");
    Q3ListViewItem *item = currentItem();

     // make sure this isn't a child of item
     Q3ListViewItem *p = target;
     for(; p != NULL; p = p->parent()) {
       if(p == item) {
	 DBGOUT("\tThe dragged item is a parent");
	 return true;
       }
     }
  }

  return false;
}

SafeListViewItem *SafeListView::findItem(SafeItem *item)
{
  Q3ListViewItemIterator it(this);
  while(it.current()) {
    Q3ListViewItem *list_item = it.current();

    if(list_item->rtti() == SafeListViewEntry::RTTI ||
       list_item->rtti() == SafeListViewGroup::RTTI) {
      SafeListViewItem *casted_item = (SafeListViewItem *)list_item;
      if(casted_item->item() == item)
	return casted_item;
    }
    it++;
  }
  return NULL;
}

void SafeListView::populate(SafeGroup *group, SafeListViewGroup *view)
{
  if(group) {
    DBGOUT("populate");
    SafeGroup::Iterator iter(group);
    while(iter.current()) {
      SafeItem *item = iter.current();
      if(item->rtti() == SafeEntry::RTTI) {
	DBGOUT("Adding item: " << ((SafeEntry *)item)->name());
	if(view == NULL)
	  (void)new SafeListViewEntry(this, (SafeEntry *)item);
	else
	  (void)new SafeListViewEntry(view, (SafeEntry *)item);
      }
      else if(item->rtti() == SafeGroup::RTTI) {
	SafeListViewGroup *g = NULL;
	DBGOUT("Adding group: " << ((SafeGroup *)item)->name());
	if(view == NULL)
	  g = new SafeListViewGroup(this, (SafeGroup *)item);
	else
	  g = new SafeListViewGroup(view, (SafeGroup *)item);
	populate((SafeGroup *)item, g);
      }
      else {
	DBGOUT("Unknown item");
      }
      ++iter;
    }
  }
}

#if 0
QString SafeListView::thisGroup(const QString &group)
{
  unsigned int i;

  DBGOUT("length: " << group.length());

  for(i = (group.length() - 1); i != 0; i--) {
    if(group[i] == SafeEntry::GroupSeperator) {
      if(i != 0 && group[i-1] == '\\') {
	continue;
      }
      else {
	break;
      }
    }
  }

  if(group[i] == SafeEntry::GroupSeperator)
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
}

QString SafeListView::parentGroup(const QString &group)
{
  unsigned int i;

  for(i = group.length() - 1; i != 0; i--) {
    if(group[i] == SafeEntry::GroupSeperator) {
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
}
#endif
