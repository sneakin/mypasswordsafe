/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.hpp,v 1.11 2004/07/26 07:11:30 nolan Exp $
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
#ifndef SAFE_HPP
#define SAFE_HPP

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>
#include <vector>
#include <map>
#include "securedstring.hpp"
#include "encryptedstring.hpp"
#include "uuid.hpp"
#include "xmlable.hpp"

using std::map;
using std::vector;

class QDomElement;
class QDomDocument;
class QDomNode;

class SafeSerializer;
class SafeGroup;
class Safe;

class SafeItem: public Xmlable
{
public:
  SafeItem(SafeGroup *parent);
  virtual ~SafeItem();

  inline SafeGroup *parent() { return m_parent; }
  inline const SafeGroup *parent() const { return m_parent; }

  inline virtual Safe *safe() { return m_safe; }
  inline virtual const Safe *safe() const { return m_safe; }

private:
  SafeGroup *m_parent;
  Safe *m_safe;
};

class SafeGroup: public SafeItem
{
public:
  typedef QPtrList<SafeItem> ItemList;
  typedef QPtrListIterator<SafeItem> ItemListIterator;

  SafeGroup(SafeGroup *parent, const QString &name = QString::null);
  virtual ~SafeGroup();

  const QString &name() const { return m_name; }
  void setName(const QString &name);

  void addItem(SafeItem *item);
  bool takeItem(SafeItem *item);

  int count() const;
  void empty();

  virtual bool fromXml(const QDomNode &node);
  virtual QDomNode toXml(QDomDocument &doc) const;

private:
  QString m_name;
  ItemList m_items;
};

class SafeEntry: public SafeItem
{
public:
  static const char GroupSeperator = '/'; //!< group delimeter

  SafeEntry(SafeGroup *parent);
  SafeEntry(SafeGroup *parent,
	    const QString &n, const QString &u,
	    const EncryptedString &p, const QString &n);
  SafeEntry(SafeGroup *parent,
	    const QString &n, const QString &u,
	    const EncryptedString &p, const QString &n,
	    const QString &g);
  SafeEntry(const SafeEntry &item);

  void copy(const SafeEntry &item);
  void clear();

  inline const UUID &uuid() const { return m_uuid; }
  inline const unsigned char *policy() const { return m_policy; }

  inline time_t creationTime() const { return m_creation_time; }
  inline time_t modificationTime() const { return m_mod_time; }
  inline time_t accessTime() const { return m_access_time; }
  inline time_t lifetime() const { return m_life_time; }

  inline const QString &name() const { return m_name; }
  inline const QString &user() const { return m_user; }
  inline const EncryptedString &password() const { return m_password; }
  inline const QString &notes() const { return m_notes; }
  inline const QString &group() const { return m_group; }

  void setUUID(const unsigned char u[16]);
  void setUUID(const UUID &u);
  void setPolicy(const unsigned char p[4]);

  void setCreationTime(time_t t);
  void setModificationTime(time_t t);
  void setAccessTime(time_t t);
  void setLifetime(time_t t);

  void setName(const QString &n);
  void setUser(const QString &u);
  void setPassword(const EncryptedString &p);
  void setPassword(const char *p);
  void setNotes(const QString &n);
  void setGroup(const QString &g);

  void updateModTime();
  void updateAccessTime();

  virtual bool fromXml(const QDomNode &node);
  virtual QDomNode toXml(QDomDocument &doc) const;

private:
  void init();
  QDomElement fieldToXml(QDomDocument &doc, const QString &field,
			 const QString &value,
			 bool multiline = false) const;
  QDomElement fieldToXml(QDomDocument &doc, const QString &field,
			time_t time) const;

  UUID m_uuid;
  unsigned char m_policy[4];
  time_t m_creation_time, m_mod_time, m_access_time, m_life_time;
  QString m_name, m_user, m_notes, m_group;
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

  Error save(const QString &path, const QString &type,
	    const EncryptedString &passphrase, const QString &def_user);
  Error save(const QString &path, const EncryptedString &passphrase, const QString &def_user);
  Error save(const QString &def_user);
    
  inline const QString &getPath() const { return m_path; }
  void setPath(const QString &path);
  inline const QString &getType() const { return m_type; }

  inline const EncryptedString getPassPhrase() const { return m_passphrase.get(); }
  void setPassPhrase(const EncryptedString &phrase);

  inline bool changed() { return m_changed; }
  void setChanged(bool value);

  inline virtual Safe *safe() { return this; }
  inline virtual const Safe *safe() const { return this; }

protected:
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
