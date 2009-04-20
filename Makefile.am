# $Header: /data/zender/nco_20150216/nco/Makefile.am,v 1.14 2009-04-20 00:28:31 zender Exp $ -*-makefile-*-

SUBDIRS = data src man doc

# Separately add desired files from nco/bld/ to distribution
# Autoconf does not know about the bld directory 
# In particular, nco/bld/Makefile was created manually, not by autoconf
bld_extras = bld/Makefile bld/pvmgetarch bld/nco.spec
EXTRA_DIST = $(bld_extras)
ACLOCAL_AMFLAGS = -I m4

test:
	$(MAKE) check
	cd bm && env MY_BIN_DIR=../src/nco DATA=/tmp ./nco_bm.pl --regress

regress:
	cd bm && env MY_BIN_DIR=../src/nco DATA=/tmp ./nco_bm.pl --regress

regress-mpi:
	cd bm && env MY_BIN_DIR=../src/nco DATA=/tmp ./nco_bm.pl --regress --mpi_prc=2
