#!/usr/bin/env bash

# $Header: /data/zender/nco_20150216/nco/bm/ipcc_dly_T85.sh,v 1.4 2005-03-25 21:30:38 mangalam Exp $
# top section is all the shell cruft to set DATA dir and testing to see if
# we can use the GNU time command to test various things.  
# The guts of the script start at ~line 100.

# 1st test to see whether user has a DATA dir
echo ""
if [ -n "${DATA}" ]
then 
    echo "\$DATA (your data dir) is defined as [$DATA]."
    echo ""
    if [[ -e $DATA ]]
    then 
        if [[ -w $DATA ]]
        then 
            echo "...and it exists and is writable, so using it as such"; echo ""
        else
            echo "... and it exists, but isn't writable by you - a mistake?  Try again."
            echo ""
            exit 1;
        fi           
    else
        echo "...but it doesn't exist - trying to make it"
        echo ""
        mkdir -p $DATA
        echo ""
        if [[ ! -e $DATA ]]
        then 
            echo "there was a problem making this dir; try again..."
            echo ""
            exit 1
        else
            echo "[$DATA] made ok; continuing..."    
            echo ""
        fi
    fi
else
    echo "Env \$DATA (your data dir) not defined.  Please enter it or leave blank to define as \$HOME"
    echo ""
    echo -n "DATA DIR? "
    read DATA
    echo ""
    if [ -n "${DATA}" ]
    then 
        echo "OK - You've set DATA DIR set to [$DATA]"
        if [[ -e $DATA ]]
        then
            echo "... and that dir exists."
            echo ""
        else 
            echo "trying to make $DATA"
            echo ""
            mkdir -p $DATA
            if [[ ! -e $DATA ]]
            then 
                echo "there was a problem making this dir; try again..."
                echo ""
                exit 1
            else
                echo "$DATA made ok; continuing..."    
                echo ""
            fi            
        fi
    else
        DATA=${HOME}
        echo ""
        echo "no input detected, therefore DATA DIR set to [$DATA]"
    fi
fi

echo "checking to see if system has gnu time (better timing than bash timer)"
echo ""
if [[ -e "/usr/bin/time" ]]
then # is it GNU time or a pale imitation?
#    echo "/usr/bin/time exists; now is it the gnu version?"
#    echo ""
    /usr/bin/time --version >& out
    if grep -q GNU out
    then
        TIMER="/usr/bin/time -v"
#        echo "Hooray!!, I think it is!"
    else
        TIMER="/usr/bin/time"
#        echo "no, just a pale imitation - oh well."
    fi
else 
    TIMER="time"  
#    echo "no /usr/bin/time; have to use the bash timer - oh well."  
fi    
echo "<<FYI: TIMER is set to [$TIMER]>>"
echo

# for debugging, you might want to uncomment the next 2 lines to intercept the start.
#echo "hit [Enter] to continue or ^C to abort"; echo ""
#read continue


echo "Creating ${DATA}/nco/ipcc_dly_T85.nc file from template"
echo "This stage takes ~6.3s on the ESMF"
echo "                 ~1m on a 1.8GHz/4GB Opteron"
echo "                 ~1m36s on a 2.8GHz/2GB Xeon"
echo "                 ~1m s on a 1.8GHz/4GB Opteron"
echo ""
mkdir -p ${DATA}/nco

$TIMER ncgen -b -o ${DATA}/nco/ipcc_dly_T85.nc ipcc_dly_T85.cdl

echo ""
echo "Populating ${DATA}/nco/ipcc_dly_T85.nc file with floats"
echo "This entire script takes ~76.5m on a 2.8GHz Xeon "
echo "                         ~15.5m on the ESMF - YMMV"
echo "                         ~46m on a 1.8GHZ/4GB Opteron"
echo ""
echo "Starting to populate variables."


echo "ncapping 8 scalar vars takes:"
# ~47s on ESMF
# ~4m31s on sand

$TIMER ncap -O -s "weepy=0.8f;dopey=0.8f;sleepy=0.8f;grouchy=0.8f;sneezy=0.8f;doc=0.8f;wanky=0.8f;skanky=0.8f;" ${DATA}/nco/ipcc_dly_T85.nc ${DATA}/nco/ipcc_dly_T85.nc 

echo "ncapping 8 1D vars takes:"
# ~ 27s on ESMF
# ~4m17s on sand
$TIMER ncap -O -s "d1_00[time]=1.8f;d1_01[time]=1.8f;d1_02[time]=1.8f;d1_03[time]=1.8f;d1_04[time]=1.8f;d1_05[time]=1.8f;d1_06[time]=1.8f;d1_07[time]=1.8f;"  ${DATA}/nco/ipcc_dly_T85.nc ${DATA}/nco/ipcc_dly_T85.nc 

echo "ncapping 16 2D vars takes:"
# 1m on ESMF
# 4m50s on sand

$TIMER ncap -O -s "d2_00[lat,lon]=16.2f;d2_01[lat,lon]=16.2f;d2_02[lat,lon]=16.2f;d2_03[lat,lon]=16.2f;d2_04[lat,lon]=16.2f;d2_05[lat,lon]=16.2f;d2_06[lat,lon]=16.2f;d2_07[lat,lon]=16.2f;d2_08[lat,lon]=16.2f;d2_09[lat,lon]=16.2f;d2_10[lat,lon]=16.2f;d2_11[lat,lon]=16.2f;d2_12[lat,lon]=16.2f;d2_13[lat,lon]=16.2f;d2_14[lat,lon]=16.2f;d2_15[lat,lon]=16.2f;"  ${DATA}/nco/ipcc_dly_T85.nc ${DATA}/nco/ipcc_dly_T85.nc 


echo "ncapping 64 3D vars takes:"
# 106s on ESMF
# 4m20s on sand

$TIMER ncap -O -s "d3_00[time,lat,lon]=64.0f;d3_01[time,lat,lon]=64.0f;d3_02[time,lat,lon]=64.0f;d3_03[time,lat,lon]=64.0f;d3_04[time,lat,lon]=64.0f;d3_05[time,lat,lon]=64.0f;d3_06[time,lat,lon]=64.0f;d3_07[time,lat,lon]=64.0f;d3_08[time,lat,lon]=64.0f;d3_09[time,lat,lon]=64.0f;d3_10[time,lat,lon]=64.0f;d3_11[time,lat,lon]=64.0f;d3_12[time,lat,lon]=64.0f;d3_13[time,lat,lon]=64.0f;d3_14[time,lat,lon]=64.0f;d3_15[time,lat,lon]=64.0f;d3_16[time,lat,lon]=64.0f;d3_17[time,lat,lon]=64.0f;d3_18[time,lat,lon]=64.0f;d3_19[time,lat,lon]=64.0f;d3_20[time,lat,lon]=64.0f;d3_21[time,lat,lon]=64.0f;d3_22[time,lat,lon]=64.0f;d3_23[time,lat,lon]=64.0f;d3_24[time,lat,lon]=64.0f;d3_25[time,lat,lon]=64.0f;d3_26[time,lat,lon]=64.0f;d3_27[time,lat,lon]=64.0f;d3_28[time,lat,lon]=64.0f;d3_29[time,lat,lon]=64.0f;d3_30[time,lat,lon]=64.0f;d3_31[time,lat,lon]=64.0f;d3_32[time,lat,lon]=64.0f;d3_33[time,lat,lon]=64.0f;d3_34[time,lat,lon]=64.0f;d3_35[time,lat,lon]=64.0f;d3_36[time,lat,lon]=64.0f;d3_37[time,lat,lon]=64.0f;d3_38[time,lat,lon]=64.0f;d3_39[time,lat,lon]=64.0f;d3_40[time,lat,lon]=64.0f;d3_41[time,lat,lon]=64.0f;d3_42[time,lat,lon]=64.0f;d3_43[time,lat,lon]=64.0f;d3_44[time,lat,lon]=64.0f;d3_45[time,lat,lon]=64.0f;d3_46[time,lat,lon]=64.0f;d3_47[time,lat,lon]=64.0f;d3_48[time,lat,lon]=64.0f;d3_49[time,lat,lon]=64.0f;d3_50[time,lat,lon]=64.0f;d3_51[time,lat,lon]=64.0f;d3_52[time,lat,lon]=64.0f;d3_53[time,lat,lon]=64.0f;d3_54[time,lat,lon]=64.0f;d3_55[time,lat,lon]=64.0f;d3_56[time,lat,lon]=64.0f;d3_57[time,lat,lon]=64.0f;d3_58[time,lat,lon]=64.0f;d3_59[time,lat,lon]=64.0f;d3_60[time,lat,lon]=64.0f;d3_61[time,lat,lon]=64.0f;d3_62[time,lat,lon]=64.0f;d3_63[time,lat,lon]=64.0f;" ${DATA}/nco/ipcc_dly_T85.nc ${DATA}/nco/ipcc_dly_T85.nc 


echo "ncapping 32 4D vars takes:"
#takes ~8-20m on the ESMF
# takes 28m on sand/Opteron
 
$TIMER ncap -O -s "d4_00[time,lev,lat,lon]=1.1f;d4_01[time,lev,lat,lon]=1.2f;d4_02[time,lev,lat,lon]=1.3f;d4_03[time,lev,lat,lon]=1.4f;d4_04[time,lev,lat,lon]=1.5f;d4_05[time,lev,lat,lon]=1.6f;d4_06[time,lev,lat,lon]=1.7f;d4_07[time,lev,lat,lon]=1.8f;d4_08[time,lev,lat,lon]=1.9f;d4_09[time,lev,lat,lon]=1.11f;d4_10[time,lev,lat,lon]=1.12f;d4_11[time,lev,lat,lon]=1.13f;d4_12[time,lev,lat,lon]=1.14f;d4_13[time,lev,lat,lon]=1.15f;d4_14[time,lev,lat,lon]=1.16f;d4_15[time,lev,lat,lon]=1.17f;d4_16[time,lev,lat,lon]=1.18f;d4_17[time,lev,lat,lon]=1.19f;d4_18[time,lev,lat,lon]=1.21f;d4_19[time,lev,lat,lon]=1.22f;d4_20[time,lev,lat,lon]=1.23f;d4_21[time,lev,lat,lon]=1.24f;d4_22[time,lev,lat,lon]=1.25f;d4_23[time,lev,lat,lon]=1.26f;d4_24[time,lev,lat,lon]=1.27f;d4_25[time,lev,lat,lon]=1.28f;d4_26[time,lev,lat,lon]=1.29f;d4_27[time,lev,lat,lon]=1.312f;d4_28[time,lev,lat,lon]=1.322f;d4_29[time,lev,lat,lon]=1.332f;d4_30[time,lev,lat,lon]=1.342f;d4_31[time,lev,lat,lon]=1.352f;" ${DATA}/nco/ipcc_dly_T85.nc ${DATA}/nco/ipcc_dly_T85.nc 


echo ""
echo ""
echo "ncks has this to say about the test file ${DATA}/nco/ipcc_dly_T85.nc:"
echo ""
echo ""

ncks ${DATA}/nco/ipcc_dly_T85.nc |head -111

