PROGRAM=MyPasswordSafe
DATE=$(shell date +%Y%m%d)
GPGSIGN=gpg --detach-sign -a
BRANCH=
RELEASE=$(PROGRAM)-$(DATE)
UPLOAD_TO=sg:/home/sneakin/web/semanticgap.com/myps/release/devel

all: src bin

sign: $(RELEASE).src.tgz.asc $(RELEASE).bin.tgz.asc

src: $(RELEASE).src.tgz

bin: $(RELEASE).bin.tgz

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
	mv $</images/about_banner.png .
	mv $</images/about_banner.src.png $</images/about_banner.png
	tar cfz $@ $<
	mv about_banner.png $</images/about_banner.png

$(RELEASE).bin.tgz.asc: $(RELEASE).bin.tgz
	$(GPGSIGN) $<

$(RELEASE).src.tgz.asc: $(RELEASE).src.tgz
	$(GPGSIGN) $<

upload-src: $(RELEASE).src.tgz.asc
	scp $(RELEASE).src.tgz $(UPLOAD_TO)
	scp $(RELEASE).src.tgz.asc $(UPLOAD_TO)

upload-bin: $(RELEASE).bin.tgz.asc
	scp $(RELEASE).bin.tgz $(UPLOAD_TO)
	scp $(RELEASE).bin.tgz.asc $(UPLOAD_TO)

release: upload-src upload-bin

clean:
	rm -r $(PROGRAM)*
