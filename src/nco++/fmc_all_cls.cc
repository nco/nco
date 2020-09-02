/* $Header$ */

/* Purpose: netCDF arithmetic processor class methods: families of functions/methods */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "fmc_all_cls.hh"
//Conversion Functions /***********************************/
 
  cnv_cls::cnv_cls(bool){
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
    int nbr_fargs=0;
    int fdx=fmc_obj.fdx();
    var_sct *var1=NULL_CEWI;
   
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

  agg_cls::agg_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("avg",this,(int)PAVG));
      fmc_vtr.push_back( fmc_cls("avgsqr",this,(int)PAVGSQR));
      fmc_vtr.push_back( fmc_cls("max",this,(int)PMAX));
      fmc_vtr.push_back( fmc_cls("mibs",this,(int)PMIBS));
      fmc_vtr.push_back( fmc_cls("mabs",this,(int)PMABS));
      fmc_vtr.push_back( fmc_cls("mebs",this,(int)PMEBS));
      fmc_vtr.push_back( fmc_cls("min",this,(int)PMIN));
      fmc_vtr.push_back( fmc_cls("rms",this,(int)PRMS));
      fmc_vtr.push_back( fmc_cls("rmssdn",this,(int)PRMSSDN));
      fmc_vtr.push_back( fmc_cls("sqravg",this,(int)PSQRAVG));
      fmc_vtr.push_back( fmc_cls("total",this,(int)PTTL));
      fmc_vtr.push_back( fmc_cls("ttl",this,(int)PTTL));
      fmc_vtr.push_back( fmc_cls("sum",this,(int)PTTL));
      fmc_vtr.push_back( fmc_cls("tabs",this,(int)PTABS));
      fmc_vtr.push_back( fmc_cls("ttlabs",this,(int)PTABS));

    }
  }		      
		      
  var_sct *agg_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("agg_cls::fnd");
            int fdx;
            int nbr_args;
            int idx;
            int nbr_dim;
            int avg_nbr_dim; 
            dmn_sct **dim=NULL_CEWI;
            dmn_sct **dim_nw=NULL_CEWI;  
            var_sct *var=NULL_CEWI;
            var_sct *var1=NULL_CEWI;
            var_sct *var_att_cll_mtd=NULL_CEWI;
           
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

            if((tr=fargs->getFirstChild())) {
              do  
	        vtr_args.push_back(tr);
              while((tr=tr->getNextSibling()));    
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
                var=ncap_sclr_var_mk(SCS("~dot_methods"),var1->type,false);    
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


          if(prs_arg->FLG_CLL_MTH) {
            NcapVar *Nvar;
            var_att_cll_mtd = ncap_att_cll_mtd(var1->nm,dim, avg_nbr_dim, (nco_op_typ)fdx);
            Nvar=new NcapVar(var_att_cll_mtd);
            // mark attribute as transient
            // this mean it is propagated to the LHS only once then deleted
            Nvar->flg_mem=true;
            prs_arg->var_vtr.push_ow(Nvar);
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
                    
                case PMIBS:
                    (void)nco_var_abs(var1->type,var1->sz,var1->has_mss_val,var1->mss_val,var1->val);
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_mibs,False,&ddra_info);
                    break;
                    
                case PMABS:
                    (void)nco_var_abs(var1->type,var1->sz,var1->has_mss_val,var1->mss_val,var1->val);
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_mabs,False,&ddra_info);
                    break;
                    
                case PMEBS:
                    (void)nco_var_abs(var1->type,var1->sz,var1->has_mss_val,var1->mss_val,var1->val);
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_avg,False,&ddra_info);
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    break;

                case PTABS:
                    (void)nco_var_abs(var1->type,var1->sz,var1->has_mss_val,var1->mss_val,var1->val);
                    var=nco_var_avg(var1,dim,avg_nbr_dim,nco_op_avg,False,&ddra_info);
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

   utl_cls::utl_cls(bool){
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
      fmc_vtr.push_back( fmc_cls("mask_miss",this,(int)MASK_MISS));
      /* synomn */
      fmc_vtr.push_back( fmc_cls("missing",this,(int)MASK_MISS));
      fmc_vtr.push_back( fmc_cls("linear_fill_miss",this,(int)LINEAR_FILL_MISS));
      fmc_vtr.push_back( fmc_cls("simple_fill_miss",this,(int)SIMPLE_FILL_MISS));
      fmc_vtr.push_back( fmc_cls("weighted_fill_miss",this,(int)WEIGHT_FILL_MISS));
     
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

    if((tr=fargs->getFirstChild())) {
      do  
	vtr_args.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
     nbr_args=vtr_args.size();  
      
     if(nbr_args ==0) 
       err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 



     // deal with is_miss in a seperate function
    /*
     if(fdx==NUM_MISS||fdx==HAS_MISS)
       return is_fnd(is_mtd, vtr_args,fmc_obj,walker);           
     if(fdx==GET_MISS)       
       return get_fnd(is_mtd, vtr_args,fmc_obj,walker);
    if(fdx==FILL_LINEAR_MISS)
      return fill_fnd(is_mtd, vtr_args,fmc_obj,walker);

    */

    switch(fdx)
    {
      case NUM_MISS:
      case HAS_MISS:
        return is_fnd(is_mtd, vtr_args,fmc_obj,walker);
        break;

      case GET_MISS:
        return get_fnd(is_mtd, vtr_args,fmc_obj,walker);
        break;

      case LINEAR_FILL_MISS:
        return linear_fill_fnd(is_mtd, vtr_args,fmc_obj,walker);
        break;

      case MASK_MISS:
        return mask_fnd(is_mtd, vtr_args,fmc_obj,walker);
        break;

      case SIMPLE_FILL_MISS:
      case WEIGHT_FILL_MISS:
        return fill_fnd(is_mtd, vtr_args,fmc_obj,walker);
        break;

        // do nothing just continue
      default:
        break;
    }



    if( fdx==SET_MISS || fdx==CH_MISS) {

      if( nbr_args==1) {
	serr=sfnm+ " is missing an argument";
        err_prn(fnc_nm,serr);
      }      
      var=walker.out(vtr_args[1] );
    } 
     
            
    lcl_typ=expr_typ(vtr_args[0]);         

    /* allow att identifier for RAM_DELETE */
    if(lcl_typ != VVAR &&  lcl_typ != VPOINTER &&  !(fdx == RAM_DELETE && lcl_typ==VATT)) {
      serr="The first operand of the " + sfnm+ " must be a variable identifier or a variable pointer only.";
      err_prn(fnc_nm,serr);
    }

    if(lcl_typ==VVAR || lcl_typ==VATT )       
       va_nm=vtr_args[0]->getText();
    else if(lcl_typ==VPOINTER)
      { // get contents of att for var-pointer  
      std::string att_nm=vtr_args[0]->getFirstChild()->getText();
      va_nm=ncap_att2var(prs_arg,att_nm);
    }

    Nvar=prs_arg->var_vtr.find(va_nm);


    /* Deal with GET_MISS as its different from other methods */


    if(prs_arg->ntl_scn) {
      if(var) var=nco_var_free(var);
      return ncap_sclr_var_mk(SCS("~utility_function"),(nc_type)NC_INT,false);  
    }



    if(!Nvar ){
       wrn_prn(fnc_nm,sfnm+" unable to find variable: "+va_nm); 
       if(var) var=nco_var_free(var);
       return ncap_sclr_var_mk(SCS("~utility_function"),(nco_int)rval);        
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
               if(Nvar->flg_mem==false){
                 wrn_prn(fnc_nm,sfnm+" cannot remove disk variable:\""+va_nm+ "\". Delete can only remove RAM variables.");
                 rval=0;  
	        }else{ 
                 prs_arg->var_vtr.erase_all(va_nm); 
                 prs_arg->int_vtr.erase_all(va_nm); 
                 rval=1;
               }
             } 
             if(vtr_args[0]->getType()==ATT_ID){ 
               prs_arg->var_vtr.erase(va_nm);
               rval=1; 
	     }
            } break;    

    }// end switch

    return ncap_sclr_var_mk(SCS("~utility_function"),(nco_int)rval);        

  }// end function

  
var_sct * utl_cls::is_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("utl_cls::is_fnd");
    // int nbr_args;
    int fdx=fmc_obj.fdx();
    long icnt=0;
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
  


    var=walker.out(args_vtr[0] );

    if(prs_arg->ntl_scn){
       nco_var_free(var);     
       return ncap_sclr_var_mk(SCS("~utility_function"),styp,false);         
    }  

    if(!var->has_mss_val)      
      icnt=0; 
    else if( fdx==HAS_MISS)    
      icnt=1; 
    else if( fdx==NUM_MISS)   
    {
      char *cp_out=(char*)var->val.vp; 
      long idx;
      size_t slb_sz;  

      icnt=0;       

      slb_sz=nco_typ_lng(var->type); 
      for(idx=0 ;idx<var->sz;idx++)
      {
        if( !memcmp(cp_out,var->mss_val.vp,slb_sz))
	  icnt++;
        cp_out+=(ptrdiff_t)slb_sz;
      }   
    }
    nco_var_free(var); 

    if(styp==NC_UINT64) 
       return ncap_sclr_var_mk(SCS("~utility_function"),(nco_uint64)icnt);
    else 
      return ncap_sclr_var_mk(SCS("~utility_function"),(nco_int)icnt);
}  

 
// custom function for GET_MISS
var_sct * utl_cls::get_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("utl_cls::get_fnd");
    int nbr_args;
    // int fdx=fmc_obj.fdx();
    var_sct *var=NULL_CEWI;
    var_sct *var_ret=NULL_CEWI;
    std::string sfnm =fmc_obj.fnm(); //method name
    vtl_typ lcl_typ;
    std::string va_nm;
    std::string susg;
    std::string serr;
    prs_cls *prs_arg=walker.prs_arg;    


    nbr_args=args_vtr.size(); 

    if(nbr_args ==0) 
       err_prn(fnc_nm," \""+sfnm+"\" has been called with no arguments"); 


      
    lcl_typ=expr_typ(args_vtr[0]);         

    if(lcl_typ != VVAR &&  lcl_typ != VPOINTER ) 
    {
      serr="The first operand of the " + sfnm+ " must be a variable identifier or a variable pointer only.";
      err_prn(fnc_nm,serr);
    }

    if(lcl_typ==VVAR )       
       va_nm=args_vtr[0]->getText();
    else if(lcl_typ==VPOINTER)
    { 
      // get contents of att for var-pointer  
      std::string att_nm=args_vtr[0]->getFirstChild()->getText();
      va_nm=ncap_att2var(prs_arg,att_nm);
    }


    if(va_nm.size())
       var=prs_arg->ncap_var_init(va_nm,false);

    // Initial scan
    if(prs_arg->ntl_scn) 
    {
        if(var)  
	  var_ret= ncap_sclr_var_mk(SCS ("~utility_function"),var->type,false);
        else
	  var_ret=ncap_var_udf("~utility_function");

        nco_var_free(var);  
        return var_ret;      
    }     

    // Final scan
    if(var)
    {
        // nb ncap_sclr_var_mk() calls nco_mss_val_mk() and fills var_ret with the default fill value
	// for that type.  So if the var has no missing value then this is the value returned 
	// Default fill  values are defined in  netcdf.h . 
	var_ret=ncap_sclr_var_mk(SCS("~utility_function"),var->type,true);
        if(var->has_mss_val)
	  (void)memcpy(var_ret->val.vp, var->mss_val.vp,nco_typ_lng(var->type)); 
    }
    else
    {          
        /* Cant find variable blow out */ 
        serr=sfnm+ " Unable to locate missing value for "+ va_nm;
        err_prn(fnc_nm,serr);
    } 

 
    if(var) 
      var=nco_var_free(var);

    return var_ret; 	

}

var_sct * utl_cls::linear_fill_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("utl_cls::fill_linear_fnd");
  nco_bool do_permute=False;
  int idx;
  int jdx;
  int nbr_dim;
  int nbr_args;
  int re_dim_nbr;
  var_sct *var=NULL_CEWI;
  var_sct *var_int=NULL_CEWI;
  std::string sfnm =fmc_obj.fnm(); //method name
  std::string var_nm;
  std::string dim_nm;
  std::string susg;
  prs_cls *prs_arg=walker.prs_arg;


  sfnm =fmc_obj.fnm(); //method name

  susg="usage: var_out="+sfnm+"(var_in,$dim?)";


  nbr_args=args_vtr.size();
  var=walker.out(args_vtr[0] );
  nbr_dim=var->nbr_dim;

  if(nbr_args==0)
    err_prn(sfnm,"Function has been called with no arguments\n"+susg);

  if(prs_arg->ntl_scn){
    return var;

  }

  if(nbr_args >1)
  {
    RefAST aRef=args_vtr[1];
    if(aRef->getType() != DIM_ID)
      err_prn(sfnm, "Second argument must be a single dimension\n"+susg);

    dim_nm=aRef->getText();

    for(idx=0;idx<nbr_dim;idx++)
      if(!strcmp(var->dim[idx]->nm, dim_nm.c_str())) break;

    if(idx==nbr_dim)
      err_prn(sfnm, "Unable to find dim " + dim_nm +" in var "+ SCS(var->nm)+".");

    re_dim_nbr=idx;

  }
  // dim not specified so choose last dim in var
  else
  {
    re_dim_nbr = var->nbr_dim - 1;
    dim_nm=SCS(var->dim[re_dim_nbr]->nm );
  }

  std::vector<nco_bool>  bool_vtr(nbr_dim,False);
  std::vector<int> dmn_idx_in_out(nbr_dim,0);
  std::vector<int> dmn_idx_out_in(nbr_dim,0);

  for(idx=0;idx<nbr_dim;idx++)
    dmn_idx_in_out[idx]=idx;

  // do we need to permute dims ?
  if(nbr_dim ==1 || re_dim_nbr == nbr_dim-1)
  {
   do_permute=False;

  }
  else
  {
    dmn_sct *swp_dim;
    do_permute=True;
    // swap about last value;
    dmn_idx_in_out[re_dim_nbr]=nbr_dim-1;
    dmn_idx_in_out[nbr_dim-1]=re_dim_nbr;

    var_int=nco_var_dpl(var);
    swp_dim=var->dim[re_dim_nbr];

    var_int->dim[re_dim_nbr]=var_int->dim[nbr_dim-1];
    var_int->dim[nbr_dim-1]=swp_dim;

    // create "out_in" mapping from "in_out" mapping
    for(idx=0 ; idx <nbr_dim ; idx++)
      for(jdx=0 ; jdx<nbr_dim; jdx++)
        if( idx==dmn_idx_in_out[jdx]){
          dmn_idx_out_in[idx]=jdx;
          break;
        }



  }

  //do opera
  if(do_permute)
  {
    (void) nco_var_dmn_rdr_val(var, var_int, &dmn_idx_out_in[0], &bool_vtr[0]);
  }

  // do fill
  if(1)
  {

    int slb_sz=var_int->dim[nbr_dim-1]->cnt;
    int sz=var->sz/slb_sz;
    double *dp;

    cast_void_nctype(NC_DOUBLE,&var_int->val);
    dp=var_int->val.dp;

    for(idx=0;idx<sz;idx++)
      for(jdx=0;jdx<slb_sz;jdx++)
        dp[idx*slb_sz+jdx]=jdx;


    cast_nctype_void(NC_DOUBLE,&var_int->val);
  }

  // permute back to original state
  if(do_permute)
  {

    (void) nco_var_dmn_rdr_val(var_int, var, &dmn_idx_in_out[0], &bool_vtr[0]);
    var_int = (var_sct *) nco_var_free(var_int);

  }

  return var;



}


/* fill_miss() method  assumes that the final two dims of the var are lat,lon */
/* if type not double then converts to double for fill, then converts back to orginal type */
var_sct * utl_cls::fill_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("utl_cls::fill_fnd");
  int idx;
  int nbr_dim;
  int nbr_args;
  int fdx=fmc_obj.fdx();
  double *lat_dp=NULL_CEWI;
  double *lon_dp=NULL_CEWI;
  nc_type lcl_typ;
  var_sct *var=NULL_CEWI;
  var_sct *var_lat=NULL_CEWI;
  var_sct *var_lon=NULL_CEWI;

  std::string sfnm =fmc_obj.fnm(); //method name
  std::string susg;
  prs_cls *prs_arg=walker.prs_arg;


  sfnm =fmc_obj.fnm(); //method name

  susg="usage: var_out="+sfnm+"(var_in)";



  nbr_args=args_vtr.size();
  var=walker.out(args_vtr[0] );
  nbr_dim=var->nbr_dim;

  if(nbr_args==0)
    err_prn(sfnm,"Function has been called with no arguments\n"+susg);

  if(prs_arg->ntl_scn){
    return var;

  }

  // number of dims
  if(var->nbr_dim<2)
    err_prn(sfnm,"variable must have a least 2 dims.\"" + SCS(var->nm) + "\" has "+nbr2sng(var->nbr_dim)+ " dims." );

  lcl_typ=var->type;
  // if not double then convert to double
  if(var->type != NC_DOUBLE )
    nco_var_cnf_typ(NC_DOUBLE, var);

  nbr_dim = var->nbr_dim;

  if(fdx==WEIGHT_FILL_MISS) {
    // grab lat & lon -- assume 1D and there names are the same as their dim names and the dim order is lat,lon
    var_lat = prs_arg->ncap_var_init(std::string(var->dim[nbr_dim - 2]->nm), true); // lat second to last
    var_lon = prs_arg->ncap_var_init(std::string(var->dim[nbr_dim - 1]->nm), true); // lon last dim

    if (var_lat && var_lon) {

      if (var_lat->type != NC_DOUBLE)
        nco_var_cnf_typ(NC_DOUBLE, var_lat);

      if (var_lon->type != NC_DOUBLE)
        nco_var_cnf_typ(NC_DOUBLE, var_lon);

      cast_void_nctype(var_lat->type, &var_lat->val);
      cast_void_nctype(var_lon->type, &var_lon->val);

      lat_dp = var_lat->val.dp;
      lon_dp = var_lon->val.dp;
    }
    else
      err_prn(sfnm,"to get the lat/lon coord-variables this function assumes that they are named after the final two dims in your variable argument.");


  }

  {
    // move through data in blocks if number of dims >2
    int blk_nbr;
    size_t blk_sz;
    size_t slb_sz;
    void *vp;
    void *msk_vp = NULL_CEWI;


    blk_sz = var->dim[nbr_dim - 2]->cnt * var->dim[nbr_dim - 1]->cnt;
    blk_nbr = var->sz / blk_sz;

    slb_sz = nco_typ_lng(var->type);

    // save pointer to restore later
    vp = var->val.vp;
    // create space here for use in beta_fill
    msk_vp = nco_malloc(blk_sz * slb_sz);

    for (idx = 0; idx < blk_nbr; idx++) {
      var->val.vp = (char *) vp + (ptrdiff_t) (blk_sz * idx * slb_sz);
      if(fdx==SIMPLE_FILL_MISS)
         simple_fill(var, msk_vp);
      else if(fdx==WEIGHT_FILL_MISS)
         weight_fill(var,msk_vp,lat_dp,lon_dp);
    }

    var->val.vp = vp;


    msk_vp = nco_free(msk_vp);

  }

  // convert back to original type
  if(lcl_typ != var->type)
     nco_var_cnf_typ(lcl_typ, var);


  // free lat/lon
  if(fdx==WEIGHT_FILL_MISS)
  {
    cast_nctype_void(var_lat->type,&var_lat->val);
    cast_nctype_void(var_lon->type,&var_lon->val);
    var_lat=nco_var_free(var_lat);
    var_lon=nco_var_free(var_lon);
  }



  return var;

}



/* simple fill function for replacing _FillValue with average of nearest neighbour(s) */
int utl_cls::simple_fill(var_sct* var, void* msk_vp){

  // we now have a 2 D var assume [lat, lon]
  int idx;
  int jdx;
  int nbr_dim=var->nbr_dim;
  int imax_loop=1000;
  int num_miss;
  int cnt=0;
  int lat_sz;
  int lon_sz;
  size_t slb_sz;
  double dbl_mss_val;
  double **msk_dp=NULL_CEWI;
  double **dp=NULL_CEWI;

  slb_sz=nco_typ_lng(var->type);
  lat_sz=var->dim[nbr_dim-2]->cnt;
  lon_sz=var->dim[nbr_dim-1]->cnt;


  // make indexing easier
  msk_dp=(double**)nco_malloc( sizeof(double*) * lat_sz);
  dp=(double**)nco_malloc( sizeof(double*) * lat_sz);

  cast_void_nctype(var->type, &var->val);
  if(var->has_mss_val)
    dbl_mss_val=*var->mss_val.dp;
  else
    dbl_mss_val=NC_FILL_DOUBLE;


  // make indexing easier
  for(idx=0;idx<lat_sz;idx++) {
    dp[idx] = &(var->val.dp[lon_sz * idx]);
    msk_dp[idx]= (double*)msk_vp+ptrdiff_t(lon_sz*idx);

  }

  // set num miss to get loop going
  num_miss=999;


  while( imax_loop-->0 && num_miss>0) {
    double sum=0.0;
    num_miss=0;
    // set msk to latest values
    memcpy((char*)msk_vp, (char*)var->val.dp, lat_sz * lon_sz * slb_sz);

    // move from bottom to top (lat)  and left to right (lon)
    for (idx = 0 ; idx < lat_sz; idx++)
      for (jdx = 0; jdx < lon_sz; jdx++)
        if (msk_dp[idx][jdx] == dbl_mss_val) {
          sum = 0.0;
          cnt = 0;
          if (idx > 0 && msk_dp[idx - 1][jdx] != dbl_mss_val) {
            sum += msk_dp[idx - 1][jdx];
            cnt++;
          }

          if (idx < lat_sz - 1 && msk_dp[idx + 1][jdx] != dbl_mss_val) {
            sum += msk_dp[idx + 1][jdx];
            cnt++;
          }

          if (jdx > 0 && msk_dp[idx][jdx - 1] != dbl_mss_val) {
            sum += msk_dp[idx][jdx - 1];
            cnt++;
          }

          if (jdx < lon_sz - 1 && msk_dp[idx][jdx + 1] != dbl_mss_val) {
            sum += msk_dp[idx][jdx + 1];
            cnt++;
          }

          if (cnt > 0)
            dp[idx][jdx] = sum / cnt;
          else
            num_miss++;
        }

    // get number of missing elements
    // printf("beta_fill: inum=%d  num_miss=%ld\n", imax_loop, num_miss);


  }

  cast_nctype_void(var->type,&var->val);

  dp=(double**)nco_free(dp);
  msk_dp=(double**)nco_free(dp);

  return NCO_NOERR;

}


/* fill function uses weighted value of eight nearest neighbours */
int utl_cls::weight_fill(var_sct* var, void* msk_vp, double *lat, double *lon){

  // we now have a 2 D var assume [lat, lon]
  int idx;
  int jdx;
  int nbr_dim=var->nbr_dim;
  int imax_loop=1000;
  int num_miss;
  int lat_sz;
  int lon_sz;
  size_t slb_sz;
  double dbl_mss_val;
  double **msk_dp=NULL_CEWI;
  double **dp=NULL_CEWI;

  slb_sz=nco_typ_lng(var->type);
  lat_sz=var->dim[nbr_dim-2]->cnt;
  lon_sz=var->dim[nbr_dim-1]->cnt;


  // make indexing easier
  msk_dp=(double**)nco_malloc( sizeof(double*) * lat_sz);
  dp=(double**)nco_malloc( sizeof(double*) * lat_sz);

  cast_void_nctype(var->type, &var->val);
  if(var->has_mss_val)
    dbl_mss_val=*var->mss_val.dp;
  else
    dbl_mss_val=NC_FILL_DOUBLE;


  // make indexing easier
  for(idx=0;idx<lat_sz;idx++) {
    dp[idx] = &(var->val.dp[lon_sz * idx]);
    msk_dp[idx]= (double*)msk_vp+ptrdiff_t(lon_sz*idx);

  }

  // set num miss to get loop going
  num_miss=999;


  while( imax_loop-->0 && num_miss>0) {

    num_miss = 0;
    // set msk to latest values
    memcpy((char *) msk_vp, (char *) var->val.dp, lat_sz * lon_sz * slb_sz);

    // move from bottom to top (lat)  and left to right (lon)
    for (idx = 0; idx < lat_sz; idx++)
      for (jdx = 0; jdx < lon_sz; jdx++)
        if (msk_dp[idx][jdx] == dbl_mss_val) {
          // sum numerator
          double sum_nd = 0.0;
          //  sum denominator
          double sum_dd = 0.0;
          // distance from target point - to neighbour
          double dist = 0.0;
          int cnt = 0;

          for (int xdx = idx - 1; xdx <= idx + 1; xdx++) {
            if (xdx < 0 || xdx >= lat_sz) continue;

            for (int ydx = jdx - 1; ydx <= jdx + 1; ydx++) {
              if (ydx < 0 || ydx >= lon_sz || (xdx == idx && ydx == jdx)) continue;

              if (msk_dp[xdx][ydx] != dbl_mss_val) {
                dist = point2point(lat[idx], lon[jdx], lat[xdx], lon[ydx]);
                sum_nd += msk_dp[xdx][ydx] / dist /dist;
                sum_dd += 1.0 / dist / dist ;
                cnt++;
              }

              if (cnt > 0)
                dp[idx][jdx] = sum_nd / sum_dd;
              else
                num_miss++;
            }
          }
        }
  }

    // get number of missing elements
    // printf("beta_fill: inum=%d  num_miss=%ld\n", imax_loop, num_miss);




  cast_nctype_void(var->type,&var->val);

  dp=(double**)nco_free(dp);
  msk_dp=(double**)nco_free(dp);

  return NCO_NOERR;

}


// distance between two points on a great circle
double utl_cls::point2point(double lat1,double lon1,double lat2, double lon2) {

double dist;
double alpha;


  if(lon1==lon2)
    return fabs(lat2-lat1);
  else if(lat1==lat2)
    alpha=  pow(sin(lat1),2.0) +  pow( cos(lat1),2.0) * cos(lon2-lon1);
  else
    alpha=sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon2-lon1);

  dist=fabs(acos(alpha));

  return dist;

}


var_sct * utl_cls::mask_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker) {
  const std::string fnc_nm("utl_cls::mask_fnd");
  int nbr_args;
  var_sct *var = NULL_CEWI;
  var_sct * var_miss= NULL_CEWI;
  std::string sfnm = fmc_obj.fnm(); //method name
  std::string var_nm;
  std::string dim_nm;
  std::string susg;
  prs_cls *prs_arg = walker.prs_arg;


  sfnm = fmc_obj.fnm(); //method name

  susg = "usage: var_out=" + sfnm + "(var_in)";


  nbr_args = args_vtr.size();
  var = walker.out(args_vtr[0]);

  if (nbr_args == 0)
    err_prn(sfnm, "Function has been called with no arguments\n" + susg);

  if (prs_arg->ntl_scn)
  {
    if(var->has_mss_val)
      if(var->mss_val.vp) {
        var->mss_val.vp = nco_free(var->mss_val.vp);
        var->has_mss_val = False;
      }


    return var;

  }

  /* remember the default fill for var->type is used for val.vp in this function call */
  var_miss=ncap_sclr_var_mk("~var_miss",var->type,true);

  if(var->has_mss_val){

    (void)memcpy(var_miss->val.vp, var->mss_val.vp, nco_typ_lng(var->type));
    var->has_mss_val=False;
    var->mss_val.vp=(void*)NULL;

  }else{
   wrn_prn(sfnm,"Warning method is using default fill value as \""+ SCS(var->nm)+ "\" has no missing value.");

  }

  /* remember this function calls frees up second operand miss_var */
  (void)ncap_var_var_op(var,var_miss, EQ );

  return var;


}







//Basic Functions /******************************************/

  bsc_cls::bsc_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("size",this,(int)PSIZE));
      fmc_vtr.push_back( fmc_cls("type",this,(int)PTYPE));
      fmc_vtr.push_back( fmc_cls("ndims",this,(int)PNDIMS));
      fmc_vtr.push_back( fmc_cls("getdims",this,(int)PGETDIMS));
      fmc_vtr.push_back( fmc_cls("exists",this,(int)PEXISTS));

    }
  }

  var_sct *bsc_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("bsc_cls::fnd");
  bool is_mtd;
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

    if((tr=fargs->getFirstChild())) {
      do  
	vtr_args.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
     nbr_args=vtr_args.size();  
   
     is_mtd=(expr ? true: false);
  

    // no arguments - bomb out
    if(nbr_args==0){    
        std::string serr;
	serr="Function has been called with no argument\n"+susg;               
        err_prn(sfnm,serr);
	// more than one arg -- only print message once 
    } else if(nbr_args >1 && !prs_arg->ntl_scn)
        wrn_prn(sfnm,"Function has been called with more than one argument");
     
    // custom functiom
    if(fdx==PGETDIMS)
      return getdims_fnd(is_mtd, vtr_args,fmc_obj, walker); 

    tr=vtr_args[0];  


    lcl_typ=expr_typ(tr);          
   

    // If initial scan
    if(prs_arg->ntl_scn){

      // Evaluate argument on first scan for side-effects eg var1++ or var1+=10 etc 
      if( lcl_typ!=VVAR && lcl_typ !=VATT && lcl_typ !=VPOINTER){
      var1=walker.out(tr) ;
      var1=nco_var_free(var1);
      }
      return ncap_sclr_var_mk(SCS("~basic_function"),(nc_type)NC_INT,false);        
    }
       
    // from here on dealing with final scan
    if(lcl_typ==VVAR || lcl_typ==VATT )       
       va_nm=vtr_args[0]->getText();
    else if(lcl_typ==VPOINTER)
      { // get contents of att for var-pointer  
      std::string att_nm=vtr_args[0]->getFirstChild()->getText();
      va_nm=ncap_att2var(prs_arg,att_nm);
    }




    // deal with PEXISTS here 
    if(fdx==PEXISTS){
      int iret=0;
      switch(lcl_typ){

      case VPOINTER: 
        case VVAR: 
          if(prs_arg->ncap_var_init_chk(va_nm)) 
            iret=1;
          break;
        case VATT:
	  if( prs_arg->var_vtr.find(va_nm) !=NULL)
	    iret=1;
          else if( (var1=ncap_att_init(va_nm,prs_arg))!=NULL) 
            iret=1;
          break;   

      default:
	 var1=walker.out(tr);  
         iret=1; 
         break;
      }   
      
      if(var1) nco_var_free(var1);    
      return ncap_sclr_var_mk(SCS("~basic_function"),(nco_int)iret);
    }

     
      if(lcl_typ==VVAR || lcl_typ==VPOINTER)
	var1=prs_arg->ncap_var_init(va_nm,false); 
      else 
        var1=walker.out(tr);
    

      switch(fdx){ 
           case PSIZE:
             if(mp_typ==NC_UINT64) 
               var=ncap_sclr_var_mk(SCS("~basic_function"),(nco_uint64)var1->sz);
             else 
               var=ncap_sclr_var_mk(SCS("~basic_function"),(nco_int)var1->sz);
             break;
           case PTYPE:
             var=ncap_sclr_var_mk(SCS("~basic_function"),(nco_int)var1->type);
             break;
           case PNDIMS:
             var=ncap_sclr_var_mk(SCS("~basic_function"),(nco_int)var1->nbr_dim);            

      } // end switch        
      
      var1=nco_var_free(var1);
      return var;		 
  }       


var_sct * bsc_cls::getdims_fnd(bool &, std::vector<RefAST> &vtr_args, fmc_cls &fmc_obj, ncoTree &walker)
{
  const std::string fnc_nm("bsc_cls::getndims_fnd");
    int nbr_args;
    // int fdx=fmc_obj.fdx();
    var_sct *var=NULL_CEWI;
    var_sct *var_att=NULL_CEWI;
    std::string sfnm =fmc_obj.fnm(); //method name
    vtl_typ lcl_typ;
    std::string va_nm;
    std::string susg;
    std::string serr;
    prs_cls *prs_arg=walker.prs_arg;    


    nbr_args=vtr_args.size(); 

    if(nbr_args ==0) 
       err_prn(fnc_nm," \""+sfnm+"\" has been called with no arguments"); 


      
    lcl_typ=expr_typ(vtr_args[0]);         

    // If initial scan
    if(prs_arg->ntl_scn){

      // Evaluate argument on first scan for side-effects eg var1++ or var1+=10 etc 
      if( lcl_typ!=VVAR && lcl_typ !=VPOINTER)
      {
        var=walker.out(vtr_args[0]) ;
        var=nco_var_free(var);
      }
      return ncap_var_udf("~zz@getndims");          
    }
       
    // from here on dealing with final scan
    if(lcl_typ==VVAR )
    {       
       va_nm=vtr_args[0]->getText();
       var=prs_arg->ncap_var_init(va_nm,false); 
    }
    else if(lcl_typ==VPOINTER)
    { // get contents of att for var-pointer  
      std::string att_nm=vtr_args[0]->getFirstChild()->getText();
      va_nm=ncap_att2var(prs_arg,att_nm);
      var=prs_arg->ncap_var_init(va_nm,false); 
    }
    else
      var=walker.out(vtr_args[0]) ;

    // blow out if scalar var or is att
    if(var->nbr_dim==0 || ncap_var_is_att(var) )      
      err_prn(sfnm,"Cannot get dim names from a scalar var or an att\n"); 

    // do heavy lifting
    { 
      int ndims;
      int idx;
 
      ndims=var->nbr_dim;
     
      var_att=ncap_sclr_var_mk("~zz@getndims",NC_STRING,false);

      var_att->val.vp=(void*)nco_malloc(ndims* nco_typ_lng(NC_STRING));         
      var_att->sz=ndims; 

      (void)cast_void_nctype((nc_type)NC_STRING,&var_att->val);                  
      for(idx=0;idx<ndims;idx++)     
	var_att->val.sngp[idx]=strdup(var->dim[idx]->nm);

      (void)cast_nctype_void((nc_type)NC_STRING,&var_att->val); 
    }

    nco_free(var);   

    return var_att;
}
  
//Math Functions /******************************************/
  mth_cls::mth_cls(bool){
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
      
#if defined(LINUX) || defined(LINUXAMD64) || defined(MACOSX) || defined(_MSC_VER)
      sym_vtr.push_back(sym_cls("erf",erf,erff));
      sym_vtr.push_back(sym_cls("erfc",erfc,erfcf));
      sym_vtr.push_back(sym_cls("gamma",tgamma,tgammaf));
#endif /* !LINUX */
      
       /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh
	  20020703: AIX, SGI*, WIN32 do not define acoshf, asinhf, atanhf
	  20050610: C99 mandates support for acosh(), asinh(), atanh(), cosh(), sinh(), tanh()
	  20160515: Add hyperbolic function support to MACOSX
	  Eventually users without C99 will forego ncap */
#if defined(LINUX) || defined(LINUXAMD64) || defined(MACOSX)
      sym_vtr.push_back(sym_cls("acosh",acosh,acoshf));
      sym_vtr.push_back(sym_cls("asinh",asinh,asinhf));
      sym_vtr.push_back(sym_cls("atanh",atanh,atanhf));
      sym_vtr.push_back(sym_cls("cosh",cosh,coshf));
      sym_vtr.push_back(sym_cls("sinh",sinh,sinhf));
      sym_vtr.push_back(sym_cls("tanh",tanh,tanhf));
#endif /* !LINUX */
      
      /* Advanced Rounding: nearbyint, rint, round, trunc */
      /* Advanced Rounding: nearbyint, round, trunc */
      /* 20020703: AIX, MACOSX, SGI*, WIN32 do not define rintf()
	 Only LINUX* supplies all of these and I do not care about them enough
	 to activate them on LINUX* but not on MACOSX* and SUN* */
      /* 20130326: Re-activate these functions on all architectures */
      sym_vtr.push_back(sym_cls("nearbyint",nearbyint,nearbyintf)); /* Round to integer value in floating point format using current rounding direction, do not raise inexact exceptions */
      sym_vtr.push_back(sym_cls("rint",rint,rintf)); /* Round to integer value in floating point format using current rounding direction, raise inexact exceptions */
      sym_vtr.push_back(sym_cls("round",round,roundf)); /* Round to nearest integer away from zero */
      sym_vtr.push_back(sym_cls("trunc",trunc,truncf)); /* Round to nearest integer not larger in absolute value */
  
      // Create fmc vector from sym vector
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

    if((tr=fargs->getFirstChild())) {
      do  
	vtr_args.push_back(tr);
      while((tr=tr->getNextSibling()));    
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
  mth2_cls::mth2_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("pow",this,(int)PPOW));
      fmc_vtr.push_back( fmc_cls("atan2",this,(int)PATAN2));
      fmc_vtr.push_back( fmc_cls("convert",this,(int)PCONVERT));
      fmc_vtr.push_back( fmc_cls("xratio",this,(int)PXRATIO));
      fmc_vtr.push_back( fmc_cls("solar_zenith_angle",this,(int)PSOLARZENITHANGLE));
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

    if((tr=fargs->getFirstChild())) {
      do  
	vtr_args.push_back(tr);
      while((tr=tr->getNextSibling()));    
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
      


     // check for undefined
     if( var1->undefined || var2->undefined ) {
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

      case PCONVERT:          
        if(prs_arg->ntl_scn){
          var=ncap_var_udf("~mth2_cls");  
        }else{

          /* Change type to int */
          int c_typ;
          var2=nco_var_cnf_typ(NC_INT,var2);
          (void)cast_void_nctype(NC_INT,&var2->val);
          c_typ=var2->val.ip[0];      
          (void)cast_nctype_void(NC_INT,&var2->val);
          var2=nco_var_free(var2);

          var=nco_var_cnf_typ( (nc_type)c_typ, var1);
        } 
        break;
      
      case  PXRATIO: 
	if( var1->type == NC_DOUBLE || var2->type == NC_DOUBLE )
	{       
            var1=nco_var_cnf_typ( NC_DOUBLE, var1);
            var2=nco_var_cnf_typ( NC_DOUBLE, var2);
        }
        else if ( var1->type == NC_FLOAT || var2->type == NC_FLOAT )
	{        
            var1=nco_var_cnf_typ( NC_FLOAT, var1);
            var2=nco_var_cnf_typ( NC_FLOAT, var2);
        }
        else 
	{    
            var1=nco_var_cnf_typ( NC_DOUBLE, var1);
            var2=nco_var_cnf_typ( NC_DOUBLE, var2);
        }  


        var=nco_var_dpl(var1);      

        if(prs_arg->ntl_scn)
	{                    
          var1=nco_var_free(var1);  
	  var2=nco_var_free(var2);

        }
        else 
	{ // start heavy lifting      

	  if(var1->sz != var2->sz)
          {
            std::string serr;
	    serr="For  this function to succeed Operands must have same number of elements \n"+susg;               
            err_prn(sfnm,serr);
          }  

          (void)cast_void_nctype(var->type,&var->val);      
          (void)cast_void_nctype(var1->type,&var1->val);      
          (void)cast_void_nctype(var2->type,&var2->val);

          // FLOAT      
          if(var1->type == NC_FLOAT)   
	  {
	    nco_bool has_mss;
            long idx;
            long sz;         
            float mss_flt;  
            float *fp;     
            float *fp1;
            float *fp2;
                          
            fp=var->val.fp;
            fp1=var1->val.fp;
            fp2=var2->val.fp;
                             

            has_mss=False;         
            if( var1->has_mss_val)
	    {       
	      has_mss=True; 
              mss_flt=var1->mss_val.fp[0];  

            }   
            else if(var2->has_mss_val)
	    { 
              has_mss=True;       
              mss_flt=var2->mss_val.fp[0];        
              nco_mss_val_cp(var2,var);

            }   

            sz=var1->sz;   
              
            if( has_mss)      
	    {   
              for(idx=0 ; idx<sz ; idx++)   
                if( fp1[idx] == mss_flt || fp2[idx]== mss_flt ) 
                  fp[idx]=mss_flt;
                else       
		  fp[idx] = ( fp1[idx] - fp2[idx] ) /  ( fp1[idx] + fp2[idx] ) ;     

	    }    
            else
	    {  
              for(idx=0 ; idx<sz ; idx++)   
		fp[idx]= ( fp1[idx] - fp2[idx] ) /  ( fp1[idx] + fp2[idx] ) ;     




            }
       

	  }       


          // DOUBLE
          if(var1->type == NC_DOUBLE)   
	  {
	    nco_bool has_mss;
            long idx;
            long sz;         
            double mss_dbl;  
            double *dp;     
            double *dp1;
            double *dp2;
                          
            
            dp=var->val.dp;
            dp1=var1->val.dp;
            dp2=var2->val.dp;
                             

            has_mss=False;         
            if( var1->has_mss_val)
	    {       
	      has_mss=True; 
              mss_dbl=var1->mss_val.dp[0];  

            }   
            else if(var2->has_mss_val)
	    { 
              has_mss=True;       
              mss_dbl=var2->mss_val.dp[0];        
              nco_mss_val_cp(var2,var);
              // var->has_mss_val=True;   
               
            }   

            sz=var1->sz;   
              
            if( has_mss)      
	    {   
              for(idx=0 ; idx<sz ; idx++)   
                if( dp1[idx] == mss_dbl || dp2[idx]== mss_dbl ) 
                  dp[idx]=mss_dbl;
                else       
		  dp[idx] = ( dp1[idx] - dp2[idx] ) /  ( dp1[idx] + dp2[idx] ) ;     

	    }    
            else
	    {  
              for(idx=0 ; idx<sz ; idx++)   
		dp[idx]= ( dp1[idx] - dp2[idx] ) /  ( dp1[idx] + dp2[idx] ) ;     




            }
       

	  }       

            (void)cast_nctype_void(var->type,&var->val);      
            (void)cast_nctype_void(var1->type,&var1->val);      
            (void)cast_nctype_void(var2->type,&var2->val);    
            var1=nco_var_free(var1);  
	    var2=nco_var_free(var2);




	}  
        break;


       case PSOLARZENITHANGLE:
	 { 
           // convert all args to type double              
           var1=nco_var_cnf_typ( NC_FLOAT, var1);
           var2=nco_var_cnf_typ( NC_FLOAT, var2);

           var=nco_var_dpl(var1);

           if(!prs_arg->ntl_scn)       
	   {    
             long idx;
             long sz; 
             float *z_fp; 
	     float *tm_fp;   // array of times 
             float  lat_flt; // latitude in degrees
             float cosSZA;      
             char units_sng[200]={0};             
             char units_new_sng[200]={0};

             //grab units atts for var1->nm
             // fixme:(2017-11-20)   for now not reading calendar type of var1->nm: assume it is cln_std            
             {  

               var_sct *var_att=NULL_CEWI;
               NcapVar *Nvar=NULL;
	       std::string att_nm; 
               tm_cln_sct lcl_tm_sct;      

	       att_nm=std::string(var1->nm)+ "@units";       
	       Nvar=prs_arg->var_vtr.find(att_nm);

	       if(Nvar !=NULL)
		 var_att=nco_var_dpl(Nvar->var);
	       else    
		 var_att=ncap_att_init(att_nm,prs_arg);

	       if(var_att == NULL_CEWI )
		 err_prn(fnc_nm,"Unable to locate attribute " +att_nm+ " in input or output files.");
	       if(var_att->type != NC_CHAR && var_att->type != NC_STRING)
		 err_prn(fnc_nm,"The NC type for "+ att_nm+ " must be NC_CHAR or NC_STRING");

	       (void)cast_void_nctype(var_att->type,&var_att->val); 
  
	       // copy string - no need for NULL as units_sng is all nulls     
               if( var_att->type == NC_CHAR) 
	          strncpy(units_sng,var_att->val.cp,var_att->sz); 
               else if(var_att->type==NC_STRING)
		  strcpy(units_sng, var_att->val.sngp[0]);   
        
	       (void)cast_nctype_void(var_att->type,&var_att->val);    
	       var_att=nco_var_free(var_att); 

               // we want the time coord to be "days since start of year" 
               // need to parse the date string to extract the year
               if( nco_cln_prs_tm(units_sng, &lcl_tm_sct) != NCO_NOERR)              
                 err_prn(fnc_nm, "Error trying to parse the units string " + std::string(units_sng) + " \n"); 

               // assemble new units string
               sprintf(units_new_sng,"days since %d-01-01 00:00:00", lcl_tm_sct.year);           
 
             }   
                 
             // convert var1 from units_sng  units_new_sng that is calendar_day_of_year; 
             // nb calendar is cln_std for now       
             (void)nco_cln_clc_dbl_var_dff(units_sng,units_new_sng, cln_std, (double*)NULL, var1); 

             (void)cast_void_nctype(var->type,&var->val);      
             (void)cast_void_nctype(var1->type,&var1->val);      
             (void)cast_void_nctype(var2->type,&var2->val);

	  
             z_fp=var->val.fp;    
             tm_fp=var1->val.fp;    

             lat_flt=var2->val.fp[0];
        
             // convert lat_dbl to radians    
             lat_flt *=  (float)M_PI /180.0f;      

             sz=var1->sz;
    
             for(idx=0;idx<sz;idx++)
	     {                          
               cosSZA=-1;;
	       (void)solar_geometry( lat_flt, tm_fp[idx], 1, (float *)NULL, &cosSZA, (float *)NULL);
               // convert back to degree's 
               z_fp[idx]=180.0f/ M_PI *  acos( cosSZA);                      

             } 


            (void)cast_nctype_void(var->type,&var->val);      
            (void)cast_nctype_void(var1->type,&var1->val);      
            (void)cast_nctype_void(var2->type,&var2->val);      



           }  
           var1=nco_var_free(var1);   
           var2=nco_var_free(var2);   
    
         } 
         break; 


    }
      
    return var; 

  }



void mth2_cls::solar_geometry(float latitude_rad, float calendar_day_of_year, int num_long, float *local_time, float *cosSZA, float *eccentricity_factor)
{

  float cos_lat;
  float cos_delta;
  float cphase;
  float delta;
  float phi;
  float sin_lat;
  float sin_delta;
  float theta;
  float eccentricity;

  int ilong;

  int days_per_year=365;

  /* compute eccentricity factor (sun-earth distance factor) */
  theta=2.0*M_PI*calendar_day_of_year/days_per_year;
  eccentricity=1.000110+.034221*cos(theta)+0.001280*sin(theta)+ 0.000719*cos(2.*theta)+0.000077*sin(2.*theta);

  /* solar declination in radians: */
  delta=0.006918-0.399912*cos(theta)+0.070257*sin(theta)- 0.006758*cos(2.*theta)+0.000907*sin(2.*theta)- 0.002697*cos(3.*theta)+0.001480*sin(3.*theta);

  /* compute local cosine solar zenith angle: */
  sin_lat=sin(latitude_rad);
  sin_delta=sin(delta);
  cos_lat=cos(latitude_rad);
  cos_delta=cos(delta);

  /* calendar_day_of_year is the calender day for greenwich, including fraction
     of day; the fraction of the day represents a local time at
     greenwich; to adjust this to produce a true instantaneous time
     for other longitudes, we must correct for the local time change: */

  for(ilong=1;ilong<=num_long;ilong++)
  {
    phi=calendar_day_of_year+((float)(ilong-1)/(float)(num_long));
    cphase=cos(2.0*M_PI*phi);

    if( cosSZA != NULL)
      cosSZA[ilong-1]=sin_lat*sin_delta-cos_lat*cos_delta*cphase;

    if( local_time != NULL)   
      local_time[ilong-1]=12.0*acos(cphase)/M_PI;

    if(eccentricity_factor != NULL)
      eccentricity_factor[ilong-1]=eccentricity;
  }

  return;
}



//PDQ Functions /******************************************/
  pdq_cls::pdq_cls(bool){
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

            if((tr=fargs->getFirstChild())) {
              do  
	       vtr_args.push_back(tr);
              while((tr=tr->getNextSibling()));    
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

              // Check location of record dimension if not a netcdf4 file
              if( walker.prs_arg->fl_out_fmt != NC_FORMAT_NETCDF4){
                for(idx=0 ; idx<nbr_dim ; idx++){
		  if( idx>0 && dmn_vtr[idx]->is_rec_dmn){
                    ostringstream os; 
		    os<<"The record dimension \""<< dmn_vtr[idx]->nm <<"\" must be the first dimension in the list."; 
                    err_prn(fnc_nm,os.str());  
	          }
                }
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
                  cst_vtr.push_back( SCS((dmn_vtr[idx]->nm)));

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

  msk_cls::msk_cls(bool){
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

    if((tr=fargs->getFirstChild())) {
       do  
         vtr_args.push_back(tr);
       while((tr=tr->getNextSibling()));    
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

  pck_cls::pck_cls(bool){
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
    var_sct *var_scl_fct=NULL_CEWI;
    var_sct *var_add_fst=NULL_CEWI; 
    nc_type typ;
    nco_bool PCK_VAR_WITH_NEW_PCK_ATT;
    
    std::string susg;
    std::string serr; 
    std::string sfnm=fmc_obj.fnm();
    RefAST tr;
    std::vector<RefAST> vtr_args; 

    susg="usage: var_out="+sfnm+"(var_in, var_scale_factor?, var_offset?)";


    if(expr)
      vtr_args.push_back(expr);

    if((tr=fargs->getFirstChild())) {
       do  
         vtr_args.push_back(tr);
       while((tr=tr->getNextSibling()));    
    } 
      
    nbr_args=vtr_args.size();  


    if(nbr_args==0 || nbr_args==2)
      err_prn(sfnm,"Function has been called with incorrect number of arguments\n"+susg);
      
    var_in=walker.out(vtr_args[0]); 

    if(nbr_args >=3)
    {  
      var_scl_fct=walker.out(vtr_args[1]);
      var_add_fst=walker.out(vtr_args[2]);
    }

      

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
	  if(var_scl_fct) var_scl_fct=nco_var_free(var_scl_fct);
	  if(var_add_fst) var_add_fst=nco_var_free(var_add_fst); 
	     
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
          if(nbr_args>=3)
	  {     
	    var_add_fst=nco_var_cnf_typ(var_in->type,var_add_fst);
	    var_in->has_add_fst=True;
	    var_in->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var_in->type));
	    (void)memcpy(var_in->add_fst.vp,var_add_fst->val.vp,nco_typ_lng(var_in->type));
	    var_add_fst=nco_var_free(var_add_fst);
	    
	    var_scl_fct=nco_var_cnf_typ(var_in->type,var_scl_fct);
	    var_in->has_scl_fct=True;
	    var_in->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var_in->type));
	    (void)memcpy(var_in->scl_fct.vp,var_scl_fct->val.vp,nco_typ_lng(var_in->type));	    
	    var_scl_fct=nco_var_free(var_scl_fct);

            PCK_VAR_WITH_NEW_PCK_ATT=True;   
	  }  
          else
	   {  
	      PCK_VAR_WITH_NEW_PCK_ATT=False;    
              var_in->has_add_fst=False;     
	      var_in->has_scl_fct=False;

	      if(var_in->scl_fct.vp)  
                  var_in->scl_fct.vp=nco_free(var_in->scl_fct.vp);
                
	      if(var_in->add_fst.vp)
                  var_in->add_fst.vp=nco_free(var_in->add_fst.vp);
 	      
	   }      
	    
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
 
srt_cls::srt_cls(bool){
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
  
  if((tr=fargs->getFirstChild())) {
    do  
      vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));    
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
    // 20161205: Always return value to non-void functions: good practice and required by rpmlint
  default:
    return NULL;
    break;
  }

} // end gsl_fit_cls::fnd 
  
var_sct * srt_cls::imap_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("srt_cls::imap_fnd");
    int nbr_args;
    // int fdx=fmc_obj.fdx();
    var_sct *var1=NULL_CEWI;
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
       unsigned long idx;
       unsigned long sz; 
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




  
var_sct * srt_cls::srt_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
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
        if(args_vtr[1]->getType() != CALL_REF) 
          err_prn(sfnm," second argument must be a call by reference variable\n"+susg);   

	var_nm=args_vtr[1]->getFirstChild()->getText(); 
	bdef=(prs_arg->ncap_var_init_chk(var_nm)>0);
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

          if( (bdef && prs_arg->ntl_scn) || (bdef && !prs_arg->ntl_scn && Nvar->flg_stt==2) ){
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
           case NC_STRING: 
	     (void)ncap_sort_and_map<nco_string>(var1,var2,bdirection);    
            break;
  
           default: nco_dfl_case_nc_type_err(); break;
            
	} // end big switch
 
         // Write out mapping
         (void)prs_arg->ncap_var_write(var2,false);             
      }

      return var1;
    

} // end srt_cls::srt_fnd()


  
var_sct * srt_cls::mst_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
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
          unsigned long idx; 
          unsigned long jdx;
          unsigned long sz; 
          unsigned long sz_idx;
          unsigned long slb_sz;
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
	      if(lp_mp[jdx]< sz )
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
	      if(lp_mp[jdx]< sz )
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


// MIN & MAX INDEX Functions /***********************************/
  agg_idx_cls::agg_idx_cls(bool){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("min_index" , this,PMIN)); 
          fmc_vtr.push_back( fmc_cls("max_index" , this,PMAX)); 
			     		      
    }
  }


  var_sct *agg_idx_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
    const std::string fnc_nm("agg__idx_cls::fnd");
    int fdx;
    int nbr_args;
    int idx;
    int nbr_dim;
        
    long sz_dim=1L;
    long my_index=0L;
       
    var_sct *var=NULL_CEWI;
    var_sct *var_out=NULL_CEWI;
           
    std::string susg;
    std::string sfnm=fmc_obj.fnm();

    RefAST aRef;
    RefAST tr;
    std::vector<RefAST> vtr_args; 
       
    nc_type mp_typ=NC_INT;
		
            
    // de-reference 
    prs_cls *prs_arg=walker.prs_arg;

    fdx=fmc_obj.fdx();
 
 
    // Put args into vector 
    if(expr)
      vtr_args.push_back(expr);

    if((tr=fargs->getFirstChild())) {
      do  
	vtr_args.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
    nbr_args=vtr_args.size();  

    susg="usage: var_out="+sfnm+"(var_in)";

    if(nbr_args==0)
      err_prn(sfnm, " Function has been called with no arguments\n"+susg); 


    var=walker.out(vtr_args[0]);
		
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



    

	if(prs_arg->ntl_scn)
	{		
	   if(var->undefined )
		{
		  var_out=ncap_var_udf("~dot_methods");
		}   		
		else
		{	
       // create empty var to return
       var_out=ncap_sclr_var_mk(SCS("~zz@value_list"),mp_typ,false);          
	    var_out->sz=var->nbr_dim;
		}
       nco_var_free(var);
	   return var_out; 
	}
		
    nbr_dim=var->nbr_dim;       
	// create return attribute/var	
	var_out=ncap_sclr_var_mk(SCS("~zz@value_list"),NC_UINT64,true);          
    var_out->has_dpl_dmn=-1;
 
	ncap_att_stretch(var_out, nbr_dim); 
    cast_void_nctype(NC_UINT64,&var_out->val); 

    cast_void_nctype(var->type,&var->val);  
    // now do heavy lifting 
    
    if(fdx==PMIN)
    {    
    
      switch (var->type) {
          case NC_DOUBLE: 
            my_index=ncap_min_index<double>(var);    
            break;  
          case NC_FLOAT: 
            my_index=ncap_min_index<float>(var);    
            break;  
          case NC_INT: 
            my_index=ncap_min_index<nco_int>(var);    
            break;  
          case NC_SHORT: 
            my_index=ncap_min_index<nco_short>(var);    
            break;  
          case NC_USHORT: 
            my_index=ncap_min_index<nco_ushort>(var);    
            break;  
          case NC_UINT: 
            my_index=ncap_min_index<nco_uint>(var);    
            break;  
          case NC_INT64: 
            my_index=ncap_min_index<nco_int64>(var);    
            break;  
          case NC_UINT64: 
            my_index=ncap_min_index<nco_uint64>(var);    
            break;  
          case NC_BYTE: 
            my_index=ncap_min_index<nco_byte>(var);    
            break;  
          case NC_UBYTE: 
            my_index=ncap_min_index<nco_ubyte>(var);    
            break;  
          case NC_CHAR: 
            my_index=ncap_min_index<nco_char>(var);    
            break;  
           case NC_STRING: break; /* Do nothing */
             
           default: nco_dfl_case_nc_type_err(); break;
            
        } // end big switch

    
    }
    
    
    if(fdx==PMAX)
    {    
      switch (var->type) {
          case NC_DOUBLE: 
            my_index=ncap_max_index<double>(var);    
            break;  
          case NC_FLOAT: 
            my_index=ncap_max_index<float>(var);    
            break;  
          case NC_INT: 
            my_index=ncap_max_index<nco_int>(var);    
            break;  
          case NC_SHORT: 
            my_index=ncap_max_index<nco_short>(var);    
            break;  
          case NC_USHORT: 
            my_index=ncap_max_index<nco_ushort>(var);    
            break;  
          case NC_UINT: 
            my_index=ncap_max_index<nco_uint>(var);    
            break;  
          case NC_INT64: 
            my_index=ncap_max_index<nco_int64>(var);    
            break;  
          case NC_UINT64: 
            my_index=ncap_max_index<nco_uint64>(var);    
            break;  
          case NC_BYTE: 
            my_index=ncap_max_index<nco_byte>(var);    
            break;  
          case NC_UBYTE: 
            my_index=ncap_max_index<nco_ubyte>(var);    
            break;  
          case NC_CHAR: 
            my_index=ncap_max_index<nco_char>(var);    
            break;  
           case NC_STRING: break; /* Do nothing */
             
           default: nco_dfl_case_nc_type_err(); break;
            
        } // end big switch
    
    }
    
    
    
   sz_dim=1L;  
   // convert my_index into multiple indices  
   for(idx=0;idx<nbr_dim;idx++)
      sz_dim*= var->cnt[idx]; 

   for(idx=0; idx<nbr_dim; idx++){                   
      sz_dim/=var->cnt[idx];
      var_out->val.ui64p[idx]=my_index/sz_dim; 
      my_index-=var_out->val.ui64p[idx]*sz_dim;
    }

    cast_nctype_void(NC_UINT64,&var_out->val);

    if(var_out->type != mp_typ) 
         nco_var_cnf_typ(mp_typ,var_out); 

     nco_var_free(var);
    
    
	    
	return var_out;

  }



//Unary Functions /***********************************/
 
  unr_cls::unr_cls(bool){
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

    if((tr=fargs->getFirstChild())) {
      do  
	args_vtr.push_back(tr);
      while((tr=tr->getNextSibling()));    
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
  arr_cls::arr_cls(bool){
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
  var_sct *var1=NULL_CEWI;
  var_sct *var2=NULL_CEWI;
  var_sct *var_ret;
           
  std::string susg;
  std::string sfnm=fmc_obj.fnm();

  RefAST tr;
  RefAST aRef;;
  std::vector<RefAST> args_vtr; 
  std::vector<std::string> cst_vtr;              

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;            

  fdx=fmc_obj.fdx();
 

  if(expr)
      args_vtr.push_back(expr);

  if((tr=fargs->getFirstChild())) {
      do  
	args_vtr.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
  nbr_args=args_vtr.size();  

  susg="usage: var_out="+sfnm+"(start_exp,inc_exp,$dim|dim_list|var)"; 

  
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

  }
  else if( args_vtr[2]->getType()==DMN_ARG_LIST)
  {
    aRef=args_vtr[2]->getFirstChild();
    while(aRef) 
    {           
      cst_vtr.push_back(aRef->getText());    
      aRef=aRef->getNextSibling();  
    } 

    var_ret=ncap_cst_mk(cst_vtr,prs_arg);   
    // convert to type of first arg
    var_ret=nco_var_cnf_typ(var1->type,var_ret);  

  }
  else
  {
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
    // e.g.,  var_out=array(1,-3, $time);  
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

// Bounds function - calculates bounds of a 1D coordinate variable
  bnds_cls::bnds_cls(bool){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("make_bounds",this,PBOUNDS)); 

    }		      
  }

  var_sct * bnds_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("arr_cls::fnd");
  // int fdx=fmc_obj.fdx();
  int nbr_args;
  int idx;
  nc_type itype;
  var_sct *var1=NULL_CEWI;
  var_sct *var_txt=NULL_CEWI;
  var_sct *var_ret=NULL_CEWI;
           
  std::string susg;
  std::string sfnm=fmc_obj.fnm();

  RefAST tr;
  std::vector<RefAST> args_vtr; 
  std::vector<std::string> cst_vtr;              

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;            

  
 

  if(expr)
      args_vtr.push_back(expr);

  if((tr=fargs->getFirstChild())) 
   {
      do  
	args_vtr.push_back(tr);
      while((tr=tr->getNextSibling()));    
   } 
      
  nbr_args=args_vtr.size();  

  susg="usage: var_out="+sfnm+"(coordinate_var,$dim, string_nm?)"; 

  
  if(nbr_args<2)
      err_prn(sfnm,"Function has been called with less than two arguments\n"+susg); 



  if(nbr_args >4 &&!prs_arg->ntl_scn) 
      wrn_prn(sfnm,"Function been called with more than three arguments"); 


   
  
  var1=walker.out(args_vtr[0]);  
  if(nbr_args >=3)
    var_txt=walker.out(args_vtr[2]); 


  /* second argument must be a single dimension */
  if(args_vtr[1]->getType() != DIM_ID ) 
     err_prn(sfnm,"Second argument must be a dimension\n"+susg); 

  // cast a var from using the dim arg
  if(var1->undefined==False &&  args_vtr[1]->getType() == DIM_ID   ) 
  {
    for(idx=0;idx<var1->nbr_dim;idx++)    
      cst_vtr.push_back( var1->dim[idx]->nm);     

   cst_vtr.push_back(args_vtr[1]->getText());
     
   var_ret=ncap_cst_mk(cst_vtr,prs_arg);

   //  This a real hack - we use a random name for the var as we wish to control
   //  the attribute propagation. If we used the coordinate var name then the RHS would
   //  get all the atts from coord name we only wish to get units, cal, and a custom
   nco_free(var_ret->nm);
   var_ret->nm=(char*)nco_calloc(40, sizeof(char));
   sprintf(var_ret->nm, "xyz%d", rand() );

   // convert to type of first arg
   var_ret=nco_var_cnf_typ(var1->type,var_ret);  

  }


  if(prs_arg->ntl_scn)
  {
    if(var_txt) 
      nco_var_free(var_txt);    

    if(var1->undefined) 
      return var1; 
    else 
    {     
      var1=nco_var_free(var1);
      return var_ret;    
    }
  }



  /* Remember this puts @bounds into var1 - the input coordinate var */
  if(var_txt)
  {
    string att_nm;
    NcapVar *Nvar=NULL;

    if(var_txt->type !=NC_CHAR && var_txt->type !=NC_STRING)     
      wrn_prn(fnc_nm,"Third argument is the bounds name and must be a  text type");  
    else
    {
      att_nm=var1->nm;att_nm+="@bounds";
      Nvar=new NcapVar(var_txt,att_nm);
      prs_arg->var_vtr.push_ow(Nvar);       
    }       

  }


  /* grab if they exist @units and @calendar from input var ( coordinate var) */
  {

    char bnds_txt[100];
    nco_char ch=nco_char_CEWI;

    var_sct *att_units=NULL_CEWI;
    var_sct *att_cal=NULL_CEWI;
    var_sct *att_long_name=NULL_CEWI;

    
    std::string att_nm_in;
    std::string att_nm_out;
    NcapVar *Nvar=NULL;

    /* get @units from coordinate var */
    att_nm_in=var1->nm; att_nm_in+="@units";

    Nvar=prs_arg->var_vtr.find(att_nm_in);

    if(Nvar !=NULL)
      att_units=nco_var_dpl(Nvar->var);
    else
      att_units=ncap_att_init(att_nm_in,prs_arg);

    if(att_units)
    {
      att_nm_out=var_ret->nm; att_nm_out+="@units";
      Nvar=new NcapVar(att_units,att_nm_out);
      prs_arg->var_vtr.push_ow(Nvar);


    }


    /* get @calendar from coordinate var */
    att_nm_in=var1->nm; att_nm_in+="@calendar";

    Nvar=prs_arg->var_vtr.find(att_nm_in);

    if(Nvar !=NULL)
      att_cal=nco_var_dpl(Nvar->var);
    else
      att_cal=ncap_att_init(att_nm_in,prs_arg);

    if(att_cal)
    {
      att_nm_out=var_ret->nm; att_nm_out+="@calendar";
      Nvar=new NcapVar(att_cal,att_nm_out);
      prs_arg->var_vtr.push_ow(Nvar);


    }

    /* create a new @long_name */
    att_long_name=ncap_sclr_var_mk("~zz@tmp", ch);

    sprintf(bnds_txt,"Bounds for $%s coordinate",var1->nm  );
    att_long_name->sz=strlen(bnds_txt);

    att_long_name->val.cp=(char*)nco_malloc(sizeof(char)*att_long_name->sz  );
    strncpy(att_long_name->val.cp, bnds_txt, att_long_name->sz );
    cast_nctype_void(att_long_name->type,&att_long_name->val);

    att_nm_out=var_ret->nm; att_nm_out+="@long_name";
    Nvar=new NcapVar(att_long_name,att_nm_out);
    prs_arg->var_vtr.push_ow(Nvar);


  }


  // if type less than float then promote to double do calculation the convert back to original type
  // else if float then leave as is 
  itype=var1->type;   

  if( itype !=NC_FLOAT && itype !=NC_DOUBLE) 
  {  
    nco_var_cnf_typ(NC_DOUBLE, var1);
    nco_var_cnf_typ(NC_DOUBLE, var_ret);
  }  

  
  (void)cast_void_nctype(var1->type,&var1->val);
  (void)cast_void_nctype(var_ret->type,&var_ret->val);


   // allocate space for results 
  void *vp=nco_malloc( var_ret->sz * nco_typ_lng(var_ret->type)); 
  var_ret->val.vp=vp;


 
  // do the heavy lifting 
  if(var1->type==NC_DOUBLE)
  {  
    long sz; 
    double *idp;
    double *rdp;
    double dprev;


    // reduce indrection 
    idp=var1->val.dp;
    rdp=var_ret->val.dp;
    
    sz=var1->sz;

    dprev=2* idp[0]-idp[1];     
    for(idx=0;idx<sz;idx++)  
    {
      rdp[2*idx]= 0.5*(dprev+idp[idx]);                                                                                   
      if(idx>0)  
	rdp[2*idx-1] = rdp[2*idx];

      dprev=idp[idx]; 
    }
    // calculate final value 
    rdp[2*sz-1]= 2*dprev-rdp[2*sz-2]; 


  }
  else if(var1->type==NC_FLOAT)
  {  
    long sz; 
    float *ifp;
    float *rfp;
    float fprev;

    ifp=var1->val.fp;
    rfp=var_ret->val.fp;
    
    sz=var1->sz;

    fprev=2* ifp[0]-ifp[1];     
    for(idx=0;idx<sz;idx++)  
    {
      rfp[2*idx]= 0.5*(fprev+ifp[idx]);                                                                                   
      if(idx>0)  
	rfp[2*idx-1] = rfp[2*idx];

      fprev=ifp[idx]; 
    }
    // calculate final value 
    rfp[2*sz-1]= 2*fprev-rfp[2*sz-2]; 

  }

  (void)cast_nctype_void(var1->type,&var1->val);
  (void)cast_nctype_void(var_ret->type,&var_ret->val);
  
  // convert results back to orignal type
  nco_var_cnf_typ(itype,var_ret);  
  nco_var_free(var1);

  return var_ret;

  }


//Bilinear  Interpolation Functions /****************************************/
bil_cls::bil_cls(bool){
  //Populate only on  constructor call
  if(fmc_vtr.empty()){
    fmc_vtr.push_back( fmc_cls("bilinear_interp",this,PBIL_ALL)); 
    fmc_vtr.push_back( fmc_cls("bilinear_interp_wrap",this,PBIL_ALL_WRP)); 
    
  }		      
} 

var_sct * bil_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("bil_cls::fnd");
  bool bwrp;  //if tue then wrap X and Y coo-ordinates in grid

  int fdx;
  int nbr_args;
  int in_nbr_args=0;
  int idx;
  var_sct *var_arr[6];
  
  nc_type in_typ;           
  
  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::string serr;
  
  RefAST tr;
  std::vector<RefAST> vtr_args; 
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;            
  
  fdx=fmc_obj.fdx();
  bwrp=false;
  
  if(expr)
    vtr_args.push_back(expr);
  
  if((tr=fargs->getFirstChild())) {
    do  
      vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));    
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
      if(var_arr[1]->nbr_dim < 2)
	err_prn(sfnm,"Output data variable "+std::string(var_arr[1]->nm) + " must have at least two dimensions ");

      // get output co-ordinate vars   
      if(in_nbr_args < 3) 
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

    // 20161205: Always return value to non-void functions: good practice and required by rpmlint
    return NULL;

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

  // Sanity check for input/ooooutput data
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
     if(y_min==v_yin->sz || ( y_min==0L && v_yout->val.dp[jdx]< v_yin->val.dp[0] ) )   
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
       if(x_min==v_xin->sz || ( x_min==0L && v_xout->val.dp[kdx]< v_xin->val.dp[0] ))   
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
        
       //deal with no missing values in v_din
       if( !v_din->has_mss_val ) {   
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
       // missing values  
       }else{ 
	 cast_void_nctype(NC_DOUBLE,&v_din->mss_val);
         double mss_dbl=*v_din->mss_val.dp;      
         cast_nctype_void(NC_DOUBLE,&v_din->mss_val);      
                 
	 bool mQ00=Q[0][0]==mss_dbl ? true : false;
         bool mQ10=Q[1][0]==mss_dbl ? true : false;
         bool mQ01=Q[0][1]==mss_dbl ? true : false;
         bool mQ11=Q[1][1]==mss_dbl ? true : false;  

           // nb its possible with below for d_int1 to be assigned missing value   
           if(mQ00)
             d_int1=Q[1][0];
           else if(mQ10)
             d_int1=Q[0][0]; 
           else
	     d_int1=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][0],Q[1][0]);               

           // nb its possible with below for d_int2 to be assigned missing value  
           if(mQ01)
             d_int2=Q[1][1];
           else if(mQ11)
             d_int2=Q[0][1]; 
           else
             d_int2=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][1],Q[1][1]);         

           
           // nb its possible with below for rslt to be assigned missing value   
           if(d_int1==mss_dbl)
	     rslt=d_int2;
           else if(d_int2==mss_dbl)
             rslt=d_int1;
           else  
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

       // no missing value
       if(!v_din->has_mss_val)  
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
       }else{ 
	 // missing value  
	 cast_void_nctype(NC_DOUBLE,&v_din->mss_val);
         double mss_dbl=*v_din->mss_val.dp;      
         cast_nctype_void(NC_DOUBLE,&v_din->mss_val);      
                 
	 bool mQ00=Q[0][0]==mss_dbl ? true : false;
         bool mQ10=Q[1][0]==mss_dbl ? true : false;
         bool mQ01=Q[0][1]==mss_dbl ? true : false;
         bool mQ11=Q[1][1]==mss_dbl ? true : false;  

           // nb its possible with below for d_int1 to be assigned missing value   
           if(mQ00)
             d_int1=Q[1][0];
           else if(mQ10)
             d_int1=Q[0][0]; 
           else
	     d_int1=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][0],Q[1][0]);               

           // nb its possible with below for d_int2 to be assigned missing value  
           if(mQ01)
             d_int2=Q[1][1];
           else if(mQ11)
             d_int2=Q[0][1]; 
           else
             d_int2=clc_lin_ipl(v_xin->val.dp[x_min],v_xin->val.dp[x_max],v_xout->val.dp[kdx],Q[0][1],Q[1][1]);         

           
           // nb its possible with below for rslt to be assigned missing value   
           if(d_int1==mss_dbl)
	     rslt=d_int2;
           else if(d_int2==mss_dbl)
             rslt=d_int1;
           else  
             rslt=clc_lin_ipl(v_yin->val.dp[y_min],v_yin->val.dp[y_max],v_yout->val.dp[jdx],d_int1,d_int2);                  
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
  cod_cls::cod_cls(bool){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("min_coords",this,PCOORD)); 

    }		      
  } 

  var_sct * cod_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("cod_cls::fnd");
  // int fdx=fmc_obj.fdx();
  int nbr_args;
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


 

  if(expr)
      args_vtr.push_back(expr);

  if((tr=fargs->getFirstChild())) {
      do  
	args_vtr.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
  nbr_args=args_vtr.size();  

  // function takes a coordinate var and value and returns the index of the nearest 
  // grid point. returns -1 if value not in range
  susg="usage: crd_idx="+sfnm+"(crd_var,crd_value)"; 

  
  if(nbr_args<2)
      err_prn(sfnm,"Function has been called with less than two arguments\n"+susg); 



  if(nbr_args >2 &&!prs_arg->ntl_scn) 
      wrn_prn(sfnm,"Function been called with more than two arguments"); 

   
  var1=walker.out(args_vtr[0]);  
  var2=walker.out(args_vtr[1]);  

  
  var_ret=nco_var_dpl(var2);     
  
  if(!var_ret->undefined)
      var_ret=nco_var_cnf_typ(NC_INT,var_ret);
  
  if(prs_arg->ntl_scn  )
  {
    nco_var_free(var1);
    nco_var_free(var2);
    return var_ret;
  }

  // do heavy lifting 
  {
    bool bInc;
    long idx;
    long jdx;
    long c_sz;
    long r_sz;  
    nco_int *ip;
    double dval;
    double *dp_crd; 
    
    var1=nco_var_cnf_typ(NC_DOUBLE,var1);
    var2=nco_var_cnf_typ(NC_DOUBLE,var2);

    // convert everything to type double         
    (void)cast_void_nctype(NC_DOUBLE,&(var1->val));
    (void)cast_void_nctype(NC_DOUBLE,&(var2->val));
    (void)cast_void_nctype(NC_INT,&(var_ret->val));

    r_sz=var_ret->sz;
    ip=var_ret->val.ip;  

    c_sz=var1->sz;  
    dp_crd=var1->val.dp;  
       
    // dval=var2->val.dp[0];   

    
    // determine if co-ord is montonic increasing or decreasing 
    // true if increasing 
    bInc= ( dp_crd[1] > dp_crd[0]); 
        
    // check limits co-ord increasing 
    if(bInc){
      for(idx=0;idx<r_sz;idx++){ 
	    dval=var2->val.dp[idx];
         /* default set to out of range */   
        ip[idx]=-1;
        if(dval>=dp_crd[0] && dval<=dp_crd[c_sz-1] )
           for(jdx=0 ; jdx<c_sz-1 ; jdx++)
             if( dval >= dp_crd[jdx] && dval <= dp_crd[jdx+1] ) {
               ip[idx] = (dval - dp_crd[jdx] <= dp_crd[jdx + 1] - dval ? jdx : jdx + 1);
               break;
             }
      }
    }
    // check limits co-ord decreasing
    if(!bInc){
      for(idx=0;idx<r_sz;idx++){  
	    dval=var2->val.dp[idx];
        /* default set to out of range */
        ip[idx]=-1;
        if(dval<=dp_crd[0] && dval>=dp_crd[c_sz-1] )
          for(jdx=0 ; jdx<c_sz-1 ; jdx++)
            if( dval <= dp_crd[jdx] && dval >= dp_crd[jdx+1] )
            {
	          ip[idx]=(dp_crd[jdx]-dval <= dval-dp_crd[jdx+1] ? jdx: jdx+1 );
              break;
	        }
      }  
    }              
    (void)cast_nctype_void(NC_DOUBLE,&var1->val);
    (void)cast_nctype_void(NC_DOUBLE,&var2->val);
    (void)cast_nctype_void(NC_INT,&var_ret->val);
  
   
  }

  nco_var_free(var1);   
  nco_var_free(var2);

  return var_ret;        
  }



//misc Functions /******************************************/
// These fuctions are used to create and apply masks to 2D grid variables
//    mask_out=imask_make(var_in,lat,lon)
//
//    var_in is a 2D var. mask_out is a 0/1 mask of grid points.
//    value is set to 1 if point in var_in is  missing_value but has a neigbour with a non-missing value;
//
//    var_out=imask_fill(var_in.mask_out,lat,lon)
//  
//    Points  in var_in are filled if they are missing AND are specified in mask_out.
//    The fill value is an average from the nearest neigbours. nb if a neighbour has just been filled 
//    in then this value is NOT used in the calculation of the average      
//    Typically mask_out and var_in should be the same shape. 
//    Howver its possible to specify multiple mask in mask_out - in this case each mask is iterativly applied to var_in
//    so the shape would be like mask_out(mask_size,lat,lon) 
// 
//
//
  misc_cls::misc_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("imask",this,(int)PMISC1));
      fmc_vtr.push_back( fmc_cls("imask_make",this,(int)PMISC2));
      fmc_vtr.push_back( fmc_cls("imask_fill",this,(int)PMISC3));
    }
  }




  var_sct *misc_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("misc_cls::fnd");
    int idx;
    int fdx=fmc_obj.fdx();   //index
    int nbr_args;
    int in_nbr_args=0;
    prs_cls* prs_arg=walker.prs_arg;
    var_sct *var_arr[4]={NULL_CEWI};
    var_sct *var_out=NULL_CEWI;
    RefAST tr;
    nc_type in_typ;  
    std::string susg;
    std::string serr;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::vector<RefAST> vtr_args; 
    NcapVector<dmn_sct*> dmn_vtr;


    if(expr)
      vtr_args.push_back(expr);

    if((tr=fargs->getFirstChild())) {
       do  
         vtr_args.push_back(tr);
       while((tr=tr->getNextSibling()));    
    } 

      
    nbr_args=vtr_args.size();  

    switch(fdx){

      case PMISC1:
       in_nbr_args=nbr_args;  
       susg="usage: var_out="+sfnm+"(Data_2D, X_in, Y_in)"; 
       break;

      case PMISC2: 
        in_nbr_args=nbr_args;  
        susg="usage: var_out="+sfnm+"(data_2D, mask,X_in?, Y_in?)"; 
        break;


      case PMISC3:
       in_nbr_args=nbr_args;  
       susg="usage: var_out="+sfnm+"(data_2D,mask, X_in, Y_in)"; 
       break;


    } // end switch


    if(in_nbr_args <3 ){   
      serr="function requires at least two arguments. You have only supplied "+nbr2sng(in_nbr_args)+ " arguments\n"; 
      err_prn(sfnm,serr+susg);
    }

    
   
    // process input args 
    for(idx=0 ; idx<in_nbr_args; idx++)
      var_arr[idx]=walker.out(vtr_args[idx]);

   
    // initial scan
    if(prs_arg->ntl_scn){
        for(idx=1 ; idx<in_nbr_args ; idx++)
	    nco_var_free(var_arr[idx]);

        return var_arr[0];
    }

    // save inital type
    in_typ=var_arr[0]->type;     
    // convert all args to type DOUBLE 
    for(idx=0;idx<in_nbr_args;idx++)
       var_arr[idx]=nco_var_cnf_typ(NC_DOUBLE,var_arr[idx]);               

   

    if(fdx==PMISC1){
      // recall input arguments in order
      // 0 - input data
      // 1 - input X   co-ordinate var
      // 2 - input Y   co-ordinate var
      long cnt=0l; 
      long ldx; 
      long jdx;
      long v_sz;
      long iidx;
      long jjdx;
      long lldx; 
      long x_sz;
      long y_sz;
      double *dpi;
      double *dpo;
      double mss_dbl=9.999e20;; 

      var_out=nco_var_dpl(var_arr[0]);
      (void)cast_void_nctype(NC_DOUBLE,&var_out->val);   

      for(idx=0;idx<in_nbr_args;idx++) 
         (void)cast_void_nctype(NC_DOUBLE,&var_arr[idx]->val);

      // grab missing value;  
      if( var_arr[0]->has_mss_val ){
         cast_void_nctype(NC_DOUBLE,&var_arr[0]->mss_val);
         mss_dbl=*var_arr[0]->mss_val.dp;      
         cast_nctype_void(NC_DOUBLE,&var_arr[0]->mss_val);       
       }   

      dpi=var_arr[0]->val.dp; 
      dpo=var_out->val.dp;          
      v_sz=var_arr[0]->sz;     
      x_sz=var_arr[1]->sz;   
      y_sz=var_arr[2]->sz;   

      
      
      // set all values to zero      
     memset((void*)dpo,0, sizeof(double) *v_sz);     

     for(idx=2;idx<x_sz-2; idx++)       
       for(jdx=2;jdx<y_sz-2;jdx++ ){
         ldx=idx*y_sz+jdx; 
         // set output to default missing vale; 
         dpo[ldx]=mss_dbl; 
         // mask either 1 or mss_dbl
	 if( dpi[ldx] !=1.0 ){
	   for(iidx=idx-2; iidx<idx+3;iidx++){ 
	     for(jjdx=jdx-2; jjdx<jdx+3;jjdx++){ 
	       lldx=iidx*y_sz+jjdx;
	        if(dpi[lldx]==1.0){
		  dpo[ldx]=1.0;  
                  cnt++; 
                  goto outer; 
	        }             
	     }
           }
	 } 
       outer: ;
       }
       
     printf("misc_cls number of spots processed=%ld\n" ,cnt);
                       
    } // end MISC1

    if(fdx==PMISC2){
      // recall input arguments in order
      // 0 - input var -2D INPUT GRID 
      // 1 - input X   co-ordinate var
      // 2 - input Y   co-ordinate var
      long ldx;  
      long v_sz;
      long x_sz;
      long y_sz;
      long mcnt=0l;   // num missing values count 
      long vmcnt=0l; // num missing processed;   
      double mss_dbl=9.999e20; 
      double *dpi;   // input pointer
      double *dpo;   //output pointer
      // save in type; 
      ostringstream os; 
      
      // var_out is the output mask - same shape as input var 
      var_out=nco_var_dpl(var_arr[0]);         
      (void)cast_void_nctype(NC_DOUBLE,&var_out->val); 

      // cast all vars
      for(idx=0;idx<in_nbr_args;idx++)
        (void)cast_void_nctype(NC_DOUBLE,&var_arr[idx]->val);
        
      v_sz=var_arr[0]->sz;
      x_sz=var_arr[1]->sz;
      y_sz=var_arr[2]->sz;
   
      
      dpo=var_out->val.dp;
      dpi=var_arr[0]->val.dp;     
 
      // grab missing value;  
     if( var_arr[0]->has_mss_val ){
         cast_void_nctype(NC_DOUBLE,&var_arr[0]->mss_val);
         mss_dbl=*var_arr[0]->mss_val.dp;      
         cast_nctype_void(NC_DOUBLE,&var_arr[0]->mss_val);       
       }   
      
      for(ldx=0; ldx<v_sz ; ldx++){
        // set otuput to default -0.0 
        dpo[ldx]=0.0;

        if(dpi[ldx]==mss_dbl ){  
          int cnt=0;  
          int jdx;
          long iX=ldx/y_sz;
          long iY= ldx -iX*y_sz;
          double sum=0.0;
          double Q[4]={mss_dbl,mss_dbl,mss_dbl,mss_dbl};
          mcnt++; 
            
  
          if(iY >0 )    Q[0]=dpi[ldx-1];
          if(iY+1<y_sz) Q[1]=dpi[ldx+1]; 
          if(iX>0 )     Q[2]=dpi[ldx-y_sz];
          if(iX+1 < x_sz) Q[3]=dpi[ldx+y_sz];   
           
          for(jdx=0 ; jdx<4 ;jdx++)
	    if(Q[jdx] !=mss_dbl) {
	      sum+=Q[jdx];              
              cnt++; 
	    }
       
	  if(cnt){ 
	    dpo[ldx]=1.0;   
            vmcnt++;
	  }
           
	} // end if
               
      } // end for

      if(nco_dbg_lvl_get() >= 2){ 
          os<< sfnm<<": num missing="<<mcnt<<+" num valid="<<vmcnt;
          dbg_prn(fnc_nm,os.str());
      } 

    } // end fdx  


    if(fdx==PMISC3){
      // recall input arguments in order
      // 0 - input var -2D INPUT GRID 
      // 1 - input mask 2D GRID (VARMASK these are the places we want to fill a value)
      //     nb this mask may contain multiple masks. So size of mask is a multiple of the
      //     the input size grid i.e mask size = n * v_sz, When its a multiple we apply each mask
      //     i an iteration    
      // 2 - input X   co-ordinate var
      // 3 - input Y   co-ordinate var
      long ldx;  
      long mdx;
      long v_sz;
      long m_sz;
      long x_sz;
      long y_sz;
      double mss_dbl=9.999e20; 
      double *dpi;   // input pointer
      double *dpo;   //output pointer 
      double *dpm;   // mask pointer;  
   
      // save in type; 
      


      var_out=nco_var_dpl(var_arr[0]);         
      (void)cast_void_nctype(NC_DOUBLE,&var_out->val);

      // cast all arguments
      for(idx=0;idx<in_nbr_args;idx++)
        (void)cast_void_nctype(NC_DOUBLE,&var_arr[idx]->val);

      v_sz=var_arr[0]->sz;
      m_sz=var_arr[1]->sz; 
      x_sz=var_arr[2]->sz;
      y_sz=var_arr[3]->sz;
      
    
      assert(v_sz==x_sz*y_sz); 
      // check mask size  
      assert(m_sz % v_sz ==0l);    
     
      dpo=var_out->val.dp;
      dpi=var_arr[0]->val.dp;     
 
 
      // grab missing value;  
      if( var_arr[0]->has_mss_val ){
	cast_void_nctype(NC_DOUBLE,&var_arr[0]->mss_val);
	mss_dbl=*var_arr[0]->mss_val.dp;      
	cast_nctype_void(NC_DOUBLE,&var_arr[0]->mss_val);       
      }   

      for(mdx=0;mdx < m_sz/v_sz ; mdx++){  
        long mcnt=0l;
        long vmcnt=0l;        
	ostringstream os; 
        dpm=var_arr[1]->val.dp+mdx*v_sz;

        // copy new values into input pointer !!
        // dont want dpi and dpo the same
        if(mdx >0)
          memcpy(dpi,dpo, v_sz* sizeof(double));    

	for(ldx=0; ldx<v_sz ; ldx++){
	  //check mask  
	  // if(dpm[ldx]==1.0 && dpi[ldx]==mss_dbl){  
	  // if( dpi[ldx]==mss_dbl) dpo[ldx]=0.0; 
	  // if(dpm[ldx]==1.0 && dpi[ldx]==mss_dbl){     
	  if(dpi[ldx]==mss_dbl && dpm[ldx]==1.0){  
	    int cnt=0;  
	    int jdx;  
	    long iX=ldx/y_sz;
	    long iY= ldx -iX*y_sz;
	    double sum=0.0;
	    double Q[4]={mss_dbl,mss_dbl,mss_dbl,mss_dbl};
	    mcnt++;
       
	    if(iY >0 )    Q[0]=dpi[ldx-1];
	    if(iY+1<x_sz) Q[1]=dpi[ldx+1]; 
	    if(iX>0 )     Q[2]=dpi[ldx-y_sz];
	    if(iX+1 < x_sz) Q[3]=dpi[ldx+y_sz];   
           
	    for(jdx=0 ; jdx<4 ;jdx++){
	      if(Q[jdx] !=mss_dbl) {
		sum+=Q[jdx];              
		cnt++; 
	      }
	    }
	    if(cnt){ 
	      dpo[ldx]=sum/cnt;
	      vmcnt++;  
	    } 

	  } // end if

	} // end for ldx 

        if(nco_dbg_lvl_get() >= 2){ 
	      os<< sfnm<<" mask num="<<mdx<<" : num missing="<<mcnt<<+" num processed="<<vmcnt;
              dbg_prn(fnc_nm,os.str());
	} 
         
      } // end for mdx
    } 

    // cast everything back to void,and free up
    for(idx=0;idx<in_nbr_args;idx++){
      (void)cast_nctype_void(NC_DOUBLE,&var_arr[idx]->val);
      nco_var_free(var_arr[idx]);
    } 

    (void)cast_nctype_void(NC_DOUBLE,&var_out->val);    
    // change back to input type  
    var_out=nco_var_cnf_typ(in_typ,var_out);

    return var_out;
  } // end  



  vlist_cls::vlist_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("join",this,(int)PJOIN));
      fmc_vtr.push_back( fmc_cls("push",this,(int)PPUSH));
      fmc_vtr.push_back( fmc_cls("get_vars_in",this,(int)PGET_VARS_IN));
      fmc_vtr.push_back( fmc_cls("get_vars_out",this,(int)PGET_VARS_OUT));
      fmc_vtr.push_back( fmc_cls("atoi",this,(int)PATOI));
      fmc_vtr.push_back( fmc_cls("atol",this,(int)PATOL));
    }
  }


  var_sct *vlist_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("vlist_cls::fnd");
  int fdx;
  // int nbr_args;

  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::string att_nm;

  RefAST tr;
  std::vector<RefAST> vtr_args; 
  // prs_cls *prs_arg=walker.prs_arg;

  bool is_mtd;  
        
  fdx=fmc_obj.fdx();


  // Put args into vector 
  if(expr)
    vtr_args.push_back(expr);

  if((tr=fargs->getFirstChild()))
    do  
      vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));    
  
  
  is_mtd=(expr ? true: false);

  switch(fdx) 
  {
     case PPUSH:  
       return push_fnd(is_mtd, vtr_args,fmc_obj, walker);   
       break; 

     case PGET_VARS_IN:  
     case PGET_VARS_OUT:  
       return get_fnd(is_mtd, vtr_args,fmc_obj, walker);    
       break;
  
     case PATOI:
     case PATOL:
       return atoi_fnd(is_mtd, vtr_args,fmc_obj, walker);     
       break;

       // 20170317 Patch to build on OpenSUSE provided by Manfred Schwarb https://sourceforge.net/p/nco/bugs/94/
  default:        // PJOIN, PATOL
    return NULL;  // silence rpmlint error: is this correct?
    break;        // E: nco no-return-in-nonvoid-function fmc_all_cls.cc:3780
  }

  }


var_sct *vlist_cls::push_fnd(bool &, std::vector<RefAST> &vtr_args, fmc_cls &fmc_obj, ncoTree &walker)
  {
  const std::string fnc_nm("push_fnd::fnd");
  bool bret;
  // int fdx=fmc_obj.fdx();
  int nbr_args;
  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::string att_nm;

  var_sct *var=NULL_CEWI;
  var_sct *var_att=NULL_CEWI;
  var_sct *var_add=NULL_CEWI;


  RefAST aRef;
  RefAST tr;
  prs_cls *prs_arg=walker.prs_arg;

  NcapVar *Nvar=NULL;
          


  nbr_args=vtr_args.size();  

  susg="usage: att_out="+sfnm+"(att_id, att_nm|var_nm|string)";

  if(nbr_args!=2)
    err_prn(sfnm, " Function has been called with wrong number of arguments arguments\n"+susg); 

  var_add=walker.out(vtr_args[1]);
             


  // deal with call by ref 
  if(vtr_args[0]->getType() == CALL_REF )     
  {   

    bret=false;   
    // final scan starts here   
     att_nm=vtr_args[0]->getFirstChild()->getText();  

    // do nothing on initial scan with call-by ref 
    if(prs_arg->ntl_scn)
    { 
      nco_var_free(var_add);  
      var=ncap_sclr_var_mk(att_nm,(nc_type)NC_INT,false);  
      return var;
    }

    Nvar=prs_arg->var_vtr.find(att_nm);

    if(Nvar !=NULL)
      var_att=nco_var_dpl(Nvar->var);
    else    
      var_att=ncap_att_init(att_nm,prs_arg);
  
    // if new var then write var - end of story   
    if(!var_att) 
    {   
	nco_free(var_add->nm);
	var_add->nm=strdup(att_nm.c_str());      
	Nvar=new NcapVar(var_add,att_nm);

	var=ncap_sclr_var_mk(att_nm,(nco_int)var_add->sz);
	prs_arg->var_vtr.push_ow(Nvar);       

	return var;
    } 

  }
  // deal with regular argument
  else
  {
    bret=true;
    var_att=walker.out(vtr_args[0]);      
  
    if(prs_arg->ntl_scn) 
    {   
     nco_var_free(var_add);
     nco_var_free(var_att);
     var=ncap_var_udf("~zz@join_methods");  
     return var; 
    
    } 
  }

  

  if(!var_att )
     err_prn(sfnm, " first argument has evaluated to null\n"+susg); 
  
  
  if(var_att->type==NC_CHAR && var_add->type !=NC_CHAR ) 
     err_prn(sfnm, "Cannot push to a NC_CHAR attribute a non NC_CHAR type"+susg);           
  

  if(var_att->type == NC_STRING && !(var_add->type == NC_CHAR || var_add->type==NC_STRING) )
     err_prn(sfnm, "Cannot push to a NC_STRING attribute a non-text type "+susg);           



  // deal with this corner case first
  if(var_att->type==NC_STRING && var_add->type==NC_CHAR)
  { 
     
    char buffer[NC_MAX_ATTRS];
    size_t sz_new;
    size_t slb_sz=nco_typ_lng(var_att->type);   
      
    strncpy(buffer,(char*)var_add->val.vp, var_add->sz);            
    buffer[var_add->sz]='\0';
    sz_new=var_att->sz+1;
 
    var_att->val.vp=nco_realloc(var_att->val.vp, sz_new*slb_sz);           
    cast_void_nctype(NC_STRING,&var_att->val);         
       
    var_att->val.sngp[var_att->sz]=strdup(buffer);      
    cast_nctype_void(NC_STRING,&var_att->val);           
    var_att->sz++;
          
   }
   else 
   {
     char *cp_in;
     char *cp_out;
     size_t slb_sz=nco_typ_lng(var_att->type);   
     long sz_new=var_att->sz+var_add->sz;

     nco_var_cnf_typ(var_att->type,var_add);         
     var_att->val.vp=nco_realloc(var_att->val.vp, sz_new*slb_sz);           
     
     cp_in=(char*)var_add->val.vp;
     cp_out=(char*)var_att->val.vp + (size_t)(var_att->sz*slb_sz);    
   
     memcpy(cp_out,cp_in, slb_sz*var_add->sz); 
     var_att->sz=sz_new;  

      // a bit cheeky here - set to null so we DONT have to deep-copy ragged array of strings    
     if(var_add->type==NC_STRING) 
     {
       nco_free(var_add->val.vp);  
       var_add->val.vp=(void*)NULL_CEWI;   
     }
        

   }
  

  nco_var_free(var_add);

  // deal with call by ref 
  // not sure if we need a return value -?  for now return the att size 
  if(bret) 
  {
     return var_att; 
  }
  else
  {
    Nvar=new NcapVar(var_att); 
     var=ncap_sclr_var_mk(SCS("~zz@join_methods"), (nco_int)var_att->sz);
     prs_arg->var_vtr.push_ow(Nvar);       
     return var;  
  }


  }

  var_sct *vlist_cls::get_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker)
  {
  const std::string fnc_nm("push_fnd::fnd");
  int fdx;
  int nbr_args;
  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::string att_nm;

  var_sct *var=NULL_CEWI;
  var_sct *var_regexp=NULL_CEWI;
  
  nc_type vtype;
  prs_cls *prs_arg=walker.prs_arg;

  fdx=fmc_obj.fdx();

  nbr_args=args_vtr.size();  

  susg="usage: att_out="+sfnm+"( (regexp|var_nm)? )";

  if(nbr_args<0)
    err_prn(sfnm, " Function has been called with wrong number of arguments arguments\n"+susg); 


  if(nbr_args)
     var_regexp=walker.out(args_vtr[0]);
  
   
  if(prs_arg->ntl_scn)  
  { 
    if(var_regexp)
      nco_var_free(var_regexp); 
      
    var=ncap_var_udf("~zz@join_methods");  
    return var;
  }
    

  if(var_regexp)
  {
    vtype=var_regexp->type;
    if(vtype !=NC_STRING && vtype != NC_CHAR) 
       err_prn(sfnm,"First argument must be a text type\n" );
    else
      cast_void_nctype(vtype,&var_regexp->val);
  }
  // no regexp or var list
  else
    vtype=NC_NAT;

  
  // local scope
  {   
    int idx;
    int xtr_nbr;
    int nbr_var_fl;
    int fl_id;
    nm_id_sct *xtr_lst=NULL_CEWI; 
    nco_string *val_str=NULL_CEWI;        

    xtr_nbr=0;
    nbr_var_fl=0;
    fl_id=0;   
 
    if(fdx==PGET_VARS_IN)
      fl_id=prs_arg->in_id; 
    else if(fdx==PGET_VARS_OUT)
      fl_id=prs_arg->out_id; 

    (void)nco_inq(fl_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int*)NULL);

    if(vtype==NC_STRING)
    {  
      xtr_nbr=var_regexp->sz;
       xtr_lst=nco_var_lst_mk(fl_id, nbr_var_fl, var_regexp->val.sngp  , False, False, &xtr_nbr);    
    }

    if(vtype==NC_CHAR)
    {
      char *lcp;   
      char buffer[1200];
 
      strncpy(buffer, var_regexp->val.cp, var_regexp->sz);   
      buffer[var_regexp->sz]='\0';
      lcp=&buffer[0];
      xtr_nbr=1;
      xtr_lst=nco_var_lst_mk(fl_id, nbr_var_fl, &lcp  , False, False,&xtr_nbr);       
    }
   
    // extract all vars   
    if(vtype==NC_NAT) 
    {
      xtr_nbr=0; 
      xtr_lst=nco_var_lst_mk(fl_id, nbr_var_fl, (char**)NULL  , False, False, &xtr_nbr);    
    }

    var=ncap_sclr_var_mk(SCS("~zz@string"), NC_STRING,false );  



    if(nco_dbg_lvl_get() >= nco_dbg_scl)     
          wrn_prn(sfnm,"nbr_var_fl="+nbr2sng(nbr_var_fl)+" xtr_nbr="+nbr2sng(xtr_nbr));

    if(xtr_nbr==0) 
    {   
      wrn_prn(sfnm,"this function has returned an empty variable list");
      var->sz=0;  
    }
    else
    {   
      val_str=(nco_string*)nco_malloc( xtr_nbr* nco_typ_lng(NC_STRING));
      var->sz=xtr_nbr;
    
      for(idx=0; idx<xtr_nbr;idx++)
        val_str[idx]=strdup(xtr_lst[idx].nm); 
      
      cast_void_nctype(NC_STRING, &var->val);        
      var->val.sngp=val_str;
      cast_nctype_void(NC_STRING, &var->val);        
   

    }

    xtr_lst=nco_nm_id_lst_free(xtr_lst,xtr_nbr);

  }
  
  if(var_regexp)
  {
    cast_nctype_void(vtype,&var_regexp->val);
    nco_var_free(var_regexp);
  }



  return var;

  }

  var_sct *vlist_cls::atoi_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker)
  {
  const std::string fnc_nm("push_fnd::fnd");
  int fdx;
  int ierr=0;
  int nbr_args;
  char buffer[100];
  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::string att_nm;
   
  nc_type vtype;
  var_sct *var=NULL_CEWI;
  var_sct *var_in=NULL_CEWI;
  var_sct *var_err=NULL_CEWI;

  prs_cls *prs_arg=walker.prs_arg;

  fdx=fmc_obj.fdx();

  nbr_args=args_vtr.size();  

  vtype=NC_NAT;
  susg="usage: att_out="+sfnm+"( nc_char|nc_string  )";

  if(nbr_args<1)
    err_prn(sfnm, " Function has been called with wrong number of arguments arguments\n"+susg); 

  var_in=walker.out(args_vtr[0]);
  
  if(var_in->type != NC_CHAR && var_in->type != NC_STRING)
        err_prn(sfnm, " Function must be called with a text type argument"+susg); 

  // deal with initial scan      
  switch(fdx)
  {
     case PATOI: 
       vtype=NC_INT;     
       break; 

     case PATOL: 
       vtype=NC_INT64; 
  }


  if(prs_arg->ntl_scn)
  {
    var=ncap_sclr_var_mk( SCS("~zz@value_list"), vtype,false); 
    nco_var_free(var_in);
    return var; 

  }
   
  cast_void_nctype(var_in->type,&var_in->val);
  if(var_in->type==NC_CHAR)
  {
    strncpy(buffer, var_in->val.cp, var_in->sz); 
    buffer[var_in->sz]='\0';
  }
  else if(var_in->type==NC_STRING)
    strcpy(buffer, var_in->val.sngp[0]); 
 
  cast_nctype_void(var_in->type,&var_in->val);
  nco_var_free(var_in);
  
  

  switch(fdx)
  {
     case PATOI: 
       {          
         char *pend=NULL;
         nco_int iout;
         iout=0; 

	 // allows whites space prefix & suffix                                                                                                                                
         iout=(nco_int)std::strtol(buffer,&pend,10);

         if(pend != buffer && (*pend == '\0'|| *pend == ' '))
            ierr=0;
         else
            ierr=errno;

         //wrn_prn(fnc_nm,"buffer="+ SCS(buffer)+ " value=" + nbr2sng(iout)+"\n" );
         //var=ncap_sclr_var_mk( SCS("~zz@value_list"), vtype,iout);
         var=ncap_sclr_var_mk(SCS("~zz@value_list"),iout);

       }   
       break; 

     case PATOL: 
       {          
         char *pend=NULL;
         nco_int64 lout;
         lout=0; 

	      // allows whites space prefix & suffix
         lout=(nco_int64)std::strtoll(buffer,&pend,10);

         if( pend !=buffer && (*pend == '\0' || *pend == ' '))
            ierr=0;
         else
	        ierr=errno;


         wrn_prn(fnc_nm,"buffer="+ SCS(buffer)+ " value=" + nbr2sng(lout)+"\n" );
         var=ncap_sclr_var_mk( SCS("~zz@value_list"),lout);

       }   
       break; 
  }

  if(ierr) {
    var_err = ncap_sclr_var_mk(std::string("~zz@atoi_methods_err"), ierr);
    prs_arg->ncap_var_write(var_err, true);
  }
                 



  return var;

  }

  var_sct *vlist_cls::join_fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("vlist_cls::fnd");
  unsigned  idx;
  // int fdx=fmc_obj.fdx();
  int nbr_args;
  char *cstr;
  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  var_sct *var=NULL_CEWI;
  var_sct *var_att=NULL_CEWI;

  RefAST tr;
  RefAST aRef;

  std::vector<RefAST> vtr_args; 
  prs_cls *prs_arg=walker.prs_arg;
  std::vector<std::string> str_vtr;
          
  


  // Put args into vector 
  if(expr)
    vtr_args.push_back(expr);

  if((tr=fargs->getFirstChild()))
  {
    do  
      vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));    
  } 
            
  nbr_args=vtr_args.size();  

  susg="usage: att_out="+sfnm+"(att_id, att_nm|var_nm|string)";

  if(nbr_args!=2)
    err_prn(sfnm, " Function has been called with wrong number of arguments arguments\n"+susg); 


  // inital scan just return udf
  if(prs_arg->ntl_scn)
  {
   var=ncap_var_udf("~zz@join_methods");  
   return var;
  }

  var_att=walker.out(vtr_args[0]);

  if(!var_att )
     err_prn(sfnm, " first argument has evaluated to null\n"+susg); 

  if( var_att->type != NC_STRING && var_att->type != NC_CHAR )
    err_prn(sfnm, " first argument must be of character string type and NOT("+nbr2sng(var_att->type)+")\n"+susg); 


  ncap_att_str(var_att, str_vtr); 
  
  // deal with second argument    
  aRef=vtr_args[1];
  switch(aRef->getType())
  {
    case VAR_ID:
      str_vtr.push_back(aRef->getText());
      break;

    case ATT_ID: 
      {
	var_sct *var_arg;
        var_arg=walker.out(aRef); 
        ncap_att_str(var_arg, str_vtr); 
        nco_var_free(var_arg);    
      }
      break;

    case NSTRING:
    case N4STRING:
      str_vtr.push_back(aRef->getText());
      break;  
   
    default:
       err_prn(sfnm, " problem with second argument, must be a var_nm or a string or a string in an attribute  "+susg);      
    
  }

  // create output attribute
  var=ncap_sclr_var_mk("~zz@join_methods",(nc_type)NC_STRING,true);

  // stretch att if necessary
  if(str_vtr.size() >1 )
     ncap_att_stretch(var, str_vtr.size()); 
  
  (void)cast_void_nctype((nc_type)NC_STRING,&var->val);

  for(idx=0;idx<str_vtr.size();idx++)
  { 
    cstr=strdup(str_vtr[idx].c_str());
    var->val.sngp[idx]=cstr;  
  }

  (void)cast_nctype_void((nc_type)NC_STRING,&var->val);

  
  nco_var_free(var_att);

  return var;

}


//Derived Aggregate Functions /************************************************/

  aggd_cls::aggd_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("ncap_stats_wvariance",this,(int)PWVARIANCE));
    }
  }		      

  var_sct *aggd_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("aggd_cls::fnd");
  // int fdx=fmc_obj.fdx();
  int nbr_args;
           
  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::vector<RefAST> vtr_args;             
  RefAST aRef;
  RefAST tr;

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  var_sct *var_weight=NULL_CEWI;
  var_sct *var_weight_sum=NULL_CEWI;
  var_sct *var_weight_avg=NULL_CEWI;
  var_sct *var_in=NULL_CEWI;
  var_sct *var_out=NULL_CEWI;

  ddra_info_sct ddra_info;        
 
 
  // Put args into vector 
  if(expr)
    vtr_args.push_back(expr);

  if((tr=fargs->getFirstChild())) {
    do  
      vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));    
  } 
      
  nbr_args=vtr_args.size();  

  susg="usage var_out="+sfnm+"(var, weight)";

  if(nbr_args!=2)
    err_prn(sfnm, " Function has been called with no arguments\n"+susg); 

          
  var_in=walker.out(vtr_args[0]);   
  var_weight=walker.out(vtr_args[1]);   

  // deal with initial scan
  if(prs_arg->ntl_scn)
  {
    if(var_in->undefined)
       var_out=ncap_var_udf("~aggd_methods");   
    else
       var_out=ncap_sclr_var_mk(SCS("~aggd_methods"),var_in->type,false);       
  
    nco_var_free(var_in);
    nco_var_free(var_weight);  
    return var_out; 
     
  }
  
  /* nco stript we are implementing 
  
     (S1)  sum_weights = weights2.ttl();
     (S2)  weighted_avg = (weights2*var).ttl()/sum_weights;
     (S3)  anomaly = var - weighted_avg;
     (S3)  numerator = (weights2*anomaly*anomaly).ttl();
     (s4)  variance = numerator/sum_weights;
     std = variance.sqrt();
  */


  // make weight same type as var_in
  var_weight=nco_var_cnf_typ(var_in->type,var_weight);         
  
  // make vars conform or die 
  if( !ncap_var_stretch(&var_in,&var_weight) )
    err_prn(sfnm ,"unable to make weight var conform to input var");
  

  

  // make sure missing value is same is same in both vars
  // copy missing datums in var_in to var_weight
  if(var_in->has_mss_val)
  {
    long idx;
    long sz=var_in->sz;
    char *cp_in=(char*)var_in->val.vp;
    char *cp_out=(char*)var_weight->val.vp;
    char  *cp_miss_var;
    char  *cp_miss_weight;
    size_t slb_sz=nco_typ_lng(var_in->type);

    if(!var_weight->has_mss_val)
       nco_mss_val_cp(var_in,var_weight);
  
    cp_miss_var=(char*)var_in->mss_val.vp;
    cp_miss_weight=(char*)var_weight->mss_val.vp;
  


    for(idx=0;idx<sz;idx++)
    {
      if( !memcmp(cp_in, cp_miss_var, slb_sz) || !memcmp(cp_out, cp_miss_weight, slb_sz) )
	 memcpy(cp_out, cp_miss_var, slb_sz);   

      cp_in+=slb_sz;
      cp_out+=slb_sz;
    }
    
    memcpy(cp_miss_weight, cp_miss_var, slb_sz);   
  }
 

  // [S1] single value - duplicate is destroyed
  var_weight_sum=nco_var_avg(nco_var_dpl(var_weight),var_weight->dim,var_weight->nbr_dim ,nco_op_ttl,False,&ddra_info);
  
  // return var_weight_sum;
  // [S2] single value - duplicate is destroyed
  var_weight_avg = nco_var_avg(  ncap_var_var_stc( nco_var_dpl(var_weight), var_in, TIMES), var_weight->dim, var_weight->nbr_dim, nco_op_ttl,False,&ddra_info);

  // [S2] single value
  var_weight_avg = ncap_var_var_stc ( var_weight_avg, var_weight_sum, DIVIDE);  


  // [S2a] stretch var_weight_avg so it CONFORMS to var_in
  (void)ncap_att_stretch(var_weight_avg, var_in->sz);
  
  // [S3] var_weight destroyed in this operation
  var_out = nco_var_avg( ncap_var_var_stc( var_weight, ncap_var_var_stc( ncap_var_var_stc(var_in,var_weight_avg,MINUS), NULL_CEWI, VSQR2 ), TIMES ), var_weight->dim, var_weight->nbr_dim, nco_op_ttl,False,&ddra_info);

  // [S4]
  var_out= ncap_var_var_stc( var_out, var_weight_sum, DIVIDE);       
                                 
  nco_var_free(var_weight_avg);
  nco_var_free(var_weight_sum);
  nco_var_free(var_in);

  return var_out;

 }



//Print Function family /************************************************/

  print_cls::print_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("print",this,(int)PPRINT));
      fmc_vtr.push_back( fmc_cls("sprint",this,(int)PSPRINT));
      fmc_vtr.push_back( fmc_cls("sprint4",this,(int)PSNPRINT));

    }
  }		      

  var_sct *print_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("print_cls::fnd");
  int fdx;
  int nbr_args;
    
  char *fmt_sng=(char*)NULL;
  vtl_typ lcl_type;
  std::string susg;
  std::string sfnm=fmc_obj.fnm();
  std::vector<RefAST> vtr_args;             
  RefAST aRef;
  RefAST tr;

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  var_sct *var =NULL_CEWI;
  var_sct *var_att=NULL_CEWI; 
  var_sct *var_fmt=NULL_CEWI; 
        

  fdx=fmc_obj.fdx();
 
 
  // Put args into vector 
  if(expr)
    vtr_args.push_back(expr);

  if((tr=fargs->getFirstChild())) {
    do  
      vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));    
  } 

  nbr_args=vtr_args.size();

  susg="usage: att_out ?="+sfnm+"( att_id|var_id format_string?  )";
  if( nbr_args <1 ) 
     err_prn(sfnm, " Function has been called with wrong number of arguments arguments\n"+susg);  

  lcl_type=expr_typ(vtr_args[0]);          
  
  if(lcl_type == VVAR && fdx==PPRINT)  
    var_att=NULL;
  else if(lcl_type==VSTRING)
  {
    char *cp;
    nco_string val_string;

    cp = strdup(vtr_args[0]->getText().c_str());          
    (void)sng_ascii_trn(cp);            
    val_string=cp; 
    var_att=ncap_sclr_var_mk("~zz@print_methods", val_string); 

  }
  else  
    var_att=walker.out(vtr_args[0]);       


 if(nbr_args>1)
   var_fmt=walker.out(vtr_args[1]);
 
 if(prs_arg->ntl_scn)
 {   
   if(var_att)
     nco_var_free(var_att);
   if(var_fmt)
     nco_var_free(var_fmt);

   var=ncap_var_udf("~zz@print_methods");
   return  var;

 }

 
 if(var_fmt) 
 {
   if(var_fmt->type != NC_CHAR && var_fmt->type != NC_STRING) 
     err_prn(sfnm,"Format string must be a text type");

   cast_void_nctype(var_fmt->type,&var_fmt->val); 
   fmt_sng=(char*)nco_malloc(sizeof(char)*(var_fmt->sz+1));      
  
   if(var_fmt->type==NC_STRING)
     strcpy(fmt_sng,var_fmt->val.sngp[0]);
   else
   {
     strncpy(fmt_sng,var_fmt->val.cp, var_fmt->sz);  
     fmt_sng[var_fmt->sz]='\0';   
   }

  cast_nctype_void(var_fmt->type,&var_fmt->val); 

  nco_var_free(var_fmt);
 }

 // deal with important special case print && VVAR 
 if(fdx==PPRINT &&  lcl_type==VVAR )
 {
   // check output first -nb can only print out vars that are defined AND written
   // flg_stt==2 means var defined with data
   int var_id;
   int fl_id=-1;

   const char *var_nm;
   NcapVar *Nvar;
   var_nm=vtr_args[0]->getText().c_str();

   Nvar=prs_arg->var_vtr.find(var_nm);

  
    

   if(Nvar && Nvar->flg_stt==2)
     fl_id=prs_arg->out_id;   
   else
    // Check input file for var   
    if(NC_NOERR==nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id))
     fl_id=prs_arg->in_id;
    

   if(fl_id==-1) 
   {
    wrn_prn(fnc_nm,"Print function cannot find var \""+SCS(var_nm)+"\" in input or output");
    return var;
   }

   
   if( fl_id >=0)
     (void)nco_prn_var_val_lmt(fl_id,var_nm,(lmt_sct*)NULL,0L,fmt_sng,prs_arg->FORTRAN_IDX_CNV,False,False);

   nco_free(fmt_sng);
   return var;
 }





 switch(fdx)
 {
   case PPRINT:
       ncap_att_prn(var_att,fmt_sng);  
       var=NULL_CEWI;   
     break;
   case PSPRINT:
     {
       char *cp;   
       long sz;  
       cp=ncap_att_sprn(var_att,fmt_sng);
       sz=strlen(cp);
       var=ncap_sclr_var_mk("~zz@print_methods",NC_CHAR,false);    
       cast_void_nctype(NC_CHAR,&var->val);
       var->val.cp=(char*) nco_malloc( sizeof(char)*sz);
       strncpy(var->val.cp,cp,sz);
       var->sz=sz;
       cast_nctype_void(NC_CHAR,&var->val);            
       free(cp);
     }
     break; 
  case PSNPRINT:
     {
       char *cp; 
       cp=ncap_att_sprn(var_att,fmt_sng);
       var=ncap_sclr_var_mk("~zz@print_methods",NC_STRING,true);    
       cast_void_nctype(NC_STRING,&var->val);
       var->val.sngp[0]=(nco_string)cp;   
       cast_nctype_void(NC_STRING,&var->val);            
       var->sz=1; 
     }
    break;

 }

 if(var_att)
    nco_var_free(var_att);

 if(fmt_sng)
   nco_free(fmt_sng);



 return var;

}



//udunits Functions /***********************************/ 
  udunits_cls::udunits_cls(bool){
    //Populate only on  constructor call
    if(fmc_vtr.empty()){
          fmc_vtr.push_back( fmc_cls("udunits",this,PUNITS1)); 
          fmc_vtr.push_back( fmc_cls("strftime",this,PSTRFTIME));
          fmc_vtr.push_back( fmc_cls("regular",this,PREGULAR));
          fmc_vtr.push_back( fmc_cls("clbtime",this,CLBTIME));


    }		      
  } 

  var_sct * udunits_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("udunits_cls::fnd");
  int fdx;
  int nbr_args;
  int rcd;
  bool is_mtd;
 
  var_sct *var=NULL_CEWI;
  var_sct *var_ud_out=NULL_CEWI;


  std::string susg;
  std::string sfnm=fmc_obj.fnm();

  std::string att_sng;
  std::string units_in_sng;
  std::string calendar_in_sng;
  //std::string udunits_out_sng


  RefAST tr;
  std::vector<RefAST> args_vtr; 
  std::vector<std::string> cst_vtr;              

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;            
  nc_type lcl_typ;


  fdx=fmc_obj.fdx();
 

  if(expr)
      args_vtr.push_back(expr);

    if((tr=fargs->getFirstChild())) {
      do  
	args_vtr.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
  nbr_args=args_vtr.size();  
  is_mtd=(expr ? true: false);

  /* deal with printing a time stamp here */
  if(fdx==PSTRFTIME)
    return strftime_fnd( is_mtd,args_vtr, fmc_obj, walker); 
  else if(fdx==PREGULAR)
    return regular_fnd( is_mtd,args_vtr, fmc_obj, walker);
  else if(fdx==CLBTIME)
    return clbtime_fnd( is_mtd,args_vtr, fmc_obj, walker);

  susg="usage: var_out="+sfnm+"(var_in ,unitsOutString)"; 

  
  if(nbr_args<2)
      err_prn(sfnm,"Function has been called with less than two arguments\n"+susg); 



  if(nbr_args >2 &&!prs_arg->ntl_scn) 
      wrn_prn(sfnm,"Function been called with more than two arguments"); 

  
  /* data to convert */ 
  var=walker.out(args_vtr[0]);  

  /* text string output units */
  var_ud_out=walker.out(args_vtr[1]);  

  lcl_typ=var->type;
  if( !var->undefined && var->type !=NC_FLOAT && var->type !=NC_DOUBLE )
    nco_var_cnf_typ(NC_DOUBLE,var); 

  
  if(prs_arg->ntl_scn  ){
    nco_var_free(var_ud_out);
    return var;
  }

  if(var_ud_out->type !=NC_CHAR && var_ud_out->type !=NC_STRING)
     err_prn(sfnm,"The second argument must be a netCDF text type\n"+susg);


  // grab units attribute of var
  units_in_sng=ncap_att2var(prs_arg, std::string(var->nm)+"@units");



  // grab calendar attribute of var
  att_sng=std::string(var->nm)+"@calendar";

  if( ncap_att2var_chk(prs_arg, att_sng) )
  {
     calendar_in_sng=ncap_att2var(prs_arg, att_sng);

  }





  // do heavy lifting 
  {
   char *units_out_sng;

   nco_cln_typ cln_typ=cln_nil;

   units_out_sng=ncap_att_char(var_ud_out);   

   if(calendar_in_sng.size())
     cln_typ=nco_cln_get_cln_typ(calendar_in_sng.c_str());

   rcd=nco_cln_clc_dbl_var_dff(units_in_sng.c_str(),units_out_sng,cln_typ,(double*)NULL, var);
    
   if(rcd!=NCO_NOERR)
     err_prn(sfnm, "Udunits was unable to convert data in the var '"+std::string(var->nm)+"' from '" +std::string(units_in_sng) +"' to '"+std::string(units_out_sng)+"'\n");

   // nco_free(units_in_sng);
   nco_free(units_out_sng);
  }

  /* revert var back to original type */
  if(var->type != lcl_typ)
    nco_var_cnf_typ(lcl_typ,var);

  return var;

}


var_sct *udunits_cls::strftime_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker)
 {
    int nbr_args;
    //int fdx=fmc_obj.fdx();
    int rcd;
    char *cformat=(char*)NULL;

    var_sct *var=NULL_CEWI;
    var_sct *var_cformat=NULL_CEWI;
    var_sct *var_ret=NULL_CEWI;

    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    std::string att_sng;
    std::string units_in_sng;
    std::string units_out_sng;
    std::string calendar_in_sng;


    prs_cls *prs_arg=walker.prs_arg;            
    nco_cln_typ cln_typ;

    nbr_args=args_vtr.size();

    // convert time to UTC for now -hwta about other calendars ?
    units_out_sng="seconds since 1970-01-01";

    susg="usage: var_out="+sfnm+"(var_in ,\"format-timestring ? \")";

  
    if(nbr_args<1)
       err_prn(sfnm,"Function has been called with less than two arguments\n"+susg); 
    
    /* data to convert */ 
    var=walker.out(args_vtr[0]);  


    if( !var->undefined && var->type !=NC_FLOAT && var->type !=NC_DOUBLE )
      nco_var_cnf_typ(NC_DOUBLE,var); 

     
    var_ret=nco_var_dpl(var);
    if(var_ret->val.vp)
      var_ret->val.vp=nco_free(var_ret->val.vp);
    
    var_ret=nco_var_cnf_typ(NC_STRING, var_ret);

    /* text string output units */
    if(nbr_args >=2)
       var_cformat=walker.out(args_vtr[1]);


    if(prs_arg->ntl_scn  )
    {
      if(var_cformat)  
         nco_var_free(var_cformat);
      
      if(var)        
	     nco_var_free(var);

      return var_ret;
    }


    if(var_cformat)
    {
       if (var_cformat->type != NC_CHAR && var_cformat->type != NC_STRING)
          err_prn(sfnm, "The second argument must be a netCDF text type\n" + susg);

       cformat = ncap_att_char(var_cformat);
       var_cformat=(var_sct*)nco_var_free(var_cformat);

    }
    // use default strtime string - this is like the ncdump -t output string
    else
    {
      cformat=strdup("%Y-%m-%d %H:%M:%S");
    }


     // grab units attribute of var
     units_in_sng=ncap_att2var(prs_arg, std::string(var->nm)+"@units");

    // grab calendar attribute of var
    att_sng=std::string(var->nm)+"@calendar";

    if( ncap_att2var_chk(prs_arg, att_sng) )
    {
      calendar_in_sng = ncap_att2var(prs_arg, att_sng);
      cln_typ=nco_cln_get_cln_typ(calendar_in_sng.c_str());
    }
    else
      cln_typ=cln_nil;


    rcd=nco_cln_clc_dbl_var_dff(units_in_sng.c_str(),units_out_sng.c_str(),cln_typ,(double*)NULL, var);

    if(rcd!=NCO_NOERR)
         err_prn(sfnm, "Udunits was unable to convert data in the var '"+std::string(var->nm)+"' from '" +units_in_sng +"' to '"+units_out_sng+"'\n");


     //start heavy lifting
    {
      long idx;
      long sz;
      char schar[200];
      time_t sgmt;
      struct tm tp;  
      double *dp;


      // make some space for strings
      var_ret->val.vp=(void *)nco_malloc(var_ret->sz*nco_typ_lng(var_ret->type));
       
       
      (void)cast_void_nctype(var->type,&var->val);     
      (void)cast_void_nctype(var_ret->type,&var->val);

         
     
      dp=var->val.dp; 
       
      sz=var->sz;

      for(idx=0;idx<sz;idx++)
      {
          
        sgmt=(time_t)dp[idx];  
#ifdef _MSC_VER
    (void)gmtime_s(&tp, &sgmt);
#else
    (void)gmtime_r(&sgmt,&tp);
#endif /* !_MSC_VER */
        strftime(schar,sizeof(schar),cformat,&tp);
        var_ret->val.sngp[idx]=strdup(schar);           
      }


      (void)cast_nctype_void(var->type,&var->val);
      (void)cast_nctype_void(var_ret->type,&var_ret->val);     

    }


    var=(var_sct*)nco_var_free(var);

    return var_ret;

 }


var_sct *udunits_cls::regular_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker)
 {
    int nbr_args;
    // int fdx=fmc_obj.fdx();
    int rcd;
    int iformat=0;

    var_sct *var=NULL_CEWI;
    var_sct *var_iformat=NULL_CEWI;
    var_sct *var_ret=NULL_CEWI;

    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    std::string att_sng;
    std::string units_in_sng;
    std::string units_out_sng;
    std::string calendar_in_sng;


    prs_cls *prs_arg=walker.prs_arg;            
    nco_cln_typ cln_typ;

    nbr_args=args_vtr.size();

    // convert time to UTC for now -hwta about other calendars ?
    units_out_sng="seconds since 1970-01-01";

    susg="usage: var_out="+sfnm+"(var_in ,\"format-timestring ? \")";
  
    if(nbr_args<1)
       err_prn(sfnm,"Function has been called with less than two arguments\n"+susg); 
    
    /* data to convert */ 
    var=walker.out(args_vtr[0]);  

    if( !var->undefined && var->type !=NC_FLOAT && var->type !=NC_DOUBLE )
      nco_var_cnf_typ(NC_DOUBLE,var); 
     
    var_ret=nco_var_dpl(var);
    if(var_ret->val.vp)
      var_ret->val.vp=nco_free(var_ret->val.vp);
    
    var_ret=nco_var_cnf_typ(NC_STRING, var_ret);

    /* text string output units */
    if(nbr_args >=2)
       var_iformat=walker.out(args_vtr[1]);

    if(prs_arg->ntl_scn  )
    {
      if(var_iformat)  
         nco_var_free(var_iformat);
      
      if(var)        
	     nco_var_free(var);

      return var_ret;
    }

    if(var_iformat)
    {
       nco_var_cnf_typ(NC_INT,var_iformat);
       cast_void_nctype(NC_INT,&var_iformat->val);
       iformat=var_iformat->val.ip[0];

       var_iformat=(var_sct*)nco_var_free(var_iformat);
    }
    // use default format
    else
      iformat=1;

     // grab units attribute of var
     units_in_sng=ncap_att2var(prs_arg, std::string(var->nm)+"@units");

    // grab calendar attribute of var
    att_sng=std::string(var->nm)+"@calendar";

    if( ncap_att2var_chk(prs_arg, att_sng) )
    {
      calendar_in_sng = ncap_att2var(prs_arg, att_sng);
      cln_typ=nco_cln_get_cln_typ(calendar_in_sng.c_str());
    }
    else
      cln_typ=cln_nil;

    rcd=nco_cln_var_prs(units_in_sng.c_str(), cln_typ,iformat, var,var_ret);
    if(rcd==NCO_ERR)
        err_prn(sfnm,"Error formatting time string");

    var=(var_sct*)nco_var_free(var);
    return var_ret;
 }

var_sct *udunits_cls::clbtime_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker)
{
  int idx;
  const int md_sz=8;
  int iret=0;

  std::vector<std::string> str_vtr;

  std::string sfnm =fmc_obj.fnm(); //method name
  var_sct *var_md[md_sz];

  var_sct *var_ret=NULL_CEWI;

  /* arg list
  0   yr_srt    int
  1   yr_end    int
  2   mth_srt   int
  3   mth_end   int
  4   tpd       int
  5   unt_sng   const *char
  6   cln_sng   const *char
  7   time coord to be duplicated and returned
  */

  for(idx=0;idx<md_sz;idx++) {
    var_md[idx] = walker.out(args_vtr[idx]);
    cast_void_nctype(var_md[idx]->type,&var_md[idx]->val);
  }

  if( var_md[5]->type !=NC_CHAR && var_md[5]->type !=NC_STRING )
    err_prn(sfnm, "unt_sng - 5th argument must be of type string\n");

  if( var_md[6]->type !=NC_CHAR && var_md[6]->type !=NC_STRING)
    err_prn(sfnm, "cln_sng - 6th argument must be of type string\n");

  ncap_att_str(var_md[5], str_vtr );
  ncap_att_str(var_md[6], str_vtr );

  /* print out all args */
  if(nco_dbg_lvl_get() >= nco_dbg_dev &&  !walker.prs_arg->ntl_scn ){
    (void) fprintf(stderr, "clbtime: yr_srt=%d yr_end=%d mth_srt=%d mth_end=%d tpd=%d\n",
     var_md[0]->val.ip[0], var_md[1]->val.ip[0], var_md[2]->val.ip[0], var_md[3]->val.ip[0],var_md[4]->val.ip[0]);
    (void) fprintf(stderr,"unt_sng=%s cln_sng=%s\n", str_vtr[0].c_str(), str_vtr[1].c_str());
  }
  {
    iret = nco_clm_nfo_to_tm_bnds( var_md[0]->val.ip[0], var_md[1]->val.ip[0], var_md[2]->val.ip[0],
                                   var_md[3]->val.ip[0], var_md[4]->val.ip[0],
                                   str_vtr[0].c_str(), str_vtr[1].c_str(),
                                  (var_md[7]->nbr_dim==2 ? var_md[7]->val.dp: NULL ), (var_md[7]->nbr_dim==1 ? var_md[7]->val.dp: NULL )
                                  );
  }

   if(iret==NCO_ERR)
     err_prn(sfnm,"nco_cln_nfo_to_tm_bnds() returned error");

  /* duplicate time var  */
  var_ret=nco_var_dpl(var_md[7]);

  for(idx=0;idx<md_sz;idx++) {
    cast_nctype_void(var_md[idx]->type,&var_md[idx]->val);
    var_md[idx]=nco_var_free(var_md[idx]);
  }
  return var_ret;
}

//Polygon Function family /************************************************/
polygon_cls::polygon_cls(bool){
  //Populate only on first constructor call
  if(fmc_vtr.empty()){
    fmc_vtr.push_back( fmc_cls("print_kml",this,(int)PKML));
    fmc_vtr.push_back( fmc_cls("print_kml_filter",this,(int)PKMLFILTER));
  }
}

var_sct *polygon_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("polygon_cls::fnd");
  int fdx=fmc_obj.fdx();   //index
  int nbr_args;


  prs_cls* prs_arg=walker.prs_arg;

  var_sct *var_ret=NULL_CEWI;
  var_sct *var_lat=NULL_CEWI;
  var_sct *var_lon=NULL_CEWI;
  var_sct *var_filter=NULL_CEWI;

  RefAST tr;
  std::string susg;
  std::string serr;
  std::string sfnm =fmc_obj.fnm(); //method name
  std::vector<RefAST> vtr_args;
  NcapVector<dmn_sct*> dmn_vtr;


  susg="ret_code="+sfnm+"(grid_corner_lat, grid_corner_lon)";

  if(expr)
    vtr_args.push_back(expr);

  if((tr=fargs->getFirstChild())) {
    do
      vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));
  }

  nbr_args=vtr_args.size();


  if(nbr_args < 2)
    err_prn(fnc_nm, "function must be called with at least 2 arguments\n"  + susg);

  var_lat=walker.out(vtr_args[0]);
  var_lon=walker.out(vtr_args[1]);

  if(nbr_args>2)
    var_filter=walker.out(vtr_args[2]);




  if(prs_arg->ntl_scn) {
    var_lat=nco_var_free(var_lat);
    var_lon=nco_var_free(var_lon);

    if(var_filter)
      var_filter=nco_var_free(var_filter);


    var_ret=ncap_sclr_var_mk(SCS("~dot_methods"),NC_INT,false);
    return var_ret;
  }


  // print out kml
  {
    bool is_convex;  /* true if polygon is convex */
    int idx;
    int jdx;
    int grid_corners;
    int grid_size;

    const char prn_str[]="%.14f,%.14f,0\n";


    if(var_lat->nbr_dim != 2 || var_lon->nbr_dim !=2 || var_lat->type != NC_DOUBLE || var_lon->type != NC_DOUBLE)
      err_prn(fnc_nm, "both input vars( "+  SCS(var_lat->nm)+","+ SCS(var_lon->nm) + ") must have 2 dims,  and be of type NC_DOUBLE\n");

    grid_size=var_lat->dim[0]->cnt;
    grid_corners=var_lat->dim[1]->cnt;

    if(var_lon->dim[0]->cnt != grid_size || var_lon->dim[1]->cnt != grid_corners)
      err_prn(fnc_nm, "var " + SCS(var_lon->nm) + " must be same shape as " + SCS(var_lat->nm) + ".\n");

    (void)cast_void_nctype(NC_DOUBLE,&var_lon->val);
    (void)cast_void_nctype(NC_DOUBLE,&var_lat->val);

    if(var_filter)
      (void)cast_void_nctype(NC_DOUBLE,&var_filter->val);

    (void)fprintf(stdout, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    (void)fprintf(stdout, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");

    (void)fprintf(stdout,"\n<Document>\n");
    for(idx=0;idx<grid_size;idx++){
      //      bool brep=false;
      //      double pdlon=1e10;
      //      double pdlat=1e10;
      double dlon;
      double dlat;

      is_convex=chk_polygon_convex( &var_lon->val.dp[ idx*grid_corners ], &var_lat->val.dp[ idx*grid_corners ], grid_corners );

      // filter grids by x width (longitude)
      if( fdx== PKMLFILTER)

      {
        double minmax[2];

        /* longitude */
        (void)get_minmax_polygon( &var_lon->val.dp[ idx*grid_corners ], grid_corners, &minmax[0], &minmax[1] );

        if(minmax[1]-minmax[0] > var_filter->val.dp[0] )
          continue;


        (void)get_minmax_polygon( &var_lat->val.dp[ idx*grid_corners ], grid_corners,  &minmax[0], &minmax[1] );
        if(minmax[1]-minmax[0] > var_filter->val.dp[0] )
          continue;


      }


      if(!is_convex)
      {
        fprintf(stderr, "WARNING: polygon %d not convex\n",idx );
        /* we can identify Concave polygon because  Polygon TAG is on its own line  */
        (void)fprintf(stdout, "<Placemark>\n<Polygon>\n<outerBoundaryIs> <LinearRing>\n<coordinates>\n");
      }
      else
        (void)fprintf(stdout, "<Placemark>\n<Polygon><outerBoundaryIs> <LinearRing>\n<coordinates>\n");

      for(jdx=0;jdx<grid_corners;jdx++){
        /// assume input range 0-360.0
        // KML google world -180.0 /  180.0
        // check repeat of final values

        dlon=var_lon->val.dp[ idx*grid_corners+jdx ] ;
        dlat=var_lat->val.dp[ idx*grid_corners+jdx ];

        (void)fprintf(stdout,prn_str, dlon, dlat);
        /*
        if( pdlon != dlon &&  pdlat != dlat ){
           (void)fprintf(stdout,prn_str, dlon, dlat);
           pdlon=dlon; pdlat=dlat;
        }
            */
      }

      // output first point again KML demand this ? (only output if not alreaady repeated
      (void)fprintf(stdout, prn_str, var_lon->val.dp[ idx*grid_corners] ,var_lat->val.dp[ idx*grid_corners]);
      (void)fprintf(stdout, "</coordinates>\n</LinearRing></outerBoundaryIs></Polygon>\n</Placemark>\n");

    }
    (void)fprintf(stdout,"</Document>\n</kml>");



    (void)cast_nctype_void(NC_DOUBLE,&var_lon->val);
    (void)cast_nctype_void(NC_DOUBLE,&var_lat->val);
    if(var_filter)
      (void)cast_nctype_void(NC_DOUBLE,&var_filter->val);





  }
  var_lat=nco_var_free(var_lat);
  var_lon=nco_var_free(var_lon);

  if(var_filter)
    var_filter=nco_var_free(var_filter);

  var_ret=ncap_sclr_var_mk(SCS("~dot_methods"),1);

  return var_ret;

}

bool polygon_cls::chk_polygon_convex(double *dp_x,  double *dp_y, int grid_corners){

  bool sign;
  bool sign_init;

  int idx;
  int idx1;
  int idx2;
  int sz=grid_corners;

  double area;


  sign_init=false;

  for(idx=0;idx<sz;idx++)
  {
    idx1=(idx+1)%sz;
    idx2=(idx+2)%sz;
    area= ( dp_x[idx1] -dp_x[idx] )  *  (dp_y[idx2] -dp_y[idx1] )  -  (dp_x[idx2] - dp_x[idx1]) * (dp_y[idx1] -dp_y[idx]) ;

    /* skip contiguous identical vertex */
    if(area==0.0)
      continue;


    if(!sign_init)
    {
      sign=(area>0.0);
      sign_init=true;
    }

    if(sign != (area>0.0))
      return false;


  }
  return true;

}


void polygon_cls::get_minmax_polygon(double *dp, int grid_corners, double *xmin, double *xmax)
{
  int idx;

  *xmin=1.0e30;
  *xmax=-1.0e30;

  for(idx=0;idx<grid_corners;idx++)
  {
    if(dp[idx]< *xmin )
      *xmin=dp[idx];

    if(dp[idx]>  *xmax )
      *xmax=dp[idx];



  }

  return;

}







/* ncap2 functions and methods */

/* To avoid confusion when I say FUNC (uppercase) I mean a custom ncap2 function.
   When I say METHOD (uppercase) I mean a custom ncap2 method 
   When I say method (lowercase) I mean a C++ class method
   When I say function(lowercase) I mean a C/C++ function

   When I refer to an ncap2  FUNC. I also mean a METHOD 
   most of the FUNCS can be called as  METHODS  e.g.,  
 
  sum=three_dmn_var_dbl.total($time) or sum=total(three_dmn_var_dbl,$time)
 
  So for a method the "first argument" is the evaluation of everything prefixing the DOT.
  The nice thing about METHODS is that you can daisy chain them together.  
  e.g., promote to type double - find avg - then convert back to short()

  three_avg=three_dmn_var_sht.double().avg($time).short();
  
  file: ncap2.cc
  The vector fmc_vtr in ncap2.cc contains all the method pointers that will deal with all the FUNC's 
  Each element in this vector has the following properties

  string fnm() -- The name of the FUNC - as used in the script.
                  It is defined in one place only  - the constructor of the C++ class 
                  that will deal with the FUNC  

  vtl_cls vfnc -- This points to the  C++ method that will deal with the FUNC.
                  As you look at all the classes in fmc_all_cls.cc you will see that all
                  the  methods thet deal with a FUNC share the same argument signature
                  More on this later.   

  int fdx()   --  This is the index (with respect to a particular class) of the FUNC called.
                  Some classes deal with a whole family of FUNCS others only one or two
                  The index's are defined in an enum. 
                  so for (basic)-    bsc_cls we have:   enum {PSIZE, PTYPE, PNDIMS, PEXISTS };
                  and for(aggregate) agg_cls we have:   enum {PAVG ,PAVGSQR ,PMIBS, PMABS, PMEBS, PMAX ,PMIN ,PRMS, PRMSSDN, PSQRAVG, PTTL} 

  This vector 'fmc_vtr' is used by the lexer to identify FUNC names and to distingush them from var names 
  so for example:
     total = three_dmn_var_dbl.total($lat,$lon);   
  Is completely valid - The lexer sees that the lvalue total has no trailing '(' and so 
  identifies it as a variable. If I subsequently said:
     total0= total(0);       
  The lexer  would get confused. It would recognize 'total' on the RHS as a FUNC call rather than a hyperslab.
   
  After being populated fmc_vtr is then passed by reference to the object prs_arg. This object provides the 
  Parser with the necessary detail to enable IO to netcdf files. It also maintains a lookup table for vars/ atts 
  and memory vars. The most important properties listed are below

  NcapVector<dmn_sct*> &dmn_in_vtr;        //Vector of dimensions in input file nb doesn't change
  NcapVector<dmn_sct*> &dmn_out_vtr;       //Vector of dimensions in output file file
  std::vector<fmc_cls> &fmc_vtr;           //List of functions/methods nb doesn't change 
  NcapVarVector &var_vtr;                  // list of attributes & variables
  NcapVarVector &int_vtr;                  // stores vars/atts in FIRST PARSE
  bool ntl_scn;                            // [flg] Initial scan of script 
 
  The most important methods are the following:
  
 // initialize var_sct with the variable snm
  var_sct *ncap_var_init(const std::string &snm, bool bfll);        
 // write var to disc
 int ncap_var_write (var_sct *var, bool bram);           
 
 Perhaps the most important thing to grasp is that given a list of statements TWO PARSES of the script (syntax tree) are made.
 In the first parse the vars are defined on disc. In the second parse the vars are populated. 
 This greatly increases peformance. 

 The Grammer file ncoGrammer.g file is composed of three sections
 ncoLexer  -  returns tokens/ deals with include files
 ncoParser -  The main grammer - creates a syntax tree
 ncoTree   -  Reads a syntax tree

 Hopefully the only thing you'll deal with here is the ncoTree methods */

/*
  Lets take a look at mth2_cls(). 
  The enum in the include file is: 
    enum {PPOW,PATAN2,PCONVERT};
  The constructor associates the pointer(this) and  FUNC script name  with the enum index:  
   fmc_vtr.push_back( fmc_cls("pow",this,(int)PPOW));
   fmc_vtr.push_back( fmc_cls("atan2",this,(int)PATAN2));
   fmc_vtr.push_back( fmc_cls("convert",this,(int)PCONVERT));

 The method that does the heavy lifting is:
    var_sct *mth2_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker)
  
 It takes two fragments of the the parse tree ( expr, fargs) and returns a var_sct. 
 
 If expr is null then it is A FUNC else its a METHOD */



