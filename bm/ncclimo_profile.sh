#!/bin/bash

# get time from stderr log
function ex_time () {    
  tt_ar=($( cat  "$1" |grep zzz))  
  tt_el="${tt_ar[0]}"
  echo $tt_el 
}

dbg_flg=1
sim_flag=0
idx=0
model="cam"

ncclimo_sh="/ccs/home/butowsky/bin/ncclimo"
in_dir="/lustre/atlas1/cli115/world-shared/mbranst/famipc5_ne120_v0.3_00003-wget-test"
out_dir="/lustre/atlas/proj-shared/cli115/butowsky/ne120"


# lst_tst="B1 B3 B5 B10 B15 B20"
lst_tst="B1 B3 B5"

# add_opts="-7 -L 7 --cnk_plc=g2d --cnk_map=rew"
std_opts="$1"
ext_args="$2"

for tst in $lst_tst; do

      case "$tst" in 
        
         B1)     
           info_dbg="B1 - test a single year "
	   srt_year=1979
	   end_year=1979
          ;;     

         B3)   
           info_dbg="B3 -  test 3 years"
	   srt_year=1979
	   end_year=1981
          ;;     

      
         B5)
           info_dbg="test B5 - test 5 years  "
	   srt_year=1979
	   end_year=1983
          ;;
                  
         B10)  
           info_dbg="test B10 - test 10 years"
	   srt_year=1979
	   end_year=1988
          ;;     

         B15)  
           info_dbg="test B15 - test 15 years"
	   srt_year=0000
	   end_year=0014
          ;;     

         B20)  
           info_dbg="test B20 - test 20 years"
	   srt_year=0000
	   end_year=0019
          ;;     


         DEBUG)
	  info_dbg="B1 - test a single year "
	  exe_cmd="ncclimo -i . -o out_climo/ -c essgcm14 -m $model -s 0000 -e 0000 -a sdc"
          ;;     

        esac   

        exe_cmd="${ncclimo_sh} -d 1 -p mpi -i $in_dir -o $out_dir -c famipc5_ne120_v0.3_00003 -m $model -s $srt_year -e $end_year -a sdc $std_opts  -n '$ext_args'"
        # do the deed  - nb zzz is so we can grep stderr
        [[ $dbg_flg -ne 0 ]] && echo "exe_cmd=$exe_cmd"
       
        # delete old climo files
	rm "${out_dir}/*.nc"
        
        eval "/usr/bin/time --format \" %e zzz\" $exe_cmd 2>my_error_log" 
        if [[ $? -ne 0 ]]; then   
          echo "NCO ERROR"
          cat my_error_log        
        fi   
        # extract time from stderr 
        ts=$(ex_time my_error_log)
     
        [[ $dbg_flg -ne 0 ]] && echo  "test  $info_dbg time=$ts"  

        if [[ -n "$ts" ]]; then
          bm_times[idx]="$ts"
        fi
  
        : $(( idx++ ))  

done

# format times into ncl array
# e.g (/3.60,4.83,6.07,9.13,12.29,15.34/)
ncl_sng="(/${bm_times[@]}/)"
ncl_sng="${ncl_sng// /,}"

echo $ncl_sng


