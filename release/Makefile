PROGRAM=MyPasswordSafe
DATE=$(shell date +%Y%m%d)
GPGSIGN=gpg --detach-sign -a
BRANCH=
RELEASE=$(PROGRAM)-$(DATE)
UPLOAD_TO=sg:/home/nolan/web/semanticgap.com/myps/release/devel

all: upload
#src bin

src: $(RELEASE).src.tgz.asc

bin: $(RELEASE).bin.tgz.asc

$(RELEASE):
	#cvs co -r $(BRANCH) $@
	cvs co $(PROGRAM)
	mv $(PROGRAM) $@
	$(MAKE) -C $@ ChangeLog

$(RELEASE).bin.tgz: $(RELEASE)
	$(MAKE) -C $< all
	tar cfz $@ $</MyPasswordSafe $</Makefile $</README $</ChangeLog $</CHANGES $</doc/manual.html $</doc/manual.fr.html \
		$</doc/sshots $</locale

$(RELEASE).src.tgz: $(RELEASE)
	tar cfz $@ $<

$(RELEASE).bin.tgz.asc: $(RELEASE).bin.tgz
	$(GPGSIGN) $<

$(RELEASE).src.tgz.asc: $(RELEASE).src.tgz
	$(GPGSIGN) $<

upload-src: src
	scp $(RELEASE).src.tgz $(UPLOAD_TO)
	scp $(RELEASE).src.tgz.asc $(UPLOAD_TO)

upload-bin: bin
	scp $(RELEASE).bin.tgz $(UPLOAD_TO)
	scp $(RELEASE).bin.tgz.asc $(UPLOAD_TO)

upload: upload-src upload-bin

clean:
	rm -r $(PROGRAM)*
