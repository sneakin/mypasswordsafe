/* $Header: /home/cvsroot/MyPasswordSafe/src/securedstring.hpp,v 1.4 2004/11/01 21:34:58 nolan Exp $
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
#ifndef SECUREDSTRING_HPP
#define SECUREDSTRING_HPP

#include <string>

using std::string;

class SecuredString
{
public:
  SecuredString(const char *str);
  SecuredString(const char *str, int len);
  SecuredString(const SecuredString &str);
  SecuredString(const string &str);
  SecuredString();
  ~SecuredString();

  void clear();
    
  const char *get() const { return m_str; }
  void set(const char *str);
  void set(const char *str, int len);
  void set(const SecuredString &str);
  // trashes m_str before allocating memory to copy str into
    
  int length() const { return m_len; }
  // returns the length of m_str

  bool operator==(const SecuredString &s) const;    
  inline bool operator!=(const SecuredString &s) const { return !(*this == s); }
protected:
  void trash();
  // zeroes out m_str, deletes m_str, and sets m_len to 0
    
private:
  char *m_str;
  int m_len;
};

#endif
