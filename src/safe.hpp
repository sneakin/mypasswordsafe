/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.hpp,v 1.10 2004/07/25 18:29:15 nolan Exp $
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

#include <qstring.h>
#include <vector>
#include <map>
#include "securedstring.hpp"
#include "encryptedstring.hpp"
#include "uuid.hpp"

using std::map;
using std::vector;

class SafeItem
{
public:
  static const char GroupSeperator = '/'; //!< group delimeter

  SafeItem();
  SafeItem(const QString &name, const QString &user,
	   const EncryptedString &password, const QString &notes);
  SafeItem(const QString &name, const QString &user,
	   const EncryptedString &password, const QString &notes,
	   const QString &group);
  SafeItem(const SafeItem &item);

  void copy(const SafeItem &item);
  void clear();

  inline const UUID &getUUID() const { return m_uuid; }
  inline const unsigned char *getPolicy() const { return m_policy; }

  inline time_t getCreationTime() const { return m_creation_time; }
  inline time_t getModificationTime() const { return m_mod_time; }
  inline time_t getAccessTime() const { return m_access_time; }
  inline time_t getLifetime() const { return m_life_time; }

  inline const QString &getName() const { return m_name; }
  inline const QString &getUser() const { return m_user; }
  inline const EncryptedString &getPassword() const { return m_password; }
  inline const QString &getNotes() const { return m_notes; }
  inline const QString &getGroup() const { return m_group; }

  void setUUID(const unsigned char uuid[16]);
  void setUUID(const UUID &uuid);
  void setPolicy(const unsigned char policy[4]);

  void setCreationTime(time_t t);
  void setModificationTime(time_t t);
  void setAccessTime(time_t t);
  void setLifetime(time_t t);

  void setName(const QString &name);
  void setUser(const QString &user);
  void setPassword(const EncryptedString &password);
  void setPassword(const char *password);
  void setNotes(const QString &notes);
  void setGroup(const QString &group);

  void updateModTime();
  void updateAccessTime();

private:
  void init();

  UUID m_uuid;
  unsigned char m_policy[4];
  time_t m_creation_time, m_mod_time, m_access_time, m_life_time;
  QString m_name, m_user, m_notes, m_group;
  EncryptedString m_password;
};

class SafeSerializer;

class Safe
{
public:
  typedef vector<SafeItem *> ItemList; //!< Container for the items
  typedef ItemList::iterator iterator; //!< Iterator shortcut

  Safe();
  ~Safe();

  enum Error {
    Failed, //!< General error condition
    Success, //!< Everything was a success
    BadFormat, //!< Wrong or unsupported filter
    BadFile, //!< File couldn't be opened
    IOError, //!< Trouble reading the file
    UUIDError //!< UUID threw an exception
  };

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

  //SafeItem *findItem(QListViewItem *item);

  SafeItem *addItem(SafeItem &item);
  bool delItem(SafeItem *item);

  /** Returns the list of items.
   */
  ItemList &getItems() { return m_items; }
  /** Returns an iterator for the first item.
   */
  iterator firstItem() { return m_items.begin(); }
  /** Returns an iterator that is one past the last item.
   */
  iterator lastItem() { return m_items.end(); }

  /** Returns the number of items in the safe.
   */
  inline int size() { return m_items.size(); }

  void empty();

  static const char *errorToString(Error e);

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
