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
