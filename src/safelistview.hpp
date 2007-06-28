/* $Header: /home/cvsroot/MyPasswordSafe/src/safelistview.hpp,v 1.15 2005/12/17 10:03:46 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
 * http://www.semanticgap.com/
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
#ifndef SAFELISTVIEW_HPP
#define SAFELISTVIEW_HPP

#include <q3listview.h>
//Added by qt3to4:
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include "safe.hpp"

/* SafeListView is a basic list view w/o
 * tree support.
 */
class SafeListView;
class SafeListViewGroup;

class SafeListViewItem: public Q3ListViewItem
{
public:
  SafeListViewItem(SafeListView *parent, SafeItem *i);
  SafeListViewItem(SafeListViewGroup *parent, SafeItem *i);
  virtual ~SafeListViewItem();

  inline SafeItem *item() const { return m_item; }
  void setItem(SafeItem *i);

  virtual bool acceptDrop(const QMimeSource *mime) const;
  virtual void dropped(QDropEvent *event);

private:
  SafeItem *m_item;
};

class SafeListViewEntry: public SafeListViewItem
{
public:
  static const int RTTI = 1001;

  SafeListViewEntry(SafeListView *parent, SafeEntry *item);
  SafeListViewEntry(SafeListViewGroup *parent, SafeEntry *item);
  virtual ~SafeListViewEntry();

  int rtti() const { return RTTI; }

  virtual void setSelected(bool yes);
  virtual QString text(int col = 0) const;

  inline SafeEntry *entry() const { return (SafeEntry *)item(); }

  virtual void dropped(QDropEvent *event);
};

class SafeListViewGroup: public SafeListViewItem
{
public:
  static const int RTTI = 1002;

  SafeListViewGroup(SafeListView *parent, SafeGroup *group);
  SafeListViewGroup(SafeListViewGroup *parent, SafeGroup *group);

  int rtti() const { return RTTI; }

  virtual void setOpen(bool yes);

  virtual void setText(int col, const QString &);
  virtual QString text(int col = 0) const;

  inline SafeGroup *group() const { return (SafeGroup *)item(); }

  virtual void dropped(QDropEvent *event);

protected:
  void init();
};

class SafeListView: public Q3ListView
{
  Q_OBJECT;

public:
  SafeListView(QWidget *parent = NULL,
	       const char *name = NULL, Safe *safe = NULL);
  virtual ~SafeListView();

  virtual void setResizePolicy(Q3ListView::WidthMode mode);

  void setSafe(Safe *safe);
  SafeItem *getSelectedItem();
  SafeItem *getCurrentItem();

  virtual void startDrag();

  //signals:
  //void contextMenuRequested(QListViewItem *, const QPoint &, int);
  //void doubleClicked(QListViewItem *);

  void itemChanged(SafeItem *);
  void itemAdded(SafeItem *, bool make_current = false);
  void itemDeleted(SafeItem *);

  void dropped(QDropEvent *, SafeListViewItem *);

signals:
  void deleteItem(SafeItem *);
  void dragObjectDropped(QMimeSource *drag, SafeListViewItem *target);
  //void itemRenamed(QListViewItem *, int, const QString &);

public slots:
  void dropped(QDropEvent *);

protected:
#if 0
  virtual void contentsDragEnterEvent(QDragEnterEvent *);
  virtual void contentsDragMoveEvent(QDragMoveEvent *);
#endif

private:
  bool isTargetChild(QDropEvent *event, SafeListViewItem *target);
  SafeListViewItem *findItem(SafeItem *);
  void populate(SafeGroup *group, SafeListViewGroup *view = NULL);

  Safe *m_safe;
  SafeListViewItem *m_drop_target;
  bool m_target_is_child;
};

#endif
