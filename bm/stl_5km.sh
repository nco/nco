#!/usr/bin/env bash
# $Header: /data/zender/nco_20150216/nco/bm/stl_5km.sh,v 1.2 2005-03-23 18:28:16 mangalam Exp $
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

echo "Creating ${DATA}/nco/stl_5km.nc file from template"
echo "This step takes ~6.5s on a 900MHz/512MB RAM Thinkpad" 
echo "                ~1.5s on a 2.8GHz/2GB Xeon."
echo "                ~1.2s on the ESMF."
mkdir -p ${DATA}/nco

$TIMER ncgen -b -o ${DATA}/nco/stl_5km.nc stl_5km.cdl

echo ""
echo "Populating stl_5km file with floats"
echo "This takes ~5.5m on the Thinkpad"
echo "           ~2m on the 2.8GHz Xeon"
echo "           ~1.2m on the ESMF - YMMV"
echo ""

echo "ncapping 8 2D vars"
$TIMER ncap -O -s "d2_00[lat,lon]=2.8f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=2.8f;d2_03[lat,lon]=2.8f;d2_04[lat,lon]=2.8f;d2_05[lat,lon]=2.8f;d2_06[lat,lon]=2.8f;d2_07[lat,lon]=2.8f;"  ${DATA}/nco/stl_5km.nc ${DATA}/nco/stl_5km.nc 

echo ""; echo ""
echo "ncks has this to say about the test file: [stl_5km.nc]"
echo ""; echo ""

ncks stl_5km.nc |head -60

