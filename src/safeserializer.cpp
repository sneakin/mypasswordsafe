/* $HEADER$
 * Copyright (c) 2004, Semantic Gap (TM) <http://www.semanticgap.com/>
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
