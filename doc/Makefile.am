info_TEXINFOS = nco.texi
EXTRA_DIST = nco.pdf nco.ps nco.html

# our own additions for NCO; they may be supported by automake later on
html_docs = nco.html
.texi.html:
	$(MAKEINFO) --html --no-split $<
.PHONY: html_docs pdf_docs
pdf_docs = nco.pdf
.texi.pdf:
	$(TEXI2DVI) --pdf $<
	
