/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.hpp,v 1.18 2005/11/23 13:21:29 nolan Exp $
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
#ifndef SAFE_HPP
#define SAFE_HPP

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>
#include <qdatetime.h>
#include <vector>
#include <map>
#include "securedstring.hpp"
#include "encryptedstring.hpp"
#include "uuid.hpp"

using std::map;
using std::vector;

class QDomElement;
class QDomDocument;
class QDomNode;

class SafeSerializer;
class SafeGroup;
class Safe;

SafeGroup *findGroup(SafeGroup *group, const QString &full_group);
SafeGroup *findOrCreateGroup(Safe *safe, const QString &group_name);

class SafeItem
{
public:
  SafeItem(SafeGroup *parent);
  virtual ~SafeItem();

  inline SafeGroup *parent() { return m_parent; }
  inline const SafeGroup *parent() const { return m_parent; }

  inline Safe *safe() { return m_safe; }
  inline const Safe *safe() const { return m_safe; }

  virtual int rtti() const;

  void setParent(SafeGroup *parent);

private:
  SafeGroup *m_parent;
  Safe *m_safe;
};

class SafeGroup: public SafeItem
{
public:
  class Iterator
  {
    friend class SafeGroup;

  public:
    Iterator(const SafeGroup *group);

    SafeItem *current() const;
    SafeItem *next();
    SafeItem *prev();
    SafeItem *toFirst();
    SafeItem *toLast();

    bool atFirst() const;
    bool atLast() const;

    inline SafeItem *operator *() const { return current(); }
    inline SafeItem *operator++() { return next(); }
    inline SafeItem *operator--() { return prev(); }

  private:
    QPtrListIterator<SafeItem> m_iter;
  };

  static const int RTTI;

  SafeGroup(SafeGroup *parent, const QString &name = QString::null);
  virtual ~SafeGroup();

  virtual int rtti() const;

  const QString &name() const { return m_name; }
  void setName(const QString &name);

  void addItem(SafeItem *item);
  bool takeItem(SafeItem *item);

  int count() const;

  void empty();
  SafeItem *at(int i);

  Iterator first();
  Iterator last();

private:
  typedef QPtrList<SafeItem> ItemList;
  typedef QPtrListIterator<SafeItem> ItemListIterator;

  QString m_name;
  ItemList m_items;
};

class SafeEntry: public SafeItem
{
public:
  static const char GroupSeperator = '/'; //!< group delimeter
  static const int RTTI;

  SafeEntry(SafeGroup *parent);
  SafeEntry(SafeGroup *parent,
	    const QString &n, const QString &u,
	    const EncryptedString &p, const QString &n);
  SafeEntry(const SafeEntry &item);
  ~SafeEntry();

  virtual int rtti() const;

  void copy(const SafeEntry &item);
  void clear();

  inline const UUID &uuid() const { return m_uuid; }
  inline const unsigned char *policy() const { return m_policy; }

  inline const QDateTime &creationTime() const { return m_creation_time; }
  inline const QDateTime &modifiedTime() const { return m_mod_time; }
  inline const QDateTime &accessTime() const { return m_access_time; }
  inline const QTime &lifetime() const { return m_life_time; }

  inline const QString &name() const { return m_name; }
  inline const QString &user() const { return m_user; }
  inline const EncryptedString &password() const { return m_password; }
  inline const QString &notes() const { return m_notes; }

  void setUUID(const unsigned char u[16]);
  void setUUID(const UUID &u);
  void setPolicy(const unsigned char p[4]);

  void setCreationTime(const QDateTime &t);
  void setModifiedTime(const QDateTime &t);
  void setAccessTime(const QDateTime &t);
  void setLifetime(const QTime &t);

  void setName(const QString &n);
  void setUser(const QString &u);
  void setPassword(const EncryptedString &p);
  void setPassword(const char *p);
  void setNotes(const QString &n);

  void updateModTime();
  void updateAccessTime();

private:
  void init();

  UUID m_uuid;
  unsigned char m_policy[4];
  QDateTime m_creation_time, m_mod_time, m_access_time;
  QTime m_life_time;
  QString m_name, m_user, m_notes;
  EncryptedString m_password;
};

class Safe: public QObject, public SafeGroup
{
  Q_OBJECT;

public:
  typedef vector<SafeEntry *> ItemList; //!< Container for the items
  typedef ItemList::iterator iterator; //!< Iterator shortcut

  Safe();
  virtual ~Safe();

  enum Error {
    Failed, //!< General error condition
    Success, //!< Everything was a success
    BadFormat, //!< Wrong or unsupported filter
    BadFile, //!< File couldn't be opened
    IOError, //!< Trouble reading the file
    UUIDError //!< UUID threw an exception
  };

  static const char *errorToString(Error e);

  static QString getExtensions();
  static QString getTypes();

  static Error checkPassword(const QString &path, const EncryptedString &password);
  static Error checkPassword(const QString &path, const QString &type, const EncryptedString &password);

  Error load(const QString &path, const QString &type, const EncryptedString &passphrase, const QString &def_user);
  Error load(const QString &path, const EncryptedString &passphrase, const QString &def_user);

  Error save(const QString &path, const QString &type, const QString &def_user);
  Error save(const QString &path, const QString &def_user);
  Error save(const QString &def_user);
    
  inline const QString &getPath() const { return m_path; }
  void setPath(const QString &path);
  inline const QString &getType() const { return m_type; }

  inline const EncryptedString getPassPhrase() const { return m_passphrase.get(); }
  void setPassPhrase(const EncryptedString &phrase);

  inline bool hasChanged() { return m_changed; }
  void setChanged(bool value);

  int totalNumEntries(const SafeGroup *group = NULL) const;
  int totalNumGroups(const SafeGroup *group = NULL) const;

signals:
  void changed();
  void itemChanged(SafeItem *);
  void itemAdded(SafeItem *, SafeGroup *);
  void itemDeleted(SafeItem *, SafeGroup *);
  void saved();
  void loaded();

protected:
  int totalNumItems(const SafeGroup *group, int type) const;

  bool makeBackup(const QString &path);
  void setType(const QString &type);
  static SafeSerializer *createSerializer(const QString &extension,
					  const QString &serializer);

private:
  QString m_path, m_type;
  EncryptedString m_passphrase;
  ItemList m_items;
  bool m_changed;
};

#endif
