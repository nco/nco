if BUILD_NCAP 
  NCAP=ncap
else
  NCAP=
endif 

if TEST_DODS
  DODS_TESTS=test2.sh
else
  DODS_TESTS=
endif

if NCO_NEED_GETOPT_LONG
  NCO_GETOPT=nco_getopt.c
else
  NCO_GETOPT=
endif

bin_PROGRAMS = ${NCAP} ncatted ncbo ncecat ncflint ncks ncra ncrename ncwa

AM_YFLAGS = -d

ncap_SOURCES = ncap_utl.c ncap.c ncap_yacc.y ncap_lex.l 
ncap_LDADD = libnco.la

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

ncra_SOURCES = ncra.c
ncra_LDADD = libnco.la

ncrename_SOURCES = ncrename.c
ncrename_LDADD = libnco.la

ncwa_SOURCES = ncwa.c
ncwa_LDADD = libnco.la

HEADER_SRC = libnco.h nco_cnf_dmn.h nco_fl_utl.h nco_omp.h nco_sng_utl.h \
ncap.h nco_cnf_typ.h nco_pck.h nco_var_avg.h nco_rth_flt.h \
ncap_yacc.h nco_cnv_arm.h nco_lmt.h nco_prn.h nco_var_lst.h \
nco.h nco_cnv_csm.h nco_lst_utl.h nco_rec_var.h nco_var_rth.h \
nco_att_utl.h nco_ctl.h nco_mmr.h nco_rth_utl.h nco_var_scv.h \
nco_bnr.h nco_dbg.h nco_msa.h nco_mss_val.h nco_scl_utl.h nco_var_utl.h \
nco_cln_utl.h nco_dmn_utl.h nco_netcdf.h nco_scm.h nco_getopt.h

lib_LTLIBRARIES = libnco.la

libnco_la_LDFLAGS = -version-info 1:0:0

libnco_la_SOURCES = nco_att_utl.c nco_bnr.c nco_cln_utl.c nco_cnf_dmn.c \
nco_cnf_typ.c nco_cnv_arm.c nco_rth_flt.c ${NCO_GETOPT} \
nco_cnv_csm.c nco_ctl.c nco_dbg.c nco_dmn_utl.c nco_fl_utl.c nco_lmt.c \
nco_lst_utl.c nco_mmr.c nco_msa.c nco_mss_val.c nco_netcdf.c nco_omp.c nco_pck.c \
nco_prn.c nco_rec_var.c nco_rth_utl.c nco_scl_utl.c nco_scm.c nco_sng_utl.c \
nco_var_avg.c nco_var_lst.c nco_var_rth.c nco_var_scv.c nco_var_utl.c $(HEADER_SRC)

TESTS = test1.sh ${DODS_TESTS}

EXTRA_DIST = nco_getopt.c

install-exec-hook:
#	ln -s -f $(DESTDIR)$(bindir)/ncbo $(DESTDIR)$(bindir)/ncdiff
#	ln -s -f $(DESTDIR)$(bindir)/ncra $(DESTDIR)$(bindir)/ncea
#	ln -s -f $(DESTDIR)$(bindir)/ncra $(DESTDIR)$(bindir)/ncrcat
	cd $(DESTDIR)$(bindir) && ln -sf ncbo ncdiff
	cd $(DESTDIR)$(bindir) && ln -sf ncra ncea
	cd $(DESTDIR)$(bindir) && ln -sf ncra ncrcat

uninstall-hook:
#	rm -f $(DESTDIR)$(bindir)/ncdiff
#	rm -f $(DESTDIR)$(bindir)/ncea
#	rm -f $(DESTDIR)$(bindir)/ncrcat
	cd $(DESTDIR)$(bindir) && rm -f ncdiff ncea ncrcat
