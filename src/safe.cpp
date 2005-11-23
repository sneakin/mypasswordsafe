/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.cpp,v 1.29 2005/11/23 13:21:29 nolan Exp $
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
#include <time.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <qobject.h>
#include <qdom.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include "securedstring.hpp"
#include "safe.hpp"
#include "safeserializer.hpp"
#include "serializers.hpp"
#include "myutil.hpp"
#include "pwsafe/Util.h"

using namespace std;

/** Finds the subgroup in group by full name.
 * @param group the group to search
 * @param full_group The group's fullname that may contain escaped characters.
 * @return The found group or NULL if it wasn't found.
 */
SafeGroup *findGroup(SafeGroup *group, const QString &full_group)
{
  if(group == NULL || full_group.isEmpty())
    return NULL;

  DBGOUT("findGroup: " << full_group);

  QString group_name;
  unsigned int index = 0;
  for(; index < full_group.length(); index++) {
    if(full_group.at(index) != GroupSeperator)
      break;
  }

  // get the first section
  for(; index < full_group.length(); index++) {
    if(full_group.at(index) == GroupSeperator) {
      if(index > 0 && full_group.at(index - 1) != '\\')
	break;
    }
    group_name += full_group.at(index);
  }

  group_name = unescapeGroup(group_name);
  //DBGOUT("\tName: " << group_name);

  SafeGroup::Iterator it(group);
  while(it.current()) {
    if(it.current()->rtti() == SafeGroup::RTTI) {
      SafeGroup *temp = (SafeGroup *)it.current();
      if(temp->name() == group_name) {
	if(index == full_group.length())
	  return temp;

	QString child_group = full_group.right(full_group.length() - (index + 1));
	return findGroup(temp, child_group);
      }
#ifdef DEBUG
      else {
	if(!group_name.isEmpty())
	  DBGOUT(temp->name() << " != " << group_name);
      }
#endif
    }
    ++it;
  }

  return NULL;
}

/** Searches for a group given by its fullname, and creates it if it
 * isn't found.
 * @param safe The safe that is being searched/modified.
 * @group_name The group's fullname which may contain escaped characters.
 * @return The group. It'll be newly created if it isn't found.
 */
SafeGroup *findOrCreateGroup(Safe *safe, const QString &group_name)
{
  DBGOUT("findOrCreateGroup");

  if(safe == NULL)
    return NULL;

  // if the group's name is empty,
  if(group_name.isEmpty()) {
    DBGOUT("Group name is empty");
    return NULL;
  }
  // if name has zero seperators or
  else {
    QString group_path(group_name);

    if(group_path.at(0) == '/') {
      for(unsigned int i = 0; i < group_path.length(); i++) {
	if(group_path.at(i) != '/') {
	  group_path.remove(0, i);
	  break;
	}
      }
      DBGOUT("Group path: " << group_path);
    }

    SafeGroup *group = findGroup(safe, group_path);
    if(group != NULL) {
      DBGOUT("Found group " << group_path);
      return group;
    }

#ifdef DEBUG
    if(!group_path.isEmpty())
      DBGOUT(group_path);
#endif

    QString this_group = unescapeGroup(thisGroup(group_path));
    QString parent_group = parentGroup(group_path);
    DBGOUT("adding group: " << this_group << " to " << parent_group);

    if(parent_group.isEmpty()) {
      return new SafeGroup(safe, this_group);
    }
    else {
      DBGOUT("parent = " << parent_group);
      // search for the parent
      // if it doesn't exist add it
      SafeGroup *parent = findOrCreateGroup(safe, parent_group);
      return new SafeGroup(parent, this_group);
    }
  }

  return NULL;
}

SafeItem::SafeItem(SafeGroup *parent)
  : m_parent(NULL)
{
  Q_ASSERT(parent != this);

  if(parent != NULL) {
    parent->addItem(this);
    m_safe = parent->safe();
  }
  else {
    m_safe = (Safe *)this;
  }
}

SafeItem::~SafeItem()
{
  if(m_parent != NULL)
    m_parent->takeItem(this);
}

int SafeItem::rtti() const
{
  return -1;
}

void SafeItem::setParent(SafeGroup *parent)
{
  m_parent = parent;
}


SafeGroup::Iterator::Iterator(const SafeGroup *group)
  : m_iter(group->m_items)
{
}

SafeItem *SafeGroup::Iterator::current() const
{
  return m_iter.current();
}

SafeItem *SafeGroup::Iterator::next()
{
  return ++m_iter;
}

SafeItem *SafeGroup::Iterator::prev()
{
  return --m_iter;
}

SafeItem *SafeGroup::Iterator::toFirst()
{
  return m_iter.toFirst();
}

SafeItem *SafeGroup::Iterator::toLast()
{
  return m_iter.toLast();
}

bool SafeGroup::Iterator::atFirst() const
{
  return m_iter.atFirst();
}

bool SafeGroup::Iterator::atLast() const
{
  return m_iter.atLast();
}



const int SafeGroup::RTTI = 1;

SafeGroup::SafeGroup(SafeGroup *p, const QString &name)
  : SafeItem(p), m_name(name)
{
}

SafeGroup::~SafeGroup()
{
  empty();
}

int SafeGroup::rtti() const
{
  return RTTI;
}

void SafeGroup::setName(const QString &name)
{
  m_name = name;
}

void SafeGroup::addItem(SafeItem *item)
{
  m_items.append(item);
  item->setParent(this);
}

bool SafeGroup::takeItem(SafeItem *item)
{
  bool ret = m_items.remove(item);
  return ret;
}

int SafeGroup::count() const
{
  return m_items.count();
}

void SafeGroup::empty()
{
  // this can be done because SafeItem will
  // call SafeGroup::delItem which will remove
  // the item from m_items
  while(m_items.first()) {
    delete m_items.first();
  }
}

SafeItem *SafeGroup::at(int i)
{
  if(i < count())
    return m_items.at(i);
  else
    return NULL;
}

SafeGroup::Iterator SafeGroup::first()
{
  Iterator i(this);
  i.toFirst();
  return i;
}

SafeGroup::Iterator SafeGroup::last()
{
  Iterator i(this);
  i.toLast();
  return i;
}



/** \class SafeEntry safe.hpp
 * \brief Represents an entry in the Safe.
 * The information contained in each entry is stored in a SafeEntry.
 */

const int SafeEntry::RTTI = 2;

SafeEntry::SafeEntry(SafeGroup *parent)
  : SafeItem(parent)
{
  init();
}

SafeEntry::SafeEntry(SafeGroup *parent,
		     const QString &n, const QString &u,
		     const EncryptedString &p,
		     const QString &note)
  : SafeItem(parent), m_name(n), m_user(u),
    m_notes(note), m_password(p)
{
  init();
}

SafeEntry::SafeEntry(const SafeEntry &item)
  : SafeItem((SafeGroup *)item.parent())
{
  copy(item);
}

SafeEntry::~SafeEntry()
{
}

int SafeEntry::rtti() const
{
  return RTTI;
}

void SafeEntry::copy(const SafeEntry &item)
{
  setName(item.name());
  setUser(item.user());
  setPassword(item.password());
  setNotes(item.notes());
  setUUID(item.uuid());

  setCreationTime(item.creationTime());
  setModifiedTime(item.modifiedTime());
  setAccessTime(item.accessTime());
  setLifetime(item.lifetime());

  for(int i = 0; i < 4; i++) {
    m_policy[i] = item.m_policy[i];
  }
}

void SafeEntry::clear()
{
  m_name.truncate(0);
  m_user.truncate(0);
  m_notes.truncate(0);
  m_password.clear();

  init();
}


void SafeEntry::setUUID(const unsigned char u[16])
{
  m_uuid.fromArray(u);
}

void SafeEntry::setUUID(const UUID &u)
{
  m_uuid.copy(u);
}

void SafeEntry::setPolicy(const unsigned char p[4])
{
  memcpy(m_policy, p, 4);
}

void SafeEntry::setCreationTime(const QDateTime &t)
{
  m_creation_time = t;
}

void SafeEntry::setModifiedTime(const QDateTime &t)
{
  m_mod_time = t;
}

void SafeEntry::setAccessTime(const QDateTime &t)
{
  m_access_time = t;
}

void SafeEntry::setLifetime(const QTime &t)
{
  m_life_time = t;
}

void SafeEntry::setName(const QString &n)
{
  m_name = n;
  updateModTime();
}

void SafeEntry::setUser(const QString &u)
{
  m_user = u;
  updateModTime();
}

void SafeEntry::setPassword(const EncryptedString &p)
{
  m_password.set(p);
  updateModTime();
}

void SafeEntry::setPassword(const char *p)
{
  m_password.set(p);
  updateModTime();
}

void SafeEntry::setNotes(const QString &n)
{
  m_notes = n;
  updateModTime();
}

void SafeEntry::updateModTime()
{
  m_mod_time = QDateTime::currentDateTime();
}

void SafeEntry::updateAccessTime()
{
  m_access_time = QDateTime::currentDateTime();
}

void SafeEntry::init()
{
  m_uuid.make();
  memset(m_policy, 0, 4);
  m_creation_time = m_mod_time = m_access_time = QDateTime::currentDateTime();
  m_life_time = QTime(0, 0); // FIXME: get this from config file
}



/** \class Safe safe.hpp
 * \brief Represents a safe.
 *
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

/* \example safe.cpp
 * Demonstrates how to load a safe, display the entries,
 * and saving it to another file.
 */

Safe::Safe()
  : SafeGroup(NULL), m_passphrase(NULL), m_changed(false)
{
}

Safe::~Safe()
{
}

/** Returns a string that lists allowable safe extensions.
 */
QString Safe::getExtensions()
{
  return SafeSerializer::getExtensions();
}

/** Returns a string that lists the types of safes.
 */
QString Safe::getTypes()
{
  return SafeSerializer::getTypes();
}

/** Checks the password of a file.
 * The serializer is choosen from the file's extension.
 *
 * @pre path != NULL
 * @param path File's path.
 * @param password Password to check.
 * @return Safe::Success if the password is correct, Safe::Failed if not.
 */
Safe::Error Safe::checkPassword(const QString &path, const EncryptedString &password)
{
  return checkPassword(path, NULL, password);
}

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
Safe::Error Safe::checkPassword(const QString &path, const QString &type, const EncryptedString &password)
{
  QFileInfo info(path);
  if(!info.exists())
    return BadFile;

  QString ext(info.extension(false));
  SafeSerializer *serializer(createSerializer(ext, type));

  DBGOUT("Path: " << path);

  if(serializer != NULL) {
    Safe::Error ret = Failed;
    do {
      DBGOUT("Serializer: " << serializer->name());

      // NOTE: password is decrypted
      QString p(path);
      ret = serializer->checkPassword(p, password.get());
      if(ret == BadFormat) {
	DBGOUT("Bad format, getting next");
	serializer = SafeSerializer::getNextExt(serializer);
	if(serializer == NULL) {
	  DBGOUT("No more serializers");
	  return BadFormat;
	}
      }
    } while(ret == BadFormat);

    return ret;
  }
  else {
    DBGOUT("No serializer found for: " << ext);
    return BadFormat;
  }
}


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
Safe::Error Safe::load(const QString &path, const QString &type,
		       const EncryptedString &passphrase, const QString &def_user)
{
  Q_ASSERT(!path.isEmpty());

  QFileInfo info(path);
  if(!info.exists())
    return BadFile;

  QString ext(info.extension(false));
  SafeSerializer *serializer(createSerializer(ext, type));

  if(serializer) {
    Error err = Failed;

    do {
      empty();

      DBGOUT("Using " << serializer->name() << " to serialize");

      try {
	err = serializer->load(*this, path,
			       passphrase, def_user);
	DBGOUT("serializer->load: " << err);
      }
      catch(UUID::Exception e) {
	DBGOUT("UUID exception caught: " << UUID::exceptionToString(e));
	return UUIDError;
      }

      if(err == Success) {
	DBGOUT("Success");
	setPath(path);
	setType(serializer->name());
	setPassPhrase(passphrase);
	setChanged(false);
	break;
      }
      else if(err == BadFormat) {
	DBGOUT("BadFormat");
	serializer = SafeSerializer::getNextExt(serializer);
	if(serializer == NULL) {
	  DBGOUT("No more serializers");
	  return BadFormat;
	}
      }
    } while(err == BadFormat);// && by_ext == true);

    DBGOUT("Returning err");
    return err;
  }
  else {
    return BadFormat;
  }
}

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
Safe::Error Safe::load(const QString &path, const EncryptedString &passphrase, const QString &def_user)
{
  Q_ASSERT(!path.isEmpty());
  return load(path, NULL, passphrase, def_user);
}

/** Saves the safe to a file.
 * The safe is saved to the specified file with the status of the
 * save returned.
 *
 * @param path File's path
 * @param type Name of SafeSerializer to use
 * @param def_user Default username
 * @return Safe::Success if the file is saved, otherwise the Safe::Error
 *         condition.
 * @post getPath() == path
 * @post getType() == type
 * @post getPassPhrase() == passphrase
 * @post changed() == false
 */
Safe::Error Safe::save(const QString &path, const QString &type,
		       const QString &def_user)
{
  Q_ASSERT(!path.isEmpty());

  QFileInfo info(path);
  QString ext(info.extension(false));
  SafeSerializer *serializer(createSerializer(ext, type));

  if(serializer) {
    QString full_path(path);

    if(ext.length() == 0) {
      full_path += ".";
      full_path += serializer->extension();
    }

    DBGOUT("Path: " << full_path);
    DBGOUT("Serializer: " << serializer->name());

    setPath(full_path);
    setType(serializer->name());

    // FIXME: make this an option
    makeBackup(full_path);

    Safe::Error error = serializer->save(*this, full_path, def_user);
    if(error == Safe::Success) {
      setChanged(false);
    }
    return error;
  }

  return BadFormat;
}

/** Saves the safe to a file.
 * The safe is saved to the specified file with the status of the
 * save returned.
 *
 * @param path File's path
 * @param def_user Default username
 * @return Safe::Success if the file is saved, otherwise the Safe::Error
 *         condition.
 * @post getPath() == path
 * @post getPassPhrase() == passphrase
 * @post changed() == false
 */
Safe::Error Safe::save(const QString &path, const QString &def_user)
{
  return save(path, m_type, def_user);
}

/** Saves the safe to a file.
 * The safe is saved to it's previous location using the same
 * pass-pharse and serializer.
 *
 * @param def_user Default username
 * @return Safe::Success if the file is saved, otherwise the Safe::Error
 *         condition.
 * @post changed() == false
 */
Safe::Error Safe::save(const QString &def_user)
{
  if(!m_path.isEmpty() && m_passphrase.length() > 0) {
    return save(m_path, m_type, def_user);
  }
  return Failed;
}

void Safe::setPath(const QString &path)
{
  m_path = path;
}

/** Sets the safe's serializer's name.
 */
void Safe::setType(const QString &type)
{
  // NOTE: we don't check type because only Safe::save calls it, and
  // it used type to create a serializer so it's good
  m_type = type;
}


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
SafeSerializer *Safe::createSerializer(const QString &extension,
				       const QString &serializer)
{
  if(serializer.isEmpty()) {
    if(extension.isEmpty())
      return NULL;

    return SafeSerializer::createByExt(extension);
  }
  else {
    return SafeSerializer::createByName(serializer);
  }
}


void Safe::setPassPhrase(const EncryptedString &phrase)
{
  m_passphrase.set(phrase);
  setChanged(true);
}

void Safe::setChanged(bool value)
{
  m_changed = value;
  changed();
}

int Safe::totalNumItems(const SafeGroup *group, int type) const
{
  // c++ didn't like this at toplevel
  if(group == NULL) {
    group = this;
  }

  SafeGroup::Iterator it(group);
  int count = 0;

  while(it.current()) {
    SafeItem *item = *it;
    if(item->rtti() == type) {
      count++;
    }

    if(item->rtti() == SafeGroup::RTTI) {
      count += totalNumItems(dynamic_cast<const SafeGroup *>(item), type);
    }

    ++it;
  }

  return count;
}

int Safe::totalNumEntries(const SafeGroup *group) const
{
  return totalNumItems(group, SafeEntry::RTTI);
}

int Safe::totalNumGroups(const SafeGroup *group) const
{
  return totalNumItems(group, SafeGroup::RTTI);
}

/** Returns a string that describes an error.
 * @param e Safe::Error that needs to be translated.
 * @return Descriptive string
 */
const char *Safe::errorToString(Safe::Error e)
{
  static const char *errors[] = { QT_TR_NOOP("Failed to open safe"),
				  QT_TR_NOOP("Safe successfully opened"),
				  QT_TR_NOOP("Wrong filter used or the file is bad"),
				  QT_TR_NOOP("Trouble reading the file"),
				  QT_TR_NOOP("Unable to generate a UUID")
  };
  return errors[e];
}

/** Backs up a file appending '~' to the name.
 * Thus "somefile.txt" becomes "somefile.txt~".
 *
 * @param path Path to file
 * @return True if the backup was made, false if not
 */
bool Safe::makeBackup(const QString &path)
{
  if(path.isEmpty())
    return false;

  FILE *in = fopen(path, "rb");
  if(in != NULL) {
    QString new_path(path);
    new_path += "~";
    FILE *out = fopen(new_path, "wb");
    char buffer[1024];
    int num_read = 0;
    do {
      num_read = fread(buffer, 1, 1024, in);
      if(num_read != 0)
	fwrite(buffer, 1, num_read, out);
    } while(num_read > 0);
    fclose(out);
    fclose(in);
    return true;
  }
  return false;
}
