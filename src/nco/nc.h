/* $Header: /data/zender/nco_20150216/nco/src/nco/nc.h,v 1.2 1998-08-18 16:56:50 zender Exp $ */

/* Typedefs and global variables for netCDF operators */ 

/* (c) Copyright 1995 University Corporation for Atmospheric Research/
   National Center for Atmospheric Research/
   Climate and Global Dynamics Division

   The file nc_copyright.txt contains the full copyright notice, or 
   you may contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance. */

#ifndef NC_H /* This include file has not yet been defined in the current source file */ 
#define NC_H

#define bool int
#define True 1
#define False 0

/*const short True=1;*/
/*const short False=0;*/
/*extern const short True=1;*/
/*extern const short False=0;*/

#ifdef MAIN_PROGRAM_FILE /* The current file contains main() */

/* Global variables and variables with scope limited to the main.c file allocated here */ 

int prg;
int prg_get(void){return prg;}

char *prg_nm;
char *prg_nm_get(void){return prg_nm;}

unsigned short dbg_lvl=0; /* Option D */
unsigned short dbg_lvl_get(void){return dbg_lvl;}

#else /* MAIN_PROGRAM_FILE is NOT defined, i.e., the current file does not contain main() */ 

/* External references to global variables are declared as extern here.
   Variables with local file scope in all files except the main.c file are allocated here. */ 

#endif /* MAIN_PROGRAM_FILE is NOT defined, i.e., the current file does not contain main() */

/* Enumerate the key values for all the netCDF filters */ 
#if ( ! defined SGI5 ) && ( ! defined SGI64 )
enum prg{
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
}; /* end enum */

enum{
  aed_append,
  aed_create,
  aed_delete,
  aed_modify,
  aed_overwrite
}; /* end enum */ 

enum{
  nc_op_eq,
  nc_op_ne,
  nc_op_lt,
  nc_op_gt,
  nc_op_le,
  nc_op_ge
}; /* end enum */
#else /* !SGI */
#define ncap 0
#define ncatted 1
#define ncdiff 2
#define ncea 3
#define ncecat 4
#define ncflint 5
#define ncks 6
#define ncra 7
#define ncrcat 8
#define ncrename 9
#define ncwa 10

#define nc_op_eq 0
#define nc_op_ne 1
#define nc_op_lt 2
#define nc_op_gt 3
#define nc_op_le 4
#define nc_op_ge 5

#define aed_append 0
#define aed_create 1
#define aed_delete 2
#define aed_modify 3
#define aed_overwrite 4

#endif /* !SGI */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif /* SUN4 */

typedef struct {
  char *nm;
  char *min_sng; /* user-specified string for dimension minimum */ 
  char *max_sng; /* user-specified string for dimension maximum */ 
  char *srd_sng; /* user-specified string for dimension stride */ 
  int id; /* dimension ID */ 
  long min_idx; /* index of minimum requested value in the dimension */ 
  long max_idx; /* index of maximum requested value in the dimension */ 
  double min_val; /* double precision representation of minimum value of coord requested or implied */ 
  double max_val; /* double precision representation of maximum value of coord requested or implied */ 
  long srt; /* index to start of hyperslab */ 
  long end; /* index to end of hyperslab */ 
  long cnt; /* # of valid elements in this dimension (including effects of stride and wrapping) */ 
  long srd; /* stride of hyperslab */ 
} lim_sct;

typedef struct{
  char *nm;
  int id;
} nm_id_sct;

typedef struct{
  char *old_nm;
  char *new_nm;
  int id;
} rnm_sct;

typedef union {
  float *fp;
  double *dp;
  nclong *lp;
  short *sp;
  signed char *cp;
  unsigned char *bp;
  void *vp;
} ptr_unn;

typedef union {
  float f;
  double d;
  nclong l;
  short s;
  char c;
} val_unn;

typedef struct{
  char *att_nm; /* Name of attribute */ 
  char *var_nm; /* Name of variable, or NULL for global attribute */ 
  int id; /* Variable ID or NC_GLOBAL ( = -1) for global attribute */ 
  long sz; /* Number of elements in attribute */ 
  nc_type type; /* Type of attribute */ 
  ptr_unn val; /* Pointer to attribute value */ 
  short mode; /* action to perform with attribute */ 
} aed_sct;

typedef struct {
  char *nm;
  nc_type type;
  int sz;
  char fmt[5];
  ptr_unn val;
} att_sct;

typedef struct dim_sct_tag{
  char *nm; /* name */ 
  int id; /* dimension ID */ 
  int nc_id; /* file ID */
  long sz; /* full size of dimension in file (NOT the hyperslabbed size) */ 
  short is_rec_dim; /* is this the record dimension? */ 
  short is_crd_dim; /* is this a coordinate dimension? */ 
  int cid; /* Variable ID of the associated coordinate, if any */ 
  nc_type type; /* type of coordinate, if applicable */ 
  char fmt[5]; /* hint for printf()-style formatting */ 
  long srt; /* index to start of hyperslab */ 
  long end; /* index to end of hyperslab */ 
  long cnt; /* # of valid elements in this dimension (including effects of stride and wrapping) */ 
  long srd; /* stride of hyperslab */ 
  ptr_unn val; /* buffer to hold hyperslab */ 
  struct dim_sct_tag *xrf; /* cross-reference to associated dimension structure (usually the structure for the dimension on output) */ 
} dim_sct;

typedef struct var_sct_tag{
  char *nm; /* name */ 
  int id; /* variable ID */
  int nc_id; /* file ID */
  nc_type type; /* type of variable */ 
  int nbr_dim; /* number of dimensions of variable in input file */ 
  short is_rec_var; /* is this a record variable? */ 
  short is_crd_var; /* is this a coordinate variable? */ 
  long sz; /* number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */ 
  long sz_rec; /* number of elements in one record of hyperslab */ 
  int nbr_att; /* number of attributes */ 
  int has_mss_val; /* is there a missing_value attribute? */ 
  ptr_unn mss_val; /* value of the missing_value attribute, if any */ 
  int cid; /* Dimension ID of the associated coordinate, if any */ 
  char fmt[5]; /* hint for printf()-style formatting */ 
  dim_sct **dim; /* pointers to full dimension structures */ 
  int *dim_id; /* contiguous vector of dimension IDs */ 
  long *srt; /* contiguous vector of indices to start of hyperslab */ 
  long *end; /* contiguous vector of indices to end of hyperslab */ 
  long *cnt; /* contiguous vector of lengths of hyperslab */ 
  long *srd; /* contiguous vector of stride of hyperslab */ 
  ptr_unn val; /* buffer to hold hyperslab */ 
  long *tally; /* number of valid operations performed so far */ 
  struct var_sct_tag *xrf; /* cross-reference to associated variable structure (usually the structure for the variable on output) */ 
} var_sct;

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
#define FORTRAN_foo FOO
#endif /* CRAY */ 
#ifdef RS6K
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
#define FORTRAN_foo foo
#endif /* RS6K */ 
#ifdef LINUX /* NB: g77 subroutines have two underscores, while g77 functions (e.g., newdate()) have only one! */ 
#define FORTRAN_add_real add_real__
#define FORTRAN_add_double_precision add_double_precision__
#define FORTRAN_avg_reduce_real avg_reduce_real__
#define FORTRAN_avg_reduce_double_precision avg_reduce_double_precision__
#define FORTRAN_normalize_real normalize_real__
#define FORTRAN_normalize_double_precision normalize_double_precision__
#define FORTRAN_newdate newdate_
#define FORTRAN_subtract_real subtract_real__
#define FORTRAN_subtract_double_precision subtract_double_precision__
#define FORTRAN_multiply_real multiply_real__
#define FORTRAN_multiply_double_precision multiply_double_precision__
#define FORTRAN_divide_real divide_real__
#define FORTRAN_divide_double_precision divide_double_precision__
#define FORTRAN_foo foo__
#endif /* LINUX */ 
#if ( defined SUN4 ) || ( defined SUN4SOL2 ) || ( defined SUNMP ) || ( defined SGI5 ) || ( defined SGI64 )
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
#define FORTRAN_foo foo_
#endif /* SUN-style */ 

/* Function prototypes */ 
extern bool ncar_csm_inq(int);
extern bool arm_inq(int);
extern char **fl_lst_mk(char **,int,int,int *,char **);
extern char **lst_prs(char *,const char *,int *);
extern char *c_type_nm(nc_type);
extern char *cmd_ln_sng(int,char **);
extern char *cvs_vrs_prs();
extern char *fl_mk_lcl(char *,char *,int *);
extern char *fl_nm_prs(char *,int,int *,char **,int,char **,char *);
extern char *fl_out_open(char *,bool,bool,int *);
extern char *fortran_type_nm(nc_type);
extern char *nc_type_nm(nc_type);
extern char *prg_nm_get(void);
extern char *prg_prs(char *,int *);
extern dim_sct *dim_dup(dim_sct *);
extern dim_sct *dim_fll(int,int,char *);
extern double arm_time_mk(int,double);
extern int mss_val_get(int,var_sct *);
extern int op_prs(char *);
extern int prg_get(void);
extern lim_sct *lim_prs(int,char **);
extern lim_sct lim_dim_mk(int,int,lim_sct *,int,bool);
extern nclong arm_base_time_get(int);
extern nm_id_sct *dim_lst_ass_var(int,nm_id_sct *,int,int *);
extern nm_id_sct *dim_lst_mk(int,char **,int);
extern nm_id_sct *lst_heapsort(nm_id_sct *,int);
extern nm_id_sct *var_lst_add_crd(int,int,int,nm_id_sct *,int *);
extern nm_id_sct *var_lst_ass_crd_add(int,nm_id_sct *,int *);
extern nm_id_sct *var_lst_crd_xcl(int,int,nm_id_sct *,int *);
extern nm_id_sct *var_lst_mk(int,int,char **,bool,int *);
extern nm_id_sct *var_lst_xcl(int,int,nm_id_sct *,int *);
extern ptr_unn mss_val_mk(nc_type);
extern unsigned short dbg_lvl_get(void);
extern var_sct *scl_dbl_mk_var(double);
extern var_sct *var_avg(var_sct *,dim_sct **,int);
extern var_sct *var_conform_dim(var_sct *,var_sct *,var_sct *);
extern var_sct *var_conform_type(nc_type,var_sct *);
extern var_sct *var_dup(var_sct *);
extern var_sct *var_fll(int,int,char *,dim_sct **,int);
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
extern void arm_time_install(int,nclong);
extern void att_cpy(int,int,int,int);
extern void cast_nctype_void(nc_type,ptr_unn *);
extern void cast_void_nctype(nc_type,ptr_unn *);
extern void copyright_prn(char *,char *);
extern void dim_def(char *,int,dim_sct **,int);
extern void dim_lim_merge(dim_sct **,int,lim_sct *,int);
extern void dim_xrf(dim_sct *,dim_sct *);
extern void fl_cmp_err_chk();
extern void fl_cp(char *,char *);
extern void fl_mv(char *,char *);
extern void fl_out_close(char *,char *,int);
extern void fl_rm(char *);
extern void hst_att_cat(int,char *);
extern void indexx(int,int *,int *);
extern void lim_evl(int,lim_sct *,bool);
extern void ncar_csm_date(int,var_sct **,int);
extern void rec_crd_chk(var_sct *,char *,char *,long,long);
extern void usg_prn(void);
extern void val_conform_type(nc_type,ptr_unn,nc_type,ptr_unn);
extern void var_add(nc_type,long,int,ptr_unn,long *,ptr_unn,ptr_unn);
extern void var_avg_reduce(nc_type,long,long,int,ptr_unn,long *,ptr_unn,ptr_unn);
extern void var_def(int,char *,int,var_sct **,int,dim_sct **,int);
extern void var_dim_xrf(var_sct *);
extern void var_divide(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_get(int,var_sct *);
extern void var_lst_convert(int,nm_id_sct *,int,dim_sct **,int,var_sct ***,var_sct ***);
extern void var_lst_divide(var_sct **,var_sct **,int,bool,dim_sct **,int,var_sct ***,var_sct ***,int *,var_sct ***,var_sct ***,int *);
extern void var_mask(nc_type,long,int,ptr_unn,double,int,ptr_unn,ptr_unn);
extern void var_multiply(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_normalize(nc_type,long,int,ptr_unn,long *,ptr_unn);
extern void var_refresh(int,var_sct *);
extern void var_srt_zero(var_sct **,int);
extern void var_subtract(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
extern void var_val_cpy(int,int,var_sct **,int);
extern void var_xrf(var_sct *,var_sct *);
extern void var_zero(nc_type,long,ptr_unn);
extern void zero_long(long,long *op1);

#endif /* NC_H */ 
