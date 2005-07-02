#!/bin/sh

# $Header: /data/zender/nco_20150216/nco/doc/opendap.sh,v 1.1 2005-07-02 21:48:34 zender Exp $

# Purpose: Install OPeNDAP prior to building NCO as DAP-enabled clients
# Terminology: 
# DAP is the protocol, OPeNDAP is the free server, analogous to SSH and OpenSSH
# DODS-terminology is being obsoleted in favor of DAP-terminology
# Transition is not complete yet, so hybrid is necessary, e.g., DODS_ROOT

# Testing: 
# NCO/SDO OPeNDAP server/repositories are at 
# soot.ess.uci.edu:/var/www/html/dodsdata
# dust.ess.uci.edu:/var/www/html/dodsdata
# http://soot.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata/
# http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata/

# Example DAP commands:
# ncra -O -C -l /tmp -p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata in.nc in.nc ~/foo.nc
# ncwa -O -C -a lat,lon,time -d lon,-10.,10. -d lat,-10.,10. -l /tmp -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface pres.sfc.1969.nc ~/foo.nc

# Usage: 
# Set installation directory below ${DODS_ROOT}
# ~/nco/doc/dap.sh
# Default invocation retrieves, builds, and installs DAP in ${DATA}/tmp 

# ${DATA}/tmp exists on most machines, and equals /tmp when ${DATA} is not defined
cd ${DATA}/tmp
# Remove debris from last DAP install
/bin/rm -r -f DODS* libnc-dap* libdap*
# Get about 2.5 more MB than we need
wget http://www.opendap.org/pub/3.5/source/libdap-3.5.1.tar.gz
wget http://www.opendap.org/pub/3.5/source/libnc-dap-3.5.1.tar.gz
# Resolve DAP dependencies on external libraries
apt-file search libcurl.a
sudo apt-get install libcurl3-dev          
apt-file search libxml2.a
sudo apt-get install libxml2-dev
# Assume tar is GNU tar, otherwise must gunzip tarballs first
# NCO only needs these packages pre-installed to enable NCO as DAP clients...
tar xvzf libdap-3.5.1.tar.gz # Provides libdap++.a
tar xvzf libnc-dap-3.5.1.tar.gz # Provides libnc-dap.a

# Go to common source directory shared by all DAP packages
cd ${DATA}/tmp/libdap-3.5.1
# Set permanent installation directory (/tmp is not permanent!)
DODS_ROOT=/usr/local # For server (rather than private) installs
#export DODS_ROOT=`pwd` 
# Set compiler environment variables CC and CXX before building
# On Opteron, may want to set 64-bit?
# CFLAGS='-m64' CXXFLAGS='-m64' ./configure --prefix=${DODS_ROOT}
CC='gcc' CXX='g++' FC='g95' ./configure --prefix=${DODS_ROOT}
# Build necessary libraries
make
sudo make install
cd ${DATA}/tmp/libnc-dap-3.5.1
CC='gcc' CXX='g++' FC='g95' ./configure --prefix=${DODS_ROOT}
make
sudo make install

# Keep DAP libraries in ${DODS_ROOT} so NCO finds them and in the darkness binds them 
printf "DAP-enabled NCO may now be built. To do so, set environment variable\nDODS_ROOT=${DODS_ROOT}\nand then re-build NCO.\n"
