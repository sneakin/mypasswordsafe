/* $Header: /home/cvsroot/MyPasswordSafe/src/crypto/bfproxy.hpp,v 1.2 2004/12/06 13:07:04 nolan Exp $
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
#ifndef BFPROXY_HPP
#define BFPROXY_HPP

#include "blowfish.h"
#include "cryptointerface.hpp"

/** Proxies Blowfish so it matches BlowFish's interface
 */
class BFProxy: public CryptoInterface
{
public:
	/** Constructs a BFProxy.
	 * @param key The key to use for encryption/decryption
	 * @param len The length of the key.
	 */
	BFProxy(const unsigned char *, int);
	virtual ~BFProxy();

	void setKey(const unsigned char *, int);

	/** Encrypts a block.
	 * @param in The input block.
	 * @param out The output.
	 */
	void encrypt(const block, block);

	/** Decrypts a block.
	 * @param in The input block.
	 * @param out The output.
	 */
	void decrypt(const block, block);

private:
	void swap(const block, block);
	Blowfish m_fish;
};

#endif
