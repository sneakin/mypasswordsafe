#include <string.h>
#include "bfproxy.hpp"
#include "PW_BlowFish.h"
#include "bfproxytest.hpp"

void BFProxyTest::setUp()
{
	unsigned char key[] = "hello world";
	m_proxy = new BFProxy(key, strlen((const char *)key));
	m_fish = new BlowFish(key, strlen((const char *)key));
}

void BFProxyTest::tearDown()
{
	delete m_proxy;
	delete m_fish;
}

void BFProxyTest::testEncrypt()
{
	const unsigned char block[CryptoInterface::BlockLength] = "1234567";
	CryptoInterface::block p_out, f_out;

	m_proxy->encrypt(block, p_out);
	m_fish->encrypt(block, f_out);

	CPPUNIT_ASSERT(memcmp(p_out, f_out, CryptoInterface::BlockLength) == 0);
}

void BFProxyTest::testDecrypt()
{
	const unsigned char block[CryptoInterface::BlockLength] = "1234567";
	CryptoInterface::block p_out, f_out;

	m_proxy->decrypt(block, p_out);
	m_fish->decrypt(block, f_out);

	CPPUNIT_ASSERT(memcmp(p_out, f_out, CryptoInterface::BlockLength) == 0);
}

void BFProxyTest::testMulti()
{
	const unsigned char block[CryptoInterface::BlockLength] = "1234567";
	CryptoInterface::block out, orig;

	m_proxy->encrypt(block, out);
	m_proxy->encrypt(block, out);
	m_proxy->decrypt(out, orig);
	CPPUNIT_ASSERT(memcmp(orig, block, CryptoInterface::BlockLength) == 0);
}

void BFProxyTest::testInPlace()
{
	unsigned char block[CryptoInterface::BlockLength] = "1234567";
	unsigned char orig[CryptoInterface::BlockLength] = "1234567";

	m_proxy->encrypt(block, block);
	m_proxy->decrypt(block, block);
	CPPUNIT_ASSERT(memcmp(orig, block, CryptoInterface::BlockLength) == 0);
}

CPPUNIT_TEST_SUITE_REGISTRATION(BFProxyTest);
