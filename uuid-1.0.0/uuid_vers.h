/*
**  uuid_vers.h -- Version Information for OSSP uuid (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _UUID_VERS_H_AS_HEADER_

#ifndef _UUID_VERS_H_
#define _UUID_VERS_H_

#define _UUID_VERSION 0x100200

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} _uuid_version_t;

extern _uuid_version_t _uuid_version;

#endif /* _UUID_VERS_H_ */

#else /* _UUID_VERS_H_AS_HEADER_ */

#define _UUID_VERS_H_AS_HEADER_
#include "uuid_vers.h"
#undef  _UUID_VERS_H_AS_HEADER_

_uuid_version_t _uuid_version = {
    0x100200,
    "1.0.0",
    "1.0.0 (16-Feb-2004)",
    "This is OSSP uuid, Version 1.0.0 (16-Feb-2004)",
    "OSSP uuid 1.0.0 (16-Feb-2004)",
    "OSSP uuid/1.0.0",
    "@(#)OSSP uuid 1.0.0 (16-Feb-2004)",
    "$Id: uuid_vers.h,v 1.1 2004/06/10 11:45:47 nolan Exp $"
};

#endif /* _UUID_VERS_H_AS_HEADER_ */

