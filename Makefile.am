SUBDIRS = data doc src man

# everything in bld/ is added to the distribution seperately since autoconf
# doesn't work with it (the Makefile is not autoconf's)
bld_extras = bld/Makefile bld/nco_tst.pl bld/pvmgetarch bld/nco.spec
EXTRA_DIST = $(bld_extras)

test:
	$(MAKE) check
	cd bld && env MY_BIN_DIR=../src/nco ./nco_tst.pl
