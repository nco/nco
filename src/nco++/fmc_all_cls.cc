/* $Header: /data/zender/nco_20150216/nco/src/nco++/fmc_all_cls.cc,v 1.53 2012-07-10 10:37:25 hmb Exp $ */

/* Purpose: netCDF arithmetic processor class methods: families of functions/methods */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "fmc_all_cls.hh"


//Conversion Functions /***********************************/
 
  cnv_cls::cnv_cls(bool flg_dbg){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("float" , this,(int)NC_FLOAT)); 
          fmc_vtr.push_back( fmc_cls("double", this,(int)NC_DOUBLE)); 
          fmc_vtr.push_back( fmc_cls("long" ,  this,(int)NC_INT)); 
          fmc_vtr.push_back( fmc_cls("int" ,   this,(int)NC_INT)); 
          fmc_vtr.push_back( fmc_cls("short" , this,(int)NC_SHORT)); 
          fmc_vtr.push_back( fmc_cls("ushort" ,this,(int)NC_USHORT)); 
          fmc_vtr.push_back( fmc_cls("uint" ,  this,(int)NC_UINT)); 
          fmc_vtr.push_back( fmc_cls("int64" , this,(int)NC_INT64)); 
          fmc_vtr.push_back( fmc_cls("uint64", this,(int)NC_UINT64)); 
          fmc_vtr.push_back( fmc_cls("ubyte",  this,(int)NC_UBYTE)); 
          fmc_vtr.push_back( fmc_cls("byte" ,  this,(int)NC_BYTE)); 
          fmc_vtr.push_back( fmc_cls("char",   this,(int)NC_CHAR)); 
    }		      
  } 
  var_sct * cnv_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("cnv_cls::fnd");
    int nbr_fargs;
    int fdx=fmc_obj.fdx();
    var_sct *var1;
   
    std::string sfnm =fmc_obj.fnm(); //method name
    //n.b fargs is an imaginary node -and is ALWAYS present
    nbr_fargs=fargs->getNumberOfChildren();
    
    // no arguments - bomb out
    if(!expr && nbr_fargs==0){    
        std::string serr;
	serr="Function has been called without an argument";               
        err_prn(sfnm,serr);
    }

    if(expr) 
      var1=walker.out(expr);
    else
      var1=walker.out(fargs->getFirstChild());   

    //do conversion 
    var1=nco_var_cnf_typ( (nc_type)fdx, var1);  
      
    return var1;

  }

		      
//Aggregate Functions /************************************************/

  agg_cls::agg_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("avg",this,(int)PAVG));
      fmc_vtr.push_back( fmc_cls("avgsqr",this,(int)PAVGSQR));
      fmc_vtr.push_back( fmc_cls("max",this,(int)PMAX));
      fmc_vtr.push_back( fmc_cls("min",this,(int)PMIN));
      fmc_vtr.push_back( fmc_cls("rms",this,(int)PRMS));
      fmc_vtr.push_back( fmc_cls("rmssdn",this,(int)PRMSSDN));
      fmc_vtr.push_back( fmc_cls("sqravg",this,(int)PSQRAVG));
      fmc_vtr.push_back( fmc_cls("total",this,(int)PTTL));
      fmc_vtr.push_back( fmc_cls("ttl",this,(int)PTTL));
    }
  }		      
		      
  var_sct *agg_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("agg_cls::fnd");
            int fdx;
            int nbr_args;
            int idx;
            int nbr_dim;
            int avg_nbr_dim; 
            dmn_sct **dim;
            dmn_sct **dim_nw=NULL_CEWI;  
            var_sct *var=NULL_CEWI;
            var_sct *var1=NULL_CEWI;
           
	    std::string susg;
	    std::string sfnm=fmc_obj.fnm();

            RefAST aRef;
            RefAST tr;
            std::vector<std::string> str_vtr;
            std::vector<RefAST> vtr_args; 
            NcapVector<dmn_sct*> dmn_vtr;
            
            // de-reference 
            ddra_info_sct ddra_info;        
            prs_cls *prs_arg=walker.prs_arg;

            fdx=fmc_obj.fdx();
 
 
            // Put args into vector 
            if(expr)
               vtr_args.push_back(expr);

            if(tr=fargs->getFirstChild()) {
              do  
	        vtr_args.push_back(tr);
              while(tr=tr->getNextSibling());    
            } 
      
            nbr_args=vtr_args.size();  

            susg="usage: var_out="+sfnm+"(var_in,$dim1,$dim2...$dimn)";

            if(nbr_args==0)
              err_prn(sfnm, " Function has been called with no arguments\n"+susg); 


            var1=walker.out(vtr_args[0]);
            nbr_dim=var1->nbr_dim;  

            // Process function arguments if any exist !! 
            for(idx=1; idx<nbr_args; idx++){  
                aRef=vtr_args[idx];
           
                switch(aRef->getType()){
                  case DIM_ID: 
                  case DIM_MTD_ID:
                       str_vtr.push_back(aRef->getText());
                       break;    

                  // This is garanteed to contain at least one DIM_ID or DIM_MTD  
                  // and NOTHING else --no need to type check!!
                  case DMN_ARG_LIST: 
                       { RefAST bRef=aRef->getFirstChild();
                         while(bRef){
                           str_vtr.push_back(bRef->getText());
                           bRef=bRef->getNextSibling();
                         }  
                        break;
                       }   

                  // ignore expr type argument
                  default:
                       std::string serr;
                       serr="Argument "+nbr2sng(idx)+ " is not a dimension"; 
                       wrn_prn(sfnm,serr);
                       break;
                } // end switch

             } // end for 

	    // Important to note that dmn_vtr contains dim pointers
            // picked up from var1->dim so there is no need to free them 
             if(vtr_args.size() >1) 
               dmn_vtr=ncap_dmn_mtd(var1, str_vtr);

             
            
            // Initial scan 
            if(prs_arg->ntl_scn){
                if(var1->undefined)
                var=ncap_var_udf("~dot_methods");  
                // deal with average over all dims or scalar var
                else if( nbr_dim==0 || dmn_vtr.size()== 0 || dmn_vtr.size()==nbr_dim)  
                var=ncap_sclr_var_mk(static_cast<std::string>("~dot_methods"),var1->type,false);    
                else {
                    // cast a variable with the correct dims in the correct order
                    dim=var1->dim;
                    std::vector<std::string> cst_vtr;              
                    
                    for(idx=0 ; idx < nbr_dim ; idx++){
                        std::string sdm(dim[idx]->nm);    
                        if( dmn_vtr.findi(sdm) == -1)
                        cst_vtr.push_back(sdm);       
                    }                
                    
                    var=ncap_cst_mk(cst_vtr,prs_arg);
                    var=nco_var_cnf_typ(var1->type,var);
                }
                
                var1=nco_var_free(var1);
                return var;
            } // end Initial scan

            
            // from here on dealing with a final scan  


            // deal with a hyperslab 
            // Code only reaches here on a final scan as
            // as an irregular hyperslab is flagged as var1->undefined==True on
            // initial scan  
            if(var1->has_dpl_dmn){   
                
              // can only process an irregular hyperslab if avergaing over all dimensions 
              // else bomb out 
              
              if(dmn_vtr.size() !=0 && dmn_vtr.size() != nbr_dim  )
                err_prn(sfnm, "This method can only work with a hyperslab if it being applied to all dimensions\nIf you wish to apply it over some of the dimensions then cast the variable first\n");
	     
              // create local copy of dims - The dim list created by var_lmt only
              // contains the regular dims from output. The srt/cnt/srd/end has been
              // lost. So this hack recreates the dims with this information.
              // since var1->dim isn't freed up when the var is freed up 
              // these dims are freed up seperatly. The new dims are not propagated
              // into var as var is always scalar --see code above 
      
              dim_nw=(dmn_sct**)nco_malloc(nbr_dim*sizeof(dmn_sct*));

	      for(idx=0 ; idx<nbr_dim; idx++){ 
                dim_nw[idx]=nco_dmn_dpl(var1->dim[idx]);   
                dim_nw[idx]->srt=var1->srt[idx];
                dim_nw[idx]->end=var1->end[idx];
                dim_nw[idx]->cnt=var1->cnt[idx];
                dim_nw[idx]->srd=var1->srd[idx];
                   
	        var1->dim[idx]=dim_nw[idx]; 
              }

            }  
            
            if(dmn_vtr.size() >0 && dmn_vtr.size()<nbr_dim ){
                dim=&dmn_vtr[0];
                avg_nbr_dim=dmn_vtr.size();
	    // average over all dims                           
            }else{
                dim=var1->dim;
                avg_nbr_dim=nbr_dim; 
            }    
            
            // do the heavy lifting
            switch(fdx){
                    
                case PAVG:
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_avg,False,&ddra_info);
                    // Use tally to normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    break;
                    
                case PAVGSQR:
                    var1=ncap_var_var_op(var1, NULL_CEWI,VSQR2);
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_avgsqr,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    break;
                    
                case PMAX:
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_max,False,&ddra_info);
                    break;
                    
                case PMIN:
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_min,False,&ddra_info);
                    break; 
                    
                case PRMS:
                    var1=ncap_var_var_op(var1, NULL_CEWI,VSQR2);
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_rms,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Take root
                    (void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
                    break;
                    
                case PRMSSDN:
                    var1=ncap_var_var_op(var1, NULL_CEWI,VSQR2);
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_rmssdn,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm_sdn(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Take root
                    (void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
                    break;
                    
                case PSQRAVG:
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_sqravg,False,&ddra_info);
                    // Normalize 
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Square mean
                    (void)nco_var_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,var->val);
                    break;
                    
                case PTTL:
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_ttl,False,&ddra_info);
                    break;

            } // end switch
             // var1 is freed/destroyed in nco_var_avg()


            // free local dim list if necessary
            if(dim_nw){
	      for(idx=0; idx<nbr_dim;idx++)
		dim_nw[idx]=nco_dmn_free(dim_nw[idx]);
              nco_free(dim_nw);
            }  		 
            return var;                

  } // end agg_cls::fnd     
            
	    

//Utility Functions /******************************************/

   utl_cls::utl_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("set_miss",this,(int)SET_MISS));
      fmc_vtr.push_back( fmc_cls("get_miss",this,(int)GET_MISS));
      fmc_vtr.push_back( fmc_cls("change_miss",this,(int)CH_MISS));
      fmc_vtr.push_back( fmc_cls("delete_miss",this,(int)DEL_MISS));
      fmc_vtr.push_back( fmc_cls("number_miss",this,(int)NUM_MISS));
      fmc_vtr.push_back( fmc_cls("has_miss",this,(int)HAS_MISS));
      fmc_vtr.push_back( fmc_cls("ram_write",this,(int)RAM_WRITE));
      fmc_vtr.push_back( fmc_cls("ram_delete",this,(int)RAM_DELETE));
     
    }
  }

  var_sct *utl_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("utl_cls::fnd");
    bool is_mtd;
    int rval=0;
    int nbr_args; 
    int fdx=fmc_obj.fdx();   //index
    prs_cls *prs_arg=walker.prs_arg;    
    vtl_typ lcl_typ;
    var_sct *var=NULL_CEWI;
    var_sct *var_in=NULL_CEWI;

    std::string serr;
    std::string sfnm;
    std::string styp;
    std::string va_nm;
    NcapVar *Nvar;
    RefAST tr;
    std::vector<RefAST> vtr_args; 

    sfnm =fmc_obj.fnm(); //method name
    is_mtd=(expr ? true: false);
    styp= (expr? "method":"function");
    nbr_args=0;

    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
     nbr_args=vtr_args.size();  
      
     if(nbr_args ==0) 
       err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 
     
     // deal with is_miss in a seperate function     
     if(fdx==NUM_MISS)
       return is_fnd(is_mtd, vtr_args,fmc_obj,walker);           
   
  

    if( fdx==SET_MISS || fdx==CH_MISS) {

      if( nbr_args==1) {
	serr=sfnm+ " is missing an argument";
        err_prn(fnc_nm,serr);
      }      
      var=walker.out(vtr_args[1] );
    } 
     
            
    lcl_typ=expr_typ(vtr_args[0]);          

    /* allow att identifier for RAM_DELETE */
    if(lcl_typ !=VVAR && !(fdx == RAM_DELETE && lcl_typ==VATT)) {
      serr="The first operand of the " + sfnm+ " must be a variable identifier only.";
      err_prn(fnc_nm,serr);
    }

    va_nm=vtr_args[0]->getText();
    Nvar=prs_arg->var_vtr.find(va_nm);


    /* Deal with GET_MISS as its different from other methods */
    if(fdx==GET_MISS){
      var_sct *var_tmp=NULL_CEWI;
      var_sct *var_ret=NULL_CEWI;
   
      var_tmp=prs_arg->ncap_var_init(va_nm,false);

      // Initial scan
      if(prs_arg->ntl_scn) 
        if(var_tmp)  
	  var_ret= ncap_sclr_var_mk(static_cast<std::string>
("~utility_function"),var_tmp->type,false);
        else
	  var_ret=ncap_var_udf("~utility_function");

      // Final scan
      if(!prs_arg->ntl_scn){
        if(var_tmp){
           // nb ncap_sclr_var_mk() calls nco_mss_val_mk() and fills var_ret with the default fill value
           // for that type.  So if the var has no missing value then this is the value returned 
           // Default fill  values are defined in  netcdf.h . 
           var_ret=ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),var_tmp->type,true);
           if(var_tmp->has_mss_val)
             (void)memcpy(var_ret->val.vp, var_tmp->mss_val.vp,nco_typ_lng(var_tmp->type)); 
        }else{          
        /* Cant find variable blow out */ 
          serr=sfnm+ " Unable to locate missing value for "+ va_nm;
          err_prn(fnc_nm,serr);
        } 
      } // end else
 
      if(var_tmp) var_tmp=nco_var_free(var_tmp);

      return var_ret; 	
     } // end GET_MISS 


    if(prs_arg->ntl_scn) {
      if(var) var=nco_var_free(var);
      return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),(nc_type)NC_INT,false);  
    }



    if(!Nvar ){
       wrn_prn(fnc_nm,sfnm+" unable to find variable: "+va_nm); 
       if(var) var=nco_var_free(var);
       return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),(nco_int)rval);        
    }


    //De-reference
    var_in=Nvar->var;  

    switch(fdx){


    case SET_MISS: {

          var=nco_var_cnf_typ(var_in->type,var);
          var->has_mss_val=True;
          var->mss_val=nco_mss_val_mk(var->type);
          (void)memcpy(var->mss_val.vp, var->val.vp,nco_typ_lng(var->type));
          nco_mss_val_cp(var,var_in);
          var=(var_sct*)nco_var_free(var);

          rval=1; 
          break;
          }
    case CH_MISS: {

          char *cp_out;
          long slb_sz;

          var=nco_var_cnf_typ(var_in->type,var);
          var->has_mss_val=True;
          var->mss_val=nco_mss_val_mk(var->type);
          (void)memcpy(var->mss_val.vp, var->val.vp,nco_typ_lng(var->type));
          
          // if no missing add one then exit
          if(!var_in->has_mss_val){
             nco_mss_val_cp(var,var_in);
             var=(var_sct*)nco_var_free(var);
	     break;
          
	  }
	  //get variable 
           var_in=prs_arg->ncap_var_init(va_nm,true);
             
           cp_out=(char*)var_in->val.vp;
           slb_sz=nco_typ_lng(var_in->type);

           for(long idx=0 ;idx<var_in->sz;idx++){
            if( !memcmp(cp_out,var_in->mss_val.vp,slb_sz))
             (void)memcpy(cp_out,var->mss_val.vp,slb_sz);
            cp_out+=(ptrdiff_t)slb_sz;
           }   
           // Copy new missing value 
           nco_mss_val_cp(var,var_in);
           //write variable
           (void)prs_arg->ncap_var_write(var_in,false);
           (void)nco_var_free(var);         
           
           rval=1;
           break;
          }

    case DEL_MISS: {
          // Dereference
           rval=0;
           var_in=Nvar->var;                
           if(var_in->has_mss_val){
             var_in->has_mss_val=False;
	     var_in->mss_val.vp=(void*)nco_free(var_in->mss_val.vp);
             rval=1;
           }
          break;
        }

    case RAM_WRITE: {

      if(Nvar->flg_mem==false){
              wrn_prn(fnc_nm,sfnm+" called with:"+va_nm+ " .This variable is already on disk");
	      rval=0;
            }else{
              var_sct *var_nw;
              var_nw=nco_var_dpl(Nvar->var);          
              prs_arg->var_vtr.erase(va_nm); 
              prs_arg->ncap_var_write(var_nw,false);
              rval=1;
             }

          } break;

    case RAM_DELETE: {
             // deal with var
             if(vtr_args[0]->getType()==VAR_ID){
               if(Nvar->flg_mem==false)
                 wrn_prn(fnc_nm,sfnm+" cannot remove disk variable:\""+va_nm+ "\". Delete can only remove RAM variables.");           
	         rval=0;
               }else{
                 prs_arg->var_vtr.erase(va_nm); 
                 rval=1;
               
             }
             if(vtr_args[0]->getType()==ATT_ID){ 
               prs_arg->var_vtr.erase(va_nm);
               rval=1; 
	     }
            } break;    

    }// end switch

    return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),(nco_int)rval);        

  }// end function

  
var_sct * utl_cls::is_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("srt_cls::imap_fnd");
    int nbr_args;
    int fdx=fmc_obj.fdx();
    long icnt;
    var_sct *var=NULL_CEWI;
    nc_type styp=NC_INT; // used to hold the mapping type either NC_INT or NC_UINT64 
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string var_nm;
    std::string susg;
    prs_cls *prs_arg=walker.prs_arg;    

    
#ifdef ENABLE_NETCDF4
  { /* scope for fl_fmt temporary */
    int fl_fmt; 
    (void)nco_inq_format(walker.prs_arg->out_id,&fl_fmt);
    if(fl_fmt==NC_FORMAT_NETCDF4 || fl_fmt==NC_FORMAT_NETCDF4_CLASSIC)
      styp=NC_UINT64;
    else    
     styp=NC_INT;   
  } /* end scope */

#endif 
  

    nbr_args=args_vtr.size(); 
    var=walker.out(args_vtr[0] );

    if(prs_arg->ntl_scn){
       nco_var_free(var);     
       return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),styp,false);         
    }  

    // from now on dealing with a final scan !!
    if(var->has_mss_val){
      char *cp_out=(char*)var->val.vp; 
      long idx;
      size_t slb_sz;  

      icnt=0;       

      slb_sz=nco_typ_lng(var->type); 
      for(idx=0 ;idx<var->sz;idx++){
        if( !memcmp(cp_out,var->mss_val.vp,slb_sz))
	  icnt++;
        cp_out+=(ptrdiff_t)slb_sz;
      }   
    }else{
       icnt=0; 
    }     

    nco_var_free(var); 

    if(styp==NC_UINT64) 
       return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),(nco_uint64)icnt);
    else 
      return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),(nco_int)icnt);
}  
    



//Basic Functions /******************************************/

  bsc_cls::bsc_cls(bool  flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("size",this,(int)PSIZE));
      fmc_vtr.push_back( fmc_cls("type",this,(int)PTYPE));
      fmc_vtr.push_back( fmc_cls("ndims",this,(int)PNDIMS));
      fmc_vtr.push_back( fmc_cls("exists",this,(int)PEXISTS));

    }
  }

  var_sct *bsc_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("bsc_cls::fnd");

    int fdx=fmc_obj.fdx();   //index
    int nbr_args;
    std::string va_nm;
    std::string susg; 
    std::string sfnm =fmc_obj.fnm(); //method name

    nc_type mp_typ=NC_INT;
    var_sct *var=NULL_CEWI;
    var_sct *var1=NULL_CEWI;
    prs_cls* prs_arg=walker.prs_arg;
      
    RefAST tr;
    vtl_typ lcl_typ;
    std::vector<RefAST> vtr_args;


#ifdef ENABLE_NETCDF4
  { /* scope for fl_fmt temporary */
    int fl_fmt; 
    (void)nco_inq_format(walker.prs_arg->out_id,&fl_fmt);
    if(fl_fmt==NC_FORMAT_NETCDF4 || fl_fmt==NC_FORMAT_NETCDF4_CLASSIC)
      mp_typ=NC_UINT64;
    else    
      mp_typ=NC_INT;   
  } /* end scope */

#endif 



    susg="usage: property="+sfnm+"( var_nm | att_nm | var_exp )";


    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
     nbr_args=vtr_args.size();  


    // no arguments - bomb out
    if(nbr_args==0){    
        std::string serr;
	serr="Function has been called with no argument\n"+susg;               
        err_prn(sfnm,serr);
	// more than one arg -- only print message once 
    } else if(nbr_args >1 && !prs_arg->ntl_scn)
        wrn_prn(sfnm,"Function has been called with more than one argument");
           

    tr=vtr_args[0];  


    lcl_typ=expr_typ(tr);          
   

    // If initial scan
    if(prs_arg->ntl_scn){

      // Evaluate argument on first scan for side-effects eg var1++ or var1+=10 etc 
      if( lcl_typ!=VVAR && lcl_typ !=VATT){
      var1=walker.out(tr) ;
      var1=nco_var_free(var1);
      }
      return ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nc_type)NC_INT,false);        
    }
       
    // from here on dealing with final scan
    va_nm=tr->getText();


    // deal with PEXISTS here 
    if(fdx==PEXISTS){
      int iret=0;
      switch(lcl_typ){
        case VVAR: 
           if(prs_arg->ncap_var_init_chk(va_nm)) iret=1;
           break;
        case VATT:
	  if( prs_arg->var_vtr.find(va_nm) !=NULL)
	    iret=1;
          else if( (var1=ncap_att_init(va_nm,prs_arg))!=NULL) ;      
            iret=1;
          break;   

      default:
	 var1=walker.out(tr);  
         iret=1; 
         break;
      }   
      
      if(var1) nco_var_free(var1);    
      return ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_int)iret);
    }

     
      if(lcl_typ==VVAR)
	var1=prs_arg->ncap_var_init(va_nm,false); 
      else 
        var1=walker.out(tr);
    

      switch(fdx){ 
           case PSIZE:
             if(mp_typ==NC_UINT64) 
               var=ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_uint64)var1->sz);
             else 
               var=ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_int)var1->sz);
             break;
           case PTYPE:
             var=ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_int)var1->type);
             break;
           case PNDIMS:
             var=ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_int)var1->nbr_dim);            

      } // end switch        
      
      var1=nco_var_free(var1);
      return var;		 
  }       

  
//Math Functions /******************************************/
  mth_cls::mth_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){


      /* Basic math: acos, asin, atan, cos, exp, log, log10, rnd_nbr, sin, sqrt, tan */
      //sym_vtr.push_back(sym_cls("rnd_nbr",rnd_nbr,rnd_nbrf));
      sym_vtr.push_back(sym_cls("acos",acos,acosf));  
      sym_vtr.push_back(sym_cls("asin",asin,asinf));
      sym_vtr.push_back(sym_cls("atan",atan,atanf));
      sym_vtr.push_back(sym_cls("cos",cos,cosf));  
      sym_vtr.push_back(sym_cls("exp",exp,expf));
      sym_vtr.push_back(sym_cls("fabs",fabs,fabsf));
      sym_vtr.push_back(sym_cls("log",log,logf));
      sym_vtr.push_back(sym_cls("log10",log10,log10f));
      sym_vtr.push_back(sym_cls("sin",sin,sinf));
      sym_vtr.push_back(sym_cls("sqrt",sqrt,sqrtf));
      sym_vtr.push_back(sym_cls("tan",tan,tanf));

      /* Basic math synonyms: ln */
      sym_vtr.push_back(sym_cls("ln",log,logf)); /* ln() is synonym for log() */
  
      /* Basic Rounding: ceil, fl<oor */
      sym_vtr.push_back(sym_cls("ceil",ceil,ceilf)); /* Round up to nearest integer */
      sym_vtr.push_back(sym_cls("floor",floor,floorf)); /* Round down to nearest integer */
  
     /* Advanced math: erf, erfc, gamma
       LINUX*, MACOSX*, and SUN* provide these functions with C89
       20020122 and 20020422: AIX, CRAY, SGI*, WIN32 do not define erff(), erfcf(), gammaf() with C89
       20050610: C99 mandates support for erf(), erfc(), tgamma()
       Eventually users without C99 will forego ncap */

       #if defined(LINUX) || defined(LINUXAMD64)  || defined(MACOSX)
         sym_vtr.push_back(sym_cls("erf",erf,erff));
         sym_vtr.push_back(sym_cls("erfc",erfc,erfcf));
         sym_vtr.push_back(sym_cls("gamma",tgamma,tgammaf));
       #endif /* !LINUX */

       /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh
         20020703: AIX, SGI*, WIN32 do not define acoshf, asinhf, atanhf
         20050610: C99 mandates support for acosh(), asinh(), atanh(), cosh(), sinh(), tanh()
         Eventually users without C99 will forego ncap */
       #if defined(LINUX) || defined(LINUXAMD64)
          sym_vtr.push_back(sym_cls("acosh",acosh,acoshf));
          sym_vtr.push_back(sym_cls("asinh",asinh,asinhf));
          sym_vtr.push_back(sym_cls("atanh",atanh,atanhf));
          sym_vtr.push_back(sym_cls("cosh",cosh,coshf));
          sym_vtr.push_back(sym_cls("sinh",sinh,sinhf));
          sym_vtr.push_back(sym_cls("tanh",tanh,tanhf));
       #endif /* !LINUX */
  
      /* 20020703: AIX, MACOSX, SGI*, WIN32 do not define rintf
      Only LINUX* supplies all of these and I do not care about them enough
      to activate them on LINUX* but not on MACOSX* and SUN* */
      /* Advanced Rounding: nearbyint, rint, round, trunc */
     /* Advanced Rounding: nearbyint, round, trunc */
     /* sym_vtr.push(sym_cls("nearbyint",nearbyint,nearbyintf)); *//* Round to integer value in floating point format using current rounding direction, do not raise inexact exceptions */
     /* sym_vtr.push(sym_cls("round",round,roundf)); *//* Round to nearest integer away from zero */
      /* sym_vtr.push(sym_cls("trunc",trunc,truncf)); *//* Round to nearest integer not larger in absolute value */
      /* sym_vtr.push(sym_cls("rint",rint,rintf)); *//* Round to integer value in floating point format using current rounding direction, raise inexact exceptions */
  
     // create fmc vector from sym vector
     int idx;
     int sz=sym_vtr.size();
            
     for(idx=0 ; idx<sz ;idx++){
       fmc_cls fmc_obj( sym_vtr[idx].fnm(),this,idx);   
       fmc_vtr.push_back(fmc_obj);
     }
    }
  }		      
  var_sct * mth_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("mth_cls::fnd");
    int fdx;
    int nbr_args;
    var_sct *var1;
    var_sct *var;   
    fdx=fmc_obj.fdx(); 
    std::string susg;
    std::string sfnm =fmc_obj.fnm(); 
   
    RefAST tr;
    std::vector<RefAST> vtr_args;    
    // de-reference
    prs_cls* prs_arg=walker.prs_arg;

    susg="usage: var_out="+sfnm+"(var_exp)";


    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
     nbr_args=vtr_args.size();  


    // no arguments - bomb out
    if(nbr_args==0){    
        std::string serr;
	serr="Function has been called with no argument\n"+susg;               
        err_prn(sfnm,serr);
	// more than one arg -- only print message once 
    } else if(nbr_args >1 && !prs_arg->ntl_scn)
        wrn_prn(sfnm,"Function has been called with more than one argument");
           
    var1=walker.out(vtr_args[0]);   


    var=ncap_var_fnc(var1, sym_vtr[fdx]._fnc_dbl,sym_vtr[fdx]._fnc_flt);

    return var;      

  }

//Maths2 - Maths functions that take 2 args /*********/
  mth2_cls::mth2_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("pow",this,(int)PPOW));
      fmc_vtr.push_back( fmc_cls("atan2",this,(int)PATAN2));
      fmc_vtr.push_back( fmc_cls("convert",this,(int)PCONVERT));
    }
  }


  var_sct *mth2_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("mth2_cls::fnd");

    int fdx=fmc_obj.fdx();   //index
    int nbr_args=0;

    var_sct *var=NULL_CEWI;
    var_sct *var1=NULL_CEWI;
    var_sct *var2=NULL_CEWI;

    std::string susg;
    std::string sfnm =fmc_obj.fnm(); //method name

    RefAST tr;
    std::vector<RefAST> vtr_args; 
    prs_cls* prs_arg=walker.prs_arg;


    susg="usage: var_out="+sfnm+"(var_exp,var_exp)"; 

    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
     nbr_args=vtr_args.size();  
      
    

    // no arguments - bomb out
    if(nbr_args<2){    
        std::string serr;
	serr="Function has been called with less than two argument\n"+susg;               
        err_prn(sfnm,serr);
	// more than one arg -- only print message once 
    } else if(nbr_args >2 && !prs_arg->ntl_scn)
        wrn_prn(sfnm,"Function has been called with more than two arguments");
      
     var1=walker.out(vtr_args[0]);
     //
     var2=walker.out(vtr_args[1]);
      


    
    if(prs_arg->ntl_scn)
       if(var1->undefined || var2->undefined){
	var1=nco_var_free(var1);
        var2=nco_var_free(var2);
        var=ncap_var_udf("~mth2_cls");
        return var;
       }                          
      

    switch(fdx){

      case PPOW: 
        var=ncap_var_var_op(var1,var2,CARET);
        break;

      case PATAN2: 
      var=ncap_var_var_op(var1,var2,ATAN2);
        break;                

      case PCONVERT:{
        /* Change type to int */
        int c_typ;

        var2=nco_var_cnf_typ(NC_INT,var2);
        (void)cast_void_nctype(NC_INT,&var2->val);
        c_typ=var2->val.ip[0];      
        (void)cast_nctype_void(NC_INT,&var2->val);
        var2=nco_var_free(var2);

        var=nco_var_cnf_typ( (nc_type)c_typ, var1);
        break;
      }    

    }
      
    return var; 

  }


//PDQ Functions /******************************************/
  pdq_cls::pdq_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("reverse",this,(int)PREVERSE));
      fmc_vtr.push_back( fmc_cls("permute",this,(int)PPERMUTE));

    }
  }

  var_sct *pdq_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("pdq_cls::fnd");
            int idx;       
            int jdx;
            int fdx;
            int nbr_args;
            int nbr_dim;
            var_sct *var_in=NULL_CEWI;
            var_sct *var_out=NULL_CEWI;
           
	    std::string sfnm;
            std::string styp;
            std::string serr;

            RefAST tr;
            std::vector<std::string> str_vtr;
            std::vector<RefAST> vtr_args; 
            NcapVector<dmn_sct*> dmn_vtr;
            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;

            fdx=fmc_obj.fdx();

	    sfnm =fmc_obj.fnm(); //method name


            if(expr)
              vtr_args.push_back(expr);

            if(tr=fargs->getFirstChild()) {
              do  
	       vtr_args.push_back(tr);
              while(tr=tr->getNextSibling());    
            } 
      
            nbr_args=vtr_args.size();  

            if(nbr_args==0)
              err_prn(sfnm,"Function has been called with no arguments"); 

            var_in=walker.out(vtr_args[0]);

	    nbr_dim=var_in->nbr_dim;

            for(idx=1; idx<nbr_args; idx++){
              switch( vtr_args[idx]->getType() ){
                     
                case DIM_ID: 
                case DIM_MTD_ID:  
                     str_vtr.push_back(vtr_args[idx]->getText());
                     break;    
                    // ignore expr type arguments
                default:
		  // warn only on final scan
		  if(!prs_arg->ntl_scn)    
		       wrn_prn(sfnm,"Argument "+nbr2sng(idx) +" is not a dimension");
                     break;
              } // end switch
            } // end for
        

            if(prs_arg->ntl_scn)
              dmn_vtr=ncap_dmn_mtd(var_in,str_vtr);
            else{
              var_out=nco_var_dpl(var_in);
              dmn_vtr=ncap_dmn_mtd(var_out,str_vtr);
            }

            if(fdx==PPERMUTE){

              if((size_t)dmn_vtr.size() < str_vtr.size())
	        wrn_prn(sfnm, "Unrecognized dimension arguments");

	      if(dmn_vtr.size() < nbr_dim ) {
                ostringstream os; 
	        os<<"You have only specified "<< dmn_vtr.size()<< " dimension  args. You need to specify  "<< nbr_dim<<". All of the variable's dimensions must be arguments"; 
              err_prn(sfnm,os.str());
                }

              // Check location of record dimension
              for(idx=0 ; idx<nbr_dim ; idx++)
		if( idx>0 && dmn_vtr[idx]->is_rec_dmn){
                  ostringstream os; 
		  os<<"The record dimension \""<< dmn_vtr[idx]->nm <<"\" must be the first dimension in the list."; 
                  err_prn(fnc_nm,os.str());  
	        }

	    }
            
  
            //Initial scan 
            if(prs_arg->ntl_scn ) {

              if(fdx==PREVERSE || var_in->undefined ) 
                return var_in; 
             
              // deal with PPERMUTE
              if(fdx==PPERMUTE){
		std::vector<std::string> cst_vtr;

                for(idx=0 ; idx<nbr_dim ; idx++)
                  cst_vtr.push_back( static_cast<std::string>((dmn_vtr[idx]->nm)));

                var_out=ncap_cst_mk(cst_vtr,prs_arg);
                var_out=nco_var_cnf_typ(var_in->type,var_out);
                var_in=nco_var_free(var_in);
                return var_out;
              }
            }

           
            std::vector<nco_bool>  bool_vtr(nbr_dim,False);           
	    std::vector<int> dmn_idx_in_out(nbr_dim,0);
	    std::vector<int> dmn_idx_out_in(nbr_dim,0);
             

	     switch(fdx) {

               case PREVERSE: 

                 for(idx=0 ; idx < nbr_dim ; idx++){
                   std::string sdm(var_out->dim[idx]->nm);    
                   bool_vtr[idx]= (dmn_vtr.findi(sdm) >=0 ? True:False);

                   //straight mapping !!
                   dmn_idx_out_in[idx]=idx; 
                  }                
                 break;
          
	       case PPERMUTE:{ 
	         for(idx=0 ; idx<nbr_dim; idx++){
                   std::string sdm(var_in->dim[idx]->nm);    
	           dmn_idx_in_out[idx]=dmn_vtr.findi(sdm);
		   var_out->dim[idx]=dmn_vtr[idx];
                   var_out->dmn_id[idx]=dmn_vtr[idx]->id;
	         }

                 // create "out_in" mapping from "in_out" mapping
                 for(idx=0 ; idx <nbr_dim ; idx++)
		   for(jdx=0 ; jdx<nbr_dim; jdx++)
		     if( idx==dmn_idx_in_out[jdx]){
                       dmn_idx_out_in[idx]=jdx;
                       break;
                     }  
	       } 
               break;

	     }// end switch

             //do operation
             (void)nco_var_dmn_rdr_val(var_in,var_out,&dmn_idx_out_in[0],&bool_vtr[0]);
             var_in=(var_sct*)nco_var_free(var_in);
             return var_out; 
    }     


//Mask Function /******************************************/

  msk_cls::msk_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("mask",this,(int)PMASK1));
      fmc_vtr.push_back( fmc_cls("mask_clip",this,(int)PMASK_CLIP));
    }
  }

  var_sct *msk_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("msk_cls::fnd");
    int fdx=fmc_obj.fdx();   //index
    int nbr_args;
    prs_cls* prs_arg=walker.prs_arg;
    var_sct *var=NULL_CEWI;
    var_sct *var_msk=NULL_CEWI;

    RefAST tr;

    std::string sfnm =fmc_obj.fnm(); //method name
    std::vector<RefAST> vtr_args; 
    NcapVector<dmn_sct*> dmn_vtr;

    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
       do  
         vtr_args.push_back(tr);
       while(tr=tr->getNextSibling());    
    } 
      
    nbr_args=vtr_args.size();  

    if(nbr_args<2)
      err_prn(sfnm, "Function has been called with less than two arguments"); 

    if(nbr_args >2 && !prs_arg->ntl_scn) 
      wrn_prn(sfnm," Function has been called with more than two arguments"); 
    
    var=walker.out(vtr_args[0]);
    var_msk=walker.out(vtr_args[1]);

    // Deal with initial scan
    if(prs_arg->ntl_scn) {

      switch(fdx) {
        case PMASK1:
	     var_msk=nco_var_cnf_typ(var->type,var_msk);     
             var=nco_var_free(var);
             return var_msk; 
             break;

        case PMASK_CLIP:
             var_msk=nco_var_free(var_msk);
             return var;
             break;

      }// end switch
    } // end if

    if( fdx==PMASK_CLIP && var->sz%var_msk->sz !=0 ) {
      std::ostringstream os;
      os<< " Function cannot clip var  ";
      os<< var->nm <<" as size(" <<var->sz<<") is not divisible by mask var "<<var_msk->nm <<" size("<< var_msk->sz <<")"; 
      err_prn(sfnm,os.str()); 
    }

    switch(fdx) {
    
      case PMASK_CLIP: {
       char *cp_in;
       char *cp_out;  
       short *sp;
       long idx;
       long jdx;
       long cnt;
       long msk_sz=var_msk->sz;
       size_t slb_sz;

       var_msk=nco_var_cnf_typ(NC_SHORT,var_msk);    
       slb_sz=nco_typ_lng(var->type);    
       cnt=var->sz/var_msk->sz;

       (void)cast_void_nctype(NC_SHORT,&var_msk->val);
       //Dereference 
       sp=var_msk->val.sp; 

       for(idx=0; idx <cnt ; idx++){
	 cp_out=(char*)(var->val.vp)+(size_t)(idx*msk_sz*slb_sz);
	 cp_in=cp_out;

	 for(jdx=0 ;jdx<msk_sz; jdx++){
	   if(sp[jdx]){ 
	     (void)memcpy(cp_out,cp_in,slb_sz);
	     cp_out+=slb_sz; 
	   }
	   cp_in+=slb_sz;
	 }   
       } 

       (void)cast_nctype_void(NC_SHORT,&var_msk->val);
       var_msk=nco_var_free(var_msk);

       return var;
      } 
      break; 

    case PMASK1: {
      //convert to ints
      char *cp_in;
      char *cp_out;
      long idx;
      nco_int *ip;
      long var_sz;
      long msk_sz;
      size_t slb_sz;
      var_sct *var_out;

      var_msk=nco_var_cnf_typ(NC_INT,var_msk);     
      
      var_out=nco_var_dpl(var_msk);
      var_out=nco_var_cnf_typ(var->type,var_out);

      // De-Reference 
      (void)cast_void_nctype(NC_INT,&var_msk->val);
      ip=var_msk->val.ip;

      msk_sz=var_msk->sz;
      var_sz=var->sz;
      slb_sz=nco_typ_lng(var->type);

      cp_out=(char*)(var_out->val.vp);
      
      for(idx=0 ; idx<msk_sz ;idx++){
        // index out of bounds bomb out
        if(prs_arg->FORTRAN_IDX_CNV) {

          if( ip[idx]<1L || ip[idx] > var_sz){
            std::ostringstream os;
            os<<" Function reporting that fortran index "<<ip[idx]<<" into "<<var->nm<<" is out of bounds 1"<<"-"<<var_sz; 
            err_prn(sfnm,os.str());         
          }
	  --ip[idx];
        }else{
         
          if( ip[idx]<0L || ip[idx] >= var_sz){
            std::ostringstream os;
            os<<"Function reporting that index "<<ip[idx]<<" into "<<var->nm<<" is out of bounds 0"<<"-"<<var_sz-1; 
            err_prn(sfnm,os.str());         
          }
        }

        cp_in=(char*)(var->val.vp)+ (size_t)ip[idx]*slb_sz;
	(void)memcpy(cp_out,cp_in ,slb_sz);            
        cp_out+=slb_sz;
      } // end for

      (void)cast_nctype_void(NC_INT,&var_msk->val);
      var_msk=nco_var_free(var_msk);
      var=nco_var_free(var);

      return var_out; 
    } 
    break;

  } // end switch

    return NULL; // fxm csz correct? 20090228
} // end function

//Pack Function /******************************************/

  pck_cls::pck_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("pack",this,(int)PPACK));
      fmc_vtr.push_back( fmc_cls("pack_byte",this,(int)PPACK_BYTE));
      fmc_vtr.push_back( fmc_cls("pack_char",this,(int)PPACK_CHAR));
      fmc_vtr.push_back( fmc_cls("pack_short",this,(int)PPACK_SHORT));
      fmc_vtr.push_back( fmc_cls("pack_int",this,(int)PPACK_INT));
      fmc_vtr.push_back( fmc_cls("unpack",this,(int)PUNPACK));
    }
  }

  var_sct *pck_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("pck_cls::fnd");
    int nbr_args; 
    int fdx=fmc_obj.fdx();   //index
    prs_cls *prs_arg=walker.prs_arg;    
    var_sct *var_in=NULL_CEWI;
    var_sct *var_out=NULL_CEWI;
    nc_type typ;
    nco_bool PCK_VAR_WITH_NEW_PCK_ATT;
    
    std::string susg;
    std::string serr; 
    std::string sfnm=fmc_obj.fnm();
    RefAST tr;
    std::vector<RefAST> vtr_args; 

    susg="usage: var_out="+sfnm+"(var_in)";


    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
       do  
         vtr_args.push_back(tr);
       while(tr=tr->getNextSibling());    
    } 
      
    nbr_args=vtr_args.size();  


    if(nbr_args==0)
      err_prn(sfnm,"Function has been called with no argument\n"+susg);
      


    if(nbr_args>1 && !prs_arg->ntl_scn)
      wrn_prn(sfnm,"Function has been called with more than one argument");


    var_in=walker.out(vtr_args[0]); 


     switch(fdx) {
	case PPACK:
        case PPACK_SHORT:
          typ=NC_SHORT;
          break;	
        case PPACK_BYTE:
          typ=NC_BYTE;
	  break;			
        case PPACK_CHAR:
          typ=NC_CHAR;
          break; 
        case PPACK_INT:
          typ=NC_INT;
          break; 
        case PUNPACK:
          break;
     }	

    /* deal with initial scan */  
    if(prs_arg->ntl_scn ){

      switch(fdx) {

        case PPACK:
        case PPACK_SHORT:
        case PPACK_BYTE:
        case PPACK_CHAR:
        case PPACK_INT:
	  var_out=nco_var_cnf_typ(typ, var_in);  
          break;

        case PUNPACK: 
          var_sct *var_att;
	  var_att=ncap_att_get(var_in->id,var_in->nm,"scale_factor",1,prs_arg);
          if(var_att == (var_sct*)NULL) 
	    var_att=ncap_att_get(var_in->id,var_in->nm,"add_offset",1,prs_arg);	

          if(var_att==(var_sct*)NULL){ 
            var_in=nco_var_free(var_in);     
            var_out=ncap_var_udf("~dot_methods");  
          }else{
	    var_out=nco_var_cnf_typ(var_att->type, var_in);  	
            var_att=nco_var_free(var_att); 
         }
         break;
      } /* end switch */
      
      return var_out;
    } /* if initial scan */


    /* Deal with final scan */
    switch(fdx) {

	case PPACK: 
        case PPACK_SHORT:
        case PPACK_BYTE:
        case PPACK_CHAR:
        case PPACK_INT:
          var_out=nco_var_pck(var_in,typ,&PCK_VAR_WITH_NEW_PCK_ATT);	
          //var_in=nco_var_free(var_in); 
          break;
        case PUNPACK:
          // Unpacking variable does not create duplicate so DO NOT free var
          var_out=nco_var_upk(var_in); 
          break;
    } 

    return var_out; 


}



//Sort Functions /***********************************/
 
  srt_cls::srt_cls(bool flg_dbg){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("sort" , this,PASORT)); 
          fmc_vtr.push_back( fmc_cls("asort" , this,PASORT)); 
          fmc_vtr.push_back( fmc_cls("dsort" , this,PDSORT)); 
          fmc_vtr.push_back( fmc_cls("remap" , this,PREMAP)); 
          fmc_vtr.push_back( fmc_cls("unmap" , this,PUNMAP)); 
          fmc_vtr.push_back( fmc_cls("invert_map" , this,PIMAP)); 

			     		      
    }
  }


  var_sct *srt_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_fit_cls::fnd");
    bool is_mtd;
    int fdx=fmc_obj.fdx();   //index
    RefAST tr;    
    std::vector<RefAST> vtr_args; 
       

    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    }
    

    is_mtd=(expr ? true: false);

    switch(fdx){
      case PASORT:
      case PDSORT: 
        return srt_fnd(is_mtd,vtr_args,fmc_obj,walker);  
        break;
      case PREMAP:
      case PUNMAP:
        return mst_fnd(is_mtd,vtr_args,fmc_obj,walker);  
      case PIMAP:
        return imap_fnd(is_mtd,vtr_args,fmc_obj,walker);   
        break;
    }



} // end gsl_fit_cls::fnd 


  
var_sct * srt_cls::imap_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("srt_cls::imap_fnd");
    int nbr_args;
    int fdx=fmc_obj.fdx();
    var_sct *var1=NULL_CEWI;
    var_sct *var_ret=NULL_CEWI;
    nc_type styp; // used to hold the mapping type either NC_INT or NC_UINT64 
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string var_nm;
    std::string susg;
    prs_cls *prs_arg=walker.prs_arg;    


    nbr_args=args_vtr.size(); 


    if(nbr_args==0)
       err_prn(sfnm,"Function has been called with no arguments"); 

    
    var1=walker.out(args_vtr[0]);   

    if(prs_arg->ntl_scn)     
      return var1;  

    // remember original type 
    styp=var1->type; 

    var1=nco_var_cnf_typ((nc_type)NC_UINT64, var1);  

    // var2 contains the mapping
    (void)cast_void_nctype((nc_type)NC_UINT64,&var1->val);

     {
       long idx;
       long sz; 
       nco_uint64 *lp_mp;  
       nco_uint64 *lp_mp_out;   
       sz=var1->sz;
       lp_mp=var1->val.ui64p; 
        
       lp_mp_out=(nco_uint64*)nco_calloc(sz,sizeof( nco_uint64));  
         
       for(idx=0;idx<sz;idx++)
         if(lp_mp[idx]<sz) lp_mp_out[ lp_mp[idx] ] =idx;

       //swap values 
       nco_free(var1->val.ui64p);
       var1->val.ui64p=lp_mp_out; 
            
     }

     (void)cast_nctype_void((nc_type)NC_UINT64,&var1->val);  
     // convert back to original type 
     var1=nco_var_cnf_typ(styp, var1);  
      
     return var1;

}




  
var_sct * srt_cls::srt_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("srt_cls::srt_fnd");
    int nbr_args;
    int fdx=fmc_obj.fdx();
    var_sct *var1=NULL_CEWI;
    var_sct *var2=NULL_CEWI;
    nc_type mp_typ; // used to hold the mapping type either NC_INT or NC_UINT64 
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string styp;
    std::string var_nm;
    std::string susg;
    bool bdirection; // ascending or desending sort
    prs_cls *prs_arg=walker.prs_arg;    


    nbr_args=args_vtr.size();  

    susg="usage: var_out="+sfnm+"(var_exp,&var_map)\n";  

    mp_typ=NC_INT;

#ifdef ENABLE_NETCDF4
  { /* scope for fl_fmt temporary */
    int fl_fmt; 
    (void)nco_inq_format(walker.prs_arg->out_id,&fl_fmt);
    if(fl_fmt==NC_FORMAT_NETCDF4 || fl_fmt==NC_FORMAT_NETCDF4_CLASSIC)
      mp_typ=NC_UINT64;
    else    
      mp_typ=NC_INT;   
  } /* end scope */

#endif 


    if(nbr_args==0)
       err_prn(sfnm,"Function has been called with no arguments"); 

         
      var1=walker.out(args_vtr[0]);
       
      if(nbr_args>1){
        bool bdef;
        NcapVar *Nvar;
        if(args_vtr[1]->getType() != CALL_REF ) 
          err_prn(sfnm," second argument must be a call by reference variable\n"+susg);   

          var_nm=args_vtr[1]->getFirstChild()->getText(); 
          bdef=prs_arg->ncap_var_init_chk(var_nm);
          Nvar=prs_arg->var_vtr.find(var_nm);  
           
        /*This horrible line below:
          Initial scan -- prs_arg->ntl_scn=True 
          If variable has been already been defined:
          Then read it.

          Final scan -- prs_arg->ntl_scn=False
	  We have the situation where call by-ref variable has been defined in the 
          first pass, but not populated i.e Nvar->flg_stt==1. So we create the variable 
          from var1.
          Also covered is the situation where the call_by_ref variable has been defined 
          and populated earlier in the script i.e Nvar->flg_stt==2. So we simly read in this 
          variable 
	 */

          if(bdef && prs_arg->ntl_scn || bdef && !prs_arg->ntl_scn && Nvar->flg_stt==2 ){
            var2=prs_arg->ncap_var_init(var_nm,true);  
         }else{
	    var2=nco_var_dpl(var1);
            if(!var2->undefined) 
              var2=nco_var_cnf_typ(mp_typ,var2);
            nco_free(var2->nm);
            var2->nm=strdup(var_nm.c_str());
        }    

        if(prs_arg->ntl_scn)
          prs_arg->ncap_var_write(var2,false);

      }
        if(prs_arg->ntl_scn) 
          return var1;
       
     if(fdx==PASORT) bdirection=true;          
     if(fdx==PDSORT) bdirection=false;          



             
      if(var2==NULL){
	if(bdirection)   
	  var1=ncap_var_var_op(var1,(var_sct*)NULL,VSORT);  
        else 
	  var1=ncap_var_var_op(var1,(var_sct*)NULL,VRSORT);  
      }else{

       // convert map 
       var2=nco_var_cnf_typ(mp_typ, var2);  

       // check if map is large enough  
       if( var2->sz < var1->sz) {
         ostringstream os; 
	 os<<"Size of map  "<<var_nm<<"("<< var2->sz<<") is less than size of var(" << var1->sz<<")";
             err_prn(sfnm,os.str());
        }

        switch (var1->type) {
          case NC_DOUBLE: 
	    (void)ncap_sort_and_map<double>(var1,var2,bdirection);    
            break;  
          case NC_FLOAT: 
	    (void)ncap_sort_and_map<float>(var1,var2,bdirection);    
            break;  
          case NC_INT: 
	    (void)ncap_sort_and_map<nco_int>(var1,var2,bdirection);    
            break;  
          case NC_SHORT: 
	    (void)ncap_sort_and_map<nco_short>(var1,var2,bdirection);    
            break;  
          case NC_USHORT: 
	    (void)ncap_sort_and_map<nco_ushort>(var1,var2,bdirection);    
            break;  
          case NC_UINT: 
	    (void)ncap_sort_and_map<nco_uint>(var1,var2,bdirection);    
            break;  
          case NC_INT64: 
	    (void)ncap_sort_and_map<nco_int64>(var1,var2,bdirection);    
            break;  
          case NC_UINT64: 
	    (void)ncap_sort_and_map<nco_uint64>(var1,var2,bdirection);    
            break;  
          case NC_BYTE: 
	    (void)ncap_sort_and_map<nco_byte>(var1,var2,bdirection);    
            break;  
          case NC_UBYTE: 
	    (void)ncap_sort_and_map<nco_ubyte>(var1,var2,bdirection);    
            break;  
          case NC_CHAR: 
	    (void)ncap_sort_and_map<nco_char>(var1,var2,bdirection);    
            break;  
           case NC_STRING: break; /* Do nothing */
             
           default: nco_dfl_case_nc_type_err(); break;
            
	} // end big switch
 
         // Write out mapping
         (void)prs_arg->ncap_var_write(var2,false);             
      }

      return var1;
    

} // end srt_cls::srt_fnd()


  
var_sct * srt_cls::mst_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("srt_cls::mst_fnd");
    int nbr_args;
    int fdx=fmc_obj.fdx();
    var_sct *var1=NULL_CEWI;
    var_sct *var2=NULL_CEWI;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;

    prs_cls *prs_arg=walker.prs_arg;    

           
    nbr_args=args_vtr.size();

    susg="usage: var_out="+sfnm+"(var_exp,var_map)\n";  


    if(nbr_args<2 )
       err_prn(sfnm,"Function requires two arguments\n"+susg); 


    // only warn on final scan 
    if(nbr_args>2 && !prs_arg->ntl_scn)
      err_prn(sfnm,"Function has more than two arguments"); 


    var1=walker.out(args_vtr[0]);
    var2=walker.out(args_vtr[1]);
    var2=nco_var_cnf_typ((nc_type)NC_UINT64, var2);


      
    if(prs_arg->ntl_scn){
       var2=nco_var_free(var2);
       return var1;    
     }
   
      // here usage PREMAP, PUNMAP 
      {
          char *cp_in;
          char *cp_out;
          long idx; 
          long jdx;
          long sz; 
          long sz_idx;
          long slb_sz;
          nco_uint64 *lp_mp; 
          var_sct *var_out;

          var_out=nco_var_dpl(var1); 
           

          sz=var2->sz;
          sz_idx=var_out->sz/var2->sz;

          // var size must be exactly divisble by map size
          if( var_out->sz % var2->sz != 0   ) {
             ostringstream os; 
	     os<<"Size of input var("<< var_out->sz<<") must be exactly divisble by map size(" << var2->sz<<")";
             err_prn(sfnm,os.str());
          }

          slb_sz=nco_typ_lng(var_out->type);
          cp_in=(char*)var1->val.vp; 
          cp_out=(char*)var_out->val.vp;

          // var2 contains the mapping
          (void)cast_void_nctype((nc_type)NC_UINT64,&var2->val);
          lp_mp=var2->val.ui64p; 
         
	  switch(fdx){ 
          
          case PREMAP:  
          for(idx=0; idx<sz_idx; idx++){ 
            cp_out=(char*)var_out->val.vp+ (ptrdiff_t)idx*sz*slb_sz;              
            for(jdx=0 ;jdx<sz; jdx++){
              // do bounds checking for the mapping
	      if(lp_mp[jdx] >=0 && lp_mp[jdx]< sz )
                // copy element from var1 to var_out
                (void)memcpy(cp_out+(ptrdiff_t)(lp_mp[jdx]*slb_sz),cp_in,slb_sz); 
	      cp_in+=(ptrdiff_t)slb_sz;
	    } // end jdx
          } //end idx; 
          break;
         
          // reverse map
          case PUNMAP:  
          for(idx=0; idx<sz_idx; idx++){ 
            cp_in=(char*)var1->val.vp+ (ptrdiff_t)idx*sz*slb_sz;              
            for(jdx=0 ;jdx<sz; jdx++){
              // do bounds checking for the mapping
	      if(lp_mp[jdx] >=0 && lp_mp[jdx]< sz )
                // copy element from var1 to var_out
                (void)memcpy(cp_out,cp_in+(ptrdiff_t)(lp_mp[jdx]*slb_sz),slb_sz); 
	      cp_out+=(ptrdiff_t)slb_sz;
	    } // end jdx
          } //end idx; 
          break;
         
	  } // end switch


         
          var1=nco_var_free(var1);
          (void)cast_nctype_void((nc_type)NC_UINT64,&var2->val);
          var2=nco_var_free(var2);
             
          var1=var_out;

 
      }


    return var1;


} // end srt_cls::mst_fnd()  





//Unary Functions /***********************************/
 
  unr_cls::unr_cls(bool flg_dbg){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("abs",this,PABS)); 
          fmc_vtr.push_back( fmc_cls("sqr",this,PSQR)); 

    }		      
  } 
  var_sct * unr_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("unr_cls::fnd");
    int fdx;
    int nbr_args;
    var_sct *var1;
    var_sct *var;   
    fdx=fmc_obj.fdx(); 
    std::string susg;
    std::string sfnm =fmc_obj.fnm(); 
    RefAST tr;
    std::vector<RefAST> args_vtr;    
    // de-reference
    prs_cls* prs_arg=walker.prs_arg;

    susg="usage: var_out="+sfnm+"(var_exp)";


    if(expr)
      args_vtr.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	args_vtr.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
     nbr_args=args_vtr.size();  


    // no arguments - bomb out
    if(nbr_args==0){    
        std::string serr;
	serr="Function has been called with no argument\n"+susg;               
        err_prn(sfnm,serr);
	// more than one arg -- only print message once 
    } else if(nbr_args >1 && !prs_arg->ntl_scn)
        wrn_prn(sfnm,"Function has been called with more than one argument");
           

    var1=walker.out(args_vtr[0]);   



    if(prs_arg->ntl_scn)
      return var1;      

    
    // do the deed
    switch(fdx){

      case PABS: 
        var1=ncap_var_var_op(var1,NULL_CEWI,VABS);
        break;             

      case PSQR: 
        var1=ncap_var_var_op(var1,NULL_CEWI,VSQR2);
        break;             
    }
     
    return var1;

  }



//Array Functions /***********************************/ 
  arr_cls::arr_cls(bool flg_dbg){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("array",this,PARRAY)); 

    }		      
  } 
  var_sct * arr_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("arr_cls::fnd");
  int fdx;
  int nbr_args;
  int idx;
  int nbr_dim;
  dmn_sct **dim;
  var_sct *var1=NULL_CEWI;
  var_sct *var2=NULL_CEWI;
  var_sct *var_ret;
           
  std::string susg;
  std::string sfnm=fmc_obj.fnm();

  RefAST tr;
  std::vector<RefAST> args_vtr; 
  std::vector<std::string> cst_vtr;              

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;            
  vtl_typ lcl_typ;

  fdx=fmc_obj.fdx();
 

  if(expr)
      args_vtr.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	args_vtr.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
  nbr_args=args_vtr.size();  

  susg="usage: var_out="+sfnm+"(start_exp,inc_exp,$dim|var)"; 

  
  if(nbr_args<3)
      err_prn(sfnm,"Function has been called with less than three arguments\n"+susg); 



  if(nbr_args >3 &&!prs_arg->ntl_scn) 
      wrn_prn(sfnm,"Function been called with more than three arguments"); 


   
  
  var1=walker.out(args_vtr[0]);  
  var2=walker.out(args_vtr[1]);  

  
  if(prs_arg->ntl_scn && var1->undefined ){
    var2=nco_var_free(var2);
    return var1;
  }

          

  /* third argument must be a single dimension */
  //if(args_vtr[2]->getType() != DIM_ID ) 
  //  err_prn(sfnm,"Third argument must be a dimension\n"+susg); 

  // cast a var from using the dim arg
  if(args_vtr[2]->getType() == DIM_ID ) 
  {
   cst_vtr.push_back(args_vtr[2]->getText());
     
   var_ret=ncap_cst_mk(cst_vtr,prs_arg);
   
   // convert to type of first arg
   var_ret=nco_var_cnf_typ(var1->type,var_ret);  

  }else{
    // assume third argument is var - interested in only using its shape !!  
    var_ret=walker.out(args_vtr[2]);
    
    for(idx=0; idx<var_ret->nbr_dim;idx++)
      cst_vtr.push_back(var_ret->dim[idx]->nm);

    var_ret=ncap_cst_mk(cst_vtr,prs_arg);
    // convert to type of first arg
    var_ret=nco_var_cnf_typ(var1->type,var_ret); 

  }

  if(prs_arg->ntl_scn){
    
    var1=nco_var_free(var1);
    var2=nco_var_free(var2);

    return var_ret; 

  }

    
  // do the deed
  switch(fdx){


    // This array method returns an array of numbers of type var1->type
    // The array starts at the first value in var1 and the increment is         
    // is the first value in var2. When necessary the var2 type is converted
    // to the var one type. The size of array is equal to the dimension size
    // arguments 
    // e.g  var_out=array(1,-3, $time);  
    //      var_out={1,-2,-5,-8,-11,-14,-17,-20,-23,-26 };        
    
     case PARRAY: {

         var2=nco_var_cnf_typ(var1->type,var2);  
         // malloc space
         var_ret->val.vp= (void*)nco_malloc(var_ret->sz*nco_typ_lng(var1->type));   

           switch (var1->type) {
             case NC_DOUBLE: 
	        (void)ncap_array<double>(var1,var2,var_ret);    
                break;  
             case NC_FLOAT: 
	        (void)ncap_array<float>(var1,var2,var_ret);    
                break;  
             case NC_INT: 
	        (void)ncap_array<nco_int>(var1,var2,var_ret);    
                break;  
             case NC_SHORT: 
	        (void)ncap_array<nco_short>(var1,var2,var_ret);    
                break;  
             case NC_USHORT: 
	        (void)ncap_array<nco_ushort>(var1,var2,var_ret);    
                break;  
             case NC_UINT: 
	        (void)ncap_array<nco_uint>(var1,var2,var_ret);    
                break;  
             case NC_INT64: 
	        (void)ncap_array<nco_int64>(var1,var2,var_ret);    
                break;  
             case NC_UINT64: 
	        (void)ncap_array<nco_uint64>(var1,var2,var_ret);    
                break;  
             case NC_BYTE: 
	        (void)ncap_array<nco_byte>(var1,var2,var_ret);    
                break;  
             case NC_UBYTE: 
	        (void)ncap_array<nco_ubyte>(var1,var2,var_ret);    
                break;  
             case NC_CHAR: 
	        (void)ncap_array<char>(var1,var2,var_ret);    
                break;  
             case NC_STRING: break; /* Do nothing */
             
            default: nco_dfl_case_nc_type_err(); break;
            
           } // end big switch
 
           var1=nco_var_free(var1);
           var2=nco_var_free(var2);
  
     } break;             


  } // end switch
     
  return var_ret;

  } 



//Bilinear  Interpolation Functions /****************************************/
  bil_cls::bil_cls(bool flg_dbg){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("bilinear_interp",this,PBIL_ALL)); 
          fmc_vtr.push_back( fmc_cls("bilinear_interp_wrap",this,PBIL_ALL_WRP)); 

    }		      
  } 
  var_sct * bil_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
   const std::string fnc_nm("bil_cls::fnd");
  bool bwrp;  //if tue then wrap X and Y coo-ordinates in grid
  bool b_rev_y;
  bool b_rev_x;
  int fdx;
  int nbr_args;
  int in_nbr_args;
  int idx;
  int nbr_dim;
  var_sct *var_arr[6];

  nc_type in_typ;           

  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::string serr;

  RefAST tr;
  std::vector<RefAST> vtr_args; 
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;            
  vtl_typ lcl_typ;

  fdx=fmc_obj.fdx();
 

  if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
  nbr_args=vtr_args.size();  

  switch(fdx){

    case PBIL_ALL:
           in_nbr_args=nbr_args;  
           susg="usage: var_out="+sfnm+"(Data_in, Data_out, X_out?, Y_out?, X_in?, Y_in?)"; 
           bwrp=false;
           break;

    case PBIL_ALL_WRP: 
           in_nbr_args=nbr_args;  
           susg="usage: var_out="+sfnm+"(Data_in, Data_out, X_out?, Y_out?, X_in?, Y_in?)"; 
           bwrp=true;
           break;


  } // end switch



    if(in_nbr_args <2 ){   
      serr="function requires at least two arguments. You have only supplied "+nbr2sng(in_nbr_args)+ " arguments\n"; 
      err_prn(sfnm,serr+susg);
    }


    if(in_nbr_args >6 &&!prs_arg->ntl_scn) 
      wrn_prn(sfnm,"Function been called with more than "+ nbr2sng(in_nbr_args)+ "arguments"); 

    // process input args 
    for(idx=0 ; idx<in_nbr_args; idx++)
      var_arr[idx]=walker.out(vtr_args[idx]);
 
    in_typ=var_arr[0]->type;    



    // initial scan
    if(prs_arg->ntl_scn){
        var_arr[1]=nco_var_cnf_typ(in_typ,var_arr[1]);
        for(idx=0 ; idx<in_nbr_args ; idx++)
	  if(idx !=1) nco_var_free(var_arr[idx]);

        return var_arr[1];
    }



    if(fdx==PBIL_ALL || fdx==PBIL_ALL_WRP){
      // recall input arguments in order
      // 0 - input data
      // 1 - output data
      // 2 - output X  co-ordinate var
      // 3 - output Y  co-ordinate var
      // 4 - input X   co-ordinate var
      // 5 - input Y   co-ordinate var
     

        
      if(in_nbr_args<4){
        if(var_arr[1]->nbr_dim <2 )
          err_prn(sfnm,"Output data variable "+std::string(var_arr[1]->nm) + " must have at least two dimensions ");

        // get output co-ordinate vars   
        if(in_nbr_args <3) 
          var_arr[2]=prs_arg->ncap_var_init(std::string(var_arr[1]->dim[0]->nm),true); 
           
        var_arr[3]=prs_arg->ncap_var_init(std::string(var_arr[1]->dim[1]->nm),true); 
      }

        
      if(in_nbr_args<6){
        if(var_arr[0]->nbr_dim <2 )
          err_prn(sfnm,"Input data variable "+std::string(var_arr[0]->nm) + " must have at least two dimensions ");

        // get input co-ordinate vars   
        if(in_nbr_args <5) 
          var_arr[4]=prs_arg->ncap_var_init(std::string(var_arr[0]->dim[0]->nm),true); 
           
        var_arr[5]=prs_arg->ncap_var_init(std::string(var_arr[0]->dim[1]->nm),true); 
      }



      // convert all args to type double and then cast
      for(idx=0 ; idx<6; idx++){
         var_arr[idx]=nco_var_cnf_typ(NC_DOUBLE,var_arr[idx]);
         (void)cast_void_nctype(NC_DOUBLE,&var_arr[idx]->val);
      }

      // call generic function
      (void)clc_bil_fnc(var_arr[4],var_arr[5],var_arr[0],var_arr[2],var_arr[3],var_arr[1],bwrp,sfnm);

      // cast back to void and free
      for(idx=0 ; idx<6; idx++){
        (void)cast_nctype_void(NC_DOUBLE,&var_arr[idx]->val);
        if(idx !=1) 
           (void)nco_var_free(var_arr[idx]);
      }
 
      // convert to input type
      var_arr[1]=nco_var_cnf_typ(in_typ,var_arr[1]);

      return var_arr[1];   

    }


  } // end fnc

// Generic function for bilinear interpolation
// function assumes all arguments are of type double and that
// v_xin & v_yin are monotonic(increasing or decreasing with respect to var->sz
void bil_cls::clc_bil_fnc(var_sct *v_xin,var_sct *v_yin, var_sct *v_din, var_sct *v_xout,var_sct *v_yout, var_sct *v_dout,bool bwrp,std::string sfnm){
   bool b_rev_x; // flg v_xin monotonic( increasing - false dcreasing true ) 
   bool b_rev_y; // flg v_yin monotonic( increasing - false dcreasing true ) 
   long x_sz;    // size of X dim in OUTPUT  
   long y_sz;    // size of Y dim in OUTPUT  
   long jdx;
   long kdx;


  // Sanity check for input/output data
  if( v_xin->sz *v_yin->sz != v_din->sz)
    err_prn(sfnm,"Dimension size mismatch with input variables\n"); 

  if( v_xout->sz *v_yout->sz != v_dout->sz)
    err_prn(sfnm,"Dimension size mismatch with output variables\n"); 


  b_rev_x=false; 
  // assume X input monotnic - if decreasing, reverse and set flag  
  if( v_xin->sz >1 && v_xin->val.dp[0]> v_xin->val.dp[1] ){
    reverse(v_xin->val.dp,v_xin->val.dp+v_xin->sz);
    b_rev_x=true;          
  }

  b_rev_y=false; 
  // assume Y input monotnic - if decreasing, reverse and set flag  
     if( v_yin->sz >1 && v_yin->val.dp[0]> v_yin->val.dp[1] ){
    reverse(v_yin->val.dp,v_yin->val.dp+v_yin->sz);
    b_rev_y=true;          
  }
  

   x_sz=v_xout->sz;
   y_sz=v_yout->sz;

            
   // deal with regular unwrapped grid
   if(!bwrp){ 


   for(jdx=0 ; jdx<y_sz;jdx++){
     long y_min; // min value in Y input co-ord
     long y_max; // max value in Y input co-ord
     long y_min_org;
     long y_max_org;

     // find range in input Y 
     y_min=(long)(lower_bound(v_yin->val.dp, v_yin->val.dp+v_yin->sz, v_yout->val.dp[jdx])-v_yin->val.dp);

     // do some bounds checking      
     if(y_min==v_yin->sz || y_min==0L && v_yout->val.dp[jdx]< v_yin->val.dp[0] )   
         err_prn(sfnm, "Bounding error with \""+string(v_yout->nm)+"\" output co-ordinate variable");   

     // not an exact match 
     if(v_yout->val.dp[jdx]< v_yin->val.dp[y_min])
       y_max=y_min--;
     else
     // exact match 
       y_max=y_min; 

     // Y co-ordinate reversed ?  
     if(b_rev_y){      
       y_min_org=v_yin->sz-y_min-1;
       y_max_org=v_yin->sz-y_max-1;
     }else{
       y_min_org=y_min;
       y_max_org=y_max;
     }  

     for(kdx=0;kdx<x_sz;kdx++){
       long x_min; // min value in X input co-ord
       long x_max; // max value in X input co-ord
       double Q[2][2]; 
       double d_int1; // intermediate values
       double d_int2; // intermediate values
       double rslt;
       // find range in input X 
       x_min=(long)(lower_bound(v_xin->val.dp, v_xin->val.dp+v_xin->sz, v_xout->val.dp[kdx])-v_xin->val.dp);     

       // do some bounds checking      
       if(x_min==v_xin->sz || x_min==0L && v_xout->val.dp[kdx]< v_xin->val.dp[0] )   
         err_prn(sfnm, "Bounding error with \""+string(v_xout->nm)+"\" output co-ordinate variable");   

       // not an exact match
       if( v_xout->val.dp[kdx] < v_xin->val.dp[x_min])
         x_max=x_min--;
       // an exact match
       else
         x_max=x_min;

       // X co-ordinate reversed
       if(b_rev_x){    
         Q[0][0]=v_din->val.dp[(v_xin->sz-x_min-1)*v_yin->sz+y_min_org];       
         Q[1][0]=v_din->val.dp[(v_xin->sz-x_max-1)*v_yin->sz+y_min_org];
         Q[0][1]=v_din->val.dp[(v_xin->sz-x_min-1)*v_yin->sz+y_max_org];          
         Q[1][1]=v_din->val.dp[(v_xin->sz-x_max-1)*v_yin->sz+y_max_org];        
       }else{ 
         Q[0][0]=v_din->val.dp[x_min*v_yin->sz+y_min_org];       
         Q[1][0]=v_din->val.dp[x_max*v_yin->sz+y_min_org];
         Q[0][1]=v_din->val.dp[x_min*v_yin->sz+y_max_org];          
         Q[1][1]=v_din->val.dp[x_max*v_yin->sz+y_max_org];        
       }

         
       if(x_min==x_max && y_min==y_max)
	 rslt=Q[0][0];       
       else if( y_min==y_max) 
         rslt=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][0],Q[1][0]);                            
       else if( x_min==x_max) 
         rslt=clc_lin_ipl(v_yin->val.dp[y_min],v_yin->val.dp[y_max],v_yout->val.dp[jdx],Q[0][0],Q[1][1]);               
       else{
         d_int1=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][0],Q[1][0]);               
         d_int2=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][1],Q[1][1]);         
         rslt=clc_lin_ipl(v_yin->val.dp[y_min],v_yin->val.dp[y_max],v_yout->val.dp[jdx],d_int1,d_int2);               
       }
       v_dout->val.dp[kdx*v_yout->sz+jdx]=rslt;


     }//end for kdx
 
   }// end for jdx 

   // deal with wrapped co-ordinates
   }else{      


   for(jdx=0 ; jdx<y_sz;jdx++){
     long y_min; // min value in Y input co-ord
     long y_max; // max value in Y input co-ord
     double y_min_dbl;
     double y_max_dbl;  

     // find range in input Y 
     y_min=(long)(lower_bound(v_yin->val.dp, v_yin->val.dp+v_yin->sz, v_yout->val.dp[jdx])-v_yin->val.dp);


      // point off RHS of grid  
      if(y_min==v_yin->sz){ 
        y_min--; y_max=0;
        y_min_dbl=v_yin->val.dp[y_min]; 
        y_max_dbl=v_yin->val.dp[y_max]+360.0; 
      // exact match  
      } else if(v_yout->val.dp[jdx]==v_yin->val.dp[y_min]){
        y_max=y_min; 
        y_min_dbl=y_max_dbl=v_yin->val.dp[y_min]; 
      // point off LHS side of grid
      } else if( y_min==0){
        y_min=v_yin->sz-1;y_max=0;
        y_min_dbl=v_yin->val.dp[y_min]-360.0; 
        y_max_dbl=v_yin->val.dp[y_max]; 
      // regular point in grid  
      }else{
        y_max=y_min--;
        y_min_dbl=v_yin->val.dp[y_min]; 
        y_max_dbl=v_yin->val.dp[y_max]; 
      } 



     for(kdx=0;kdx<x_sz;kdx++){
       long x_min; // min value in X input co-ord
       long x_max; // max value in X input co-ord
       double Q[2][2]; 
       double d_int1; // intermediate values
       double d_int2; // intermediate values
       double rslt;
       // find range in input X 
       x_min=(long)(lower_bound(v_xin->val.dp, v_xin->val.dp+v_xin->sz, v_xout->val.dp[kdx])-v_xin->val.dp);     


      // point off RHS of grid  
      if(x_min==v_xin->sz){ 
        x_min--; x_max=x_min-1;
       // exact match  
      } else if(v_xout->val.dp[kdx]==v_xin->val.dp[x_min]){
        x_max=x_min; 
      // point off LHS side of grid
      } else if( x_min==0){
          x_max=1;
      // regular point in grid  
      }else{
        x_max=x_min--;
      }  



       // X co-ordinate reversed
       if(b_rev_x){    
         Q[0][0]=v_din->val.dp[(v_xin->sz-x_min-1)*v_yin->sz+y_min];       
         Q[1][0]=v_din->val.dp[(v_xin->sz-x_max-1)*v_yin->sz+y_min];
         Q[0][1]=v_din->val.dp[(v_xin->sz-x_min-1)*v_yin->sz+y_max];          
         Q[1][1]=v_din->val.dp[(v_xin->sz-x_max-1)*v_yin->sz+y_max];        
       }else{ 
         Q[0][0]=v_din->val.dp[x_min*v_yin->sz+y_min];       
         Q[1][0]=v_din->val.dp[x_max*v_yin->sz+y_min];
         Q[0][1]=v_din->val.dp[x_min*v_yin->sz+y_max];          
         Q[1][1]=v_din->val.dp[x_max*v_yin->sz+y_max];        
       }

         
       if(x_min==x_max && y_min==y_max)
	 rslt=Q[0][0];       
       else if( y_min==y_max) 
         rslt=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][0],Q[1][0]);                            
       else if( x_min==x_max) 
         rslt=clc_lin_ipl(y_min_dbl,y_max_dbl,v_yout->val.dp[jdx],Q[0][0],Q[1][1]);               
       else{
         d_int1=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][0],Q[1][0]);               
         d_int2=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][1],Q[1][1]);         
         rslt=clc_lin_ipl(y_min_dbl,y_max_dbl,v_yout->val.dp[jdx],d_int1,d_int2);               
       }
       v_dout->val.dp[kdx*v_yout->sz+jdx]=rslt;


     }//end for kdx
 
   }// end for jdx 



   }// end else wrapped co-ordinates

} // end clc_bil_fnc 

 
//Linear interpolation formula
double bil_cls::clc_lin_ipl(double x1,double x2, double x, double Q0,double Q1){
  const double  _delta=1e-20;
  double n1,n2,d1;


  if( Q0==Q1 || fabs(d1=x2-x1)<_delta || fabs(n1=x-x1)<_delta )
    return Q0; 
  
  if( fabs(n2=x2-x) <_delta)
    return Q1;  
 
  return (n2*Q0+n1*Q1)/d1;
  

}



//Coordinate Functions /***********************************/ 
  cod_cls::cod_cls(bool flg_dbg){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("min_coords",this,PCOORD)); 

    }		      
  } 

  var_sct * cod_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("cod_cls::fnd");
  int fdx;
  int nbr_args;
  int nbr_dim;
  long lret;
  dmn_sct **dim;
  var_sct *var1=NULL_CEWI;
  var_sct *var2=NULL_CEWI;
  var_sct *var_ret;
           
  std::string susg;
  std::string sfnm=fmc_obj.fnm();

  RefAST tr;
  std::vector<RefAST> args_vtr; 
  std::vector<std::string> cst_vtr;              

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;            
  vtl_typ lcl_typ;

  fdx=fmc_obj.fdx();
 

  if(expr)
      args_vtr.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	args_vtr.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
  nbr_args=args_vtr.size();  

  
  susg="usage: crd_idx="+sfnm+"(crd_var,crd_value)"; 

  
  if(nbr_args<2)
      err_prn(sfnm,"Function has been called with less than two arguments\n"+susg); 



  if(nbr_args >2 &&!prs_arg->ntl_scn) 
      wrn_prn(sfnm,"Function been called with more than two arguments"); 

   
  var1=walker.out(args_vtr[0]);  
  var2=walker.out(args_vtr[1]);  

  
  if(prs_arg->ntl_scn  ){
    nco_var_free(var1);
    nco_var_free(var2);
    return ncap_sclr_var_mk(static_cast<std::string>("~coord_function"),(nc_type)NC_INT,false);  ;
  }


  {
    long idx;
    long sz;
    double dval;
    double dmin; 
    double *dp_crd; 
    
    var1=nco_var_cnf_typ(NC_DOUBLE,var1);
    var2=nco_var_cnf_typ(NC_DOUBLE,var2);

    // convert everything to type double         
    (void)cast_void_nctype(NC_DOUBLE,&(var1->val));
    (void)cast_void_nctype(NC_DOUBLE,&(var2->val));

    dp_crd=var1->val.dp;  
    dval=var2->val.dp[0];   
    sz=var1->sz;  
    
    // check limits  
    if(dval<dp_crd[0] || dval>dp_crd[sz-1]) 
      lret=-1;  
    else{
    
      for(idx=0 ; idx<sz-1 ; idx++)
        if( dval >= dp_crd[idx] && dval <= dp_crd[idx+1] ){  
	  lret=(dval-dp_crd[idx]<= dp_crd[idx+1]-dval ? idx: idx+1 );    
          break;
	}  

    }
    (void)cast_nctype_void(NC_DOUBLE,&var1->val);
    (void)cast_nctype_void(NC_DOUBLE,&var2->val);
   
  }

  nco_var_free(var1);   
  nco_var_free(var2);

  return ncap_sclr_var_mk(static_cast<std::string>("~coord_function"),(nco_int)lret);        
  }

