/* $Header: /home/cvsroot/MyPasswordSafe/src/safelistview.hpp,v 1.8 2004/07/31 00:03:52 nolan Exp $
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

  /*
  void setName(const QString &name);
  QString getName() const;

  void setUser(const QString &user);
  QString getUser() const;

  void setPassword(const EncryptedString &pword);
  const EncryptedString &getPassword() const;

  void setNotes(const QString &notes);
  QString getNotes() const;

  void setGroup(const QString &group);
  QString getGroup() const;

  inline time_t getCreationTime() const { return m_item->getCreationTime(); }
  inline time_t getModificationTime() const { return m_item->getModificationTime(); }
  inline time_t getAccessTime() const { return m_item->getAccessTime(); }
  inline time_t getLifetime() const { return m_item->getLifetime(); }

  inline QString getUUID() const { return QString(m_item->getUUID().toString().c_str()); } // FIXME: decouple!!

  inline void updateModTime() { m_item->updateModTime(); }
  inline void updateAccessTime() { m_item->updateAccessTime(); }
  */
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

  virtual bool acceptDrop(const QMimeSource *mime) const;
  virtual void dropped(QDropEvent *event);

  inline SafeGroup *group() const { return (SafeGroup *)item(); }

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

  //signals:
  //void contextMenuRequested(QListViewItem *, const QPoint &, int);
  //void doubleClicked(QListViewItem *);

  void itemChanged(SafeItem *);
  void itemAdded(SafeItem *, SafeGroup *);
  void itemDeleted(SafeItem *);

private:
  SafeListViewItem *findItem(SafeItem *);
  void populate(SafeGroup *group, SafeListViewGroup *view = NULL);

  Safe *m_safe;
};

#endif
