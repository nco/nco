# $Header: /data/zender/nco_20150216/nco/data/Makefile.am,v 1.20 2013-08-10 21:00:02 pvicente Exp $ -*-makefile-*-

test_data = big.nc in.nc in_rec_zero.nc

test_data_netCDF4 = in_grp.nc in_grp_1.nc in_grp_2.nc in_grp_3.nc cmip5.nc obs.nc 

# netCDF build rules
%.nc : %.cdl
	-@PATH_TO_NCGEN@ -o $@ $< 

in_grp.nc : in_grp.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_grp_1.nc : in_grp_1.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_grp_2.nc : in_grp_2.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_1.nc : in_1.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_2.nc : in_2.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_grp_3.nc : in_grp_3.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

cmip5.nc : cmip5.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

obs.nc : obs.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

all: ${test_data} ${test_data_netCDF4}

EXTRA_DIST = big.cdl in.cdl in_grp.cdl in_grp_1.cdl in_grp_2.cdl in_grp_3.cdl in_1.cdl in_2.cdl cmip5.cdl obs.cdl buggy.cdl in_rec_zero.cdl ncap.in ncap.in2 ncap2.in tst.nco

# Automake manual section 14:
# If configure built it, 'distclean' should delete it
# If make built it, 'clean' should delete it
DISTCLEANFILES = foo*

# Build procedure produces these files:
CLEANFILES = 85.nc 86.nc 87.nc 88.nc 89.nc big.nc foo.nc h0001.nc h0002.nc h0003.nc in.nc in_grp.nc in_grp_1.nc in_grp_2.nc in_grp_3.nc cmip5.nc obs.nc buggy.nc in_rec_zero.nc ../src/nco_c++/in.nc

MAINTAINERCLEANFILES = Makefile.in

test: check


