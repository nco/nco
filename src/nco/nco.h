/* $Header: /data/zender/nco_20150216/nco/src/nco/nco.h,v 1.47 2002-12-30 02:56:14 zender Exp $ */

/* Purpose: netCDF operator definitions */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco.h" *//* netCDF operator definitions */

#ifndef NCO_H /* Contents have not yet been inserted in current source file */
#define NCO_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

/* Personal headers */

/* Encapsulate C++ const usage in C99-safe macro 
   C++ compilers will use type-safe version
   C89 and C99 compilers will use version as type-safe as possible without */
/* fxm: This is incomplete and needs to be extended and applied to all source */
#ifdef __cplusplus
#define CST_X_PTR_CST_PTR_CST_Y(x,y) const x * const * const y
#define X_CST_PTR_CST_PTR_Y(x,y) x const * const * y
#else /* !__cplusplus */
#define CST_X_PTR_CST_PTR_CST_Y(x,y) x * const * const y
#define X_CST_PTR_CST_PTR_Y(x,y) x * const * y
#endif /* !__cplusplus */

#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */

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
#define byte_CEWI 0
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
  
#ifndef EXIT_SUCCESS /* Most likely this is a SUN4 machine */
#define EXIT_SUCCESS 0
#endif /* SUN4 */
#ifndef EXIT_FAILURE /* Most likely this is a SUN4 machine */
#define EXIT_FAILURE 1
#endif /* SUN4 */

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
  
  enum aed{ /* [enm] Attribute editor mode */
    aed_append,
    aed_create,
    aed_delete,
    aed_modify,
    aed_overwrite
  }; /* end enum */
  
  enum lmt_typ{ /* [enm] Limit type */
    lmt_crd_val, /* 0, Coordinate value limit */
    lmt_dmn_idx /* 1, Dimension index limit */
  }; /* end lmt_typ enum */
  
  enum nco_rlt_opr{ /* [enm] Arithmetic relations (comparisons) for masking */
    nco_op_eq, /* Equality */
    nco_op_ne, /* Inequality */
    nco_op_lt, /* Less than */
    nco_op_gt, /* Greater than */
    nco_op_le, /* Less than or equal to */
    nco_op_ge /* Greater than or equal to */
  }; /* end enum */
  
  enum nco_op_typ{ /* [enm] Operation type */
    nco_op_avg, /* [enm] Average */
    nco_op_min, /* [enm] Minimum value */
    nco_op_max, /* [enm] Maximum value */
    nco_op_ttl, /* [enm] Linear sum */
    nco_op_sqravg, /* [enm] Square of mean */
    nco_op_avgsqr, /* [enm] Mean of sum of squares */
    nco_op_sqrt, /* [enm] Square root of mean */
    nco_op_rms, /* [enm] Root-mean-square (normalized by N) */
    nco_op_rmssdn, /* [enm] Root-mean square normalized by N-1 */
    nco_op_nil /* [enm] Nil or undefined operation type  */
  }; /* end nco_op_typ enum */

  /* Following typedef's from Nie02 */
  typedef enum { /* [enm] Node enumerator Nie02 nodeEnum */
    typ_scv, /* [enm] Scalar value */
    typ_sym, /* [enm] Symbol identifier */
    typ_opr /* [enm] Operator */
  } nod_typ_enm;
  /* end enumeration section */
  
  /* Limit structure */
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

/* Container for limit structures */
typedef struct {
 char *dmn_nm; /* dimension name */
 long dmn_cnt;    /* total number of hyperslabs to extract */
 int lmt_dmn_nbr;    /* number of lmt args */
 lmt_sct **lmt_dmn; /* list of limit structures associated with each dimension */  
} lmt_all;

  /* Name ID structure */
  typedef struct{ /* nm_id_sct */
    char *nm;
    int id;
  } nm_id_sct;
  
  /* Rename structure */
  typedef struct{ /* rnm_sct */
    char *old_nm;
    char *new_nm;
    int id;
  } rnm_sct;

  /* Pointer union */
  typedef union{ /* ptr_unn */
    float *fp;
    double *dp;
    nco_long *lp; /* NC_INT is stored in native type nco_long */
    short *sp;
    unsigned char *cp; /* Range of unsigned char is 0--255 */
    signed char *bp; /* Range of signed char is -127--127 */
    void *vp;
  } ptr_unn;

  /* Value union */
  typedef union{ /* val_unn */
    float f;
    double d;
    nco_long l;
    short s;
    unsigned char c; /* Range of unsigned char is 0--255 */
    signed char b; /* Range of signed char is -127--127 */
  } val_unn;

  /* Scalar value structure */
  typedef struct{ /* scv_sct */
    val_unn val; /* [sct] Value */
    nc_type type; /* [enm] netCDF typ */
    nod_typ_enm nod_typ; /* [enm] Node type */
  } scv_sct;      

  /* Attribute editing structure */
  typedef struct{ /* aed_sct */
    char *att_nm; /* Name of attribute */
    char *var_nm; /* Name of variable, or NULL for global attribute */
    int id; /* Variable ID or NC_GLOBAL ( = -1) for global attribute */
    long sz; /* Number of elements in attribute */
    nc_type type; /* Type of attribute */
    ptr_unn val; /* Pointer to attribute value */
    short mode; /* Action to perform with attribute */
  } aed_sct;
  
  /* Attribute structure */
  typedef struct{ /* att_sct */
    char *nm;
    nc_type type;
    long sz;
    char fmt[5];
    ptr_unn val;
  } att_sct;
  
  /* Dimension structure */
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
  } dmn_sct; /* end dmn_sct_tag */
  
  /* Each member of var_sct structure should be initialized to default in var_dfl_set()
     Each pointer member of var_sct structure should be freed in nco_var_free()
     Each pointer member of var_sct structure should be copied in nco_var_dpl() */
  /* Variable structure */
  typedef struct var_sct_tag{ /* var_sct */
    char *nm; /* [sng] Variable name */
    int id; /* [id] Variable ID */
    int nc_id; /* [id] File ID */
    int nbr_dim; /* [nbr] Number of dimensions of variable in input file */
    nc_type type; /* [enm] Type of variable in RAM */
    nc_type typ_dsk; /* [enm] Type of variable on disk (never changes) */
    short is_rec_var; /* [flg] Is this a record variable? */
    short is_crd_var; /* [flg] Is this a coordinate variable? */
    long sz; /* [nbr] Number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */
    long sz_rec; /* [nbr] Number of elements in one record of hyperslab */
    int nbr_att; /* [nbr] Number of attributes */
    int has_mss_val; /* [flg] Is there a missing_value attribute? */
    ptr_unn mss_val; /* [frc] Value of missing_value attribute, if any (mss_val stored in this structure must be same type as variable) */
    int cid; /* [id] Dimension ID of associated coordinate, if any */
    char fmt[5]; /* [sng] Hint for printf()-style formatting */
    dmn_sct **dim; /* [sct] Pointers to full dimension structures */
    int *dmn_id; /* [id] Contiguous vector of dimension IDs */
    long *srt; /* [id] Contiguous vector of indices to start of hyperslab */
    long *end; /* [id] Contiguous vector of indices to end of hyperslab */
    long *cnt; /* [id] Contiguous vector of lengths of hyperslab */
    long *srd; /* [id] Contiguous vector of stride of hyperslab */
    ptr_unn val; /* [bfr] Buffer to hold hyperslab */
    long *tally; /* [nbr] Number of valid operations performed so far */
    struct var_sct_tag *xrf; /* [sct] Cross-reference to associated variable structure (usually the structure for variable on output) */
    int pck_dsk; /* [flg] Variable is packed on disk (valid scale_factor, add_offset, or both attributes exist) */
    int pck_ram; /* [flg] Variable is packed in memory (valid scale_factor, add_offset, or both attributes exist) */
    int has_scl_fct; /* [flg] Valid scale_factor attribute exists */
    int has_add_fst; /* [flg] Valid add_offset attribute exists */
    ptr_unn scl_fct; /* [frc] Value of scale_factor attribute of type typ_upk */
    ptr_unn add_fst; /* [frc] Value of add_offset attribute of type typ_upk */
    nc_type typ_pck; /* [enm] Type of variable when packed (on disk). typ_pck = typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
    nc_type typ_upk; /* [enm] Type of variable when unpacked (expanded) (in memory) */
  } var_sct; /* end var_sct_tag */
  
  /* Function prototypes */
  char *prg_nm_get(void);
  int prg_get(void);
  unsigned short dbg_lvl_get(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_H */
