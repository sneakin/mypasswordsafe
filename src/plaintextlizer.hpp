/* $Header: /home/cvsroot/MyPasswordSafe/src/plaintextlizer.hpp,v 1.2 2004/11/01 21:34:58 nolan Exp $
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
#ifndef PLAINTEXTLIZER_HPP
#define PLAINTEXTLIZER_HPP

#include "safe.hpp"
#include "safeserializer.hpp"
#include "pwsafe/Util.h"

class QTextOStream;
class QString;

class PlainTextLizer: public SafeSerializer
{
public:
  PlainTextLizer();
  virtual ~PlainTextLizer();

  virtual Safe::Error checkPassword(const QString &path, const SecuredString &password);
  virtual Safe::Error load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &def_user);
  virtual Safe::Error save(Safe &safe, const QString &path, const QString &def_user);

protected:
  Safe::Error saveGroup(QTextStream &file, const SafeGroup *group, const QString &group_name = QString::null);
  Safe::Error saveEntry(QTextStream &file, const SafeEntry *entry, const QString &group_name);
  void saveText(QTextStream &file, const QString &text);
  void saveDate(QTextStream &file, const QDateTime &date);
  QString field(QStringList &list, unsigned int field);
};

#endif
