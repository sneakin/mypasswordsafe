/* $Header: /home/cvsroot/MyPasswordSafe/src/securedstring.cpp,v 1.5 2004/11/01 21:34:58 nolan Exp $
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
