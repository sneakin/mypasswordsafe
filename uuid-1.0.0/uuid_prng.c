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
**  uuid_prng.c: PRNG API implementation
*/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>

#include "uuid_prng.h"

struct prng_st {
    int devfd;
};

prng_rc_t prng_create(prng_t **prng)
{
    int fd = -1;
    struct timeval tv;
    pid_t pid;
    size_t i;

    /* sanity check argument(s) */
    if (prng == NULL)
        return PRNG_RC_ARG;

    /* allocate object */
    if ((*prng = (prng_t *)malloc(sizeof(prng_t))) == NULL)
        return PRNG_RC_MEM;

    /* try to open the system PRNG device */
    (*prng)->devfd = -1;
    if ((fd = open("/dev/urandom", O_RDONLY)) == -1)
        fd = open("/dev/random", O_RDONLY|O_NONBLOCK);
    if (fd != -1) {
        fcntl(fd, F_SETFD, FD_CLOEXEC);
        (*prng)->devfd = fd;
    }

    /* seed the C library PRNG once */
    gettimeofday(&tv, NULL);
    pid = getpid();
    srand((unsigned int)(
        ((unsigned int)pid << 16)
        ^ (unsigned int)pid
        ^ (unsigned int)tv.tv_sec
        ^ (unsigned int)tv.tv_usec));

    /* crank the PRNG a few times */
    gettimeofday(&tv, NULL);
    for (i = (unsigned int)(tv.tv_sec ^ tv.tv_usec) & 0x1F; i > 0; i--)
        (void)rand();

    return PRNG_RC_OK;
}

prng_rc_t prng_data(prng_t *prng, void *data_ptr, size_t data_len)
{
    size_t n;
    unsigned char *p;
    int cnt;
    int i;

    /* sanity check argument(s) */
    if (prng == NULL || data_len == 0)
        return PRNG_RC_ARG;

    /* try to gather data from the system PRNG device */
    if (prng->devfd != -1) {
        p = (unsigned char *)data_ptr;
        n = data_len;
        cnt = 0;
        while (n > 0) {
            i = read(prng->devfd, (void *)p, n);
            if (i <= 0) {
                if (cnt++ > 16)
                    break;
                continue;
            }
            n -= i;
            p += i;
            cnt = 0;
        }
    }

    /* always also apply the weaker PRNG. In case the stronger PRNG device
       based source failed, this is the only remaining randomness, of course */
    for (p = (unsigned char *)data_ptr, n = 0; n < data_len; n++)
        *p++ ^= (unsigned char)(((unsigned int)rand() >> 7) & 0xFF);

    return PRNG_RC_OK;
}

prng_rc_t prng_destroy(prng_t *prng)
{
    /* sanity check argument(s) */
    if (prng == NULL)
        return PRNG_RC_ARG;

    /* close PRNG device */
    if (prng->devfd != -1)
        close(prng->devfd);

    /* free object */
    free(prng);

    return PRNG_RC_OK;
}

