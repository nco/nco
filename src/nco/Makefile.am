# $Header$ -*-makefile-*-

if NCO_NEED_GETOPT_LONG
NCO_GETOPT_C=nco_getopt.c
NCO_GETOPT_H=nco_getopt.h
else
NCO_GETOPT_C=
NCO_GETOPT_H=
endif

if ENABLE_MPI
bin_PROGRAMS = ncatted ncbo ncecat ncflint ncks ncpdq ncra ncrename mpncbo mpncecat mpncflint mpncpdq mpncra mpncwa ncwa
else
bin_PROGRAMS = ncatted ncbo ncecat ncflint ncks ncpdq ncra ncrename ncwa
endif

AM_YFLAGS = -d --name-prefix=nco_yy

ncatted_SOURCES = ncatted.c
ncatted_LDADD = libnco.la

ncbo_SOURCES = ncbo.c
ncbo_LDADD = libnco.la

ncecat_SOURCES = ncecat.c
ncecat_LDADD = libnco.la

ncflint_SOURCES = ncflint.c
ncflint_LDADD = libnco.la

ncks_SOURCES = ncks.c
ncks_LDADD = libnco.la

ncpdq_SOURCES = ncpdq.c
ncpdq_LDADD = libnco.la

ncra_SOURCES = ncra.c
ncra_LDADD = libnco.la

ncrename_SOURCES = ncrename.c
ncrename_LDADD = libnco.la

mpncbo_SOURCES = mpncbo.c
mpncbo_LDADD = libnco.la

mpncecat_SOURCES = mpncecat.c
mpncecat_LDADD = libnco.la

mpncflint_SOURCES = mpncflint.c
mpncflint_LDADD = libnco.la

mpncpdq_SOURCES = mpncpdq.c
mpncpdq_LDADD = libnco.la

mpncra_SOURCES = mpncra.c
mpncra_LDADD = libnco.la

ncwa_SOURCES = ncwa.c ncap_utl.c ncap_lex.l 
ncwa_LDADD = libnco.la

mpncwa_SOURCES = mpncwa.c ncap_utl.c ncap_lex.l 
mpncwa_LDADD = libnco.la

HEADER_SRC = \
libnco.h \
${NCO_GETOPT_H} \
nco_kd.h \
nco_att_utl.h \
nco_aux.h \
nco_bnr.h \
nco_cln_utl.h \
nco_cnf_dmn.h \
nco_cnf_typ.h \
nco_cnk.h \
nco_cnv_arm.h \
nco_cnv_csm.h \
nco_crt.h \
nco_ctl.h \
nco_dbg.h \
nco_dmn_utl.h \
nco_fl_utl.h \
nco_flt.h \
nco_grp_utl.h \
nco_grp_trv.h \
nco_lmt.h \
nco_lst_utl.h \
nco_map.h \
nco_md5.h \
nco_mmr.h \
nco_mpi.h \
nco_msa.h \
nco_mss_val.h \
nco_mta.h \
nco_netcdf.h \
nco_omp.h \
nco_pck.h \
nco_ply.h \
nco_ply_lst.h \
nco_ppc.h \
nco_prn.h \
nco_rec_var.h \
nco_rgr.h \
nco_rth_flt.h \
nco_rth_utl.h \
nco_scl_utl.h \
nco_s1d.h \
nco_scm.h \
nco_sld.h \
nco_sph.h \
nco_sng_utl.h \
nco_srm.h \
nco_typ.h \
nco_uthash.h \
nco_var_avg.h \
nco_var_lst.h \
nco_var_rth.h \
nco_var_scv.h \
nco_var_utl.h

lib_LTLIBRARIES = libnco.la

# This creates version-stamped shared-library, e.g., libnco-2.9.4.so
# NB: VERSION here refers to argument to AC_INIT() in configure.ac
libnco_la_LDFLAGS = -release @VERSION@

# Fedora Core nco.spec in nco-3.1.2 wants to install headers
# Fedora Core nco.spec in nco-3.9.7 still wants to install headers
# Maybe smart/required for -devel packages?
# To my knowledge, though, only NCO programs use libnco
# For this reason Debian packages should not (yet) install NCO headers
# Remove following line until learn how to prevent Debian from installing headers
#include_HEADERS = $(HEADER_SRC)

libnco_la_SOURCES = \
${HEADER_SRC} \
${NCO_GETOPT_C} \
nco_att_utl.c \
nco_aux.c \
nco_bnr.c \
nco_cln_utl.c \
nco_cnf_dmn.c \
nco_cnf_typ.c \
nco_cnk.c \
nco_cnv_arm.c \
nco_cnv_csm.c \
nco_crt.c \
nco_ctl.c \
nco_dbg.c \
nco_dmn_utl.c \
nco_fl_utl.c \
nco_flt.c \
nco_grp_utl.c \
nco_grp_trv.c \
nco_kd.c \
nco_lmt.c \
nco_lst_utl.c \
nco_map.c \
nco_md5.c \
nco_mmr.c \
nco_msa.c \
nco_mss_val.c \
nco_mta.c \
nco_netcdf.c \
nco_omp.c \
nco_pck.c \
nco_ply.c \
nco_ply_lst.c \
nco_ppc.c \
nco_prn.c \
nco_rec_var.c \
nco_rgr.c \
nco_rth_flt.c \
nco_rth_utl.c \
nco_scl_utl.c \
nco_s1d.c \
nco_scm.c \
nco_sld.c \
nco_sng_utl.c \
nco_sph.c \
nco_srm.c \
nco_var_avg.c \
nco_var_lst.c \
nco_var_rth.c \
nco_var_scv.c \
nco_var_utl.c

TESTS = ${DAP_TESTS}

EXTRA_DIST = ${NCO_GETOPT_C}

DISTCLEANFILES = ncap_lex.c

install-exec-hook:
	cd $(DESTDIR)$(bindir) && ln -s -f ncbo ncdiff || (rm -f ncdiff && ln -s -f ncbo ncdiff)
	cd $(DESTDIR)$(bindir) && ln -s -f ncra ncea || (rm -f ncea && ln -s -f ncra ncea)
	cd $(DESTDIR)$(bindir) && ln -s -f ncra nces || (rm -f nces && ln -s -f ncra nces)
	cd $(DESTDIR)$(bindir) && ln -s -f ncra ncrcat || (rm -f ncrcat && ln -s -f ncra ncrcat)
if ENABLE_MPI
	cd $(DESTDIR)$(bindir) && ln -s -f mpncbo mpncdiff || (rm -f mpncdiff && ln -s -f mpncbo mpncdiff)
	cd $(DESTDIR)$(bindir) && ln -s -f mpncra mpncea || (rm -f mpncea && ln -s -f mpncra mpncea)
	cd $(DESTDIR)$(bindir) && ln -s -f mpncra mpnces || (rm -f mpnces && ln -s -f mpncra mpnces)
	cd $(DESTDIR)$(bindir) && ln -s -f mpncra mpncrcat || (rm -f mpncrcat && ln -s -f mpncra mpncrcat)
endif

uninstall-hook:
	cd $(DESTDIR)$(bindir) && rm -f ncdiff ncea nces ncrcat mpncdiff mpncea mpnces mpncrcat

CLEANFILES=ncdiff ncea nces ncrcat mpncdiff mpncea mpnces mpncrcat

