# $Header: /data/zender/nco_20150216/nco/data/Makefile.am,v 1.14 2013-04-29 22:00:33 zender Exp $ -*-makefile-*-

test_data = big.nc in.nc in_grp.nc in_rec_zero.nc

# netCDF build rules
%.nc : %.cdl
	@PATH_TO_NCGEN@ -o $@ $< 

all: ${test_data}

EXTRA_DIST = big.cdl in.cdl in_grp.cdl in_grp_1.cdl in_grp_2.cdl cmip5.cdl obs.cdl in_rec_zero.cdl ncap.in ncap.in2 ncap2.in tst.nco

# Automake manual section 14:
# If configure built it, 'distclean' should delete it
# If make built it, 'clean' should delete it
DISTCLEANFILES = foo*

# Build procedure produces these files:
CLEANFILES = 85.nc 86.nc 87.nc 88.nc 89.nc big.nc foo.nc h0001.nc h0002.nc h0003.nc in.nc in_grp.nc in_rec_zero.nc ../src/nco_c++/in.nc

MAINTAINERCLEANFILES = Makefile.in

test: check


