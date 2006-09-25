#!/bin/sh

# Purpose: Install HDF5 and netCDF4 for use by NCO

# Source: Updated from latest working stanza of ~zender/linux/sw/netcdf.txt

#***********************************************************************          
# 20060924: ashes.ess.uci.edu Debian Ubuntu 6.05 Dapper Drake gcc/g95 netcdf-4.0-alpha16
#***********************************************************************
cd ${DATA}/tmp
wget -c ftp://ftp.ncsa.uiuc.edu/HDF/pub/outgoing/hdf5/snapshots/hdf5-1.7.51.tar.bz2
wget -c ftp://ftp.unidata.ucar.edu/pub/netcdf/netcdf-4/netcdf-4.0-alpha16.tar.gz
sudo /bin/rm -r ${DATA}/tmp/hdf5-1.7.51
tar xvjf hdf5-1.7.51.tar.bz2
# Build HDF5 first ('make install' installs locally)
cd ${DATA}/tmp/hdf5-1.7.51
make distclean
bash # New shell level keeps environment variables sandboxed
export GNU_TRP=`~/nco/autobld/config.guess`
export CC='gcc'
export CXX='g++'
export CPPFLAGS='-DNDEBUG -DpgiFortran'
export CFLAGS='-O -g'
export FC='g95'
export F90='g95'
export FFLAGS='-O -g -fno-second-underscore'
export F90FLAGS='-O -g -fno-second-underscore'
# Install HDF5 and netCDF4 in same directory for easier paths
export NETCDF4_ROOT=/usr/local/netcdf4
export HDF5_ROOT=${NETCDF4_ROOT}
# HDF5 builds here unless otherwise specified
# HDF5_ROOT=${DATA}/tmp/hdf5-1.7.51/hdf5
# Do not attempt to build Fortran interface because g95 not understood
./configure --prefix=${HDF5_ROOT} --disable-fortran
make
make check
sudo make install
# Do same for netcdf4
cd ${DATA}/tmp
/bin/rm -r ${DATA}/tmp/netcdf-4.0-alpha16
sudo /bin/rm -r ${NETCDF4_ROOT}/include/netcdf.* ${NETCDF4_ROOT}/lib/libnetcdf.* 
tar -xzvf netcdf-4.0-alpha16.tar.gz
cd ${DATA}/tmp/netcdf-4.0-alpha16
make distclean
# Build parallel netCDF with LAM MPI
CPPFLAGS='-I/usr/include/lam' ./configure --prefix=${NETCDF4_ROOT} --enable-netcdf-4 --disable-parallel --disable-shared --with-hdf5=${HDF5_ROOT} --disable-cxx --disable-f90 --disable-f77 > netcdf.configure.${GNU_TRP}.foo 2>&1
# Build parallel netCDF with MPICH2 MPI
./configure --prefix=${NETCDF4_ROOT} --enable-netcdf-4 --disable-parallel --disable-shared --with-hdf5=${HDF5_ROOT} --disable-cxx --disable-f90 --disable-f77 > netcdf.configure.${GNU_TRP}.foo 2>&1
make
make check
sudo make install
exit # Return to previous shell level
# NB: Fortran symbols do get munged into libnetcdf.a somehow
sudo cp ${DATA}/tmp/netcdf-4.0-alpha16/libsrc4/.libs/libnetcdf.a /usr/local/netcdf4/lib/libnetcdf.a.gcc-g95
# netCDF4 Fortran90 interfaces not yet available
#sudo cp ${DATA}/tmp/netcdf-4.0-alpha16/f90/typesizes.mod /usr/local/netcdf4/include/typesizes.mod.g95
#sudo cp ${DATA}/tmp/netcdf-4.0-alpha16/f90/netcdf.mod /usr/local/netcdf4/include/netcdf.mod.g95
make clean
#***********************************************************************
# 20060924: ashes.ess.uci.edu Debian Ubuntu 6.05 Dapper Drake gcc/g95 netcdf-4.0-alpha16
#***********************************************************************
