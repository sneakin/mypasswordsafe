/* $Header: /home/cvsroot/MyPasswordSafe/src/plaintextlizer.cpp,v 1.5 2004/11/01 21:34:58 nolan Exp $
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
#include <qstring.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include "safe.hpp"
#include "securedstring.hpp"
#include "encryptedstring.hpp"
#include "plaintextlizer.hpp"
#include "myutil.hpp"
#include "uuid.hpp"

using namespace std;

PlainTextLizer::PlainTextLizer()
  : SafeSerializer("txt", "Plain UNENCRYPTED Text (*.txt)")
{
}

PlainTextLizer::~PlainTextLizer()
{
}

/** Checks the password on a plaintext file.
 */
Safe::Error PlainTextLizer::checkPassword(const QString &path, const SecuredString &password)
{
  QFile file(path);
  if(file.open(IO_ReadOnly)) {
    QTextStream stream(&file);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    QString line;
    line = stream.readLine();
    if(line == password.get())
      return Safe::Success;
    else
      return Safe::Failed;
    file.close();
  }

  return Safe::BadFile;
}

/** Loads a plaintext file that was created by MyPasswordSafe.
 */
Safe::Error PlainTextLizer::load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &)
{
  QFile file(path);
  if(file.open(IO_ReadOnly)) {
    QTextStream stream(&file);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    QString line;

    line = stream.readLine();
    if(EncryptedString(line.utf8()) != passphrase)
      return Safe::Failed; // passphrase was invalid

    while(!stream.atEnd()) {
      line = stream.readLine();
      QStringList items = QStringList::split('\t', line, true);
      DBGOUT("items.count = " << items.count());
      if(items.count() == 9) {
	SafeGroup *group = findOrCreateGroup(&safe, field(items, 3));
	if(group == NULL)
	  group = &safe;

	SafeEntry *item = new SafeEntry(group);
	item->setName(field(items, 0));
	item->setUser(field(items, 1));
	item->setPassword(field(items, 2).utf8());
	item->setCreationTime(QDateTime::fromString(field(items, 4), Qt::ISODate));
	item->setModifiedTime(QDateTime::fromString(field(items, 5), Qt::ISODate));
	item->setAccessTime(QDateTime::fromString(field(items, 6), Qt::ISODate));
	item->setLifetime(QTime::fromString(field(items, 7), Qt::ISODate));

	UUID uuid;
	uuid.fromString(field(items, 8));
	item->setUUID(uuid);

	line = stream.readLine();
	line.replace("\\n", "\n");
	item->setNotes(line);
      }
      else {
	file.close();
	return Safe::BadFile;
      }
    }

    file.close();
    return Safe::Success;
  }

  return Safe::BadFile;
#if 0
  ifstream file(path);
  if(file.is_open()) {
    string line;
    getline(file, line, '\n');


    while(!file.eof()) {
      getline(file, line, '\n');
      //if(items.size() == 4)
      //	safe_item.setGroup(items[3]);
    }

    file.close();
    return Safe::Success;
  }
#endif
  return Safe::Failed;
}

/** Saves a safe as a plaintext tab delimeted file.
 */
Safe::Error PlainTextLizer::save(Safe &safe, const QString &path, const QString &)
{
  QFile file(path);
  if(file.open(IO_WriteOnly)) {
    QTextStream stream(&file);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    // NOTE: the passphrase is decrypted...AND SAVED TO DISK!!
    SecuredString password(safe.getPassPhrase().get());
    stream << password.get() << endl;
    Safe::Error ret = saveGroup(stream, &safe);
    file.close();
    return ret;
  }

  return Safe::BadFile;
}

/** Saves a group and all of its children to a stream.
 */
Safe::Error PlainTextLizer::saveGroup(QTextStream &file, const SafeGroup *group, const QString &group_name)
{
  Safe::Error ret = Safe::Success;
  SafeGroup::Iterator it(group);

  while(it.current()) {
    SafeItem *item = it.current();
    if(item->rtti() == SafeEntry::RTTI) {
      DBGOUT("item");
      ret = saveEntry(file, (SafeEntry *)item, group_name);
    }
    else if(item->rtti() == SafeGroup::RTTI) {
      QString next_group_name(group_name);
      next_group_name = next_group_name + "/" + escapeGroup(((SafeGroup *)item)->name());
      DBGOUT("Group: " << next_group_name);
      ret = saveGroup(file, (SafeGroup *)item, next_group_name);
    }

    if(ret != Safe::Success)
      break;

    ++it;
  }

  return ret;
}

/** Saves an individual entry to a stream.
 */
Safe::Error PlainTextLizer::saveEntry(QTextStream &file, const SafeEntry *entry, const QString &group_name)
{
  saveText(file, entry->name());
  saveText(file, entry->user());
  saveText(file, QString::fromUtf8(entry->password().get().get()));
  saveText(file, group_name);
  saveDate(file, entry->creationTime());
  saveDate(file, entry->modifiedTime());
  saveDate(file, entry->accessTime());

  file << entry->lifetime().toString(Qt::ISODate) << '\t';
  file << entry->uuid().toString();
  file << endl;
  
  QString notes(entry->notes());
  if(!notes.isEmpty()) {
    notes.replace('\n', QString("\\n"));
    DBGOUT("Notes: " << notes);
    file << notes;
  }

  file << endl;

  return Safe::Success;
}

/** Help method to save text to a stream.
 */
void PlainTextLizer::saveText(QTextStream &file, const QString &text)
{
  if(!text.isEmpty())
    file << text;
  file << '\t';
}

/** Help method to save dates to a stream.
 */
void PlainTextLizer::saveDate(QTextStream &file, const QDateTime &date)
{
  file << date.toString(Qt::ISODate);
  file << '\t';
}

/** Helper method that does some checking on retrieving a string
 * from a list.
 */
QString PlainTextLizer::field(QStringList &list, unsigned int field)
{
  if(list.count() >= field)
    return list[field];
  else
    return QString::null;
}
