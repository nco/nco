#!/usr/bin/env bash
# $Id: fullpbs.bash,v 1.1 2007-01-31 00:44:49 wangd Exp $
./scriptwrap.py full_resamp.swamp http://pbs.calit2.uci.edu:8000/cgi-bin/nph-dods >temptokens.txt
grep SSDTOKEN temptokens.txt  | sed '1,$s#.*logical="%\(.*\)%".*token="\(.*\)".*#echo ssd_retrieve \2 >ssdhelper.swamp \&\& ./scriptwrap.py ssdhelper.swamp http://pbs.calit2.uci.edu:8000/cgi-bin/nph-dods \1.nc#gi' | sh



