PROGRAM=MyPasswordSafe
DATE=$(shell date +%Y%m%d)
BRANCH=
RELEASE=$(PROGRAM)-$(DATE)

all: src bin

src: $(RELEASE).src.tgz

bin: $(RELEASE).bin.tgz

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

clean:
	rm -r $(RELEASE)
