# $Header: /data/zender/nco_20150216/nco/data/Makefile.am,v 1.44 2015-01-20 18:44:52 zender Exp $ -*-makefile-*-

test_data = big.nc in.nc in_rec_zero.nc

test_data_netCDF4 = in_4c.nc in_grp.nc in_grp_1.nc in_grp_2.nc in_grp_3.nc in_grp_4.nc in_grp_5.nc in_grp_6.nc in_grp_7.nc cmip5.nc obs.nc mdl_1.nc mdl_2.nc mdl_3.nc hdn.nc mrd.nc

# netCDF build rules
%.nc : %.cdl
	-@PATH_TO_NCGEN@ -o $@ $< 

in_4c.nc : in.cdl
	-ncks -O -7 --cnk_dmn time,10 in.nc $@
# ncgen default chunksize for time = 4 MB produces ~200 MB in_4c.nc file. Avoid that!
#	-@PATH_TO_NCGEN@ -k hdf5-nc3 -o $@ $< 

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

in_grp_4.nc : in_grp_4.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_grp_5.nc : in_grp_5.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_grp_6.nc : in_grp_6.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

in_grp_7.nc : in_grp_7.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

cmip5.nc : cmip5.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

dsm.nc : dsm.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

hdn.nc : hdn.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

mrd.nc : mrd.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

obs.nc : obs.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

mdl_1.nc : mdl_1.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

mdl_2.nc : mdl_2.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

mdl_3.nc : mdl_3.cdl
	-@PATH_TO_NCGEN@ -k netCDF-4 -o $@ $< 

all: ${test_data} ${test_data_netCDF4}

EXTRA_DIST = big.cdl dsm.cdl hdn.cdl mrd.cdl in.cdl in_grp.cdl in_grp_1.cdl in_grp_2.cdl in_grp_3.cdl in_grp_4.cdl in_grp_5.cdl in_grp_6.cdl in_grp_7.cdl in_1.cdl in_2.cdl cmip5.cdl obs.cdl mdl_1.cdl buggy.cdl in_rec_zero.cdl ncap.in ncap.in2 ncap2.in tst.nco

# Automake manual section 14:
# If configure built it, 'distclean' should delete it
# If make built it, 'clean' should delete it
DISTCLEANFILES = foo*

# Build procedure produces these files:
CLEANFILES = 85.nc 86.nc 87.nc 88.nc 89.nc big.nc foo.nc h0001.nc h0002.nc h0003.nc in.nc in_1.nc in_2.nc in_grp.nc in_grp_1.nc in_grp_2.nc in_grp_3.nc in_grp_4.nc in_grp_5.nc in_grp_6.nc in_grp_7.nc cmip5.nc hdn.nc mrd.nc dsm.nc lrg_bm1.nc lrg_bm.nc obs.nc mdl.nc mdl2.nc mdl_1.nc mdl_2.nc mdl_3.nc buggy.nc in_rec_zero.nc ../src/nco_c++/in.nc

MAINTAINERCLEANFILES = Makefile.in

test: check


