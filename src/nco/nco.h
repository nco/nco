/* $Header: /data/zender/nco_20150216/nco/src/nco/nco.h,v 1.7 2002-01-22 08:54:47 zender Exp $ */

/* Purpose: Typedefs and global variables for NCO netCDF operators */

/* Copyright (C) 1995--2002 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the NetCDF and HDF libraries 
   and distribute the resulting executables under the terms of the GPL, 
   but in addition obeying the extra stipulations of the netCDF and 
   HDF library licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   The file LICENSE contains the GNU General Public License, version 2
   It may be viewed interactively by typing, e.g., ncks -L

   The author of this software, Charlie Zender, would like to receive
   your suggestions, improvements, bug-reports, and patches, for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
 */

#ifndef NCO_H /* Contents have not yet been inserted in current source file */
#define NCO_H

#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* __cplusplus */

/* NCO uses native type nco_long to store variables of type NC_INT */
typedef long nco_long;

#ifndef bool
#define bool int
#endif /* bool */
#ifndef True
#define True 1
#endif /* True */
#ifndef False
#define False 0
#endif /* False */

/* Variables marked CEWI "Compiler Error Warning Initialization" are initialized
   to prevent spurious "warning: `float foo' might be used uninitialized in 
   this function" warnings when, e.g., GCC -Wuninitialized is turned on.
   Note that these warning messages are compiler and OS dependent
   GCC on Alpha, e.g., emits warnings which cannot be removed by this trick */
#define NULL_CEWI NULL
#define char_CEWI '\0'
#define double_CEWI 0.0
#define float_CEWI 0.0
#define int_CEWI 0
#define long_CEWI 0L
#define nco_long_CEWI 0L
#define short_CEWI 0

  /* netcdf.h NC_GLOBAL is, strictly, the variable ID for global attributes
     NCO_REC_DMN_UNDEFINED is variable ID of record dimension iff record dimension is undefined
     Normally using -1 for this ID is fine, but token makes meaning clearer
     NB: nc_inq() family is defined to return -1 for missing record dimensions */
#define NCO_REC_DMN_UNDEFINED -1

#ifdef MAIN_PROGRAM_FILE /* Current file contains main() */

/* Global variables and variables with scope limited to main.c allocated here */

int prg; /* [enm] Program ID */
int prg_get(void){return prg;} /* [enm] Program ID */

char *prg_nm; /* [sng] Program name */
char *prg_nm_get(void){return prg_nm;} /* [sng] Program name */

unsigned short dbg_lvl=0; /* [enm] Debugging level */
unsigned short dbg_lvl_get(void){return dbg_lvl;} /* [enm] Debugging level */

#else /* MAIN_PROGRAM_FILE is NOT defined, i.e., current file does not contain main() */

/* External references to global variables are declared as extern here
   Variables with local file scope in all files except the main.c file are allocated here */

#endif /* MAIN_PROGRAM_FILE is NOT defined, i.e., the current file does not contain main() */

enum prg{ /* [enm] Key value for all netCDF operators */
  ncap,
  ncatted,
  ncdiff,
  ncea,
  ncecat,
  ncflint,
  ncks,
  ncra,
  ncrcat,
  ncrename,
  ncwa
}; /* end prg enum */

enum{ /* [enm] Attribute editor mode */
  aed_append,
  aed_create,
  aed_delete,
  aed_modify,
  aed_overwrite
}; /* end enum */

enum nco_rlt_opr{ /* [enm] Arithmetic relations (comparisons) for masking */
  nco_op_eq, /* Equality */
  nco_op_ne, /* Inequality */
  nco_op_lt, /* Less than */
  nco_op_gt, /* Greater than */
  nco_op_le, /* Less than or equal to */
  nco_op_ge /* Greater than or equal to */
}; /* end enum */

enum lmt_typ{ /* [enm] Limit type */
  lmt_crd_val, /* 0, Coordinate value limit */
  lmt_dmn_idx /* 1, Dimension index limit */
}; /* end lmt_typ enum */

enum nco_pck_typ{ /* [enm] Packing type */
  nco_pck_all_xst_att, /* 0, Pack all variables, keeping existing packing attributes if any */
  nco_pck_all_new_att, /* 1, Pack all variables, always generating new packing attributes */
  nco_pck_xst_xst_att, /* 2, Pack existing packed variables, keeping existing packing attributes if any */
  nco_pck_xst_new_att, /* 3, Pack existing packed variables, always generating new packing attributes */
  nco_pck_upk, /* 4, Unpack all packed variables */
  nco_pck_nil /* 5, Do not think about packing */
}; /* end nco_pck_typ enum */

enum nco_op_typ{ /* [enm] Operation type */
  nco_op_avg, /* Average */
  nco_op_min, /* Minimum value */
  nco_op_max, /* Maximum value */
  nco_op_ttl, /* Linear sum */
  nco_op_sqravg, /*  */
  nco_op_avgsqr,
  nco_op_sqrt,
  nco_op_rms,
  nco_op_rmssdn
}; /* end nco_op_typ enum */
/* end enumeration section */
  
#ifndef EXIT_SUCCESS /* Most likely this is a SUN4 machine */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif /* SUN4 */

typedef struct { /* lmt_sct */
  char *nm; /* [sng] Variable name */
  int lmt_typ; /* crd_val or dmn_idx */
  /* Following four flags are used only by multi-file operators ncra and ncrcat: */
  bool is_usr_spc_lmt; /* True if any part of limit is user-specified, else False */
  bool is_usr_spc_min; /* True if user-specified, else False */
  bool is_usr_spc_max; /* True if user-specified, else False */
  bool is_rec_dmn; /* True if record dimension, else False */
  long rec_skp_vld_prv; /* Records skipped at end of previous valid file (multi-file record dimension only) */
  long rec_skp_nsh_spf; /* Number of records skipped in initial superfluous files (multi-file record dimension only) */
  char *min_sng; /* User-specified string for dimension minimum */
  char *max_sng; /* User-specified string for dimension maximum */
  char *srd_sng; /* User-specified string for dimension stride */
  int id; /* Dimension ID */
  long min_idx; /* Index of minimum requested value in dimension */
  long max_idx; /* Index of maximum requested value in dimension */
  double min_val; /* Double precision representation of minimum value of coordinate requested or implied */
  double max_val; /* Double precision representation of maximum value of coordinate requested or implied */
  long srt; /* Index to start of hyperslab */
  long end; /* Index to end of hyperslab */
  long cnt; /* # of valid elements in this dimension (including effects of stride and wrapping) */
  long srd; /* Stride of hyperslab */
} lmt_sct;

typedef struct{ /* nm_id_sct */
  char *nm;
  int id;
} nm_id_sct;

typedef struct{ /* rnm_sct */
  char *old_nm;
  char *new_nm;
  int id;
} rnm_sct;

typedef union{ /* ptr_unn */
  float *fp;
  double *dp;
  nco_long *lp; /* NC_INT is stored in native type nco_long */
  short *sp;
  unsigned char *cp;
  signed char *bp;
  void *vp;
} ptr_unn;

typedef union{ /* val_unn */
  float f;
  double d;
  nco_long l;
  short s;
  unsigned char c;
  signed char b;
} val_unn;

typedef struct{ /* aed_sct */
  char *att_nm; /* Name of attribute */
  char *var_nm; /* Name of variable, or NULL for global attribute */
  int id; /* Variable ID or NC_GLOBAL ( = -1) for global attribute */
  long sz; /* Number of elements in attribute */
  nc_type type; /* Type of attribute */
  ptr_unn val; /* Pointer to attribute value */
  short mode; /* action to perform with attribute */
} aed_sct;

typedef struct { /* att_sct */
  char *nm;
  nc_type type;
  long sz;
  char fmt[5];
  ptr_unn val;
} att_sct;

typedef struct dmn_sct_tag{ /* dmn_sct */
  char *nm; /* Dimension name */
  int id; /* Dimension ID */
  int nc_id; /* File ID */
  long sz; /* Full size of dimension in file (NOT the hyperslabbed size) */
  short is_rec_dmn; /* Is this the record dimension? */
  short is_crd_dmn; /* Is this a coordinate dimension? */
  int cid; /* Variable ID of the associated coordinate, if any */
  nc_type type; /* Type of coordinate, if applicable */
  char fmt[5]; /* Hint for printf()-style formatting */
  long srt; /* Index to start of hyperslab */
  long end; /* Index to end of hyperslab */
  long cnt; /* Number of valid elements in this dimension (including effects of stride and wrapping) */
  long srd; /* Stride of hyperslab */
  ptr_unn val; /* Buffer to hold hyperslab */
  struct dmn_sct_tag *xrf; /* Cross-reference to associated dimension structure (usually the structure for dimension on output) */
} dmn_sct;

/* Each member of var_sct structure should be initialized to default in var_dfl_set()
   Each pointer member of var_sct structure should be freed in var_free()
   Each pointer member of var_sct structure should be copied in var_dpl() */
typedef struct var_sct_tag{ /* var_sct */
  char *nm; /* Variable name */
  int id; /* Variable ID */
  int nc_id; /* File ID */
  int nbr_dim; /* Number of dimensions of variable in input file */
  nc_type type; /* Type of variable in RAM */
  nc_type typ_dsk; /* Type of variable on disk (never changes) */
  short is_rec_var; /* Is this a record variable? */
  short is_crd_var; /* Is this a coordinate variable? */
  long sz; /* Number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */
  long sz_rec; /* Number of elements in one record of hyperslab */
  int nbr_att; /* Number of attributes */
  int has_mss_val; /* Is there a missing_value attribute? */
  ptr_unn mss_val; /* Value of missing_value attribute, if any (mss_val stored in this structure must be same type as variable) */
  int cid; /* Dimension ID of the associated coordinate, if any */
  char fmt[5]; /* Hint for printf()-style formatting */
  dmn_sct **dim; /* Pointers to full dimension structures */
  int *dmn_id; /* Contiguous vector of dimension IDs */
  long *srt; /* Contiguous vector of indices to start of hyperslab */
  long *end; /* Contiguous vector of indices to end of hyperslab */
  long *cnt; /* Contiguous vector of lengths of hyperslab */
  long *srd; /* Contiguous vector of stride of hyperslab */
  ptr_unn val; /* Buffer to hold hyperslab */
  long *tally; /* Number of valid operations performed so far */
  struct var_sct_tag *xrf; /* Cross-reference to associated variable structure (usually the structure for variable on output) */
  int pck_dsk; /* Variable is packed on disk (valid scale_factor, add_offset, or both attributes exist) */
  int pck_ram; /* Variable is packed in memory (valid scale_factor, add_offset, or both attributes exist) */
  int has_scl_fct; /* Valid scale_factor attribute exists */
  int has_add_fst; /* Valid add_offset attribute exists */
  ptr_unn scl_fct; /* Value of scale_factor attribute of type typ_upk */
  ptr_unn add_fst; /* Value of add_offset attribute of type typ_upk */
  nc_type typ_pck; /* Type of variable when packed (on disk). typ_pck = typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  nc_type typ_upk; /* Type of variable when unpacked (expanded) (in memory) */
} var_sct;

/* Fortran functions are deprecated as of NCO 1.2, will be removed unless volunteer takes over their maintenance */
#ifdef USE_FORTRAN_ARITHMETIC
#ifdef CRAY
#define FORTRAN_add_real ADD_REAL
#define FORTRAN_add_double_precision ADD_DOUBLE_PRECISION
#define FORTRAN_avg_reduce_real AVG_REDUCE_REAL
#define FORTRAN_avg_reduce_double_precision AVG_REDUCE_DOUBLE_PRECISION
#define FORTRAN_normalize_real NORMALIZE_REAL
#define FORTRAN_normalize_double_precision NORMALIZE_DOUBLE_PRECISION
#define FORTRAN_newdate NEWDATE
#define FORTRAN_subtract_real SUBTRACT_REAL
#define FORTRAN_subtract_double_precision SUBTRACT_DOUBLE_PRECISION
#define FORTRAN_multiply_real MULTIPLY_REAL
#define FORTRAN_multiply_double_precision MULTIPLY_DOUBLE_PRECISION
#define FORTRAN_divide_real DIVIDE_REAL
#define FORTRAN_divide_double_precision DIVIDE_DOUBLE_PRECISION
#endif /* CRAY */
#if ( defined RS6K ) || ( defined AIX )
#define FORTRAN_add_real add_real
#define FORTRAN_add_double_precision add_double_precision
#define FORTRAN_avg_reduce_real avg_reduce_real
#define FORTRAN_avg_reduce_double_precision avg_reduce_double_precision
#define FORTRAN_normalize_real normalize_real
#define FORTRAN_normalize_double_precision normalize_double_precision
#define FORTRAN_newdate newdate
#define FORTRAN_subtract_real subtract_real
#define FORTRAN_subtract_double_precision subtract_double_precision
#define FORTRAN_multiply_real multiply_real
#define FORTRAN_multiply_double_precision multiply_double_precision
#define FORTRAN_divide_real divide_real
#define FORTRAN_divide_double_precision divide_double_precision
#endif /* RS6K || AIX */
/* 
   pgf90 subroutines have one underscore by default
   pgf90 underscore behavior is altered by -Mnosecond_underscore
   g77 subroutines have two underscores by default
   g77 functions (e.g., newdate()) have one underscore by default 
   g77 underscore behavior is altered by -fno-second-underscore 
*/
#ifdef LINUX 
#define FORTRAN_add_real add_real_
#define FORTRAN_add_double_precision add_double_precision_
#define FORTRAN_avg_reduce_real avg_reduce_real_
#define FORTRAN_avg_reduce_double_precision avg_reduce_double_precision_
#define FORTRAN_normalize_real normalize_real_
#define FORTRAN_normalize_double_precision normalize_double_precision_
#define FORTRAN_newdate newdate_
#define FORTRAN_subtract_real subtract_real_
#define FORTRAN_subtract_double_precision subtract_double_precision_
#define FORTRAN_multiply_real multiply_real_
#define FORTRAN_multiply_double_precision multiply_double_precision_
#define FORTRAN_divide_real divide_real_
#define FORTRAN_divide_double_precision divide_double_precision_
#endif /* LINUX */
#if ( defined ALPHA ) || ( defined SUN4 ) || ( defined SUN4SOL2 ) || ( defined SUNMP ) || ( defined SGI5 ) || ( defined SGI64 ) || ( defined SGIMP64 )
#define FORTRAN_add_real add_real_
#define FORTRAN_add_double_precision add_double_precision_
#define FORTRAN_avg_reduce_real avg_reduce_real_
#define FORTRAN_avg_reduce_double_precision avg_reduce_double_precision_
#define FORTRAN_normalize_real normalize_real_
#define FORTRAN_normalize_double_precision normalize_double_precision_
#define FORTRAN_newdate newdate_
#define FORTRAN_subtract_real subtract_real_
#define FORTRAN_subtract_double_precision subtract_double_precision_
#define FORTRAN_multiply_real multiply_real_
#define FORTRAN_multiply_double_precision multiply_double_precision_
#define FORTRAN_divide_real divide_real_
#define FORTRAN_divide_double_precision divide_double_precision_
#endif /* SUN-style */
#endif /* USE_FORTRAN_ARITHMETIC */

/* Function prototypes 
   Prototypes without descriptive variable names were written before ANSI C compilers were widely available and should be migrated to include arguments names and comments
   Specifying extern is redundant (functions are extern by default) but Emacs highlights them in a nice shade of lavender :-)
   fxm: TODO 210 Break into sourcefile specific headers, e.g., csz.h, pck.h... */

extern void aed_prc(int,int,aed_sct);
extern bool arm_inq(int);
extern bool ncar_csm_inq(int);
extern char **fl_lst_mk(char **,int,int,int *,char **);
extern char **lst_prs(char *,const char *,int *);
extern char *cmd_ln_sng(int,char **);
extern char *cvs_vrs_prs();
extern char *fl_mk_lcl(char *,char *,int *);
extern char *fl_nm_prs(char *,int,int *,char **,int,char **,char *);
extern char *fl_out_open(char *,bool,bool,int *);
extern char *nmn_get(void);
extern char *prg_nm_get(void);
extern char *prg_prs(char *,int *);
extern char *sng_lst_prs(char **,const long, const char *);
extern int sng_ascii_trn(char *);
extern dmn_sct *dmn_dpl(dmn_sct *);
extern dmn_sct *dmn_fll(int,int,char *);
extern double arm_time_mk(int,double);
extern int mss_val_get(int,var_sct *);
extern int nd2endm(int,int);
extern int op_prs_rlt(char *);
extern int nco_op_typ_get(char *);
extern int nco_pck_typ_get(char *);
extern int prg_get(void);
extern lmt_sct *lmt_prs(int,char **);
extern lmt_sct lmt_sct_mk(int,int,lmt_sct *,int,bool);
extern nco_long FORTRAN_newdate(nco_long *,int *);
extern nco_long arm_base_time_get(int);
extern nco_long newdate(nco_long,int);
extern nm_id_sct *dmn_lst_ass_var(int,nm_id_sct *,int,int *);
extern nm_id_sct *dmn_lst_mk(int,char **,int);
extern nm_id_sct *lst_heapsort(nm_id_sct *,int,bool);
extern nm_id_sct *var_lst_add_crd(int,int,int,nm_id_sct *,int *);
extern nm_id_sct *var_lst_ass_crd_add(int,nm_id_sct *,int *);
extern nm_id_sct *var_lst_crd_xcl(int,int,nm_id_sct *,int *);
extern nm_id_sct *var_lst_mk(int,int,char **,bool,int *);
extern nm_id_sct *var_lst_xcl(int,int,nm_id_sct *,int *);
extern ptr_unn mss_val_mk(nc_type);
extern unsigned short dbg_lvl_get(void);
extern var_sct *scl_dbl_mk_var(double);
extern var_sct *var_avg(var_sct *,dmn_sct **,int,int);
extern var_sct *var_conform_dim(var_sct *,var_sct *,var_sct *,bool,bool *);
extern var_sct *var_conform_type(nc_type,var_sct *);
extern var_sct *var_dpl(var_sct *);
extern var_sct *var_fll(int,int,char *,dmn_sct **,int);
extern var_sct *var_free(var_sct *);
extern void Exit_gracefully(void);
extern void FORTRAN_add_double_precision(long *,int *,double *,long *,double *,double *);
extern void FORTRAN_add_real(long *,int *,float *,long *,float *,float *);
extern void FORTRAN_avg_reduce_double_precision(long *,long *,int *,double *,long *,double *,double *);
extern void FORTRAN_avg_reduce_real(long *,long *,int *,float *,long *,float *,float *);
extern void FORTRAN_divide_double_precision(long *,int *,double *,double *,double *);
extern void FORTRAN_divide_real(long *,int *,float *,float *,float *);
extern void FORTRAN_multiply_double_precision(long *,int *,double *,double *,double *);
extern void FORTRAN_multiply_real(long *,int *,float *,float *,float *);
extern void FORTRAN_normalize_double_precision(long *,int *,double *,long *,double *);
extern void FORTRAN_normalize_real(long *,int *,float *,long *,float *);
extern void FORTRAN_subtract_double_precision(long *,int *,double *,double *,double *);
extern void FORTRAN_subtract_real(long *,int *,float *,float *,float *);
extern void arm_time_install(int,nco_long);
extern void att_cpy(int,int,int,int);
extern void cast_nctype_void(nc_type,ptr_unn *);
extern void cast_void_nctype(nc_type,ptr_unn *);
extern void copyright_prn(char *,char *);
extern void dmn_dfn(char *,int,dmn_sct **,int);
extern void dmn_lmt_mrg(dmn_sct **,int,lmt_sct *,int);
extern void dmn_xrf(dmn_sct *,dmn_sct *);
extern void fl_cmp_err_chk(void);
extern void fl_cp(char *,char *);
extern void fl_mv(char *,char *);
extern void fl_out_cls(char *,char *,int);
extern void fl_rm(char *);
extern void hst_att_cat(int,char *);
extern void index_alpha(int,char **,int *);
extern void indexx(int,int *,int *);
extern void lmt_evl(int,lmt_sct *,long,bool);
extern void mss_val_cp(var_sct *,var_sct *);
extern void nco_lib_vrs_prn(void);
extern void ncar_csm_date(int,var_sct **,int);
extern void *nco_calloc(size_t lmn_nbr,size_t lmn_sz);
extern void *nco_malloc(size_t size);
extern void *nco_realloc(void *ptr, size_t size);
extern void rec_crd_chk(var_sct *,char *,char *,long,long);
extern void usg_prn(void);
extern void val_conform_type(nc_type,ptr_unn,nc_type,ptr_unn);
extern void var_add(nc_type,long,int,ptr_unn,long *,ptr_unn,ptr_unn);
extern void var_avg_reduce_ttl(nc_type,long,long,int,ptr_unn,long *,ptr_unn,ptr_unn);
extern void var_avg_reduce_min(nc_type,long,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_avg_reduce_max(nc_type,long,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_copy(nc_type,long,ptr_unn,ptr_unn);
extern void var_dfn(int,char *,int,var_sct **,int,dmn_sct **,int);
extern void var_dmn_xrf(var_sct *);
extern void var_divide(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_get(int,var_sct *);
extern void var_lst_convert(int,nm_id_sct *,int,dmn_sct **,int,var_sct ***,var_sct ***);
extern void var_lst_divide(var_sct **,var_sct **,int,bool,dmn_sct **,int,var_sct ***,var_sct ***,int *,var_sct ***,var_sct ***,int *);
extern void var_mask(nc_type,long,int,ptr_unn,double,int,ptr_unn,ptr_unn);
extern void var_max_bnr(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_min_bnr(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_multiply(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void nco_opr_drv(int,int,var_sct *, var_sct * );
extern void var_normalize(nc_type,long,int,ptr_unn,long *,ptr_unn);
extern void var_normalize_sdn(nc_type,long,int,ptr_unn,long *,ptr_unn);
extern void var_refresh(int,var_sct *);
extern void var_sqrt(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2);
extern void var_srt_zero(var_sct **,int);
extern void var_subtract(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_val_cpy(int,int,var_sct **,int);
extern void var_xrf(var_sct *,var_sct *);
extern void var_zero(nc_type,long,ptr_unn);
extern void vec_set(nc_type,long,ptr_unn,double);
extern void zero_long(long,long *op1);

extern var_sct * /* O [var] Variable after (possible) conversion */
nco_typ_cnv_rth  /* [fnc] Convert char, short, long, int types to doubles before arithmetic */
(var_sct *var, /* I/O [var] Variable to be considered for conversion */
 int nco_op_typ); /* I [enm] Operation type */

extern int ncvarid_or_die /* O [enm] Variable ID */
(int nc_id, /* I [enm] File ID */
 char *var_nm); /* I [sng] Variable name */
 
extern int ncdimid_or_die /* O [enm] Dimension ID */
(int nc_id, /* I [enm] File ID */
 char *dmn_nm); /* I [sng] Dimension name */

extern var_sct * /* O [sct] Variable reverted to previous type */
nco_cnv_var_typ_dsk  /* [fnc] Revert variable to previous type */
(var_sct *var); /* I [sct] Variable to be reverted */

extern bool /* O [flg] Variable is packed */
pck_dsk_inq /* [fnc] Check whether variable is packed */
(int nc_id, /* I [idx] netCDF file ID */
 var_sct *var); /* I/O [sct] Variable */

extern var_sct * /* O [sct] Unpacked variable */
var_upk /* [fnc] Unpack variable in memory */
(var_sct *var); /* I/O [sct] Variable to be unpacked */

extern int /* O [enm] Return code */
var_dfl_set /* [fnc] Set defaults for each member of variable structure */
(var_sct *var); /* [sct] Pointer to variable strucutre to initialize to defaults */

extern var_sct * /* O [sct] Output netCDF variable structure representing val */
scl_mk_var /* [fnc] Convert scalar value of any type into NCO variable */
(val_unn val, /* I [frc] Scalar value to turn into netCDF variable */
 nc_type val_typ); /* I [enm] netCDF type of value */

extern var_sct * /* O [sct] Output netCDF variable structure representing value */
scl_ptr_mk_var /* [fnc] Convert void pointer to scalar of any type into NCO variable */
(ptr_unn val_ptr_unn, /* I [unn] Pointer union to scalar value to turn into netCDF variable */
 nc_type val_typ); /* I [enm] netCDF type of pointer/value */

extern var_sct * /* O [sct] Packed variable */
var_pck /* [fnc] Pack variable in memory */
(var_sct *var, /* I/O [sct] Variable to be packed */
 nc_type typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
 bool USE_EXISTING_PCK); /* I [flg] Use existing packing scale_factor and add_offset */

extern double /* O [frc] Double precision representation of var->val.?p[0] */
ptr_unn_2_scl_dbl /* [fnc] Convert first element of NCO variable to a scalar double */
(ptr_unn val, /* I [sct] Pointer union to variable values */
 nc_type type); /* I [enm] Type of values pointed to by pointer union */

extern int /* O [enm] Return code */
nco_openmp_ini(); /* [fnc] Set up OpenMP multi-threading environment */

extern int /* O [enm] Return code */
nco_var_prc_crr_prn /* [fnc] Print name of current variable */
(int idx, /* I [idx] Index of current variable */
 char *var_nm); /* I [sng] Variable name */

extern var_sct * /* O [sct] Packed variable */
nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
(var_sct *var, /* I/O [sct] Variable to be packed */
 int nco_pck_typ); /* [enm] Packing operation type */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_H */
