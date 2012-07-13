#!/bin/csh

#-------------------------------------------------------------------------
#
# Compute the size of extra padding in Netcdf-3 format files.
#
# 2012-jul-11	Original version.  By Dave Allured, NOAA/PSD/CIRES.
# 2012-jul-12	Use faster nccopy instead of ncdump/ncgen.
#		Thanks to Russ Rew of Unidata for the suggestion.
# 20120713 Modified by C. Zender to check performance of NCO --hdr_pad

cat > /dev/null << EOF
cd ~/nco/data
./hdr_pad.sh ~/nco/data/in.nc
ncks -O -h -3 --hdr_pad 100 ~/nco/data/in.nc ~/in_3.nc
ncks -O -h --64 --hdr_pad 100 ~/nco/data/in.nc ~/in_64.nc
ncks -O -h -4 --hdr_pad 100 ~/nco/data/in.nc ~/in_4.nc
./hdr_pad.sh ~/in_3.nc
./hdr_pad.sh ~/in_64.nc
./hdr_pad.sh ~/in_4.nc
EOF

# This method is slow and cumbersome for large files.  Also,
# it can not separately determine the sizes of the header pad
# and the data pad.  It should be replaced by direct parsing
# of the Netcdf-3 data structure, should anyone ever become
# motivated to write the necessary code.
#
# This method uses a temp file in the system /tmp directory.
# This requires that there is enough free disk space in /tmp
# for a full copy of the input file.
#
#-------------------------------------------------------------------------

if ( $#argv != 1 ) then
   echo Error, this command requires a single argument.
   echo Usage: hdr_pad file.nc
   exit 1
endif

set infile = "$1"
set temp1 = /tmp/hdr_pad.temp.$$.nc

# File diagnostics.

set format = `ncdump -k "$infile"`
set xx = $status

if ( $xx != 0 ) then			# abort on ncdump error
   exit $xx
endif

echo "File format = $format"

set size1 = `ls -Ll "$infile" | tr -s ' ' | cut -f5 -d' '`
echo "File size   = $size1"

if ( "$format" == classic ) then
   set ktype = 1
else
if ( "$format" == "64-bit offset" ) then
   set ktype = 2
else
   echo Nothing to compute. This format does not contain any padding sections.
   exit
endif
endif

# Use nccopy to reconstruct the file without any padding

nccopy -k $ktype "$infile" $temp1

set size2 = `ls -Ll $temp1 | tr -s ' ' | cut -f5 -d' '`
set dif = `expr $size1 - $size2`

echo "Size of padding (header and data combined) = $dif bytes"

/bin/rm $temp1				# clean up temp file
