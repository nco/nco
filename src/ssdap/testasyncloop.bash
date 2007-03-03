#!/usr/bin/env bash
./swampsendscript.py asyncinit.ssdap http://localhost/cgi-bin/nph-dods >temptokens.txt
grep SSDTOKEN temptokens.txt  | sed '1,$s#.*logical="%\(.*\)%".*token="\(.*\)".*#echo ssd_retrieve \2 >ssdhelper.ssdap \&\& ./swampsendscript.py ssdhelper.ssdap http://localhost/cgi-bin/nph-dods \1.nc#gi' | sh



