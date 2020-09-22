# $Header$ -*-makefile-*-

# fxm: 20150923 Allow for building documentation (man pages) without TeXInfo
info_TEXINFOS = nco.texi
EXTRA_DIST = nco nco.dvi nco.html nco.pdf nco.ps nco.xml VERSION

# csz 20061120 override default MAKEINFOHTML defaults:
# --ifinfo: Treat HTML branches same as info branches
# --output=html: Output to html directory
# Unfortunately, autoconf overrides --output
# AM_MAKEINFOHTMLFLAGS = --ifinfo
# Create single HTML file instead of file-per-node
AM_MAKEINFOHTMLFLAGS = --ifinfo --no-split

# 20200917 Attempt to improve TAG-generation
ETAGS_ARGS = Makefile.am VERSION nco.texi ANNOUNCE MANIFEST debian.txt
TAGS_DEPENDENCIES = Makefile.am VERSION nco.texi ANNOUNCE MANIFEST debian.txt

## Automake manual section 14:
## If configure built it, 'distclean' should delete it
## If make built it, 'clean' should delete it
#CLEANFILES = nco nco.dvi nco.info* nco.pdf nco.ps nco.txt nco.xml
#
## Additions for NCO; automake may support these later on
## .PHONY tells make to remake the following non-file targets 
#.PHONY: html_docs pdf_docs txt_docs xml_docs
#html_docs = nco.html
#.texi.html:
#	$(MAKEINFO) --html --ifinfo --no-split --output=$@ $<
##	$(MAKEINFO) --html --no-split $<
#pdf_docs = nco.pdf
#.texi.pdf:
#	$(TEXI2DVI) --pdf $<
##ps_docs = nco.ps
##.texi.ps:
##	dvips -o $@ $<
#txt_docs = nco.txt
#.texi.txt:
#	$(MAKEINFO) --no-headers --output=$@ $<
#xml_docs = nco.xml
#.texi.xml:
#	$(MAKEINFO) --xml --ifinfo --no-split --output=$@ $<

