/* $Header: /home/cvsroot/MyPasswordSafe/src/serializers.hpp,v 1.11 2004/11/01 21:34:58 nolan Exp $
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
#ifndef _SERIALIZERS_HPP_
#define _SERIALIZERS_HPP_

#include "safe.hpp"
#include "safeserializer.hpp"
#include "pwsafe/Util.h"

class QString;

/* Instances of these classes are in safe.cpp after the SerializerMap.
 */

class BlowfishLizer: public SafeSerializer
{
public:
  BlowfishLizer(const QString &ext = "dat",
		const QString &description = "Password Safe (*.dat)");
  virtual ~BlowfishLizer();

  virtual Safe::Error checkPassword(const QString &path, const SecuredString &password);
  virtual Safe::Error load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &def_user);
  virtual Safe::Error save(Safe &safe, const QString &path, const QString &def_user);

protected:
  virtual int readHeader(FILE *in, unsigned char randstuff[8],
		 unsigned char randhash[20],
		 unsigned char salt[SaltLength],
		 unsigned char ipthing[8]);
  virtual int readEntry(FILE *in, SafeEntry &item, BlowFish *fish,
		unsigned char *ipthing, const QString &def_user);

  virtual int writeHeader(FILE *out, unsigned char randstuff[8],
			  unsigned char randhash[20],
			  unsigned char salt[SaltLength],
			  unsigned char ipthing[8]);
  virtual int writeEntry(FILE *out, SafeEntry &item, BlowFish *fish,
			 unsigned char *ipthing, const QString &def_user,
			 bool v2_hdr = false);
  Safe::Error saveGroup(FILE *out, SafeGroup *group, BlowFish *fish,
			unsigned char *ipthing, const QString &def_user);

  int writeCBC(FILE *fp, BlowFish *fish, const char *data,
	       int length, int type, unsigned char *ipthing);
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

  virtual Safe::Error load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &def_user);
  virtual Safe::Error save(Safe &safe, const QString &path, const QString &def_user);

  // field types, per formatV2.txt
  enum BlockType {
    NAME=0, UUID_BLOCK=0x1, GROUP = 0x2,
    TITLE = 0x3, USER = 0x4, NOTES = 0x5,
    PASSWORD = 0x6, CTIME = 0x7, MTIME = 0x8,
    ATIME = 0x9, LTIME = 0xa, POLICY = 0xb,
    END = 0xff
  };

protected:
  static const char *FormatName, *FormatVersion;

  QString parseGroup(const QString &group);
  QString readyGroup(const QString &group);

  virtual int readEntry(FILE *in, SafeEntry &item, QString &group,
			BlowFish *fish,
			unsigned char *ipthing,
			const QString &def_user);
  virtual int writeEntry(FILE *out, SafeEntry &item, BlowFish *fish,
			 unsigned char *ipthing, const QString &def_user);
  int writeString(FILE *out, BlowFish *fish, const QString &str,
		  int type, unsigned char *ipthing);
  int writeTime(FILE *out, BlowFish *fish, time_t time, int type, unsigned char *ipthing);
  Safe::Error saveGroup(FILE *out, SafeGroup *group, BlowFish *fish,
			unsigned char *ipthing, const QString &def_user);

  QString groupName(SafeEntry &entry);
};

#endif
