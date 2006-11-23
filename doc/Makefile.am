info_TEXINFOS = nco.texi
EXTRA_DIST = nco nco.pdf nco.ps VERSION

# csz 20061120 override default MAKEINFOHTML defaults:
# --ifinfo: Treat HTML branches same as info branches
# --output=html: Output to html directory
# Unfortunately, autoconf overrides --output
AM_MAKEINFOHTMLFLAGS = --ifinfo
# Create single HTML file instead of file-per-node
#AM_MAKEINFOHTMLFLAGS = --ifinfo --no-split

# Automake manual section 14:
# If configure built it, 'distclean' should delete it
# If make built it, 'clean' should delete it
CLEANFILES = nco nco.info* nco.pdf nco.txt nco.xml

# Additions for NCO; automake may support these later on
html_docs = nco.html
.texi.html:
	$(MAKEINFO) --html $<
#	$(MAKEINFO) --html --no-split $<
.PHONY: html_docs pdf_docs
pdf_docs = nco.pdf
.texi.pdf:
	$(TEXI2DVI) --pdf $<
	
