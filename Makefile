
package = waverous
version = beta2
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

.PHONY: all clean dist