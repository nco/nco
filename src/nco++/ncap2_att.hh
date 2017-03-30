#ifndef NCAP2_ATT_HH /* Header file has not yet been defined in current source file */
#define NCAP2_ATT_HH

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <string>
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <time.h> /* machine time */
#ifndef _MSC_VER
#include <unistd.h> /* POSIX stuff */
#endif
#if !(defined __xlC__) && !(defined SGIMP64) // C++ compilers that do not allow stdint.h
#include <stdint.h> // Required by g++ for LLONG_MAX, ULLONG_MAX, by icpc for int64_t
#endif // C++ compilers that do not allow stdint.h

#include <assert.h>
#include <cmath> // sin cos cos sin 3.14159
#include <ctype.h>

// Standard C++ headers
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>


/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for libnetcdf.a */
//#include <antlr/AST.hpp> /* nneeded for ast_ind struct */
//#include "ncoParserTokenTypes.hpp"


/* Personal headers */
#include "libnco.h" /* netCDF Operator (NCO) library */
#include "ncap2_utl.hh"
#include "prs_cls.hh"



nco_bool        /* Reurns True if var has attribute style name */
ncap_var_is_att( 
var_sct *var);

std::string ncap_att2var
( prs_cls *prs_arg,   
  std::string att_nm);

int                    // O [flg] 0 - att doesnt exist 1 - att exists
ncap_att2var_chk       //   [fnc] returns nco_bool
( prs_cls *prs_arg,    // I [cls] var/att symbol tables
std::string att_nm);   // I [sn]  contains var name to read



var_sct * /* O [sct] variable containing attribute */
ncap_att_init /* [fnc] Grab an attribute from input file */
(const std::string s_va_nm, /* I [sng] att name of form var_nm&att_nm */ 
 prs_cls *prs_arg); /* I/O vectors of atts & vars & file names */


var_sct*               /* O [sct] variable containing attribute */
ncap_att_get	       /*   [fnc] Grab an attribute from input file */	
(int var_id,           /*   I  var id        */ 
 const char *var_nm,   /*   I [sng] var name */
 const char *att_nm,   /*   I [sng] att name */
 int location,         /*   I [flg] 1 - att from INPUT file  2 - att from OUTPUT file */
 prs_cls *prs_arg);    /* I/O vectors of atts & vars & file names  */


var_sct *                /* O [sct] variable containing attribute */
ncap_att_init(           /*   [fnc] Grab an attribute from input file */
std::string va_nm,       /* I [sng] att name of form var_nm&att_nm */ 
prs_cls *prs_arg);       /* I/O vectors of atts & vars & file names  */



int
ncap_att_gnrl
(const std::string s_dst,
 const std::string s_src,
 int location,         /*   I [flg] 1 - att from INPUT file  2 - att from OUTPUT file */
 prs_cls  *prs_arg);



nco_bool                /* O [flg] true if var has been stretched */
ncap_att_stretch    /* stretch a single valued attribute from 1 to sz */
(var_sct* var,      /* I/O [sct] variable */       
 long nw_sz);       /* I [nbr] new var size */


nco_bool                 /* O [flg] true if var has been stretched */
ncap_att_char_stretch    /* pad out or truncate a NC_CHAR attribute */
(var_sct* var,           /* I/O [sct] var/att */
long nw_sz);             /* I [nbr] new var size */


int 
ncap_att_cpy_sct
(var_sct *var1,
 var_sct *var2,
 prs_cls  *prs_arg);

int             
ncap_att_cpy
(const std::string s_dst,
 const std::string s_src,
 prs_cls  *prs_arg);

void 
ncap_att_prn   
(var_sct *var, 
 char *const att_in_sng);

char *            /* [0] sng - malloced inside function */
ncap_att_sprn     /* [fnc] Print a single attribute*/
(var_sct *var,   /* I Variable containing att */
 char *const att_in_sng); /* user defined format string */


int                 /* number appended */ 
ncap_att_str        /* extract string(s) from a NC_CHAR or NC_STRING type attribute */
(var_sct *var_att,  /* I [sct] input attribute */
 std::vector<std::string> &str_vtr);

char *            /* new malloc'ed string */ 
ncap_att_char    /* extract string from a NC_CHAR or first NC_STRING */
(var_sct *var_att);

var_sct *
ncap_att_cll_mtd(
char const *nm,
dmn_sct **dim,
int nbr_dim,
enum nco_op_typ op_typ
);

/*********************** make scalar vars/atts ********************************************/

// ncap_sclr_var_mk() overloads
/* Create a scalar variable of type, if bfill then malloc ptr_unn */
var_sct*
ncap_sclr_var_mk(
const std::string var_nm,
nc_type type,
//bool bfll=false); fxm csz
bool bfll);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
float val_float);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
double val_double);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_int val_int);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_short val_short);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_char val_char);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_byte val_byte);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_ubyte val_ubyte);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_ushort val_ushort);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_uint val_uint);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_int64 val_int64);

var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_uint64 val_uint64);



var_sct *
ncap_sclr_var_mk(
const std::string var_nm,
nco_string val_string);

/******************************************************************************************/



#endif  /* NCAP2_ATT_HH */
