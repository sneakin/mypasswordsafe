/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.cpp,v 1.13 2004/06/24 07:46:21 nolan Exp $
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
#include "securedstring.hpp"
#include "safe.hpp"
#include "safeserializer.hpp"
#include "serializers.hpp"
#include "myutil.hpp"
#include "pwsafe/Util.h"

using namespace std;

SafeSerializer::SerializerVec SafeSerializer::m_serializers;
BlowfishLizer2 _blowfish_lizer2;
BlowfishLizer _blowfish_lizer;
PlainTextLizer _plain_text_lizer;

SafeItem::SafeItem()
{
  init();
}

SafeItem::SafeItem(const QString &name, const QString &user,
		   const EncryptedString &password, const QString &notes)
  : m_name(name), m_user(user), m_notes(notes), m_group(""),
    m_password(password)
{
  init();
}

SafeItem::SafeItem(const QString &name, const QString &user,
		   const EncryptedString &password, const QString &notes,
		   const QString &group)
  : m_name(name), m_user(user), m_notes(notes), 
    m_group(group), m_password(password)
{
  init();
}


void SafeItem::clear()
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


void SafeItem::setUUID(const unsigned char uuid[16])
{
  m_uuid.fromArray(uuid);
}

void SafeItem::setUUID(const UUID &uuid)
{
  m_uuid.copy(uuid);
}

void SafeItem::setPolicy(const unsigned char policy[4])
{
  memcpy(m_policy, policy, 4);
}

void SafeItem::setCreationTime(time_t t)
{
  m_creation_time = t;
}

void SafeItem::setModificationTime(time_t t)
{
  m_mod_time = t;
}

void SafeItem::setAccessTime(time_t t)
{
  m_access_time = t;
}

void SafeItem::setLifetime(time_t t)
{
  m_life_time = t;
}

void SafeItem::setName(const QString &name)
{
  m_name = name;
  updateModTime();
}

void SafeItem::setUser(const QString &user)
{
  m_user = user;
  updateModTime();
}

void SafeItem::setPassword(const EncryptedString &password)
{
  m_password.set(password);
  updateModTime();
}

void SafeItem::setPassword(const char *password)
{
  m_password.set(password);
  updateModTime();
}

void SafeItem::setNotes(const QString &notes)
{
  m_notes = notes;
  updateModTime();
}

void SafeItem::setGroup(const QString &group)
{
  m_group = group;
  updateModTime();
}

void SafeItem::updateModTime()
{
  m_mod_time = time(NULL);
}

void SafeItem::updateAccessTime()
{
  m_access_time = time(NULL);
}

void SafeItem::init()
{
  m_uuid.make();
  memset(m_policy, 0, 4);
  m_creation_time = m_mod_time = m_access_time = time(NULL);
  m_life_time = 0;
}

Safe::Safe()
  : m_passphrase(NULL), m_changed(false)
{
}

struct SafeCleaner
{
  void operator () (Safe::ItemList::reference r)
  {
    if(r != NULL) {
      delete r;
    }
  }
};

Safe::~Safe()
{
  empty();
}

QString Safe::getExtensions()
{
  return SafeSerializer::getExtensions();
}

QString Safe::getTypes()
{
  return SafeSerializer::getTypes();
}

Safe::Error Safe::checkPassword(const QString &path, const EncryptedString &password)
{
  return checkPassword(path, NULL, password);
}

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

Safe::Error Safe::load(const QString &path, const EncryptedString &passphrase, const QString &def_user)
{
  assert(!path.isEmpty());
  return load(path, NULL, passphrase, def_user);
}

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

Safe::Error Safe::save(const QString &path, const EncryptedString &passphrase, const QString &def_user)
{
  if(m_type.isEmpty())
    return save(path, NULL, passphrase, def_user);
  else
    return save(path, m_type, passphrase, def_user);
}

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

void Safe::setType(const QString &type)
{
  // NOTE: we don't check type because only Safe::save calls it, and
  // it used type to create a serializer so it's good
  m_type = type;
}


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
}

void Safe::setChanged(bool value)
{
  m_changed = value;
}

SafeItem *Safe::addItem(SafeItem &item)
{
  SafeItem *new_item = new SafeItem(item);
  if(new_item != NULL) {
    m_items.push_back(new_item);
    setChanged(true);
    return new_item;
  }

  return NULL;
}

bool Safe::delItem(SafeItem *item)
{
  for(ItemList::iterator iter = m_items.begin();
      iter != m_items.end();
      iter++) {
    if(*iter == item) {
      delete (*iter);
      m_items.erase(iter);
      setChanged(true);
      return true;
    }
  }

  return false;
}

void Safe::empty()
{
  for(ItemList::iterator i = m_items.begin();
    i != m_items.end();
    i++) {
    if(*i != NULL) {
      delete(*i);
    }
  }
  m_items.erase(m_items.begin(), m_items.end());
}

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
  SafeItem item;
  cout << item.getUUID().toString() << endl;
  item.clear();
  cout << item.getUUID().toString() << endl;
}
#endif
