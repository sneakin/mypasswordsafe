/* $Header: /home/cvsroot/MyPasswordSafe/src/pwsafe/PwsPlatform.h,v 1.3 2004/10/11 10:27:10 nolan Exp $
 */
#ifndef PwsPlatform_h
#define PwsPlatform_h

#ifdef __NetBSD__
#include <sys/endian.h>
#else
#include <endian.h>
#endif

#ifndef BYTE_ORDER
#error BYTE_ORDER not defined
#else
#if((BYTE_ORDER != LITTLE_ENDIAN) && (BYTE_ORDER != BIG_ENDIAN))
#error Unknown byte order
#endif
#endif /* BYTE_ORDER */

#if WIN32
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif
