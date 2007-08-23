
/* Purpose: netCDF arithmetic processor -  */
/* prs_cls -- symbol table - data members & class methods */

/* Copyright (C) 1995--2007 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */



#ifndef INC_PRS_CLS_hh_
#define INC_PRS_CLS_hh_


#include <vector>
#include <string>
#include <stdio.h>
#include "nco.h"
#include "NcapVector.hh"
#include "NcapVarVector.hh"
#include "NcapVar.hh"

//forward declaration
class fmc_cls;


 class prs_cls{ /* prs_cls */
  public:
  char *fl_in; /* [sng] Input data file */
  int in_id; /* [id] Input data file ID */
  char *fl_out; /* [sng] Output data file */
  int out_id; /* [id] Output data file ID */

  NcapVector<dmn_sct*> &dmn_in_vtr;        //Vector of dimensions in input file nb doesn't change
  NcapVector<dmn_sct*> &dmn_out_vtr;       //Vector of dimensions in output file file
  std::vector<fmc_cls> &fmc_vtr;         //List of functions/methods nb doesn't change 
  NcapVarVector &var_vtr;                  // list of attributes & variables
  NcapVarVector &int_vtr;                  // stores vars/atts in FIRST PARSE
  bool ntl_scn;                          // [flg] Initial scan of script 
  bool FORTRAN_IDX_CNV;                  //Use fortran convention with hyperslab indices
  bool ATT_PROPAGATE;                    //Var on LHS gets attributtes from the leftermost var on the RHS
  bool ATT_INHERIT;                      //Var on LHS inherits attributtes from var of the same name
                                         // in the input file 
  bool NCAP_MPI_SORT;                    // sort exressions after second parse for MPI optimization
  int dfl_lvl;                           // Set Lempel-Ziv compression level                                              
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
		int_vtr(p_int_vtr) {; }

var_sct *                  /* O [sct] initialized variable */
ncap_var_init(
const std::string &snm,    /* I [sng] variable name constant */
bool bfll);                /* if true fill var with data */ 

int                        /* O  [bool] bool - true if sucessful */
ncap_var_write             /*    [fnc] Write var to output file prs_arg->fl_out */ 
(var_sct *var,             /* I  [sct] variable to be written - freed at end */  
 bool bram);               /* I  [bool] true if a ram only variable */

void 
ncap_def_ntl_scn           /* define variables captured on first parse */
(void); 

};


#endif
