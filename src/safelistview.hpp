/* $Header: /home/cvsroot/MyPasswordSafe/src/safelistview.hpp,v 1.2 2004/05/04 22:48:44 nolan Exp $
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
  static const int RTTI = 1001;

  SafeListViewItem(SafeListView *parent, SafeItem *item);
  SafeListViewItem(SafeListViewGroup *parent, SafeItem *item);
  virtual ~SafeListViewItem();

  int rtti() const { return RTTI; }

  virtual void setSelected(bool yes);
  virtual QString text(int col = 0) const;

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

  inline SafeItem *item() { return m_item; }

private:
  SafeItem *m_item;
};

class SafeListViewGroup: public QListViewItem
{
public:
  static const int RTTI = 1002;

  SafeListViewGroup(SafeListView *parent, const QString &name);
  SafeListViewGroup(SafeListViewGroup *parent, const QString &name);

  int rtti() const { return RTTI; }

  virtual void setOpen(bool yes);
  virtual void setName(const QString &name);

  virtual QString text(int col = 0) const;

  const QString fullname() const;

  virtual bool acceptDrop(const QMimeSource *mime) const;
  virtual void dropped(QDropEvent *event);

protected:
  void init();
  QString escape(const QString &name) const;

  void updateItems();

private:
  QString m_name;
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
  SafeListViewItem *getSelectedItem();

  SafeListViewItem *addItem(SafeItem *item);
  // adds an item to the view, and updates its group
  void delItem(SafeListViewItem *item);

  SafeListViewGroup *findGroup(const QString &group_name);
  SafeListViewGroup *addGroup(const QString &group_name);
  // adds a new group specified by a full group name

  virtual void startDrag();

  signals:
  //void contextMenuRequested(QListViewItem *, const QPoint &, int);
  //void doubleClicked(QListViewItem *);

protected:
  void populate();
  QString thisGroup(const QString &group);
  QString parentGroup(const QString &group);


private:
  Safe *m_safe;
};

#endif
