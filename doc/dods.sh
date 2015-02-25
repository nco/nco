#!/bin/sh

# $Header$

# Purpose: Install DODS prior to building NCO as DODS-enabled clients
# NB: OPeNDAP/DODS netCDF client library versions 3.4.X are obsolete
# NCO versions > 3.0.1 support OPeNDAP client library versions > 3.5.1
# This file is provided for supporting older NCO with older DODS libraries
# See file opendap.sh for up-to-date instructions

# Testing: 
# NCO/SDO DODS server/repositories are at 
# soot.ess.uci.edu:/var/www/html/dodsdata
# dust.ess.uci.edu:/var/www/html/dodsdata
# http://soot.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata/
# http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata/

# Example DODS commands:
# ncra -O -C -l /tmp -p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata in.nc in.nc ~/foo.nc
# ncwa -O -C -a lat,lon,time -d lon,-10.,10. -d lat,-10.,10. -l /tmp -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface pres.sfc.1969.nc ~/foo.nc

# Usage: 
# Set installation directory below ($DODS_ROOT)
# ~/nco/doc/dods.sh
# Default invocation retrieves, builds, and installs DODS in ${DATA}/tmp 

# ${DATA}/tmp exists on most machines, and equals /tmp when ${DATA} is not defined
cd ${DATA}/tmp
# Remove debris from last DODS install
/bin/rm -r -f DODS*
# Get about 2.5 more MB than we need
wget "ftp://ftp.unidata.ucar.edu/pub/dods/DODS-3.4/source/DODS*.tar.gz"
# Assume tar is GNU tar, otherwise must gunzip tarballs first
# NCO only needs these packages pre-installed to enable NCO as DODS clients...
tar xvzf DODS-dap-3.4.?.tar.gz      # Provides libdap++.a
tar xvzf DODS-nc3-dods-3.4.?.tar.gz # Provides libnc-dods.a
tar xvzf DODS-packages-3.4.?.tar.gz # Provides libcurl.a, libxml2.a
# ...or, alternatively, build all DODS files including server, Matlab, HDF, ....
#for fl in `ls DODS*.tar.gz` ; do
#    tar xvzf ${fl}
#done

# Copy patched code with gcc-3.4-compliant fixes before building
scp dust.ess.uci.edu:/data/zender/tmp/DODS/src/dap-3.4.5/RValue.cc ${DATA}/tmp/DODS/src/dap-3.4.5
scp dust.ess.uci.edu:/data/zender/tmp/DODS/src/dap-3.4.5/Byte.cc ${DATA}/tmp/DODS/src/dap-3.4.5
scp dust.ess.uci.edu:/data/zender/tmp/DODS/src/dap-3.4.5/Operators.h ${DATA}/tmp/DODS/src/dap-3.4.5

# Go to common source directory shared by all DODS packages
cd ${DATA}/tmp/DODS
# Set permanent installation directory (/tmp is not permanent!)
#DODS_ROOT=/usr/local # For server (rather than private) installs
export DODS_ROOT=`pwd` 
# Set compiler environment variables CC and CXX before executing this
# You cannot do it on the command line like with NCO 
# On Linux, it defaults to gcc and g++
# On Opteron, may want to set 64-bit?
# 20041222: DODS configure scripts do not recognize GNU_TRP=x86_64-unknown-linux-gnu
# CFLAGS='-m64' CXXFLAGS='-m64' ./configure --prefix=${DODS_ROOT}
CC='gcc-3.4' CXX='g++-3.4' CXXCPP='/usr/bin/cpp' ./configure --prefix=${DODS_ROOT}
# Build necessary libraries
make
# 'make' usually fails because we did not extract (and build) all DODS sources
# This is fine since 'make' fails only after building the libraries that NCO needs
# If 'make' works, install DODS libraries in ${DODS_ROOT}
# make install

# Keep DODS libraries in ${DODS_ROOT} so NCO finds them and in the darkness binds them 
printf "DODS-enabled NCO may now be built. To do so, set environment variable\nDODS_ROOT=$DODS_ROOT\nand then re-build NCO.\n"
