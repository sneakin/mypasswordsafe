# MyPasswordSafe Makefile
# $Header: /home/cvsroot/MyPasswordSafe/Makefile,v 1.12 2004/09/27 20:44:21 nolan Exp $

# This is the only configuration setting. It specifies where the files will
# be copied when "make install" is called, and where MyPS will search for
# its language files.
ifndef PREFIX
	PREFIX=/usr/local
endif

QMAKE=qmake
DATE=$(shell date +%Y%m%d)
VERSION=0.6.4
ifndef USER
	# get the user's name from the environment
	USER=$(USER)
endif
HOST=$(shell hostname).$(shell domainname)

all: MyPasswordSafe

ChangeLog:
	./tools/cvs2cl.pl

MyPasswordSafe.mak: MyPasswordSafe.pro
	$(QMAKE) -o $@ $<

MyPasswordSafe: config uuid-1.0.0/.libs/libuuid.a MyPasswordSafe.mak
	make -f MyPasswordSafe.mak
	lrelease locale/*.ts

uuid-1.0.0/.libs/libuuid.a:
	cd uuid-1.0.0 && ./configure --prefix=$(PREFIX) && cd ..
	make -C uuid-1.0.0

doc/api:
	mkdir doc/api

doc: doc/api
	doxygen doxygen.conf

src/config.h:
	echo "#ifndef CONFIG_H" > src/config.h
	echo "#define CONFIG_H" >> src/config.h
	echo "#define PREFIX \"$(PREFIX)\"" >> src/config.h
	echo "#define VERSION \"$(VERSION)\"" >> src/config.h
	echo "#define COMP_DATE \"$(DATE)\"" >> src/config.h
	echo "#define COMP_USER \"$(USER)\"" >> src/config.h
	echo "#define COMP_HOST \"$(HOST)\"" >> src/config.h
	echo "#endif" >> src/config.h

config: src/config.h

clean: MyPasswordSafe.mak
	make -f $< clean
	make -C uuid-1.0.0 clean
	rm $<
	rm -r doc/api

install:
	install -d $(PREFIX)/bin $(PREFIX)/share/MyPasswordSafe/locale \
		$(PREFIX)/doc/MyPasswordSafe/sshots
	install MyPasswordSafe $(PREFIX)/bin/MyPasswordSafe
	install -m 0644 locale/*.qm $(PREFIX)/share/MyPasswordSafe/locale
	install -m 0644 doc/manual.html $(PREFIX)/doc/MyPasswordSafe
	install -m 0644 doc/sshots/*.jpg $(PREFIX)/doc/MyPasswordSafe/sshots

uninstall:
	rm $(PREFIX)/bin/MyPasswordSafe
	rm -r $(PREFIX)/share/MyPasswordSafe
	rm -r $(PREFIX)/doc/MyPasswordSafe

release:
	make -c release

commit:
	cvs commit
