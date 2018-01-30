load "bm_lib.ncl"


  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; vars that are in ?????_xps_img.mc 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; bm_times=new((/9/), float)



; single plot list
if(True) then  

  years=(/1,3,5,10,15,20/)
  years@units="years"


  my_list=NewList("fifo")

  bmx=make_benchmark("ctl","-3","-3",(/1.87,2.38,2.92,4.29,5.68,7.08/),"black")
  ListPush(my_list,bmx)

  ret=xy_benchmarks_plot(my_list,years,"test plot ",1.0,9.0)

  

end if




if (False) then


  my_list=NewList("fifo")
  years=(/1,3,5/)
  years@units="years"

  ListPush(my_list, make_benchmark("ctl","-6","-6",(/63.74,68.85,74.51/) , "black"))
  ListPush(my_list, make_benchmark("xpt1", "-6","-7",(/283.23,279.85,278.51/)  ,"red"))
  ListPush(my_list, make_benchmark( "xpt2","-6", "-7 -L 1", (/346.56,362.56,367.27/)  ,"blue"))
  ListPush(my_list, make_benchmark("xpt3", "-6", "-7 -L 9", (/643.09,700.14,728.24/)  ,"orange") )
  ListPush(my_list, make_benchmark("xpt4","-6", "-7 -L 1 --ppc default=3",(/401.07,436.10,431.86/)  ,"green")) 
  ListPush(my_list, make_benchmark("xpt5", "-6", "-7 --cnk_plc=g2d --cnk_map=nc4",(/355.09,360.92,366.49/) , "grey"))     
  ListPush(my_list, make_benchmark("xpt6", "-6", "-7 --cnk_plc=g2d --cnk_map=nco",(/332.39,327.01,342.70/) , "orange"))
  ListPush(my_list, make_benchmark("xpt7", "-6", "-7 --cnk_plc=g2d --cnk_map=rew",(/319.36,317.23,341.30/) , "purple"))

  ret=xy_benchmarks_plot(my_list,years,"ncclimo run times",0.0,0.0)


end if  


