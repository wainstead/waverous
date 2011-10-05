
package = waverous
version = 0.2
tarname = $(package)
distdir = $(tarname)-$(version)

all clean:
	cd server && $(MAKE) $@

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir):
	svn export https://waverous.googlecode.com/svn/trunk $(distdir)


distcheck: $(distdir).tar.gz
	gzip -cd $(distdir).tar.gz | tar xvf -
	cd $(distdir) && $(MAKE) moo
	cd $(distdir) && $(MAKE) clean
	rm -rf $(distdir)
	@echo "*** Package $(distdir).tar.gz is ready for distribution."

.PHONY: all clean dist distcheck
