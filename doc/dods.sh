# $Header: /data/zender/nco_20150216/nco/doc/dods.sh,v 1.2 2003-12-23 18:42:36 zender Exp $

# Purpose: Demonstrate how to install DODS prior to building NCO as DODS-enabled clients

# Usage: Run script at command line with
# dods.sh
# or edit and cut and paste to fit

cd ${DATA}/tmp
# This gets about 2.5 more MB than we need
wget "ftp://ftp.unidata.ucar.edu/pub/dods/DODS-3.4/RC6/source/DODS*.tar.gz"
# Build all DODS files including server, Matlab, HDF, ....
#for fl in `ls DODS*.tar.gz` ; do
#    tar xvzf ${fl}
#done
# ...or only build these packages for NCO DODS clients:
tar xvzf DODS-dap-3.4.?.tar.gz
tar xvzf DODS-nc3-dods-3.4.?.tar.gz
tar xvzf DODS-packages-3.4.?.tar.gz

# Go to source directory
cd DODS
# Set installation directory
#DODS_ROOT=/usr/local # For server (rather than private) installs
DODS_ROOT=`pwd` 
# Set compiler environment variables CC and CXX before executing this
# You cannot do it on the command line like with NCO 
# On Linux, it defaults to gcc and g++
./configure --prefix=${DODS_ROOT}
# Build necessary libraries and install them in /usr/local
make
# This 'make' fails because we did not extract all DODS source, and that's OK
# If it works then do a 
# make install

# Keep DODS libraries in ${DODS_ROOT} so NCO finds them and in the darkness binds them 
printf "DODS-enabled NCO may now be built. To do so, set the environment variable\nDODS_ROOT=$DODS_ROOT\nand then re-build NCO.\n"
