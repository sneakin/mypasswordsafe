/* $HEADER$
 * Copyright (c) 2004, Semantic Gap <http://www.semanticgap.com/>
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
#include <string>
#include "safe.hpp"
#include "safeserializer.hpp"

using namespace std;

SafeSerializer::SafeSerializer(const char *extension, const char *description)
  : m_extension(extension), m_description(description)
{
  add(this);
}

SafeSerializer::~SafeSerializer()
{
}

SafeSerializer *SafeSerializer::createByExt(const char *ext)
{
  string extension(ext);

  if(!m_serializers.empty()) {
    for(SerializerVec::iterator i = m_serializers.begin();
	i != m_serializers.end();
	i++) {
      SafeSerializer *serializer(*i);
      if(string(serializer->extension()) == extension) {
	return serializer;
      }
    }
  }
  return NULL;
}

SafeSerializer *SafeSerializer::createByName(const char *name)
{
  string name_str(name);

  if(!m_serializers.empty()) {
    for(SerializerVec::iterator i = m_serializers.begin();
	i != m_serializers.end();
	i++) {
      SafeSerializer *serializer(*i);
      if(string(serializer->description()) == name_str) {
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
  string result;

public:
  void setResult(const string &s) { result = s; }
  const string &getResult() { return result; }

  void operator () (SafeSerializer::SerializerVec::reference r)
  {
    setResult(getResult() + r->description() + "\n");
  }
};

string SafeSerializer::getTypes()
{
  TypeConCater cater;
  return(for_each(m_serializers.begin(),
		  m_serializers.end(),
		  cater).getResult());
}

struct ExtConCater
{
private:
  string result;

public:
  void setResult(const string &s) { result = s; }
  const string &getResult() { return result; }

  void operator () (SafeSerializer::SerializerVec::reference r)
  {
    setResult(getResult() + "*." + r->extension() + " ");
  }
};

string SafeSerializer::getExtensions()
{
  ExtConCater cater;
  return(for_each(m_serializers.begin(),
		  m_serializers.end(),
		  cater).getResult());
}

const char *SafeSerializer::getExtForType(const char *type)
{
  for(SerializerVec::iterator i = m_serializers.begin();
      i != m_serializers.end();
      i++) {
    SafeSerializer *serializer(*i);
    if(string(serializer->description()) == string(type)) {
      return serializer->extension();
    }
  }
  return NULL;
}

SafeSerializer *SafeSerializer::getNextExt(SafeSerializer *serializer)
{
  for(SerializerVec::iterator i = m_serializers.begin();
      i != m_serializers.end();
      i++) {
    if(*i == serializer) {
      string extension(serializer->extension());

      for(i++; i != m_serializers.end(); i++) {
	if(string((*i)->extension()) == extension);
	  return *i;
      }
      break;
    }
  }

  return NULL;
}
