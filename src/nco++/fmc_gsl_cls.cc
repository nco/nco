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

      // Airy Functions *******************************************************************************************/
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

      // Bessel Functions *******************************************************************************************/
        gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_J0",f_unn(gsl_sf_bessel_J0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_J1",f_unn(gsl_sf_bessel_J1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Jn",f_unn(gsl_sf_bessel_Jn_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Jn_array",f_unn(gsl_sf_bessel_Jn_array),hnd_fnc_iidpd,PBESSEL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Y0",f_unn(gsl_sf_bessel_Y0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Y1",f_unn(gsl_sf_bessel_Y1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Yn",f_unn(gsl_sf_bessel_Yn_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Yn_array",f_unn(gsl_sf_bessel_Yn_array),hnd_fnc_iidpd,PBESSEL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_I0",f_unn(gsl_sf_bessel_I0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_I1",f_unn(gsl_sf_bessel_I1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_In",f_unn(gsl_sf_bessel_In_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_In_array",f_unn(gsl_sf_bessel_In_array),hnd_fnc_iidpd,PBESSEL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_I0_scaled",f_unn(gsl_sf_bessel_I0_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_I1_scaled",f_unn(gsl_sf_bessel_I1_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_In_scaled",f_unn(gsl_sf_bessel_In_scaled_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_In_scaled_array",f_unn(gsl_sf_bessel_In_scaled_array),hnd_fnc_iidpd,PBESSEL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_K0",f_unn(gsl_sf_bessel_K0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_K1",f_unn(gsl_sf_bessel_K1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Kn",f_unn(gsl_sf_bessel_Kn_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Kn_array",f_unn(gsl_sf_bessel_Kn_array),hnd_fnc_iidpd,PBESSEL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_K0_scaled",f_unn(gsl_sf_bessel_K0_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_K1_scaled",f_unn(gsl_sf_bessel_K1_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Kn_scaled",f_unn(gsl_sf_bessel_Kn_scaled_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Kn_scaled_array",f_unn(gsl_sf_bessel_Kn_scaled_array),hnd_fnc_iidpd,PBESSEL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_j0",f_unn(gsl_sf_bessel_j0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_j1",f_unn(gsl_sf_bessel_j1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_j2",f_unn(gsl_sf_bessel_j2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_jl",f_unn(gsl_sf_bessel_jl_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_jl_array",f_unn(gsl_sf_bessel_jl_array),hnd_fnc_idpd));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_jl_steed_array",f_unn(gsl_sf_bessel_jl_steed_array),hnd_fnc_idpd));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_y0",f_unn(gsl_sf_bessel_y0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_y1",f_unn(gsl_sf_bessel_y1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_y2",f_unn(gsl_sf_bessel_y2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_yl",f_unn(gsl_sf_bessel_yl_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_yl_array",f_unn(gsl_sf_bessel_yl_array),hnd_fnc_idpd));
 	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_i0_scaled",f_unn(gsl_sf_bessel_i0_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_i1_scaled",f_unn(gsl_sf_bessel_i1_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_i2_scaled",f_unn(gsl_sf_bessel_i2_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_il_scaled",f_unn(gsl_sf_bessel_il_scaled_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_il_scaled_array",f_unn(gsl_sf_bessel_il_scaled_array),hnd_fnc_idpd));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_k0_scaled",f_unn(gsl_sf_bessel_k0_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_k1_scaled",f_unn(gsl_sf_bessel_k1_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_k2_scaled",f_unn(gsl_sf_bessel_k2_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_kl_scaled",f_unn(gsl_sf_bessel_kl_scaled_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_kl_scaled_array",f_unn(gsl_sf_bessel_kl_scaled_array),hnd_fnc_idpd));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Jnu",f_unn(gsl_sf_bessel_Jnu_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Ynu",f_unn(gsl_sf_bessel_Ynu_e),hnd_fnc_xd,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_sequence_Jnu",f_unn(gsl_sf_bessel_sequence_Jnu_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Inu_scaled",f_unn(gsl_sf_bessel_Inu_scaled_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Inu",f_unn(gsl_sf_bessel_Inu_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Knu_scaled",f_unn(gsl_sf_bessel_Knu_scaled_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Knu",f_unn(gsl_sf_bessel_Knu_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_lnKnu",f_unn(gsl_sf_bessel_lnKnu_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_zero_J0",f_unn(gsl_sf_bessel_zero_J0_e),hnd_fnc_x,NC_UINT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_zero_J1",f_unn(gsl_sf_bessel_zero_J1_e),hnd_fnc_x,NC_UINT));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_zero_Jnu",f_unn(gsl_sf_bessel_zero_Jnu_e),));

      // Clausen  Function *******************************************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_clausen",f_unn(gsl_sf_clausen),hnd_fnc_x,NC_DOUBLE));
 
      // Coulomb  Function *******************************************************************************************/

      // Coupling  Coefficents ***************************************************************************************/






      // Dawson  Function *******************************************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_dawson",f_unn(gsl_sf_dawson),hnd_fnc_x,NC_DOUBLE));

      // Debye  Functions *******************************************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_debye_1",f_unn(gsl_sf_debye_1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_debye_2",f_unn(gsl_sf_debye_2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_debye_3",f_unn(gsl_sf_debye_3_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_debye_4",f_unn(gsl_sf_debye_4_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_debye_5",f_unn(gsl_sf_debye_5_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_debye_6",f_unn(gsl_sf_debye_6_e),hnd_fnc_x,NC_DOUBLE));

      // Dilogarithm ****************************************************************************/
	// not implemented as all involve complex numbers
   
      // Elementary Operations  *****************************************************************/

      // Elliptic Integrals  ********************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Kcomp",f_unn(gsl_sf_ellint_Kcomp_e),hnd_fnc_nd,P1DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Ecomp",f_unn(gsl_sf_ellint_Ecomp_e),hnd_fnc_nd,P1DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Pcomp",f_unn(gsl_sf_ellint_Pcomp_e),hnd_fnc_nd,P2DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Dcomp",f_unn(gsl_sf_ellint_Dcomp_e),hnd_fnc_nd,P1DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_F",f_unn(gsl_sf_ellint_F_e),hnd_fnc_nd,P2DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_E",f_unn(gsl_sf_ellint_E_e),hnd_fnc_nd,P2DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_P",f_unn(gsl_sf_ellint_P_e),hnd_fnc_nd,P3DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_D",f_unn(gsl_sf_ellint_D_e),hnd_fnc_nd,P3DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RC",f_unn(gsl_sf_ellint_RC_e), hnd_fnc_nd,P2DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RD",f_unn(gsl_sf_ellint_RD_e),hnd_fnc_nd,P3DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RF",f_unn(gsl_sf_ellint_RF_e),hnd_fnc_nd,P3DBLMD));
	gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RJ",f_unn(gsl_sf_ellint_RJ_e),hnd_fnc_nd,P4DBLMD));

 
      // Elliptic Function (Jacobi)*************************************************************/


      // Error  Functions ***********************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_erfc",f_unn(gsl_sf_erfc_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_log_erfc",f_unn(gsl_sf_log_erfc_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_erf",f_unn(gsl_sf_erf_e),hnd_fnc_x,NC_DOUBLE));
     	gpr_vtr.push_back(gpr_cls("gsl_sf_erf_Z",f_unn(gsl_sf_erf_Z_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_erf_Q",f_unn(gsl_sf_erf_Q_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hazard",f_unn(gsl_sf_hazard_e),hnd_fnc_x,NC_DOUBLE));


      // Exponential  Functions ******************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_exp",f_unn(gsl_sf_exp_e),hnd_fnc_x,NC_DOUBLE));
	//gpr_vtr.push_back(gpr_cls("gsl_sf_exp_e10",f_unn(gsl_sf_exp_10_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult",f_unn(gsl_sf_exp_mult_e),hnd_fnc_xd,NC_DOUBLE));
	//gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult_e10",f_unn(gsl_sf_exp_mult_e10_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_exprel",f_unn(gsl_sf_exprel_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_exprel_2",f_unn(gsl_sf_exprel_2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_exprel_n",f_unn(gsl_sf_exprel_n_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_exp_err",f_unn(gsl_sf_exp_err_e),hnd_fnc_xd,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_exp_err_e10",f_unn(gsl_sf_exp_err_e10_e),hnd_fnc_xd,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult_err_e",f_unn(gsl_sf_exp_mult_err_e),hnd_fnc_xd,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult_err_e10_e",f_unn(gsl_sf_exp_mult_err_e10_e),hnd_fnc_xd,NC_DOUBLE));


       // Exponential Integral Functions *********************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E1",f_unn(gsl_sf_expint_E1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E2",f_unn(gsl_sf_expint_E2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_En",f_unn(gsl_sf_expint_En_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E1_scaled",f_unn(gsl_sf_expint_E1_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E2_scaled",f_unn(gsl_sf_expint_E2_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_En_scaled",f_unn(gsl_sf_expint_En_scaled_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_Ei",f_unn(gsl_sf_expint_Ei_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_Ei_scaled",f_unn(gsl_sf_expint_Ei_scaled_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_Shi",f_unn(gsl_sf_Shi_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_Chi",f_unn(gsl_sf_Chi_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_expint_3",f_unn(gsl_sf_expint_3_e),hnd_fnc_x,NC_DOUBLE));       
	gpr_vtr.push_back(gpr_cls("gsl_sf_Si",f_unn(gsl_sf_Si_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_Ci",f_unn(gsl_sf_Ci_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_atanint",f_unn(gsl_sf_atanint_e),hnd_fnc_x,NC_DOUBLE));

       // Fermi Dirac Functions ******************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_m1",f_unn(gsl_sf_fermi_dirac_m1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_0",f_unn(gsl_sf_fermi_dirac_0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_1",f_unn(gsl_sf_fermi_dirac_1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_2",f_unn(gsl_sf_fermi_dirac_2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_int",f_unn(gsl_sf_fermi_dirac_int_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_mhalf",f_unn(gsl_sf_fermi_dirac_mhalf_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_half",f_unn(gsl_sf_fermi_dirac_half_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_3half",f_unn(gsl_sf_fermi_dirac_3half_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_inc_0",f_unn(gsl_sf_fermi_dirac_inc_0_e),hnd_fnc_xd,NC_DOUBLE));

       // Gamma & Beta Functions *****************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_lngamma",f_unn(gsl_sf_lngamma_e),hnd_fnc_x,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_gammagsl_sf_lngamma_sgn",f_unn(gsl_sf_gammagsl_sf_lngamma_sgn_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gamma",f_unn(gsl_sf_gamma_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gammastar",f_unn(gsl_sf_gammastar_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gammainv",f_unn(gsl_sf_gammainv_e),hnd_fnc_x,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_gammagsl_sf_lngamma_complex",f_unn(gsl_sf_gammagsl_sf_lngamma_complex_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_taylorcoeff",f_unn(gsl_sf_taylorcoeff_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_fact",f_unn(gsl_sf_fact_e),hnd_fnc_x,NC_UINT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_doublefact",f_unn(gsl_sf_doublefact_e),hnd_fnc_x,NC_UINT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_lnfact",f_unn(gsl_sf_lnfact_e),hnd_fnc_x,NC_UINT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_lndoublefact",f_unn(gsl_sf_lndoublefact_e),hnd_fnc_x,NC_UINT));
	//gpr_vtr.push_back(gpr_cls("gsl_sf_lnchoose",f_unn(gsl_sf_lnchoose_e),));
	//gpr_vtr.push_back(gpr_cls("gsl_sf_choose",f_unn(gsl_sf_choose_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_lnpoch",f_unn(gsl_sf_lnpoch_e),hnd_fnc_xd,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_gammagsl_sf_lnpoch_sgn",f_unn(gsl_sf_gammagsl_sf_lnpoch_sgn_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_poch",f_unn(gsl_sf_poch_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_pochrel",f_unn(gsl_sf_pochrel_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc_Q",f_unn(gsl_sf_gamma_inc_Q_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc_P",f_unn(gsl_sf_gamma_inc_P_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc",f_unn(gsl_sf_gamma_inc_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_lnbeta",f_unn(gsl_sf_lnbeta_e),hnd_fnc_xd,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_gammagsl_sf_lnbeta_sgn",f_unn(gsl_sf_gammagsl_sf_lnbeta_sgn_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_beta",f_unn(gsl_sf_beta_e),hnd_fnc_xd,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_beta_inc",f_unn(gsl_sf_beta_inc_e),));





       // Gegenbauer Functions ********************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_1",f_unn(gsl_sf_gegenpoly_1_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_2",f_unn(gsl_sf_gegenpoly_2_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_3",f_unn(gsl_sf_gegenpoly_3_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_n",f_unn(gsl_sf_gegenpoly_n_e),hnd_fnc_idd));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_array",f_unn(gsl_sf_gegenpoly_array),));


       // Hypergeometric Functions ****************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_0F1",f_unn(gsl_sf_hyperg_0F1_e),hnd_fnc_nd,P2DBL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_1F1_int",f_unn(gsl_sf_hyperg_1F1_int_e),hnd_fnc_iid));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_1F1",f_unn(gsl_sf_hyperg_1F1_e),hnd_fnc_nd,P3DBL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_U_int",f_unn(gsl_sf_hyperg_U_int_e),hnd_fnc_iid));
	//gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_U_int_e10",f_unn(gsl_sf_hyperg_U_int_e10_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_U",f_unn(gsl_sf_hyperg_U_e),hnd_fnc_nd,P3DBL));
	//gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_U_e10",f_unn(gsl_sf_hyperg_U_e10_e),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_2F1",f_unn(gsl_sf_hyperg_2F1_e),hnd_fnc_nd,P4DBL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_2F1_conj",f_unn(gsl_sf_hyperg_2F1_conj_e),hnd_fnc_nd,P4DBL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_2F1_renorm",f_unn(gsl_sf_hyperg_2F1_renorm_e),hnd_fnc_nd,P4DBL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_2F1_conj_renorm",f_unn(gsl_sf_hyperg_2F1_conj_renorm_e),hnd_fnc_nd,P4DBL));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hyperg_2F0",f_unn(gsl_sf_hyperg_2F0_e),hnd_fnc_nd,P3DBL));



       // Laguerre Functions **********************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_1",f_unn(gsl_sf_laguerre_1_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_2",f_unn(gsl_sf_laguerre_2_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_3",f_unn(gsl_sf_laguerre_3_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_n",f_unn(gsl_sf_laguerre_n_e),hnd_fnc_idd));


       // Lambert W Functions **********************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_lambert_W0",f_unn(gsl_sf_lambert_W0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_lambert_Wm1",f_unn(gsl_sf_lambert_Wm1_e),hnd_fnc_x,NC_DOUBLE));


       // Legendre Functions ***********************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Pl",f_unn(gsl_sf_legendre_Pl_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Pl_array",f_unn(gsl_sf_legendre_Pl_array),hnd_fnc_idpd));
	//gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Pl_deriv_array",f_unn(gsl_sf_legendre_Pl_deriv_array), ));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_P1",f_unn(gsl_sf_legendre_P1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_P2",f_unn(gsl_sf_legendre_P2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_P3",f_unn(gsl_sf_legendre_P3_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Q0",f_unn(gsl_sf_legendre_Q0_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Q1",f_unn(gsl_sf_legendre_Q1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Ql",f_unn(gsl_sf_legendre_Ql_e),hnd_fnc_xd,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Plm",f_unn(gsl_sf_legendre_Plm_e),hnd_fnc_iid));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Plm_array",f_unn(gsl_sf_legendre_Plm_array),hnd_fnc_iidpd,PLEGEND));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Plm_deriv_array",f_unn(gsl_sf_legendre_Plm_deriv_array),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm",f_unn(gsl_sf_legendre_sphPlm_e),hnd_fnc_iid));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm_array",f_unn(gsl_sf_legendre_sphPlm_array),hnd_fnc_iidpd,PLEGEND));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm_deriv_array",f_unn(gsl_sf_legendre_sphPlm_deriv_array),));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_array_size",f_unn(gsl_sf_legendre_array_size),));
	gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_half",f_unn(gsl_sf_conicalP_half_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_mhalf",f_unn(gsl_sf_conicalP_mhalf_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_0",f_unn(gsl_sf_conicalP_0_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_1",f_unn(gsl_sf_conicalP_1_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_sph_reg",f_unn(gsl_sf_conicalP_sph_reg_e),hnd_fnc_idd));
	gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_cyl_reg",f_unn(gsl_sf_conicalP_cyl_reg_e),hnd_fnc_idd));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d_0",f_unn(gsl_sf_legendre_H3d_0_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d_1",f_unn(gsl_sf_legendre_H3d_1_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d",f_unn(gsl_sf_legendre_H3d_e),hnd_fnc_idd));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d_array",f_unn(gsl_sf_legendre_H3d_array),));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_array_size",f_unn(gsl_sf_legendre_array_size),));



       // Logarithm and related Functions **********************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_log",f_unn(gsl_sf_log_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_log_abs",f_unn(gsl_sf_log_abs_e),hnd_fnc_x,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_complex_log",f_unn(gsl_sf_complex_log_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_log_1plusx",f_unn(gsl_sf_log_1plusx_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_log_1plusx_mx",f_unn(gsl_sf_log_1plusx_mx_e),hnd_fnc_x,NC_DOUBLE));



       // MathieuFunctions *************************************************************************/
       // None implemented    

       // Power Functions **************************************************************************/
	//gpr_vtr.push_back(gpr_cls("gsl_sf_pow_int",f_unn(gsl_sf_pow_int_e),hnd_fnc_di,NC_DOUBLE));

       // Psi   Functions **************************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_psi_int",f_unn(gsl_sf_psi_e),hnd_fnc_x,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_psi",f_unn(gsl_sf_psi_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_psi_1piy",f_unn(gsl_sf_psi_1piy_e),hnd_fnc_x,NC_DOUBLE));
	// gpr_vtr.push_back(gpr_cls("gsl_sf_complex_psi",f_unn(gsl_sf_complex_psi_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_psi_1_int",f_unn(gsl_sf_psi_1_int_e),hnd_fnc_x,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_psi_1",f_unn(gsl_sf_psi_1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_psi_n",f_unn(gsl_sf_psi_n_e),hnd_fnc_xd,NC_INT));


 
       // Synchrotron Functions *********************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_synchrotron_1",f_unn(gsl_sf_synchrotron_1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_synchrotron_2",f_unn(gsl_sf_synchrotron_2_e),hnd_fnc_x,NC_DOUBLE));

       // Transport Functions ************************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_transport_2",f_unn(gsl_sf_transport_2_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_transport_3",f_unn(gsl_sf_transport_3_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_transport_4",f_unn(gsl_sf_transport_4_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_transport_5",f_unn(gsl_sf_transport_5_e),hnd_fnc_x,NC_DOUBLE));


       // Trigometric Functions ************************************************************************/
       // None implemented    

       // Zeta Functions *******************************************************************************/
	gpr_vtr.push_back(gpr_cls("gsl_sf_zeta_int",f_unn(gsl_sf_zeta_int_e),hnd_fnc_x,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_zeta",f_unn(gsl_sf_zeta_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_zetam1",f_unn(gsl_sf_zetam1_e),hnd_fnc_x,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_zetam1_int",f_unn(gsl_sf_zetam1_int_e),hnd_fnc_x,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_hzeta",f_unn(gsl_sf_hzeta_e),hnd_fnc_xd,NC_DOUBLE));
	gpr_vtr.push_back(gpr_cls("gsl_sf_eta_int",f_unn(gsl_sf_eta_int_e),hnd_fnc_x,NC_INT));
	gpr_vtr.push_back(gpr_cls("gsl_sf_eta",f_unn(gsl_sf_eta),hnd_fnc_x,NC_DOUBLE));


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



var_sct *gsl_cls::hnd_fnc_x(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
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



var_sct *gsl_cls::hnd_fnc_xd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){ 
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



// used for evaluating arrays of bessel & legendre functions 
// bessel functions have the arg signature (int min,int max,double x,double *results )
// legendre functions have the min & max reveresed (int lmax,int m,double x,double *results)
var_sct *gsl_cls::hnd_fnc_iidpd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){ 
  const std::string fnc_nm("hnd_fnc_iidpd");
            int idx;
            int args_nbr;
            int fdx=gpr_obj.type(); // only two types at the moment 
;            vtl_typ lcl_typ;
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

            if(fdx==PBESSEL)            
               susg="usage: var_out="+sfnm+"(int nmin, int nmax, double x, var_in)";
            else if(fdx==PLEGEND) 
               susg="usage: var_out="+sfnm+"(int lmax, int m, double x, var_in)";

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
	    lcl_typ=expr_typ(args_vtr[3]);          

           if(lcl_typ !=VVAR ) {
             serr="The last argument must  be a variable identifer\n";
             err_prn(sfnm,serr+susg);
           }

          


            // do heavy listing; 
            {
	    int nbr_min;
            int nbr_max;
            int nbr_tmp; 
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


	    
	    if(fdx==PBESSEL){
              sz_out=nbr_max-nbr_min+1;
              if(sz_out<1 )  
	        err_prn(sfnm,"nmax must be greater than or equal to nmin\n"+susg);    
            } 

            // nb Legendre array functions have min/max reversed  
	    if(fdx==PLEGEND){
              sz_out=nbr_min-nbr_max+1;
              if(sz_out<1 )  
	        err_prn(sfnm,"lmax must be greater than or equal to m\n"+susg);    
	    }

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


var_sct *gsl_cls::hnd_fnc_idpd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){ 
  const std::string fnc_nm("hnd_fnc_idpd");
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
	    lcl_typ=expr_typ(args_vtr[2]);          

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


var_sct *gsl_cls::hnd_fnc_dm(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
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


var_sct *gsl_cls::hnd_fnc_ddm(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_ddm");
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

           
	    // do heavy lifting       
           {
 
	     bool has_mss_val=false;
             int sz;
             double *dp1;
             double *dp2; 
             double mss_val_dbl;
             gsl_mode_t mde_t=GSL_PREC_DOUBLE;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */
             
             int (*fnc_int)(double, double, gsl_mode_t,gsl_sf_result*);

	     fnc_int=gpr_obj.g_args().addm; 


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
                   dp2[idx]=( (*fnc_int)(dp1[idx],dp2[idx],mde_t,&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	       }else{
      		 
      		 for(idx=0;idx<sz;idx++)
                   // note fnc_int return status flag, if 0 then no error
        	   if(dp1[idx] == mss_val_dbl || 
                      dp2[idx] == mss_val_dbl ||
                      (*fnc_int)(dp1[idx],dp2[idx],mde_t, &rslt)
		      ) dp2[idx]=mss_val_dbl;
		   else
                       dp2[idx]=rslt.val;      
	       }           

               (void)cast_nctype_void(NC_DOUBLE,&(var1->val));
               (void)cast_nctype_void(NC_DOUBLE,&(var2->val));
               nco_var_free(var1);
	    }

            return var2; 




}

var_sct *gsl_cls::hnd_fnc_nd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_nd");
            int idx;
            int jdx;
            int fdx=gpr_obj.type(); // very important 
            int args_nbr;
            int args_in_nbr;
            nc_type type;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
            var_sct *var_ret; 

          
            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;
            type=gpr_obj.type();  

            args_nbr=args_vtr.size();
            
            switch(fdx) {
	      case P1DBLMD:
              case P1DBL:
                args_in_nbr=1;
                break;    
	      case P2DBLMD:
              case P2DBL:
                args_in_nbr=2;
                break;    
	      case P3DBLMD:
              case P3DBL:
                args_in_nbr=3;
                break;    
	      case P4DBLMD:
              case P4DBL:
                args_in_nbr=4;
                break;    
                   
	    default:
	      break;
            }
            

            if(args_nbr <args_in_nbr)
              if(is_mtd)
                err_prn(sfnm,styp+" requires "+nbr2sng(args_in_nbr-1)+ " arguments"); 
              else
                err_prn(sfnm,styp+" requires "+ nbr2sng(args_in_nbr) + " arguments.");    
            
            // init once we now num of args
            var_sct *var_arr[args_in_nbr];
            var_sct **var_arr_ptr[args_in_nbr]; 

             

            for(idx=0; idx<args_in_nbr ;idx++){     
              var_arr[idx]=walker.out(args_vtr[idx]);
              var_arr_ptr[idx]=&var_arr[idx];
            } 
             
            // Deal with initial scan
            if(prs_arg->ntl_scn){

	      // nb ncap_var_att_arr_cnf() frees up sct's in var_att_ptr    
               var_ret=ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,args_in_nbr );     
               if(!var_ret->undefined)
	           var_ret=nco_var_cnf_typ(NC_DOUBLE,var_ret);

      	      return var_ret;
            } 



            for(idx=0 ; idx<args_in_nbr ; idx++){
              // convert all to type double
              var_arr[idx]=nco_var_cnf_typ(NC_DOUBLE,var_arr[idx]);
              // refrsh var pointers
              var_arr_ptr[idx]=&var_arr[idx];
            }

            // make variables conform  
            (void)ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,args_in_nbr );



	    // do heavy lifting       
           {
 
	     bool has_mss_val=false;
             int sz;
             double *dp[args_in_nbr];
             double mss_val_dbl;
             gsl_mode_t mde_t=GSL_PREC_DOUBLE;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */
             


             // assume from here on that args conform
             sz=var_arr[0]->sz;

            for(idx=0 ; idx<args_in_nbr ; idx++){
             (void)cast_void_nctype(NC_DOUBLE,&(var_arr[idx]->val));                  
	     dp[idx]=var_arr[idx]->val.dp;
            }  

             has_mss_val=false;  
             for(idx=0 ; idx<args_in_nbr ;idx++) 
                if(var_arr[idx]->has_mss_val){
                 has_mss_val=true; 
                 (void)cast_void_nctype(NC_DOUBLE,&var_arr[idx]->mss_val);
                 mss_val_dbl=var_arr[idx]->mss_val.dp[0]; 
		 (void)cast_nctype_void(NC_DOUBLE,&(var_arr[idx]->mss_val));
                 break;
               } 
              

             switch(fdx){

             // one double argument
	     case P1DBL: {

                int (*fnc_int)(double,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().ad;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || (*fnc_int)(dp[0][jdx], &rslt))
		       dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;


	     case P1DBLMD: {

                int (*fnc_int)(double, gsl_mode_t,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().adm;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],mde_t,&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || (*fnc_int)(dp[0][jdx],mde_t, &rslt))
		       dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;

	     // two double arguments
	     case P2DBL: {

                int (*fnc_int)(double,double,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().add;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],dp[1][jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || 
                      dp[1][jdx] == mss_val_dbl ||

                      (*fnc_int)(dp[0][jdx],dp[1][jdx],&rslt)
		      ) dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;


	     case P2DBLMD: {

                int (*fnc_int)(double,double, gsl_mode_t,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().addm;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],dp[1][jdx],mde_t,&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || 
                      dp[1][jdx] == mss_val_dbl ||

                      (*fnc_int)(dp[0][jdx],dp[1][jdx],mde_t, &rslt)
		      ) dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;




	     //three double args 
             case P3DBL: {

                int (*fnc_int)(double, double,double,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().addd;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || 
                      dp[1][jdx] == mss_val_dbl ||
                      dp[2][jdx] == mss_val_dbl ||

                      (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx], &rslt)
		      ) dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;



             case P3DBLMD: {

                int (*fnc_int)(double, double,double, gsl_mode_t,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().adddm;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx],mde_t,&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || 
                      dp[1][jdx] == mss_val_dbl ||
                      dp[2][jdx] == mss_val_dbl ||

                      (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx],mde_t, &rslt)
		      ) dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;



	     //four double args 
             case P4DBL: {

                int (*fnc_int)(double, double,double,double,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().adddd;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || 
                      dp[1][jdx] == mss_val_dbl ||
                      dp[2][jdx] == mss_val_dbl ||
                      dp[3][jdx] == mss_val_dbl ||

                      (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx], &rslt)
		      ) dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;


             case P4DBLMD: {

                int (*fnc_int)(double, double,double,double, gsl_mode_t,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().addddm;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx],mde_t,&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[0][jdx] == mss_val_dbl || 
                      dp[1][jdx] == mss_val_dbl ||
                      dp[2][jdx] == mss_val_dbl ||
                      dp[3][jdx] == mss_val_dbl ||

                      (*fnc_int)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx],mde_t, &rslt)
		      ) dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		}           
               
             } break;

             default: break;


	     }// end big switch


	     for(idx=0;idx<args_in_nbr; idx++){ 
               (void)cast_nctype_void(NC_DOUBLE,&(var_arr[idx]->val));
               if(idx>0)  nco_var_free(var_arr[idx]);
	     }

	   } // end heavy lifting

	    return var_arr[0]; 

} //end hnd_fnc_nd 


// handle regular arguments NC_INT,NC_INT,NC_DOUBLE
var_sct *gsl_cls::hnd_fnc_iid(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_iid");
            int idx;
            int jdx;
            //int fdx=gpr_obj.type(); // very important 
            int args_nbr;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
            var_sct *var_ret; 
            var_sct *var_arr[3];
            var_sct **var_arr_ptr[3]; 

            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;

            args_nbr=args_vtr.size(); 
            

            if(args_nbr <3)
              if(is_mtd)
                err_prn(sfnm,styp+" requires two arguments"); 
              else
                err_prn(sfnm,styp+" requires three arguments.");    
             

            for(idx=0; idx< 3 ;idx++){     
              var_arr[idx]=walker.out(args_vtr[idx]);
              var_arr_ptr[idx]=&var_arr[idx];
            } 
             
            // Deal with initial scan
            if(prs_arg->ntl_scn){

	      // nb ncap_var_att_arr_cnf() frees up sct's in var_att_ptr    
               var_ret=ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,3 );     
               if(!var_ret->undefined)
	           var_ret=nco_var_cnf_typ(NC_DOUBLE,var_ret);

      	      return var_ret;
            } 

  
            var_arr[0]=nco_var_cnf_typ(NC_INT,var_arr[0]);
            var_arr[1]=nco_var_cnf_typ(NC_INT,var_arr[1]);
            var_arr[2]=nco_var_cnf_typ(NC_DOUBLE,var_arr[2]);

	    // now make variables conform 
            // make variables conform  
            (void)ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,3 );



	    // do heavy lifting       
           {
 
	     bool has_mss_val=false;
             int sz;
             double *dp;
             long *lp[2]; 
             double mss_val_dbl;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */
             

             // assume from here on that args conform
             sz=var_arr[0]->sz;


             (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));                  
	     lp[0]=var_arr[0]->val.lp;
             
             (void)cast_void_nctype(NC_INT,&(var_arr[1]->val));                  
	     lp[1]=var_arr[1]->val.lp;

             (void)cast_void_nctype(NC_DOUBLE,&(var_arr[2]->val));                  
	     dp=var_arr[2]->val.dp;
             

             has_mss_val=false;  
             if(var_arr[2]->has_mss_val){
               has_mss_val=true; 
               (void)cast_void_nctype(NC_DOUBLE,&var_arr[2]->mss_val);
               mss_val_dbl=var_arr[2]->mss_val.dp[0]; 
	       (void)cast_nctype_void(NC_DOUBLE,&(var_arr[2]->mss_val));
             }
           
             // do the deed !!!
             {
              
                int (*fnc_int)(int, int,double,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().aiid;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[jdx]=( (*fnc_int)(lp[0][jdx],lp[1][jdx],dp[jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if(dp[jdx] == mss_val_dbl || (*fnc_int)(lp[0][jdx],lp[1][jdx],dp[jdx], &rslt) )
		       dp[jdx]=mss_val_dbl;
		    else
                      dp[jdx]=rslt.val;      
		 
		} // end else           
               
             } // done the deed !!

           (void)cast_nctype_void(NC_INT,&(var_arr[0]->val));
           (void)cast_nctype_void(NC_INT,&(var_arr[1]->val));  
           (void)cast_nctype_void(NC_DOUBLE,&(var_arr[2]->val));

           (void)nco_var_free(var_arr[0]);
           (void)nco_var_free(var_arr[1]);
    
            
           }  
               
           return var_arr[2]; 


}


// handle regular arguments NC_INT,NC_DOUBLE,NC_DOUBLE
var_sct *gsl_cls::hnd_fnc_idd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_idd");
            int idx;
            int jdx;
            //int fdx=gpr_obj.type(); // very important 
            int args_nbr;
	    std::string styp=(is_mtd ? "method":"function");
	    std::string sfnm=gpr_obj.fnm();
            var_sct *var_ret;
            var_sct *var_arr[3];
            var_sct **var_arr_ptr[3]; 

            // de-reference 
            prs_cls *prs_arg=walker.prs_arg;

            args_nbr=args_vtr.size(); 
            

            if(args_nbr <3)
              if(is_mtd)
                err_prn(sfnm,styp+" requires two arguments"); 
              else
                err_prn(sfnm,styp+" requires three arguments.");    
             

            for(idx=0; idx< 3 ;idx++){     
              var_arr[idx]=walker.out(args_vtr[idx]);
              var_arr_ptr[idx]=&var_arr[idx];
            } 
             
            // Deal with initial scan
            if(prs_arg->ntl_scn){

	      // nb ncap_var_att_arr_cnf() frees up sct's in var_att_ptr    
               var_ret=ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,3 );     
               if(!var_ret->undefined)
	           var_ret=nco_var_cnf_typ(NC_DOUBLE,var_ret);

      	      return var_ret;
            } 

  
            var_arr[0]=nco_var_cnf_typ(NC_INT,var_arr[0]);
            var_arr[1]=nco_var_cnf_typ(NC_DOUBLE,var_arr[1]);
            var_arr[2]=nco_var_cnf_typ(NC_DOUBLE,var_arr[2]);

            // make variables conform  
            (void)ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,3 );



	    // do heavy lifting       
           {
 
	     bool has_mss_val=false;
             int sz;
             double *dp[2];
             long *lp; 
             double mss_val_dbl;
             gsl_sf_result rslt;  /* structure for result from gsl lib call */
             

             // assume from here on that args conform
             sz=var_arr[0]->sz;


             (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));                  
	     lp=var_arr[0]->val.lp;


             (void)cast_void_nctype(NC_DOUBLE,&(var_arr[1]->val));                  
	     dp[0]=var_arr[1]->val.dp;
             

             (void)cast_void_nctype(NC_DOUBLE,&(var_arr[2]->val));                  
	     dp[1]=var_arr[2]->val.dp;
             

             // get missing value  
             has_mss_val=false;  
             for(idx=1 ; idx<3 ;idx++) 
                if(var_arr[idx]->has_mss_val){
                 has_mss_val=true; 
                 (void)cast_void_nctype(NC_DOUBLE,&var_arr[idx]->mss_val);
                 mss_val_dbl=var_arr[idx]->mss_val.dp[0]; 
		 (void)cast_nctype_void(NC_DOUBLE,&(var_arr[idx]->mss_val));
                 break;
               } 

             // do the deed !!!
             {
              
                int (*fnc_int)(int,double,double,gsl_sf_result*);       
               	fnc_int=gpr_obj.g_args().aidd;  

                if(!has_mss_val){ 
      	          for(jdx=0;jdx<sz;jdx++) 
                    dp[0][jdx]=( (*fnc_int)(lp[jdx],dp[0][jdx],dp[1][jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    	        }else{
      		 
      		  for(jdx=0;jdx<sz;jdx++)
                    // note fnc_int return status flag, if 0 then no error
        	    if( dp[0][jdx] == mss_val_dbl || 
                        dp[1][jdx] == mss_val_dbl || 
                        (*fnc_int)(lp[jdx],dp[0][jdx],dp[1][jdx], &rslt)
                       )
		       dp[0][jdx]=mss_val_dbl;
		    else
                      dp[0][jdx]=rslt.val;      
		 
		} // end else           
               
             } // done the deed !!

           (void)cast_nctype_void(NC_INT,&(var_arr[0]->val));
           (void)cast_nctype_void(NC_DOUBLE,&(var_arr[1]->val));  
           (void)cast_nctype_void(NC_DOUBLE,&(var_arr[2]->val));

           (void)nco_var_free(var_arr[0]);
           (void)nco_var_free(var_arr[2]);
    
            
           }  
               
           return var_arr[1]; 

} // end function hnd_fnc_idd
