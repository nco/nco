# $Header: /data/zender/nco_20150216/nco/bld/nco.spec,v 1.20 2002-06-17 00:41:08 zender Exp $
# Purpose: RPM spec file for NCO
# Usage: 
# Before nco.spec is invoked (with 'rpm -ba nco.spec'), the source tarball 
# nco-2.5.0 must be in the directory /usr/src/redhat/SOURCES
# After RPMs are built, upload them to RedHat's contrib area
# ncftpput incoming.redhat.com /libc6 /usr/src/redhat/SRPMS/nco-2.5.0-1.src.rpm /usr/src/redhat/RPMS/i386/nco-2.5.0-1.i386.rpm 
# ncftpput download.sourceforge.net /incoming /usr/src/redhat/SRPMS/nco-2.5.0-1.src.rpm /usr/src/redhat/RPMS/i386/nco-2.5.0-1.i386.rpm 
# ncftpput dust.ps.uci.edu /pub/zender/nco /usr/src/redhat/SRPMS/nco-2.5.0-1.src.rpm /usr/src/redhat/RPMS/i386/nco-2.5.0-1.i386.rpm 

Summary: Arithmetic and metadata operators for netCDF and HDF4 files
Name: nco
Version: 2.5.0
# Release: refers to version of nco.spec for this version of NCO
Release: 1
Copyright: GPL
Group: Applications/Scientific
Source: ftp://nco.sourceforge.net/pub/nco/nco-2.5.0.tar.gz
URL: http://nco.sourceforge.net
# Distribution: None in particular
Vendor: Charlie Zender
Requires: netcdf
Packager: Charlie Zender <zender@uci.edu>
# Prefix tag is required to allow installation to be relocatable
Prefix: /usr

%description
The netCDF Operators, or NCO, are a suite of programs known as
operators. The operators facilitate manipulation and analysis of
self-describing data stored in the netCDF or HDF4 formats, which are
freely available (http://www.unidata.ucar.edu/packages/netcdf and
http://hdf.ncsa.uiuc.edu, respectively). Each NCO operator (e.g., 
ncks) takes netCDF or HDF4 input file(s), performs an operation (e.g.,
averaging, hyperslabbing, or renaming), and outputs a processed netCDF
file. Although most users of netCDF and HDF data are involved in
scientific research, these data formats, and thus NCO, are generic and
are equally useful in fields like finance. The NCO User's Guide
illustrates NCO use with examples from the field of climate modeling
and analysis. The NCO homepage is http://nco.sourceforge.net

%prep
rm -rf ${RPM_BUILD_DIR}/nco-2.5.0
tar xvzf ${RPM_SOURCE_DIR}/nco-2.5.0.tar.gz

%build
cd nco-2.5.0/bld
/usr/bin/env MY_BIN_DIR=/usr/bin MY_DOC_DIR=/usr/doc/nco-2.5.0 make

# make data
# Use make tst only when connected to the Internet
# make tst

%install

%files
#%doc README
#%doc ChangeLog
#%doc VERSION
#%doc TODO
#%doc index.shtml
#%doc nco_news.shtml
/usr/bin/ncap
/usr/bin/ncatted
/usr/bin/ncdiff
/usr/bin/ncea
/usr/bin/ncecat
/usr/bin/ncflint
/usr/bin/ncks
/usr/bin/ncra
/usr/bin/ncrcat
/usr/bin/ncrename
/usr/bin/ncwa

%clean
/bin/rm -r nco-2.5.0
