# $Header: /data/zender/nco_20150216/nco/bld/nco.spec,v 1.9 2000-08-28 20:18:41 zender Exp $
# Purpose: RPM spec file for NCO
# Usage: 
# Before nco.spec is invoked (with 'rpm -ba nco.spec'), the source tarball 
# nco-1.2.1 must be in the directory /usr/src/redhat/SOURCES
# After RPMs are built, upload them to RedHat's contrib area
# ncftpput incoming.redhat.com /libc6 /usr/src/redhat/SRPMS/nco-${NCO_VRS}-*.src.rpm /usr/src/redhat/RPMS/i386/nco-${NCO_VRS}-*.i386.rpm 
# ncftpput dust.ps.uci.edu /pub/zender/nco /usr/src/redhat/SRPMS/nco-${NCO_VRS}-*.src.rpm /usr/src/redhat/RPMS/i386/nco-${NCO_VRS}-*.i386.rpm 

Summary: Arithmetic and metadata operators for netCDF and HDF4 files
Name: nco
Version: 1.2
Release: 1
Copyright: GPL
Group: Applications/Scientific
Source: ftp://nco.sourceforge.net/pub/nco/nco-1.2.1.tar.gz
URL: http://nco.sourceforge.net
# Distribution: None in particular
Vendor: Charlie Zender
Packager: Charlie Zender <zender@uci.edu>
# Prefix tag is required to allow installation to be relocatable
Prefix: /usr

%description
The netCDF Operators, or NCO, are a suite of programs known as
operators. The operators facilitate manipulation and analysis of
self-describing data stored in the netCDF or HDF formats, which are
freely available (http://www.unidata.ucar.edu/packages/netcdf and
http://hdf.ncsa.uiuc.edu, respectively). Each NCO operator (e.g., 
ncks) takes netCDF or HDF input file(s), performs an operation (e.g.,
averaging, hyperslabbing, or renaming), and outputs a processed netCDF
file. Although most users of netCDF and HDF data are involved in
scientific research, these data formats, and thus NCO, are generic and
are equally useful in fields like finance. The NCO User's Guide
illustrates NCO use with examples from the field of climate modeling
and analysis. The NCO homepage is http://nco.sourceforge.net

%prep
rm -rf $RPM_BUILD_DIR/nco-1.2.1
tar xvzf $RPM_SOURCE_DIR/nco-1.2.1.tar.gz

%build
cd nco-1.2.1/bld
/usr/bin/env MY_BIN_DIR=/usr/bin MY_DOC_DIR=/usr/doc/nco-1.2.1 make

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
/usr/bin/ncatted
/usr/bin/ncdiff
/usr/bin/ncea
/usr/bin/ncecat
/usr/bin/ncflint
/usr/bin/ncks
/usr/bin/ncra
/usr/bin/ncrename
/usr/bin/ncwa

%clean
/bin/rm -r nco-1.2.1
