/* Purpose: netCDF arithmetic processor -  */
/* prs_cls -- symbol table - data members & class methods */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#ifndef PRS_CLS_HH
#define PRS_CLS_HH

#include <vector>
#include <string>
#include <stdio.h>
#include "nco.h"
#include "NcapVector.hh"
#include "NcapVarVector.hh"
#include "NcapVar.hh"

// Forward declaration
class fmc_cls;

 class prs_cls{ /* prs_cls */
  public:
  char *fl_in; /* [sng] Input data file */
  int in_id; /* [id] Input data file ID */
  char *fl_out; /* [sng] Output data file */
  int fl_out_fmt; /* format of output file */
  int out_id;   /* [id] Output data file ID -Handle for reading & writing */
  int out_id_readonly; /* [id] Output data file ID -Handle for reading only */
  NcapVector<dmn_sct*> &dmn_in_vtr;        //Vector of dimensions in input file nb doesn't change
  NcapVector<dmn_sct*> &dmn_out_vtr;       //Vector of dimensions in output file file
  NcapVector<dmn_cmn_sct*> dmn_cmn_vtr;    // Vector of dmn_cmn -updated regularly from dmn_ot_vtr
  std::vector<fmc_cls> &fmc_vtr;           //List of functions/methods nb doesn't change
  NcapVarVector &var_vtr;                  // list of attributes & variables
  NcapVarVector &int_vtr;                // stores vars/atts in FIRST PARSE
  NcapVarVector thr_vtr;                 // Temp store for atts in a parallel run  
  bool ntl_scn;                          // [flg] Initial scan of script 
  bool FORTRAN_IDX_CNV;                  //Use fortran convention with hyperslab indices
  bool ATT_PROPAGATE;                    //Var on LHS gets attributtes from the leftermost var on the RHS
  bool ATT_INHERIT;                      //Var on LHS inherits attributtes from var of the same name
                                         // in the input file 
  bool NCAP_MPI_SORT;                    // sort exressions after second parse for MPI optimization
  bool NCAP4_FILL;                       //if true Ouptut file is netcdf4 & missing value="_FillValue"
  bool FLG_CLL_MTH;                      // if true then add @cell_methods attribute to var for an agg_cls operation
  cnk_sct *cnk_in; /* [nbr] Chunk sizes */
  int dfl_lvl; /* [enm] Deflate level */

  // Constructor
  prs_cls( NcapVector<dmn_sct*> &p_dmn_in_vtr, 
		NcapVector<dmn_sct*> &p_dmn_out_vtr, 
	        std::vector<fmc_cls>  &p_fmc_vtr,
                NcapVarVector &p_var_vtr,
	        NcapVarVector &p_int_vtr) :
                dmn_in_vtr(p_dmn_in_vtr)   ,
                dmn_out_vtr(p_dmn_out_vtr) ,
		fmc_vtr(p_fmc_vtr)         ,
                var_vtr(p_var_vtr)         ,
		int_vtr(p_int_vtr) { ; }
    

   // Copy Constructor 
   prs_cls( const prs_cls &prs_cpy) : 
         dmn_in_vtr(prs_cpy.dmn_in_vtr), 
         dmn_out_vtr(prs_cpy.dmn_out_vtr),  
         fmc_vtr(prs_cpy.fmc_vtr),       
         var_vtr(prs_cpy.var_vtr),            
         int_vtr(prs_cpy.int_vtr) {       

     dmn_cmn_vtr=prs_cpy.dmn_cmn_vtr;
     thr_vtr=prs_cpy.thr_vtr;  
     fl_in=prs_cpy.fl_in;
     in_id=prs_cpy.in_id;
     fl_out=prs_cpy.fl_out;
     fl_out_fmt=prs_cpy.fl_out_fmt;
     out_id=prs_cpy.out_id;         
     out_id_readonly=prs_cpy.out_id_readonly;         

     ntl_scn=prs_cpy.ntl_scn;
     FORTRAN_IDX_CNV=prs_cpy.FORTRAN_IDX_CNV;
     ATT_PROPAGATE=prs_cpy.ATT_PROPAGATE; 
     ATT_INHERIT=prs_cpy.ATT_INHERIT; 
     NCAP_MPI_SORT=prs_cpy.NCAP_MPI_SORT;
     NCAP4_FILL=prs_cpy.NCAP4_FILL;
     FLG_CLL_MTH=prs_cpy.FLG_CLL_MTH;
     dfl_lvl=prs_cpy.dfl_lvl;
     cnk_in=prs_cpy.cnk_in;
   }

   // = operator
   prs_cls& operator= (const prs_cls &prs_cpy)  {
      
     *this=prs_cpy;
     return *this;  
     
     /*
     // check for self-assignment    
     if( &prs_cpy == this) 
       return *this;
  
     fmc_vtr=prs_cpy.fmc_vtr;      
     dmn_out_vtr=prs_cpy.dmn_out_vtr;  
     fmc_vtr=prs_cpy.fmc_vtr;      
     var_vtr=prs_cpy.var_vtr;            
     int_vtr=prs_cpy.int_vtr;       
     */
       
     fl_in=prs_cpy.fl_in;
     in_id=prs_cpy.in_id;
     fl_out=prs_cpy.fl_out;
     fl_out_fmt=prs_cpy.fl_out_fmt;
     out_id=prs_cpy.out_id;         
     out_id_readonly=prs_cpy.out_id_readonly;     

     ntl_scn=prs_cpy.ntl_scn;
     FORTRAN_IDX_CNV=prs_cpy.FORTRAN_IDX_CNV;
     ATT_PROPAGATE=prs_cpy.ATT_PROPAGATE; 
     ATT_INHERIT=prs_cpy.ATT_INHERIT; 
     NCAP_MPI_SORT=prs_cpy.NCAP_MPI_SORT;
     NCAP4_FILL=prs_cpy.NCAP4_FILL;
     FLG_CLL_MTH=prs_cpy.FLG_CLL_MTH;
     dfl_lvl=prs_cpy.dfl_lvl;      
     cnk_in=prs_cpy.cnk_in;
     return *this;
   }

var_sct *                  /* O [sct] initialized variable */
ncap_var_init(
const std::string &snm,    /* I [sng] variable name constant */
bool bfll);                /* if true fill var with data */ 

int                        /* O [flg] non-zero if var is in Input or Ouptut or int_vtr*/
ncap_var_init_chk(         
const std::string &snm     /* I [sng] variable name constant */
);                       

int                        /* O  [bool] bool - true if sucessful */
ncap_var_write             /*    [fnc] Write var to output file prs_arg->fl_out */ 
(var_sct *var,             /* I  [sct] variable to be written - freed at end */  
 bool bram);               /* I  [bool] true if a ram only variable */

int                        /* O  [bool] bool - true if sucessful */
ncap_var_write_slb(        /*    [fnc] Write hyperslab to O var already exists in O */ 
var_sct *var               /* I  [bool] var contains data + hyperslab limits */   
);
                              
int                         /* O  [bool] bool - true if sucessful */
ncap_var_write_wrp(         /*    [fnc] Write hyperslab or regular var */ 
var_sct *var,               /* I  [sct] data + limits if needed   */
bool bram,                  /* I  [flg] true if ram variable */     
bool bslb);                 /* I  [flg] true if slab to write */      

int 
ncap_var_write_omp(
var_sct *var,
bool bram);

void 
ncap_def_ntl_scn           /* define variables captured on first parse */
(void);

int 
ncap_get_cnk_sz(
var_sct *var);

     void
     ncap_pop_dmn_cmn(void);

     void
     ncap_pop_var_dmn_cmn(var_sct* var, dmn_cmn_sct *cmn);

 };

#endif // PRS_CLS_HH
