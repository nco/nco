# $Header: /data/zender/nco_20150216/nco/doc/dods.sh,v 1.6 2004-03-12 01:02:18 zender Exp $

# Purpose: Install DODS prior to building NCO as DODS-enabled clients

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

# Go to common source directory shared by all DODS packages
cd DODS
# Set permanent installation directory (/tmp is not permanent!)
#DODS_ROOT=/usr/local # For server (rather than private) installs
DODS_ROOT=`pwd` 
# Set compiler environment variables CC and CXX before executing this
# You cannot do it on the command line like with NCO 
# On Linux, it defaults to gcc and g++
./configure --prefix=${DODS_ROOT}
# Build necessary libraries
make
# 'make' usually fails because we did not extract (and build) all DODS sources
# This is fine since 'make' fails only after building the libraries that NCO needs
# If 'make' works, install DODS libraries in ${DODS_ROOT}
# make install

# Keep DODS libraries in ${DODS_ROOT} so NCO finds them and in the darkness binds them 
printf "DODS-enabled NCO may now be built. To do so, set environment variable\nDODS_ROOT=$DODS_ROOT\nand then re-build NCO.\n"
