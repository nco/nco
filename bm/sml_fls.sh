#!/usr/bin/env bash

# $Header: /data/zender/nco_20150216/nco/bm/sml_fls.sh,v 1.2 2005-03-23 18:28:15 mangalam Exp $
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

mkdir -p ${DATA}/nco

echo "The entire script takes ~1m  on a 900MHz/512MB THinkpad"
echo "                        ~56s on a 2.8GHz/2GB Xeon" 
echo "                        ~35s on the ESMF"
echo "                        YMMV"

    echo ""
	echo "Making ge test file template..."
    echo "Creating ${DATA}/nco/ge.nc file from template"  
    ncgen -b -o ${DATA}/nco/gne_exp.nc gne_exp.cdl
    echo "   ...done."
    echo ""
    echo "Now making ${DATA}/nco/smallsat.nc from template..."
    ncgen -b -o ${DATA}/nco/sml_stl.nc sml_stl.cdl
    echo "   ...done."
    echo ""
    echo "Now populating ge test file. Timing follows:"
    $TIMER ncap -O -s "base[rep,treat,cell,params,ge_atoms]=5.67f" ${DATA}/nco/gne_exp.nc ${DATA}/nco/gne_exp.nc 
    echo "   ...done."
    echo ""
    echo "Now populating smallsat test file. Timing follows:"
    $TIMER ncap -O -s "d2_00[lat,lon]=0.1f;d2_01[lat,lon]=1.2f;d2_02[lat,lon]=2.3f;" ${DATA}/nco/sml_stl.nc ${DATA}/nco/sml_stl.nc
    echo "   ...done."
    echo ""
    echo "What ncks says about gne_exp.nc:"
    echo "===================================="
    ncks ${DATA}/nco/gne_exp.nc |head -44
    echo ""
    echo ""
    echo ""
    echo "What ncks says about sml_stl.nc"  
    echo "===================================="
    ncks ${DATA}/nco/sml_stl.nc |head -44
    echo ""
    
