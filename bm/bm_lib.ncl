
;************************************************
undef("xy_benchmarks_plot")
function xy_benchmarks_plot(rfl_img, xaxis, str_title:string, ymin:float, ymax:float)
local nList, n, data, LineColors, LineThicknesses, res, wks   
begin
;
;************************************************
;  deal with multiple plots on graph 


  nList=ListCount(rfl_img)
  ; data = new((/nList,dimsizes(rfl_img[0]&years)/),float)
  data = new((/nList,dimsizes(xaxis)/),float)
  LineColors=new((/nList/),string)
  LineThicknesses=new((/nList/),float)
  Markers=new((/nList/),integer)
  LineLabelStrings=new((/nList/),string)
  
  do n=0,nList-1
    data(n,:)=rfl_img[n](:) 
    if( isatt(rfl_img[n],"Color" ) )
      LineColors(n)=rfl_img[n]@Color
    else
      LineColors(n)="blue"
    end if  
    
    LineThicknesses(n)=4.0
    ; set default to circle   
    Markers(n)=16
    LineLabelStrings(n)=rfl_img[n]@xpr_str
  end do



str_fname=str_sub_str(str_title," and ","__")

;************************************************
; plotting parameters
;************************************************

 if(ymin.eq.ymax) then 
    ymin = min(data) 
    ymax = max(data)
 end if

 wks   = gsn_open_wks ("png",str_fname)        ; send graphics to PNG file

 res                  = True                     ; plot mods desired
 res@tiMainString     = str_title          ; add title
 res@tiYAxisString = rfl_img[0]@units 

 ; res@tmXBMode  = "auto"
 res@trXMinF = min(xaxis)
 res@trXMaxF = max(xaxis)
 res@tiXAxisString    = xaxis@units
 res@tmXBValues=xaxis   

 res@trYMinF     = ymin
 res@trYMaxF     = ymax*1.06 ; make bound 6% larger than max value - tidy
 
 res@xyLineThicknesses = LineThicknesses
 ; res@xyLineColors      = (/"blue","red"/)        ; change line color
 res@xyLineColors      = LineColors

 res@xyMarkLineMode = "MarkLines"               ; Markers *and* lines
 res@xyMarkers      = Markers   ; different markers
 ; set marker colors same as line color
 res@xyMarkerColors := LineColors
 res@xyDashPattern  = 0                         ; Make curves all solid

 wks@wkWidth =  800
 wks@wkHeight = 800
 

 plot = gsn_csm_xy (wks,xaxis,data,res) ; create plot

;***********************************************
; legend resources
;***********************************************
  lgres                    = True
  lgres@lgLineColors       = LineColors
  lgres@lgLineThicknessF   = LineThicknesses
  lgres@lgItemType         = "Lines"        ; show lines only (default)
  lgres@lgLabelFontHeightF = .04            ; set the legend label font thickness
  lgres@lgMonoDashIndex    =  True
  lgres@lgDashIndex        =  0             ; set all lines solid   
  lgres@vpWidthF           = 0.15           ; width of legend (NDC)
  lgres@vpHeightF          = 0.3            ; height of legend (NDC)
  lgres@lgPerimColor       = "grey"         ; draw the box perimeter in orange
  lgres@lgPerimThicknessF  = 5.0            ; thicken the box perimeter
  lbid = gsn_create_legend(wks,nList,LineLabelStrings,lgres)         ; create legend

  amres = True
  amres@amParallelPosF   = -0.8 	    ; move legend to the right
  amres@amOrthogonalPosF = -0.8             ; move the legend down
  ; amres@amZone           = 1
  ; amres@amSide           =   0 ; NhlLEFT
  ;amres@amTrackData           =  True
  ; amres@amDataXF           =  0.0
  ; amres@amDataYF           =  0.0
  amres@amJust             =  0 ;NhlTOPLEFT 
  
  annoid1 = gsn_add_annotation(plot,lbid,amres)   ; add legend to plot

 draw(plot)   ; Draw base plot.
 frame(wks)   ; advance frame

 
 return 1
end


undef(" make_benchmark")
function make_benchmark( xpr_str:string, cmd_in_str:string, cmd_out_str:string, bm,  Color:string)
begin
  bm@xpr_str=xpr_str
  bm@cmd_in_str=cmd_in_str
  bm@cmd_out_str=cmd_out_str
  bm@Color=Color
  bm@units="seconds" ; dont think this should be here

  return(bm)   
end  






