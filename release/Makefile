DATE=$(shell date +%Y%m%d)
BRANCH=

all: src bin

src: MyPasswordSafe-$(DATE).src.tgz

bin: MyPasswordSafe-$(DATE).bin.tgz

MyPasswordSafe:
	#cvs co -r $(BRANCH) $@
	cvs co $@
	cp ../ChangeLog MyPasswordSafe

MyPasswordSafe-$(DATE).bin.tgz: MyPasswordSafe
	make -C $< all
	tar cfz $@ $</MyPasswordSafe $</Makefile $</README $</ChangeLog $</CHANGES $</doc/manual.html $</doc/manual.fr.html \
		$</doc/sshots $</locale

MyPasswordSafe-$(DATE).src.tgz: MyPasswordSafe
	tar cfz $@ $<

clean:
	rm -r MyPasswordSafe
