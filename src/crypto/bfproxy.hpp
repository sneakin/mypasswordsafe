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
