c     $Header: /data/zender/nco_20150216/nco/src/nco/nc_fortran.F,v 1.3 1998-10-30 03:39:51 zender Exp $

c     Fortran arithmetic utilities for netCDF operators

c     (c) Copyright 1995--1998University Corporation for Atmospheric Research/
c     National Center for Atmospheric Research/
c     Climate and Global Dynamics Division
c
c     The file LICENSE contains the full copyright notice, or 
c     you may contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance.

c     Define the Fortran specification required to obtain the same type of 
c     integer as a C "long int" on this platform.
#ifdef SGI64
#define FORTRAN_EQUIV_C_LONG_INT integer*8
#else /* not SGI64 */
#define FORTRAN_EQUIV_C_LONG_INT integer
#endif /* not SGI64 */

c------------------------------Subroutine-------------------------------
      subroutine add_real(sz,has_mss_val,mss_val,cnt,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for mss_val attribute
      real mss_val              ! missing_value netCDF attribute, if any
      real op1(sz)              ! next values to process
c---------------------------Input/Output Arguments----------------------
      FORTRAN_EQUIV_C_LONG_INT cnt(sz) ! count of the number of valid operations on op2 so far
      real op2(sz)              ! cumulative values (mean, min, max ...)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)+op1(idx)
            cnt(idx)=cnt(idx)+1
         end do
      else
         do idx=1,sz
            if (op1(idx).ne.mss_val) then
               op2(idx)=op2(idx)+op1(idx)
               cnt(idx)=cnt(idx)+1
            endif
         end do
      endif
      return
      end                       ! end add_real()

c------------------------------Subroutine-------------------------------
      subroutine add_double_precision(sz,has_mss_val,mss_val,cnt,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for missing_value attribute
      double precision mss_val  ! missing_value netCDF attribute, if any
      double precision op1(sz)  ! next values to process
c---------------------------Input/Output Arguments----------------------
      FORTRAN_EQUIV_C_LONG_INT cnt(sz) ! count of the number of valid operations on op2 so far
      double precision op2(sz)  ! cumulative values (mean, min, max ...)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)+op1(idx)
            cnt(idx)=cnt(idx)+1
         end do
      else
         do idx=1,sz
            if (op1(idx).ne.mss_val) then
               op2(idx)=op2(idx)+op1(idx)
               cnt(idx)=cnt(idx)+1
            endif
         end do
      endif
      return
      end                       ! end add_double_precision()

c------------------------------Subroutine-------------------------------
      subroutine normalize_real(sz,has_mss_val,mss_val,
     $     cnt,op1)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      FORTRAN_EQUIV_C_LONG_INT cnt(sz) ! count of the number of valid operations on op1 so far
      integer has_mss_val       ! flag for missing_value attribute
      real mss_val              ! missing_value netCDF attribute, if any
c---------------------------Input/Output Arguments----------------------
      real op1(sz)              ! cumulative values on input, normalized on output
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op1(idx)=op1(idx)/cnt(idx)
         end do
      else
         do idx=1,sz
            if (cnt(idx).ne.0) then
               op1(idx)=op1(idx)/cnt(idx)
            else
               op1(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end normalize_real()

c------------------------------Subroutine-------------------------------
      subroutine normalize_double_precision(sz,has_mss_val,mss_val,
     $     cnt,op1)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      FORTRAN_EQUIV_C_LONG_INT cnt(sz) ! count of the number of valid operations on op1 so far
      integer has_mss_val       ! flag for missing_value attribute
      double precision mss_val  ! missing_value netCDF attribute, if any
c---------------------------Input/Output Arguments----------------------
      double precision op1(sz)  ! cumulative values on input, normalized on output
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op1(idx)=op1(idx)/cnt(idx)
         end do
      else
         do idx=1,sz
            if (cnt(idx).ne.0) then
               op1(idx)=op1(idx)/cnt(idx)
            else
               op1(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end normalize_double_precision

c------------------------------Subroutine-------------------------------
      subroutine subtract_real(sz,has_mss_val,mss_val,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for mss_val attribute
      real mss_val              ! missing_value netCDF attribute, if any
      real op1(sz)              ! first operand (A of C:=A-B)
c---------------------------Input/Output Arguments----------------------
      real op2(sz)              ! second operand on input, result on output (B and C of C:=A-B)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)-op1(idx)
         end do
      else
         do idx=1,sz
            if ((op2(idx).ne.mss_val).and.(op1(idx).ne.mss_val)) then
               op2(idx)=op2(idx)-op1(idx)
            else
               op2(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end subtract_real()

c------------------------------Subroutine-------------------------------
      subroutine subtract_double_precision(sz,has_mss_val,mss_val,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for mss_val attribute
      double precision mss_val  ! missing_value netCDF attribute, if any
      double precision op1(sz)  ! first operand (A of C:=A-B)
c---------------------------Input/Output Arguments----------------------
      double precision op2(sz)  ! second operand on input, result on output (B and C of C:=A-B)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)-op1(idx)
         end do
      else
         do idx=1,sz
            if ((op2(idx).ne.mss_val).and.(op1(idx).ne.mss_val)) then
               op2(idx)=op2(idx)-op1(idx)
            else
               op2(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end subtract_double_precision()

c------------------------------Subroutine-------------------------------
      subroutine multiply_real(sz,has_mss_val,mss_val,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for mss_val attribute
      real mss_val              ! missing_value netCDF attribute, if any
      real op1(sz)              ! first operand (A of C:=A*B)
c---------------------------Input/Output Arguments----------------------
      real op2(sz)              ! second operand on input, result on output (B and C of C:=A*B)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Externals--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)*op1(idx)
         end do
      else
         do idx=1,sz
            if ((op2(idx).ne.mss_val).and.(op1(idx).ne.mss_val)) then
               op2(idx)=op2(idx)*op1(idx)
            else
               op2(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end multiply_real()

c------------------------------Subroutine-------------------------------
      subroutine multiply_double_precision(sz,has_mss_val,mss_val,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for mss_val attribute
      double precision mss_val  ! missing_value netCDF attribute, if any
      double precision op1(sz)  ! first operand (A of C:=A*B)
c---------------------------Input/Output Arguments----------------------
      double precision op2(sz)  ! second operand on input, result on output (B and C of C:=A*B)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)*op1(idx)
         end do
      else
         do idx=1,sz
            if ((op2(idx).ne.mss_val).and.(op1(idx).ne.mss_val)) then
               op2(idx)=op2(idx)*op1(idx)
            else
               op2(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end multiply_double_precision

c------------------------------Subroutine-------------------------------
      subroutine divide_real(sz,has_mss_val,mss_val,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for mss_val attribute
      real mss_val              ! missing_value netCDF attribute, if any
      real op1(sz)              ! first operand (A of C:=B/A)
c---------------------------Input/Output Arguments----------------------
      real op2(sz)              ! second operand on input, result on output (B and C of C:=B/A)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Externals--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)/op1(idx)
         end do
      else
         do idx=1,sz
            if ((op2(idx).ne.mss_val).and.(op1(idx).ne.mss_val)) then
               op2(idx)=op2(idx)/op1(idx)
            else
               op2(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end divide_real()

c------------------------------Subroutine-------------------------------
      subroutine divide_double_precision(sz,has_mss_val,mss_val,
     $     op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz ! size of the operand array
      integer has_mss_val       ! flag for mss_val attribute
      double precision mss_val  ! missing_value netCDF attribute, if any
      double precision op1(sz)  ! first operand (A of C:=B/A)
c---------------------------Input/Output Arguments----------------------
      double precision op2(sz)  ! second operand on input, result on output (B and C of C:=B/A)
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx=1,sz
            op2(idx)=op2(idx)/op1(idx)
         end do
      else
         do idx=1,sz
            if ((op2(idx).ne.mss_val).and.(op1(idx).ne.mss_val)) then
               op2(idx)=op2(idx)/op1(idx)
            else
               op2(idx)=mss_val
            endif
         end do
      endif
      return
      end                       ! end divide_double_precision

c------------------------------Subroutine-------------------------------
      subroutine avg_reduce_real(sz_blk,sz_op2,
     $     has_mss_val,mss_val,cnt,op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz_blk ! size of a block over which to average
      FORTRAN_EQUIV_C_LONG_INT sz_op2 ! size of the second operand array
      integer has_mss_val       ! flag for mss_val attribute
      real mss_val              ! missing_value netCDF attribute, if any
      real op1(sz_blk,sz_op2)   ! next values to process
c---------------------------Input/Output Arguments----------------------
      FORTRAN_EQUIV_C_LONG_INT cnt(sz_op2) ! count of the number of valid operations
                                ! for element of op2 so far
      real op2(sz_op2)          ! output array of average values of each 
                                ! block of size sz_blk of op1
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx_op2
      FORTRAN_EQUIV_C_LONG_INT idx_blk
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx_op2=1,sz_op2
            do idx_blk=1,sz_blk
               op2(idx_op2)=op2(idx_op2)+op1(idx_blk,idx_op2)
            end do
            cnt(idx_op2)=sz_blk
         end do
      else
         do idx_op2=1,sz_op2
            do idx_blk=1,sz_blk
               if (op1(idx_blk,idx_op2).ne.mss_val) then
                  op2(idx_op2)=op2(idx_op2)+op1(idx_blk,idx_op2)
                  cnt(idx_op2)=cnt(idx_op2)+1
               endif
            end do
         end do
      endif
      return
      end                       ! end avg_reduce_real()

c------------------------------Subroutine-------------------------------
      subroutine avg_reduce_double_precision(sz_blk,sz_op2,
     $     has_mss_val,mss_val,cnt,op1,op2)
      implicit none
c---------------------------Input Arguments-----------------------------
      FORTRAN_EQUIV_C_LONG_INT sz_blk ! size of a block over which to average
      FORTRAN_EQUIV_C_LONG_INT sz_op2 ! size of the second operand array
      integer has_mss_val       ! flag for mss_val attribute
      double precision mss_val  ! missing_value netCDF attribute, if any
      double precision op1(sz_blk,sz_op2) ! next values to process
c---------------------------Input/Output Arguments----------------------
      FORTRAN_EQUIV_C_LONG_INT cnt(sz_op2) ! count of the number of valid operations
                                ! for element of op2 so far
      double precision op2(sz_op2) ! output array of average values of each 
                                ! block of size sz_blk of op1
c---------------------------Local workspace-----------------------------
      FORTRAN_EQUIV_C_LONG_INT idx_op2
      FORTRAN_EQUIV_C_LONG_INT idx_blk
c------------------------------Main code--------------------------------
      if (has_mss_val.eq.0) then
         do idx_op2=1,sz_op2
            do idx_blk=1,sz_blk
               op2(idx_op2)=op2(idx_op2)+op1(idx_blk,idx_op2)
            end do
            cnt(idx_op2)=sz_blk
         end do
      else
         do idx_op2=1,sz_op2
            do idx_blk=1,sz_blk
               if (op1(idx_blk,idx_op2).ne.mss_val) then
                  op2(idx_op2)=op2(idx_op2)+op1(idx_blk,idx_op2)
                  cnt(idx_op2)=cnt(idx_op2)+1
               endif
            end do
         end do
      endif
      return
      end                       ! end avg_reduce_double_precision()

