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
#include "uuid.hpp"

using std::string;

UUID::UUID(bool make_uuid)
  : m_uuid(NULL)
{
  uuid_rc_t error = uuid_create(&m_uuid);
  if(error != UUID_RC_OK)
    throw errorToException(error);

  if(make_uuid) {
    make();
  }
}

UUID::UUID(const UUID &uuid)
  : m_uuid(NULL)
{
  uuid_rc_t error = uuid_create(&m_uuid);
  if(error != UUID_RC_OK)
    throw errorToException(error);

  copy(uuid);
}

UUID::~UUID()
{
  destroy();
}

bool UUID::isNil() const
{
  int result = 0;
  uuid_rc_t error = uuid_isnil(m_uuid, &result);
  if(error != UUID_RC_OK)
    throw errorToException(error);

  return (result ? true : false);
}

bool UUID::isEqual(const UUID &uuid) const
{
  int result = 0;
  uuid_rc_t error = uuid_compare(m_uuid, uuid.m_uuid, &result);
  if(error != UUID_RC_OK)
    throw errorToException(error);

  return (result == 0 ? true : false);
}

void UUID::make()
{
  uuid_rc_t error = uuid_make(m_uuid, UUID_MAKE_V1);
  if(error != UUID_RC_OK)
    throw errorToException(error);
}

void UUID::copy(const UUID &uuid)
{
  unsigned char array[UUID_LEN_BIN];
  uuid.toArray(array);
  fromArray(array);
}

void UUID::destroy()
{
  if(m_uuid != NULL) {
    uuid_rc_t error = uuid_destroy(m_uuid);
    if(error != UUID_RC_OK)
      throw errorToException(error);
    m_uuid = NULL;
  }
}

string UUID::toString() const
{
  char *buffer = NULL;

  uuid_rc_t error = uuid_export(m_uuid, UUID_FMT_STR, (void **)&buffer, NULL);
  if(error != UUID_RC_OK) {
    delete buffer; // note: this needed?
    throw errorToException(error);
  }

  string ret(buffer);
  delete buffer;
  return ret;
}

void UUID::toArray(unsigned char array[16]) const
{
  size_t size = 16;
  uuid_rc_t error = uuid_export(m_uuid, UUID_FMT_BIN, (void **)&array, &size);
  if(error != UUID_RC_OK)
    throw errorToException(error);
}

void UUID::fromString(const string &str)
{
  uuid_rc_t error = uuid_import(m_uuid, UUID_FMT_STR, (const void *)str.c_str(), str.length());
  if(error != UUID_RC_OK)
    throw errorToException(error);
}

void UUID::fromArray(unsigned char array[16])
{
  uuid_rc_t error = uuid_import(m_uuid, UUID_FMT_BIN, (const void *)array, 16);
  if(error != UUID_RC_OK)
    throw errorToException(error);
}

UUID::Exception UUID::errorToException(uuid_rc_t error)
{
  return (Exception)error;
}

const char *UUID::exceptionToString(UUID::Exception e)
{
  return uuid_error((uuid_rc_t)e);
}

#ifdef TEST
#include <iostream>
#include <vector>
using namespace std;

void printArray(UUID &uuid)
{
  unsigned char bin_uuid[16];
  uuid.toArray(bin_uuid);
  cout << "uuid.toArray = ";
  for(int i = 0; i < 16; i++)
    printf("%x ", bin_uuid[i]);
  cout << endl;
}

void testFromArray(UUID &uuid)
{
  unsigned char array[16];
  UUID copy;

  uuid.toArray(array);
  copy.fromArray(array);
  printArray(copy);
}

void outOfMemTest()
{
  vector<UUID *> vec;
  UUID *uuid;

  cout << "Hoping to exhaust memory" << endl;
  try {
    for(int i = 0; i < 200000000; i++) {
      cout << "\rat: " << i;
      uuid = new UUID();
      vec.push_back(uuid);
    }
  }
  catch(UUID::Exception &e) {
    cout << "Exception caught: " << UUID::exceptionToString(e) << endl;
  }
  catch(...) {
    cout << "Unkown exception thrown" << endl;
  }
}

int main(int argc, char *argv[])
{
  try {
    UUID uuid(false);

    cout << "uuid.isNil() == " << uuid.isNil() << endl; // should be 1
    printArray(uuid);
    uuid.make();
    cout << "uuid.isNil() == " << uuid.isNil() << endl; // should be 0
    cout << "uuid = " << uuid.toString() << endl;
    printArray(uuid);

    UUID copy(true);
    cout << "copy = " << copy.toString() << endl;
    cout << "uuid.isEqual(copy)? " << uuid.isEqual(copy) << endl; // should be 0

    copy.fromString(uuid.toString());
    cout << "copy = " << copy.toString() << endl;

    testFromArray(uuid);

    cout << "uuid.isEqual(copy)? " << uuid.isEqual(copy) << endl; // should be 1
    cout << "uuid == copy? " << (uuid == copy) << endl; // should be 1

    UUID copy2(copy);
    cout << "copy2 = " << copy2.toString() << endl;

    cout << endl << "sizeof(uuid) == " << sizeof(uuid) << endl
	 << "sizeof(UUID) == " << sizeof(UUID) << endl;

    outOfMemTest();

    throw UUID::NotImplemented;
  }
  catch(UUID::Exception &e) {
    cout << "Exception thrown: " << e << "\t"
	 << UUID::exceptionToString(e) << endl;
    return -1;
  }

  return 0;
}
#endif
