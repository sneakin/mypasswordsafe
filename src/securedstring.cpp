/* $Header: /home/cvsroot/MyPasswordSafe/src/securedstring.cpp,v 1.3 2004/08/01 09:11:28 nolan Exp $
 * Copyright (c) 2004, Semantic Gap Solutions
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
#include <cstring>
#include "securedstring.hpp"

using namespace std;

SecuredString::SecuredString(const char *str)
  : m_str(NULL), m_len(0)
{
  set(str);
}

SecuredString::SecuredString(const char *str, int len)
  : m_str(NULL), m_len(0)
{
  set(str, len);
}

SecuredString::SecuredString(const SecuredString &str)
  : m_str(NULL), m_len(0)
{
  set(str);
}

SecuredString::SecuredString(const string &str)
  : m_str(NULL), m_len(0)
{
  set(str.c_str());
}

SecuredString::SecuredString()
  : m_str(NULL), m_len(0)
{
}

SecuredString::~SecuredString()
{
  trash();
}

void SecuredString::clear()
{
  trash();
}

void SecuredString::set(const char *str)
{
  int length = 0;
  if(str != NULL)
    length = strlen(str);
  set(str, length);
}

void SecuredString::set(const char *str, int  len)
{
  trash();
  if(str != NULL) {
    m_len = len;
    m_str = new char[m_len + 1];
    memset(m_str, 0, m_len + 1);
    strncpy(m_str, str, m_len);
  }
}

void SecuredString::set(const SecuredString &str)
{
  set(str.get());
}

void SecuredString::trash()
{
  if(m_str != NULL) {
    memset(m_str, 0, m_len + 1);
    delete[] m_str;
  }
  m_str = NULL;
  m_len = 0;
}

bool SecuredString::operator==(const SecuredString &s) const
{
  if(m_str == NULL || s.m_str == NULL) {
    if(m_str == s.m_str) // both == NULL
      return true;
    else
      return false;
  }

  if(length() == s.length()) {
    for(int i = 0; i < length(); i++) {
      if(m_str[i] != s.m_str[i])
	return false;
    }
    return true;
  }
  return false;
}

#ifdef TEST
#include <iostream>

int main(int argc, char *argv[])
{
  SecuredString s;
  s.set("Hello");
  cout << "s = " << s.get() << endl;
  SecuredString t(s);
  cout << "t = " << t.get() << endl;
  t.set("World");
  s.set(t);
  cout << "s = " << s.get() << endl;
  return 0;
}
#endif
