/* $Header: /home/cvsroot/MyPasswordSafe/src/serializers.hpp,v 1.2 2004/05/04 22:48:44 nolan Exp $
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
#ifndef _SERIALIZERS_HPP_
#define _SERIALIZERS_HPP_

#include "safe.hpp"
#include "pwsafe/Util.h"

/* Instances of these classes are in safe.cpp after the SerializerMap.
 */

class PlainTextLizer: public SafeSerializer
{
public:
  PlainTextLizer();
  virtual ~PlainTextLizer();

  virtual Safe::Error checkPassword(const string &path, const SecuredString &password);
  virtual Safe::Error load(Safe &safe, const string &path, const EncryptedString &passphrase, const string &def_user);
  virtual Safe::Error save(Safe &safe, const string &path, const string &def_user);
};

class BlowfishLizer: public SafeSerializer
{
public:
  BlowfishLizer(const char *ext = "dat",
		const char *description = "Password Safe (*.dat)");
  virtual ~BlowfishLizer();

  virtual Safe::Error checkPassword(const string &path, const SecuredString &password);
  virtual Safe::Error load(Safe &safe, const string &path, const EncryptedString &passphrase, const string &def_user);
  virtual Safe::Error save(Safe &safe, const string &path, const string &def_user);

protected:
  virtual int readHeader(FILE *in, unsigned char randstuff[8],
		 unsigned char randhash[20],
		 unsigned char salt[SaltLength],
		 unsigned char ipthing[8]);
  virtual int readEntry(FILE *in, SafeItem &item, BlowFish *fish,
		unsigned char *ipthing, const string &def_user);

  virtual int writeHeader(FILE *out, unsigned char randstuff[8],
			  unsigned char randhash[20],
			  unsigned char salt[SaltLength],
			  unsigned char ipthing[8]);
  virtual int writeEntry(FILE *out, SafeItem &item, BlowFish *fish,
			 unsigned char *ipthing, const string &def_user,
			 bool v2_hdr = false);

  int writeCBC(FILE *fp, BlowFish *fish,
	       SecuredString &data, int type,
	       unsigned char *ipthing);
  // writes the data to FP
  // cbcbuffer is the initial cbc used to xor the data,
  // and is set to the last encrypted block before returning
  // Returns the number of bytes written
  int readCBC(FILE *fp, BlowFish *fish,
	      SecuredString &data,
	      int &type, unsigned char *ipthing);
  // reads a string from FP
  // cbcbuffer is the initial cbc needed to decrypt the first
  // block and is changed to the cbc needed to decrypt the
  // next entry in the safe
  // Returns the number of bytes read
};

class BlowfishLizer2: public BlowfishLizer
{
public:
  BlowfishLizer2();
  virtual ~BlowfishLizer2();

  virtual Safe::Error load(Safe &safe, const string &path, const EncryptedString &passphrase, const string &def_user);
  virtual Safe::Error save(Safe &safe, const string &path, const string &def_user);

  // field types, per formatV2.txt
  enum BlockType {
    NAME=0, UUID=0x1, GROUP = 0x2,
    TITLE = 0x3, USER = 0x4, NOTES = 0x5,
    PASSWORD = 0x6, CTIME = 0x7, MTIME = 0x8,
    ATIME = 0x9, LTIME = 0xa, POLICY = 0xb,
    END = 0xff
  };

protected:
  static const char *FormatName, *FormatVersion;

  string parseGroup(const string &group);
  string readyGroup(const string &group);

  virtual int readEntry(FILE *in, SafeItem &item,
			BlowFish *fish,
			unsigned char *ipthing,
			const string &def_user);
  virtual int writeEntry(FILE *out, SafeItem &item, BlowFish *fish,
			 unsigned char *ipthing, const string &def_user);
};

#endif
