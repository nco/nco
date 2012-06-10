# $Header: /data/zender/nco_20150216/nco/Makefile.am,v 1.16 2012-06-10 07:20:19 pvicente Exp $ -*-makefile-*-

SUBDIRS = data src man doc

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
