/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003 by Michael Buesch                                  *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include <string>

#include "endian.h"

using std::string;

typedef uint8_t		byte;

#define SHA1_HASH_LEN_BIT	160
#define SHA1_HASH_LEN_BYTE	(SHA1_HASH_LEN_BIT / 8)

/** sha1 hash algorithm.
  * Derived from libgcrypt-1.1.12
  */
class Sha1
{
	struct SHA1_CONTEXT
	{
		uint32_t  h0,h1,h2,h3,h4;
		uint32_t  nblocks;
		byte buf[64];
		int  count;
	};

public:
	Sha1() { sha1_init(); }
	static bool selfTest();

	void sha1_write(const byte *inbuf, uint32_t inlen);
	string sha1_read();
	void sha1_read(byte out[20]);

protected:
	void sha1_init();
	void sha1_final();
	void burn_stack (int bytes);
	void transform(const byte *data);

	/** Rotate a 32 bit integer by n bytes */
	uint32_t rol(uint32_t x, int n)
	{
#if defined(__GNUC__) && defined(__i386__)
		__asm__("roll %%cl,%0"
			:"=r" (x)
			:"0" (x),"c" (n));
		return x;
#else
		return ((x) << (n)) | ((x) >> (32-(n)));
#endif
	}

protected:
	struct SHA1_CONTEXT ctx;
};

#endif
