/* $Header: /home/cvsroot/MyPasswordSafe/src/encryptedstring.hpp,v 1.6 2004/12/06 12:32:05 nolan Exp $
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

class SecuredString;
class BFProxy;

class EncryptedString
{
public:
  EncryptedString();
  EncryptedString(const SecuredString &str);
  EncryptedString(const char *str);
  EncryptedString(const EncryptedString &es);
  ~EncryptedString();

  void clear();
  /** Returns the length of the string.
   */
  unsigned int length() const;

  /** Decrypts the string and returns it in a SecuredString.
   */
  SecuredString get() const;

  /** Encrypts and sets the value of the string.
   */
  void set(const SecuredString &str);
  void set(const char *str);
  void set(const EncryptedString &str);

  bool operator==(const EncryptedString &es) const;
  inline bool operator!=(const EncryptedString &es) const
  {
    return !(*this == es);
  }

protected:
  /** Trashes and deletes the string.
   */
  void trashAndDelete();

  /** Hashes the unencrypted data for quick, easy, and secure comparisons.
   */
  void hash();

private:
  /** Hashes in and places the hash in out.
   */
  void hash(const unsigned char *in, unsigned int in_len, unsigned char out[20]);

  void initCBC();

  unsigned char m_cbc[8];
  unsigned char m_hash[20];
  unsigned int m_length;
  unsigned char *m_data;

  static BFProxy algor;
};

#endif
