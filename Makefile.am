# $Header: /data/zender/nco_20150216/nco/Makefile.am,v 1.9 2005-11-09 21:15:47 zender Exp $ -*-makefile-*-

SUBDIRS = data doc src man

# everything in bld/ is added to the distribution seperately since autoconf
# doesn't work with it (the Makefile is not autoconf's)
bld_extras = bld/Makefile bld/nco_tst.pl bld/pvmgetarch bld/nco.spec
EXTRA_DIST = $(bld_extras)

test:
	$(MAKE) check
	cd bld && env MY_BIN_DIR=../src/nco ./nco_tst.pl
