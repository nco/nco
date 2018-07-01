#!/bin/bash

# Purpose: Rename EAM regional output

# Usage:
# rgn_rnm.sh rgn_sng fl_in.nc fl_out.nc
# where rgn_sng is region string that EAM construct from namelist and appended to variables, e.g.,
# ~/rgn_rnm.sh _128e_to_134e_9s_to_16s ~/rgn_in.nc ~/rgn_in_rnm.nc

function ncvarlst { ncks --trd -m ${1} | grep -E ': type' | cut -f 1 -d ' ' | sed 's/://' | sort ; }
function ncdmnlst { ncks --cdl -m ${1} | cut -d ':' -f 1 | cut -d '=' -s -f 1 ; }

dbg_lvl=0 # [nbr] Debugging level
fl_idx=0 # [idx] File index
sfx=${1}
fl_in=${2}
fl_out=${3}

var_lst=`ncvarlst ${fl_in} | grep ${sfx}`
dmn_lst=`ncdmnlst ${fl_in} | grep ${sfx}`

dmn_sng=''
if [ -n "${dmn_lst}" ]; then
    for dmn in ${dmn_lst} ; do
	dmn_sng="${dmn_sng} -d ${dmn},${dmn/${sfx}/}"
    done # !dmn
fi # !dmn_lst

var_sng=''
if [ -n "${var_lst}" ]; then
    for var in ${var_lst} ; do
	var_sng="${var_sng} -v ${var},${var/${sfx}/}"
    done # !dmn_lst
fi # !dmn_lst

cmd_rnm[${fl_idx}]="ncrename -O ${dmn_sng} ${var_sng} ${fl_in} ${fl_out}"
if [ ${dbg_lvl} -ne 2 ]; then
    eval ${cmd_rnm[${fl_idx}]}
    if [ $? -ne 0 ]; then
	printf "${spt_nm}: ERROR Failed to rename regional input file. Debug this:\n${cmd_rnm[${fl_idx}]}\n"
	exit 1
    fi # !err
fi # !dbg

exit 0
