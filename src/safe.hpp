/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.hpp,v 1.6 2004/06/20 21:25:22 nolan Exp $
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

#include <string>
#include <vector>
#include <map>
#include "securedstring.hpp"
#include "encryptedstring.hpp"
#include "uuid.hpp"

using std::string;
using std::map;
using std::vector;

/** Represents an entry in the Safe.
 * The information contained in each entry is stored in a SafeItem.
 */
class SafeItem
{
public:
  static const char GroupSeperator = '/'; //!< group delimeter

  SafeItem();
  SafeItem(const string &name, const string &user,
	   const EncryptedString &password, const string &notes);
  SafeItem(const string &name, const string &user,
	   const EncryptedString &password, const string &notes,
	   const string &group);

  void clear();

  inline const UUID &getUUID() const { return m_uuid; }
  inline const unsigned char *getPolicy() const { return m_policy; }

  inline time_t getCreationTime() const { return m_creation_time; }
  inline time_t getModificationTime() const { return m_mod_time; }
  inline time_t getAccessTime() const { return m_access_time; }
  inline time_t getLifetime() const { return m_life_time; }

  inline const char *getName() const { return m_name.c_str(); }
  inline const char *getUser() const { return m_user.c_str(); }
  inline const EncryptedString &getPassword() const { return m_password; }
  inline const char *getNotes() const { return m_notes.c_str(); }
  inline const char *getGroup() const { return m_group.c_str(); }

  void setUUID(const unsigned char uuid[16]);
  void setUUID(const UUID &uuid);
  void setPolicy(const unsigned char policy[4]);

  void setCreationTime(time_t t);
  void setModificationTime(time_t t);
  void setAccessTime(time_t t);
  void setLifetime(time_t t);

  void setName(const string &name);
  void setUser(const string &user);
  void setPassword(const EncryptedString &password);
  void setPassword(const char *password);
  void setNotes(const string &notes);
  void setGroup(const string &group);

  void updateModTime();
  void updateAccessTime();

private:
  void init();

  UUID m_uuid;
  unsigned char m_policy[4];
  time_t m_creation_time, m_mod_time, m_access_time, m_life_time;
  string m_name, m_user, m_notes, m_group;
  EncryptedString m_password;
};

class SafeSerializer;

/** Represents a safe.
 * A safe is manages a collection of items, and it can load and save
 * itself to a file. Using a Safe is as easy as creating it and loading
 * something.
 * \code
 * Safe safe;
 * Safe::Error error = safe.load("somesafe.dat", "Password Safe");
 * if(error != Safe::Success) {
 *    ...do error stuff
 * }
 * \endcode
 */
class Safe
{
public:
  /** \example safe.cpp
   * Demonstrates how to load a safe, display the entries,
   * and saving it to another file.
   */

  typedef vector<SafeItem *> ItemList; //!< Container for the items
  typedef ItemList::iterator iterator; //!< Iterator shortcut

  Safe();
  ~Safe();

  enum Error {
    Failed, //!< General error condition
    Success, //!< Everything was a success
    BadFormat, //!< Wrong or unsupported filter
    BadFile, //!< File couldn't be opened
    IOError //!< Trouble reading the file
  };

  /** Returns a string that lists allowable safe extensions.
   */
  static string getExtensions();
  /** Returns a string that lists the types of safes.
   */
  static string getTypes();

  /** Checks the password of a file.
   * The serializer is choosen from the file's extension.
   *
   * @pre path != NULL
   * @param path File's path.
   * @param password Password to check.
   * @return Safe::Success if the password is correct, Safe::Failed if not.
   */
  static Error checkPassword(const char *path, const EncryptedString &password);
  /** Checks the password of a file.
   * But allows the type of file to be specified. If type
   * is NULL or has a length of zero, the file's
   * extension is used to pick a serializer.
   *
   * @pre Path != NULL
   * @param path File's path.
   * @param type Name of the serializer to use.
   * @param password Password to check.
   * @return Safe::Success if the password is correct, Safe::Failed if not.
   */
  static Error checkPassword(const char *path, const char *type, const EncryptedString &password);

  /** Loads a safe from disk using the specified serializer.
   * Empties the safe and reads the specified file into the safe using
   * the serializer specified.
   *
   * @param path File's path
   * @param type Name of the serializer to use
   * @param passphrase Pass-phrase that'll open the safe
   * @param def_user Default username
   * @return Safe::Success if the file is opened, otherwise
   *         the Safe::Error condition
   * @post getPath() == path
   * @post getType() == type
   * @post getPassPhrase() == passphrase
   * @post changed() == false
   */
  Error load(const char *path, const char *type, const EncryptedString &passphrase, const char *def_user);

  /** Loads a safe from disk.
   * Empties the safe and reads the specified file into the safe.
   *
   * @param path File's path
   * @param passphrase Pass-phrase that'll open the safe
   * @param def_user Default username
   * @return Safe::Success if the file is opened, otherwise
   *         the Safe::Error condition
   * @post getPath() == path
   * @post getType() == name of serializer used
   * @post getPassPhrase() == passphrase
   * @post changed() == false
   */
  Error load(const char *path, const EncryptedString &passphrase, const char *def_user);

  /** Saves the safe to a file.
   * The safe is saved to the specified file with the status of the
   * save returned.
   *
   * @param path File's path
   * @param type Name of SafeSerializer to use
   * @param passphrase Pass-phrase to the safe
   * @param def_user Default username
   * @return Safe::Success if the file is saved, otherwise the Safe::Error
   *         condition.
   * @post getPath() == path
   * @post getType() == type
   * @post getPassPhrase() == passphrase
   * @post changed() == false
   */
  Error save(const char *path, const char *type,
	    const EncryptedString &passphrase, const char *def_user);

  /** Saves the safe to a file.
   * The safe is saved to the specified file with the status of the
   * save returned.
   *
   * @param path File's path
   * @param passphrase Pass-phrase to the safe
   * @param def_user Default username
   * @return Safe::Success if the file is saved, otherwise the Safe::Error
   *         condition.
   * @post getPath() == path
   * @post getPassPhrase() == passphrase
   * @post changed() == false
   */
  Error save(const char *path, const EncryptedString &passphrase, const char *def_user);
  /** Saves the safe to a file.
   * The safe is saved to it's previous location using the same
   * pass-pharse and serializer.
   *
   * @param def_user Default username
   * @return Safe::Success if the file is saved, otherwise the Safe::Error
   *         condition.
   * @post changed() == false
   */
  Error save(const char *def_user);
    
  inline const char *getPath() const { return m_path.c_str(); }
  void setPath(const string &path);
  inline const char *getType() const { return m_type.c_str(); }

  inline const EncryptedString getPassPhrase() const { return m_passphrase.get(); }
  void setPassPhrase(const EncryptedString &phrase);

  inline bool changed() { return m_changed; }
  void setChanged(bool value);

  //SafeItem *findItem(QListViewItem *item);

  /** Adds an item to the safe.
   * Adds a new item to the safe, returning a pointer to the item that is
   * in the safe.
   *
   * @param item Item to be added
   * @return Pointer to the item that exists in the safe
   */
  SafeItem *addItem(SafeItem &item);

  /** Deletes an item from the safe.
   *
   * @param item Pointer to the item in the safe
   * @return true if the item is found and deleted, otherwise false
   */
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

  /** Empties the safe.
   * Removes all the items from the safe.
   *
   * @post size() == 0
   */
  void empty();

protected:
  /** Backs up a file appending '~' to the name.
   * Thus "somefile.txt" becomes "somefile.txt~".
   *
   * @param path Path to file
   * @return True if the backup was made, false if not
   */
  bool makeBackup(const char *path);

  /** Sets the safe's serializer's name.
   */
  void setType(const string &type);

  /** Creates a serializer that matches the given extension and serializer.
   * Extensions map to serializers one to many, while
   * the serializers' names are unique and are one to
   * one. The rules governing what serializer comes out
   * are pretty easy. The serializer's name always takes
   * precedence over the extension. If the name is NULL
   * or has a length of zero, the extension is used.
   * The first serializer found to match the extension
   * is returned. SafeSerializer::nextByExt returns the
   * next possible serializer if there is any.
   *
   * @param extension File's extension
   * @param serializer Name of the serializer
   * @return SafeSerializer that matches either extension or serializer.
   */
  static SafeSerializer *createSerializer(const char *extension,
					  const char *serializer);

private:
  string m_path, m_type;
  EncryptedString m_passphrase;
  ItemList m_items;
  bool m_changed;
};

#endif
