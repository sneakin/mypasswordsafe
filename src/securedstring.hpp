/* $Header: /home/cvsroot/MyPasswordSafe/src/securedstring.hpp,v 1.2 2004/05/04 22:48:44 nolan Exp $
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
