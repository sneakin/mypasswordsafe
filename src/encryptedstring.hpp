/* $Header: /home/cvsroot/MyPasswordSafe/src/encryptedstring.hpp,v 1.5 2004/11/01 21:54:34 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
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
#ifndef ENCRYPTEDSTRING_HPP
#define ENCRYPTEDSTRING_HPP

//#include <boost/shared_ptr.hpp>
#include "smartptr.hpp"
#include "pwsafe/PW_BlowFish.h"
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
