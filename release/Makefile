PROGRAM=MyPasswordSafe
DATE=$(shell date +%Y%m%d)
GPGSIGN=gpg --detach-sign -a
BRANCH=
RELEASE=$(PROGRAM)-$(DATE)

all: src bin

src: $(RELEASE).src.tgz.asc

bin: $(RELEASE).bin.tgz.asc

$(RELEASE):
	#cvs co -r $(BRANCH) $@
	cvs co $(PROGRAM)
	mv $(PROGRAM) $@
	make -C $@ ChangeLog

$(RELEASE).bin.tgz: $(RELEASE)
	make -C $< all
	tar cfz $@ $</MyPasswordSafe $</Makefile $</README $</ChangeLog $</CHANGES $</doc/manual.html $</doc/manual.fr.html \
		$</doc/sshots $</locale

$(RELEASE).src.tgz: $(RELEASE)
	tar cfz $@ $<

$(RELEASE).bin.tgz.asc: $(RELEASE).bin.tgz
	$(GPGSIGN) $<

$(RELEASE).src.tgz.asc: $(RELEASE).src.tgz
	$(GPGSIGN) $<

clean:
	rm -r $(RELEASE) *.tgz*
