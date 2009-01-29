# $Header: /data/zender/nco_20150216/nco/Makefile.am,v 1.13 2009-01-29 00:30:38 zender Exp $ -*-makefile-*-

SUBDIRS = data src man doc

# Everything in bld/ is added to distribution separately since autoconf
# does not work with it (bld/Makefile is not autoconf's)
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
