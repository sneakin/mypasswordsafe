/* $Header: /home/cvsroot/MyPasswordSafe/src/crypto/bfproxy.cpp,v 1.2 2004/12/06 13:07:03 nolan Exp $
 * Copyright (c) 2004, Semantic Gap (TM)
 * http://www.semanticgap.com/
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
#include "bfproxy.hpp"

BFProxy::BFProxy(const unsigned char *key, int len)
{
	m_fish.bf_setkey(key, len);
}

BFProxy::~BFProxy()
{
}

void BFProxy::setKey(const unsigned char *key, int len)
{
	m_fish.bf_setkey(key, len);
}

void BFProxy::encrypt(const block in, block out)
{
	block temp;

	swap(in, out);
	m_fish.bf_encrypt((byte *)temp, (byte *)out, BlockLength);
	swap(temp, out);
}

void BFProxy::decrypt(const block in, block out)
{
	block temp;

	swap(in, out);
	m_fish.bf_decrypt((byte *)temp, (byte *)out, BlockLength);
	swap(temp, out);
}

void BFProxy::swap(const block src, block dest)
{
	// this allows inplace swapping, ie: src == dest
	block temp;
	memcpy(temp, src, BlockLength);

	dest[0] = temp[3];
	dest[1] = temp[2];
	dest[2] = temp[1];
	dest[3] = temp[0];

	dest[4] = temp[7];
	dest[5] = temp[6];
	dest[6] = temp[5];
	dest[7] = temp[4];
}
