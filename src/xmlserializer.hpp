/* $Header: /home/cvsroot/MyPasswordSafe/src/xmlserializer.hpp,v 1.5 2004/11/01 21:34:58 nolan Exp $
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
