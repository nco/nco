/* $Header: /data/zender/nco_20150216/nco/src/nco++/fmc_all_cls.cc,v 1.9 2009-03-24 15:37:32 hmb Exp $ */

/* Purpose: netCDF arithmetic processor class methods: families of functions/methods */

/* Copyright (C) 1995--2007 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
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
	serr="Function "+sfnm + " has been called without an argument";               
        err_prn(fnc_nm,serr);
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
            dmn_sct **dim;
            var_sct *var=NULL_CEWI;
            var_sct *var1=NULL_CEWI;
           
	    std::string styp;
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
 
    	    styp=(expr ? "method":"function");
 
            // Put args into vector 
            if(expr)
               vtr_args.push_back(expr);

            if(tr=fargs->getFirstChild()) {
              do  
	        vtr_args.push_back(tr);
              while(tr=tr->getNextSibling());    
            } 
      
            nbr_args=vtr_args.size();  

            if(nbr_args==0)
              err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 


            var1=walker.out(vtr_args[0]);

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
                       serr="Argument \""+aRef->getText()+"\" to " + styp+": "+sfnm+" is not a dimension"; 
                       wrn_prn(fnc_nm,serr);
                       break;
                } // end switch

             } // end for 

             if(vtr_args.size() >0) 
               dmn_vtr=ncap_dmn_mtd(var1, str_vtr);

            
            // Initial scan 
            if(prs_arg->ntl_scn){
                nbr_dim=var1->nbr_dim;
                
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
            
            if(dmn_vtr.size() >0){
                dim=&dmn_vtr[0];
                nbr_dim=dmn_vtr.size();                           
            } else {
                dim=var1->dim;
                nbr_dim=var1->nbr_dim; 
            }    
            
            // Final scan
            if(!prs_arg->ntl_scn){
                
                switch(fdx){
                    
                case PAVG:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_avg,False,&ddra_info);
                    // Use tally to normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    break;
                    
                case PAVGSQR:
                    var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_avgsqr,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    break;
                    
                case PMAX:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_max,False,&ddra_info);
                    break;
                    
                case PMIN:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_min,False,&ddra_info);
                    break; 
                    
                case PRMS:
                    var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_rms,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Take root
                    (void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
                    break;
                    
                case PRMSSDN:
                    var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_rmssdn,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm_sdn(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Take root
                    (void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
                    break;
                    
                case PSQRAVG:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_sqravg,False,&ddra_info);
                    // Normalize 
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Square mean
                    (void)nco_var_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,var->val);
                    break;
                    
                case PTTL:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_ttl,False,&ddra_info);
                    break;
                } 
                // var1 is freed in nco_var_avg()
            }
            return var;                
	    }     
            
	    

//Utility Functions /******************************************/

   utl_cls::utl_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("set_miss",this,(int)SET_MISS));
      fmc_vtr.push_back( fmc_cls("get_miss",this,(int)GET_MISS));
      fmc_vtr.push_back( fmc_cls("change_miss",this,(int)CH_MISS));
      fmc_vtr.push_back( fmc_cls("delete_miss",this,(int)DEL_MISS));
      fmc_vtr.push_back( fmc_cls("ram_write",this,(int)RAM_WRITE));
      fmc_vtr.push_back( fmc_cls("ram_delete",this,(int)RAM_DELETE));
     
    }
  }

  var_sct *utl_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("utl_cls::fnd");
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
    styp=(expr ? "method":"function");
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
	  var_ret= ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),var_tmp->type,false);
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



    if(!Nvar ){
       wrn_prn(fnc_nm,sfnm+" unable to find variable: "+va_nm); 
       if(var) var=nco_var_free(var);
       return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),(nco_int)rval);        
    }

    if(prs_arg->ntl_scn) {
      if(var) var=nco_var_free(var);
      return ncap_sclr_var_mk(static_cast<std::string>("~utility_function"),(nc_type)NC_INT,false);  
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




//Basic Functions /******************************************/

  bsc_cls::bsc_cls(bool  flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("size",this,(int)PSIZE));
      fmc_vtr.push_back( fmc_cls("type",this,(int)PTYPE));
      fmc_vtr.push_back( fmc_cls("ndims",this,(int)PNDIMS));

    }
  }

  var_sct *bsc_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("bsc_cls::fnd");

    int fdx=fmc_obj.fdx();   //index
    int nbr_fargs;
    prs_cls* prs_arg=walker.prs_arg;
    var_sct *var=NULL_CEWI;
    var_sct *var1=NULL_CEWI;
    RefAST tr;
    vtl_typ lcl_typ;

    std::string sfnm =fmc_obj.fnm(); //method name

    //n.b fargs is an imaginary node -and is ALWAYS present
    nbr_fargs=fargs->getNumberOfChildren();

    // no arguments - bomb out
    if(!expr && nbr_fargs==0){    
        std::string serr;
	serr="Function "+sfnm + " has been called without an argument";               
        err_prn(fnc_nm,serr);
    }
    
      

    if(expr)
      tr=expr;
    else
      tr=fargs->getFirstChild(); 


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

    if(!prs_arg->ntl_scn) {

      std::string va_nm=tr->getText();

     
      if(lcl_typ==VVAR)
	var1=prs_arg->ncap_var_init(va_nm,false); 
      else 
        var1=walker.out(tr);
    

      switch(fdx){ 
           case PSIZE:
             var=ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_int)var1->sz);
             break;
           case PTYPE:
             var=ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_int)var1->type);
             break;
           case PNDIMS:
             var=ncap_sclr_var_mk(static_cast<std::string>("~basic_function"),(nco_int)var1->nbr_dim);            

      } // end switch        
      
      var1=nco_var_free(var1);
       


    } //end if      
    return var;		 
 }       

  
//Math Functions /******************************************/
  mth_cls::mth_cls(bool flg_dbg){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){


      /* Basic math: acos, asin, atan, cos, exp, log, log10, rnd_nbr, sin, sqrt, tan */
      sym_vtr.push_back(sym_cls("acos",acos,acosf));  
      sym_vtr.push_back(sym_cls("asin",asin,asinf));
      sym_vtr.push_back(sym_cls("atan",atan,atanf));
      sym_vtr.push_back(sym_cls("cos",cos,cosf));  
      sym_vtr.push_back(sym_cls("exp",exp,expf));
      sym_vtr.push_back(sym_cls("fabs",fabs,fabsf));
      sym_vtr.push_back(sym_cls("log",log,logf));
      sym_vtr.push_back(sym_cls("log10",log10,log10f));
      //sym_vtr.push_back(sym_cls("rnd_nbr",rnd_nbr,rnd_nbrf));
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
    int nbr_fargs;
    var_sct *var1;
    var_sct *var;   
    fdx=fmc_obj.fdx(); 
    std::string styp;

    std::string sfnm =fmc_obj.fnm(); 

    //n.b fargs is an imaginary node -and is ALWAYS present
    nbr_fargs=fargs->getNumberOfChildren();
    
    styp=(expr ? "method":"function");

    if(expr)
      nbr_fargs++;

   
    if(nbr_fargs >1) 
      wrn_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with too many arguments"); 
  
    if(nbr_fargs<1)
      err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 
     
    if(expr) 
      var1=walker.out(expr);
    else
      var1=walker.out(fargs->getFirstChild());   

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

    prs_cls* prs_arg=walker.prs_arg;
    var_sct *var=NULL_CEWI;
    var_sct *var1=NULL_CEWI;
    var_sct *var2=NULL_CEWI;
    std::string styp;
    RefAST tr;
 
    std::vector<RefAST> vtr_args; 


    std::string sfnm =fmc_obj.fnm(); //method name

    styp=(expr ? "method":"function");

    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
     nbr_args=vtr_args.size();  
      
     if(nbr_args >2) 
       wrn_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with too many arguments"); 
  
     if(nbr_args<2)
       err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with too few arguments"); 
    
      
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
        c_typ=var2->val.lp[0];      
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

    	    styp=(expr ? "method":"function");

            if(expr)
              vtr_args.push_back(expr);

            if(tr=fargs->getFirstChild()) {
              do  
	       vtr_args.push_back(tr);
              while(tr=tr->getNextSibling());    
            } 
      
            nbr_args=vtr_args.size();  

            if(nbr_args==0)
              err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 

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
                     serr="Argument \""+vtr_args[idx]->getText()+"\" to"+sfnm +" is not a dimension";      
                     wrn_prn(fnc_nm,serr);
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
	        wrn_prn(fnc_nm, "Unrecognized dimension arguments in" +sfnm);

	      if(dmn_vtr.size() < nbr_dim ) {
                ostringstream os; 
	        os<<"You have only specified "<< dmn_vtr.size()<< " dimension  args " << "in "+sfnm+". You need to specify  "<< nbr_dim<<". All of the variables dimensions must be specfifed in the arguments."; 
              err_prn(fnc_nm,os.str());
                }

              // Check location of record dimension
              for(idx=0 ; idx<nbr_dim ; idx++)
		if( idx>0 && dmn_vtr[idx]->is_rec_dmn){
                  ostringstream os; 
		  os<<"You must specify the record dimension "<< dmn_vtr[idx]->nm <<" as the first dimension in the list." << sfnm; 
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
    int nbr_fargs;
    prs_cls* prs_arg=walker.prs_arg;
    var_sct *var=NULL_CEWI;
    var_sct *var_msk=NULL_CEWI;
    std::string styp;
    RefAST tr;

    std::string sfnm =fmc_obj.fnm(); //method name

    styp=(expr ? "method":"function");

    //n.b fargs is an imaginary node -and is ALWAYS present
    nbr_fargs=fargs->getNumberOfChildren();
   
    if(expr)
      nbr_fargs++;

   
    if(nbr_fargs >2) 
      wrn_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with too many arguments"); 
  
    if(nbr_fargs<2)
      err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with too few arguments"); 
    
     

    if(expr){ 
      var=walker.out(expr);
      var_msk=walker.out(fargs->getFirstChild());
    }else{
      var=walker.out(fargs->getFirstChild());   
      var_msk=walker.out(fargs->getFirstChild()->getNextSibling());

    }


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
      os<< styp+" \""+sfnm+"\" cannot clip var  ";
      os<< var->nm <<" as size(" <<var->sz<<") is not divisible by mask var "<<var_msk->nm <<" size("<< var_msk->sz <<")"; 
      err_prn(fnc_nm,os.str()); 
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
      long *lp;
      long var_sz;
      long msk_sz;
      size_t slb_sz;
      var_sct *var_out;

      var_msk=nco_var_cnf_typ(NC_INT,var_msk);     
      
      var_out=nco_var_dpl(var_msk);
      var_out=nco_var_cnf_typ(var->type,var_out);

      // De-Reference 
      (void)cast_void_nctype(NC_INT,&var_msk->val);
      lp=var_msk->val.lp;

      msk_sz=var_msk->sz;
      var_sz=var->sz;
      slb_sz=nco_typ_lng(var->type);

      cp_out=(char*)(var_out->val.vp);
      
      for(idx=0 ; idx<msk_sz ;idx++){
        // index not out of bounds bomb out

        if(prs_arg->FORTRAN_IDX_CNV) {

          if( lp[idx]<1L || lp[idx] > var_sz){
            std::ostringstream os;
            os<< styp+" \""+sfnm+"\" reporting that fortran index "<<lp[idx]<<" into "<<var->nm<<" is out of bounds 1"<<"-"<<var_sz; 
            err_prn(fnc_nm,os.str());         
          }
	  --lp[idx];
        }else{
         
          if( lp[idx]<0L || lp[idx] >= var_sz){
            std::ostringstream os;
            os<< styp+" \""+sfnm+"\" reporting that index "<<lp[idx]<<" into "<<var->nm<<" is out of bounds 0"<<"-"<<var_sz-1; 
            err_prn(fnc_nm,os.str());         

          }
        }

        cp_in=(char*)(var->val.vp)+ (size_t)lp[idx]*slb_sz;
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
    int nbr_fargs; 
    int fdx=fmc_obj.fdx();   //index
    prs_cls *prs_arg=walker.prs_arg;    
    var_sct *var_in=NULL_CEWI;
    var_sct *var_out=NULL_CEWI;
    nc_type typ;
    nco_bool PCK_VAR_WITH_NEW_PCK_ATT;
    
    std::string serr; 
    std::string sfnm;
    RefAST tr;
  
    //n.b fargs is an imaginary node -and is ALWAYS present
    nbr_fargs=fargs->getNumberOfChildren(); 
  
    sfnm= (expr ? " method ": " function ") + fmc_obj.fnm(); 

    // no arguments - bomb out
    if(!expr && nbr_fargs==0){    
	serr=sfnm + " has been called without an argument";               
        err_prn(fnc_nm,serr);
    }

   

    if(expr)
      tr=expr;
    else 
      tr=fargs->getFirstChild();

    var_in=walker.out(tr);

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
	  var_att=ncap_att_get(var_in->id,var_in->nm,"scale_factor",prs_arg);
          if(var_att == (var_sct*)NULL) 
	    var_att=ncap_att_get(var_in->id,var_in->nm,"add_offset",prs_arg);	

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
          fmc_vtr.push_back( fmc_cls("sort" , this,PSORT)); 
          fmc_vtr.push_back( fmc_cls("dsort" , this,PDSORT)); 

			     		      
    }
  }
  
  var_sct * srt_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("srt_cls::fnd");
    int nbr_args;
    int fdx=fmc_obj.fdx();
    var_sct *var1=NULL_CEWI;
    var_sct *var2=NULL_CEWI;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string styp;
    std::string var_nm;
    std::string susg;
    prs_cls *prs_arg=walker.prs_arg;    
    RefAST tr;
    std::vector<RefAST> vtr_args; 

    styp=(expr ? "method":"function");

    if(expr)
      vtr_args.push_back(expr);

    if(tr=fargs->getFirstChild()) {
      do  
        vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
    nbr_args=vtr_args.size();  



           
    if( fdx== PDSORT) {
      if(nbr_args<2 && expr)
        err_prn(sfnm,"method requires one argument"); 

      if(nbr_args<2 && !expr)
        err_prn(sfnm,"function requires two arguments"); 

      var1=walker.out(vtr_args[0]);
      var2=walker.out(vtr_args[1]);
      var2=nco_var_cnf_typ(NC_INT, var2);
      
      if(prs_arg->ntl_scn){
        var2=nco_var_free(var2);
        return var1;    
      }
    }   
    
       
    if(fdx==PSORT) {
        
      susg="usage: var_out=sort(var_exp,&var_map)\n";  
      if(nbr_args==0)
        err_prn(sfnm,styp+" has been called with no arguments"); 
         
      var1=walker.out(vtr_args[0]);
       
      if(nbr_args>1){
       if(vtr_args[1]->getType() != CALL_REF ) 
         err_prn(sfnm," second argument must be a call by reference variable\n"+susg);   
       var_nm=vtr_args[1]->getFirstChild()->getText(); 
       var2=prs_arg->ncap_var_init(var_nm,true); 
      }
      if(prs_arg->ntl_scn){
	if(var2) 
          prs_arg->ncap_var_write(var2,false);
        return var1;
      } 
    }

    switch(fdx) {
             
      case PSORT:
	   if(var2==NULL){
	     var1=ncap_var_var_op(var1,(var_sct*)NULL,VSORT);  
             break;   
           }

           // convert map to type int
           var2=nco_var_cnf_typ(NC_INT, var2);  

           // check if map is large enough  
           if( var2->sz < var1->sz) {
             ostringstream os; 
	     os<<"Size of map  "<<var_nm<<"("<< var2->sz<<") is less than size of var(" << var1->sz<<")";
             err_prn(sfnm,os.str());
           }

           switch (var1->type) {
             case NC_DOUBLE: 
	        (void)ncap_sort_and_map<double>(var1,var2);    
                break;  
             case NC_FLOAT: 
	        (void)ncap_sort_and_map<float>(var1,var2);    
                break;  
             case NC_INT: 
	        (void)ncap_sort_and_map<nco_int>(var1,var2);    
                break;  
             case NC_SHORT: 
	        (void)ncap_sort_and_map<nco_short>(var1,var2);    
                break;  
             case NC_USHORT: 
	        (void)ncap_sort_and_map<nco_ushort>(var1,var2);    
                break;  
             case NC_UINT: 
	        (void)ncap_sort_and_map<nco_uint>(var1,var2);    
                break;  
             case NC_INT64: 
	        (void)ncap_sort_and_map<nco_int64>(var1,var2);    
                break;  
             case NC_UINT64: 
	        (void)ncap_sort_and_map<nco_uint64>(var1,var2);    
                break;  
             case NC_BYTE: 
	        (void)ncap_sort_and_map<nco_byte>(var1,var2);    
                break;  
             case NC_UBYTE: 
	        (void)ncap_sort_and_map<nco_ubyte>(var1,var2);    
                break;  
             case NC_CHAR: 
	        (void)ncap_sort_and_map<char>(var1,var2);    
                break;  
             case NC_STRING: break; /* Do nothing */
             
            default: nco_dfl_case_nc_type_err(); break;
            
           } // end big switch
 
           // Write out mapping
           (void)prs_arg->ncap_var_write(var2,false);             
           break; 


      case PDSORT:{
          char *cp_in;
          char *cp_out;
          long idx; 
          long jdx;
          long sz; 
          long sz_idx;
          long slb_sz;
          long *lp_mp; 
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

          // var2 contains the mapping
          (void)cast_void_nctype(NC_INT,&var2->val);
          lp_mp=var2->val.lp; 
         
          for(idx=0; idx<sz_idx; idx++){ 
            cp_out=(char*)var_out->val.vp+ (ptrdiff_t)idx*sz*slb_sz;              
            for(jdx=0 ;jdx<sz; jdx++){
              // do bounds checking for the mapping
	      if(lp_mp[jdx] !=jdx && lp_mp[jdx] >=0 && lp_mp[jdx]< sz )
                // copy element from var1 to var_out
                (void)memcpy(cp_out+(ptrdiff_t)(lp_mp[jdx]*slb_sz),cp_in,slb_sz); 
	      cp_in+=(ptrdiff_t)slb_sz;
	    } // end jdx
          } //end idx; 
          
         
          var1=nco_var_free(var1);
          (void)cast_nctype_void(NC_INT,&var2->val);
          var2=nco_var_free(var2);
             
          var1=var_out;

 
       } break;  



    }


    return var1;


  }  
