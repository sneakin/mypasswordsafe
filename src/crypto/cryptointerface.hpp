#ifndef CRYPTOINTERFACE_HPP
#define CRYPTOINTERFACE_HPP

class CryptoInterface
{
public:
	static const unsigned int BlockLength = 8;
	typedef unsigned char block[BlockLength];

	virtual void encrypt(const block in, block out) = 0;
	virtual void decrypt(const block in, block out) = 0;
};

#endif
