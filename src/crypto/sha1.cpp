/* 2003.05.02: Derived from libgcrypt-1.1.12 by Michael Buesch */

/* sha1.c - SHA1 hash function
 *	Copyright (C) 1998, 2001, 2002 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

/*  Test vectors:
 *
 *  "abc"
 *  A999 3E36 4706 816A BA3E  2571 7850 C26C 9CD0 D89D
 *
 *  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *  8498 3E44 1C3B D26E BAAE  4AA1 F951 29E5 E546 70F1
 */

#include "sha1.h"

#include <string.h>
#include <stdlib.h>

#include "myendian.h"
#include "myutil.hpp"

void Sha1::burn_stack(int bytes)
{
	char buf[128];

	memset(buf, 0, sizeof buf);
	bytes -= sizeof buf;
	if (bytes > 0)
		burn_stack(bytes);
}

void Sha1::sha1_init()
{
	ctx.h0 = 0x67452301;
	ctx.h1 = 0xefcdab89;
	ctx.h2 = 0x98badcfe;
	ctx.h3 = 0x10325476;
	ctx.h4 = 0xc3d2e1f0;
	ctx.nblocks = 0;
	ctx.count = 0;
}

/****************
 * Transform the message X which consists of 16 32-bit-words
 */
void Sha1::transform(const byte *data)
{
	register uint32_t a, b, c, d, e, tm;
	uint32_t x[16];

	/* get values from the chaining vars */
	a = ctx.h0;
	b = ctx.h1;
	c = ctx.h2;
	d = ctx.h3;
	e = ctx.h4;

#if BYTE_ORDER == BIG_ENDIAN
	memcpy(x, data, 64);
#else
	{
		int i;
		byte *p2;
		for (i = 0, p2 = (byte *) x; i < 16; i++, p2 += 4) {
			p2[3] = *data++;
			p2[2] = *data++;
			p2[1] = *data++;
			p2[0] = *data++;
		}
	}
#endif

#define K1  0x5A827999L
#define K2  0x6ED9EBA1L
#define K3  0x8F1BBCDCL
#define K4  0xCA62C1D6L
#define F1(x,y,z)   ( z ^ ( x & ( y ^ z ) ) )
#define F2(x,y,z)   ( x ^ y ^ z )
#define F3(x,y,z)   ( ( x & y ) | ( z & ( x | y ) ) )
#define F4(x,y,z)   ( x ^ y ^ z )

#define M(i) ( tm =   x[i&0x0f] ^ x[(i-14)&0x0f] \
		    ^ x[(i-8)&0x0f] ^ x[(i-3)&0x0f] \
	       , (x[i&0x0f] = rol(tm, 1)) )

#define R(a,b,c,d,e,f,k,m)  do { e += rol( a, 5 )     \
				      + f( b, c, d )  \
				      + k	      \
				      + m;	      \
				 b = rol( b, 30 );    \
			       } while(0)
	R(a, b, c, d, e, F1, K1, x[0]);
	R(e, a, b, c, d, F1, K1, x[1]);
	R(d, e, a, b, c, F1, K1, x[2]);
	R(c, d, e, a, b, F1, K1, x[3]);
	R(b, c, d, e, a, F1, K1, x[4]);
	R(a, b, c, d, e, F1, K1, x[5]);
	R(e, a, b, c, d, F1, K1, x[6]);
	R(d, e, a, b, c, F1, K1, x[7]);
	R(c, d, e, a, b, F1, K1, x[8]);
	R(b, c, d, e, a, F1, K1, x[9]);
	R(a, b, c, d, e, F1, K1, x[10]);
	R(e, a, b, c, d, F1, K1, x[11]);
	R(d, e, a, b, c, F1, K1, x[12]);
	R(c, d, e, a, b, F1, K1, x[13]);
	R(b, c, d, e, a, F1, K1, x[14]);
	R(a, b, c, d, e, F1, K1, x[15]);
	R(e, a, b, c, d, F1, K1, M(16));
	R(d, e, a, b, c, F1, K1, M(17));
	R(c, d, e, a, b, F1, K1, M(18));
	R(b, c, d, e, a, F1, K1, M(19));
	R(a, b, c, d, e, F2, K2, M(20));
	R(e, a, b, c, d, F2, K2, M(21));
	R(d, e, a, b, c, F2, K2, M(22));
	R(c, d, e, a, b, F2, K2, M(23));
	R(b, c, d, e, a, F2, K2, M(24));
	R(a, b, c, d, e, F2, K2, M(25));
	R(e, a, b, c, d, F2, K2, M(26));
	R(d, e, a, b, c, F2, K2, M(27));
	R(c, d, e, a, b, F2, K2, M(28));
	R(b, c, d, e, a, F2, K2, M(29));
	R(a, b, c, d, e, F2, K2, M(30));
	R(e, a, b, c, d, F2, K2, M(31));
	R(d, e, a, b, c, F2, K2, M(32));
	R(c, d, e, a, b, F2, K2, M(33));
	R(b, c, d, e, a, F2, K2, M(34));
	R(a, b, c, d, e, F2, K2, M(35));
	R(e, a, b, c, d, F2, K2, M(36));
	R(d, e, a, b, c, F2, K2, M(37));
	R(c, d, e, a, b, F2, K2, M(38));
	R(b, c, d, e, a, F2, K2, M(39));
	R(a, b, c, d, e, F3, K3, M(40));
	R(e, a, b, c, d, F3, K3, M(41));
	R(d, e, a, b, c, F3, K3, M(42));
	R(c, d, e, a, b, F3, K3, M(43));
	R(b, c, d, e, a, F3, K3, M(44));
	R(a, b, c, d, e, F3, K3, M(45));
	R(e, a, b, c, d, F3, K3, M(46));
	R(d, e, a, b, c, F3, K3, M(47));
	R(c, d, e, a, b, F3, K3, M(48));
	R(b, c, d, e, a, F3, K3, M(49));
	R(a, b, c, d, e, F3, K3, M(50));
	R(e, a, b, c, d, F3, K3, M(51));
	R(d, e, a, b, c, F3, K3, M(52));
	R(c, d, e, a, b, F3, K3, M(53));
	R(b, c, d, e, a, F3, K3, M(54));
	R(a, b, c, d, e, F3, K3, M(55));
	R(e, a, b, c, d, F3, K3, M(56));
	R(d, e, a, b, c, F3, K3, M(57));
	R(c, d, e, a, b, F3, K3, M(58));
	R(b, c, d, e, a, F3, K3, M(59));
	R(a, b, c, d, e, F4, K4, M(60));
	R(e, a, b, c, d, F4, K4, M(61));
	R(d, e, a, b, c, F4, K4, M(62));
	R(c, d, e, a, b, F4, K4, M(63));
	R(b, c, d, e, a, F4, K4, M(64));
	R(a, b, c, d, e, F4, K4, M(65));
	R(e, a, b, c, d, F4, K4, M(66));
	R(d, e, a, b, c, F4, K4, M(67));
	R(c, d, e, a, b, F4, K4, M(68));
	R(b, c, d, e, a, F4, K4, M(69));
	R(a, b, c, d, e, F4, K4, M(70));
	R(e, a, b, c, d, F4, K4, M(71));
	R(d, e, a, b, c, F4, K4, M(72));
	R(c, d, e, a, b, F4, K4, M(73));
	R(b, c, d, e, a, F4, K4, M(74));
	R(a, b, c, d, e, F4, K4, M(75));
	R(e, a, b, c, d, F4, K4, M(76));
	R(d, e, a, b, c, F4, K4, M(77));
	R(c, d, e, a, b, F4, K4, M(78));
	R(b, c, d, e, a, F4, K4, M(79));

	/* update chainig vars */
	ctx.h0 += a;
	ctx.h1 += b;
	ctx.h2 += c;
	ctx.h3 += d;
	ctx.h4 += e;
#undef K1
#undef K2
#undef K3
#undef K4
#undef F1
#undef F2
#undef F3
#undef F4
#undef M
#undef R
}

/* Update the message digest with the contents
 * of INBUF with length INLEN.
 */
void Sha1::sha1_write(const byte * inbuf, uint32_t inlen)
{
	if (ctx.count == 64) {	/* flush the buffer */
		transform(ctx.buf);
		burn_stack(88 + 4 * sizeof(void *));
		ctx.count = 0;
		ctx.nblocks++;
	}
	if (!inbuf)
		return;
	if (ctx.count) {
		for (; inlen && ctx.count < 64; inlen--)
			ctx.buf[ctx.count++] = *inbuf++;
		sha1_write(NULL, 0);
		if (!inlen)
			return;
	}

	while (inlen >= 64) {
		transform(inbuf);
		ctx.count = 0;
		ctx.nblocks++;
		inlen -= 64;
		inbuf += 64;
	}
	burn_stack(88 + 4 * sizeof(void *));
	for (; inlen && ctx.count < 64; inlen--)
		ctx.buf[ctx.count++] = *inbuf++;
}

/* The routine final terminates the computation and
 * returns the digest.
 * The handle is prepared for a new cycle, but adding bytes to the
 * handle will the destroy the returned buffer.
 * Returns: 20 bytes representing the digest.
 */

void Sha1::sha1_final()
{
	uint32_t t, msb, lsb;
	byte *p;

	sha1_write(NULL, 0); /* flush */ ;

	t = ctx.nblocks;
	/* multiply by 64 to make a byte count */
	lsb = t << 6;
	msb = t >> 26;
	/* add the count */
	t = lsb;
	if ((lsb += ctx.count) < t)
		msb++;
	/* multiply by 8 to make a bit count */
	t = lsb;
	lsb <<= 3;
	msb <<= 3;
	msb |= t >> 29;

	if (ctx.count < 56) {	/* enough room */
		ctx.buf[ctx.count++] = 0x80;	/* pad */
		while (ctx.count < 56)
			ctx.buf[ctx.count++] = 0;	/* pad */
	} else {		/* need one extra block */
		ctx.buf[ctx.count++] = 0x80;	/* pad character */
		while (ctx.count < 64)
			ctx.buf[ctx.count++] = 0;
		sha1_write(NULL, 0); /* flush */ ;
		memset(ctx.buf, 0, 56);	/* fill next block with zeroes */
	}
	/* append the 64 bit count */
	ctx.buf[56] = msb >> 24;
	ctx.buf[57] = msb >> 16;
	ctx.buf[58] = msb >> 8;
	ctx.buf[59] = msb;
	ctx.buf[60] = lsb >> 24;
	ctx.buf[61] = lsb >> 16;
	ctx.buf[62] = lsb >> 8;
	ctx.buf[63] = lsb;
	transform(ctx.buf);
	burn_stack(88 + 4 * sizeof(void *));

	p = ctx.buf;
#if BYTE_ORDER == BIG_ENDIAN
#define X(a) do { *(uint32_t*)p = ctx.h##a ; p += 4; } while(0)
#else				/* little endian */
#define X(a) do { *p++ = ctx.h##a >> 24; *p++ = ctx.h##a >> 16;	 \
		      *p++ = ctx.h##a >> 8; *p++ = ctx.h##a; } while(0)
#endif
	X(0);
	X(1);
	X(2);
	X(3);
	X(4);
#undef X

}

string Sha1::sha1_read()
{
	sha1_final();
	string ret;
	ret.assign((const char*)ctx.buf, SHA1_HASH_LEN_BYTE);
	sha1_init();
	return ret;
}

void Sha1::sha1_read(byte out[20])
{
	memcpy(out, (const unsigned char *)sha1_read().c_str(), 20);
}

bool Sha1::selfTest()
{
	const char test1[] = { 'a', 'b', 'c' };
	const uint32_t test1_len = array_size(test1);
	const char test1_md[] = { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
				  0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D };
	const char test2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	const uint32_t test2_len = array_size(test2) - 1;
	const char test2_md[] = { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
				  0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 };
	const uint32_t test3_len = 640;
	const char test3_single[] = { '0', '1', '2', '3', '4', '5', '6', '7' };
	const uint32_t test3_single_len = array_size(test3_single);
	char test3[test3_len];
	uint32_t i;
	for (i = 0; i < test3_len / test3_single_len; ++i)
		memcpy(test3 + (i * test3_single_len), test3_single, test3_single_len);
	const char test3_md[] = { 0xDE, 0xA3, 0x56, 0xA2, 0xCD, 0xDD, 0x90, 0xC7, 0xA7, 0xEC,
				  0xED, 0xC5, 0xEB, 0xB5, 0x63, 0x93, 0x4F, 0x46, 0x04, 0x52 };
	Sha1 sha1;
	sha1.sha1_write(reinterpret_cast<const byte *>(test1), test1_len);
	if (unlikely(memcmp(sha1.sha1_read().c_str(), test1_md, SHA1_HASH_LEN_BYTE)))
		return false;
	sha1.sha1_write(reinterpret_cast<const byte *>(test2), test2_len);
	if (unlikely(memcmp(sha1.sha1_read().c_str(), test2_md, SHA1_HASH_LEN_BYTE)))
		return false;
	sha1.sha1_write(reinterpret_cast<const byte *>(test3), test3_len);
	if (unlikely(memcmp(sha1.sha1_read().c_str(), test3_md, SHA1_HASH_LEN_BYTE)))
		return false;
	return true;
}
