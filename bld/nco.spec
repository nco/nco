# $Header: /data/zender/nco_20150216/nco/bld/nco.spec,v 1.2 2000-03-06 08:04:46 zender Exp $
# Purpose: RPM spec file for NCO
# Usage: 
# Before nco.spec is invoked (with 'rpm -ba nco.spec'), the source tarball 
# nco-1.1.46 must be in the directory /usr/src/redhat/SOURCES

Summary: A suite of arithmetic and metadata operators for netCDF files
Name: nco
Version: 1.1.46
Release: 1
Copyright: GPL
Group: Applications/Scientific
Source: ftp://ftp.cgd.ucar.edu/pub/zender/nco/nco-1.1.46.tar.gz
URl: http://www.cgd.ucar.edu/cms/nco
# Distribution: None in particular
Vendor: Zendor
Packager: Charlie Zender <zender@uci.edu>
# Prefix tag is required to allow installation to be relocatable
Prefix: /usr

%description
The netCDF Operators, or NCO, is a suite of programs known as
operators. The operators facilitate manipulation and analysis of
scientific data in netCDF or HDF format
(http://www.unidata.ucar.edu/packages/netcdf and
http://hdf.ncsa.uiuc.edu, respectively).  Each NCO operator (e.g.,
ncks) takes netCDF or HDF input file(s), performs an operation (e.g.,
averaging, hyperslabbing, or renaming), and outputs a processed netCDF
file.  The NCO User's Guide illustrates their use with examples from
the field of climate modeling and analysis.

%prep
rm -rf $RPM_BUILD_DIR/nco-1.1.46
tar xvzf $RPM_SOURCE_DIR/nco-1.1.46.tar.gz

%build
cd nco-1.1.46/bld
/usr/bin/env MY_BIN_DIR=/usr/bin MY_DOC_DIR=/usr/doc/nco-1.1.46 make
/usr/bin/env MY_BIN_DIR=/usr/bin MY_DOC_DIR=/usr/doc/nco-1.1.46 make rpmdoc

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
/usr/bin/ncks
/usr/bin/ncatted
/usr/bin/ncdiff
/usr/bin/ncecat
/usr/bin/ncflint
/usr/bin/ncrename
/usr/bin/ncwa

%clean
/bin/rm -r nco-1.1.46
