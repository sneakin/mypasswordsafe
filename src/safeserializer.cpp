/* $HEADER$
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
#include "safe.hpp"
#include "safeserializer.hpp"
#include "plaintextlizer.hpp"
#include "serializers.hpp"
#include "xmlserializer.hpp"

using namespace std;

SafeSerializer::SerializerVec SafeSerializer::m_serializers;
PlainTextLizer _plain_text_lizer;
BlowfishLizer2 _blowfish_lizer2;
BlowfishLizer _blowfish_lizer;
XmlSerializer _xml_serializer;

#if BYTE_ORDER == BIG_ENDIAN
BorkedBlowfishLizer2 _borked_blowfish_lizer2;
BorkedBlowfishLizer _borked_blowfish_lizer;
#endif

SafeSerializer::SafeSerializer(const QString &extension, const QString &name)
  : m_extension(extension), m_name(name)
{
  add(this);
}

SafeSerializer::~SafeSerializer()
{
}

SafeSerializer *SafeSerializer::createByExt(const QString &ext)
{
  if(!m_serializers.empty()) {
    for(SerializerVec::iterator i = m_serializers.begin();
	i != m_serializers.end();
	i++) {
      SafeSerializer *serializer(*i);
      if(serializer->extension() == ext) {
	return serializer;
      }
    }
  }
  return NULL;
}

SafeSerializer *SafeSerializer::createByName(const QString &name)
{
  if(!m_serializers.empty()) {
    for(SerializerVec::iterator i = m_serializers.begin();
	i != m_serializers.end();
	i++) {
      SafeSerializer *serializer(*i);
      if(serializer->name() == name) {
	return serializer;
      }
    }
  }
  return NULL;
}

bool SafeSerializer::add(SafeSerializer *serializer)
{
  m_serializers.push_back(serializer);
  return true;
}

struct TypeConCater
{
private:
  QString result;

public:
  void setResult(const QString &s) { result = s; }
  const QString &getResult() { return result; }

  void operator () (SafeSerializer::SerializerVec::reference r)
  {
    setResult(getResult() + r->name() + "\n");
  }
};

QString SafeSerializer::getTypes()
{
  TypeConCater cater;
  return(for_each(m_serializers.begin(),
		  m_serializers.end(),
		  cater).getResult());
}

struct ExtConCater
{
private:
  QString result;

public:
  void setResult(const QString &s) { result = s; }
  const QString &getResult() { return result; }

  void operator () (SafeSerializer::SerializerVec::reference r)
  {
    setResult(getResult() + "*." + r->extension() + " ");
  }
};

QString SafeSerializer::getExtensions()
{
  ExtConCater cater;
  return(for_each(m_serializers.begin(),
		  m_serializers.end(),
		  cater).getResult());
}

const QString SafeSerializer::getExtForName(const QString &type)
{
  for(SerializerVec::iterator i = m_serializers.begin();
      i != m_serializers.end();
      i++) {
    SafeSerializer *serializer(*i);
    if(serializer->name() == QString(type)) {
      return serializer->extension();
    }
  }
  return QString();
}

SafeSerializer *SafeSerializer::getNextExt(SafeSerializer *serializer)
{
  for(SerializerVec::iterator i = m_serializers.begin();
      i != m_serializers.end();
      i++) {
    if(*i == serializer) {
      QString extension(serializer->extension());

      for(i++; i != m_serializers.end(); i++) {
	if((*i)->extension() == extension);
	  return *i;
      }
      break;
    }
  }

  return NULL;
}
