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
  virtual Safe::Error load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &def_user);
  virtual Safe::Error save(Safe &safe, const QString &path, const QString &def_user);

  static QDomElement safeToXml(QDomDocument &doc, const Safe &safe);
  static QDomElement safeEntryToXml(QDomDocument &doc, const SafeEntry &entry);
  static QDomElement safeGroupToXml(QDomDocument &doc, const SafeGroup &group);

  static bool safeFromXml(const QDomDocument &doc, Safe &safe);
  static bool safeEntryFromXml(const QDomElement &root, SafeEntry &entry);
  static bool safeGroupFromXml(const QDomElement &elem, SafeGroup &group);

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
