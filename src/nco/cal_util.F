c     Define the Fortran specification required to obtain the same type of 
c     integer as a C "long int" on this platform.
#if ( defined SGI64 ) || ( defined SGIMP64 )
#define FORTRAN_EQUIV_C_LONG_INT integer*8
#else /* not SGI64 */
#define FORTRAN_EQUIV_C_LONG_INT integer
#endif /* not SGI64 */

      real function diffdat( dat1, sec1, dat2, sec2 )

c Compute the difference: (dat2,sec2) - (dat1,sec1)  in days.

      implicit none

c Input arguments:

      integer
     $  dat1   ! date in yymmdd format
     $, sec1   ! seconds relative to dat1
     $, dat2   ! date in yymmdd format
     $, sec2   ! seconds relative to dat2

c Return value:

c (dat2,sec2) - (dat1,sec1)  in days.

c Local variables:

      integer lastd, lasts, firstd, firsts
     $, yr2, mo2, dy2, doy2, yr1, mo1, dy1, doy1, ndays
      real sign, days

c Externals:

      integer doy

      external
     $  doy     ! day of year (between 1 and 365)
c-----------------------------------------------------------------------

c     Dates equal?

      if ( dat1 .eq. dat2 .and. sec1 .eq. sec2 ) then
         diffdat = 0.
         return
      end if

c     Which date is later?

      if ( dat2 .gt. dat1 ) then
         sign = 1.
         lastd = dat2
         lasts = sec2
         firstd = dat1
         firsts = sec1
      else if ( dat2 .lt. dat1 ) then
         sign = -1.
         lastd = dat1
         lasts = sec1
         firstd = dat2
         firsts = sec2
      else
         if ( sec2 .gt. sec1 ) then
            sign = 1.
            lastd = dat2
            lasts = sec2
            firstd = dat1
            firsts = sec1
         else
            sign = -1.
            lastd = dat1
            lasts = sec1
            firstd = dat2
            firsts = sec2
         end if
      end if

c     Compute number of days between lastd and firstd

      yr2 = lastd / 10000
      mo2 = mod( lastd, 10000 ) / 100
      dy2 = mod( lastd, 100 )
      doy2 = doy( mo2, dy2 )

      yr1 = firstd / 10000
      mo1 = mod( firstd, 10000 ) / 100
      dy1 = mod( firstd, 100 )
      doy1 = doy( mo1, dy1 )

      ndays = 365*(yr2-yr1) + doy2 - doy1

c     Adjust for remaining seconds

      days = float( ndays ) + float( lasts - firsts )/86400.

c     Adjust sign

      diffdat = sign * days

      return
      end

c==============================================================================

      integer function doy( mon, cday )

c Compute day of year ignoring leap years.  Returns values in the range
c [1,365].

      integer mon, cday

      integer jdcon(12)
      save jdcon
      data jdcon/ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334/
c++csz
c      save jdcon
c--csz

      doy = jdcon(mon) + cday

      return
      end                                                                 ! DOY
c==============================================================================

      integer function newdate( date, iday )

c Find the date a specified number of days (possibly negative) from the
c given date.

      implicit none

c Input arguments:

      integer
     $   date  ! starting date in yymmdd format (can be negative)
     $,  iday  ! number of days to increment date (can be negative)

c Local variables:

      integer
     $  idate  ! intial value of date (may change sign)
     $, year   ! year of date
     $, month  ! month of date
     $, imonth ! save the initial value of month
     $, day    ! day of date
     $, ndays  ! running count of days to increment date by
     $, i      ! index
     $, cmonth ! current month as we increment date
     $, nd     ! days to end of month

      integer mdays(24)
      save mdays
      data mdays / 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
     $             31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 /
c++csz
c      save mdays
c--csz

c Externals:

      integer nd2endm

      external
     $  nd2endm  ! number of days to end of month
c-----------------------------------------------------------------------
      
      if ( iday .eq. 0 ) then
         newdate = date
         return
      end if

      idate = date
      year = idate / 10000
      if ( idate .lt. 0 ) idate = -idate
      month = mod( idate, 10000 ) / 100
      imonth = month
      day = mod( idate, 100 )
      
      if ( iday  .gt. 0 ) then
         ndays = iday
         year = year + ndays/365
         ndays = mod( ndays, 365 )
         do i = imonth, imonth+12
            cmonth = i  
            if ( i .gt. 12 ) cmonth = i - 12
            nd = nd2endm( cmonth, day )
            if ( ndays .gt. nd ) then
               month = month + 1
               if ( month .gt. 12 ) then
                  month = 1
                  year = year + 1
               end if
               day = 1
               ndays = ndays - nd - 1
               if ( ndays .eq. 0 ) go to 10
            else
               day = day + ndays
               go to 10
            end if
         end do
   10    continue

      else if ( iday .lt. 0 ) then
         ndays = -iday
         year = year - ndays/365
         ndays = mod( ndays, 365 )
         imonth = month
         do i = imonth+12, imonth, -1
            if ( ndays .ge. day ) then
               month = month - 1
               if ( month .lt. 1 ) then
                  month = 12
                  year = year - 1
               end if
               ndays = ndays - day
               day = mdays( month )
               if ( ndays .eq. 0 ) go to 20
            else
               day = day - ndays
               go to 20
            end if
         end do
   20    continue

      end if

      if ( year .ge. 0 ) then
         newdate = year*10000 + month*100 + day
      else
         newdate = -year*10000 + month*100 + day
         newdate = -newdate
      end if

      return
      end                                                            ! NEWDATE
c==============================================================================

      integer function nd2endm( mon, day )

c Returns the number of days to the end of the month.  This number added to
c the input arguement day gives the last day of month mon.

      implicit none

      integer mon, day, mdays(12)

      save mdays
      data mdays / 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 /
c++csz
c      save mdays
c--csz

      if ( mon .lt. 1  .or.  mon .gt. 12 ) then
c++csz
c         write(*,*)'nd2endm: illegal input... mon= ', mon
c         stop
c--csz
      end if

      nd2endm = mdays(mon) - day

      if ( day .lt. 0 ) then
c++csz
c         write(*,*)'nd2endm: illegal input... mon= ',mon,' day= ',day
c         stop
c--csz
      end if

      return
      end                                                             ! ND2ENDM
c==============================================================================

      real function caldayr( idate, isec )

c Calendar day with fractional part.  Returns values in the range
c [1., 366.)

      implicit none
      integer idate, isec, mon, day, doy
      external doy

      mon = mod( idate, 10000 ) / 100
      day = mod( idate, 100 )

      caldayr = doy( mon, day ) + float( isec )/86400.

      return
      end                                                           ! CALDAYR
c==============================================================================

      
