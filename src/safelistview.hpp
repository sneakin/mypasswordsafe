/* $Header: /home/cvsroot/MyPasswordSafe/src/safelistview.hpp,v 1.9 2004/08/02 04:03:49 nolan Exp $
 */
#ifndef SAFELISTVIEW_HPP
#define SAFELISTVIEW_HPP

#include <qlistview.h>
#include "safe.hpp"

/* SafeListView is a basic list view w/o
 * tree support.
 */
class SafeListView;
class SafeListViewGroup;

class SafeListViewItem: public QListViewItem
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

  virtual QString text(int col = 0) const;

  inline SafeGroup *group() const { return (SafeGroup *)item(); }

  virtual void dropped(QDropEvent *event);

protected:
  void init();
};

class SafeListView: public QListView
{
  Q_OBJECT;

public:
  SafeListView(QWidget *parent = NULL,
	       const char *name = NULL, Safe *safe = NULL);
  virtual ~SafeListView();

  virtual void setResizePolicy(QListView::WidthMode mode);

  void setSafe(Safe *safe);
  SafeItem *getSelectedItem();

  virtual void startDrag();
  //void dropped(QDropEvent *, SafeListViewItem *);

  //signals:
  //void contextMenuRequested(QListViewItem *, const QPoint &, int);
  //void doubleClicked(QListViewItem *);

  void itemChanged(SafeItem *);
  void itemAdded(SafeItem *, SafeGroup *);
  void itemDeleted(SafeItem *);

  void dropped(QDropEvent *, SafeListViewItem *);

public slots:
 void dropped(QDropEvent *);

private:
  SafeListViewItem *findItem(SafeItem *);
  void populate(SafeGroup *group, SafeListViewGroup *view = NULL);

  Safe *m_safe;
};

#endif
