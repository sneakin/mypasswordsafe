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
