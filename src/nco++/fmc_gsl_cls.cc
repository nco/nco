/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--2009 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

// Standard C++ headers

#include <assert.h> 
#include <gsl/gsl_errno.h>
#include <gsl/gsl_mode.h>
#include <gsl/gsl_sf.h>
#include "fmc_gsl_cls.hh"


//gsl Functions /******************************************/

  gsl_cls::gsl_cls(bool flg_dbg){
    
       
    //Populate only on first constructor call
    if(fmc_vtr.empty()){

      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai",f_unn(gsl_sf_airy_Ai_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi",f_unn(gsl_sf_airy_Bi_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai_scaled",f_unn(gsl_sf_airy_Ai_scaled_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi_scaled",f_unn(gsl_sf_airy_Bi_scaled_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai_deriv",f_unn(gsl_sf_airy_Ai_deriv_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi_deriv",f_unn(gsl_sf_airy_Bi_deriv_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai_deriv_scaled",f_unn(gsl_sf_airy_Ai_deriv_scaled_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi_deriv_scaled",f_unn(gsl_sf_airy_Bi_deriv_scaled_e),hnd_fnc_dm));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Ai",f_unn(gsl_sf_airy_zero_Ai_e),hnd_fnc_x,NC_UINT));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Bi",f_unn(gsl_sf_airy_zero_Bi_e),hnd_fnc_x,NC_UINT));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Ai_deriv",f_unn(gsl_sf_airy_zero_Ai_deriv_e),hnd_fnc_x,NC_UINT));  
      gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Bi_deriv",f_unn(gsl_sf_airy_zero_Bi_deriv_e),hnd_fnc_x,NC_UINT));  



      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_J0",f_unn(gsl_sf_bessel_J0_e),hnd_fnc_x,NC_DOUBLE));



      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_J1",f_unn(gsl_sf_bessel_J1_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Jn",f_unn(gsl_sf_bessel_Jn_e),hnd_fnc_xd,NC_INT));
      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Jn_array",f_unn(gsl_sf_bessel_Jn_array),hnd_fnc_iidpd));

      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_j0",f_unn(gsl_sf_bessel_j0_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_j1",f_unn(gsl_sf_bessel_j1_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_j2",f_unn(gsl_sf_bessel_j2_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_jl_array",f_unn(gsl_sf_bessel_jl_array),hnd_fnc_idpd));


      gpr_vtr.push_back(gpr_cls("gsl_sf_taylorcoeff",f_unn(gsl_sf_taylorcoeff_e),hnd_fnc_xd,NC_INT));

      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_zero_J0",f_unn(gsl_sf_bessel_zero_J0_e),hnd_fnc_x,NC_UINT));


      gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Knu",f_unn(gsl_sf_bessel_Knu_e),hnd_fnc_x,NC_DOUBLE));

     
      gpr_vtr.push_back(gpr_cls("gsl_sf_lngamma",f_unn(gsl_sf_lngamma_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_gamma",f_unn(gsl_sf_gamma_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_gammastar",f_unn(gsl_sf_gammastar_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_gammainv",f_unn(gsl_sf_gammainv_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc_Q",f_unn(gsl_sf_gamma_inc_Q_e),hnd_fnc_xd,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc_P",f_unn(gsl_sf_gamma_inc_P_e),hnd_fnc_xd,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc",f_unn(gsl_sf_gamma_inc_e),hnd_fnc_xd,NC_DOUBLE));
       
      

      for(unsigned idx=0; idx<gpr_vtr.size(); idx++)      
        fmc_vtr.push_back(fmc_cls(gpr_vtr[idx].fnm(),this,idx));


    }
  }

  var_sct * gsl_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_cls::fnd");
            bool is_mtd;
            int idx;
            int fdx;
            int nbr_args;
            var_sct *var_ret=NULL_CEWI;

            RefAST tr;
            std::vector<RefAST> args_vtr; 
            // function pointer
            var_sct* (*hnd_fnc)(HANDLE_ARGS);
            
            
            // de-reference 

            fdx=fmc_obj.fdx();
 

            is_mtd=(expr ? true: false);

 
            // Put args into vector 
            if(expr)
               args_vtr.push_back(expr);

            if(tr=fargs->getFirstChild()) {
              do  
	        args_vtr.push_back(tr);
              while(tr=tr->getNextSibling());    
            } 
                  
            hnd_fnc=gpr_vtr[fdx]._hnd_fnc;

            // gsl_set_error_handler_off(); /* dont abort when error */
            var_ret=hnd_fnc(is_mtd,args_vtr,gpr_vtr[fdx],walker);
             
            return var_ret; 

  }


var_sct *hnd_fnc_x(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_x");
            int idx;
            int args_nbr;
            nc_type type;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
            var_sct *var=NULL_CEWI;
            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;

            args_nbr=args_vtr.size();
            
            type=gpr_obj.type(); 

            if(args_nbr==0)
              err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 

                 
            var=walker.out(args_vtr[0]);

            
            if(prs_arg->ntl_scn){
              if(!var->undefined)
                var=nco_var_cnf_typ(NC_DOUBLE,var);                     
	      return var;
            }
            
            switch(type){ 

	    case NC_UINT:
            { 
	     bool has_mss_val;
             int sz=var->sz;
             double mss_val_dbl; 
             long *lp;
             double *dp;
              
             var_sct *var_out;
              
             gsl_sf_result rslt;  /* structure for result from gsl lib call */

             int (*fnc_int)(unsigned int, gsl_sf_result*);

	     fnc_int=gpr_obj.g_args().au; 
               
             var_out=nco_var_dpl(var);

             var_out=nco_var_cnf_typ(NC_DOUBLE,var_out);                     
             (void)cast_void_nctype(NC_DOUBLE,&(var_out->val));
             dp=var_out->val.dp;  
             has_mss_val=false;
             if(var_out->has_mss_val){
               has_mss_val=true;
               (void)cast_void_nctype(NC_DOUBLE,&(var_out->mss_val));
               mss_val_dbl=var_out->mss_val.dp[0];    
              (void)cast_nctype_void(NC_DOUBLE,&(var_out->mss_val));
             }
                   
 
             // convert to int
             var=nco_var_cnf_typ(NC_INT,var);                     
              
             (void)cast_void_nctype(NC_INT,&(var->val));
             lp=var->val.lp;  
 
             if(has_mss_val){  
      	       for(idx=0;idx<sz;idx++)
        	 if(dp[idx] != mss_val_dbl)
                     // note fnc_int return status flag, if 0 then no error occured
                   dp[idx]=( (*fnc_int)(lp[idx],&rslt) ? mss_val_dbl : rslt.val );             
             }else{
      	         for(idx=0;idx<sz;idx++) 
                   dp[idx]=( (*fnc_int)(lp[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
             }


             (void)cast_nctype_void(NC_DOUBLE,&(var_out->val));
             (void)cast_nctype_void(NC_INT,&(var->val));

             nco_var_free(var);
             var=var_out; 

	    } 
            break;  


	    case NC_INT:
            { 
	     bool has_mss_val;
             int sz=var->sz;
             double mss_val_dbl; 
             long *lp;
             double *dp;
              
             var_sct *var_out;
              
             gsl_sf_result rslt;  /* structure for result from gsl lib call */

             int (*fnc_int)(int, gsl_sf_result*);

	     fnc_int=gpr_obj.g_args().ai; 
               
             var_out=nco_var_dpl(var);

             var_out=nco_var_cnf_typ(NC_DOUBLE,var_out);                     
             (void)cast_void_nctype(NC_DOUBLE,&(var_out->val));
             dp=var_out->val.dp;  
             has_mss_val=false;
             if(var_out->has_mss_val){
               has_mss_val=true;
               (void)cast_void_nctype(NC_DOUBLE,&(var_out->mss_val));
               mss_val_dbl=var_out->mss_val.dp[0];    
              (void)cast_nctype_void(NC_DOUBLE,&(var_out->mss_val));
             }
                   
 
             // convert to int
             var=nco_var_cnf_typ(NC_INT,var);                     
              
             (void)cast_void_nctype(NC_INT,&(var->val));
             lp=var->val.lp;  
 
             if(has_mss_val){  
      	       for(idx=0;idx<sz;idx++)
        	 if(dp[idx] != mss_val_dbl)
                     // note fnc_int return status flag, if 0 then no error occured
                   dp[idx]=( (*fnc_int)(lp[idx],&rslt) ? mss_val_dbl : rslt.val );             
             }else{
      	         for(idx=0;idx<sz;idx++) 
                   dp[idx]=( (*fnc_int)(lp[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
             }


             (void)cast_nctype_void(NC_DOUBLE,&(var_out->val));
             (void)cast_nctype_void(NC_INT,&(var->val));

             nco_var_free(var);
             var=var_out; 

	    } 
            break;  



	    case NC_DOUBLE:
            
            { 
	     bool has_mss_val;
             int sz=var->sz;
             double mss_val_dbl;
             double *dp;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */

             int (*fnc_int)(double, gsl_sf_result*);


	     fnc_int=gpr_obj.g_args().ad; 
             // convert to double
             var=nco_var_cnf_typ(NC_DOUBLE,var);                     
             

             (void)cast_void_nctype(NC_DOUBLE,&(var->val));
             dp=var->val.dp;  
          
             has_mss_val=false;
             if(var->has_mss_val){
               has_mss_val=true;
               (void)cast_void_nctype(NC_DOUBLE,&(var->mss_val));
               mss_val_dbl=var->mss_val.dp[0];    
             }
 
             if(has_mss_val){  
      	       for(idx=0;idx<sz;idx++)
        	 if(dp[idx] != mss_val_dbl)
                     // note fnc_int return status flag, if 0 then no error occured
                   dp[idx]=( (*fnc_int)(dp[idx],&rslt) ? mss_val_dbl : rslt.val );             
             }else{
      	         for(idx=0;idx<sz;idx++) 
                   dp[idx]=( (*fnc_int)(dp[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
             }


             (void)cast_nctype_void(NC_DOUBLE,&(var->val));
             if(var->has_mss_val) 
               (void)cast_nctype_void(NC_DOUBLE,&(var->mss_val));
            } 
            break;
            
            default:
	      break; 

            

            } // end big switch 

          return var;


} // end hnd_fnc_x



var_sct *hnd_fnc_xd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){ 
  const std::string fnc_nm("hnd_fnc_dd");
            int idx;
            int args_nbr;
            nc_type type;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
            var_sct *var1=NULL_CEWI;
            var_sct *var2=NULL_CEWI;
            var_sct *var_ret=NULL_CEWI;
          
            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;
            type=gpr_obj.type();  

            args_nbr=args_vtr.size();
            

            if(args_nbr <2)
              if(is_mtd)
                err_prn(fnc_nm,styp+" \""+sfnm+"\" requires one argument. None given"); 
              else
                err_prn(fnc_nm,styp+" \""+sfnm+"\" requires two arguments");    
                 
            var1=walker.out(args_vtr[0]);
            var2=walker.out(args_vtr[1]);  

             

              
            // Deal with initial scan
            if(prs_arg->ntl_scn){

              if(var1->undefined || var2->undefined){
	       var1=nco_var_free(var1);
               var2=nco_var_free(var2);
               var_ret=ncap_var_udf("~gsl_cls");
              }else {
               var_ret=ncap_var_att_cnf_ntl(var2,var1);
               var_ret=nco_var_cnf_typ(NC_DOUBLE,var_ret);
              } 
      	      return var_ret;
            } 



            switch(type){

	      // Unimplemented. Currently there are no gsl functions 
              // with the signature (unsigned int, double,gsl_sf_result)
            case NC_UINT:
	      break; 
                 
            case NC_DOUBLE: {
 
	     bool has_mss_val=false;
             int sz;
             double *dp1;
             double *dp2; 
             double mss_val_dbl;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */
             int (*fnc_int)(double, double, gsl_sf_result*);

	     fnc_int=gpr_obj.g_args().add; 


            // convert to type double
            var1=nco_var_cnf_typ(NC_DOUBLE,var1); 
            var2=nco_var_cnf_typ(NC_DOUBLE,var2); 
            // make operands conform  
            ncap_var_att_cnf(var2,var1);

            sz=var1->sz;



             (void)cast_void_nctype(NC_DOUBLE,&(var1->val));
             dp1=var1->val.dp;  
             (void)cast_void_nctype(NC_DOUBLE,&(var2->val));
             dp2=var2->val.dp;  


             if(var1->has_mss_val){
                has_mss_val=true; 
                (void)cast_void_nctype(NC_DOUBLE,&var1->mss_val);
                mss_val_dbl=*var1->mss_val.dp;
                (void)cast_nctype_void(NC_DOUBLE,&(var1->mss_val));
              }else if(var2->has_mss_val){
                has_mss_val=true; 
                (void)cast_void_nctype(NC_DOUBLE,&var2->mss_val);
                mss_val_dbl=*var2->mss_val.dp;
                (void)cast_nctype_void(NC_DOUBLE,&(var2->mss_val));
              }


             if(!has_mss_val){ 
      	         for(idx=0;idx<sz;idx++) 
                   dp2[idx]=( (*fnc_int)(dp1[idx],dp2[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	       }else{
      		 
      		 for(idx=0;idx<sz;idx++)
                   // note fnc_int return status flag, if 0 then no error
        	   if(dp1[idx] == mss_val_dbl || 
                      dp2[idx] == mss_val_dbl ||
                      (*fnc_int)(dp1[idx],dp2[idx], &rslt)
		      ) dp2[idx]=mss_val_dbl;
		   else
                       dp2[idx]=rslt.val;      
	       }           

               (void)cast_nctype_void(NC_DOUBLE,&(var1->val));
               (void)cast_nctype_void(NC_DOUBLE,&(var2->val));
               nco_var_free(var1);
	    } break;


            case NC_INT: { 
	     bool has_mss_val=false;
             int sz;
             int mss_val_lp; 
             long *lp1; 
             double *dp2;
             
             double mss_val_dbl;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */
             int (*fnc_int)(int,double, gsl_sf_result*);

             
             // convert to type double
             var1=nco_var_cnf_typ(NC_INT,var1); 
             var2=nco_var_cnf_typ(NC_DOUBLE,var2); 
               
             // make operands conform  
             ncap_var_att_cnf(var2,var1);

             sz=var1->sz; 


	     fnc_int=gpr_obj.g_args().aid; 


             (void)cast_void_nctype(NC_INT,&(var1->val));
             lp1=var1->val.lp;  
             (void)cast_void_nctype(NC_DOUBLE,&(var2->val));
             dp2=var2->val.dp;  


 
             // Ignore for now missing values from first operand
             if(var1->has_mss_val){
	       //has_mss_val=true; 
                (void)cast_void_nctype(NC_INT,&var1->mss_val);
                mss_val_lp=*var1->mss_val.lp;
                (void)cast_nctype_void(NC_INT,&(var1->mss_val));
              } 

              if(var2->has_mss_val){
                has_mss_val=true; 
                (void)cast_void_nctype(NC_DOUBLE,&var2->mss_val);
                mss_val_dbl=*var2->mss_val.dp;
                (void)cast_nctype_void(NC_DOUBLE,&(var2->mss_val));
              }
 

             if(!has_mss_val){ 
      	         for(idx=0;idx<sz;idx++) 
                   // note fnc_int returns status flag, if 0 then no error
                   dp2[idx]=( (*fnc_int)(lp1[idx],dp2[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	       }else{
      		 
      		 for(idx=0;idx<sz;idx++)
                   // note fnc_int returns status flag, if 0 then no error
        	   if(dp2[idx] == mss_val_dbl || (*fnc_int)(lp1[idx],dp2[idx], &rslt) )
		       dp2[idx]=mss_val_dbl;
		   else
                       dp2[idx]=rslt.val;      
	       }           

               (void)cast_nctype_void(NC_INT,&(var1->val));
               (void)cast_nctype_void(NC_DOUBLE,&(var2->val));
               nco_var_free(var1);

	    } break;


            default:
	      break;

	    } // end big switch 

            return var2;
               
} // end function hnd_fnc_xd




var_sct *hnd_fnc_iidpd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){ 
  const std::string fnc_nm("hnd_fnc_iidpd");
            int idx;
            int args_nbr;
            vtl_typ lcl_typ;
	    std::string serr;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
	    std::string susg;        // usage string;   
            var_sct *var_arr[3];
            var_sct *var_out=NULL_CEWI;
            var_sct *var_tmp=NULL_CEWI;
          
            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;

            args_nbr=args_vtr.size();
            
            susg="usage: var_out="+sfnm+"(int nmin, int nmax, double x, var_in)";

            if(args_nbr <4)
              if(is_mtd)
                err_prn(sfnm,styp+" requires three arguments\n"+susg); 
              else
                err_prn(sfnm,styp+" requires four arguments\n"+susg);    
             
              
            // Deal with initial scan 
            // nb this method returns an int which is the gsl status flag;
            if(prs_arg->ntl_scn){
              // evaluate args for side effect 
              for(idx=0 ; idx<args_nbr ; idx++){
                var_tmp=walker.out(args_vtr[idx]);     
		var_tmp=nco_var_free(var_tmp);     
	      }

	      var_out=walker.out(args_vtr[3]);
              var_out=nco_var_cnf_typ(NC_DOUBLE,var_out); 
              return var_out;
      	      //return ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nc_type)NC_INT,false);        ;
            } 
               

            // Do the real thing 
            // nb the args are fnc_int(int nmin, int nmax, double x, double * result_array);

            // get the first three args                
            for(idx=0 ; idx<3 ; idx++)
              var_arr[idx]=walker.out(args_vtr[idx]);     

            // check the fourth argument, it can only be a var or att identifier
	    // nb this is a static method
	    lcl_typ=vtl_cls::expr_typ(args_vtr[3]);          

           if(lcl_typ !=VVAR ) {
             serr="The last argument must  be a variable identifer\n";
             err_prn(sfnm,serr+susg);
           }

          


            // do heavy listing; 
            {
	    int nbr_min;
            int nbr_max;
            int iret;
            int sz_out;
 
            double xin;     
            double *dp_out;
	    std::string va_nm;
            int (*fnc_int)(int,int,double, double*);
             
            fnc_int=gpr_obj.g_args().biidpd; 
             
            va_nm=args_vtr[3]->getText();

   
            var_out=prs_arg->ncap_var_init(va_nm,true);

            var_arr[0]=nco_var_cnf_typ(NC_INT,var_arr[0]);              
            (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));
            nbr_min=var_arr[0]->val.lp[0];    
            (void)cast_nctype_void(NC_INT,&(var_arr[0]->val));


            var_arr[1]=nco_var_cnf_typ(NC_INT,var_arr[1]);              
            (void)cast_void_nctype(NC_INT,&(var_arr[1]->val));
            nbr_max=var_arr[1]->val.lp[0];    
            (void)cast_nctype_void(NC_INT,&(var_arr[1]->val));


            var_arr[2]=nco_var_cnf_typ(NC_DOUBLE,var_arr[2]);              
            (void)cast_void_nctype(NC_DOUBLE,&(var_arr[2]->val));
            xin=var_arr[2]->val.dp[0];    
            (void)cast_nctype_void(NC_DOUBLE,&(var_arr[2]->val));

            sz_out=nbr_max-nbr_min+1;

	    // check int argument
	    if(sz_out<1 )  
	      err_prn(sfnm,"nmax must be greater than or equal to nmin\n"+susg);    

            var_out=nco_var_cnf_typ(NC_DOUBLE,var_out);              
            (void)cast_void_nctype(NC_DOUBLE,&(var_out->val));
            dp_out=var_out->val.dp;    
            (void)cast_nctype_void(NC_DOUBLE,&(var_out->val));


	    if(sz_out>var_out->sz ){
              serr="Size("+nbr2sng(sz_out)+ ") of result greater than variable size("+nbr2sng(var_out->sz)+")\n"; 
	      err_prn(sfnm,serr+susg);    
            }

            // Call the gsl function  
            iret=fnc_int(nbr_min,nbr_max,xin,dp_out); 
            //(void)prs_arg->ncap_var_write(var_out,false);           
             
            // Free args                
            for(idx=0 ; idx<3 ; idx++)
              (void)nco_var_free(var_arr[idx]);

            //var_tmp=ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nco_int)iret);
            

            }  
            return var_out;


} // end function hnd_fnc_iidpd


var_sct *hnd_fnc_idpd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){ 
  const std::string fnc_nm("hnd_fnc_iidpd");
            int idx;
            int args_nbr;
            vtl_typ lcl_typ;
	    std::string serr;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
	    std::string susg;        // usage string;   
            var_sct *var_arr[2];
            var_sct *var_out=NULL_CEWI;
            var_sct *var_tmp=NULL_CEWI;
          
            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;

            args_nbr=args_vtr.size();
            
            susg="usage: var_out="+sfnm+"(int lmax, double x, var_in)";

            if(args_nbr <3)
              if(is_mtd)
                err_prn(sfnm,styp+" requires two arguments\n"+susg); 
              else
                err_prn(sfnm,styp+" requires three arguments\n"+susg);    
             
              
            // Deal with initial scan 
            // nb this method returns an int which is the gsl status flag;
            if(prs_arg->ntl_scn){
              // evaluate args for side effect 
              for(idx=0 ; idx<2 ; idx++){
                var_tmp=walker.out(args_vtr[idx]);     
		var_tmp=nco_var_free(var_tmp);     
	      }

	      var_out=walker.out(args_vtr[2]);
              var_out=nco_var_cnf_typ(NC_DOUBLE,var_out); 
              return var_out;
      	      //return ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nc_type)NC_INT,false);        ;
            } 
               

            // Do the real thing 
            // nb the args are fnc_int(int lmax, double x, double * result_array);

            // get the first two args                
            for(idx=0 ; idx<2 ; idx++)
              var_arr[idx]=walker.out(args_vtr[idx]);     

            // check the third argument, it can only be a var identifier
	    // nb this is a static method
	    lcl_typ=vtl_cls::expr_typ(args_vtr[2]);          

           if(lcl_typ !=VVAR ) {
             serr="The last argument must  be a variable identifer\n";
             err_prn(sfnm,serr+susg);
           }

          


            // do heavy listing; 
            {
            int nbr_max;
            int iret;
            int sz_out;
 
            double xin;     
            double *dp_out;
	    std::string va_nm;
            int (*fnc_int)(int,double, double*);
             
            fnc_int=gpr_obj.g_args().bidpd; 
             
            va_nm=args_vtr[2]->getText();

   
            var_out=prs_arg->ncap_var_init(va_nm,true);

            var_arr[0]=nco_var_cnf_typ(NC_INT,var_arr[0]);              
            (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));
            nbr_max=var_arr[0]->val.lp[0];    
            (void)cast_nctype_void(NC_INT,&(var_arr[0]->val));


            var_arr[1]=nco_var_cnf_typ(NC_DOUBLE,var_arr[1]);              
            (void)cast_void_nctype(NC_DOUBLE,&(var_arr[1]->val));
            xin=var_arr[1]->val.dp[0];    
            (void)cast_nctype_void(NC_DOUBLE,&(var_arr[1]->val));

            sz_out=nbr_max+1;

	    // check int argument
	    if(sz_out<1 )  
	      err_prn(sfnm,"lmax must be greater than or equal to zero\n"+susg);    

            var_out=nco_var_cnf_typ(NC_DOUBLE,var_out);              
            (void)cast_void_nctype(NC_DOUBLE,&(var_out->val));
            dp_out=var_out->val.dp;    
            (void)cast_nctype_void(NC_DOUBLE,&(var_out->val));


	    if(sz_out>var_out->sz ){
              serr="Size("+nbr2sng(sz_out)+ ") of result greater than variable size("+nbr2sng(var_out->sz)+")\n"; 
	      err_prn(sfnm,serr+susg);    
            }

            // Call the gsl function  
            iret=fnc_int(nbr_max,xin,dp_out); 
            //(void)prs_arg->ncap_var_write(var_out,false);           
             
            // Free args                
            (void)nco_var_free(var_arr[0]);
            (void)nco_var_free(var_arr[1]);

            //var_tmp=ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nco_int)iret);
            

            }  
            return var_out;


} // end function hnd_fnc_iidpd


var_sct *hnd_fnc_dm(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_d");
            int idx;
            int args_nbr;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
            var_sct *var=NULL_CEWI;
            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;

            args_nbr=args_vtr.size();
            

            if(args_nbr==0)
              err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 

                 
            var=walker.out(args_vtr[0]);
             
            
            if(prs_arg->ntl_scn) {
	      if(!var->undefined)
               var=nco_var_cnf_typ(NC_DOUBLE,var);                  
	      return var;
            }


            // do heavy lifting 
           { 
	     bool has_mss_val;
             int sz=var->sz;
             double mss_val_dbl;
             double *dp;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */
             gsl_mode_t mde_t=GSL_PREC_DOUBLE;
             int (*fnc_int)(double, gsl_mode_t, gsl_sf_result*);


	     fnc_int=gpr_obj.g_args().adm; 
             // convert to double
             var=nco_var_cnf_typ(NC_DOUBLE,var);                     
             

             (void)cast_void_nctype(NC_DOUBLE,&(var->val));
             dp=var->val.dp;  
          
             has_mss_val=false;
             if(var->has_mss_val){
               has_mss_val=true;
               (void)cast_void_nctype(NC_DOUBLE,&(var->mss_val));
               mss_val_dbl=var->mss_val.dp[0];    
               (void)cast_nctype_void(NC_DOUBLE,&(var->mss_val));
             }
 
             if(has_mss_val){  
      	       for(idx=0;idx<sz;idx++)
        	 if(dp[idx] != mss_val_dbl)
                     // note fnc_int return status flag, if 0 then no error occured
                   dp[idx]=( (*fnc_int)(dp[idx],mde_t,&rslt) ? mss_val_dbl : rslt.val );             
             }else{
      	         for(idx=0;idx<sz;idx++) 
                   dp[idx]=( (*fnc_int)(dp[idx],mde_t,&rslt) ? NC_FILL_DOUBLE : rslt.val );
             }


             (void)cast_nctype_void(NC_DOUBLE,&(var->val));


	   } 
            





            return var;


 ; }

