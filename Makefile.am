SUBDIRS = src man
EXTRA_DIST = autobld bld data debian doc

test:
	$(MAKE) check
	cd bld && env MY_BIN_DIR=../src/nco ./nco_tst.pl
