#!/bin/bash
#######################################################################################
# purpose: benchmark chunking tools nccopy, h5repack ncks 
# author:  Henry Btowsky
# date:    28th February 2018

#######################################################################################
# usage examples
# chunking-times.sh <tool>  <input-netcdf-file> 
#
# chunking-times.sh nccopy cnk-tst-1032-29-35.nc
# chunking-times.sh ncks foo.nc
# chunking-times.sh h5repack cnk-tst-1032-29-35.nc
#######################################################################################

# get time from stderr log
function ex_time () {    
  tt_ar=($( cat  "$1" |grep zzz))  
  tt_el="${tt_ar[0]}"
  echo $tt_el 
}

dbg_flg=1
sim_flag=0
idx=0

# nccopy or h5repack
tool="$1" #  
in_fl="$2"
out_dir="."

lst_tst="C1 C2 C3 C4"

# lst_tst="C4"

for tst in $lst_tst; do

      case "$tst" in 
        
          C1)
	    info_dbg="C1 test 516x20x25"    
            cnk=(516 20 25);      
         ;;     

          C2)
	    info_dbg="C2 test 64x8x8"    
            cnk=(64 8 8);      
         ;;     

          C3)
	    info_dbg="C2 test 46x6x7"    
            cnk=(46 6 7);      
         ;;     

          C4)
	    info_dbg="C2 test 33x5x6"    
            cnk=(33 5 6);      
         ;;     


	  
      esac
      
      out_fl="$out_dir/cnk-tst-dbg-${cnk[0]}-${cnk[1]}-${cnk[2]}.nc"          
      [[ -e "$out_fl" ]]  && rm "$out_fl"

      if [[ $tool = "nccopy" ]]; then
                 
          arg_chunks="time/${cnk[0]},y/${cnk[1]},x/${cnk[2]}"
          exe_cmd="nccopy -k 4 -d0 -c ${arg_chunks} ${in_fl} ${out_fl}"        

      elif [[ $tool = "h5repack" ]]; then
           
          arg_chunks="T:CHUNK=${cnk[0]}x${cnk[1]}x${cnk[2]}"
          exe_cmd="h5repack -l ${arg_chunks} -i ${in_fl} -o ${out_fl}"        

      elif [[ $tool = "ncks" ]]; then
	  
	  arg_chunks="--cnk_dmn time,${cnk[0]} --cnk_dmn y,${cnk[1]} --cnk_dmn x,${cnk[2]}"
          exe_cmd="ncks -O -4 --cnk_plc g3d $arg_chunks  ${in_fl} ${out_fl}"  
              
      fi	  



	
        # do the deed  - nb zzz is so we can grep stderr
        [[ $dbg_flg -ne 0 ]] && echo "exe_cmd=$exe_cmd"
       
        # delete old climo files
	# rm "$out_fl"
        
        eval "/usr/bin/time --format \" %e zzz\" $exe_cmd 2>my_error_log" 
        if [[ $? -ne 0 ]]; then   
          echo "$tool ERROR"
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


