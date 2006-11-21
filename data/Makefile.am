# $Header: /data/zender/nco_20150216/nco/data/Makefile.am,v 1.6 2006-11-21 01:04:32 zender Exp $ -*-makefile-*-

# NCO-specific definitions
all: test_data
.PHONY: test_data
test_data = in.nc in_rec_zero.nc big.nc
.cdl.nc:
	ncgen -o $@ $<

EXTRA_DIST = big.cdl in.cdl in_rec_zero.cdl ncap.in ncap.in2 ncap2.in tst.nco

# Automake manual section 14:
# If configure built it, 'distclean' should delete it
# If make built it, 'clean' should delete it
DISTCLEANFILES = foo*
CLEANFILES = 85.nc 86.nc 87.nc 88.nc 89.nc foo.nc h0001.nc h0002.nc h0003.nc in.nc in_red_zero.nc ../src/nco_c++/in.nc

