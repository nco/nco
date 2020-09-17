# $Header$ -*-makefile-*-

# 20151001: Sub-folder is by default, and not when configure --disable-docs is invoked
SUBDIRS = data src man @DOC_FOLDER@

# Separately add desired files from nco/bld/ to distribution
# Autoconf does not know about the bld directory 
# In particular, nco/bld/Makefile was created manually, not by autoconf
bld_extras = bld/Makefile bld/pvmgetarch bld/nco.spec
EXTRA_DIST = $(bld_extras)
ACLOCAL_AMFLAGS = -I m4

# 20200917 Attempt to improve TAG-generation
ETAGS_ARGS = CMakeLists.txt Makefile.am configure.ac
TAGS_DEPENDENCIES = CMakeLists.txt Makefile.am configure.ac

test:	tst
tst:
	$(MAKE) check
	cd data && $(MAKE) in_4c.nc && cd ..
	cd bm && ./nco_bm.pl --regress

regress:	rgr
rgr:
	cd bm && ./nco_bm.pl --regress

regress-mpi:	rgr_mpi
rgr_mpi:
	cd bm && ./nco_bm.pl --regress --mpi_prc=2
