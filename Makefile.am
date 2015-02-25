# $Header$ -*-makefile-*-

#SUBDIRS = data src man doc
SUBDIRS = data src man @DOC_FOLDER@

# Separately add desired files from nco/bld/ to distribution
# Autoconf does not know about the bld directory 
# In particular, nco/bld/Makefile was created manually, not by autoconf
bld_extras = bld/Makefile bld/pvmgetarch bld/nco.spec
EXTRA_DIST = $(bld_extras)
ACLOCAL_AMFLAGS = -I m4

test:	tst
tst:
	$(MAKE) check
	cd bm && ./nco_bm.pl --regress

regress:	rgr
rgr:
	cd bm && ./nco_bm.pl --regress

regress-mpi:	rgr_mpi
rgr_mpi:
	cd bm && ./nco_bm.pl --regress --mpi_prc=2
