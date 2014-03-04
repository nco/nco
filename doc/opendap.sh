#!/bin/sh

# $Header: /data/zender/nco_20150216/nco/doc/opendap.sh,v 1.17 2014-03-04 22:44:15 zender Exp $

# Purpose: Install OPeNDAP prior to building NCO as DAP-enabled clients

# NB: OPeNDAP/DODS netCDF client library versions 3.4.X are obsolete
# NCO versions > 3.0.1 support OPeNDAP client library versions > 3.5.1
# This file contains up-to-date instructions
# See file dods.sh for supporting older NCO with older DODS libraries

# Terminology: 
# Protocol is DAP, its free implementation is OPeNDAP (analogous to SSH/OpenSSH)
# DODS-terminology is being obsoleted in favor of DAP-terminology
# Transition is not complete yet, so hybrid is necessary, e.g., DAP_ROOT

# Testing: 
# NCO/SDO server/repositories are accessible via DAP at
# http://esmf.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata/
# http://soot.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata/
# http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata/
# Physical locations of these repositories is
# soot.ess.uci.edu:/var/www/html/dodsdata
# dust.ess.uci.edu:/var/www/html/dodsdata
# esmf.ess.uci.edu:/data/dodsdata
# WWW of repositories is 
# http://dust.ess.uci.edu/dodsdata/
# http://esmf.ess.uci.edu/dodsdata/

# Example DAP-enabled commands:
# ncks -M http://dust.ess.uci.edu/nco/in.nc # wget
# ncks -M -p http://dust.ess.uci.edu/nco in.nc # wget
# ncks -M -p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata in.nc # DAP (borken server)
# ncks -M -p http://thredds-test.ucar.edu/thredds/dodsC/testdods in.nc # DAP
# ncks -M -p http://thredds-test.ucar.edu/thredds/dodsC/testdods in_4.nc # DAP
# ncks -M -p http://thredds.ucar.edu/thredds/catalog/grib/NCEP/GFS/Global_2p5deg/files/GFS_Global_2p5deg_20140304_0000 # DAP
# ncra -O -C -D 3 -p http://thredds-test.ucar.edu/thredds/dodsC/testdods in.nc in.nc ~/foo.nc
# ncwa -O -C -D 3 -a lat,lon,time -d lon,-10.,10. -d lat,-10.,10. -l /tmp -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface pres.sfc.1969.nc ~/foo.nc
# ncks -D 3 http://eclipse.ncdc.noaa.gov:9090/thredds/dodsC/isccp-d1/2001/ISCCP.D1.1.GLOBAL.2001.01.01.0000.GPC -d lat,1,2 -d lon,1,2 ~/test.nc 
# ncks -M -p http://username:password@esgcet.llnl.gov/cgi-bin/dap-cgi.py/ipcc4/sresa1b/ncar_ccsm3_0 pcmdi.ipcc4.ncar_ccsm3_0.sresa1b.run1.atm.mo.xml

# Usage: 
# Set installation directory below ${DAP_ROOT}
# ~/nco/doc/opendap.sh
# Default invocation retrieves, builds, and installs DAP in ${DATA}/tmp 

# ${DATA}/tmp exists on most machines, and equals /tmp when ${DATA} is not defined
cd ${DATA}/tmp
# Remove debris from last DAP install
/bin/rm -r -f DODS* libnc-dap* libdap*
wget ftp://ftp.unidata.ucar.edu/pub/opendap/source/libdap-3.7.3.tar.gz
wget ftp://ftp.unidata.ucar.edu/pub/opendap/source/libnc-dap-3.7.0.tar.gz
# Resolve DAP dependencies on external libraries
apt-file search libcurl.a
sudo aptitude install libcurl3-dev          
apt-file search libxml2.a
sudo aptitude install libxml2-dev
# Assume tar is GNU tar, otherwise must gunzip tarballs first
# NCO only needs these packages pre-installed to enable NCO as DAP clients...
tar xvzf libdap-3.7.3.tar.gz # Provides libdap++.a
tar xvzf libnc-dap-3.7.0.tar.gz # Provides libnc-dap.a

# Set permanent installation directory (/tmp is not permanent!)
export DAP_ROOT='/usr/local' # For server (rather than private) installs
#export DAP_ROOT=`pwd` 
sudo /bin/rm -r ${DAP_ROOT}/include/libdap ${DAP_ROOT}/include/libnc-dap
sudo /bin/rm ${DAP_ROOT}/lib/libdap* ${DAP_ROOT}/lib/libnc-dap*
# Go to common source directory shared by all DAP packages
cd ${DATA}/tmp/libdap-3.7.3
# Set compiler environment variables CC and CXX before building
# export OBJECT_MODE='64' # AIX-specific hack
# AIX:
# CC='xlc_r' CFLAGS='-qsmp=omp' CXX='xlC_r' LDFLAGS='-brtl' FC='g95' ./configure --prefix=${DAP_ROOT}
# LINUX:
FC='gfortran' ./configure --prefix=${DAP_ROOT}
#FC='g95' ./configure --prefix=${DAP_ROOT}
# CC='icc -std=c99' CFLAGS='-O3 -g -mp -w1' CPPFLAGS='' CXX='icpc' CXXFLAGS='-O3 -g -mp -w1' FC='ifort' FFLAGS='-O3 -g -axW -mp -vec_report1' F90FLAGS='-O3 -g -axW -mp -vec_report1' ./configure --prefix=${DAP_ROOT}
# LINUXAMD64: Explicitly set 64-bit?
# CFLAGS='-m64' CXXFLAGS='-m64' ./configure --prefix=${DAP_ROOT}
# Build necessary libraries
make
sudo make install
cd ${DATA}/tmp/libnc-dap-3.7.0
FC='gfortran' ./configure --prefix=${DAP_ROOT}
#FC='g95' ./configure --prefix=${DAP_ROOT}
make
sudo make install

# Keep DAP libraries in ${DAP_ROOT} so NCO finds them and in the darkness binds them 
printf "OPeNDAP installed. DAP-enabled NCO may now be built.\n"
