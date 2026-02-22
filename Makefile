
package = waverous
version = 0.3
tarname = $(package)
distdir = $(tarname)-$(version)

all: moo

moo:
	cd server && $(MAKE) moo

clean:
	cd server && $(MAKE) clean

distclean:
	cd server && $(MAKE) distclean

manual:
	cd manual && $(MAKE)

manual-clean:
	cd manual && $(MAKE) clean

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir):
	@if ! command -v git >/dev/null 2>&1; then \
		echo "git is required to build dist artifacts"; \
		exit 1; \
	fi
	git archive --format=tar --prefix=$(distdir)/ HEAD | tar xvf -


distcheck: $(distdir).tar.gz
	gzip -cd $(distdir).tar.gz | tar xvf -
	cd $(distdir)/server && ./configure --with-expat --with-fileio --with-fup
	cd $(distdir)/server && $(MAKE) moo
	cd $(distdir)/server && $(MAKE) clean
	rm -rf $(distdir)
	@echo "*** Package $(distdir).tar.gz is ready for distribution."

.PHONY: all moo clean distclean manual manual-clean dist distcheck
