/* $Header: /home/cvsroot/MyPasswordSafe/src/serializers.cpp,v 1.2 2004/05/04 22:48:44 nolan Exp $
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
 *
 * BlowfishLizer uses code from Password Safe, and is covered
 * by the Artistic License found in pwsafe/LICENSE
 *
 * FIXME: this file needs i8ning
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <qstring.h>
#include "safe.hpp"
#include "securedstring.hpp"
#include "encryptedstring.hpp"
#include "serializers.hpp"
#include "exception.hpp"
#include "myutil.hpp"
#include "smartptr.hpp"
#include "pwsafe/BlowFish.h"
#include "pwsafe/Util.h"

using namespace std;
//using boost::shared_ptr;

const char *BlowfishLizer2::FormatName = " !!!Version 2 File Format!!! Please upgrade to PasswordSafe 2.0 or later";
const char *BlowfishLizer2::FormatVersion = "2.0";

PlainTextLizer::PlainTextLizer()
  : SafeSerializer("txt", "Plain Text (*.txt)")
{
}

PlainTextLizer::~PlainTextLizer()
{
}

Safe::Error PlainTextLizer::checkPassword(const string &path, const SecuredString &password)
{
  ifstream in(path.c_str());
  string line;
  getline(in, line, '\n');
  in.close();
  if(line == password.get())
    return Safe::Success;
  else
    return Safe::Failed;
}

Safe::Error PlainTextLizer::load(Safe &safe, const string &path, const EncryptedString &passphrase, const string &def_user)
{
  ifstream file(path.c_str());
  if(file.is_open()) {
    string line;
    getline(file, line, '\n');

    if(EncryptedString(line) != passphrase)
      return Safe::Failed; // passphrase was invalid

    while(!file.eof()) {
      getline(file, line, '\n');
      vector<string> items = split(line, '\t');
      if(items.size() < 3) {
	break;
      }
      getline(file, line, '\n');
      QString note(line.c_str());
      note.replace("\\n", "\n");

      SafeItem safe_item(items[0], items[1],
			 SecuredString(items[2].c_str()), (const char *)note);
      if(items.size() == 4)
	safe_item.setGroup(items[3]);

      safe.addItem(safe_item);
    }

    file.close();
    return Safe::Success;
  }
  return Safe::Failed;
}

class PTLSaver
{
  ofstream &m_str;

public:
  PTLSaver(ofstream &str)
    : m_str(str)
  {
  }

  void operator() (Safe::ItemList::reference r)
  {
    SafeItem *i(r);
    // NOTE: password is decrypted
    m_str << i->getName() << '\t' << i->getUser() << '\t'
	  << i->getPassword().get().get() << '\t' << i->getGroup() << endl;

    QString notes(i->getNotes());
    notes.replace('\n', QString("\\n"));

    DBGOUT("Notes: " << notes);

    m_str << notes << endl;
  }
};

Safe::Error PlainTextLizer::save(Safe &safe, const string &path, const string &def_user)
{
  Safe::ItemList &items(safe.getItems());
  ofstream file(path.c_str());
  if(!file.is_open()) {
    return Safe::BadFile;
  }

  // NOTE: the passphrase is decrypted...AND SAVED TO DISK!!
  file << safe.getPassPhrase().get().get() << endl;
  for_each(items.begin(), items.end(), PTLSaver(file));
  file.close();
  return Safe::Success;
}


BlowfishLizer::BlowfishLizer(const char *ext, const char *description)
  : SafeSerializer(ext, description)
{
}

BlowfishLizer::~BlowfishLizer()
{
}

#if 0
int BlowfishLizer::writeCBC(FILE *fp, string &data, const SecuredString &passkey,
			    const unsigned char *salt, unsigned char *ipthing)
{
  return _writecbc(fp, (const unsigned char *)data.c_str(), data.length(),
		   (const unsigned char *)passkey.get(), passkey.length(),
		   salt, SaltLength, ipthing);
}
#endif
int BlowfishLizer::writeCBC(FILE *fp, BlowFish *fish,
			    SecuredString &data, int type,
			    unsigned char *ipthing)
{
   int numWritten = 0;

   int BlockLength = ((data.length()+7)/8)*8;
   if (BlockLength == 0)
      BlockLength = 8;

   // First encrypt and write the length of the buffer
   unsigned char lengthblock[8];
   memset(lengthblock, 0, 8);
   putInt32( lengthblock, data.length());

   lengthblock[sizeof(long)] = (unsigned char)type;

   xormem(lengthblock, ipthing, 8); // do the CBC thing
   fish->Encrypt(lengthblock, lengthblock);
   memcpy(ipthing, lengthblock, 8); // update CBC for next round

   numWritten = fwrite(lengthblock, 1, 8, fp);

   trashMemory(lengthblock, 8);

   // Now, encrypt and write the buffer
   unsigned char curblock[8];
   // NOTE: the string gets decrypted here
   // It could be possible to do the decryption and encryption
   // along side each other
   const unsigned char *buffer = (const unsigned char *)data.get();

   for (int x=0;x<BlockLength;x+=8)
   {
      if ((data.length() == 0) || ((data.length()%8 != 0) && (data.length()-x<8)))
      {
         //This is for an uneven last block
         memset(curblock, 0, 8);
         memcpy(curblock, buffer+x, data.length() % 8);
      }
      else
         memcpy(curblock, buffer+x, 8);
      xormem(curblock, ipthing, 8);
      fish->Encrypt(curblock, curblock);
      memcpy(ipthing, curblock, 8);
      numWritten += fwrite(curblock, 1, 8, fp);
   }
   trashMemory(curblock, 8);
   return numWritten;
}

// Adapted from Counterpane's PasswordSafe
#if 0
int BlowfishLizer::readCBC(FILE *fp, string &data, const SecuredString &passkey, const unsigned char *salt, unsigned char *ipthing)
{
  // We do a double cast because the LPCSTR cast operator is overridden by the CString class
  // to access the pointer we need,
  // but we in fact need it as an unsigned char. Grrrr.
  //LPCSTR passstr = LPCSTR(app.m_passkey);
  const char *passstr = passkey.get();

  unsigned char *buffer = NULL;
  unsigned int buffer_len = 0;
  int retval;

  retval = _readcbc(fp, buffer, buffer_len,
		    (const unsigned char *)passstr, passkey.length(),
		    salt, SaltLength, ipthing);
  if (buffer_len > 0) {
    //CMyString str(LPCSTR(buffer), buffer_len);
    //string str(buffer, buffer_len);
    buffer[buffer_len] = '\0';
    data = (char *)buffer;
    trashMemory(buffer, buffer_len);
    delete[] buffer;
  } else {
    data = "";
  }
  return retval;
}
#endif

int BlowfishLizer::readCBC(FILE *fp, BlowFish *fish,
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

  fish->Decrypt(lengthblock, lengthblock);
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
  unsigned char *buffer = new unsigned char[BlockLength]; // so we lie a little...

  unsigned char tempcbc[8];
  numRead += fread(buffer, 1, BlockLength, fp);
  for (int x=0;x<BlockLength;x+=8)
    {
      memcpy(tempcbc, buffer+x, 8);
      fish->Decrypt(buffer+x, buffer+x);
      xormem(buffer+x, ipthing, 8);
      memcpy(ipthing, tempcbc, 8);
    }
	
  trashMemory(tempcbc, 8);

  buffer[length] = '\0';
  data.set((const char *)buffer);

  trashMemory(buffer, BlockLength);
  delete[] buffer;

  return numRead;
}


void trimRight(string &str)
{
  string::iterator i = str.end();
  for(; i != str.begin(); i--) {
    if(!isspace(*i) && *i != '\0')
      break;
  }
  str.erase(i+1, str.end());
}

void trimLeft(string &str)
{
  string::iterator i = str.begin();
  for(; i != str.end(); i++) {
    if(!isspace(*i))
      break;
  }
  str.erase(str.begin(), i);
}

Safe::Error BlowfishLizer::checkPassword(const string &path, const SecuredString &password)
{
  FILE *in = fopen(path.c_str(), "rb");
  if(in != NULL) {
    //cout << "opened " << path << endl;
    //cout << "using " << password.get() << " as the password" << endl;
    unsigned char randstuff[StuffSize];
    unsigned char randhash[20];

    memset(randstuff, 0, StuffSize);
    fread(randstuff, 1,  8, in);
    fread(randhash,  1, 20,  in);

    fclose(in);

    unsigned char genhash[20];
    GenRandhash(password, randstuff, genhash);
    /*
      cout << "Randstuff" << endl;
      for(int i = 0; i < 8; i++) {
      printf(" %x", randstuff[i]);
      }
      cout << endl << "Genhash" << endl;
      for(int i = 0; i < 20; i++) {
      printf(" %x", genhash[i]);
      }
      cout << endl << "Randhash" << endl;
      for(int i = 0; i < 20; i++) {
      printf(" %x", randhash[i]);
      }
      cout << endl;
    */
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

int BlowfishLizer::readEntry(FILE *in, SafeItem &item,
			     BlowFish *fish,
			     unsigned char *ipthing,
			     const string &def_user)
{
  SecuredString data;
  string tempdata;
  int type;

  int numread = 0;
  //numread += es.read(in, ipthing);
  numread += readCBC(in, fish, data, type, ipthing);

  if(data.length() > 0) {
    //data.set(es.get());
    tempdata = data.get();

    vector<string> name_user(split(tempdata, '\xAD'));
    if(name_user.size() == 2) {
      trimRight(name_user[0]);
      item.setName(name_user[0]);

      trimLeft(name_user[1]);
      item.setUser(name_user[1]);
    }
    // check for the other special character
    else {
      name_user = split(tempdata, '\xA0');
      if(name_user.size() == 2) {
	trimRight(name_user[0]);
	item.setName(name_user[0]);
	item.setUser(def_user);
      }
      else {
	item.setName(tempdata);
	item.setUser("");
      }
    }
  }
  else {
    item.setName("");
    item.setUser("");
  }

  //numread += es.read(in, ipthing);
  numread += readCBC(in, fish, data, type, ipthing);
  item.setPassword(EncryptedString(fish, data));

  //numread += es.read(in, ipthing);
  numread += readCBC(in, fish, data, type, ipthing);
  if(data.length() > 0) {
    //data.set(es.get());
    tempdata = data.get();

    item.setNotes(tempdata);
  }
  else {
    item.setNotes("");
  }
  return numread;
}

Safe::Error BlowfishLizer::load(Safe &safe, const string &path, const EncryptedString &passphrase, const string &def_user)
{
  //That passkey had better be the same one that came from CheckPassword(...)

#if 0
  try {
#endif
    FILE * in = fopen(path.c_str(), "rb");

    if (in == NULL)
      return Safe::Failed;

    //ClearData(); //Before overwriting old data, but after opening the file...
    unsigned char randstuff[8];
    unsigned char randhash[20];
    unsigned char* salt = new unsigned char[SaltLength];
    unsigned char ipthing[8];
    readHeader(in, randstuff, randhash, salt, ipthing);

    //safe.setRandStuff(randstuff);
    //safe.setRandHash(randhash);

    SafeItem temp;
    //string tempdata;

    // NOTE: the passphrase is decrypted here
    /*shared_ptr<BlowFish> fish(MakeBlowFish((const unsigned char *)
      passphrase.get().get(),
      passphrase.length(),
      salt, SaltLength));
    */
    SmartPtr<BlowFish> fish(MakeBlowFish((const unsigned char *)
					 passphrase.get().get(),
					 passphrase.length(),
					 salt, SaltLength));
    int numread = 0;
    do {
      numread = readEntry(in, temp, fish, ipthing, def_user);
      if(numread > 0)
	safe.addItem(temp);
    } while(numread > 0);

    delete [] salt;
    fclose(in);

    return Safe::Success;
#if 0
  }
  catch(...) {
    // This is probably due to passphrase not being the same as the one
    // passed to checkPassword
    cout << "Exception caught" << endl;
    return false;
  }
#endif
}

Safe::Error BlowfishLizer::save(Safe &safe, const string &path, const string &def_user)
{
  FILE *out = fopen(path.c_str(), "wb");
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
    SmartPtr<BlowFish>
      fish(MakeBlowFish((unsigned char *)
			passphrase.get().get(),
			passphrase.length(),
			thesalt, SaltLength));

    for(Safe::ItemList::iterator iter = safe.getItems().begin();
	iter != safe.getItems().end();
	iter++) {
      SafeItem *item(*iter);
      if(writeEntry(out, *item, fish.get(), ipthing, def_user) == 0) {
	fclose(out);
	return Safe::IOError;
      }
    }
    fclose(out);
    delete thesalt;
    return Safe::Success;
  }
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

int BlowfishLizer::writeEntry(FILE *out, SafeItem &item, BlowFish *fish,
			      unsigned char *ipthing, const string &def_user,
			      bool v2_hdr)
{
  int num_written = 0;
  SecuredString data;
  string temp(item.getName());

  if(v2_hdr == false) {
    if(def_user == item.getUser()) {
      //temp = item.getName();
      temp += '\xA0';
    }
    else {
      //temp = item.getName();
      temp += '\xAD';
      temp += item.getUser();
    }
  }

  DBGOUT("Writing name: \"" << temp << "\"");
  
  data.set(temp);
  num_written += writeCBC(out, fish, data, 0, ipthing);
  
  data.set(item.getPassword().get()); // NOTE: decrypted password
  num_written += writeCBC(out, fish, data, 0, ipthing);
  
  temp = item.getNotes();
  data.set(item.getNotes());
  num_written += writeCBC(out, fish, data, 0, ipthing);

  return num_written;
}


BlowfishLizer2::BlowfishLizer2()
  : BlowfishLizer("dat", "Password Safe 2.0 (*.dat)")
{
}

BlowfishLizer2::~BlowfishLizer2()
{
}

extern BlowfishLizer _blowfish_lizer;

Safe::Error BlowfishLizer2::load(Safe &safe, const string &path,
			  const EncryptedString &passphrase,
			  const string &def_user)
{
  FILE * in = fopen(path.c_str(), "rb");

  if (in == NULL)
    return Safe::Failed;

  unsigned char randstuff[8];
  unsigned char randhash[20];
  unsigned char* salt = new unsigned char[SaltLength];
  unsigned char ipthing[8];
  readHeader(in, randstuff, randhash, salt, ipthing);

  SafeItem item;

  // NOTE: the passphrase is decrypted here
  SmartPtr<BlowFish> fish(MakeBlowFish((const unsigned char *)
					 passphrase.get().get(),
					 passphrase.length(),
					 salt, SaltLength));
  int numread = 0;

  // read in the format header
  numread = BlowfishLizer::readEntry(in, item, fish, ipthing, def_user);

  DBGOUT("Name: \"" << item.getName() << "\"");
  DBGOUT("pword: " << item.getPassword().get().get());

  const string pwsafe2_header(FormatName);
  if(item.getName() != pwsafe2_header) {
    DBGOUT("Not version 2, trying version 1 loader");
    return Safe::BadFormat;
  }

  do {
    item.clear();
    numread = readEntry(in, item, fish, ipthing, def_user);
    if(numread > 0)
      if(safe.addItem(item) == NULL)
	DBGOUT("addItem failed");
  } while(numread > 0);

  delete[] salt;
  fclose(in);

  return Safe::Success;
}

Safe::Error BlowfishLizer2::save(Safe &safe, const string &path, const string &def_user)
{
  FILE *out = fopen(path.c_str(), "wb");
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
    SmartPtr<BlowFish>
      fish(MakeBlowFish((unsigned char *)
			passphrase.get().get(),
			passphrase.length(),
			thesalt, SaltLength));

    // write format header
    SafeItem format_hdr(FormatName, "", FormatVersion, "");
    if(BlowfishLizer::writeEntry(out, format_hdr,
				 fish.get(), ipthing,
				 def_user, true) == 0) {
      fclose(out);
      return Safe::IOError;
    }

    for(Safe::ItemList::iterator iter = safe.getItems().begin();
	iter != safe.getItems().end();
	iter++) {
      SafeItem *item(*iter);
      if(writeEntry(out, *item, fish.get(), ipthing, def_user) == 0) {
	fclose(out);
	return Safe::IOError;
      }
    }
    fclose(out);
    delete thesalt;
    return Safe::Success;
  }
}

string BlowfishLizer2::parseGroup(const string &group)
  // This parses Password Safe's ridiculus group names.
  // '.' seperates groups, '\' is the escape sequence,
  // but only '.' and '\' need to be escaped.
{
  string ret;
  for(string::const_iterator i = group.begin();
      i != group.end();
      i++) {
    if(*i == '\\') {
      i++;
      if(i == group.end())
	break;

      if(*i == '.')
	ret.push_back(*i);
      else if(*i == '\\')
	ret += "\\\\";
    }
    else if(*i == '.')
      ret.push_back(SafeItem::GroupSeperator);
    else if(*i == SafeItem::GroupSeperator)
      ret += "\\/";
    else {
      ret.push_back(*i);
    }
  }

  return ret;
}

string BlowfishLizer2::readyGroup(const string &group)
{
  string ret;
  string::const_iterator i = group.begin();

  if(*i == SafeItem::GroupSeperator) {
    for(; i != group.end(); i++) {
      if(*i != SafeItem::GroupSeperator)
	break;
    }
  }

  for(;i != group.end(); i++) {
    if(*i == '\\') {
      i++;
      if(i == group.end())
	break;

      if(*i == SafeItem::GroupSeperator)
	ret.push_back(*i);
      else
	ret += "\\\\";
    }
    else if(*i == SafeItem::GroupSeperator)
      ret.push_back('.');
    else if(*i == '.')
      ret += "\\.";
    else {
      ret.push_back(*i);
    }
  }

  DBGOUT("\"" << group << "\" = \"" << ret << "\"");
  return ret;
}

int BlowfishLizer2::readEntry(FILE *in, SafeItem &item,
			      BlowFish *fish,
			      unsigned char *ipthing,
			      const string &def_user)
{
  SecuredString data;
  int type;
  int total_num_read = 0, num_read = 0;

  do {
    num_read += readCBC(in, fish, data, type, ipthing);
    total_num_read += num_read;

    DBGOUT("Data: " << data.get());

    if(data.length() == 0) {
      DBGOUT("data empty; type = " << type);
      continue;
    }

    switch(type) {
    case TITLE:
      item.setName(data.get());
      break;
    case PASSWORD:
      item.setPassword(data.get());
      break;
    case USER:
      item.setUser(data.get());
      break;
    case NOTES:
      item.setNotes(data.get());
      break;
    case UUID: {
      const unsigned char *uuid = (const unsigned char *)data.get();
      item.setUUID(uuid);
    } break;
    case GROUP: {
      string group(data.get());

      DBGOUT("Group: " << group << "\t" << parseGroup(group));
      item.setGroup(parseGroup(group));
      break;
    }
    case CTIME:
    case MTIME:
    case ATIME:
    case LTIME:
    case POLICY:
    default:
      DBGOUT(type << ": " << data.get());
      break;
    }
  } while(num_read > 0 && type != END);

  DBGOUT("END: " << num_read);
  return total_num_read;
}

int BlowfishLizer2::writeEntry(FILE *out, SafeItem &item, BlowFish *fish,
			       unsigned char *ipthing, const string &def_user)
{
  int num_written = 0;
  SecuredString data;
  string temp;
  int type;
  
  data.set(item.getName());
  num_written += writeCBC(out, fish, data, TITLE, ipthing);
  
  data.set(item.getUser());
  num_written += writeCBC(out, fish, data, USER, ipthing);

  data.set(item.getPassword().get()); // NOTE: decrypted password
  num_written += writeCBC(out, fish, data, PASSWORD, ipthing);

  temp = item.getGroup();
  data.set(readyGroup(temp));
  num_written += writeCBC(out, fish, data, GROUP, ipthing);
  
  data.set(item.getNotes());
  num_written += writeCBC(out, fish, data, NOTES, ipthing);

  num_written += writeCBC(out, fish, data, END, ipthing);

  return num_written;
}
