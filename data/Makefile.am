# $Header: /data/zender/nco_20150216/nco/data/Makefile.am,v 1.3 2005-11-09 21:15:48 zender Exp $ -*-makefile-*-

# NCO-specific definitions
all: test_data
.PHONY: test_data
test_data = in.nc big.nc
.cdl.nc:
	ncgen -o $@ $<

EXTRA_DIST = big.cdl ncap.in ncap.in2 in.cdl tst.nco

# from the automake manual section 14
# If configure built it, then distclean should delete it
# if make built it, then clean should delete it
DISTCLEANFILES = 85.nc 86.nc 87.nc 88.nc 89.nc foo.nc h0001.nc h0002.nc h0003.nc in.nc ../src/nco_c++/in.nc
CLEANFILES = foo*

