MI_OPTS = --fill-column 79 --no-split

OUT = ProgrammersManual.info ProgrammersManual.txt \
      html/ProgrammersManual_toc.html \
      ProgrammersManual.ps.gz ProgrammersManual.dvi.gz \
      ProgrammersManual.pdf

all: ${OUT}

info: ProgrammersManual.info
ProgrammersManual.info: ProgrammersManual.texinfo
	makeinfo ${MI_OPTS} -D INFO -D INDEX ProgrammersManual.texinfo

txt: ProgrammersManual.txt
ProgrammersManual.txt: ProgrammersManual.tex-no-info
	makeinfo ${MI_OPTS} ProgrammersManual.tex-no-info

# ought to be
#ProgrammersManual.txt: ProgrammersManual.texinfo
#	makeinfo ${MI_OPTS} -D INDEX --no-headers ProgrammersManual.texinfo > ProgrammersManual.txt

ProgrammersManual.tex-no-info: ProgrammersManual.texinfo
	sed -e '/^@node/d' \
	    -e '/^@set INDEX/d' \
	    -e '/^@menu/,/^@end menu/d' \
	    -e '/^@setfilename/s/info/txt/' \
	    -e 's/@ref{\([^,}]*,\)*\([^}]*\)}/the section "\2"/' \
	    -e 's/@pxref{\([^,}]*,\)*\(\(@[a-z]*{[^}]*}\|[^@}]\)*\)}/see "\2"/' \
	    -e 's/@xref{\([^,}]*,\)*\([^}]*\)}/See the section "\2"/' \
	    < ProgrammersManual.texinfo > ProgrammersManual.tex-no-info

ps: ProgrammersManual.ps
ProgrammersManual.ps: ProgrammersManual.dvi
	dvips ProgrammersManual.dvi > ProgrammersManual.ps

dvi: ProgrammersManual.dvi
ProgrammersManual.dvi: ProgrammersManual.texinfo
	tex ProgrammersManual.texinfo
	texindex ProgrammersManual.fn
	tex ProgrammersManual.texinfo

pdf: ProgrammersManual.pdf
ProgrammersManual.pdf: ProgrammersManual.texinfo
	texi2pdf --clean --batch ProgrammersManual.texinfo


html: html/ProgrammersManual_toc.html
html/ProgrammersManual_toc.html: ProgrammersManual.texinfo
	rm -rf html
	texi2html -menu -split section -init_file t2hinit.pl -subdir html -top_file index.html -expand info ProgrammersManual.texinfo
	cp -p common.css html
	@echo ====================
	@echo Checking for TexInfo commands left in the html...
	@texi2html -check html/ProgrammersManual_*.html
	@echo ====================


%.Z: %
	compress -c $< > $@

%.gz: %
	gzip -c $< > $@

clean:
	rm -f *~ ProgrammersManual.tex-no-info
	rm -f ProgrammersManual.cp ProgrammersManual.fn ProgrammersManual.ky
	rm -f ProgrammersManual.pg ProgrammersManual.tp ProgrammersManual.vr
	rm -f ProgrammersManual.log ProgrammersManual.aux ProgrammersManual.toc
	rm -f ProgrammersManual.fns
	rm -f ProgrammersManual.tmp

distclean: clean
	rm -f ProgrammersManual.dvi* ProgrammersManual.p* ProgrammersManual.i*
	rm -f ProgrammersManual.txt
	rm -rf html

# $Log: not supported by cvs2svn $
# Revision 1.1.1.2  2004/05/31 07:04:20  wrog
# eostrom last version
#
# Revision 1.4  1996/07/09 20:35:41  pavel
# Added short target names ('info', 'txt', 'ps', etc.) for all targets.
# Fixed up makeinfo options to get proper results for all targets.
# Added more automation to the HTML target, to push the results into the
# html/ subdirectory.
# Fixed up `make clean' to be more correct.
#
# Revision 1.3  1996/03/04  02:00:58  pavel
# Add `make clean', which is run *after* `make all' to get rid of all of the
# chaff that that latter command generates.
#
# Revision 1.2  1996/03/04  01:34:32  pavel
# Made to work with newer versions of texi2html and texinfo.tex.
