/* $Header: /home/cvsroot/MyPasswordSafe/src/xmlserializer.hpp,v 1.4 2004/11/01 17:44:45 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
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
 *  -  Neither the name of Semantic Gap (TM) nor the
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
#ifndef XMLSERIALIZER_HPP
#define XMLSERIALIZER_HPP

#include "safeserializer.hpp"

class Safe;
enum Safe::Error;
class EncryptedString;
class SecuredString;

class QString;
class QDomDocument;
class QDomElement;

class XmlSerializer: public SafeSerializer
{
public:
  XmlSerializer();
  virtual ~XmlSerializer();

  virtual Safe::Error checkPassword(const QString &path, const SecuredString &password);
  virtual Safe::Error load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &);
  virtual Safe::Error save(Safe &safe, const QString &path, const QString &);

  static QDomElement safeToXml(QDomDocument &doc, const Safe &safe);
  static QDomElement safeEntryToXml(QDomDocument &doc, const SafeEntry &entry);
  static QDomElement safeGroupToXml(QDomDocument &doc, const SafeGroup &group,
				    const QString &name = "group");

  static bool safeFromXml(const QDomDocument &doc, Safe &safe);
  static bool safeEntryFromXml(const QDomElement &root, SafeEntry *entry);
  static bool safeGroupFromXml(const QDomElement &elem, SafeGroup *group, const QString &tag_name = "group");

protected:
  static QDomElement fieldToXml(QDomDocument &doc, const QString &field,
			 const QString &value,
			 bool multiline = false);
  static QDomElement fieldToXml(QDomDocument &doc, const QString &field,
			const SecuredString &str);
  static QDomElement fieldToXml(QDomDocument &doc, const QString &field,
			const QDateTime &date);
  static QDomElement fieldToXml(QDomDocument &doc, const QString &field,
			const QTime &time);
  static QDomElement fieldToXml(QDomDocument &doc, const QString &field,
			 const UUID &uuid);
};

#endif /* XMLSERIALIZER_HPP */
