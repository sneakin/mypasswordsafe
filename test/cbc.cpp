#include <stdio.h>
#include <string>
#include <iostream>
#include "pwsafe/Util.h"
#include "encryptedstring.hpp"

using namespace std;

bool test1()
{
  SecuredString str("Hello world");
  EncryptedString es(str);
  cout << "es: " << es.get().get() << endl;
  if(str == es.get())
    return true;
  else
    return false;
}

bool test2()
{
  const char *strings[] = { "Nolan Eakins", "Hello world", NULL };
  string password("diddy");

  unsigned char salt[SaltLength];
  unsigned char ipthing[8];
  for(int i = 0; i < SaltLength; i++) {
    salt[i] = newrand();
  }
  for(int i = 0; i < 8; i++) {
    ipthing[i] = newrand();
  }

  boost::shared_ptr<BlowFish> fish(MakeBlowFish((const unsigned char *)password.c_str(),
				  password.length(), salt, SaltLength));
  EncryptedString es(fish);
  FILE *f = fopen("test.bin", "wb");
  if(f) {
    int numWritten = 0;
    fwrite(salt, 1, SaltLength, f);
    fwrite(ipthing, 1, 8, f);

    for(int i = 0; strings[i] != NULL; i++) {
#ifdef ENCRYPTEDSTRING_HPP
      es.set(SecuredString(strings[i]));
      cout << "es: " << es.get().get() << endl;
      numWritten += es.write(f, ipthing);
#else
      numWritten += _writecbc(f, (const unsigned char *)strings[i], strlen(strings[i]),
			      (const unsigned char *)password.c_str(), password.length(),
			      salt, SaltLength, ipthing);
#endif
    }
    fclose(f);
    cout << "Num bytes written: " << numWritten << endl;
  }

  f = fopen("test.bin", "rb");
  if(f) {
    int numRead = 0;

    fread(salt, 1, SaltLength, f);
    fread(ipthing, 1, 8, f);

#ifdef ENCRYPTEDSTRING_HPP
    for(int i = 0; strings[i] != NULL; i++) {
      numRead += es.read(f, ipthing);
      SecuredString str(es.get());
      cout << "Str: \"" << str.get() << "\"" << endl;
    }
#else
    for(int i = 0; strings[i] != NULL; i++) {
      numRead = _readcbc(f, buffer, buffer_len,
			 (const unsigned char *)password.c_str(), password.length(),
			 salt, SaltLength, ipthing);
      buffer[buffer_len] = '\0';
      unsigned char *buffer;
      unsigned int buffer_len = 0;
      cout << "Buffer: \"" << buffer << "\"" << endl;
    }
#endif
    cout << "Num bytes read: " << numRead << endl;
    fclose(f);
  }
  return true;
}

bool test3()
{
  EncryptedString one(""), two(one);
  cout << two.get().get() << endl;
  two.set(one);
  cout << two.get().get() << endl;
  one.set("world");
  two.set(one.get());
  cout << two.get().get() << endl;
  return true;
}

bool test4()
{
	EncryptedString a("hello"), b("world");
	cout << "a == b? " << (a == b) << endl;
	b.set("hello");
	cout << "a == b? " << (a == b) << endl;
	return true;
}

bool test5()
{
  // We're expecting "" to cause a seg fault. This was
  // fixed by setting EncryptedString::m_data to NULL
  // when its length was 0 in the copy constructor.
  // Around line 78 in encryptedstring.cpp
  cout << "Expecting a seg fault..." << endl;
  EncryptedString one(""), two(one);
  cout << two.get().get() << endl;
  return true;
}

int main()
{
  cout << "Test one: ";
  if(test1())
    cout << "ok";
  else
    cout << "failed";
  cout << endl;

  cout << "Test two: ";
  if(test2())
    cout << "ok";
  else
    cout << "failed";
  cout << endl;

  test3();
  test4();
  test5();
  return 0;
}
