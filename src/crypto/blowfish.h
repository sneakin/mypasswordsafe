/***************************************************************************
 *                                                                         *
 *   copyright (C) 2003, 2004 by Michael Buesch                            *
 *   email: mbuesch@freenet.de                                             *
 *                                                                         *
 *   blowfish.c  -  Blowfish encryption                                    *
 *       Copyright (C) 1998, 2001, 2002 Free Software Foundation, Inc.     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef BLOWFISH_H
#define BLOWFISH_H

#include <stdint.h>
#include <string>

#include "myendian.h"
#include "myutil.hpp"

using std::string;

#define BLOWFISH_BLOCKSIZE	8
#define BLOWFISH_ROUNDS		16
#define CIPHER_ALGO_BLOWFISH	4	/* blowfish 128 bit key */

typedef uint8_t		byte;

/** blowfish encryption algorithm.
  * Derived from libgcrypt-1.1.12
  */
class Blowfish
{
	struct BLOWFISH_context
	{
		uint32_t s0[256];
		uint32_t s1[256];
		uint32_t s2[256];
		uint32_t s3[256];
		uint32_t p[BLOWFISH_ROUNDS+2];
	};

public:
	Blowfish() {}
	static bool selfTest();

	/** set key to encrypt. if return == 1, it is a weak key. */
	int bf_setkey(const byte *key, unsigned int keylen );
	/** encrypt inbuf and return it in outbuf.
	  * inbuf and outbuf have to be: buf % 8 == 0
	  * You may check this with getPaddedLen() and pad with NULL.
	  */
	int bf_encrypt( byte *outbuf, byte *inbuf, unsigned int inbuf_len );
	/** decrypt inbuf and return it in outbuf.
	  * inbuf and outbuf have to be: buf % 8 == 0
	  * You may check this with getPaddedLen() and pad with NULL.
	  */
	int bf_decrypt( byte *outbuf, byte *inbuf, unsigned int inbuf_len );
	/** returns the length, the sting has to be padded to */
	static unsigned int getPaddedLen(unsigned int inLen)
			{ return ((8 - (inLen % 8)) + inLen); }
	/** pad up to 8 bytes. */
	static void padNull(string *buf);
	/** remove padded data */
	static bool unpadNull(string *buf);

protected:
#if BLOWFISH_ROUNDS != 16
	uint32_t function_F( uint32_t x)
	{
		uint16_t a, b, c, d;
#if BYTE_ORDER == BIG_ENDIAN
		a = ((byte *) & x)[0];
		b = ((byte *) & x)[1];
		c = ((byte *) & x)[2];
		d = ((byte *) & x)[3];
#else
		a = ((byte *) & x)[3];
		b = ((byte *) & x)[2];
		c = ((byte *) & x)[1];
		d = ((byte *) & x)[0];
#endif
		return ((bc.s0[a] + bc.s1[b]) ^ bc.s2[c]) + bc.s3[d];
	}
#endif
	void R(uint32_t &l, uint32_t &r, uint32_t i, uint32_t *p,
	       uint32_t *s0, uint32_t *s1, uint32_t *s2, uint32_t *s3)
	{
		l ^= p[i];
#if BYTE_ORDER == BIG_ENDIAN
		r ^= (( s0[((byte*)&l)[0]] + s1[((byte*)&l)[1]])
			^ s2[((byte*)&l)[2]]) + s3[((byte*)&l)[3]];
#else
		r ^= (( s0[((byte*)&l)[3]] + s1[((byte*)&l)[2]])
			^ s2[((byte*)&l)[1]]) + s3[((byte*)&l)[0]];
#endif
	}
	void encrypt_block(byte *outbuf, byte *inbuf);
	void decrypt_block(byte *outbuf, byte *inbuf);
	void burn_stack(int bytes);
	void do_encrypt(uint32_t *ret_xl, uint32_t *ret_xr);
	void do_decrypt(uint32_t *ret_xl, uint32_t *ret_xr);
	void do_encrypt_block(byte *outbuf, byte *inbuf);
	void do_decrypt_block(byte *outbuf, byte *inbuf);
	int do_bf_setkey(const byte *key, unsigned int keylen);

protected:
	struct BLOWFISH_context bc;
};

#endif
