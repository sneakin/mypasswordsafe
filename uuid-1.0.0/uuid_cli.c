/*
**  OSSP uuid - Universally Unique Identifier
**  Copyright (c) 2004 Ralf S. Engelschall <rse@engelschall.com>
**  Copyright (c) 2004 The OSSP Project <http://www.ossp.org/>
**
**  This file is part of OSSP uuid, a library for the generation
**  of UUIDs which can found at http://www.ossp.org/pkg/lib/uuid/
**
**  Permission to use, copy, modify, and distribute this software for
**  any purpose with or without fee is hereby granted, provided that
**  the above copyright notice and this permission notice appear in all
**  copies.
**
**  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
**  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
**  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
**  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
**  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
**  SUCH DAMAGE.
**
**  uuid_cli.c: command line tool
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "uuid.h"

/* error handler */
static void
error(int ec, const char *str, ...)
{
    va_list ap;

    va_start(ap, str);
    fprintf(stderr, "uuid:ERROR: ");
    vfprintf(stderr, str, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(ec);
}

/* usage handler */
static void
usage(const char *str, ...)
{
    va_list ap;

    va_start(ap, str);
    if (str != NULL) {
        fprintf(stderr, "uuid:ERROR: ");
        vfprintf(stderr, str, ap);
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "usage: uuid [-v version] [-m] [-n count] [-1] [-r] [-o filename] [namespace-name]\n");
    fprintf(stderr, "usage: uuid -d [-r] [-o filename] [uuid]\n");
    va_end(ap);
    exit(1);
}

/* main procedure */
int main(int argc, char *argv[])
{
    char uuid_buf_bin[UUID_LEN_BIN];
    char uuid_buf_str[UUID_LEN_STR+1];
    uuid_t *uuid;
    uuid_t *uuid_ns;
    uuid_rc_t rc;
    FILE *fp;
    char *p;
    int ch;
    int count;
    int i;
    int iterate;
    int raw;
    int decode;
    void *vp;
    size_t n;
    unsigned int version;

    /* command line parsing */
    count = -1;     /* no count yet */
    fp = stdout;    /* default output file */
    iterate = 0;    /* not one at a time */
    raw = 0;        /* default is ASCII output */
    decode = 0;     /* default is to encode */
    version = UUID_MAKE_V1;
    while ((ch = getopt(argc, argv, "1n:rdmo:v:h")) != -1) {
        switch (ch) {
            case '1':
                iterate = 1;
                break;
            case 'n':
                if (count > 0)
                    usage("option 'n' specified multiple times");
                count = strtol(optarg, &p, 10);
                if (*p != '\0' || count < 1)
                    usage("invalid argument to option 'n'");
                break;
            case 'r':
                raw = 1;
                break;
            case 'd':
                decode = 1;
                break;
            case 'o':
                if (fp != stdout)
                    error(1, "multiple output files are not allowed");
                if ((fp = fopen(optarg, "w")) == NULL)
                    error(1, "fopen: %s", strerror(errno));
                break;
            case 'm':
                version |= UUID_MAKE_MC;
                break;
            case 'v':
                i = strtol(optarg, &p, 10);
                if (*p != '\0')
                    usage("invalid argument to option 'v'");
                switch (i) {
                    case 1: version = UUID_MAKE_V1; break;;
                    case 3: version = UUID_MAKE_V3; break;;
                    case 4: version = UUID_MAKE_V4; break;;
                    default:
                        usage("invalid version on option 'v'");
                        break;
                }
                break;
            case 'h':
                usage(NULL);
                break;
            default:
                usage("invalid option '%c'", optopt);
        }
    }
    argv += optind;
    argc -= optind;
    if (count == -1)
        count = 1;

    if (decode) {
        /* decoding */
        if ((rc = uuid_create(&uuid)) != UUID_RC_OK)
            error(1, "uuid_create: %s", uuid_error(rc));
        if (argc != 1)
            usage("invalid number of arguments");
        if (strcmp(argv[0], "-") == 0) {
            if (raw) {
                if (fread(uuid_buf_bin, UUID_LEN_BIN, 1, stdin) != 1)
                    error(1, "fread: failed to read %d (UUID_LEN_BIN) bytes from stdin", UUID_LEN_BIN);
                if ((rc = uuid_import(uuid, UUID_FMT_BIN, uuid_buf_bin, UUID_LEN_BIN)) != UUID_RC_OK)
                    error(1, "uuid_import: %s", uuid_error(rc));
            }
            else {
                if (fread(uuid_buf_str, UUID_LEN_STR, 1, stdin) != 1)
                    error(1, "fread: failed to read %d (UUID_LEN_STR) bytes from stdin", UUID_LEN_STR);
                uuid_buf_str[UUID_LEN_STR] = '\0';
                if ((rc = uuid_import(uuid, UUID_FMT_STR, uuid_buf_str, UUID_LEN_STR)) != UUID_RC_OK)
                    error(1, "uuid_import: %s", uuid_error(rc));
            }
        }
        else {
            if (raw)
                error(1, "raw input mode only possible if reading from stdin");
            if (strlen(argv[0]) != UUID_LEN_STR)
                error(1, "invalid length of UUID string representation");
            if ((rc = uuid_import(uuid, UUID_FMT_STR, argv[0], strlen(argv[0]))) != UUID_RC_OK)
                error(1, "uuid_import: %s", uuid_error(rc));
        }
        vp = NULL;
        if ((rc = uuid_export(uuid, UUID_FMT_TXT, &vp, NULL)) != UUID_RC_OK)
            error(1, "uuid_export: %s", uuid_error(rc));
        fprintf(stdout, "%s", (char *)vp);
        free(vp);
        if ((rc = uuid_destroy(uuid)) != UUID_RC_OK)
            error(1, "uuid_destroy: %s", uuid_error(rc));
    }
    else {
        /* encoding */
        if (   (version == UUID_MAKE_V1 && argc != 0)
            || (version == UUID_MAKE_V3 && argc != 2)
            || (version == UUID_MAKE_V4 && argc != 0))
            usage("invalid number of arguments");
        if ((rc = uuid_create(&uuid)) != UUID_RC_OK)
            error(1, "uuid_create: %s", uuid_error(rc));
        if (argc == 1) {
            /* load initial UUID for setting old generator state */
            if (strlen(argv[0]) != UUID_LEN_STR)
                error(1, "invalid length of UUID string representation");
            if ((rc = uuid_import(uuid, UUID_FMT_STR, argv[0], strlen(argv[0]))) != UUID_RC_OK)
                error(1, "uuid_import: %s", uuid_error(rc));
        }
        for (i = 0; i < count; i++) {
            if (iterate) {
                if ((rc = uuid_load(uuid, "nil")) != UUID_RC_OK)
                    error(1, "uuid_load: %s", uuid_error(rc));
            }
            if (version == UUID_MAKE_V3) {
                if ((rc = uuid_create(&uuid_ns)) != UUID_RC_OK)
                    error(1, "uuid_create: %s", uuid_error(rc));
                if ((rc = uuid_load(uuid_ns, argv[0])) != UUID_RC_OK) {
                    if ((rc = uuid_import(uuid_ns, UUID_FMT_STR, argv[0], strlen(argv[0]))) != UUID_RC_OK)
                        error(1, "uuid_import: %s", uuid_error(rc));
                }
                if ((rc = uuid_make(uuid, version, uuid_ns, argv[1])) != UUID_RC_OK)
                    error(1, "uuid_make: %s", uuid_error(rc));
                if ((rc = uuid_destroy(uuid_ns)) != UUID_RC_OK)
                    error(1, "uuid_destroy: %s", uuid_error(rc));
            }
            else {
                if ((rc = uuid_make(uuid, version)) != UUID_RC_OK)
                    error(1, "uuid_make: %s", uuid_error(rc));
            }
            if (raw) {
                vp = NULL;
                if ((rc = uuid_export(uuid, UUID_FMT_BIN, &vp, &n)) != UUID_RC_OK)
                    error(1, "uuid_export: %s", uuid_error(rc));
                fwrite(vp, n, 1, fp);
                free(vp);
            }
            else {
                vp = NULL;
                if ((rc = uuid_export(uuid, UUID_FMT_STR, &vp, &n)) != UUID_RC_OK)
                    error(1, "uuid_export: %s", uuid_error(rc));
                fprintf(fp, "%s\n", (char *)vp);
                free(vp);
            }
        }
        if ((rc = uuid_destroy(uuid)) != UUID_RC_OK)
            error(1, "uuid_destroy: %s", uuid_error(rc));
    }

    /* close output channel */
    if (fp != stdout)
        fclose(fp);

    return 0;
}

