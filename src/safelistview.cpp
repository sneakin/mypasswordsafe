/* $Header: /home/cvsroot/MyPasswordSafe/src/safelistview.cpp,v 1.14 2004/10/02 03:26:43 nolan Exp $
 * Copyright (c) 2004, Semantic Gap Solutions
 * All rights reserved.
 *   
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *  -  Redistributions of source code must retain the
 *     above copyright notice, this list of conditions
 *     and the following disclaimer.
 *  -  Redistributions in binary form must reproduce the
 *     above copyright notice, this list of conditions and
 *     the following disclaimer in the documentation and/or
 *     other materials provided with the distribution.
 *  -  Neither the name of Semantic Gap Solutions nor the
 *     names of its contributors may be used to endorse or
 *     promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <qpixmap.h>
#include <assert.h>
#include <qdatetime.h>
#include <qvaluelist.h>
#include <qdragobject.h>
#include <qmessagebox.h>
#include "myutil.hpp"
#include "safelistview.hpp"
#include "safedragobject.hpp"

SafeListViewItem::SafeListViewItem(SafeListView *parent, SafeItem *i)
  : QListViewItem(parent), m_item(i)
{
  assert(m_item != NULL);

  setDragEnabled(true);
  setDropEnabled(true);
}

SafeListViewItem::SafeListViewItem(SafeListViewGroup *parent, SafeItem *i)
  : QListViewItem(parent), m_item(i)
{
  assert(m_item != NULL);

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
  //setPixmap(0, QPixmap::fromMimeSource("password.png"));
}

SafeListViewEntry::SafeListViewEntry(SafeListViewGroup *parent,
				   SafeEntry *item)
  : SafeListViewItem(parent, item)
{
  //setPixmap(0, QPixmap::fromMimeSource("password.png"));
}

SafeListViewEntry::~SafeListViewEntry()
{
}

void SafeListViewEntry::setSelected(bool yes)
{
  setMultiLinesEnabled(yes);
  QListViewItem::setSelected(yes);
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
  setPixmap(0, QPixmap::fromMimeSource("folder_blue.png"));
  //setRenameEnabled(0, true);
}

void SafeListViewGroup::setOpen(bool yes)
{
  if(yes)
    setPixmap(0, QPixmap::fromMimeSource("folder_blue.png"));
  else
    setPixmap(0, QPixmap::fromMimeSource("folder_blue_open.png"));

  QListViewItem::setOpen(yes);
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
  : QListView(parent, name)
{
  setShowSortIndicator(true);
  setAllColumnsShowFocus(true);
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
  setRootIsDecorated(true);

  addColumn(tr("Name"), 30);
  addColumn(tr("User"), 30);
  addColumn(tr("Notes"), 30);
  addColumn(tr("Last Modified"), 30);
  addColumn(tr("Accessed on"), 30);
  addColumn(tr("Created on"), 30);
  addColumn(tr("Lifetime"), 30);
#ifdef DEBUG
  addColumn(tr("Group"));
#endif
  setSafe(safe);

  connect(this, SIGNAL(dropped(QDropEvent *)), SLOT(dropped(QDropEvent *)));
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
  clear();

  if(m_safe != NULL) {
    populate(m_safe);
  }
}

SafeItem *SafeListView::getSelectedItem()
{
  QListViewItem *item = selectedItem();
  if(item != NULL && (item->rtti() == SafeListViewEntry::RTTI ||
		      item->rtti() == SafeListViewGroup::RTTI))
    return ((SafeListViewItem *)item)->item();

  return NULL;
}

SafeItem *SafeListView::getCurrentItem()
{
  QListViewItem *item = currentItem();
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
    QListViewItem *parent = selectedItem();
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
#endif


void SafeListView::startDrag()
{
  DBGOUT("Drag started");

  SafeDragObject *d = new SafeDragObject(viewport());

  QListViewItemIterator it(this, QListViewItemIterator::Selected |
			   QListViewItemIterator::DragEnabled);
  for(; it.current(); ++it) {
    SafeListViewItem *item = (SafeListViewItem *)it.current();
    d->addItem(item->item());
  }

  m_target_is_child = false;
  m_drop_target = NULL;

  bool drag_ret = d->drag();
  DBGOUT("drag() returned " << drag_ret);

  // drag was a move
  if(drag_ret) {
    // prevent drops from doing anything when they occur on a child
    // of the dragged object
    if(m_target_is_child) {
      QMessageBox::information(this, "Sorry", "Sorry, but items can't be dragged into their children");
    }
    else {
      // remove the item from the list
      SafeListViewItem *item = (SafeListViewItem *)currentItem();
      emit deleteItem(item->item());
    }
  }
  // drag was a copy onto a child
  else if(!drag_ret && m_target_is_child == true) {
    DBGOUT("\tCopy item to child");
    emit dragObjectDropped(d, m_drop_target);
  }

  DBGOUT("\t" << name() << ": Drag ended");
}

void SafeListView::itemChanged(SafeItem *item)
{
  DBGOUT("Item changed");
  SafeListViewItem *list_item = findItem(item);
  list_item->setup();
}

void SafeListView::itemAdded(SafeItem *item, SafeGroup *group)
{
  DBGOUT("Item added:");
  SafeListViewGroup *parent = (SafeListViewGroup *)findItem(group);
  if(parent) {
    parent->setOpen(true);
  }

  if(item->rtti() == SafeEntry::RTTI) {
    DBGOUT("Item name: " << ((SafeEntry *)item)->name());
    if(parent)
      (void)new SafeListViewEntry(parent, (SafeEntry *)item);
    else
      (void)new SafeListViewEntry(this, (SafeEntry *)item);
  }
  else if(item->rtti() == SafeGroup::RTTI) {
    SafeListViewGroup *lv_group;
    if(parent)
      lv_group = new SafeListViewGroup(parent, (SafeGroup *)item);
    else
      lv_group = new SafeListViewGroup(this, (SafeGroup *)item);

    populate((SafeGroup *)item, lv_group);
  }
  else {
    DBGOUT("Unkown item type: " << item->rtti());
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
  DBGOUT("Item dropped");
  if(SafeDragObject::canDecode(event)) {
    DBGOUT("\tSafeDragObject");

    m_drop_target = target;
    m_target_is_child = isTargetChild(event, target);

    if(m_target_is_child) {
      return;
    }

    emit dragObjectDropped(event, target);
  }
}

void SafeListView::dropped(QDropEvent *event)
{
  dropped(event, NULL);
}

/** Checks if the drop occured on a child of the item
 * that got dragged.
 */
bool SafeListView::isTargetChild(QDropEvent *event, SafeListViewItem *target)
{
  // prevent incest
  DBGOUT("isTargetChild: " << event->action());
  if(event->source() == viewport()) {
    DBGOUT("\tWarning!!");
    QListViewItem *item = currentItem();

     // make sure this isn't a child of item
     QListViewItem *p = target;
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
  QListViewItemIterator it(this);
  while(it.current()) {
    QListViewItem *list_item = it.current();

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

/*
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
*/
