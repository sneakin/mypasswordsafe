DATE=$(shell date +%Y%m%d)
BRANCH=rel-1

all: MyPasswordSafe-$(DATE).src.tgz MyPasswordSafe-$(DATE).bin.tgz

MyPasswordSafe:
	cvs co -r $(BRANCH) $@

MyPasswordSafe-$(DATE).bin.tgz: MyPasswordSafe
	make -C $< all
	tar cfz $@ $</MyPasswordSafe $</Makefile $</README $</CHANGES $</doc/manual.html $</doc/manual.fr.html \
		$</doc/sshots $</locale

MyPasswordSafe-$(DATE).src.tgz: MyPasswordSafe
	tar cfz $@ $<

clean:
	rm -r MyPasswordSafe
