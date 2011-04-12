/* $Header: /data/zender/nco_20150216/nco/src/nco++/prs_cls.cc,v 1.23 2011-04-12 03:19:47 zender Exp $ */

/* Purpose: netCDF arithmetic processor */
/* prs_cls -- symbol table - class methods */

/* Copyright (C) 1995--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

// Standard C++ headers
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

// Standard C headers
#include <assert.h>
#include <ctype.h>

// Personal headers
#include "ncap2_utl.hh"
#include "prs_cls.hh"
#include "ncap2_utl.hh" /* netCDF arithmetic processor */
#include "NcapVar.hh"
#include "sdo_utl.hh"

/********Begin prs_cls methods********************************************************/
/*************************************************************************************/


var_sct * 
prs_cls::ncap_var_init(
const std::string &snm, 
bool bfll){

  /* Purpose: Initialize variable structure, retrieve variable values from disk
     Parser calls ncap_var_init() when it encounters a new RHS variable */
  
  const char fnc_nm[]="prs_cls::ncap_var_init"; 
  bool bskp_npt=false;
  
  int idx;
  int dmn_var_nbr;
  int var_id;
  int rcd;
  int fl_id;
  char dmn_nm[NC_MAX_NAME];
  const char *var_nm;

  var_sct *var=NULL_CEWI;;
  
  NcapVar *Nvar;

  var_nm=snm.c_str();

  
  // INITIAL SCAN
    if(ntl_scn){
    // check int vtr
    Nvar=int_vtr.find(var_nm);
    // check var_vtr (output)  
    if(Nvar==NULL)
      Nvar=var_vtr.find(var_nm);
    
    if(Nvar) {
      var=Nvar->cpyVarNoData();
      return var;
    }
    bfll=false;
  }
 
  
  if(dbg_lvl_get() > 2 && !ntl_scn) {
    std::ostringstream os;
    os<< "Parser VAR action called ncap_var_init() to retrieve " <<var_nm <<" from disk";
    dbg_prn(fnc_nm,os.str());  
  }
 
  
  // FINAL SCAN
  // We have a  dilema -- its possible for a variable to exist in input & output 
  // with the var in output defined but empty - -this could occur with the
  // increment/decrement ops e.g time++, four+=10 or a LHS hyperslab 
  // e.g time(0:2)=666 or var on LHS & RHS at the same time time()=time*10;
  // So what we want to happen in these cases is read the original var from
  // input. --Later on in the script the empty var in output will be populated 
  // and subsequent reads will occur from  output 

  if(!ntl_scn){
    Nvar=var_vtr.find(var_nm);
    
    // var is defined in O and populated
    if(Nvar && Nvar->flg_stt==2 && !Nvar->flg_mem){
      var=Nvar->cpyVarNoData();

#ifdef _OPENMP
      fl_id= ( omp_in_parallel() ?out_id_readonly : out_id );
#else    
      fl_id=out_id;  
#endif
      var->tally=(long *)NULL;
      /* Retrieve variable values from disk into memory */
      if(bfll)
        (void)nco_var_get(fl_id,var); 

      return var;
    }

    // var is defined in O and populated && a RAM variable
    if(Nvar && Nvar->flg_stt==2 && Nvar->flg_mem){
      var=Nvar->cpyVar();
      return var;
    }
    
    
    // var is defined in O but NOT populated
    // Set flag so read is tried only from input
    // Maybe not the best solution ?
    // what else ??? 
    if(Nvar && Nvar->flg_stt==1 )
      bskp_npt=true;
     
  } // end !ntl_scn


  

  /* Check output file for var */  
  rcd=nco_inq_varid_flg(out_id,var_nm,&var_id);
  if(rcd == NC_NOERR && !bskp_npt){

#ifdef _OPENMP
    fl_id= ( omp_in_parallel() ?out_id_readonly : out_id );
#else    
    fl_id=out_id;  
#endif

    var=nco_var_fll(fl_id,var_id,var_nm,&dmn_out_vtr[0],dmn_out_vtr.size());
    var->tally=(long *)NULL;
    /* Retrieve variable values from disk into memory */
    if(bfll)
      (void)nco_var_get(fl_id,var); 

    return var;
  }
   
  /* Rest of function assumes var to be read is in Input */
  /* Check input file for ID */
  rcd=nco_inq_varid_flg(in_id,var_nm,&var_id);
  if(rcd != NC_NOERR){
    /* Return NULL if variable not in input or output file */
    std::ostringstream os;
    os<<"Unable to find variable " <<var_nm << " in " << fl_in <<" or " << fl_out;
    wrn_prn(fnc_nm,os.str());
    return NULL_CEWI;
  } /* end if */
    
  /* Find dimensions used in var
     Learn which are not already in output list dmn_out and output file
     Add these to output list and output file */

  fl_id=in_id;
    
  (void)nco_inq_varndims(fl_id,var_id,&dmn_var_nbr);
  if(dmn_var_nbr>0){
    int *dim_id=NULL_CEWI;
    dmn_sct *dmn_fd; 
    dmn_sct *dmn_nw;
    std::vector<dmn_sct*> dmn_tp_out;

    dim_id=(int *)nco_malloc(dmn_var_nbr*sizeof(int));
      
    (void)nco_inq_vardimid(fl_id,var_id,dim_id);
    for(idx=0;idx<dmn_var_nbr;idx++){ 
      // get dim name
      (void)nco_inq_dimname(fl_id,dim_id[idx],dmn_nm);
      // check if dim is already in output
      if(dmn_out_vtr.find(dmn_nm)) continue; 
      // Get dim from input list
      dmn_fd= dmn_in_vtr.find(dmn_nm);
      // not in list -- crash out
      if(dmn_fd == NULL_CEWI){
        std::ostringstream os;
        os<<"Unable to find dimension " <<dmn_nm << " in " << fl_in <<" or " << fl_out;
        err_prn(fnc_nm,os.str());
      }
	
      dmn_tp_out.push_back(dmn_fd);
        
    } // end idx

    // no longer needed
    (void)nco_free(dim_id);

    // define new dims in output if necessary  
    if(dmn_tp_out.size() >0){

#ifdef _OPENMP
      if( omp_in_parallel())
	err_prn(fnc_nm,"Attempt to go into netcdf define mode while in OPENMP parallel mode");
#endif

      (void)nco_redef(out_id);
      for(idx=0; idx< (int)dmn_tp_out.size();idx++){
        dmn_nw=nco_dmn_dpl(dmn_tp_out[idx]);
        (void)nco_dmn_xrf(dmn_nw,dmn_tp_out[idx]);
	(void)nco_dmn_dfn(fl_out,out_id,&dmn_nw,1);          
	(void)dmn_out_vtr.push_back(dmn_nw);

	if(dbg_lvl_get() > 2) {
          std::ostringstream os;
          os << "Found new dimension " << dmn_nw->nm << " in input variable " << var_nm <<" in file " <<fl_in;
          os << ". Defining dimension " << dmn_nw->nm << " in output file " << fl_out;
          dbg_prn(fnc_nm,os.str());
	}
      }// end idx

      (void)nco_enddef(out_id);

    } // end if 

  } // end if


   
      
  var=nco_var_fll(fl_id,var_id,var_nm,&dmn_out_vtr[0],dmn_out_vtr.size());
  /*  var->nm=(char *)nco_malloc((strlen(var_nm)+1UL)*sizeof(char));
      (void)strcpy(var->nm,var_nm); */
  /* Tally is not required yet since ncap does not perform cross-file operations (yet) */
  /* var->tally=(long *)nco_malloc_dbg(var->sz*sizeof(long),"Unable to malloc() tally buffer in variable initialization",fnc_nm);
     (void)nco_zero_long(var->sz,var->tally); */
  
   var->tally=(long *)NULL;
  /* Retrieve variable values from disk into memory */
  if(bfll)
    (void)nco_var_get(fl_id,var); 
  

  return var;

}            


int 
prs_cls::ncap_var_init_chk(
const std::string &var_nm){
int rcd;  
int var_id;
NcapVar *Nvar;


   // check output
   Nvar=var_vtr.find(var_nm);
   if(Nvar)
     return 1;
  
  // initial scan
   if(ntl_scn){
     // check int vtr
     Nvar=int_vtr.find(var_nm);
     if(Nvar)
       return 1;
   }   


   // Check output file for ID 
  rcd=nco_inq_varid_flg(out_id,var_nm.c_str(),&var_id);
  if(rcd == NC_NOERR)
    return 1;


   // Check input file for ID 
  rcd=nco_inq_varid_flg(in_id,var_nm.c_str(),&var_id);
  if(rcd == NC_NOERR)
    return 1;

  // var not in Input or Output or int_vtr
  return 0;

}


int 
prs_cls::ncap_var_write_slb( 
var_sct *var)
{
int bret;

 bret=ncap_var_write_wrp(var,false,true);
 return bret;

}


int 
prs_cls::ncap_var_write( 
var_sct *var,
bool bram){
int bret;

 bret=ncap_var_write_wrp(var,bram,false);
 return bret;

}


// We need this function for OpenMP Threading as ncap_var_write() and ncap_var_write_slb
// both write to Output -  nb only one thread can write!! 
int
prs_cls::ncap_var_write_wrp(
var_sct *var,
bool bram,
bool bslb){
int bret;

#ifdef _OPENMP
#pragma omp critical
#endif
    {
      if(bslb){
      
       // put the slab -nb var already defined+ populated in 0
       (void)nco_put_vars(out_id,var->id,var->srt,var->cnt,var->srd,var->val.vp,var->type);
       var=nco_var_free(var);       
       bret=1;  


      }else{  
          
       bret=ncap_var_write_omp(var,bram);
      } 
    } // end pragma

 return bret;


} // end ncap_var_write_wrp()

int 
prs_cls::ncap_var_write_omp(
var_sct *var,
bool bram){ 

  /* Purpose: Define variable in output file and write variable */
  /*  const char mss_val_sng[]="missing_value"; *//* [sng] Unidata standard string for missing value */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  const char fnc_nm[]="prs_cls::ncap_var_write"; 
  
  int rcd; /* [rcd] Return code */
  int var_out_id;
  
  bool bdef=false;
  NcapVar *Nvar;
  
#ifdef NCO_RUSAGE_DBG
  long maxrss; /* [B] Maximum resident set size */
#endif /* !NCO_RUSAGE_DBG */

  
  // INITIAL SCAN
  if(ntl_scn){
    Nvar=var_vtr.find(var->nm);
    if(Nvar) { 
      var=nco_var_free(var);
      return True;
    }
    
    Nvar=int_vtr.find(var->nm);
    if(Nvar) { 
      var=nco_var_free(var);
      return True;
    }  
    
    //Nvar=new NcapVar(var,"");
    Nvar=new NcapVar(var);
    Nvar->flg_mem=bram;
    int_vtr.push(Nvar);
    return True;
  } 
  
  // FINAL SCAN
  Nvar=var_vtr.find(var->nm);
  if(Nvar) {
    // temporary fix make typ_dsk same as type
    Nvar->var->typ_dsk=Nvar->var->type;
    bdef=true;

    //Possibly overwrite bram !!
    bram=Nvar->flg_mem;
      
    if(var->has_mss_val)
      (void)nco_mss_val_cp(var,Nvar->var);
    // delete missing value
    else if(Nvar->var->has_mss_val){
      Nvar->var->has_mss_val=False;
      Nvar->var->mss_val.vp=(void*)nco_free(Nvar->var->mss_val.vp);
    }      

  } 

  
  // Deal with a new RAM only variable
  if(!bdef && bram){
    //NcapVar *NewNvar=new NcapVar(var,"");
    NcapVar *NewNvar=new NcapVar(var);
    NewNvar->flg_mem=bram;
    NewNvar->flg_stt=2;
    NewNvar->var->id=-1;
    NewNvar->var->nc_id=-1;
    var_vtr.push(NewNvar);
    return True;
  }

  
  // Deal with a an existing RAM variable
  if(bdef && bram){
    var_sct *var_ref;
    void *vp_swp;
        
    // De-reference
    var_ref=Nvar->var;


    
    // check sizes are the same 
    if(var_ref->sz != var->sz) {
      std::ostringstream os;
      os<< "RAM Variable "<< var->nm << " size=" << var->sz << " has aleady been saved in ";
      os<< fl_out << " with size=" << var_ref->sz;
      
      wrn_prn(fnc_nm,os.str());  
      var = nco_var_free(var);
      return False;
    }
    
    /* convert type to disk type */
    var=nco_var_cnf_typ(var_ref->type,var);    
    
    //Swap values about  
    vp_swp=var_ref->val.vp;
    var_ref->val.vp=var->val.vp;
    var->val.vp=vp_swp;
    
    Nvar->flg_stt=2;
    
        
    (void)nco_var_free(var);
    
    return True;
  }
  
  // var is already defined but not populated 
  if(bdef && !bram && Nvar->flg_stt==1){
    ;
  }
  
  // var is already defined & populated in output 
  if(bdef && !bram && Nvar->flg_stt==2){
    var_sct* var_swp;
    var_sct* var_inf;
    var_inf=Nvar->cpyVarNoData();
    
    
    /* check sizes are the same */
    if(var_inf->sz != var->sz) {
      std::ostringstream os;
      os<< "Variable "<< var->nm << " size=" << var->sz << " has aleady been saved in ";
      os<< fl_out << " with size=" << var_inf->sz;
      
      wrn_prn(fnc_nm,os.str());  
      
      var = nco_var_free(var);
      var_inf=nco_var_free(var_inf);
      return False;
    }
    
    /* convert type to disk type */
    var=nco_var_cnf_typ(var_inf->type,var);
    
    
    //Swap values about
    var_inf->val=var->val;var->val.vp=(void*)NULL;
    var_swp=var;var=var_inf;var_inf=var_swp;
    
    var_inf=nco_var_free(var_inf);
    
    var_out_id=var->id;
    
  } 
  
  
  rcd=nco_inq_varid_flg(out_id,var->nm,&var_out_id);
  

  // Only go into define mode if necessary
  if(!bdef || var->pck_ram ){  

#ifdef _OPENMP
	if( omp_in_parallel())
	  err_prn(fnc_nm, "Attempt to go into netcdf define mode while in OPENMP parallel mode");
      
#endif
	(void)nco_redef(out_id);

  /* Define variable */   
  if(!bdef){
    (void)nco_def_var(out_id,var->nm,var->type,var->nbr_dim,var->dmn_id,&var_out_id);

    // set deflate, shuffle, chunking
    if(var->nbr_dim > 0){
      /* Set HDF Lempel-Ziv compression level, if requested */
      if(dfl_lvl > 0) 
        (void)nco_def_var_deflate(out_id,var_out_id,var->shuffle,(int)True,dfl_lvl);    
      else if(var->dfl_lvl)
        (void)nco_def_var_deflate(out_id,var_out_id,var->shuffle,(int)True,var->dfl_lvl);    

      /* Set chunk sizes, if requested */
      // fxm: must first allow cnk_sz specification in ncap2.cc main()
      //if(var->cnk_sz && var->nbr_dim > 0) (void)nco_def_var_chunking(out_id,var_id,(int)NC_CHUNKED,var->cnk_sz);
    } /* endif */

  } // bdef
  /* Put missing value 
  if(var->has_mss_val) (void)nco_put_att(out_id,var_out_id,nco_mss_val_sng_get(),var->type,1,var->mss_val.vp);
  */
  
  /* Write/overwrite scale_factor and add_offset attributes */
  if(var->pck_ram){ /* Variable is packed in memory */
    if(var->has_scl_fct) (void)nco_put_att(out_id,var_out_id,scl_fct_sng,var->typ_upk,1,var->scl_fct.vp);
    if(var->has_add_fst) (void)nco_put_att(out_id,var_out_id,add_fst_sng,var->typ_upk,1,var->add_fst.vp);
  } /* endif pck_ram */
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  } // end if

  /* Write variable */ 
  if(var->nbr_dim == 0){
    (void)nco_put_var1(out_id,var_out_id,0L,var->val.vp,var->type);
  }else{
    (void)nco_put_vara(out_id,var_out_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end else */
  
#ifdef NCO_RUSAGE_DBG
  /* Compile: cd ~/nco/bld;make 'USR_TKN=-DNCO_RUSAGE_DBG';cd - */
  /* Print rusage memory usage statistics */
  if(dbg_lvl_get() >= 0) {
    std::ostringstream os;
    os<<" Writing variable "<<var_nm; <<" to disk.";
    dbg_prn(fnc_nm,os.str());
  }
  maxrss=nco_mmr_rusage_prn((int)0);
#endif /* !NCO_RUSAGE_DBG */
  
  // save variable to output vector if new
  if(!bdef) {
    var_sct *var1;

    var->val.vp=(void*)nco_free(var->val.vp);
    var1=nco_var_dpl(var);

    var1->id=var_out_id;
    var1->nc_id=out_id;
    //temporary fix .. make typ_dsk same as type
    var1->typ_dsk=var1->type;
    Nvar=new NcapVar(var1);
    (void)var_vtr.push(Nvar);          
  } 
  
  var=nco_var_free(var);
  
  //Set flag -  indicates var is DEFINED && POPULATED
  Nvar->flg_stt=2;
  
  return True;
 
 }

void prs_cls::ncap_def_ntl_scn(void)
{
  int idx;
  int sz;
  int var_id;
  NcapVar *Nvar;
  NcapVar *Cvar;
  var_sct *var1;
  
  const std::string fnc_nm("prs_cls::ncap_def_ntl_scn"); 

  if(dbg_lvl_get() > 0)
    dbg_prn(fnc_nm, "Entered function");

  
  sz=int_vtr.size();
  
  for(idx=0; idx < sz ; idx++){
    // de-reference
    Nvar=int_vtr[idx];
    var1=Nvar->var;
    if( !Nvar->flg_udf && Nvar->xpr_typ==ncap_var) {
      
      if(dbg_lvl_get() > 0)
	dbg_prn(fnc_nm, Nvar->getFll()+ ( !Nvar->flg_mem ? " - defined in output.": " - RAM variable") );
      
      // define variable
      if(!Nvar->flg_mem) {
	(void)nco_def_var(out_id,var1->nm,var1->type,var1->nbr_dim,var1->dmn_id,&var_id);

	Nvar->var->id=var_id;
	Nvar->var->nc_id=out_id;
	Nvar->flg_stt=1;

        // set deflate,shuffle,chunking 
        if(var1->nbr_dim > 0){ 
	  /* Set HDF Lempel-Ziv compression level, if requested */
	  if(dfl_lvl > 0) 
            (void)nco_def_var_deflate(out_id,var_id,var1->shuffle,(int)True,dfl_lvl);    
          else if(var1->dfl_lvl)
            (void)nco_def_var_deflate(out_id,var_id,var1->shuffle,(int)True,var1->dfl_lvl);    

	  /* Set chunk sizes, if requested */
	  // fxm: must first allow cnk_sz specification in ncap2.cc main()
	  //if(var1->cnk_sz && var1->nbr_dim > 0) (void)nco_def_var_chunking(out_id,var_id,(int)NC_CHUNKED,var1->cnk_sz);
	} /* endif */

      } else { 
	//deal with RAM only var        
	Nvar->var->id=-1;
	Nvar->var->nc_id=-1;
	Nvar->flg_stt=1;
      }
      // save newly defined var in output vector
      
      Cvar=new NcapVar(*Nvar);
      var_vtr.push(Cvar);
      
    } 
    delete Nvar;  
  }
  
  // empty int_vtr n.b pointers have all been deleted
   int_vtr.clear();
}





/********End prs_cls methods********************************************************/
/***********************************************************************************/

