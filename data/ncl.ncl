; $Id: ncl.ncl,v 1.1 2014-09-22 18:15:44 zender Exp $

; Purpose: NCL script to compare to NCO scripts for speed

; ncl < /home/zender/nco/data/ncl.ncl
; load "/home/zender/nco/data/ncl.ncl"

begin ; begin ncl()

fl_nm="/home/zender/ET_2000-01_2001-12.nc"
fl_in=addfile(fl_nm,"r")
ET_1km=fl_in->ET_1km(0::12,:,:)
fl_nm_out="/home/zender/foo.nc"
fl_out=addfile(fl_nm_out,"w")
fl_out->ET_1km=ET_1km
end ; end ncl()

