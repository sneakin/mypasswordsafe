# MyPasswordSafe Makefile
# $Header: /home/cvsroot/MyPasswordSafe/Makefile,v 1.5 2004/06/10 11:46:38 nolan Exp $

# This is the only configuration setting. It specifies where the files will
# be copied when "make install" is called, and where MyPS will search for
# its language files.
PREFIX=/usr/local

export BOOST_DIR=$(HOME)/src/boost-1.30.2
QMAKE=qmake
DATE=$(shell date +%Y%m%d)
VERSION=0.6.1
ifndef USER
	USER=$(USER)
endif
HOST=$(shell hostname).$(shell domainname)

all: MyPasswordSafe

MyPasswordSafe.mak: MyPasswordSafe.pro
	$(QMAKE) -o $@ $<

MyPasswordSafe: config uuid MyPasswordSafe.mak
	make -f MyPasswordSafe.mak
	lrelease locale/*.ts

uuid:
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
	echo "#define ENDIAN $(shell ./config/endian)" >> src/config.h
	echo "#endif" >> src/config.h

config: src/config.h

clean: MyPasswordSafe.mak
	make -f $< clean
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
