/* $Header: /home/cvsroot/MyPasswordSafe/src/encryptedstring.cpp,v 1.6 2004/11/01 21:54:34 nolan Exp $
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
#include <time.h>
#include <iostream>
#include "pwsafe/Util.h"
#include "pwsafe/PW_BlowFish.h"
#include "encryptedstring.hpp"

using namespace std;

EncryptedString::EncryptedString()
  : m_length(0), m_data(NULL)
{
  createAlgorithm();
}

//EncryptedString::EncryptedString(boost::shared_ptr<BlowFish> algor)
//  : m_type(PASSWORD), m_algor(algor), m_length(0), m_data(NULL)
EncryptedString::EncryptedString(SmartPtr<BlowFish> algor)
  : m_algor(algor), m_length(0), m_data(NULL)
  // EncryptedString defaults to Type::PASSWORD because that's
  // what it'll primarily be used to store
{
}

//EncryptedString::EncryptedString(boost::shared_ptr<BlowFish> algor,
//				 const SecuredString &str)
EncryptedString::EncryptedString(SmartPtr<BlowFish> algor,
				 const SecuredString &str)
  : m_algor(algor), m_length(0), m_data(NULL)
{
  set(str);
}

EncryptedString::EncryptedString(const SecuredString &str)
  : m_length(0), m_data(NULL)
{
  createAlgorithm();
  set(str);
}

EncryptedString::EncryptedString(const char *str)
  : m_length(0), m_data(NULL)
{
  createAlgorithm();
  set(str);
}

EncryptedString::EncryptedString(const EncryptedString &es)
{
  m_algor = es.m_algor;
  m_length = es.length();
  memcpy(m_cbc, es.m_cbc, 8);
  memcpy(m_hash, es.m_hash, 20);

  if(m_length == 0) {
    m_data = NULL;
    return;
  }

  int BlockLength = ((m_length+7)/8)*8;
  if (BlockLength == 0)
    BlockLength = 8;
  m_data = new unsigned char[BlockLength];

  memset(m_data, 0, BlockLength);
  memcpy(m_data, es.m_data, BlockLength);
}

EncryptedString::~EncryptedString()
{
  trashAndDelete();
}

void EncryptedString::clear()
{
  trashAndDelete();
}


unsigned int EncryptedString::length() const
{
  return m_length;
}

//void EncryptedString::setAlgorithm(boost::shared_ptr<BlowFish> fish)
void EncryptedString::setAlgorithm(SmartPtr<BlowFish> fish)
{
  SecuredString s(get()); // NOTE: decrypted data
  m_algor = fish;
  set(s);
}

SecuredString EncryptedString::get() const
{
  if(m_data != NULL && m_length > 0) {
    int BlockLength = ((m_length+7)/8)*8;
    // Following is meant for lengths < 8,
    // but results in a block being read even
    // if length is zero. This is wasteful,
    // but fixing it would break all existing databases.
    if (BlockLength == 0)
      BlockLength = 8;

    unsigned char cbcbuffer[8];
    memcpy(cbcbuffer, m_cbc, 8);

    unsigned char *buffer = new unsigned char[BlockLength]; // so we lie a little...

    unsigned char tempcbc[8];
    for (int x=0;x<BlockLength;x+=8)
      {
	memcpy(tempcbc, m_data+x, 8);
	m_algor->Decrypt(m_data+x, buffer+x);
	xormem(buffer+x, cbcbuffer, 8);
	memcpy(cbcbuffer, tempcbc, 8);
      }

    buffer[m_length] = '\0';
    SecuredString s((char *)buffer);
    delete[] buffer;
    return s;
  }
  else {
    return SecuredString();
  }
}

/*void EncryptedString::makeLengthBlock(unsigned char len_block[],
				      unsigned char cbc_buffer[])
{

}*/

void EncryptedString::set(const SecuredString &str)
{
  set(str.get());
}

void EncryptedString::set(const EncryptedString &es)
{
  set(es.get());
}

void EncryptedString::set(const char *buffer)
{
  // delete the old string
  trashAndDelete();

  if(buffer == NULL)
    return;

  int length = strlen(buffer);
  /*if(length == 0) {
    return;
    }*/

  int BlockLength = ((length+7)/8)*8;
  if (BlockLength == 0)
    BlockLength = 8;

   // First encrypt and get the length of the buffer to get its
  // cbc value
   //unsigned char lengthblock[8];
   //memset(lengthblock, 0, 8);
   //putInt32( lengthblock, length );

   //xormem(lengthblock, m_cbc, 8); // do the CBC thing
   //m_algor->Encrypt(lengthblock, lengthblock);
   //memcpy(cbcbuffer, lengthblock, 8); // update CBC for next round
   //memcpy(m_cbc, cbcbuffer, 8);

   // Now, encrypt and write the buffer
   unsigned char cbcbuffer[8];
   memcpy(cbcbuffer, m_cbc, 8);

   unsigned char curblock[8];
   //SecuredString data(get());
   //const unsigned char *buffer = (const unsigned char *)str.get();

   m_data = new unsigned char[BlockLength];
   memset(m_data, 0, BlockLength);

   for (int x=0;x<BlockLength;x+=8)
   {
      if ((length == 0) || ((length%8 != 0) && (length-x<8)))
      {
         //This is for an uneven last block
         memset(curblock, 0, 8);
         memcpy(curblock, buffer+x, length % 8);
      }
      else
         memcpy(curblock, buffer+x, 8);
      xormem(curblock, cbcbuffer, 8);
      m_algor->Encrypt(curblock, curblock);
      memcpy(cbcbuffer, curblock, 8);
      // do the final copy
      memcpy(m_data+x, curblock, 8);
   }
   m_length = length;
   trashMemory(curblock, 8);
   hash();
}

bool EncryptedString::operator==(const EncryptedString &es) const
{
	if(es.length() == 0 || length() == 0) {
		if(es.length() == length())
			return true;
		else
			return false;
	}
	else {
		if(memcmp(m_hash, es.m_hash, 20) == 0)
			return true;
		else
			return false;
	}
}

void EncryptedString::trashAndDelete()
{
  if(m_data != NULL) {
    trashMemory(m_data, m_length);
    delete[] m_data;
    m_data = NULL;
    m_length = 0;
  }
}

void EncryptedString::createAlgorithm()
{
  string pword(GetAlphaNumPassword(8));
  //m_algor.reset(new BlowFish((unsigned char *)pword.c_str(),
  //			 pword.length()));
  DBGOUT("Creating fish: " << pword);
  BlowFish *fish = new BlowFish((unsigned char *)pword.c_str(),
				pword.length());
  DBGOUT("Fish created: " << fish);
  m_algor.set(fish);


  time_t cur_time;
  for(int i = 0; i < 8; i++) {
    if(time(&cur_time) != (time_t)-1) {
    	m_cbc[i] = rand();
	continue;
    }
    m_cbc[i] = (int)cur_time ^ rand();// % 255;
  }
}

void EncryptedString::hash()
{
  // NOTE: string gets decrypted here
  SecuredString temp(get());
  hash((const unsigned char *)temp.get(), m_length, m_hash);
}

void EncryptedString::hash(const unsigned char *in,
			   unsigned int in_len,
			   unsigned char out[20])
{
  SHA1_CTX hasher;
  SHA1Init(&hasher);
  for(int i = 0; i < 4; i++)
    SHA1Update(&hasher, in, in_len);
  SHA1Final(out, &hasher);
}
