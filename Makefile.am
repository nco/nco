# $Header: /data/zender/nco_20150216/nco/Makefile.am,v 1.11 2005-12-14 08:01:11 zender Exp $ -*-makefile-*-

SUBDIRS = data doc src man

# Everything in bld/ is added to distribution seperately since autoconf
# does not work with it (bld/Makefile is not autoconf's)
bld_extras = bld/Makefile bld/pvmgetarch bld/nco.spec
EXTRA_DIST = $(bld_extras)

test:
	$(MAKE) check
	cd bm && env MY_BIN_DIR=../src/nco DATA=/tmp ./nco_bm.pl --regress

regress:
	cd bm && env MY_BIN_DIR=../src/nco DATA=/tmp ./nco_bm.pl --regress

regress-mpi:
	cd bm && env MY_BIN_DIR=../src/nco DATA=/tmp ./nco_bm.pl --regress --mpi_prc=2
