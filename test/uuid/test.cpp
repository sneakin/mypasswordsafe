#include <iostream>
#include <quuid.h>
#include <endian.h>
#include "pwsafe/Util.h"
#include "uuid.hpp"

using namespace std;

uint16_t getInt16_net(unsigned char *buf)
{
#if(BYTE_ORDER == LITTLE_ENDIAN)
	return (buf[1] | (buf[0] << 8));
#elif(BYTE_ORDER == BIG_ENDIAN)
	return *(uint16_t *)buf;
#else
#error No byte order defined
#endif
}

void putInt16_net(unsigned char *buf, const uint16_t val)
{
#if(BYTE_ORDER == LITTLE_ENDIAN)
	buf[1] = val & 0xff;
	buf[0] = (val >> 8) & 0xff;
#elif(BYTE_ORDER == BIG_ENDIAN)
	*(int16 *)buf = val;
#else
#error No byte order defined
#endif
}

uint32_t getInt32_net(unsigned char *buf)
{
#if(BYTE_ORDER == LITTLE_ENDIAN)
	return (buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24));
#elif(BYTE_ORDER == BIG_ENDIAN)
	return *(uint32_t *)buf;
#else
#error No byte order defined
#endif
}

void putInt32_net(unsigned char *buf, const uint32_t val)
{
#if(BYTE_ORDER == LITTLE_ENDIAN)
	buf[3] = val & 0xff;
	buf[2] = (val >> 8) & 0xff;
	buf[1] = (val >> 16) & 0xff;
	buf[0] = (val >> 24) & 0xff;
#elif(BYTE_ORDER == BIG_ENDIAN)
	*(uint32 *)buf = val;
#else
#error No byte order defined
#endif
}

void uuid_toArray(const QUuid &u, unsigned char *a)
{
	uint *l = (uint *)a;
	putInt32_net(a, u.data1);

	putInt16_net(a + sizeof(uint), u.data2);
	putInt16_net(a + sizeof(uint) + sizeof(ushort), u.data3);

	unsigned char *b = (a + sizeof(uint) + 2 * sizeof(ushort));
	for(int i = 0; i < 8; i++)
		b[i] = u.data4[i];
}

int main()
{
	unsigned char u[16] = {
		0x0, 0x1, 0x2, 0x3, 0x4,
		0x5, 0x6, 0x7, 0x8, 0x9,
		0xa, 0xb, 0xc, 0xd, 0xe,
		0xf
	};
	
	UUID mine(u);
	cout << mine.toString().c_str() << endl;

	uint l = getInt32_net(u);
	ushort w1 = getInt16_net(u + sizeof(uint32_t));
	ushort w2 = getInt16_net(u + sizeof(uint32_t) + sizeof(uint16_t));
	QUuid qts(l, w1, w2, u[8], u[9], u[10], u[11], u[12], u[13], u[14], u[15]);
	cout << qts.toString().ascii() << endl;

	uuid_toArray(qts, u);
	mine.fromArray(u);
	cout << mine.toString().c_str() << endl;

	l = getInt32_net(u);
	w1 = u[4] << 8 | u[5];
	w2 = u[6] << 8 | u[7];
	qts = QUuid(l, w1, w2, u[8], u[9], u[10], u[11], u[12], u[13], u[14], u[15]);
	cout << qts.toString().ascii() << endl;

	return 0;
}
