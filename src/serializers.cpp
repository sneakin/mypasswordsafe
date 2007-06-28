/* $Header: /home/cvsroot/MyPasswordSafe/src/serializers.cpp,v 1.30 2006/12/17 02:55:44 nolan Exp $
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

/*
 * FIXME: this file needs i8ning
 */
#include <iostream>
#include <fstream>
#include <qstring.h>
#include <qstringlist.h>
#include <vector>
#include <algorithm>
#include <qglobal.h>
#include <qstring.h>
#include <q3cstring.h>
#include "safe.hpp"
#include "securedstring.hpp"
#include "encryptedstring.hpp"
#include "serializers.hpp"
#include "exception.hpp"
#include "myutil.hpp"
#include "smartptr.hpp"
#include "pwsafe/PW_BlowFish.h"
#include "pwsafe/Util.h"
#include "uuid.hpp"
#include "crypto/sha1.h"
#include "crypto/bfproxy.hpp"

using namespace std;

const char *BlowfishLizer2::FormatName = " !!!Version 2 File Format!!! Please upgrade to PasswordSafe 2.0 or later";
const char *BlowfishLizer2::FormatVersion = "2.0";


BlowfishLizer::BlowfishLizer(const QString &ext, const QString &description)
  : SafeSerializer(ext, description)
{
}

BlowfishLizer::~BlowfishLizer()
{
}

int BlowfishLizer::writeCBC(FILE *fp, CryptoInterface *fish,
			    const char *data, int length,
			    int type, unsigned char *ipthing)
{
   int numWritten = 0;

   int BlockLength = ((length+7)/8)*8;
   if (BlockLength == 0)
      BlockLength = 8;

   // First encrypt and write the length of the buffer
   unsigned char lengthblock[8];
   memset(lengthblock, 0, 8);
   putInt32( lengthblock, length);

   lengthblock[sizeof(int)] = (unsigned char)type;

   xormem(lengthblock, ipthing, 8); // do the CBC thing
   fish->encrypt(lengthblock, lengthblock);
   memcpy(ipthing, lengthblock, 8); // update CBC for next round

   numWritten = fwrite(lengthblock, 1, 8, fp);

   trashMemory(lengthblock, 8);

   // Now, encrypt and write the buffer
   unsigned char curblock[8];
   // NOTE: the string gets decrypted here
   // It could be possible to do the decryption and encryption
   // along side each other
   const unsigned char *buffer = (const unsigned char *)data;

   for (int x=0;x<BlockLength;x+=8)
   {
      if ((length == 0) || ((length % 8 != 0) && (length-x<8)))
      {
         //This is for an uneven last block
         memset(curblock, 0, 8);
         memcpy(curblock, buffer+x, length % 8);
      }
      else
         memcpy(curblock, buffer+x, 8);
      xormem(curblock, ipthing, 8);
      fish->encrypt(curblock, curblock);
      memcpy(ipthing, curblock, 8);
      numWritten += fwrite(curblock, 1, 8, fp);
   }
   trashMemory(curblock, 8);
   return numWritten;
}

int BlowfishLizer::writeCBC(FILE *fp, CryptoInterface *fish,
			    SecuredString &data, int type,
			    unsigned char *ipthing)
{
  return writeCBC(fp, fish, (const char *)data.get(), data.length(),
		  type, ipthing);
}

// Adapted from Counterpane's PasswordSafe
int BlowfishLizer::readCBC(FILE *fp, CryptoInterface *fish,
			   SecuredString &data, int &type,
			   unsigned char *ipthing)
{
  int numRead = 0;

  unsigned char lengthblock[8];
  unsigned char lcpy[8];

  int buffer_len = 0;
  numRead = fread(lengthblock, 1, sizeof lengthblock, fp);
  if (numRead != 8)
    return 0;
  memcpy(lcpy, lengthblock, 8);

  //BlowFish *Algorithm = MakeBlowFish(pass, passlen, salt, saltlen);

  fish->decrypt(lengthblock, lengthblock);
  xormem(lengthblock, ipthing, 8);
  memcpy(ipthing, lcpy, 8);

  int length = getInt32( lengthblock );
  type = (int)lengthblock[sizeof(int)];

  trashMemory(lengthblock, 8);
  trashMemory(lcpy, 8);

  if (length < 0) {
    return 0;
  }

  int BlockLength = ((length+7)/8)*8;
  // Following is meant for lengths < 8,
  // but results in a block being read even
  // if length is zero. This is wasteful,
  // but fixing it would break all existing databases.
  if (BlockLength == 0)
    BlockLength = 8;

  buffer_len = length;
  unsigned char *buffer = new unsigned char[BlockLength + 1]; // so we lie a little...and a little more

  unsigned char tempcbc[8];
  numRead += fread(buffer, 1, BlockLength, fp);
  for (int x=0;x<BlockLength;x+=8) {
    memcpy(tempcbc, buffer+x, 8);
    fish->decrypt(buffer+x, buffer+x);
    xormem(buffer+x, ipthing, 8);
    memcpy(ipthing, tempcbc, 8);
  }
	
  trashMemory(tempcbc, 8);

  buffer[length] = '\0';
  data.set((const char *)buffer);
  delete buffer;

  return numRead;
}

CryptoInterface *BlowfishLizer::makeBlowfish(const unsigned char *pass, int passlen,
					     const unsigned char *salt, int saltlen)
{
   unsigned char passkey[20];

   Sha1 sha;
   sha.sha1_write(pass, passlen);
   sha.sha1_write(salt, saltlen);
   sha.sha1_read(passkey);

   CryptoInterface *retval;
   retval = new BFProxy(passkey, sizeof(passkey));

   trashMemory(passkey, sizeof(passkey));
   
   return retval;
}

Safe::Error BlowfishLizer::checkPassword(const QString &path, const SecuredString &password)
{
  FILE *in = fopen(path, "rb");
  if(in != NULL) {
    unsigned char randstuff[StuffSize];
    unsigned char randhash[20];

    memset(randstuff, 0, StuffSize);
    fread(randstuff, 1,  8, in);
    fread(randhash,  1, 20,  in);

    fclose(in);

    unsigned char genhash[20];
    GenRandhash(password, randstuff, genhash);

    if(memcmp(randhash, genhash, 20) == 0) {
      return Safe::Success;
    }
    else {
      return Safe::Failed;
    }
  }
  else {
    return Safe::BadFile;
  }
}

int BlowfishLizer::readHeader(FILE *in, unsigned char randstuff[8], unsigned char randhash[20],
			      unsigned char salt[SaltLength], unsigned char ipthing[8])
{
  int numRead = 0;
  numRead += fread(randstuff, 1,  8, in);
  numRead += fread(randhash,  1, 20,  in);
  numRead += fread(salt,    1, SaltLength, in);
  numRead += fread(ipthing, 1, 8, in); 
  return numRead;
}

int BlowfishLizer::readEntry(FILE *in, SafeEntry &item,
			     CryptoInterface *fish,
			     unsigned char *ipthing,
			     const QString &def_user)
{
  SecuredString data;
  Q3CString tempdata;
  int type;

  int numread = 0;
  numread += readCBC(in, fish, data, type, ipthing);

  if(data.length() > 0) {
    tempdata = data.get();

    DBGOUT("tempdata: \"" << tempdata << "\"");

    QStringList name_user(QStringList::split('\xAD', tempdata, true));
    if(name_user.size() == 2) {
      //trimRight(name_user[0]);
      tempdata = name_user[0];
      //tempdata.truncate(tempdata.length() - 1);
      DBGOUT("\"" << tempdata << "\"\t\"" << name_user[0] << "\"");
      item.setName(tempdata);

      //trimLeft(name_user[1]);
      item.setUser(name_user[1]);
    }
    // check for the other special character
    else {
      name_user = QStringList::split('\xA0', tempdata, true);
      if(name_user.size() == 2) {
	//tempdata = name_user[0].stripWhiteSpace();
	//tempdata.truncate(tempdata.length() - 1);
	//trimRight(name_user[0]);
	item.setUser(def_user);
      }
      item.setName(tempdata);
    }
    data.clear();
  }

  numread += readCBC(in, fish, data, type, ipthing);
  if(data.length()) {
    DBGOUT("Password: " << data.get());
    item.setPassword(EncryptedString(data));
    data.clear();
  }

  numread += readCBC(in, fish, data, type, ipthing);
  if(data.length() > 0) {
    tempdata = data.get();
    DBGOUT("Notes: " << tempdata);
    item.setNotes(tempdata);
  }

  return numread;
}

Safe::Error BlowfishLizer::load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &def_user)
{
  //That passkey had better be the same one that came from CheckPassword(...)
  FILE * in = fopen(path, "rb");

  if (in == NULL)
    return Safe::Failed;

  unsigned char randstuff[8];
  unsigned char randhash[20];
  unsigned char* salt = new unsigned char[SaltLength];
  unsigned char ipthing[8];
  readHeader(in, randstuff, randhash, salt, ipthing);

  // NOTE: the passphrase is decrypted here
  SmartPtr<CryptoInterface> fish(makeBlowfish((const unsigned char *)
				       passphrase.get().get(),
				       passphrase.length(),
				       salt, SaltLength));
  int numread = 0;
  do {
    SafeEntry *temp = new SafeEntry(&safe);
    numread = readEntry(in, *temp, fish, ipthing, def_user);
    if(numread == 0)
      delete temp; // trouble reading the entry, so delete the entry
  } while(numread > 0);

  delete [] salt;
  fclose(in);

  return Safe::Success;
}

Safe::Error BlowfishLizer::save(Safe &safe, const QString &path, const QString &def_user)
{
  FILE *out = fopen(path, "wb");
  const EncryptedString &passphrase(safe.getPassPhrase());

  DBGOUT("Saving " << path);

  if(out == NULL) {
    return Safe::BadFile;
  }
  else {
    DBGOUT("File Opened for writing");

    unsigned char randstuff[10];
    unsigned char randhash[20];
    for(int i = 0; i < 8; i++)
      randstuff[i] = newrand();
    randstuff[8] = randstuff[9] = 0;

    // NOTE: the passphrase is decrypted here
    SecuredString pphrase(safe.getPassPhrase().get());
    GenRandhash(pphrase.get(), randstuff, randhash);

    // DBoxMain.cpp has a comment about the randomness needing
    // to be checked here, probably a better newrand()
    unsigned char *thesalt = new unsigned char[SaltLength];
    if(thesalt == NULL) {
      fclose(out);
      return Safe::Failed;
    }

    int i;

    for(i = 0; i < SaltLength; i++) {
      thesalt[i] = newrand();
    }
    
    unsigned char ipthing[8];
    for(i = 0; i < 8; i++) {
      ipthing[i] = newrand();
    }

    if(writeHeader(out, randstuff, randhash, thesalt, ipthing) == 0) {
      fclose(out);
      return Safe::IOError;
    }

    // NOTE: the passphrase is decrypted here
    //shared_ptr<BlowFish>
    SmartPtr<CryptoInterface>
      fish(makeBlowfish((unsigned char *)
			passphrase.get().get(),
			passphrase.length(),
			thesalt, SaltLength));

    Safe::Error ret = saveGroup(out, &safe, fish.get(), ipthing, def_user);

    fclose(out);
    delete thesalt;
    return ret;
  }
}

Safe::Error BlowfishLizer::saveGroup(FILE *out, SafeGroup *group, CryptoInterface *fish, unsigned char *ipthing, const QString &def_user)
{
  SafeGroup::Iterator iter(group);
  SafeItem *item = NULL;
  
  while(iter.current()) {
    item = iter.current();
    if(item->rtti() == SafeEntry::RTTI) {
      if(writeEntry(out, *(SafeEntry *)item, fish, ipthing, def_user) == 0) {
	return Safe::IOError;
      }
    }
    else if(item->rtti() == SafeGroup::RTTI) {
      Safe::Error ret = saveGroup(out, (SafeGroup *)item, fish, ipthing, def_user);
      if(ret != Safe::Success) {
	return ret;
      }
    }
    
    ++iter;
  }

  return Safe::Success;
}

int BlowfishLizer::writeHeader(FILE *out, unsigned char randstuff[8],
			       unsigned char randhash[20],
			       unsigned char salt[SaltLength],
			       unsigned char ipthing[8])
{
  int num_written = 0;

  num_written += fwrite(randstuff, 1, 8, out);
  num_written += fwrite(randhash, 1, 20, out);
  num_written += fwrite(salt, 1, SaltLength, out);
  num_written += fwrite(ipthing, 1, 8, out);

  return num_written;
}

int BlowfishLizer::writeEntry(FILE *out, SafeEntry &item, CryptoInterface *fish,
			      unsigned char *ipthing, const QString &def_user,
			      bool v2_hdr)
{
  DBGOUT("BlowfishLizer::writeEntry");

  int num_written = 0;
  SecuredString data;
  Q3CString temp(item.name().ascii());

  if(v2_hdr == false) {
    if(def_user == item.user()) {
      temp += '\xA0';
    }
    else {
      temp += '\xAD';
      temp += item.user().ascii();
    }
  }

  DBGOUT("Writing name: \"" << temp << "\"");
  
  data.set(temp);
  num_written += writeCBC(out, fish, data, 0, ipthing);
  
  data.set(item.password().get()); // NOTE: decrypted password
  num_written += writeCBC(out, fish, data, 0, ipthing);
  
  data.set(item.notes().ascii());
  num_written += writeCBC(out, fish, data, 0, ipthing);

  return num_written;
}

BlowfishLizer2::BlowfishLizer2(const QString &ext,
			       const QString &description)
  : BlowfishLizer(ext, description)
{
}

BlowfishLizer2::~BlowfishLizer2()
{
}

Safe::Error BlowfishLizer2::load(Safe &safe, const QString &path,
			  const EncryptedString &passphrase,
			  const QString &def_user)
{
  FILE * in = fopen(path, "rb");

  if (in == NULL)
    return Safe::Failed;

  unsigned char randstuff[8];
  unsigned char randhash[20];
  unsigned char* salt = new unsigned char[SaltLength];
  unsigned char ipthing[8];
  readHeader(in, randstuff, randhash, salt, ipthing);

  // NOTE: the passphrase is decrypted here
  SmartPtr<CryptoInterface> fish(makeBlowfish((const unsigned char *)
					 passphrase.get().get(),
					 passphrase.length(),
					 salt, SaltLength));
  int numread = 0;

  SafeEntry *item = new SafeEntry(&safe);

  // read in the format header
  numread = BlowfishLizer::readEntry(in, *item, fish.get(), ipthing, def_user);

  DBGOUT("Name: \"" << item->name() << "\"");
  DBGOUT("pword: " << item->password().get().get());

  const QString pwsafe2_header(FormatName);
  if(item->name() != pwsafe2_header) {
    DBGOUT("Not version 2, trying version 1 loader");
    return Safe::BadFormat;
  }
  delete item;

  QString group;

  do {
    item = new SafeEntry(&safe);
    group = "";
    numread = readEntry(in, *item, group, fish, ipthing, def_user);
    if(numread > 0) {
      if(!group.isEmpty()) {
	SafeGroup *parent = findOrCreateGroup(&safe, group);
	safe.takeItem(item);
	parent->addItem(item);
      }
    }
    else {
      delete item;
    }
  } while(numread > 0);

  delete[] salt;
  fclose(in);

  return Safe::Success;
}

Safe::Error BlowfishLizer2::save(Safe &safe, const QString &path, const QString &def_user)
{
  FILE *out = fopen(path, "wb");
  const EncryptedString &passphrase(safe.getPassPhrase());

  DBGOUT("Saving " << path);

  if(out == NULL) {
    return Safe::BadFile;
  }
  else {
    DBGOUT("File Opened for writing");

    unsigned char randstuff[10];
    unsigned char randhash[20];
    for(int i = 0; i < 8; i++)
      randstuff[i] = newrand();
    randstuff[8] = randstuff[9] = 0;

    // NOTE: the passphrase is decrypted here
    SecuredString pphrase(safe.getPassPhrase().get());
    GenRandhash(pphrase.get(), randstuff, randhash);

    // DBoxMain.cpp has a comment about the randomness needing
    // to be checked here, probably a better newrand()
    unsigned char *thesalt = new unsigned char[SaltLength];
    if(thesalt == NULL) {
      fclose(out);
      return Safe::Failed;
    }

    int i;

    for(i = 0; i < SaltLength; i++) {
      thesalt[i] = newrand();
    }
    
    unsigned char ipthing[8];
    for(i = 0; i < 8; i++) {
      ipthing[i] = newrand();
    }

    if(writeHeader(out, randstuff, randhash, thesalt, ipthing) == 0) {
      fclose(out);
      return Safe::IOError;
    }

    // NOTE: the passphrase is decrypted here
    SmartPtr<CryptoInterface>
      fish(makeBlowfish((unsigned char *)
			passphrase.get().get(),
			passphrase.length(),
			thesalt, SaltLength));

    // write format header
    SafeEntry *format_hdr = new SafeEntry(&safe, FormatName, "", FormatVersion, "");
    if(BlowfishLizer::writeEntry(out, *format_hdr,
				 fish.get(), ipthing,
				 def_user, true) == 0) {
      delete format_hdr;
      fclose(out);
      return Safe::IOError;
    }
    delete format_hdr;

    Safe::Error ret = saveGroup(out, &safe, fish.get(), ipthing, def_user);
    fclose(out);
    delete thesalt;
    return ret;
  }
}

Safe::Error BlowfishLizer2::saveGroup(FILE *out, SafeGroup *group, CryptoInterface *fish,
				      unsigned char *ipthing, const QString &def_user)
{
  SafeGroup::Iterator iter(group);
  SafeItem *item = NULL;

  while(iter.current()) {
    item = iter.current();
    if(item->rtti() == SafeEntry::RTTI) {
      DBGOUT("Item name: " << ((SafeEntry *)item)->name());
      if(writeEntry(out, *(SafeEntry *)item, fish, ipthing, def_user) == 0) {
	return Safe::IOError;
      }
    }
    else if(item->rtti() == SafeGroup::RTTI) {
      Safe::Error ret = saveGroup(out, (SafeGroup *)item, fish, ipthing, def_user);
      if(ret != Safe::Success)
	return ret;
    }
    ++iter;
  }

  return Safe::Success;
}

QString BlowfishLizer2::parseGroup(const QString &group)
  // This parses Password Safe's ridiculus group names.
  // '.' seperates groups, '\' is the escape sequence,
  // but only '.' and '\' need to be escaped.
{
  if(group.isEmpty())
    return QString();

  QString ret;
  QChar c;

  for(unsigned int i = 0; i < group.length(); i++) {
    c = group[i];
    if(c == '\\') {
      i++;
      if(i == group.length())
	break;

      c = group[i];
      if(c == '.')
	ret += c;
      else if(c == '\\')
	ret += "\\\\";
    }
    else if(c == '.')
      ret += SafeEntry::GroupSeperator;
    else if(c == SafeEntry::GroupSeperator)
      ret += "\\/";
    else {
      ret += c;
    }
  }

  return ret;
}

QString BlowfishLizer2::readyGroup(const QString &group)
{
  if(group.isEmpty())
    return QString();

  QString ret;
  unsigned int i = 0;
  QChar c(group[i]);

  // skip the first seperator, and find the next one
  if(c == SafeEntry::GroupSeperator) {
    for(; i < group.length(); i++) {
      c = group[i];
      if(c != SafeEntry::GroupSeperator)
	break;
    }
  }

  for(;i < group.length(); i++) {
    c = group[i];
    // handle escapes
    if(c == '\\') {
      i++;
      if(i == group.length())
	break;

      c = group[i];
      if(c == SafeEntry::GroupSeperator)
	ret += c;
      else
	ret += "\\\\";
    }
    else if(c == SafeEntry::GroupSeperator)
      ret += '.';
    else if(c == '.')
      ret += "\\.";
    else {
      ret += c;
    }
  }

  DBGOUT("\"" << group << "\" = \"" << ret << "\"");
  return ret;
}

int BlowfishLizer2::readEntry(FILE *in, SafeEntry &item, QString &group,
			      CryptoInterface *fish,
			      unsigned char *ipthing,
			      const QString &)
{
  QDateTime date;
  SecuredString data;
  int type;
  int total_num_read = 0, num_read = 0;

  do {
    num_read += readCBC(in, fish, data, type, ipthing);
    total_num_read += num_read;

#ifdef DEBUG
    if(data.length()) {
      DBGOUT("Data: " << type << "\t"
	     << data.length() << " \"" << data.get() << "\"");
    }
#endif

    if(data.length() == 0) {
      DBGOUT("data empty");
      continue;
    }

    switch(type) {
    case TITLE:
      item.setName(QString::fromUtf8(data.get()));
      break;
    case PASSWORD:
      item.setPassword(data.get());
      break;
    case USER:
      item.setUser(QString::fromUtf8(data.get()));
      break;
    case NOTES:
      item.setNotes(QString::fromUtf8(data.get()));
      break;
    case UUID_BLOCK: {
      const unsigned char *uuid_array = (const unsigned char *)data.get();
      UUID uuid(uuid_array);
      DBGOUT("UUID: " << uuid.toString());
      item.setUUID(uuid);
    } break;
    case GROUP: {
      //QString g(data.get());
      group = parseGroup(QString::fromUtf8(data.get()));
      break;
    }
    case CTIME:
      DBGOUT("CTIME size: " << data.length());
      date.setTime_t(*(time_t *)data.get());
      item.setCreationTime(date);
      break;
    case MTIME:
      date.setTime_t(*(time_t *)data.get());
      item.setModifiedTime(date);
      break;
    case ATIME:
      date.setTime_t(*(time_t *)data.get());
      item.setAccessTime(date);
      break;
    case LTIME: {
      date.setTime_t(*(time_t *)data.get());
      item.setLifetime(date.time());
    } break;
    case POLICY:
      DBGOUT("Policy size: " << data.length());
      item.setPolicy((const unsigned char *)data.get());
      break;
    case END:
      break;
    default:
      DBGOUT("unhandled block: " << type);
      break;
    }
  } while(num_read > 0 && type != END);

  // no item should have a Nil UUID
  const UUID &uuid(item.uuid());
  if(uuid.isNil()) {
    DBGOUT("UUID isNil");
    item.setUUID(UUID(true));
  }
  DBGOUT("Item UUID: " << item.uuid().toString());

  DBGOUT("END: " << num_read);

  return total_num_read;
}


int BlowfishLizer2::writeString(FILE *out, CryptoInterface *fish,
			       const QString &str,
			       int type, unsigned char *ipthing)
{
  // Note: this did check to see if the string was empty, and saved
  // nothing if it was. Password Safe had a bug which made this bad,
  // especially with groups.
  // FIXME: this should use EncryptedString
  //if(!str.isEmpty()) {
    Q3CString utf(str.utf8());
    return writeCBC(out, fish, (const char *)utf,
		    utf.length(), type, ipthing);
    //}

    //return 0;
}

int BlowfishLizer2::writeTime(FILE *out, CryptoInterface *fish, time_t time,
			      int type, unsigned char *ipthing)
{
  return writeCBC(out, fish, (const char *)&time,
		  sizeof(time_t), type, ipthing);
}

int BlowfishLizer2::writeEntry(FILE *out, SafeEntry &item, CryptoInterface *fish,
			       unsigned char *ipthing, const QString &)
{
  DBGOUT("BlowfishLizer2::writeEntry");

  int num_written = 0;
  SecuredString data;

  // Write the UUID
  const UUID &uuid(item.uuid());
  unsigned char uuid_array[16];
  uuid.toArray(uuid_array);
  data.set((const char *)uuid_array, 16);
  num_written += writeCBC(out, fish, data, UUID_BLOCK, ipthing);

  // Write the textual fields
  num_written += writeString(out, fish, item.name(), TITLE, ipthing);
  num_written += writeString(out, fish, item.user(), USER, ipthing);

  QString group(readyGroup(groupName(item)));
  num_written += writeString(out, fish, group, GROUP, ipthing);
  num_written += writeString(out, fish, item.notes(), NOTES, ipthing);

  // Write the password
  data.clear();
  if(item.password().length() > 0) {
    data.set(item.password().get()); // NOTE: decrypted password
    num_written += writeCBC(out, fish, data, PASSWORD, ipthing);
  }

  // Write the times
  num_written += writeTime(out, fish, item.creationTime().toTime_t(), CTIME, ipthing);
  num_written += writeTime(out, fish, item.modifiedTime().toTime_t(), MTIME, ipthing);
  num_written += writeTime(out, fish, item.accessTime().toTime_t(), ATIME, ipthing);

  QDateTime lifetime;
  lifetime.setTime(item.lifetime());
  num_written += writeTime(out, fish, lifetime.toTime_t(), LTIME, ipthing);

  // Write the policy
  data.set((const char *)item.policy(), 4);
  num_written += writeCBC(out, fish, data, POLICY, ipthing);

  // and finally write the ending
  num_written += writeCBC(out, fish, data, END, ipthing);

  return num_written;
}

QString BlowfishLizer2::groupName(SafeEntry &entry)
{
  SafeGroup *parent = entry.parent();

  if(!parent)
    return QString::null;

  QString ret(escapeGroup(parent->name()));
  while(parent) {
    parent = parent->parent();
    if(parent)
      ret = escapeGroup(parent->name()) + "/" + ret;
  }

  return ret;
}

#if BYTE_ORDER == BIG_ENDIAN

BorkedBlowfishLizer::BorkedBlowfishLizer()
	: BlowfishLizer("dat", "Broken MyPS Safe (*.dat)")
{
}

BorkedBlowfishLizer::~BorkedBlowfishLizer()
{
}

CryptoInterface *BorkedBlowfishLizer::makeBlowfish(const unsigned char *pass, int passlen,
							const unsigned char *salt, int saltlen)
{
	return MakeBlowFish(pass, passlen, salt, saltlen);
}

BorkedBlowfishLizer2::BorkedBlowfishLizer2()
	: BlowfishLizer2("dat", "Broken MyPS Safe 2.0 (*.dat)")
{
}

BorkedBlowfishLizer2::~BorkedBlowfishLizer2()
{
}

CryptoInterface *BorkedBlowfishLizer2::makeBlowfish(const unsigned char *pass, int passlen,
							 const unsigned char *salt, int saltlen)
{
	return MakeBlowFish(pass, passlen, salt, saltlen);
}

#endif
