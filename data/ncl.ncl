; $Header$

; Purpose: NCL script to compare to NCO scripts for speed

; time ncl < /home/zender/nco/data/ncl.ncl
; load "/home/zender/nco/data/ncl.ncl"

begin ; begin ncl()

; Examine stride-slowdown on netCDF4 datasets
; time ncks -O -d time,0,0,12 ~/ET_2000-01_2001-12.nc ~/foo.nc # borken after 6m13s, numerous HDF errors
; NCL produces same HDF errors as NCO when striding
fl_nm="/home/zender/ET_2000-01_2001-12.nc"
fl_in=addfile(fl_nm,"r")
ET_1km=fl_in->ET_1km(0::12,:,:)
;ET_1km=fl_in->ET_1km(0:0,:,:)
fl_nm_out="/home/zender/foo.nc"
fl_out=addfile(fl_nm_out,"w")
fl_out->ET_1km=ET_1km
end ; end ncl()

