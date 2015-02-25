# $Header$

# Purpose: Demonstrate SWAMP usage

# Usage:
# Using SWAMP requires two steps
# First, identify the SWAMP server with access to your data, e.g.:
# export SWAMPURL='http://sand.ess.uci.edu:8081/SOAP'
# export SWAMPURL='http://pbs.ess.uci.edu:8081/SOAP'
# Second, call your script as an argument to SWAMP's invocation:
# python ~/nco/src/ssdap/swamp_client.py ~/nco/data/swamp.sh
# Whitespace-separated list of directories in sand:${DATA}/swamp_include is exported
# to PBS (pbs.ess.uci.edu) for processing by SWAMP server there.
# Remember to set all environment variables locally within the script
# SWAMP cannot read your environment on the server

export caseid='cssnc2050_02b'

flg_tst='1' # [flg] Test mode
flg_prd='0' # [flg] Production mode

flg_typ=${flg_prd} # [enm] Run type

if [ "${flg_typ}" = "${flg_tst}" ] ; then
    ncra -O ~/nco/data/in.nc ~/foo.nc
elif [ "${flg_typ}" = "${flg_prd}" ] ; then 
    for mm in 01 02 03 04 05 06 07 08 09 10 11 12; do
	ncra -O /data/zender/${caseid}/${caseid}.cam2.h0.????-${mm}.nc ~/${caseid}_clm${mm}.nc
    done # end loop over mm
    ncra -O ~/${caseid}_clm??.nc ~/${caseid}_clm.nc
    ncwa -O ~/${caseid}_clm.nc ~/${caseid}_clm_txy.nc
fi # !prd


