/* $Header: /home/cvsroot/MyPasswordSafe/src/encryptedstring.hpp,v 1.1.1.1 2004/05/04 22:47:07 nolan Exp $
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
#ifndef ENCRYPTEDSTRING_HPP
#define ENCRYPTEDSTRING_HPP

//#include <boost/shared_ptr.hpp>
#include "smartptr.hpp"
#include "pwsafe/BlowFish.h"
#include "securedstring.hpp"

class EncryptedString
{
public:
  EncryptedString();
  EncryptedString(SmartPtr<BlowFish> algor);
  EncryptedString(SmartPtr<BlowFish> algor,
		  const SecuredString &str);

  EncryptedString(const SecuredString &str);
  EncryptedString(const char *str);
  EncryptedString(const EncryptedString &es);
  ~EncryptedString();

  void clear();
  unsigned int length() const;
  // returns the length of the string

  //inline boost::shared_ptr<BlowFish> getAlgorithm() const { return m_algor; }
  inline SmartPtr<BlowFish> getAlgorithm() { return m_algor; }
  // returns the pointer to the BlowFish object
  //  void setAlgorithm(boost::shared_ptr<BlowFish> fish);
  void setAlgorithm(SmartPtr<BlowFish> fish);
  // sets the BlowFish object pointer and re-encrypts the data

  SecuredString get() const;
  // decrypts the string and returns it in a SecuredString
  void set(const SecuredString &str);
  void set(const char *str);
  // encrypts and sets the value of the string
  void set(const EncryptedString &str);
  // re-encrypts the data from str using this' BlowFish object
  void set(const char *data, int length, const unsigned char cbc[8]);

  bool operator==(const EncryptedString &es) const;
  inline bool operator!=(const EncryptedString &es) const
  {
    return !(*this == es);
  }

protected:
  void trashAndDelete();
  // trashes and deletes m_data

  void createAlgorithm();
  // creates a new BlowFish object that uses a random password

  void hash();
  // hashes the unencrypted data for quick, easy, and secure comparisons
  void hash(const unsigned char *in, unsigned int in_len, unsigned char out[20]);
  // hashes in and places the hash in out

private:
  unsigned char m_cbc[8];
  unsigned char m_hash[20];
  SmartPtr<BlowFish> m_algor;
  unsigned int m_length;
  unsigned char *m_data;
};

#endif
