/* $Header: /home/cvsroot/MyPasswordSafe/src/pwsafe/PwsPlatform.h,v 1.4 2004/10/31 12:32:20 nolan Exp $
 */
#ifndef PwsPlatform_h
#define PwsPlatform_h

#ifdef WIN32
#  if WIN32
#    define LITTLE_ENDIAN 1234
#    define BIG_ENDIAN 4321
#    define BYTE_ORDER LITTLE_ENDIAN
#  endif
#else
#  ifdef __NetBSD__
#    include <sys/endian.h>
#  elif __APPLE__
#    include <ppc/endian.h>
#  else
#    include <endian.h>
#  endif
#endif

#ifndef BYTE_ORDER
#error BYTE_ORDER not defined
#else
#if((BYTE_ORDER != LITTLE_ENDIAN) && (BYTE_ORDER != BIG_ENDIAN))
#error Unknown byte order
#endif
#endif /* BYTE_ORDER */

#endif
