##
##  OSSP uuid - Universally Unique Identifier
##  Copyright (c) 2004 Ralf S. Engelschall <rse@engelschall.com>
##  Copyright (c) 2004 The OSSP Project <http://www.ossp.org/>
##
##  This file is part of OSSP uuid, a library for the generation
##  of UUIDs which can found at http://www.ossp.org/pkg/lib/uuid/
##
##  Permission to use, copy, modify, and distribute this software for
##  any purpose with or without fee is hereby granted, provided that
##  the above copyright notice and this permission notice appear in all
##  copies.
##
##  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
##  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
##  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
##  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
##  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
##  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
##  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
##  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
##  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
##  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
##  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
##  SUCH DAMAGE.
##
##  Makefile.in: make(1) build procedure
##



DESTDIR     =
prefix      = /usr/local
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
libdir      = ${exec_prefix}/lib
includedir  = ${prefix}/include
mandir      = ${prefix}/man

CC          = gcc
CPPFLAGS    = 
CFLAGS      = -DHAVE_CONFIG_H -O2 -pipe
LDFLAGS     = 
LIBS        = -lnsl 
RM          = rm -f
CP          = cp
RMDIR       = rmdir
SHTOOL      = ./shtool
LIBTOOL     = ./libtool
TRUE        = true
POD2MAN     = pod2man

LIB_NAME    = libuuid.la
LIB_OBJS    = uuid.lo uuid_md5.lo uuid_prng.lo uuid_mac.lo uuid_ui64.lo uuid_str.lo

DCE_NAME    = libuuid_dce.la
DCE_OBJS    = $(LIB_OBJS) uuid_dce.lo

PRG_NAME    = uuid
PRG_OBJS    = uuid_cli.o

MAN_NAME    = uuid.3 uuid.1

TARGETS     = $(LIB_NAME)  $(PRG_NAME)

WITH_DCE    = no

.SUFFIXES:
.SUFFIXES: .c .o .lo

all: $(TARGETS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

.c.lo:
	@$(LIBTOOL) --mode=compile $(CC) $(CPPFLAGS) $(CFLAGS) -c $<

$(LIB_NAME): $(LIB_OBJS)
	@$(LIBTOOL) --mode=link $(CC) -o $(LIB_NAME) $(LIB_OBJS) -rpath $(libdir) \
	    -version-info `$(SHTOOL) version -l txt -d libtool VERSION`

$(DCE_NAME): $(DCE_OBJS)
	@$(LIBTOOL) --mode=link $(CC) -o $(DCE_NAME) $(DCE_OBJS) -rpath $(libdir) \
	    -version-info `$(SHTOOL) version -l txt -d libtool VERSION`

$(PRG_NAME): $(PRG_OBJS) $(LIB_NAME)
	@$(LIBTOOL) --mode=link $(CC) $(LDFLAGS) -o $(PRG_NAME) $(PRG_OBJS) $(LIB_NAME) $(LIBS)

uuid.lo: uuid.c config.h uuid.h uuid_md5.h uuid_prng.h uuid_mac.h uuid_ui64.h uuid_str.h uuid_bm.h uuid_ac.h
uuid_mac.lo: uuid_mac.c config.h uuid_mac.h
uuid_md5.lo: uuid_md5.c uuid_md5.h
uuid_prng.lo: uuid_prng.c uuid_prng.h
uuid_str.lo: uuid_str.c config.h uuid_str.h
uuid_ui64.lo: uuid_ui64.c uuid_ui64.h
uuid_dce.lo: uuid_dce.c uuid.h uuid_dce.h
uuid_cli.o: uuid_cli.c uuid.h

man: uuid.3 uuid-config.1 uuid.1
uuid.3: uuid.pod
	V1=`$(SHTOOL) version -l txt -d short VERSION`; \
	V2=`$(SHTOOL) version -l txt -d long VERSION`; \
	D=`$(SHTOOL) version -l txt -d long VERSION | sed -e 's;.*(;;' -e 's;).*;;'`; \
	$(POD2MAN) --quotes=none \
	           --section=3 --center="Universally Unique Identifier" \
	           --release="$$D" --date="OSSP uuid $$V1" uuid.pod | \
	sed -e "s;UUID_VERSION_STR;$$V2;" >uuid.3
uuid-config.1: uuid-config.pod
	V1=`$(SHTOOL) version -l txt -d short VERSION`; \
	V2=`$(SHTOOL) version -l txt -d long VERSION`; \
	D=`$(SHTOOL) version -l txt -d long VERSION | sed -e 's;.*(;;' -e 's;).*;;'`; \
	$(POD2MAN) --quotes=none \
	           --section=3 --center="Universally Unique Identifier" \
	           --release="$$D" --date="OSSP uuid $$V1" uuid-config.pod | \
	sed -e "s;UUID_VERSION_STR;$$V2;" >uuid-config.1
uuid.1: uuid_cli.pod
	V1=`$(SHTOOL) version -l txt -d short VERSION`; \
	V2=`$(SHTOOL) version -l txt -d long VERSION`; \
	D=`$(SHTOOL) version -l txt -d long VERSION | sed -e 's;.*(;;' -e 's;).*;;'`; \
	$(CP) uuid_cli.pod /tmp/uuid.pod; \
	$(POD2MAN) --quotes=none \
	           --section=1 --center="Universally Unique Identifier" \
	           --release="$$D" --date="OSSP uuid $$V1" /tmp/uuid.pod | \
	sed -e "s;UUID_VERSION_STR;$$V2;" >uuid.1; \
	$(RM) /tmp/uuid.pod

check: $(PRG_NAME)
	@echo "==== UUID version 1 (time and node based): 4 single iterations"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v1 -n 4 -1
	@echo "==== UUID version 1 (time and node based): 4 subsequent iterations"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v1 -n 4
	@echo "==== UUID version 3 (name based): 2 times repeated"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v3 -n 2 ns:URL http://www.ossp.org/
	@echo "==== UUID version 4 (random data based): 4 single iterations"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v4 -n 4 -1
	@echo "==== UUID version 4 (random data based): 4 subsequent iterations"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v4 -n 4
	@echo "==== UUID version 1 generation and decoding"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -d `$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v1`; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -d `$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v1 -m`
	@echo "==== UUID version 3 generation and decoding"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -d `$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v3 ns:URL http://www.ossp.org/`
	@echo "==== UUID version 3 generation and decoding"; \
	$(LIBTOOL) --mode=execute ./$(PRG_NAME) -d `$(LIBTOOL) --mode=execute ./$(PRG_NAME) -v4`

install:
	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(prefix)
	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(bindir)
	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(includedir)
	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(libdir)
	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(mandir)/man3
	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(mandir)/man1
	$(SHTOOL) install -c -m 755 uuid-config $(DESTDIR)$(bindir)/
	$(SHTOOL) install -c -m 644 uuid-config.1 $(DESTDIR)$(mandir)/man1/
	$(SHTOOL) install -c -m 644 uuid.h $(DESTDIR)$(includedir)/
	-@if [ ".$(WITH_DCE)" = .yes ]; then \
	    echo "$(SHTOOL) install -c -m 644 uuid_dce.h $(DESTDIR)$(includedir)/"; \
	    $(SHTOOL) install -c -m 644 uuid_dce.h $(DESTDIR)$(includedir)/; \
	fi
	$(SHTOOL) install -c -m 644 uuid.3 $(DESTDIR)$(mandir)/man3/
	@$(LIBTOOL) --mode=install $(SHTOOL) install -c -m 644 libuuid.la $(DESTDIR)$(libdir)/
	-@if [ ".$(WITH_DCE)" = .yes ]; then \
	    $(LIBTOOL) --mode=install $(SHTOOL) install -c -m 644 libuuid_dce.la $(DESTDIR)$(libdir)/; \
	fi
	@$(LIBTOOL) --mode=install $(SHTOOL) install -c -s -m 755 uuid $(DESTDIR)$(bindir)/
	$(SHTOOL) install -c -m 644 uuid.1 $(DESTDIR)$(mandir)/man1/

uninstall:
	-$(RM) $(DESTDIR)$(mandir)/man1/uuid.1
	-@$(LIBTOOL) --mode=uninstall $(RM) $(DESTDIR)$(bindir)/uuid
	-@$(LIBTOOL) --mode=uninstall $(RM) $(DESTDIR)$(libdir)/libuuid.la
	-@if [ ".$(WITH_DCE)" = .yes ]; then \
	    $(LIBTOOL) --mode=uninstall $(RM) $(DESTDIR)$(libdir)/libuuid_dce.la; \
	fi
	-$(RM) $(DESTDIR)$(mandir)/man3/uuid.3
	-$(RM) $(DESTDIR)$(includedir)/uuid.h
	-@if [ ".$(WITH_DCE)" = .yes ]; then \
	    echo "$(RM) $(DESTDIR)$(includedir)/uuid_dce.h"; \
	    $(RM) $(DESTDIR)$(includedir)/uuid_dce.h; \
	fi
	-$(RM) $(DESTDIR)$(mandir)/man1/uuid-config.1
	-$(RM) $(DESTDIR)$(bindir)/uuid-config
	-$(RMDIR) $(DESTDIR)$(mandir)/man1 >/dev/null 2>&1 || $(TRUE)
	-$(RMDIR) $(DESTDIR)$(mandir)/man3 >/dev/null 2>&1 || $(TRUE)
	-$(RMDIR) $(DESTDIR)$(mandir) >/dev/null 2>&1 || $(TRUE)
	-$(RMDIR) $(DESTDIR)$(libdir) >/dev/null 2>&1 || $(TRUE)
	-$(RMDIR) $(DESTDIR)$(includedir) >/dev/null 2>&1 || $(TRUE)
	-$(RMDIR) $(DESTDIR)$(bindir) >/dev/null 2>&1 || $(TRUE)
	-$(RMDIR) $(DESTDIR)$(prefix) >/dev/null 2>&1 || $(TRUE)

clean:
	-$(RM) $(LIB_NAME) $(LIB_OBJS)
	-@if [ ".$(WITH_DCE)" = .yes ]; then \
	    echo "$(RM) $(DCE_NAME) $(DCE_OBJS)"; \
	    $(RM) $(DCE_NAME) $(DCE_OBJS); \
	fi
	-$(RM) $(PRG_NAME) $(PRG_OBJS)
	-$(RM) -r .libs >/dev/null 2>&1 || $(TRUE)
	-$(RM) *.o *.lo

distclean: clean
	-$(RM) config.log config.status config.cache
	-$(RM) Makefile config.h uuid-config uuid.h
	-$(RM) libtool

realclean: distclean
	-$(RM) uuid.3 uuid.1
	-$(RM) configure config.h.in
	-$(RM) shtool
	-$(RM) ltmain.sh libtool.m4 config.guess config.sub

