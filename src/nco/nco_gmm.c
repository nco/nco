/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_gmm.c,v 1.6 2010-01-05 20:02:17 zender Exp $ */

/* Purpose: Calculate incomplete gamma function */

/* Numeric source code is
   Copyright (C) 2008 John Burkardt
   Downloaded 20080924 from 
   http://people.scs.fsu.edu/~burkardt/cpp_src/asa032/asa032.html
   Additional copyright below is for NCO interface only */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include <math.h>
#include "nco_gmm.h" /* Incomplete Gamma function */

double r8_abs ( double x )
{
/* Purpose: R8_ABS returns the absolute value of an R8.
  Modified: 17 January 2008
  Author:   John Burkardt
  Parameters:  Input, double X, the argument.
  Output, double R8_ABS, the absolute value of the argument */
  if ( 0.0 <= x )
  {
    return x;
  }
  else
  {
    return ( - x );
  }
}

double alngam ( double xvalue, int *ifault )
{
/* Purpose: ALNGAM computes the logarithm of the gamma function.
   Modified: 13 January 2008
   Author: Allan Macleod
   C++ version by John Burkardt

  Reference:

    Allan Macleod,
    Algorithm AS 245,
    A Robust and Reliable Algorithm for the Logarithm of the Gamma Function,
    Applied Statistics,
    Volume 38, Number 2, 1989, pages 397-402.

  Parameters:

    Input, double XVALUE, the argument of the Gamma function.

    Output, int IFAULT, error flag.
    0, no error occurred.
    1, XVALUE is less than or equal to 0.
    2, XVALUE is too big.

    Output, double ALNGAM, the logarithm of the gamma function of X */
  double alr2pi = 0.918938533204673;
  double r1[9] = {
    -2.66685511495, 
    -24.4387534237, 
    -21.9698958928, 
     11.1667541262, 
     3.13060547623, 
     0.607771387771, 
     11.9400905721, 
     31.4690115749, 
     15.2346874070 };
  double r2[9] = {
    -78.3359299449, 
    -142.046296688, 
     137.519416416, 
     78.6994924154, 
     4.16438922228, 
     47.0668766060, 
     313.399215894, 
     263.505074721, 
     43.3400022514 };
  double r3[9] = {
    -2.12159572323E+05, 
     2.30661510616E+05, 
     2.74647644705E+04, 
    -4.02621119975E+04, 
    -2.29660729780E+03, 
    -1.16328495004E+05, 
    -1.46025937511E+05, 
    -2.42357409629E+04, 
    -5.70691009324E+02 };
  double r4[5] = {
     0.279195317918525, 
     0.4917317610505968, 
     0.0692910599291889, 
     3.350343815022304, 
     6.012459259764103 };
  double value;
  double x;
  double x1;
  double x2;
  double xlge = 510000.0;
  double xlgst = 1.0E+30;
  double y;

  x = xvalue;
  value = 0.0;

  /* Check the input */

  if ( xlgst <= x )
  {
    *ifault = 2;
    return value;
  }

  if ( x <= 0.0 )
  {
    *ifault = 1;
    return value;
  }

  *ifault = 0;

/*  Calculation for 0 < X < 0.5 and 0.5 <= X < 1.5 combined. */

  if ( x < 1.5 )
  {
    if ( x < 0.5 )
    {
      value = - log ( x );
      y = x + 1.0;

/*  Test whether X < machine epsilon. */

      if ( y == 1.0 )
      {
        return value;
      }
    }
    else
    {
      value = 0.0;
      y = x;
      x = ( x - 0.5 ) - 0.5;
    }

    value = value + x * (((( 
        r1[4]   * y 
      + r1[3] ) * y 
      + r1[2] ) * y 
      + r1[1] ) * y 
      + r1[0] ) / (((( 
                  y 
      + r1[8] ) * y 
      + r1[7] ) * y 
      + r1[6] ) * y 
      + r1[5] );

    return value;
  }

/*  Calculation for 1.5 <= X < 4.0. */

  if ( x < 4.0 )
  {
    y = ( x - 1.0 ) - 1.0;

    value = y * (((( 
        r2[4]   * x 
      + r2[3] ) * x 
      + r2[2] ) * x 
      + r2[1] ) * x 
      + r2[0] ) / (((( 
                  x 
      + r2[8] ) * x 
      + r2[7] ) * x 
      + r2[6] ) * x 
      + r2[5] );
  }

  /*  Calculation for 4.0 <= X < 12.0. */

  else if ( x < 12.0 ) 
  {
    value = (((( 
        r3[4]   * x 
      + r3[3] ) * x 
      + r3[2] ) * x 
      + r3[1] ) * x 
      + r3[0] ) / (((( 
                  x 
      + r3[8] ) * x 
      + r3[7] ) * x 
      + r3[6] ) * x 
      + r3[5] );
  }

  /*  Calculation for 12.0 <= X.*/

  else
  {
    y = log ( x );
    value = x * ( y - 1.0 ) - 0.5 * y + alr2pi;

    if ( x <= xlge )
    {
      x1 = 1.0 / x;
      x2 = x1 * x1;

      value = value + x1 * ( ( 
             r4[2]   * 
        x2 + r4[1] ) * 
        x2 + r4[0] ) / ( ( 
        x2 + r4[4] ) * 
        x2 + r4[3] );
    }
  }

  return value;
}

double nco_gamain ( double p, double x, int *ifault )
{
/*
  Purpose: GAMAIN computes the incomplete gamma ratio.
  Discussion: A series expansion is used if P > X or X <= 1.  Otherwise, a
  continued fraction approximation is used.
  Modified: 17 January 2008
  Author:

    G Bhattacharjee
    FORTRAN90 version by John Burkardt

  Reference:

    G Bhattacharjee,
    Algorithm AS 32:
    The Incomplete Gamma Integral,
    Applied Statistics,
    Volume 19, Number 3, 1970, pages 285-287.

  Parameters:

    Input, double X, P, the parameters of the incomplete 
    gamma ratio.  0 <= X, and 0 < P.

    Output, int *IFAULT, error flag.
    0, no errors.
    1, P <= 0.
    2, X < 0.
    3, underflow.
    4, error return from the Log Gamma routine.

    Output, double GAMAIN, the value of the incomplete gamma ratio */
  double a;
  double acu = 1.0E-08;
  double an;
  double arg;
  double b;
  double dif;
  double factor;
  double g;
  double gin;
  int i;
  double oflo = 1.0E+37;
  double pn[6];
  double rn;
  double term;
  double uflo = 1.0E-37;
  double value;

  /*  Check the input. */

  if ( p <= 0.0 )
  {
    *ifault = 1;
    value = 0.0;
    return value;
  }

  if ( x < 0.0 )
  {
    *ifault = 2;
    value = 0.0;
    return value;
  }

  if ( x == 0.0 )
  {
    *ifault = 0;
    value = 0.0;
    return value;
  }

  g = alngam ( p, ifault );

  if ( *ifault != 0 )
  {
    *ifault = 4;
    value = 0.0;
    return value;
  }

  arg = p * log ( x ) - x - g;

  if ( arg < log ( uflo ) )
  {
    *ifault = 3;
    value = 0.0;
    return value;
  }

  *ifault = 0;
  factor = exp ( arg );

  /*  Calculation by series expansion.*/

  if ( x <= 1.0 || x < p )
  {
    gin = 1.0;
    term = 1.0;
    rn = p;

    for ( ; ; )
    {
      rn = rn + 1.0;
      term = term * x / rn;
      gin = gin + term;

      if ( term <= acu )
      {
        break;
      }
    }

    value = gin * factor / p;
    return value;
  }

/*  Calculation by continued fraction. */

  a = 1.0 - p;
  b = a + x + 1.0;
  term = 0.0;

  pn[0] = 1.0;
  pn[1] = x;
  pn[2] = x + 1.0;
  pn[3] = x * b;

  gin = pn[2] / pn[3];

  for ( ; ; )
  {
    a = a + 1.0;
    b = b + 2.0;
    term = term + 1.0;
    an = a * term;
    for ( i = 0; i <= 1; i++ )
    {
      pn[i+4] = b * pn[i+2] - an * pn[i];
    }

    if ( pn[5] != 0.0 )
    {
      rn = pn[4] / pn[5];
      dif = r8_abs ( gin - rn );

/*  Absolute error tolerance satisfied? */

      if ( dif <= acu )
      {

/*  Relative error tolerance satisfied? */

        if ( dif <= acu * rn )
        {
          value = 1.0 - factor * gin;
          break;
        }
      }
      gin = rn;
    }

    for ( i = 0; i < 4; i++ )
    {
      pn[i] = pn[i+2];
    }

    if ( oflo <= r8_abs ( pn[4] ) )
    {
      for ( i = 0; i < 4; i++ )
      {
        pn[i] = pn[i] / oflo;
      }
    }
  }

  return value;
}

float r8_abs_f ( float x )
{
  /* Purpose: R8_ABS returns the absolute value of an R8.
     Modified: 17 January 2008
     Author: John Burkardt
     Parameters: Input, float X, the argument.
     Output, float R8_ABS, the absolute value of the argument */
  if ( 0.0 <= x )
  {
    return x;
  }
  else
  {
    return ( - x );
  }
}

float alngam_f ( float xvalue, int *ifault )
/*
  Purpose:

    ALNGAM computes the logarithm of the gamma function.

  Modified:

    13 January 2008

  Author:

    Allan Macleod
    C++ version by John Burkardt

  Reference:

    Allan Macleod,
    Algorithm AS 245,
    A Robust and Reliable Algorithm for the Logarithm of the Gamma Function,
    Applied Statistics,
    Volume 38, Number 2, 1989, pages 397-402.

  Parameters:

    Input, float XVALUE, the argument of the Gamma function.

    Output, int IFAULT, error flag.
    0, no error occurred.
    1, XVALUE is less than or equal to 0.
    2, XVALUE is too big.

    Output, float ALNGAM, the logarithm of the gamma function of X */
{
  float alr2pi = 0.918938533204673;
  float r1[9] = {
    -2.66685511495, 
    -24.4387534237, 
    -21.9698958928, 
     11.1667541262, 
     3.13060547623, 
     0.607771387771, 
     11.9400905721, 
     31.4690115749, 
     15.2346874070 };
  float r2[9] = {
    -78.3359299449, 
    -142.046296688, 
     137.519416416, 
     78.6994924154, 
     4.16438922228, 
     47.0668766060, 
     313.399215894, 
     263.505074721, 
     43.3400022514 };
  float r3[9] = {
    -2.12159572323E+05, 
     2.30661510616E+05, 
     2.74647644705E+04, 
    -4.02621119975E+04, 
    -2.29660729780E+03, 
    -1.16328495004E+05, 
    -1.46025937511E+05, 
    -2.42357409629E+04, 
    -5.70691009324E+02 };
  float r4[5] = {
     0.279195317918525, 
     0.4917317610505968, 
     0.0692910599291889, 
     3.350343815022304, 
     6.012459259764103 };
  float value;
  float x;
  float x1;
  float x2;
  float xlge = 510000.0;
  float xlgst = 1.0E+30;
  float y;

  x = xvalue;
  value = 0.0;

  /*  Check the input. */

  if ( xlgst <= x )
  {
    *ifault = 2;
    return value;
  }

  if ( x <= 0.0 )
  {
    *ifault = 1;
    return value;
  }

  *ifault = 0;

/*  Calculation for 0 < X < 0.5 and 0.5 <= X < 1.5 combined. */

  if ( x < 1.5 )
  {
    if ( x < 0.5 )
    {
      value = - logf ( x );
      y = x + 1.0;

/*  Test whether X < machine epsilon. */

      if ( y == 1.0 )
      {
        return value;
      }
    }
    else
    {
      value = 0.0;
      y = x;
      x = ( x - 0.5 ) - 0.5;
    }

    value = value + x * (((( 
        r1[4]   * y 
      + r1[3] ) * y 
      + r1[2] ) * y 
      + r1[1] ) * y 
      + r1[0] ) / (((( 
                  y 
      + r1[8] ) * y 
      + r1[7] ) * y 
      + r1[6] ) * y 
      + r1[5] );

    return value;
  }

  /*  Calculation for 1.5 <= X < 4.0. */

  if ( x < 4.0 )
  {
    y = ( x - 1.0 ) - 1.0;

    value = y * (((( 
        r2[4]   * x 
      + r2[3] ) * x 
      + r2[2] ) * x 
      + r2[1] ) * x 
      + r2[0] ) / (((( 
                  x 
      + r2[8] ) * x 
      + r2[7] ) * x 
      + r2[6] ) * x 
      + r2[5] );
  }

  /*  Calculation for 4.0 <= X < 12.0. */

  else if ( x < 12.0 ) 
  {
    value = (((( 
        r3[4]   * x 
      + r3[3] ) * x 
      + r3[2] ) * x 
      + r3[1] ) * x 
      + r3[0] ) / (((( 
                  x 
      + r3[8] ) * x 
      + r3[7] ) * x 
      + r3[6] ) * x 
      + r3[5] );
  }

  /*  Calculation for 12.0 <= X. */

  else
  {
    y = logf ( x );
    value = x * ( y - 1.0 ) - 0.5 * y + alr2pi;

    if ( x <= xlge )
    {
      x1 = 1.0 / x;
      x2 = x1 * x1;

      value = value + x1 * ( ( 
             r4[2]   * 
        x2 + r4[1] ) * 
        x2 + r4[0] ) / ( ( 
        x2 + r4[4] ) * 
        x2 + r4[3] );
    }
  }

  return value;
}

float nco_gamain_f ( float p, float x, int *ifault )
/*
  Purpose:

    GAMAIN computes the incomplete gamma ratio.

  Discussion:

    A series expansion is used if P > X or X <= 1.  Otherwise, a
    continued fraction approximation is used.

  Modified:

    17 January 2008

  Author:

    G Bhattacharjee
    FORTRAN90 version by John Burkardt

  Reference:

    G Bhattacharjee,
    Algorithm AS 32:
    The Incomplete Gamma Integral,
    Applied Statistics,
    Volume 19, Number 3, 1970, pages 285-287.

  Parameters:

    Input, float X, P, the parameters of the incomplete 
    gamma ratio.  0 <= X, and 0 < P.

    Output, int *IFAULT, error flag.
    0, no errors.
    1, P <= 0.
    2, X < 0.
    3, underflow.
    4, error return from the Log Gamma routine.

    Output, float GAMAIN, the value of the incomplete gamma ratio */
{
  float a;
  float acu = 1.0E-08;
  float an;
  float arg;
  float b;
  float dif;
  float factor;
  float g;
  float gin;
  int i;
  float oflo = 1.0E+37;
  float pn[6];
  float rn;
  float term;
  float uflo = 1.0E-37;
  float value;

  /*  Check the input. */

  if ( p <= 0.0 )
  {
    *ifault = 1;
    value = 0.0;
    return value;
  }

  if ( x < 0.0 )
  {
    *ifault = 2;
    value = 0.0;
    return value;
  }

  if ( x == 0.0 )
  {
    *ifault = 0;
    value = 0.0;
    return value;
  }

  g = alngam_f ( p, ifault );

  if ( *ifault != 0 )
  {
    *ifault = 4;
    value = 0.0;
    return value;
  }

  arg = p * logf ( x ) - x - g;

  if ( arg < logf ( uflo ) )
  {
    *ifault = 3;
    value = 0.0;
    return value;
  }

  *ifault = 0;
  factor = exp ( arg );

  /*  Calculation by series expansion. */

  if ( x <= 1.0 || x < p )
  {
    gin = 1.0;
    term = 1.0;
    rn = p;

    for ( ; ; )
    {
      rn = rn + 1.0;
      term = term * x / rn;
      gin = gin + term;

      if ( term <= acu )
      {
        break;
      }
    }

    value = gin * factor / p;
    return value;
  }

/*  Calculation by continued fraction.*/

  a = 1.0 - p;
  b = a + x + 1.0;
  term = 0.0;

  pn[0] = 1.0;
  pn[1] = x;
  pn[2] = x + 1.0;
  pn[3] = x * b;

  gin = pn[2] / pn[3];

  for ( ; ; )
  {
    a = a + 1.0;
    b = b + 2.0;
    term = term + 1.0;
    an = a * term;
    for ( i = 0; i <= 1; i++ )
    {
      pn[i+4] = b * pn[i+2] - an * pn[i];
    }

    if ( pn[5] != 0.0 )
    {
      rn = pn[4] / pn[5];
      dif = r8_abs_f ( gin - rn );

/*  Absolute error tolerance satisfied? */

      if ( dif <= acu )
      {

/*  Relative error tolerance satisfied? */

        if ( dif <= acu * rn )
        {
          value = 1.0 - factor * gin;
          break;
        }
      }
      gin = rn;
    }

    for ( i = 0; i < 4; i++ )
    {
      pn[i] = pn[i+2];
    }

    if ( oflo <= r8_abs_f ( pn[4] ) )
    {
      for ( i = 0; i < 4; i++ )
      {
        pn[i] = pn[i] / oflo;
      }
    }
  }

  return value;
}
