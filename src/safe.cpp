/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.cpp,v 1.17 2004/07/26 07:11:30 nolan Exp $
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
#include <time.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <qobject.h>
#include <qdom.h>
#include <qstringlist.h>
#include "securedstring.hpp"
#include "safe.hpp"
#include "safeserializer.hpp"
#include "serializers.hpp"
#include "myutil.hpp"
#include "pwsafe/Util.h"

using namespace std;

SafeItem::SafeItem(SafeGroup *parent)
  : m_parent(parent)
{
  DBGOUT("SafeItem");

  if(m_parent != NULL) {
    m_parent->addItem(this);
    m_safe = m_parent->safe();
  }
  else {
    m_safe = (Safe *)this;
  }
}

SafeItem::~SafeItem()
{
  DBGOUT("~SafeItem");

  if(m_parent != NULL)
    m_parent->takeItem(this);
}



SafeGroup::SafeGroup(SafeGroup *safe, const QString &name)
  : SafeItem(safe), m_name(name)
{
}

SafeGroup::~SafeGroup()
{
  // this can be done because SafeItem will
  // call SafeGroup::delItem which will remove
  // the item from m_items
  while(m_items.first()) {
    delete m_items.first();
  }
}

void SafeGroup::setName(const QString &name)
{
  m_name = name;
}

void SafeGroup::addItem(SafeItem *item)
{
  m_items.append(item);
}

bool SafeGroup::takeItem(SafeItem *item)
{
  return m_items.remove(item);
}

int SafeGroup::count() const
{
  return m_items.count();
}

void SafeGroup::empty()
{
  m_items.clear();
}

bool SafeGroup::fromXml(const QDomNode &node)
{
  return false;
}

QDomNode SafeGroup::toXml(QDomDocument &doc) const
{
  QDomElement e = doc.createElement("group");
  e.setAttribute("name", name());

  ItemListIterator it(m_items);
  while(it.current()) {
    SafeItem *item = it.current();
    e.appendChild(item->toXml(doc));
    ++it;
  }

  return e;
}



/** \class SafeEntry safe.hpp
 * \brief Represents an entry in the Safe.
 * The information contained in each entry is stored in a SafeEntry.
 */

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
    m_notes(note), m_group(""), m_password(p)
{
  init();
}

SafeEntry::SafeEntry(SafeGroup *parent, const QString &n,
		     const QString &u,
		     const EncryptedString &p,
		     const QString &note, const QString &g)
  : SafeItem(parent), m_name(n), m_user(u), m_notes(note), 
    m_group(g), m_password(p)
{
  init();
}

SafeEntry::SafeEntry(const SafeEntry &item)
  : SafeItem((SafeGroup *)item.parent())
{
  copy(item);
}

void SafeEntry::copy(const SafeEntry &item)
{
  setName(item.name());
  setUser(item.user());
  setPassword(item.password());
  setNotes(item.notes());
  setGroup(item.group());
  setUUID(item.uuid());

  setCreationTime(item.creationTime());
  setModificationTime(item.modificationTime());
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
  m_group.truncate(0);
  m_password.clear();
  m_uuid.create();
  memset(m_policy, 0, 4);
  m_creation_time = m_mod_time = m_access_time = time(NULL);
  m_life_time = 0;
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

void SafeEntry::setCreationTime(time_t t)
{
  m_creation_time = t;
}

void SafeEntry::setModificationTime(time_t t)
{
  m_mod_time = t;
}

void SafeEntry::setAccessTime(time_t t)
{
  m_access_time = t;
}

void SafeEntry::setLifetime(time_t t)
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

void SafeEntry::setGroup(const QString &g)
{
  m_group = g;
  updateModTime();
}

void SafeEntry::updateModTime()
{
  m_mod_time = time(NULL);
}

void SafeEntry::updateAccessTime()
{
  m_access_time = time(NULL);
}

bool SafeEntry::fromXml(const QDomNode &node)
{
  return false;
}

QDomNode SafeEntry::toXml(QDomDocument &doc) const
{
  QDomElement e = doc.createElement("item");
  e.appendChild(fieldToXml(doc, "name", name()));
  e.appendChild(fieldToXml(doc, "user", user()));
  e.appendChild(fieldToXml(doc, "password",
			   password().get().get())); // NOTE: password decrypted
  e.appendChild(fieldToXml(doc, "notes", notes(), true));

  return e;
}

void SafeEntry::init()
{
  m_uuid.make();
  memset(m_policy, 0, 4);
  m_creation_time = m_mod_time = m_access_time = time(NULL);
  m_life_time = 0;
}

QDomElement SafeEntry::fieldToXml(QDomDocument &doc,
				  const QString &field,
				  const QString &value,
				  bool multiline) const
{
  QDomElement xml = doc.createElement(field);

  if(!multiline) {
    xml.appendChild(doc.createTextNode(value));
  }
  else {
    QStringList lines = QStringList::split("\n", value);
    QStringList::Iterator it = lines.begin();
    QStringList::Iterator end = lines.end();

    while(it != end) {
      xml.appendChild(fieldToXml(doc, "line", *it));
      it++;
    }
  }

  return xml;
}

QDomElement SafeEntry::fieldToXml(QDomDocument &doc,
				  const QString &field,
				  time_t time) const
{
  QDomElement xml = doc.createElement(field);
  xml.appendChild(doc.createTextNode("FIXME"));
  return xml;
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
  QString ext(getExtension(path));
  SafeSerializer *serializer(createSerializer(ext, type));

  DBGOUT("Path: " << path);

  /*  if(type != NULL && strlen(type) > 0) {
      serializer = SafeSerializer::createByName(type);
      }
      else {
      if(path != NULL) {
      DBGOUT("Ext: \"" << ext << "\"");
      serializer = SafeSerializer::createByExt(ext.c_str());
      }
      }*/

  if(serializer != NULL) {
    DBGOUT("Serializer: " << serializer->name());

    // NOTE: password is decrypted
    QString p(path);
    return serializer->checkPassword(p, password.get());
  }
  else {
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
  assert(!path.isEmpty());

  QString ext(getExtension(path));
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
	if(serializer == NULL)
	  return BadFormat;
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
  assert(!path.isEmpty());
  return load(path, NULL, passphrase, def_user);
}

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
Safe::Error Safe::save(const QString &path, const QString &type,
		       const EncryptedString &passphrase, const QString &def_user)
{
  assert(!path.isEmpty());

  QString ext(getExtension(path));
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
    setType(type);

    // setPassPhrase can't set it back to itself because
    // the references all go back to m_passphrase. So
    // we do a quick check to avert the problems that
    // that was causing.
    if(passphrase != getPassPhrase())
      setPassPhrase(passphrase);

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
 * @param passphrase Pass-phrase to the safe
 * @param def_user Default username
 * @return Safe::Success if the file is saved, otherwise the Safe::Error
 *         condition.
 * @post getPath() == path
 * @post getPassPhrase() == passphrase
 * @post changed() == false
 */
Safe::Error Safe::save(const QString &path, const EncryptedString &passphrase, const QString &def_user)
{
  if(m_type.isEmpty())
    return save(path, NULL, passphrase, def_user);
  else
    return save(path, m_type, passphrase, def_user);
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
    return save(m_path, m_type, m_passphrase, def_user);
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
  if(!serializer.isEmpty()) {
    return SafeSerializer::createByName(serializer);
  }
  else {
    if(!extension.isEmpty())
      return SafeSerializer::createByExt(extension);
    else
      return NULL;
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

#ifdef SAFE_TEST
int main(int argc, char *argv[])
{
  SafeEntry item;
  cout << item.getUUID().toString() << endl;
  item.clear();
  cout << item.getUUID().toString() << endl;
}
#endif
