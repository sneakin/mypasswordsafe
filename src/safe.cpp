/* $Header: /home/cvsroot/MyPasswordSafe/src/safe.cpp,v 1.6 2004/06/12 06:42:18 nolan Exp $
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
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
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
  : m_name(""), m_user(""), m_notes("")
{
  init();
}

SafeItem::SafeItem(const string &name, const string &user,
		   const EncryptedString &password, const string &notes)
  : m_name(name), m_user(user), m_notes(notes), m_group(""),
    m_password(password)
{
  init();
}

SafeItem::SafeItem(const string &name, const string &user,
		   const EncryptedString &password, const string &notes,
		   const string &group)
  : m_name(name), m_user(user), m_notes(notes), 
    m_group(group), m_password(password)
{
  init();
}


void SafeItem::clear()
{
  m_name.clear();
  m_user.clear();
  m_notes.clear();
  m_group.clear();
  m_password.clear();
  memset(m_uuid, 0, 16);
  memset(m_policy, 0, 4);
  m_creation_time = m_mod_time = m_access_time = time(NULL);
  m_life_time = 0;
}


void SafeItem::setUUID(const unsigned char uuid[16])
{
  memcpy(m_uuid, uuid, 16);
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

void SafeItem::setName(const string &name)
{
  m_name = name;
  updateModTime();
}

void SafeItem::setUser(const string &user)
{
  m_user = user;
  updateModTime();
}

void SafeItem::setPassword(const EncryptedString &password)
{
  //m_password.setAlgorithm(password.getAlgorithm());
  m_password.set(password);
  updateModTime();
}

void SafeItem::setPassword(const char *password)
{
  m_password.set(password);
  updateModTime();
}

void SafeItem::setNotes(const string &notes)
{
  m_notes = notes;
  updateModTime();
}

void SafeItem::setGroup(const string &group)
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
  memset(m_uuid, 0, 16);
  memset(m_policy, 0, 4);
  m_creation_time = m_mod_time = m_access_time = time(NULL);
  m_life_time = 0;
}

Safe::Safe()
  : m_path(""), m_type(""), m_passphrase(NULL),
    m_changed(false)
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

string Safe::getExtensions()
{
  return SafeSerializer::getExtensions();
}

string Safe::getTypes()
{
  return SafeSerializer::getTypes();
}

Safe::Error Safe::checkPassword(const char *path, const EncryptedString &password)
{
  return checkPassword(path, NULL, password);
}

Safe::Error Safe::checkPassword(const char *path, const char *type, const EncryptedString &password)
{
  string ext(getExtension(path));
  SafeSerializer *serializer(createSerializer(ext.c_str(), type));

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
    DBGOUT("Serializer: " << serializer->description());

    // NOTE: password is decrypted
    string p(path);
    return serializer->checkPassword(p, password.get());
  }
  else {
    return BadFormat;
  }
}


Safe::Error Safe::load(const char *path, const char *type, const EncryptedString &passphrase, const char *def_user)
{
  assert(path != NULL);

  string ext(getExtension(path));
  SafeSerializer *serializer(createSerializer(ext.c_str(), type));
  //bool by_ext = false;

/*  if(type != NULL && strlen(type) > 0) {
    serializer = SafeSerializer::createByName(type);
  }
  else {
    if(ext.length() != 0)
      serializer = SafeSerializer::createByExt(ext.c_str());
    //by_ext = true;
  }
*/
  if(serializer) {
    //Safe *safe = new Safe();
    //if(safe != NULL) {
    string def_user_str;
    if(def_user != NULL)
      def_user_str = def_user;

    Error err = Failed;

    do {
      empty();

      DBGOUT("Using " << serializer->description() << " to serialize");

      err = serializer->load(*this, string(path),
				   passphrase, def_user_str);
      DBGOUT("serializer->load: " << err);

      if(err == Success) {
	DBGOUT("Success");
	setPath(path);
	setType(serializer->description());
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

Safe::Error Safe::load(const char *path, const EncryptedString &passphrase, const char *def_user)
{
  assert(path != NULL);
  return load(path, NULL, passphrase, def_user);
}

Safe::Error Safe::save(const char *path, const char *type,
		const EncryptedString &passphrase, const char *def_user)
{
  assert(path != NULL);

  string ext(getExtension(path));
  SafeSerializer *serializer(createSerializer(ext.c_str(), type));

  /*if(type != NULL && strlen(type) > 0) {
    serializer = SafeSerializer::createByName(type);
    if(ext.length() == 0) {
      full_path += ".";
      full_path += serializer->extension();
    }
  }
  else if(ext.length() != 0) {
      serializer = SafeSerializer::createByExt(ext.c_str());
      }*/
  if(serializer) {
    string full_path(path);

    if(ext.length() == 0) {
      full_path += ".";
      full_path += serializer->extension();
    }

    DBGOUT("Path: " << full_path);
    DBGOUT("Serializer: " << serializer->description());

    setPath(full_path);
    setType(type);

    // setPassPhrase can't set it back to itself because
    // the references all go back to m_passphrase. So
    // we do a quick check to avert the problems that
    // that was causing.
    if(passphrase != getPassPhrase())
      setPassPhrase(passphrase);

    // FIXME: make this an option
    makeBackup(full_path.c_str());

    Safe::Error error = serializer->save(*this, full_path, def_user);
    if(error == Safe::Success) {
      setChanged(false);
    }
    return error;
  }

  return BadFormat;
}

Safe::Error Safe::save(const char *path, const EncryptedString &passphrase, const char *def_user)
{
  if(m_type.length() > 0)
    return save(path, m_type.c_str(), passphrase, def_user);
  else
    return save(path, NULL, passphrase, def_user);
}

Safe::Error Safe::save(const char *def_user)
{
  if(m_path.length() > 0 && m_passphrase.length() > 0) {
    return save(m_path.c_str(), m_type.c_str(), m_passphrase, def_user);
  }
  return Failed;
}

void Safe::setPath(const string &path)
{
  m_path = path;
}

void Safe::setType(const string &type)
{
  // NOTE: we don't check type because only Safe::save calls it, and
  // it used type to create a serializer so it's good
  m_type = type;
}


SafeSerializer *Safe::createSerializer(const char *extension,
				       const char *serializer)
{
  if(serializer != NULL && strlen(serializer) > 0) {
    return SafeSerializer::createByName(serializer);
  }
  else {
    if(extension != NULL && strlen(extension) > 0)
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
/*
  if(!m_items.empty())
    for_each(m_items.begin(), m_items.end(), SafeCleaner());
*/
  for(ItemList::iterator i = m_items.begin();
    i != m_items.end();
    i++) {
    if(*i != NULL) {
      // FIXME: i->first is CONST!!
      delete(*i);
    }
  }
  m_items.erase(m_items.begin(), m_items.end());
}

bool Safe::makeBackup(const char *path)
{
  if(path == NULL)
    return false;

  FILE *in = fopen(path, "rb");
  if(in != NULL) {
    string new_path(path);
    new_path += "~";
    FILE *out = fopen(new_path.c_str(), "wb");
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
