/* $Header$ */

/* Purpose: netCDF arithmetic processor class methods for GSL */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "fmc_gsl_cls.hh"
#include "nco_gsl.hh"

// GSL Functions
#ifdef ENABLE_GSL

// dummy function -used to fill out arg list
int  ncap_void(void){
  return 10;
}

gsl_cls::gsl_cls(bool ){
    // populate gpr_vtr
  gsl_ini_sf();    // Special Functions 
  gsl_ini_cdf();   // Cumulative distribution Functions
  gsl_ini_ran();   // Random Number Generator functions
  gsl_ini_stats();  // Statistics  
  // Copy into fmc_cls vector  
  for(unsigned idx=0;idx<gpr_vtr.size();idx++) fmc_vtr.push_back(fmc_cls(gpr_vtr[idx].fnm(),this,idx));
}

void gsl_cls::gsl_ini_sf(void) {
    // Airy Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai",f_unn(gsl_sf_airy_Ai_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi",f_unn(gsl_sf_airy_Bi_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai_scaled",f_unn(gsl_sf_airy_Ai_scaled_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi_scaled",f_unn(gsl_sf_airy_Bi_scaled_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai_deriv",f_unn(gsl_sf_airy_Ai_deriv_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi_deriv",f_unn(gsl_sf_airy_Bi_deriv_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Ai_deriv_scaled",f_unn(gsl_sf_airy_Ai_deriv_scaled_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_Bi_deriv_scaled",f_unn(gsl_sf_airy_Bi_deriv_scaled_e),hnd_fnc_nd,P1DBLMD));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Ai",f_unn(gsl_sf_airy_zero_Ai_e),hnd_fnc_x,NC_UINT));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Bi",f_unn(gsl_sf_airy_zero_Bi_e),hnd_fnc_x,NC_UINT));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Ai_deriv",f_unn(gsl_sf_airy_zero_Ai_deriv_e),hnd_fnc_x,NC_UINT));  
    gpr_vtr.push_back(gpr_cls("gsl_sf_airy_zero_Bi_deriv",f_unn(gsl_sf_airy_zero_Bi_deriv_e),hnd_fnc_x,NC_UINT));  
    
    // Bessel Functions
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
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Jnu",f_unn(gsl_sf_bessel_Jnu_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Ynu",f_unn(gsl_sf_bessel_Ynu_e),hnd_fnc_nd,P2DBL));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_sequence_Jnu",f_unn(gsl_sf_bessel_sequence_Jnu_e),));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Inu_scaled",f_unn(gsl_sf_bessel_Inu_scaled_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Inu",f_unn(gsl_sf_bessel_Inu_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Knu_scaled",f_unn(gsl_sf_bessel_Knu_scaled_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_Knu",f_unn(gsl_sf_bessel_Knu_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_lnKnu",f_unn(gsl_sf_bessel_lnKnu_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_zero_J0",f_unn(gsl_sf_bessel_zero_J0_e),hnd_fnc_x,NC_UINT));
    gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_zero_J1",f_unn(gsl_sf_bessel_zero_J1_e),hnd_fnc_x,NC_UINT));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_bessel_zero_Jnu",f_unn(gsl_sf_bessel_zero_Jnu_e),));
    
    // Clausen Function
    gpr_vtr.push_back(gpr_cls("gsl_sf_clausen",f_unn(gsl_sf_clausen),hnd_fnc_x,NC_DOUBLE));
    
    // Coulomb Function
    
    // Coupling Coefficents
    
    // Dawson Function
    gpr_vtr.push_back(gpr_cls("gsl_sf_dawson",f_unn(gsl_sf_dawson),hnd_fnc_x,NC_DOUBLE));
    
    // Debye Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_debye_1",f_unn(gsl_sf_debye_1_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_debye_2",f_unn(gsl_sf_debye_2_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_debye_3",f_unn(gsl_sf_debye_3_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_debye_4",f_unn(gsl_sf_debye_4_e),hnd_fnc_x,NC_DOUBLE));

# if NCO_GSL_VERSION >= 108
      gpr_vtr.push_back(gpr_cls("gsl_sf_debye_5",f_unn(gsl_sf_debye_5_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_debye_6",f_unn(gsl_sf_debye_6_e),hnd_fnc_x,NC_DOUBLE));
# endif // NCO_GSL_VERSION < 108
    
    // Dilogarithm
    // not implemented as all involve complex numbers
    
    // Elementary Operations
    
    // Elliptic Integrals
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Kcomp",f_unn(gsl_sf_ellint_Kcomp_e),hnd_fnc_nd,P1DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Ecomp",f_unn(gsl_sf_ellint_Ecomp_e),hnd_fnc_nd,P1DBLMD));

# if NCO_GSL_VERSION >= 109 
      gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Pcomp",f_unn(gsl_sf_ellint_Pcomp_e),hnd_fnc_nd,P2DBLMD));
      gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Dcomp",f_unn(gsl_sf_ellint_Dcomp_e),hnd_fnc_nd,P1DBLMD));
# endif // NCO_GSL_VERSION < 9
 
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_F",f_unn(gsl_sf_ellint_F_e),hnd_fnc_nd,P2DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_E",f_unn(gsl_sf_ellint_E_e),hnd_fnc_nd,P2DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_P",f_unn(gsl_sf_ellint_P_e),hnd_fnc_nd,P3DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_D",f_unn(gsl_sf_ellint_D_e),hnd_fnc_nd,P3DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RC",f_unn(gsl_sf_ellint_RC_e), hnd_fnc_nd,P2DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RD",f_unn(gsl_sf_ellint_RD_e),hnd_fnc_nd,P3DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RF",f_unn(gsl_sf_ellint_RF_e),hnd_fnc_nd,P3DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_RJ",f_unn(gsl_sf_ellint_RJ_e),hnd_fnc_nd,P4DBLMD));
    
    // Elliptic Function (Jacobi)
    
    
    // Error  Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_erfc",f_unn(gsl_sf_erfc_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_log_erfc",f_unn(gsl_sf_log_erfc_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_erf",f_unn(gsl_sf_erf_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_erf_Z",f_unn(gsl_sf_erf_Z_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_erf_Q",f_unn(gsl_sf_erf_Q_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_hazard",f_unn(gsl_sf_hazard_e),hnd_fnc_x,NC_DOUBLE));
    
    
    // Exponential  Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_exp",f_unn(gsl_sf_exp_e),hnd_fnc_x,NC_DOUBLE));
    //gpr_vtr.push_back(gpr_cls("gsl_sf_exp_e10",f_unn(gsl_sf_exp_10_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult",f_unn(gsl_sf_exp_mult_e),hnd_fnc_nd,P2DBL));
    //gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult_e10",f_unn(gsl_sf_exp_mult_e10_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_expm1",f_unn(gsl_sf_expm1_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_exprel",f_unn(gsl_sf_exprel_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_exprel_2",f_unn(gsl_sf_exprel_2_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_exprel_n",f_unn(gsl_sf_exprel_n_e),hnd_fnc_xd,NC_INT));
    gpr_vtr.push_back(gpr_cls("gsl_sf_exp_err",f_unn(gsl_sf_exp_err_e),hnd_fnc_nd,P2DBL));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_exp_err_e10",f_unn(gsl_sf_exp_err_e10_e),hnd_fnc_nd,P2DBL));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult_err_e",f_unn(gsl_sf_exp_mult_err_e),hnd_fnc_nd,P2DBL));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_exp_mult_err_e10_e",f_unn(gsl_sf_exp_mult_err_e10_e),hnd_fnc_nd,P2DBL));
    
    // Exponential Integral Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E1",f_unn(gsl_sf_expint_E1_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E2",f_unn(gsl_sf_expint_E2_e),hnd_fnc_x,NC_DOUBLE));

    gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E1_scaled",f_unn(gsl_sf_expint_E1_scaled_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_expint_E2_scaled",f_unn(gsl_sf_expint_E2_scaled_e),hnd_fnc_x,NC_DOUBLE));

# if NCO_GSL_VERSION >= 110 
      gpr_vtr.push_back(gpr_cls("gsl_sf_expint_En",f_unn(gsl_sf_expint_En_e),hnd_fnc_xd,NC_INT));
      gpr_vtr.push_back(gpr_cls("gsl_sf_expint_En_scaled",f_unn(gsl_sf_expint_En_scaled_e),hnd_fnc_xd,NC_INT));
# endif // NCO_GSL_VERSION < 10

    gpr_vtr.push_back(gpr_cls("gsl_sf_expint_Ei",f_unn(gsl_sf_expint_Ei_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_expint_Ei_scaled",f_unn(gsl_sf_expint_Ei_scaled_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_Shi",f_unn(gsl_sf_Shi_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_Chi",f_unn(gsl_sf_Chi_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_expint_3",f_unn(gsl_sf_expint_3_e),hnd_fnc_x,NC_DOUBLE));       
    gpr_vtr.push_back(gpr_cls("gsl_sf_Si",f_unn(gsl_sf_Si_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_Ci",f_unn(gsl_sf_Ci_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_atanint",f_unn(gsl_sf_atanint_e),hnd_fnc_x,NC_DOUBLE));
    
    // Fermi Dirac Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_m1",f_unn(gsl_sf_fermi_dirac_m1_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_0",f_unn(gsl_sf_fermi_dirac_0_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_1",f_unn(gsl_sf_fermi_dirac_1_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_2",f_unn(gsl_sf_fermi_dirac_2_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_int",f_unn(gsl_sf_fermi_dirac_int_e),hnd_fnc_xd,NC_INT));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_mhalf",f_unn(gsl_sf_fermi_dirac_mhalf_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_half",f_unn(gsl_sf_fermi_dirac_half_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_3half",f_unn(gsl_sf_fermi_dirac_3half_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_fermi_dirac_inc_0",f_unn(gsl_sf_fermi_dirac_inc_0_e),hnd_fnc_nd,P2DBL));
    
    // Gamma & Beta Functions
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
    gpr_vtr.push_back(gpr_cls("gsl_sf_lnpoch",f_unn(gsl_sf_lnpoch_e),hnd_fnc_nd,P2DBL));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_lnpoch_sgn",f_unn(gsl_sf_lnpoch_sign_e)),);
    gpr_vtr.push_back(gpr_cls("gsl_sf_poch",f_unn(gsl_sf_poch_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_pochrel",f_unn(gsl_sf_pochrel_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc_Q",f_unn(gsl_sf_gamma_inc_Q_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc_P",f_unn(gsl_sf_gamma_inc_P_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_gamma_inc",f_unn(gsl_sf_gamma_inc_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_lnbeta",f_unn(gsl_sf_lnbeta_e),hnd_fnc_nd,P2DBL));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_gammagsl_sf_lnbeta_sgn",f_unn(gsl_sf_gammagsl_sf_lnbeta_sgn_e),));
    gpr_vtr.push_back(gpr_cls("gsl_sf_beta",f_unn(gsl_sf_beta_e),hnd_fnc_nd,P2DBL));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_beta_inc",f_unn(gsl_sf_beta_inc_e),));
    
    // Gegenbauer Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_1",f_unn(gsl_sf_gegenpoly_1_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_2",f_unn(gsl_sf_gegenpoly_2_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_3",f_unn(gsl_sf_gegenpoly_3_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_n",f_unn(gsl_sf_gegenpoly_n_e),hnd_fnc_idd));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_gegenpoly_array",f_unn(gsl_sf_gegenpoly_array),));
    
    // Hypergeometric Functions
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
    
    // Laguerre Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_1",f_unn(gsl_sf_laguerre_1_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_2",f_unn(gsl_sf_laguerre_2_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_3",f_unn(gsl_sf_laguerre_3_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_laguerre_n",f_unn(gsl_sf_laguerre_n_e),hnd_fnc_idd));
    
    // Lambert W Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_lambert_W0",f_unn(gsl_sf_lambert_W0_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_lambert_Wm1",f_unn(gsl_sf_lambert_Wm1_e),hnd_fnc_x,NC_DOUBLE));
    
    // Legendre Functions
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

    // gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Plm_deriv_array",f_unn(gsl_sf_legendre_Plm_deriv_array),));
    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm",f_unn(gsl_sf_legendre_sphPlm_e),hnd_fnc_iid));

    // 20161118: These two functions were deprecated after GSL version 1.x
    // Unable them working on grele with bld/Makefile, temporarily disable
    #if NCO_GSL_VERSION < 200    
    //    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Plm_array",f_unn(gsl_sf_legendre_Plm_array),hnd_fnc_iidpd,PLEGEND));
    //    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm_array",f_unn(gsl_sf_legendre_sphPlm_array),hnd_fnc_iidpd,PLEGEND));
    #endif

    // gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm_deriv_array",f_unn(gsl_sf_legendre_sphPlm_deriv_array),));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_array_size",f_unn(gsl_sf_legendre_array_size),));
    gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_half",f_unn(gsl_sf_conicalP_half_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_mhalf",f_unn(gsl_sf_conicalP_mhalf_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_0",f_unn(gsl_sf_conicalP_0_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_1",f_unn(gsl_sf_conicalP_1_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_sph_reg",f_unn(gsl_sf_conicalP_sph_reg_e),hnd_fnc_idd));
    gpr_vtr.push_back(gpr_cls("gsl_sf_conicalP_cyl_reg",f_unn(gsl_sf_conicalP_cyl_reg_e),hnd_fnc_idd));
    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d_0",f_unn(gsl_sf_legendre_H3d_0_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d_1",f_unn(gsl_sf_legendre_H3d_1_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d",f_unn(gsl_sf_legendre_H3d_e),hnd_fnc_idd));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_H3d_array",f_unn(gsl_sf_legendre_H3d_array),));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_array_size",f_unn(gsl_sf_legendre_array_size),));
    
    // Logarithm and related Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_log",f_unn(gsl_sf_log_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_log_abs",f_unn(gsl_sf_log_abs_e),hnd_fnc_x,NC_DOUBLE));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_complex_log",f_unn(gsl_sf_complex_log_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_log_1plusx",f_unn(gsl_sf_log_1plusx_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_log_1plusx_mx",f_unn(gsl_sf_log_1plusx_mx_e),hnd_fnc_x,NC_DOUBLE));
    
    // Mathieu Functions
    // None implemented    
    
    // Power Functions
    //gpr_vtr.push_back(gpr_cls("gsl_sf_pow_int",f_unn(gsl_sf_pow_int_e),hnd_fnc_di,NC_DOUBLE));
    
    // Psi Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_psi_int",f_unn(gsl_sf_psi_e),hnd_fnc_x,NC_INT));
    gpr_vtr.push_back(gpr_cls("gsl_sf_psi",f_unn(gsl_sf_psi_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_psi_1piy",f_unn(gsl_sf_psi_1piy_e),hnd_fnc_x,NC_DOUBLE));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_complex_psi",f_unn(gsl_sf_complex_psi_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_psi_1_int",f_unn(gsl_sf_psi_1_int_e),hnd_fnc_x,NC_INT));
   
# if NCO_GSL_VERSION >= 105 
      gpr_vtr.push_back(gpr_cls("gsl_sf_psi_1",f_unn(gsl_sf_psi_1_e),hnd_fnc_x,NC_DOUBLE));
# endif // NCO_GSL_VERSION < 105

    gpr_vtr.push_back(gpr_cls("gsl_sf_psi_n",f_unn(gsl_sf_psi_n_e),hnd_fnc_xd,NC_INT));
    
    // Synchrotron Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_synchrotron_1",f_unn(gsl_sf_synchrotron_1_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_synchrotron_2",f_unn(gsl_sf_synchrotron_2_e),hnd_fnc_x,NC_DOUBLE));
    
    // Transport Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_transport_2",f_unn(gsl_sf_transport_2_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_transport_3",f_unn(gsl_sf_transport_3_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_transport_4",f_unn(gsl_sf_transport_4_e),hnd_fnc_x,NC_DOUBLE));
    gpr_vtr.push_back(gpr_cls("gsl_sf_transport_5",f_unn(gsl_sf_transport_5_e),hnd_fnc_x,NC_DOUBLE));
        
    // Trigonometric Functions
    // None implemented    
    
    // Zeta Functions
    gpr_vtr.push_back(gpr_cls("gsl_sf_zeta_int",f_unn(gsl_sf_zeta_int_e),hnd_fnc_x,NC_INT));
    gpr_vtr.push_back(gpr_cls("gsl_sf_zeta",f_unn(gsl_sf_zeta_e),hnd_fnc_x,NC_DOUBLE));

# if NCO_GSL_VERSION >= 105 
      gpr_vtr.push_back(gpr_cls("gsl_sf_zetam1",f_unn(gsl_sf_zetam1_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_zetam1_int",f_unn(gsl_sf_zetam1_int_e),hnd_fnc_x,NC_INT));
# endif // NCO_GSL_VERSION < 105
    
    gpr_vtr.push_back(gpr_cls("gsl_sf_hzeta",f_unn(gsl_sf_hzeta_e),hnd_fnc_nd,P2DBL));
    gpr_vtr.push_back(gpr_cls("gsl_sf_eta_int",f_unn(gsl_sf_eta_int_e),hnd_fnc_x,NC_INT));
    gpr_vtr.push_back(gpr_cls("gsl_sf_eta",f_unn(gsl_sf_eta),hnd_fnc_x,NC_DOUBLE));

} // end gsl_ini_sf


// Cumulative Distribution Functions
void gsl_cls::gsl_ini_cdf(void){

    gpr_vtr.push_back(gpr_cls("gsl_cdf_ugaussian_P",f_unn(gsl_cdf_ugaussian_P),hnd_fnc_nd,P1DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_ugaussian_Q",f_unn(gsl_cdf_ugaussian_Q),hnd_fnc_nd,P1DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_ugaussian_Pinv",f_unn(gsl_cdf_ugaussian_Pinv),hnd_fnc_nd,P1DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_ugaussian_Qinv",f_unn(gsl_cdf_ugaussian_Qinv),hnd_fnc_nd,P1DBLX));
    
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gaussian_P",f_unn(gsl_cdf_gaussian_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gaussian_Q",f_unn(gsl_cdf_gaussian_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gaussian_Pinv",f_unn(gsl_cdf_gaussian_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gaussian_Qinv",f_unn(gsl_cdf_gaussian_Qinv),hnd_fnc_nd,P2DBLX));
  
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gamma_P",f_unn(gsl_cdf_gamma_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gamma_Q",f_unn(gsl_cdf_gamma_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gamma_Pinv",f_unn(gsl_cdf_gamma_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gamma_Qinv",f_unn(gsl_cdf_gamma_Qinv),hnd_fnc_nd,P3DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_cauchy_P",f_unn(gsl_cdf_cauchy_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_cauchy_Q",f_unn(gsl_cdf_cauchy_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_cauchy_Pinv",f_unn(gsl_cdf_cauchy_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_cauchy_Qinv",f_unn(gsl_cdf_cauchy_Qinv),hnd_fnc_nd,P2DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_laplace_P",f_unn(gsl_cdf_laplace_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_laplace_Q",f_unn(gsl_cdf_laplace_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_laplace_Pinv",f_unn(gsl_cdf_laplace_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_laplace_Qinv",f_unn(gsl_cdf_laplace_Qinv),hnd_fnc_nd,P2DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_rayleigh_P",f_unn(gsl_cdf_rayleigh_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_rayleigh_Q",f_unn(gsl_cdf_rayleigh_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_rayleigh_Pinv",f_unn(gsl_cdf_rayleigh_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_rayleigh_Qinv",f_unn(gsl_cdf_rayleigh_Qinv),hnd_fnc_nd,P2DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_chisq_P",f_unn(gsl_cdf_chisq_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_chisq_Q",f_unn(gsl_cdf_chisq_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_chisq_Pinv",f_unn(gsl_cdf_chisq_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_chisq_Qinv",f_unn(gsl_cdf_chisq_Qinv),hnd_fnc_nd,P2DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_exponential_P",f_unn(gsl_cdf_exponential_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_exponential_Q",f_unn(gsl_cdf_exponential_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_exponential_Pinv",f_unn(gsl_cdf_exponential_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_exponential_Qinv",f_unn(gsl_cdf_exponential_Qinv),hnd_fnc_nd,P2DBLX));
# if NCO_GSL_VERSION >= 106
    gpr_vtr.push_back(gpr_cls("gsl_cdf_exppow_P",f_unn(gsl_cdf_exppow_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_exppow_Q",f_unn(gsl_cdf_exppow_Q),hnd_fnc_nd,P3DBLX));
# endif
    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_P",f_unn(gsl_cdf_tdist_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_Q",f_unn(gsl_cdf_tdist_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_Pinv",f_unn(gsl_cdf_tdist_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_Qinv",f_unn(gsl_cdf_tdist_Qinv),hnd_fnc_nd,P2DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_P",f_unn(gsl_cdf_fdist_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_Q",f_unn(gsl_cdf_fdist_Q),hnd_fnc_nd,P3DBLX));
# if NCO_GSL_VERSION >= 108 
    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_Pinv",f_unn(gsl_cdf_fdist_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_Qinv",f_unn(gsl_cdf_fdist_Qinv),hnd_fnc_nd,P3DBLX));
# endif // NCO_GSL_VERSION < 108

    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_P",f_unn(gsl_cdf_beta_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_Q",f_unn(gsl_cdf_beta_Q),hnd_fnc_nd,P3DBLX));
# if NCO_GSL_MAJOR_VERSION >= 2 || ( NCO_GSL_MAJOR_VERSION == 1 && NCO_GSL_MINOR_VERSION >= 8 )
    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_Pinv",f_unn(gsl_cdf_beta_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_Qinv",f_unn(gsl_cdf_beta_Qinv),hnd_fnc_nd,P3DBLX));
# endif // GSL VERSION > 1.8

    gpr_vtr.push_back(gpr_cls("gsl_cdf_flat_P",f_unn(gsl_cdf_flat_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_flat_Q",f_unn(gsl_cdf_flat_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_flat_Pinv",f_unn(gsl_cdf_flat_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_flat_Qinv",f_unn(gsl_cdf_flat_Qinv),hnd_fnc_nd,P3DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_lognormal_P",f_unn(gsl_cdf_lognormal_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_lognormal_Q",f_unn(gsl_cdf_lognormal_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_lognormal_Pinv",f_unn(gsl_cdf_lognormal_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_lognormal_Qinv",f_unn(gsl_cdf_lognormal_Qinv),hnd_fnc_nd,P3DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel1_P",f_unn(gsl_cdf_gumbel1_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel1_Q",f_unn(gsl_cdf_gumbel1_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel1_Pinv",f_unn(gsl_cdf_gumbel1_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel1_Qinv",f_unn(gsl_cdf_gumbel1_Qinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel2_P",f_unn(gsl_cdf_gumbel2_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel2_Q",f_unn(gsl_cdf_gumbel2_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel2_Pinv",f_unn(gsl_cdf_gumbel2_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_gumbel2_Qinv",f_unn(gsl_cdf_gumbel2_Qinv),hnd_fnc_nd,P3DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_weibull_P",f_unn(gsl_cdf_weibull_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_weibull_Q",f_unn(gsl_cdf_weibull_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_weibull_Pinv",f_unn(gsl_cdf_weibull_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_weibull_Qinv",f_unn(gsl_cdf_weibull_Qinv),hnd_fnc_nd,P3DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_pareto_P",f_unn(gsl_cdf_pareto_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_pareto_Q",f_unn(gsl_cdf_pareto_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_pareto_Pinv",f_unn(gsl_cdf_pareto_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_pareto_Qinv",f_unn(gsl_cdf_pareto_Qinv),hnd_fnc_nd,P3DBLX));

      gpr_vtr.push_back(gpr_cls("gsl_cdf_logistic_P",f_unn(gsl_cdf_logistic_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_logistic_Q",f_unn(gsl_cdf_logistic_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_logistic_Pinv",f_unn(gsl_cdf_logistic_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_logistic_Qinv",f_unn(gsl_cdf_logistic_Qinv),hnd_fnc_nd,P2DBLX));

//     gpr_vtr.push_back(gpr_cls("gsl_cdf_binomial_P",f_unn(gsl_cdf_binomial_P),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_binomial_Q",f_unn(gsl_cdf_binomial_Q),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_poisson_P",f_unn(gsl_cdf_poisson_P),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_poisson_Q",f_unn(gsl_cdf_poisson_Q),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_geometric_P",f_unn(gsl_cdf_geometric_P),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_geometric_Q",f_unn(gsl_cdf_geometric_Q),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_negative_binomial_P",f_unn(gsl_cdf_negative_binomial_P),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_negative_binomial_Q",f_unn(gsl_cdf_negative_binomial_Q),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_pascal_P",f_unn(gsl_cdf_pascal_P),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_pascal_Q",f_unn(gsl_cdf_pascal_Q),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_hypergeometric_P",f_unn(gsl_cdf_hypergeometric_P),hnd_fnc_nd,P1DBLX));
//     gpr_vtr.push_back(gpr_cls("gsl_cdf_hypergeometric_Q",f_unn(gsl_cdf_hypergeometric_Q),hnd_fnc_nd,P1DBLX));






} // end gsl_ini_cdf()


// Random Number Generators
void gsl_cls::gsl_ini_ran(void){
 
  gpr_vtr.push_back(gpr_cls("gsl_ran_bernoulli",f_unn(gsl_ran_bernoulli),hnd_fnc_udrx,NC_DOUBLE));
  gpr_vtr.push_back(gpr_cls("gsl_ran_bernoulli_pdf",f_unn(gsl_ran_bernoulli_pdf),hnd_fnc_ud));

  gpr_vtr.push_back(gpr_cls("gsl_ran_beta",f_unn(gsl_ran_beta),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_beta_pdf",f_unn(gsl_ran_beta_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_binomial",f_unn(gsl_ran_binomial),hnd_fnc_udrdu));
# if NCO_GSL_VERSION >= 107 
  gpr_vtr.push_back(gpr_cls("gsl_ran_binomial_knuth",f_unn(gsl_ran_binomial_knuth),hnd_fnc_udrdu));
# endif
  gpr_vtr.push_back(gpr_cls("gsl_ran_binomial_tpe",f_unn(gsl_ran_binomial_tpe),hnd_fnc_udrdu));
  gpr_vtr.push_back(gpr_cls("gsl_ran_binomial_pdf",f_unn(gsl_ran_binomial_pdf),hnd_fnc_udu));
 
  gpr_vtr.push_back(gpr_cls("gsl_ran_exponential",f_unn(gsl_ran_exponential),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_exponential_pdf",f_unn(gsl_ran_exponential_pdf),hnd_fnc_nd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_exppow",f_unn(gsl_ran_exppow),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_exppow_pdf",f_unn(gsl_ran_exppow_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_cauchy",f_unn(gsl_ran_cauchy),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_cauchy_pdf",f_unn(gsl_ran_cauchy_pdf),hnd_fnc_nd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_chisq",f_unn(gsl_ran_chisq),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_chisq_pdf",f_unn(gsl_ran_chisq_pdf),hnd_fnc_nd,P2DBLX));

  //gpr_vtr.push_back(gpr_cls("gsl_ran_dirichlet",f_unn(gsl_ran_dirichlet),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_dirichlet_pdf",f_unn(gsl_ran_dirichlet_pdf),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_dirichlet_lnpdf",f_unn(gsl_ran_dirichlet_lnpdf),hnd_fnc_rnd,P1DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_erlang",f_unn(gsl_ran_erlang),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_erlang_pdf",f_unn(gsl_ran_erlang_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_fdist",f_unn(gsl_ran_fdist),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_fdist_pdf",f_unn(gsl_ran_fdist_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_flat",f_unn(gsl_ran_flat),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_flat_pdf",f_unn(gsl_ran_flat_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma",f_unn(gsl_ran_gamma),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_int",f_unn(gsl_ran_gamma_int),hnd_fnc_ru));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_pdf",f_unn(gsl_ran_gamma_pdf),hnd_fnc_nd,P3DBLX));
# if NCO_GSL_VERSION >= 108 
  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_mt",f_unn(gsl_ran_gamma_mt),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_knuth",f_unn(gsl_ran_gamma_knuth),hnd_fnc_rnd,P2DBLX));
# endif // NCO_GSL_VERSION < 108

  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian",f_unn(gsl_ran_gaussian),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian_ratio_method",f_unn(gsl_ran_gaussian_ratio_method),hnd_fnc_rnd,P1DBLX));

# if NCO_GSL_VERSION >= 108 
  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian_ziggurat",f_unn(gsl_ran_gaussian_ziggurat),hnd_fnc_rnd,P1DBLX));
# endif // NCO_GSL_VERSION < 108

  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian_pdf",f_unn(gsl_ran_gaussian_pdf),hnd_fnc_nd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_ugaussian",f_unn(gsl_ran_ugaussian),hnd_fnc_rnd,P0DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_ugaussian_ratio_method",f_unn(gsl_ran_ugaussian_ratio_method),hnd_fnc_rnd,P0DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_ugaussian_pdf",f_unn(gsl_ran_ugaussian_pdf),hnd_fnc_nd,P1DBLX));


  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian_tail",f_unn(gsl_ran_gaussian_tail),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian_tail_pdf",f_unn(gsl_ran_gaussian_tail_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_ugaussian_tail",f_unn(gsl_ran_ugaussian_tail),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_ugaussian_tail_pdf",f_unn(gsl_ran_ugaussian_tail_pdf),hnd_fnc_nd,P2DBLX));

  //gpr_vtr.push_back(gpr_cls("gsl_ran_bivariate_gaussian",f_unn(gsl_ran_bivariate_gaussian),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_bivariate_gaussian_pdf",f_unn(gsl_ran_bivariate_gaussian_pdf),hnd_fnc_nd,P5DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_landau",f_unn(gsl_ran_landau),hnd_fnc_rnd,P0DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_landau_pdf",f_unn(gsl_ran_landau_pdf),hnd_fnc_nd,P1DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_geometric",f_unn(gsl_ran_geometric),hnd_fnc_udrx,NC_DOUBLE));
  gpr_vtr.push_back(gpr_cls("gsl_ran_geometric_pdf",f_unn(gsl_ran_geometric_pdf),hnd_fnc_ud));

  //gpr_vtr.push_back(gpr_cls("gsl_ran_hypergeometric",f_unn(gsl_ran_hypergeometric),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_hypergeometric_pdf",f_unn(gsl_ran_hypergeometric_pdf),hnd_fnc_rnd,P1DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_gumbel1",f_unn(gsl_ran_gumbel1),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gumbel1_pdf",f_unn(gsl_ran_gumbel1_pdf),hnd_fnc_nd,P3DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gumbel2",f_unn(gsl_ran_gumbel2),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gumbel2_pdf",f_unn(gsl_ran_gumbel2_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_logistic",f_unn(gsl_ran_logistic),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_logistic_pdf",f_unn(gsl_ran_logistic_pdf),hnd_fnc_nd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_lognormal",f_unn(gsl_ran_lognormal),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_lognormal_pdf",f_unn(gsl_ran_lognormal_pdf),hnd_fnc_nd,P3DBLX));

  //gpr_vtr.push_back(gpr_cls("gsl_ran_logarithmic",f_unn(gsl_ran_logarithmic),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_logarithmic_pdf",f_unn(gsl_ran_logarithmic_pdf),hnd_fnc_nd,P1DBLX));

  //gpr_vtr.push_back(gpr_cls("gsl_ran_multinomial",f_unn(gsl_ran_multinomial),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_multinomial_pdf",f_unn(gsl_ran_multinomial_pdf),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_multinomial_lnpdf",f_unn(gsl_ran_multinomial_lnpdf),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_negative_binomial",f_unn(gsl_ran_negative_binomial),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_negative_binomial_pdf",f_unn(gsl_ran_negative_binomial_pdf),hnd_fnc_rnd,P1DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_pascal",f_unn(gsl_ran_pascal),hnd_fnc_udrdu));
  gpr_vtr.push_back(gpr_cls("gsl_ran_pascal_pdf",f_unn(gsl_ran_pascal_pdf),hnd_fnc_udu));

  gpr_vtr.push_back(gpr_cls("gsl_ran_pareto",f_unn(gsl_ran_pareto),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_pareto_pdf",f_unn(gsl_ran_pareto_pdf),hnd_fnc_nd,P3DBLX));

  //gpr_vtr.pus h_back(gpr_cls("gsl_ran_poisson",f_unn(gsl_ran_poisson),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_poisson_array",f_unn(gsl_ran_poisson_array),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_poisson_pdf",f_unn(gsl_ran_poisson_pdf),hnd_fnc_rnd,P1DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_rayleigh",f_unn(gsl_ran_rayleigh),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_rayleigh_pdf",f_unn(gsl_ran_rayleigh_pdf),hnd_fnc_nd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_rayleigh_tail",f_unn(gsl_ran_rayleigh_tail),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_rayleigh_tail_pdf",f_unn(gsl_ran_rayleigh_tail_pdf),hnd_fnc_nd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_tdist",f_unn(gsl_ran_tdist),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_tdist_pdf",f_unn(gsl_ran_tdist_pdf),hnd_fnc_nd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_laplace",f_unn(gsl_ran_laplace),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_laplace_pdf",f_unn(gsl_ran_laplace_pdf),hnd_fnc_nd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_levy",f_unn(gsl_ran_levy),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_levy_skew",f_unn(gsl_ran_levy_skew),hnd_fnc_rnd,P3DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_weibull",f_unn(gsl_ran_weibull),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_weibull_pdf",f_unn(gsl_ran_weibull_pdf),hnd_fnc_nd,P3DBLX));

//   gpr_vtr.push_back(gpr_cls("gsl_ran_dir_2d",f_unn(gsl_ran_dir_2d),hnd_fnc_rnd,P1DBLX));
//   gpr_vtr.push_back(gpr_cls("gsl_ran_dir_2d_trig_method",f_unn(gsl_ran_dir_2d_trig_method),hnd_fnc_rnd,P1DBLX));
//   gpr_vtr.push_back(gpr_cls("gsl_ran_dir_3d",f_unn(gsl_ran_dir_3d),hnd_fnc_rnd,P1DBLX));
//   gpr_vtr.push_back(gpr_cls("gsl_ran_dir_nd",f_unn(gsl_ran_dir_nd),hnd_fnc_rnd,P1DBLX));

// gpr_vtr.push_back(gpr_cls("gsl_ran_shuffle",f_unn(gsl_ran_shuffle),hnd_fnc_rnd,P1DBLX));
//gpr_vtr.push_back(gpr_cls("gsl_ran_choose",f_unn(gsl_ran_choose),hnd_fnc_rnd,P1DBLX));
//gpr_vtr.push_back(gpr_cls("gsl_ran_sample",f_unn(gsl_ran_sample),hnd_fnc_rnd,P1DBLX));
//gpr_vtr.push_back(gpr_cls("gsl_ran_discrete_t",f_unn(gsl_ran_discrete_t),hnd_fnc_rnd,P1DBLX));
//gpr_vtr.push_back(gpr_cls("gsl_ran_discrete_free",f_unn(gsl_ran_discrete_free),hnd_fnc_rnd,P1DBLX));
//gpr_vtr.push_back(gpr_cls("gsl_ran_discrete",f_unn(gsl_ran_discrete),hnd_fnc_rnd,P1DBLX));
//gpr_vtr.push_back(gpr_cls("gsl_ran_discrete_pdf",f_unn(gsl_ran_discrete_pdf),hnd_fnc_rnd,P1DBLX));



  // functions from gsl_rng.h
  gpr_vtr.push_back(gpr_cls("gsl_rng_get",f_unn(gsl_rng_get),hnd_fnc_uerx,NC_NAT));
  gpr_vtr.push_back(gpr_cls("gsl_rng_uniform",f_unn(gsl_rng_uniform),hnd_fnc_rnd,P0DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_rng_uniform_pos",f_unn(gsl_rng_uniform_pos),hnd_fnc_rnd,P0DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_rng_uniform_int",f_unn(gsl_rng_uniform_int),hnd_fnc_uerx,NC_UINT));









} // gsl_ini_ran()


// GSL Statistical Functions
void gsl_cls::gsl_ini_stats(void){

  // order of gsl functions very important there are 10 types 
  // The native gsl types in order are
  // char/short/int/float/double/uchar/ushort/uint/ulong/long

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_mean), f_unn(gsl_stats_short_mean), f_unn(gsl_stats_int_mean), f_unn(gsl_stats_float_mean), 
                    f_unn(gsl_stats_mean), f_unn(gsl_stats_uchar_mean), f_unn(gsl_stats_ushort_mean), f_unn(gsl_stats_uint_mean),
                    f_unn(gsl_stats_long_mean),f_unn(gsl_stats_ulong_mean) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_mean",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_variance), f_unn(gsl_stats_short_variance), f_unn(gsl_stats_int_variance), f_unn(gsl_stats_float_variance), f_unn(gsl_stats_variance), f_unn(gsl_stats_uchar_variance), f_unn(gsl_stats_ushort_variance), f_unn(gsl_stats_uint_variance),
                    f_unn(gsl_stats_long_variance),f_unn(gsl_stats_ulong_variance) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_variance",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_sd), f_unn(gsl_stats_short_sd), f_unn(gsl_stats_int_sd), f_unn(gsl_stats_float_sd), 
                    f_unn(gsl_stats_sd), f_unn(gsl_stats_uchar_sd), f_unn(gsl_stats_ushort_sd), f_unn(gsl_stats_uint_sd),
                    f_unn(gsl_stats_long_sd),f_unn(gsl_stats_ulong_sd) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_sd",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_variance_with_fixed_mean), f_unn(gsl_stats_short_variance_with_fixed_mean), f_unn(gsl_stats_int_variance_with_fixed_mean), f_unn(gsl_stats_float_variance_with_fixed_mean), f_unn(gsl_stats_variance_with_fixed_mean), f_unn(gsl_stats_uchar_variance_with_fixed_mean), f_unn(gsl_stats_ushort_variance_with_fixed_mean), f_unn(gsl_stats_uint_variance_with_fixed_mean),
                    f_unn(gsl_stats_long_variance_with_fixed_mean),f_unn(gsl_stats_ulong_variance_with_fixed_mean) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_variance_with_fixed_mean",ARR2VTR(lcl_arr),hnd_fnc_stat2));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_sd_with_fixed_mean), f_unn(gsl_stats_short_sd_with_fixed_mean), f_unn(gsl_stats_int_sd_with_fixed_mean), f_unn(gsl_stats_float_sd_with_fixed_mean), f_unn(gsl_stats_sd_with_fixed_mean), f_unn(gsl_stats_uchar_sd_with_fixed_mean), f_unn(gsl_stats_ushort_sd_with_fixed_mean), f_unn(gsl_stats_uint_sd_with_fixed_mean),
                    f_unn(gsl_stats_long_sd_with_fixed_mean),f_unn(gsl_stats_ulong_sd_with_fixed_mean) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_sd_with_fixed_mean",ARR2VTR(lcl_arr),hnd_fnc_stat2));

  }


    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_absdev), f_unn(gsl_stats_short_absdev), f_unn(gsl_stats_int_absdev), f_unn(gsl_stats_float_absdev), f_unn(gsl_stats_absdev), f_unn(gsl_stats_uchar_absdev), f_unn(gsl_stats_ushort_absdev), f_unn(gsl_stats_uint_absdev), f_unn(gsl_stats_long_absdev),f_unn(gsl_stats_ulong_absdev) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_absdev",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_skew), f_unn(gsl_stats_short_skew), f_unn(gsl_stats_int_skew), f_unn(gsl_stats_float_skew), 
                    f_unn(gsl_stats_skew), f_unn(gsl_stats_uchar_skew), f_unn(gsl_stats_ushort_skew), f_unn(gsl_stats_uint_skew),
                    f_unn(gsl_stats_long_skew),f_unn(gsl_stats_ulong_skew) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_skew",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_kurtosis), f_unn(gsl_stats_short_kurtosis), f_unn(gsl_stats_int_kurtosis), f_unn(gsl_stats_float_kurtosis), 
                    f_unn(gsl_stats_kurtosis), f_unn(gsl_stats_uchar_kurtosis), f_unn(gsl_stats_ushort_kurtosis), f_unn(gsl_stats_uint_kurtosis),
                    f_unn(gsl_stats_long_kurtosis),f_unn(gsl_stats_ulong_kurtosis) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_kurtosis",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_lag1_autocorrelation), f_unn(gsl_stats_short_lag1_autocorrelation), f_unn(gsl_stats_int_lag1_autocorrelation), f_unn(gsl_stats_float_lag1_autocorrelation), f_unn(gsl_stats_lag1_autocorrelation), f_unn(gsl_stats_uchar_lag1_autocorrelation), f_unn(gsl_stats_ushort_lag1_autocorrelation), f_unn(gsl_stats_uint_lag1_autocorrelation), f_unn(gsl_stats_long_lag1_autocorrelation),f_unn(gsl_stats_ulong_lag1_autocorrelation) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_lag1_autocorrelation",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }


    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_variance_m), f_unn(gsl_stats_short_variance_m), f_unn(gsl_stats_int_variance_m), f_unn(gsl_stats_float_variance_m), f_unn(gsl_stats_variance_m), f_unn(gsl_stats_uchar_variance_m), f_unn(gsl_stats_ushort_variance_m), f_unn(gsl_stats_uint_variance_m),
                    f_unn(gsl_stats_long_variance_m),f_unn(gsl_stats_ulong_variance_m) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_variance_m",ARR2VTR(lcl_arr),hnd_fnc_stat2));

  }


    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_sd_m), f_unn(gsl_stats_short_sd_m), f_unn(gsl_stats_int_sd_m), f_unn(gsl_stats_float_sd_m), 
                    f_unn(gsl_stats_sd_m), f_unn(gsl_stats_uchar_sd_m), f_unn(gsl_stats_ushort_sd_m), f_unn(gsl_stats_uint_sd_m),
                    f_unn(gsl_stats_long_sd_m),f_unn(gsl_stats_ulong_sd_m) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_sd_m",ARR2VTR(lcl_arr),hnd_fnc_stat2));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_absdev_m), f_unn(gsl_stats_short_absdev_m), f_unn(gsl_stats_int_absdev_m), f_unn(gsl_stats_float_absdev_m), 
                    f_unn(gsl_stats_absdev_m), f_unn(gsl_stats_uchar_absdev_m), f_unn(gsl_stats_ushort_absdev_m), f_unn(gsl_stats_uint_absdev_m),
                    f_unn(gsl_stats_long_absdev_m),f_unn(gsl_stats_ulong_absdev_m) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_absdev_m",ARR2VTR(lcl_arr),hnd_fnc_stat2));

  }

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_lag1_autocorrelation_m), f_unn(gsl_stats_short_lag1_autocorrelation_m), f_unn(gsl_stats_int_lag1_autocorrelation_m), f_unn(gsl_stats_float_lag1_autocorrelation_m), f_unn(gsl_stats_lag1_autocorrelation_m), f_unn(gsl_stats_uchar_lag1_autocorrelation_m), f_unn(gsl_stats_ushort_lag1_autocorrelation_m), f_unn(gsl_stats_uint_lag1_autocorrelation_m), f_unn(gsl_stats_long_lag1_autocorrelation_m),f_unn(gsl_stats_ulong_lag1_autocorrelation_m) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_lag1_autocorrelation_m",ARR2VTR(lcl_arr),hnd_fnc_stat2));

  }


  gpr_vtr.push_back(gpr_cls("gsl_stats_max",f_unn(ncap_void),hnd_fnc_stat3,PS_MAX));
  gpr_vtr.push_back(gpr_cls("gsl_stats_min",f_unn(ncap_void),hnd_fnc_stat3,PS_MIN));
  gpr_vtr.push_back(gpr_cls("gsl_stats_max_index",f_unn(ncap_void),hnd_fnc_stat3,PS_MAX_IDX));
  gpr_vtr.push_back(gpr_cls("gsl_stats_min_index",f_unn(ncap_void),hnd_fnc_stat3,PS_MIN_IDX));

    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_median_from_sorted_data), f_unn(gsl_stats_short_median_from_sorted_data), f_unn(gsl_stats_int_median_from_sorted_data), f_unn(gsl_stats_float_median_from_sorted_data), f_unn(gsl_stats_median_from_sorted_data), f_unn(gsl_stats_uchar_median_from_sorted_data), f_unn(gsl_stats_ushort_median_from_sorted_data), f_unn(gsl_stats_uint_median_from_sorted_data), f_unn(gsl_stats_long_median_from_sorted_data),f_unn(gsl_stats_ulong_median_from_sorted_data) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_median_from_sorted_data",ARR2VTR(lcl_arr),hnd_fnc_stat1));

  }


    
{ f_unn lcl_arr[]={ f_unn(gsl_stats_char_quantile_from_sorted_data), f_unn(gsl_stats_short_quantile_from_sorted_data), f_unn(gsl_stats_int_quantile_from_sorted_data), f_unn(gsl_stats_float_quantile_from_sorted_data), f_unn(gsl_stats_quantile_from_sorted_data), f_unn(gsl_stats_uchar_quantile_from_sorted_data), f_unn(gsl_stats_ushort_quantile_from_sorted_data), f_unn(gsl_stats_uint_quantile_from_sorted_data), f_unn(gsl_stats_long_quantile_from_sorted_data),f_unn(gsl_stats_ulong_quantile_from_sorted_data) };   
  gpr_vtr.push_back(gpr_cls("gsl_stats_quantile_from_sorted_data",ARR2VTR(lcl_arr),hnd_fnc_stat2));

  }

  gpr_vtr.push_back(gpr_cls("gsl_stats_covariance",f_unn(ncap_void),hnd_fnc_stat4,PS_COV));

# if NCO_GSL_VERSION >= 110
  gpr_vtr.push_back(gpr_cls("gsl_stats_correlation",f_unn(ncap_void),hnd_fnc_stat4,PS_COR));
# endif // NCO_GSL_VERSION < 110

  gpr_vtr.push_back(gpr_cls("gsl_stats_pvariance",f_unn(ncap_void),hnd_fnc_stat4,PS_PVAR));
  gpr_vtr.push_back(gpr_cls("gsl_stats_ttest",f_unn(ncap_void),hnd_fnc_stat4,PS_TTST));

} // end gsl_ini_stats

var_sct * gsl_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_cls::fnd");
  bool is_mtd;
  int fdx;
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
  
  if((tr=fargs->getFirstChild())) {
    do  
      args_vtr.push_back(tr);
    while((tr=tr->getNextSibling()));    
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
  
  if(args_nbr==0) err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 
  
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
      long sz=var->sz;
      double mss_val_dbl=0.0; 
      nco_uint *uip;
      double *dp;
      
      var_sct *var_out=NULL_CEWI;
      
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
      var=nco_var_cnf_typ((nc_type)NC_UINT,var);                     
      
      (void)cast_void_nctype((nc_type)NC_UINT,&(var->val));
      uip=var->val.uip;  
      
      if(has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(dp[idx] != mss_val_dbl)
	    // note fnc_int return status flag, if 0 then no error occured
	    dp[idx]=( (*fnc_int)(uip[idx],&rslt) ? mss_val_dbl : rslt.val );             
      }else{
	for(idx=0;idx<sz;idx++) 
	  dp[idx]=( (*fnc_int)(uip[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
      }
      
      
      (void)cast_nctype_void(NC_DOUBLE,&(var_out->val));
      (void)cast_nctype_void((nc_type)NC_UINT,&(var->val));
      
      nco_var_free(var);
      var=var_out; 
      
    } 
    break;  

  case NC_INT:
    { 
      bool has_mss_val;
      int sz=var->sz;
      double mss_val_dbl=0; 
      nco_int *ip;
      double *dp;
      
      var_sct *var_out=NULL_CEWI;
      
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
      ip=var->val.ip;  
      
      if(has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(dp[idx] != mss_val_dbl)
	    // note fnc_int return status flag, if 0 then no error occured
	    dp[idx]=( (*fnc_int)(ip[idx],&rslt) ? mss_val_dbl : rslt.val );             
      }else{
	for(idx=0;idx<sz;idx++) 
	  dp[idx]=( (*fnc_int)(ip[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
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
  
  if(args_nbr <2){
    if(is_mtd) err_prn(fnc_nm,styp+" \""+sfnm+"\" requires one argument. None given"); else err_prn(fnc_nm,styp+" \""+sfnm+"\" requires two arguments");
  }
  
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
    
  case NC_DOUBLE:
    // This is handled in hnd_fnc_nd,P2DBL
     break;

     
  case NC_INT: { 
    bool has_mss_val=false;
    int sz;
    nco_int *ip1; 
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
    ip1=var1->val.ip;  
    (void)cast_void_nctype(NC_DOUBLE,&(var2->val));
    dp2=var2->val.dp;  
    
    // Ignore for now missing values from first operand
    if(var1->has_mss_val){
      //has_mss_val=true; 
      (void)cast_void_nctype(NC_INT,&var1->mss_val);
      // mss_val_ntg=*var1->mss_val.ip;
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
	dp2[idx]=( (*fnc_int)(ip1[idx],dp2[idx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
    }else{
      
      for(idx=0;idx<sz;idx++)
	// note fnc_int returns status flag, if 0 then no error
	if(dp2[idx] == mss_val_dbl || (*fnc_int)(ip1[idx],dp2[idx], &rslt) )
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
  std::string serr;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  std::string susg;        // usage string;   
  std::string var_nm;
  var_sct *var_arr[3];
  var_sct *var_out=NULL_CEWI;
  var_sct *var_tmp=NULL_CEWI;
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  args_nbr=args_vtr.size();
  
  if(fdx==PBESSEL)            
    susg="usage: status="+sfnm+"(int nmin, int nmax, double x, &var_in)";
  else{
    if(fdx==PLEGEND) susg="usage: status="+sfnm+"(int lmax, int m, double x, &var_in)";}
  
  if(args_nbr <4){
    if(is_mtd) err_prn(sfnm,styp+" requires three arguments\n"+susg); else err_prn(sfnm,styp+" requires four arguments\n"+susg);    
  }
  
  args_nbr=4;  
  
  // check fourth argument 
  
  // fourth agument must be a call by reference VAR_ID or ATT_ID   
  if(args_vtr[3]->getType() != CALL_REF ) 
    err_prn(sfnm,styp+". fourth argument must be a call by reference variable\n"+susg);   
  var_nm=args_vtr[3]->getFirstChild()->getText(); 
  var_out=prs_arg->ncap_var_init(var_nm,true);             
  
  
  if(!var_out->undefined && var_out->type !=NC_DOUBLE )
    err_prn(sfnm,styp+". Reference variable var_in must be of type DOUBLE\n"+susg);   
  
  // Deal with initial scan 
  if(prs_arg->ntl_scn){
    // evaluate first 3 args for side effect 
    for(idx=0 ; idx<args_nbr-1 ; idx++){
      var_tmp=walker.out(args_vtr[idx]);     
      var_tmp=nco_var_free(var_tmp);     
    }
    
    if(var_out->undefined)
      var_out=nco_var_free(var_out);
    else     
      (void)prs_arg->ncap_var_write(var_out,false); 
    
    return ncap_sclr_var_mk(SCS("~gsl_function"),(nc_type)NC_INT,false); 
  }
  
  // Do the real thing 
  // nb the args are fnc_int(int nmin, int nmax, double x, double * result_array);
  
  // get the first three args                
  for(idx=0;idx<3;idx++) var_arr[idx]=walker.out(args_vtr[idx]);     
  
  // do heavy listing; 
  {
    int nbr_min;
    int nbr_max;
    int status;
    int sz_out=0;
    
    double xin;     
    double *dp_out;
    
    int (*fnc_int)(int,int,double, double*);
    
    fnc_int=gpr_obj.g_args().biidpd; 
    
    var_arr[0]=nco_var_cnf_typ(NC_INT,var_arr[0]);              
    (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));
    nbr_min=var_arr[0]->val.ip[0];    
    (void)cast_nctype_void(NC_INT,&(var_arr[0]->val));
    
    var_arr[1]=nco_var_cnf_typ(NC_INT,var_arr[1]);              
    (void)cast_void_nctype(NC_INT,&(var_arr[1]->val));
    nbr_max=var_arr[1]->val.ip[0];    
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
    
    (void)cast_void_nctype(NC_DOUBLE,&(var_out->val));
    dp_out=var_out->val.dp;    
    
    if(sz_out>var_out->sz ){
      serr="Size("+nbr2sng(sz_out)+ ") of result greater than variable size("+nbr2sng(var_out->sz)+")\n"; 
      err_prn(sfnm,serr+susg);    
    }
    
    // Call the gsl function  
    status=fnc_int(nbr_min,nbr_max,xin,dp_out); 
    // write the results
    (void)cast_nctype_void(NC_DOUBLE,&(var_out->val));
    (void)prs_arg->ncap_var_write(var_out,false);           
    
    // Free args                
    for(idx=0;idx<3;idx++) (void)nco_var_free(var_arr[idx]);
    
    var_tmp=ncap_sclr_var_mk(SCS("~gsl_function"),(nco_int)status);
  }  
  // return status
  return var_tmp;
  
} // end function hnd_fnc_iidpd

var_sct *gsl_cls::hnd_fnc_idpd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){ 
  const std::string fnc_nm("hnd_fnc_idpd");
  int idx;
  int args_nbr;
  std::string serr;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  std::string susg;        // usage string;   
  std::string var_nm;
  var_sct *var_arr[2]={NULL_CEWI};
  var_sct *var_out=NULL_CEWI;
  var_sct *var_tmp=NULL_CEWI;
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  args_nbr=args_vtr.size();
  
  susg="usage: status="+sfnm+"(int lmax, double x, &var_in)";
  
  if(args_nbr < 3){
    if(is_mtd) err_prn(sfnm,styp+" requires two arguments\n"+susg); else err_prn(sfnm,styp+" requires three arguments\n"+susg);
  }
  
  args_nbr=3; // Ignore extra arguments
  
  // Check third argument 
  
  // Third agument must be a call by reference VAR_ID or ATT_ID   
  if(args_vtr[2]->getType() != CALL_REF ) 
    err_prn(sfnm,styp+". third argument must be a call by reference variable\n"+susg);   
  var_nm=args_vtr[2]->getFirstChild()->getText(); 
  var_out=prs_arg->ncap_var_init(var_nm,true);             
  
  if(!var_out->undefined && var_out->type !=NC_DOUBLE )
    err_prn(sfnm,styp+". reference variable var_in must be of type DOUBLE\n"+susg);   
  
  // Deal with initial scan 
  // nb this method returns an int which is the gsl status flag;
  if(prs_arg->ntl_scn){
    // evaluate args for side effect 
    for(idx=0 ; idx<2 ; idx++){
      var_tmp=walker.out(args_vtr[idx]);     
      var_tmp=nco_var_free(var_tmp);     
    }
    
    if(var_out->undefined)
      var_out=nco_var_free(var_out);
    else     
      (void)prs_arg->ncap_var_write(var_out,false); 
    
    return ncap_sclr_var_mk(SCS("~gsl_function"),(nc_type)NC_INT,false);   
  } 
  
  // Do the real thing 
  // nb the args are fnc_int(int lmax, double x, double * result_array);
  
  // get the first two args                
  for(idx=0;idx<2;idx++) var_arr[idx]=walker.out(args_vtr[idx]);     
  
  // do heavy listing; 
  {
    int nbr_max;
    int status;
    int sz_out;
    
    double xin;     
    double *dp_out;
    int (*fnc_int)(int,double, double*);
    
    fnc_int=gpr_obj.g_args().bidpd; 
    
    var_arr[0]=nco_var_cnf_typ(NC_INT,var_arr[0]);              
    (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));
    nbr_max=var_arr[0]->val.ip[0];    
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
    
    if(sz_out>var_out->sz ){
      serr="Size("+nbr2sng(sz_out)+ ") of result greater than variable size("+nbr2sng(var_out->sz)+")\n"; 
      err_prn(sfnm,serr+susg);    
    }
    
    // Call the gsl function  
    status=fnc_int(nbr_max,xin,dp_out); 
    (void)cast_nctype_void(NC_DOUBLE,&(var_out->val));
    // write the result
    (void)prs_arg->ncap_var_write(var_out,false);           
    
    // Free args                
    (void)nco_var_free(var_arr[0]);
    (void)nco_var_free(var_arr[1]);
    
    var_tmp=ncap_sclr_var_mk(SCS("~gsl_function"),(nco_int)status);
  }  
  return var_tmp;
} // end function hnd_fnc_idpd


var_sct *gsl_cls::hnd_fnc_nd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_nd");
  int idx;
  int jdx;
  int fdx=gpr_obj.type(); // very important 
  int args_nbr;
  int args_in_nbr(-1); // CEWI
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var_ret=NULL_CEWI; 
  var_sct **var_arr=NULL_CEWI;
  var_sct ***var_arr_ptr=NULL_CEWI;
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  args_nbr=args_vtr.size();
  
  switch(fdx) {
  case P1DBLMD:
  case P1DBL:
  case P1DBLX:
    args_in_nbr=1;
    break;    
  case P2DBLMD:
  case P2DBL:
  case P2DBLX:
    args_in_nbr=2;
    break;    
  case P3DBLMD:
  case P3DBL:
  case P3DBLX:
    args_in_nbr=3;
    break;    
  case P4DBLMD:
  case P4DBL:
  case P4DBLX:
    args_in_nbr=4;
    break;    
  
  case P5DBLX:
    args_in_nbr=5;  
    break;

  default:
    break;
  }

  if(args_nbr < args_in_nbr){
    if(is_mtd) err_prn(sfnm,styp+" requires "+nbr2sng(args_in_nbr-1)+ " arguments"); else err_prn(sfnm,styp+" requires "+ nbr2sng(args_in_nbr) + " arguments.");    
  }

  // init once we now num of args
  var_arr=(var_sct**)nco_malloc(sizeof(var_sct*)*args_in_nbr);
  var_arr_ptr=(var_sct***)nco_malloc(sizeof(var_sct**)*args_in_nbr);
    
  for(idx=0;idx<args_in_nbr;idx++){     
    var_arr[idx]=walker.out(args_vtr[idx]);
    var_arr_ptr[idx]=&var_arr[idx];
  } 
  
  // Deal with initial scan
  if(prs_arg->ntl_scn){

    if(args_in_nbr >1 )
    // nb ncap_var_att_arr_cnf() frees up sct's in var_att_ptr    
      var_ret=ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,args_in_nbr );     
    else
      var_ret=var_arr[0];
  
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
  

  if(args_in_nbr >1)
    // make variables conform  
    (void)ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,args_in_nbr );
  
  // do heavy lifting       
  {
    bool has_mss_val=false;
    int sz;
    double **dp;
    double mss_val_dbl=0.0;
              
    gsl_mode_t mde_t=ncap_gsl_mode_prec; // initialize local from global variable */
    gsl_sf_result rslt;  /* structure for result from gsl lib call */

    dp=(double**)nco_malloc(sizeof(double*)*args_in_nbr);
    
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


    case P1DBLX: {
      
      double (*fnc_dbl)(double);       
      fnc_dbl=gpr_obj.g_args().cd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]= (*fnc_dbl)(dp[0][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)

	  if(dp[0][jdx] != mss_val_dbl )
	  // note fnc_dbl returns a double
	    dp[0][jdx]=(*fnc_dbl)(dp[0][jdx]);

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

    case P2DBLX: {
      
      double (*fnc_dbl)(double,double);       
      fnc_dbl=gpr_obj.g_args().cdd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  if(dp[0][jdx]==mss_val_dbl || dp[1][jdx]==mss_val_dbl )
	    dp[0][jdx]=mss_val_dbl;
	  else
	    dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx]);
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
	  else dp[0][jdx]=rslt.val;      
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

    case P3DBLX: {
      
      double (*fnc_dbl)(double, double,double);       
      fnc_dbl=gpr_obj.g_args().cddd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx],dp[2][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  // note fnc_dbl return status flag, if 0 then no error
	  if(dp[0][jdx] == mss_val_dbl || 
	     dp[1][jdx] == mss_val_dbl ||
	     dp[2][jdx] == mss_val_dbl)
 
            dp[0][jdx]=mss_val_dbl; 
	  else
	    dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx],dp[2][jdx]);
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

     
    case P4DBLX: {
      
      double (*fnc_dbl)(double, double,double,double);       
      fnc_dbl=gpr_obj.g_args().cdddd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  if(dp[0][jdx] == mss_val_dbl || 
	     dp[1][jdx] == mss_val_dbl ||
	     dp[2][jdx] == mss_val_dbl ||
	     dp[3][jdx] == mss_val_dbl 
            ) dp[0][jdx]=mss_val_dbl;       
	  else   
	    dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx]);

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


    // whew --five double arguments only one case here
    case P5DBLX: {
      
      double (*fnc_dbl)(double, double,double,double,double);       
      fnc_dbl=gpr_obj.g_args().cddddd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx],dp[4][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  if(dp[0][jdx] == mss_val_dbl || 
	     dp[1][jdx] == mss_val_dbl ||
	     dp[2][jdx] == mss_val_dbl ||
	     dp[3][jdx] == mss_val_dbl ||
  	     dp[4][jdx] == mss_val_dbl 
            ) dp[0][jdx]=mss_val_dbl;       
	  else   
	    dp[0][jdx]=(*fnc_dbl)(dp[0][jdx],dp[1][jdx],dp[2][jdx],dp[3][jdx],dp[4][jdx]);

      }           
      
    } break;

      
    default: break;
    }// end big switch
    
    for(idx=0;idx<args_in_nbr; idx++){ 
      (void)cast_nctype_void(NC_DOUBLE,&(var_arr[idx]->val));
      if(idx>0) nco_var_free(var_arr[idx]);
    }

    dp=(double**)nco_free(dp);
    
  } // end heavy lifting
  
  var_arr_ptr=(var_sct***)nco_free(var_arr_ptr);

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
  
  
  if(args_nbr <3){
    if(is_mtd) err_prn(sfnm,styp+" requires two arguments"); else err_prn(sfnm,styp+" requires three arguments.");    
  }

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
    nco_int *ip[2]; 
    double mss_val_dbl;
    gsl_sf_result rslt;  /* structure for result from gsl lib call */
    
    // assume from here on that args conform
    sz=var_arr[0]->sz;

    (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));                  
    ip[0]=var_arr[0]->val.ip;
    
    (void)cast_void_nctype(NC_INT,&(var_arr[1]->val));                  
    ip[1]=var_arr[1]->val.ip;
    
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
	  dp[jdx]=( (*fnc_int)(ip[0][jdx],ip[1][jdx],dp[jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  // note fnc_int return status flag, if 0 then no error
	  if(dp[jdx] == mss_val_dbl || (*fnc_int)(ip[0][jdx],ip[1][jdx],dp[jdx], &rslt) )
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
  var_sct *var_ret=NULL_CEWI;
  var_sct *var_arr[3];
  var_sct **var_arr_ptr[3]; 
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  args_nbr=args_vtr.size(); 
  
  if(args_nbr <3){
    if(is_mtd) err_prn(sfnm,styp+" requires two arguments"); else err_prn(sfnm,styp+" requires three arguments.");}
  
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
    nco_int *ip; 
    double mss_val_dbl;
    gsl_sf_result rslt;  /* structure for result from gsl lib call */

    // assume from here on that args conform
    sz=var_arr[0]->sz;
    
    (void)cast_void_nctype(NC_INT,&(var_arr[0]->val));                  
    ip=var_arr[0]->val.ip;
    
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
	  dp[0][jdx]=( (*fnc_int)(ip[jdx],dp[0][jdx],dp[1][jdx],&rslt) ? NC_FILL_DOUBLE : rslt.val );
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  // note fnc_int return status flag, if 0 then no error
	  if( dp[0][jdx] == mss_val_dbl || 
	      dp[1][jdx] == mss_val_dbl || 
	      (*fnc_int)(ip[jdx],dp[0][jdx],dp[1][jdx], &rslt)
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



// handle regular arguments NC_UINT,NC_DOUBLE
var_sct *gsl_cls::hnd_fnc_ud(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_ud");
  long idx;
  int args_nbr;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var1=NULL_CEWI;
  var_sct *var2=NULL_CEWI;
  var_sct *var_ret=NULL_CEWI;
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  args_nbr=args_vtr.size();
  
  if(args_nbr <2){
    if(is_mtd) err_prn(fnc_nm,styp+" \""+sfnm+"\" requires one argument. None given"); else err_prn(fnc_nm,styp+" \""+sfnm+"\" requires two arguments");
  }
  
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
  
    
  { 
    bool has_mss_val=false;
    long sz;
    nco_uint *uip; 
    double *dp;
    double mss_val_dbl;
    double (*fnc_dbl)(unsigned,double);
    
    // convert to type double
    var1=nco_var_cnf_typ((nc_type)NC_UINT,var1); 
    var2=nco_var_cnf_typ(NC_DOUBLE,var2); 
    
    // make operands conform  
    ncap_var_att_cnf(var2,var1);
    
    sz=var1->sz; 
    fnc_dbl=gpr_obj.g_args().cud; 
    
    (void)cast_void_nctype((nc_type)NC_UINT,&(var1->val));
    uip=var1->val.uip;  
    (void)cast_void_nctype(NC_DOUBLE,&(var2->val));
    dp=var2->val.dp;  
    

    if(var2->has_mss_val){
      has_mss_val=true; 
      (void)cast_void_nctype(NC_DOUBLE,&var2->mss_val);
      mss_val_dbl=var2->mss_val.dp[0];
      (void)cast_nctype_void(NC_DOUBLE,&(var2->mss_val));
    }
    
    if(!has_mss_val){ 
      for(idx=0;idx<sz;idx++) 
	dp[idx]=(*fnc_dbl)(uip[idx],dp[idx]);
    }else{
      
      for(idx=0;idx<sz;idx++)
	if(dp[idx] != mss_val_dbl)
	  dp[idx]=(*fnc_dbl)(uip[idx],dp[idx]);

    }           
    
    (void)cast_nctype_void((nc_type)NC_UINT,&(var1->val));
    (void)cast_nctype_void(NC_DOUBLE,&(var2->val));
    nco_var_free(var1);
    
  }
    
  return var2;

}



// handle regular arguments NC_UINT,NC_DOUBLE,NC_UINT
var_sct *gsl_cls::hnd_fnc_udu(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_udu");
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
  
  
  if(args_nbr <3){
    if(is_mtd) err_prn(sfnm,styp+" requires two arguments"); else err_prn(sfnm,styp+" requires three arguments.");    
  }

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

  var_arr[0]=nco_var_cnf_typ((nc_type)NC_UINT,var_arr[0]);
  var_arr[1]=nco_var_cnf_typ(NC_DOUBLE,var_arr[1]);
  var_arr[2]=nco_var_cnf_typ((nc_type)NC_UINT,var_arr[2]);
  
  // now make variables conform 
  // make variables conform  
  (void)ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,3 );
  
  // do heavy lifting       
  {
    bool has_mss_val=false;
    int sz;
    double *dp;
    nco_uint *uip[2]; 
    double mss_val_dbl;
    double (*fnc_dbl)(unsigned,double,unsigned);       

    fnc_dbl=gpr_obj.g_args().cudu;  
     
    // assume from here on that args conform
    sz=var_arr[0]->sz;

    (void)cast_void_nctype((nc_type)NC_UINT,&(var_arr[0]->val));                  
    uip[0]=var_arr[0]->val.uip;
    
    (void)cast_void_nctype(NC_DOUBLE,&(var_arr[1]->val));                  
    dp=var_arr[1]->val.dp;
    
    (void)cast_void_nctype((nc_type)NC_UINT,&(var_arr[2]->val));                  
    uip[1]=var_arr[1]->val.uip;
    
    has_mss_val=false;  
    if(var_arr[1]->has_mss_val){
      has_mss_val=true; 
      (void)cast_void_nctype(NC_DOUBLE,&var_arr[1]->mss_val);
      mss_val_dbl=var_arr[1]->mss_val.dp[0]; 
      (void)cast_nctype_void(NC_DOUBLE,&(var_arr[1]->mss_val));
    }
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[jdx]=(*fnc_dbl)(uip[0][jdx],dp[jdx],uip[1][jdx] );
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  if(dp[jdx] != mss_val_dbl)
	   dp[jdx]=(*fnc_dbl)(uip[0][jdx],dp[jdx],uip[1][jdx] );

      } // end else           

    
    (void)cast_nctype_void((nc_type)NC_UINT,&(var_arr[0]->val));
    (void)cast_nctype_void(NC_DOUBLE,&(var_arr[1]->val));  
    (void)cast_nctype_void((nc_type)NC_UINT,&(var_arr[2]->val));
    
    (void)nco_var_free(var_arr[0]);
    (void)nco_var_free(var_arr[2]);
  }  
  
  return var_arr[1]; 

} //end hnd_fnc_udu





var_sct *gsl_cls::hnd_fnc_rnd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_rnd");
  int idx;
  int jdx;
  int fdx=gpr_obj.type(); // very important 
  int args_nbr;
  int args_in_nbr(-1); // CEWI
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var_ret; 
  var_sct **var_arr;
  var_sct ***var_arr_ptr;
  // GSL random Number stuff
  gsl_rng *ncap_rng;

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  args_nbr=args_vtr.size();
  
  switch(fdx) {
  case P0DBLX:
  case P1DBLX:
    args_in_nbr=1;
    break;    
  case P2DBLX:
    args_in_nbr=2;
    break;    
  case P3DBLX:
    args_in_nbr=3;
    break;    

  default:
    break;
  }

  if(args_nbr < args_in_nbr){
    if(is_mtd) err_prn(sfnm,styp+" requires "+nbr2sng(args_in_nbr-1)+ " arguments"); else err_prn(sfnm,styp+" requires "+ nbr2sng(args_in_nbr) + " arguments.");    
  }

  // init once we now num of args
  var_arr=(var_sct**)nco_malloc(sizeof(var_sct*)*args_in_nbr);
  var_arr_ptr=(var_sct***)nco_malloc(sizeof(var_sct**)*args_in_nbr);
    
  for(idx=0;idx<args_in_nbr;idx++){     
    var_arr[idx]=walker.out(args_vtr[idx]);
    var_arr_ptr[idx]=&var_arr[idx];
  } 
  
  // Deal with initial scan
  if(prs_arg->ntl_scn){

    if(args_in_nbr >1 )
    // nb ncap_var_att_arr_cnf() frees up sct's in var_att_ptr    
      var_ret=ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,args_in_nbr );     
    else
      var_ret=var_arr[0];
  
    if(!var_ret->undefined)
      var_ret=nco_var_cnf_typ(NC_DOUBLE,var_ret);
    
    return var_ret;
  } 


  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  

  
  for(idx=0 ; idx<args_in_nbr ; idx++){
    // convert all to type double
    var_arr[idx]=nco_var_cnf_typ(NC_DOUBLE,var_arr[idx]);
    // refrsh var pointers
    var_arr_ptr[idx]=&var_arr[idx];
  }
  

  if(args_in_nbr >1)
    // make variables conform  
    (void)ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,args_in_nbr );
  
  // do heavy lifting       
  {
    bool has_mss_val=false;
    int sz;
    double **dp;
    double mss_val_dbl=0.0;
              

    dp=(double**)nco_malloc(sizeof(double*)*args_in_nbr);
    
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

      
      // no double argument
    case P0DBLX: {
      
      double (*fnc_dbl)(const gsl_rng*);       
      fnc_dbl=gpr_obj.g_args().cr;  
      
      // no need to worry about missing values here !! 
      for(jdx=0;jdx<sz;jdx++) 
	dp[0][jdx]= (*fnc_dbl)(ncap_rng);
 
    } break;

      
      // one double argument
    case P1DBLX: {
      
      double (*fnc_dbl)(const gsl_rng*,double);       
      fnc_dbl=gpr_obj.g_args().crd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]= (*fnc_dbl)(ncap_rng,dp[0][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  if(dp[0][jdx] != mss_val_dbl )
	  // note fnc_dbl returns a double
	    dp[0][jdx]=(*fnc_dbl)(ncap_rng,dp[0][jdx]);

      }           
    } break;

    // two double argument
    case P2DBLX: {
      
      double (*fnc_dbl)(const gsl_rng*,double,double);       
      fnc_dbl=gpr_obj.g_args().crdd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]=(*fnc_dbl)(ncap_rng,dp[0][jdx],dp[1][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  if(dp[0][jdx]==mss_val_dbl || dp[1][jdx]==mss_val_dbl )
	    dp[0][jdx]=mss_val_dbl;
	  else
	    dp[0][jdx]=(*fnc_dbl)(ncap_rng,dp[0][jdx],dp[1][jdx]);
      }           
    } break;



    // Three double args    
    case P3DBLX: {
      
      double (*fnc_dbl)(const gsl_rng*,double, double,double);       
      fnc_dbl=gpr_obj.g_args().crddd;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  dp[0][jdx]=(*fnc_dbl)(ncap_rng,dp[0][jdx],dp[1][jdx],dp[2][jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  // note fnc_dbl return status flag, if 0 then no error
	  if(dp[0][jdx] == mss_val_dbl || 
	     dp[1][jdx] == mss_val_dbl ||
	     dp[2][jdx] == mss_val_dbl)
 
            dp[0][jdx]=mss_val_dbl; 
	  else
	    dp[0][jdx]=(*fnc_dbl)(ncap_rng,dp[0][jdx],dp[1][jdx],dp[2][jdx]);
      }           
    } break;

      
    default: break;

    }// end big switch
    
    for(idx=0;idx<args_in_nbr; idx++){ 
      (void)cast_nctype_void(NC_DOUBLE,&(var_arr[idx]->val));
      if(idx>0) nco_var_free(var_arr[idx]);
    }

    dp=(double**)nco_free(dp);
    
  } // end heavy lifting
  
  // free Random Number generator
  gsl_rng_free(ncap_rng);

  var_arr_ptr=(var_sct***)nco_free(var_arr_ptr);

  return var_arr[0]; 

} // hnd_fnc_rnd




var_sct *gsl_cls::hnd_fnc_ru(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_ru");
  int idx;
  int args_nbr;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var=NULL_CEWI;
  var_sct *var1=NULL_CEWI;
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  gsl_rng *ncap_rng;
  
  
  args_nbr=args_vtr.size();
  
  
  if(args_nbr==0) err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 
  
  var=walker.out(args_vtr[0]);
    
  if(prs_arg->ntl_scn){
    if(!var->undefined)
      var=nco_var_cnf_typ(NC_DOUBLE,var);                     
    return var;
  }
 

  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  
  

    { 
      long sz=var->sz;
      nco_uint  mss_val_ntg=0; 
      nco_uint *uip;
      double *dp;      

      double (*fnc_dbl)(const gsl_rng*,unsigned);
      
      fnc_dbl=gpr_obj.g_args().cru; 
      
      var=nco_var_cnf_typ((nc_type)NC_UINT,var); 
      
      var1=nco_var_dpl(var);
      var1=nco_var_cnf_typ(NC_DOUBLE,var1);         
      (void)cast_void_nctype(NC_DOUBLE,&(var1->val));
      dp=var1->val.dp;
                     
      (void)cast_void_nctype((nc_type)NC_UINT,&(var->val));
      uip=var->val.uip;  
      
      
      if(var->has_mss_val){
	(void)cast_void_nctype((nc_type)NC_UINT,&(var->mss_val));
	mss_val_ntg=var->mss_val.uip[0];    
	(void)cast_nctype_void((nc_type)NC_UINT,&(var->mss_val));
      }

      
      if(var->has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(uip[idx] != mss_val_ntg)
            dp[idx]=(*fnc_dbl)(ncap_rng,uip[idx]);   

      }else{
	for(idx=0;idx<sz;idx++) 
	  dp[idx]=(*fnc_dbl)(ncap_rng,uip[idx]);
      }
      
      
      (void)cast_nctype_void((nc_type)NC_UINT,&(var->val));
      nco_var_free(var);

     (void)cast_nctype_void(NC_DOUBLE,&(var1->val));
      
    }

     
  gsl_rng_free(ncap_rng);
  
  return var1;

} // end hnd_fnc_ru






var_sct *gsl_cls::hnd_fnc_udrx(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_udrx");
  int idx;
  int args_nbr;
  nc_type type;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var=NULL_CEWI;
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  gsl_rng *ncap_rng;
  
  
  args_nbr=args_vtr.size();
  
        type=gpr_obj.type(); 
  
  if(args_nbr==0) err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 
  
  var=walker.out(args_vtr[0]);
    
  if(prs_arg->ntl_scn){
    if(!var->undefined)
      var=nco_var_cnf_typ((nc_type)NCO_TYP_GSL_UINT,var);                     
    return var;
  }
 

  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  
  
  switch(type){ 
    
  case NC_UINT:
    { 
      long sz=var->sz;
      nco_uint  mss_val_uint=0; 
      nco_uint *uip;
      
      unsigned (*fnc_int)(const gsl_rng*,unsigned);
      
      fnc_int=gpr_obj.g_args().dru; 
      
      var=nco_var_cnf_typ((nc_type)NC_UINT,var);                     
      (void)cast_void_nctype((nc_type)NC_UINT,&(var->val));
      uip=var->val.uip;  
      
      if(var->has_mss_val)
        mss_val_uint=var->mss_val.uip[0]; 
       
      
      if(var->has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(uip[idx] != mss_val_uint)
            uip[idx]=(*fnc_int)(ncap_rng,uip[idx]);   

      }else{
	for(idx=0;idx<sz;idx++) 
	  uip[idx]=(*fnc_int)(ncap_rng,uip[idx]);
      }
      
      
      (void)cast_nctype_void((nc_type)NC_UINT,&(var->val));
      
      
    } 
    break;  
    
  case NC_DOUBLE:
    { 
      bool has_mss_val;
      int sz=var->sz;
      nco_uint *uip;
      double mss_val_dbl;
      double *dp;

      var_sct *var_out;
      
      unsigned (*fnc_int)(const gsl_rng*,double);
      
      

      fnc_int=gpr_obj.g_args().drd; 

      // convert to double
      var=nco_var_cnf_typ(NC_DOUBLE,var);                     

      var_out=nco_var_dpl(var);

      var_out=nco_var_cnf_typ((nc_type)NC_UINT,var_out);                        

      (void)cast_void_nctype((nc_type)NC_UINT,&(var_out->val));
      uip=var_out->val.uip;  
      
      
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
	   uip[idx]=(*fnc_int)(ncap_rng,dp[idx]);
      }else{
	for(idx=0;idx<sz;idx++) 
	  uip[idx]=(*fnc_int)(ncap_rng,dp[idx]);
      }
      
      
      (void)cast_nctype_void(NC_DOUBLE,&(var->val));
      var=nco_var_free(var);
     
      (void)cast_nctype_void((nc_type)NC_UINT,&(var_out->val));     
      var=var_out; 
  

    } 
    break;
    
  default:
    break; 
    
  } // end big switch 

  // convert var if necessary
  var=nco_var_cnf_typ((nc_type)NCO_TYP_GSL_UINT,var);                     

  gsl_rng_free(ncap_rng);
  
  return var;
} // end hnd_fnc_udrx



var_sct *gsl_cls::hnd_fnc_udrdu(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_udrdu");
  int idx;
  long jdx;
  int args_nbr;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var_ret; 
  var_sct *var_arr[2];
  var_sct **var_arr_ptr[2]; 
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  gsl_rng *ncap_rng;

  
  args_nbr=args_vtr.size(); 
  
  
  if(args_nbr <2){
    if(is_mtd) err_prn(sfnm,styp+" requires one argument"); else err_prn(sfnm,styp+" requires two arguments.");    
  }

  for(idx=0; idx< 2 ;idx++){     
    var_arr[idx]=walker.out(args_vtr[idx]);
    var_arr_ptr[idx]=&var_arr[idx];
  } 
  
  // Deal with initial scan
  if(prs_arg->ntl_scn){
    
    // nb ncap_var_att_arr_cnf() frees up sct's in var_att_ptr    
    var_ret=ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,2 );     
    if(!var_ret->undefined)
      var_ret=nco_var_cnf_typ((nc_type)NCO_TYP_GSL_UINT,var_ret);
    
    return var_ret;
  } 


  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  


  var_arr[0]=nco_var_cnf_typ(NC_DOUBLE,var_arr[0]);
  var_arr[1]=nco_var_cnf_typ((nc_type)NC_UINT,var_arr[1]);
  
  // now make variables conform 
  // make variables conform  
  (void)ncap_var_att_arr_cnf(prs_arg->ntl_scn,var_arr_ptr,2 );
  
  // do heavy lifting       
  {
    bool has_mss_val=false;
    long sz;
    double *dp;
    nco_uint  mss_val_uint=0; 
    nco_uint *uip;

    double mss_val_dbl;
    
    // assume from here on that args conform
    sz=var_arr[0]->sz;

    (void)cast_void_nctype(NC_DOUBLE,&(var_arr[0]->val));                  
    dp=var_arr[0]->val.dp;
    
    (void)cast_void_nctype((nc_type)NC_UINT,&(var_arr[1]->val));                  
    uip=var_arr[1]->val.uip;
    
    
    has_mss_val=false;  
    if(var_arr[0]->has_mss_val){
      has_mss_val=true; 
      (void)cast_void_nctype(NC_DOUBLE,&var_arr[0]->mss_val);
      mss_val_dbl=var_arr[0]->mss_val.dp[0]; 
      (void)cast_nctype_void(NC_DOUBLE,&(var_arr[0]->mss_val));
      // use C imlicit conversion  
      mss_val_uint=(nco_uint)mss_val_dbl;  
    } 
    
    // do the deed !!!
    {
      unsigned int (*fnc_int)(const gsl_rng* ,double,unsigned);       
      fnc_int=gpr_obj.g_args().drdu;  
      
      if(!has_mss_val){ 
	for(jdx=0;jdx<sz;jdx++) 
	  uip[jdx]=(*fnc_int)(ncap_rng,dp[jdx],uip[jdx]);
      }else{
	
	for(jdx=0;jdx<sz;jdx++)
	  if(dp[jdx] != mss_val_dbl )
             uip[jdx]=(*fnc_int)(ncap_rng,dp[jdx],uip[jdx]);  
	  else
	     uip[jdx]=mss_val_uint;      
      } // end else           
    
    } // done the deed !!
    

    (void)cast_nctype_void(NC_DOUBLE,&(var_arr[0]->val));
    (void)cast_nctype_void((nc_type)NC_UINT,&(var_arr[1]->val));  

    
    (void)nco_var_free(var_arr[0]);

  }  

  gsl_rng_free(ncap_rng);


  // Convert if necessary
  var_arr[1]=nco_var_cnf_typ((nc_type)NCO_TYP_GSL_UINT,var_arr[1]);

  return var_arr[1]; 


} // end hnd_fnc_udrdu







var_sct *gsl_cls::hnd_fnc_uerx(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_udrx");
  int args_nbr;
  long idx;
  unsigned long rng_max_lng;
  const char *rng_nm_sng;
  nc_type type;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var=NULL_CEWI;
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  gsl_rng *ncap_rng;
  
  
  args_nbr=args_vtr.size();
  
  type=gpr_obj.type(); 
  
  if(args_nbr==0) err_prn(fnc_nm,styp+" \""+sfnm+"\" has been called with no arguments"); 
  
  var=walker.out(args_vtr[0]);
    
  if(prs_arg->ntl_scn){
    if(!var->undefined)
      var=nco_var_cnf_typ((nc_type)NCO_TYP_GSL_UINT,var);                    
    return var;
  }


  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  

  rng_max_lng=gsl_rng_max(ncap_rng);
  rng_nm_sng=gsl_rng_name(ncap_rng);

  switch(type){ 
    

  // This case is a wrapper for the single function
  // gsl_rng_get(). It shouldn't be used for any other function
  case NC_NAT:
    { 
      long sz=var->sz;
      nco_uint *uip;
      
      unsigned long int (*fnc_int)(const gsl_rng*);
      
      fnc_int=gpr_obj.g_args().er; 

      /* bomb out if max value of number generator can't be held in regular int */
      if( NCO_TYP_GSL_UINT==NC_INT &&  rng_max_lng > INT_MAX ){
        ostringstream os; 
        os<<"Possible integer overflow. You are using the random number generator \"" <<rng_nm_sng<<"\". This can return a value up to "<<rng_max_lng<<". This is greater than "<<INT_MAX<<" - the maximum value that can be stored in the netcdf datatype NC_INT. Consider using another random number generator e.g., ran0,fishman18 or knuthran. Consult the GSL manual for details. Alternatively recompile nco for netcdf4 and set the compile flag NCO_TYP_GSL_UINT=NC_UINT\n"; 

        err_prn(sfnm,os.str());
      } 


      
      var=nco_var_cnf_typ((nc_type)NC_UINT,var);                     
      (void)cast_void_nctype((nc_type)NC_UINT,&(var->val));
      uip=var->val.uip;  

      for(idx=0;idx<sz;idx++) 
	uip[idx]=(*fnc_int)(ncap_rng);
      
      (void)cast_nctype_void((nc_type)NC_UINT,&(var->val));
      
      
    } 
    break;  


  // This case is a wrapper for the single function
  // gsl_rng_uniform_int(). It shouldn't be used for any other function
  case NC_UINT:
    { 
      bool has_mss_val;
      long sz=var->sz;
      nco_uint64 *ui64p;
      nco_uint64 mss_val_uint64;

      unsigned long int(*fnc_int)(const gsl_rng*,unsigned long int);
      
      fnc_int=gpr_obj.g_args().eru; 
 

      var=nco_var_cnf_typ((nc_type)NC_UINT64,var);                        

      (void)cast_void_nctype((nc_type)NC_UINT64,&(var->val));
      ui64p=var->val.ui64p;  
      
      //check requested  max values
      if( NCO_TYP_GSL_UINT==NC_INT && rng_max_lng > INT_MAX)
        for(idx=0 ; idx<sz ;idx++)
          if( ui64p[idx]>INT_MAX ) {
	    // bomb out if necessary  
            ostringstream os; 
            os<<"Possible integer overflow. You have requested the generation of integers up to the value of " <<ui64p[idx]<<" .This is greater than "<<INT_MAX<<" - the maximum value that can be stored in the netcdf datatype NC_INT. Consider using another random number generator e.g., ran0,fishman18 or knuthran. Consult the GSL manual for details. Alternatively recompile nco for netcdf4 and set the compile flag NCO_TYP_GSL_UINT=NC_UINT\n"; 
            err_prn(sfnm,os.str());
          }
          
      has_mss_val=false;
      if(var->has_mss_val){
	has_mss_val=true;
	(void)cast_void_nctype((nc_type)NC_UINT64,&(var->mss_val));
	mss_val_uint64=var->mss_val.ui64p[0];    
	(void)cast_nctype_void((nc_type)NC_UINT64,&(var->mss_val));
      }
      
      if(has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(ui64p[idx] != mss_val_uint64)
	   ui64p[idx]=(*fnc_int)(ncap_rng,ui64p[idx]);
      }else{
	for(idx=0;idx<sz;idx++) 
	  ui64p[idx]=(*fnc_int)(ncap_rng,ui64p[idx]);
      }
      
      
      (void)cast_nctype_void((nc_type)NC_UINT64,&(var->val));     

    }
    break;
    
  default:
    break; 
    
  } // end big switch 

  gsl_rng_free(ncap_rng);
  
  // convert if necessary
  var=nco_var_cnf_typ((nc_type)NCO_TYP_GSL_UINT,var);                        

  return var;
} // end hnd_fnc_uerx

     
var_sct *gsl_cls::hnd_fnc_stat1(bool&,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_stat1");
  int idx;
  int nbr_args;
  int in_nbr_args;
  std::string susg;
  std::string sfnm=gpr_obj.fnm();
  var_sct *var[3];
  double r_val;
   
  
  var[0]=var[1]=var[2]=(var_sct*)NULL;    
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  nbr_args=args_vtr.size();
  
  susg=susg="usage: double_val="+sfnm+"(var_data, data_stride?, n?)";

  r_val=0.0;
  
  if(nbr_args <1){
    err_prn(sfnm,"Function requires at least one argument.\n"+susg  ); 
  }
   
  if(nbr_args >3)
    in_nbr_args=3;
  else
    in_nbr_args=nbr_args; 


  for(idx=0 ;idx<in_nbr_args; idx++)
    var[idx]=walker.out(args_vtr[idx]);

  
  // Deal with initial scan
  if(prs_arg->ntl_scn){

    for(idx=0 ;idx<in_nbr_args; idx++)
      var[idx]=nco_var_free(var[idx]);

    return ncap_sclr_var_mk(static_cast< std::string>("~hnd_fnc_stat1"),NC_DOUBLE,false);   
  }

  // do the heavy lifting 
  {
    size_t sz; 
    size_t d_srd;
    
    // first arg the data 
   (void)cast_void_nctype(var[0]->type,&(var[0]->val));  
   

   // 2nd arg data stride
   if(var[1] !=(var_sct*)NULL){
     var[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var[1]);
     (void)cast_void_nctype((nc_type)NC_UINT64,&(var[1]->val));  
     d_srd=var[1]->val.ui64p[0];       
     (void)cast_nctype_void((nc_type)NC_UINT64,&(var[1]->val));
     var[1]=nco_var_free(var[1]);   
   }else{
     d_srd=1L; 
   }

   // 3rd arg size
   if(var[2] !=(var_sct*)NULL){
     var[2]=nco_var_cnf_typ((nc_type)NC_UINT64,var[2]);
     (void)cast_void_nctype((nc_type)NC_UINT64,&(var[2]->val));  
     sz=var[2]->val.ui64p[0];       
     (void)cast_nctype_void((nc_type)NC_UINT64,&(var[2]->val));
     var[2]=nco_var_free(var[2]);   
   }else{
     sz=1+ (var[0]->sz-1)/d_srd;
   }
   
   // Check hyperslab limits
   if( 1+ (sz-1)*d_srd >(size_t)var[0]->sz){
     err_prn(sfnm,"Requested hyperslab with stride="+nbr2sng(d_srd)+" and n="+ nbr2sng(sz)+" doesn't fit into variable \""+string(var[0]->nm)+"\" with size="+nbr2sng(var[0]->sz)); 
    }
   
   // remember we are dealing with g_args() -- an array of function pointers here
   // the order of gsl function pointers is significant -it is
  // char/short/int/float/double/uchar/ushort/uint/ulong/long
   switch(var[0]->type){
     case NC_FLOAT:  r_val=gpr_obj.g_args(3).csfpss( var[0]->val.fp,d_srd,sz);break;
     case NC_DOUBLE: r_val=gpr_obj.g_args(4).csdpss( var[0]->val.dp,d_srd,sz);break;

     case NC_INT:    // NC_INT rpresented as int in nco
                     #if NCO_INT==NCO_TYP_INT
                       r_val=gpr_obj.g_args(2).csipss(var[0]->val.ip,d_srd,sz);
                     // NC_INT rpresented as long  in nco
		     #else
                       r_val=gpr_obj.g_args(8).cslpss(var[0]->val.ip,d_srd,sz);
                     #endif
                     break;

     case NC_SHORT:  r_val=gpr_obj.g_args(1).csspss( var[0]->val.sp,d_srd,sz);break;
     case NC_CHAR:   r_val=gpr_obj.g_args(0).cscpss((const char*)var[0]->val.cp,d_srd,sz);break;
     case NC_BYTE:   r_val=gpr_obj.g_args(0).cscpss((const char*)var[0]->val.bp,d_srd,sz);break;
       
#ifdef ENABLE_NETCDF4
     case NC_UBYTE:  r_val=gpr_obj.g_args(5).csucpss((const unsigned char*)var[0]->val.ubp,d_srd,sz);break;
     case NC_USHORT: r_val=gpr_obj.g_args(6).csuspss( var[0]->val.usp,d_srd,sz);break;
     case NC_UINT:   r_val=gpr_obj.g_args(7).csuipss(var[0]->val.uip,d_srd,sz);break;

     case NC_INT64:  
                     if( sizeof(long)!=sizeof(long long int) )
		       err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_INT64");
                      
                      r_val=gpr_obj.g_args(8).cslpss((const long*)var[0]->val.i64p,d_srd,sz);
                      break;
     case NC_UINT64: 
                     if( sizeof(unsigned long)!=sizeof(unsigned long long int) )
		       err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_UINT64");

                      r_val=gpr_obj.g_args(9).csulpss((const unsigned long*)var[0]->val.ui64p,d_srd,sz);
                     break;

     case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;
     
   } // end switch  

   (void)cast_nctype_void(var[0]->type,&(var[0]->val));

  } // end heavy lifting
 
 var[0]=nco_var_free(var[0]);


 return ncap_sclr_var_mk(SCS("~gsl_stt2_function"),r_val);  
}


var_sct *gsl_cls::hnd_fnc_stat2(bool&,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_stat2");
  int idx;
  int args_nbr;
  std::string susg;
  std::string sfnm=gpr_obj.fnm();
  var_sct *var_arr[4];
  double r_val;

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  r_val=0.0;
  
  args_nbr=args_vtr.size();
  
  susg=susg="usage: double_val="+sfnm+"(var_data, data_stride, n, double_val)";

  if(args_nbr <4){
    err_prn(sfnm,"Function requires four arguments.\n"+susg  ); 
  }
   

  for(idx=0;idx<4;idx++)
    var_arr[idx]=walker.out(args_vtr[idx]);


  
  // Deal with initial scan
  if(prs_arg->ntl_scn){
   
    for(idx=0;idx<4;idx++)
      var_arr[idx]=nco_var_free(var_arr[idx]);
    
    return ncap_sclr_var_mk(static_cast< std::string>("~hnd_fnc_stat2"),(nc_type)NC_DOUBLE,false);   
  }

  // do the heavy lifting 
  {
    double dmean; 
    size_t sz; 
    size_t d_srd;
    
    // first arg the data 
   (void)cast_void_nctype(var_arr[0]->type,&(var_arr[0]->val));  
   

   // 2nd arg data stride
   var_arr[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[1]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[1]->val));  
   d_srd=(size_t) var_arr[1]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[1]->val));


   // 3nd arg n --number of elements to perform function over
   var_arr[2]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[2]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[2]->val));  
   sz=(size_t) var_arr[2]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[2]->val));

   // 4th arg the mean
   var_arr[3]=nco_var_cnf_typ(NC_DOUBLE,var_arr[3]);
   (void)cast_void_nctype(NC_DOUBLE,&(var_arr[3]->val));  
   dmean=var_arr[3]->val.dp[0];       
   (void)cast_nctype_void(NC_DOUBLE,&(var_arr[3]->val));

   
   // Check hyperslab limits
   if( 1+ (sz-1)*d_srd > (size_t)var_arr[0]->sz){
     err_prn(sfnm,"Requested hyperslab with stride="+nbr2sng(d_srd)+" and n="+ nbr2sng(sz)+" doesn't fit into variable \""+string(var_arr[0]->nm)+"\" with size="+nbr2sng(var_arr[0]->sz)); 
    }



   
   // remember we are dealing with g_args() -- an array of function pointers here
   // the order of gsl function pointers is significant -it is
  // char/short/int/float/double/uchar/ushort/uint/ulong/long
   switch(var_arr[0]->type){
     case NC_FLOAT:  r_val=gpr_obj.g_args(3).csfpssd( var_arr[0]->val.fp,d_srd,sz,dmean);break;
     case NC_DOUBLE: r_val=gpr_obj.g_args(4).csdpssd( var_arr[0]->val.dp,d_srd,sz,dmean);break;

     case NC_INT:    // NC_INT rpresented as int in nco
                     #if NCO_INT==NCO_TYP_INT
                       r_val=gpr_obj.g_args(2).csipssd(var_arr[0]->val.ip,d_srd,sz,dmean);
                     // NC_INT rpresented as long  in nco
		     #else
                       r_val=gpr_obj.g_args(8).cslpssd(var_arr[0]->val.ip,d_srd,sz,dmean);
                     #endif
                     break;

     case NC_SHORT:  r_val=gpr_obj.g_args(1).csspssd( var_arr[0]->val.sp,d_srd,sz,dmean);break;
     case NC_CHAR:   r_val=gpr_obj.g_args(0).cscpssd((const char*)var_arr[0]->val.cp,d_srd,sz,dmean);break;
     case NC_BYTE:   r_val=gpr_obj.g_args(0).cscpssd((const char*)var_arr[0]->val.bp,d_srd,sz,dmean);break;
       
#ifdef ENABLE_NETCDF4
     case NC_UBYTE:  r_val=gpr_obj.g_args(5).csucpssd((const unsigned char*)var_arr[0]->val.ubp,d_srd,sz,dmean);break;
     case NC_USHORT: r_val=gpr_obj.g_args(6).csuspssd( var_arr[0]->val.usp,d_srd,sz,dmean);break;
     case NC_UINT:   r_val=gpr_obj.g_args(7).csuipssd(var_arr[0]->val.uip,d_srd,sz,dmean);break;

     case NC_INT64:  
                     if( sizeof(long)!=sizeof(long long int) )
		       err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_INT64");
                      
                      r_val=gpr_obj.g_args(8).cslpssd((const long*)var_arr[0]->val.i64p,d_srd,sz,dmean);
                      break;
     case NC_UINT64: 
                     if( sizeof(unsigned long)!=sizeof(unsigned long long int) )
		       err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_UINT64");

                      r_val=gpr_obj.g_args(9).csulpssd((const unsigned long*)var_arr[0]->val.ui64p,d_srd,sz,dmean);
                     break;

     case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;
     
   } // end switch  

   (void)cast_nctype_void(var_arr[0]->type,&(var_arr[0]->val));

  } // end heavy lifting
 
  
  // free vars 
  for(idx=0;idx<4;idx++)
    var_arr[idx]=nco_var_free(var_arr[idx]);
   

 return ncap_sclr_var_mk(SCS("~gsl_stt2_function"),r_val);  

}


var_sct *gsl_cls::hnd_fnc_stat3(bool&,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_stat3");
  int idx;
  int nbr_args;
  int in_nbr_args;
  int fdx=gpr_obj.type(); // very important
  std::string susg;
  std::string sfnm=gpr_obj.fnm();
  var_sct *var[3];
  var_sct *var_ret=NULL_CEWI;
   
  
  var[0]=var[1]=var[2]=(var_sct*)NULL;    
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  nbr_args=args_vtr.size();
  
  susg=susg="usage: double_val="+sfnm+"(var_data, data_stride?, n?)";

  if(nbr_args <1){
    err_prn(sfnm,"Function requires at least one argument.\n"+susg  ); 
  }
   
  if(nbr_args >3)
    in_nbr_args=3;
  else
    in_nbr_args=nbr_args; 


  for(idx=0 ;idx<in_nbr_args; idx++)
    var[idx]=walker.out(args_vtr[idx]);




  // Deal with initial scan
  if(prs_arg->ntl_scn){


    if(var[0]->undefined)
      var_ret=ncap_var_udf("~rhs_undefined");

    else if(fdx ==PS_MIN_IDX ||fdx==PS_MAX_IDX) 
      var_ret=ncap_sclr_var_mk(static_cast< std::string>("~hnd_fnc_stat3"),NC_INT,false);   

    else if(fdx==PS_MIN || fdx==PS_MAX)
      var_ret=ncap_sclr_var_mk(static_cast< std::string>("~hnd_fnc_stat3"),(nc_type)var[0]->type,false);   


    for(idx=0 ;idx<in_nbr_args; idx++)
      var[idx]=nco_var_free(var[idx]);


    return var_ret; 

  }



  if(var[0]->type==(nc_type)NC_INT64 && sizeof(long)!=sizeof(long long int) )
    err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_INT64");

  if(var[0]->type==(nc_type)NC_UINT64 && sizeof(unsigned long)!=sizeof(unsigned long long int) )
    err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_UINT64");



  // do the heavy lifting 
  {
    size_t sz; 
    size_t d_srd;
    
    // first arg the data 
   (void)cast_void_nctype(var[0]->type,&(var[0]->val));  
   

   // 2nd arg data stride
   if(var[1] !=(var_sct*)NULL){
     var[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var[1]);
     (void)cast_void_nctype((nc_type)NC_UINT64,&(var[1]->val));  
     d_srd=var[1]->val.ui64p[0];       
     (void)cast_nctype_void((nc_type)NC_UINT64,&(var[1]->val));
     var[1]=nco_var_free(var[1]);   
   }else{
     d_srd=1L; 
   }

   // 3rd arg size
   if(var[2] !=(var_sct*)NULL){
     var[2]=nco_var_cnf_typ((nc_type)NC_UINT64,var[2]);
     (void)cast_void_nctype((nc_type)NC_UINT64,&(var[2]->val));  
     sz=var[2]->val.ui64p[0];       
     (void)cast_nctype_void((nc_type)NC_UINT64,&(var[2]->val));
     var[2]=nco_var_free(var[2]);   
   }else{
     sz=1L+(var[0]->sz-1)/d_srd;
   }
   
   // Check hyperslab limits
   if(1L+(sz-1)*d_srd > (size_t)var[0]->sz){
     err_prn(sfnm,"Requested hyperslab with stride="+nbr2sng(d_srd)+" and n="+ nbr2sng(sz)+" does not fit into variable \""+string(var[0]->nm)+"\" with size="+nbr2sng(var[0]->sz)); 
    }
  
   switch(fdx){
    
   case PS_MAX_IDX:{
     nco_int r_val;
     r_val=0;

     switch(var[0]->type){
       case NC_FLOAT:  r_val=gsl_stats_float_max_index( var[0]->val.fp,d_srd,sz);break;
       case NC_DOUBLE: r_val=gsl_stats_max_index( var[0]->val.dp,d_srd,sz);break;

       case NC_INT:    // NC_INT rpresented as int in nco
                       #if NCO_INT==NCO_TYP_INT
                         r_val=gsl_stats_int_max_index(var[0]->val.ip,d_srd,sz);
                       // NC_INT rpresented as long  in nco
		       #else
                         r_val=gsl_stats_long_max_index(var[0]->val.ip,d_srd,sz);
                       #endif
                       break;

       case NC_SHORT:  r_val=gsl_stats_short_max_index( var[0]->val.sp,d_srd,sz);break;
       case NC_CHAR:   r_val=gsl_stats_char_max_index((const char*)var[0]->val.cp,d_srd,sz);break;
       case NC_BYTE:   r_val=gsl_stats_char_max_index((const char*)var[0]->val.bp,d_srd,sz);break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  r_val=gsl_stats_uchar_max_index((const unsigned char*)var[0]->val.ubp,d_srd,sz);break;
       case NC_USHORT: r_val=gsl_stats_ushort_max_index( var[0]->val.usp,d_srd,sz);break;
       case NC_UINT:   r_val=gsl_stats_uint_max_index(var[0]->val.uip,d_srd,sz);break;
       case NC_INT64:  r_val=gsl_stats_long_max_index((const long*)var[0]->val.i64p,d_srd,sz); break;
       case NC_UINT64:  r_val=gsl_stats_ulong_max_index((const unsigned long*)var[0]->val.ui64p,d_srd,sz); break;
       case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;    
    } 
    var_ret=ncap_sclr_var_mk(SCS("~gsl_stt3_function"),r_val);    
   } break;  
   
    
   case PS_MIN_IDX:{
     nco_int r_val;
     r_val=0;

     switch(var[0]->type){
       case NC_FLOAT:  r_val=gsl_stats_float_min_index( var[0]->val.fp,d_srd,sz);break;
       case NC_DOUBLE: r_val=gsl_stats_min_index( var[0]->val.dp,d_srd,sz);break;

       case NC_INT:    // NC_INT rpresented as int in nco
                       #if NCO_INT==NCO_TYP_INT
                         r_val=gsl_stats_int_min_index(var[0]->val.ip,d_srd,sz);
                       // NC_INT rpresented as long  in nco
		       #else
                         r_val=gsl_stats_long_min_index(var[0]->val.ip,d_srd,sz);
                       #endif
                       break;

       case NC_SHORT:  r_val=gsl_stats_short_min_index( var[0]->val.sp,d_srd,sz);break;
       case NC_CHAR:   r_val=gsl_stats_char_min_index((const char*)var[0]->val.cp,d_srd,sz);break;
       case NC_BYTE:   r_val=gsl_stats_char_min_index((const char*)var[0]->val.bp,d_srd,sz);break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  r_val=gsl_stats_uchar_min_index((const unsigned char*)var[0]->val.ubp,d_srd,sz);break;
       case NC_USHORT: r_val=gsl_stats_ushort_min_index( var[0]->val.usp,d_srd,sz);break;
       case NC_UINT:   r_val=gsl_stats_uint_min_index(var[0]->val.uip,d_srd,sz);break;
       case NC_INT64:  r_val=gsl_stats_long_min_index((const long*)var[0]->val.i64p,d_srd,sz); break;
       case NC_UINT64:  r_val=gsl_stats_ulong_min_index((const unsigned long*)var[0]->val.ui64p,d_srd,sz); break;
       case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;    
     } 
    var_ret=ncap_sclr_var_mk(SCS("~gsl_stt3_function"),r_val);    
   } break;  
   
    
   case PS_MIN:{

     switch(var[0]->type){
       case NC_FLOAT:  
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(float)gsl_stats_float_min( var[0]->val.fp,d_srd,sz)); break;
       case NC_DOUBLE: 
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(double)gsl_stats_min( var[0]->val.dp,d_srd,sz));break;

       case NC_INT:  
             // NC_INT rpresented as int in nco
            #if NCO_INT==NCO_TYP_INT
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_int)gsl_stats_int_min(var[0]->val.ip,d_srd,sz));
            // NC_INT rpresented as long  in nco
	    #else
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_int)gsl_stats_long_min(var[0]->val.ip,d_srd,sz));
            #endif
            break;

       case NC_SHORT:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_short)gsl_stats_short_min( var[0]->val.sp,d_srd,sz));break;
       case NC_CHAR:   
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_char)gsl_stats_char_min((const char*)var[0]->val.cp,d_srd,sz));break;
       case NC_BYTE:   
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_byte)gsl_stats_char_min((const char*)var[0]->val.bp,d_srd,sz));break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_ubyte)gsl_stats_uchar_min((const unsigned char*)var[0]->val.ubp,d_srd,sz));break;
       case NC_USHORT: 
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_ushort)gsl_stats_ushort_min( var[0]->val.usp,d_srd,sz));
            break;
       case NC_UINT:   
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_uint)gsl_stats_uint_min(var[0]->val.uip,d_srd,sz));
           break;
       case NC_INT64:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_int64)gsl_stats_long_min((const long*)var[0]->val.i64p,d_srd,sz));
           break;
       case NC_UINT64:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_uint64)gsl_stats_ulong_min((const unsigned long*)var[0]->val.ui64p,d_srd,sz)); break;

       case NC_STRING: break;
#endif /* !ENABLE_NETCDF4 */
		    
     default: nco_dfl_case_nc_type_err(); break;    
     } 

   } break;  
  
 
   case PS_MAX:{

     switch(var[0]->type){
       case NC_FLOAT:  
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(float)gsl_stats_float_max( var[0]->val.fp,d_srd,sz)); break;
       case NC_DOUBLE: 
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(double)gsl_stats_max( var[0]->val.dp,d_srd,sz));break;

       case NC_INT:  
             // NC_INT rpresented as int in nco
            #if NCO_INT==NCO_TYP_INT
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_int)gsl_stats_int_max(var[0]->val.ip,d_srd,sz));
            // NC_INT rpresented as long  in nco
	    #else
               var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_int)gsl_stats_long_max(var[0]->val.ip,d_srd,sz));
            #endif
            break;

       case NC_SHORT:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_short)gsl_stats_short_max( var[0]->val.sp,d_srd,sz));break;
       case NC_CHAR:   
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_char)gsl_stats_char_max((const char*)var[0]->val.cp,d_srd,sz));break;
       case NC_BYTE:   
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_byte)gsl_stats_char_max((const char*)var[0]->val.bp,d_srd,sz));break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_ubyte)gsl_stats_uchar_max((const unsigned char*)var[0]->val.ubp,d_srd,sz));break;
       case NC_USHORT: 
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_ushort)gsl_stats_ushort_max( var[0]->val.usp,d_srd,sz));
            break;
       case NC_UINT:   
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_uint)gsl_stats_uint_max(var[0]->val.uip,d_srd,sz));
           break;
       case NC_INT64:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_int64)gsl_stats_long_max((const long*)var[0]->val.i64p,d_srd,sz));
           break;
       case NC_UINT64:  
              var_ret=ncap_sclr_var_mk("~gsl_stat3",(nco_uint64)gsl_stats_ulong_max((const unsigned long*)var[0]->val.ui64p,d_srd,sz)); break;
       case NC_STRING: break;
#endif /* !ENABLE_NETCDF4 */
		    
     default: nco_dfl_case_nc_type_err(); break;    
     } 

   } break;  
  

     


  } // end big switch


  } // end heavy lifting

 (void)cast_nctype_void(var[0]->type,&(var[0]->val)); 
 var[0]=nco_var_free(var[0]);


 return var_ret;


} // end hnd_fnc_stat3




     
var_sct *gsl_cls::hnd_fnc_stat4(bool&,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_stat4");
  int idx;
  int fdx=gpr_obj.type(); // very important
  int args_nbr;
  std::string susg;
  std::string sfnm=gpr_obj.fnm();
  var_sct *var_arr[6];
  double r_val;
   
    
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  
  args_nbr=args_vtr.size();
  
  susg=susg="usage: double_val="+sfnm+"(var_data1, stride_data1, n1, var_data2, stride_data2, n2)";

  if(args_nbr <6){
    err_prn(sfnm,"Function requires six arguments.\n"+susg  ); 
  }
   

  for(idx=0;idx<6;idx++)
    var_arr[idx]=walker.out(args_vtr[idx]);


  
  // Deal with initial scan
  if(prs_arg->ntl_scn){
   
    for(idx=0;idx<6;idx++)
      var_arr[idx]=nco_var_free(var_arr[idx]);
    
    return ncap_sclr_var_mk(static_cast< std::string>("~hnd_fnc_stat4"),(nc_type)NC_DOUBLE,false);   
  }


  
 // check weight type and data type 
 if(var_arr[0]->type != var_arr[3]->type  ){   
   ostringstream os;  
   os<<"The data1 type and the data2 type must be the same . In your arguments the data1 is type "<<nco_typ_sng(var_arr[0]->type)<< " and data2 is type "<<nco_typ_sng(var_arr[2]->type);
   err_prn(sfnm,os.str());  
 }



  if(var_arr[0]->type==(nc_type)NC_INT64 && sizeof(long)!=sizeof(long long int) )
    err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_INT64");

  if(var_arr[0]->type==(nc_type)NC_UINT64 && sizeof(unsigned long)!=sizeof(unsigned long long int) )
    err_prn(sfnm,"This function from the GSL Library is not implemented for the type NC_UINT64");





 // make weight and data conform only for _covariance and _correlation
 if(fdx==PS_COV || fdx==PS_COR)
   ncap_var_att_cnf(var_arr[3],var_arr[0]);


 // do heavy lifting
 { 
   size_t sz1;
   size_t sz2;
   size_t d1_srd;  // stride for data1
   size_t d2_srd;  // stride for data2

   // 1st arg -- data1
   (void)cast_void_nctype(var_arr[0]->type,&(var_arr[0]->val));  

   // 2nd arg data1 stride
   var_arr[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[1]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[1]->val));  
   d1_srd=(size_t) var_arr[1]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT,&(var_arr[1]->val));


   // 3rd arg n1
   var_arr[2]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[2]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[2]->val));  
   sz1=(size_t) var_arr[2]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[2]->val));


   // 4rd arg -- data2
   (void)cast_void_nctype(var_arr[3]->type,&(var_arr[3]->val));  

   // 5th arg data2 stride
   var_arr[4]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[4]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[4]->val));  
   d2_srd=(size_t) var_arr[4]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[4]->val));


   // 6th arg n2
   var_arr[5]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[5]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[5]->val));  
   sz2=(size_t) var_arr[5]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[5]->val));




   switch(fdx){

    
   case PS_COV:
     switch(var_arr[0]->type){
       case NC_FLOAT:  r_val=gsl_stats_float_covariance( var_arr[0]->val.fp,d1_srd,var_arr[3]->val.fp,d2_srd,sz1 );break;
       case NC_DOUBLE: r_val=gsl_stats_covariance(var_arr[0]->val.dp,d1_srd,var_arr[3]->val.dp,d2_srd,sz1 );break;

       case NC_INT:    // NC_INT rpresented as int in nco
                       #if NCO_INT==NCO_TYP_INT
                         r_val=gsl_stats_int_covariance(var_arr[0]->val.ip,d1_srd,var_arr[3]->val.ip,d2_srd,sz1 );break;
                       // NC_INT rpresented as long  in nco
		       #else
                         r_val=gsl_stats_long_covariance(var_arr[0]->val.ip,d1_srd,var_arr[3]->val.ip,d2_srd,sz1 );break;
                       #endif
                       break;

       case NC_SHORT:  r_val=gsl_stats_short_covariance( var_arr[0]->val.sp,d1_srd,var_arr[3]->val.sp,d2_srd,sz1 );break;
       case NC_CHAR:   r_val=gsl_stats_char_covariance((const char*)var_arr[0]->val.cp,d1_srd,(const char*)var_arr[3]->val.cp,d2_srd,sz1 );break;
       case NC_BYTE:   r_val=gsl_stats_char_covariance((const char*)var_arr[0]->val.bp,d1_srd,(const char*)var_arr[3]->val.bp,d2_srd,sz1 );break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  r_val=gsl_stats_uchar_covariance((const unsigned char*)var_arr[0]->val.ubp,d1_srd,(const unsigned char*)var_arr[3]->val.ubp,d2_srd,sz1 );break;
       case NC_USHORT: r_val=gsl_stats_ushort_covariance(var_arr[0]->val.usp,d1_srd,var_arr[3]->val.usp,d2_srd,sz1 );break;
       case NC_UINT:   r_val=gsl_stats_uint_covariance(var_arr[0]->val.uip,d1_srd,var_arr[3]->val.uip,d2_srd,sz1 );break;
       case NC_INT64:  r_val=gsl_stats_long_covariance((const long*)var_arr[0]->val.i64p,d1_srd,(const long*)var_arr[3]->val.i64p,d2_srd,sz1 );break;
       case NC_UINT64: r_val=gsl_stats_ulong_covariance((const unsigned long*)var_arr[0]->val.ui64p,d1_srd,(const unsigned long*)var_arr[3]->val.ui64p,d2_srd,sz1);break;
       case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;    
     }  break;  
  
# if NCO_GSL_VERSION >= 110
   case PS_COR:
     switch(var_arr[0]->type){
       case NC_FLOAT:  r_val=gsl_stats_float_correlation( var_arr[0]->val.fp,d1_srd,var_arr[3]->val.fp,d2_srd,sz1 );break;
       case NC_DOUBLE: r_val=gsl_stats_correlation(var_arr[0]->val.dp,d1_srd,var_arr[3]->val.dp,d2_srd,sz1 );break;

       case NC_INT:    // NC_INT rpresented as int in nco
                       #if NCO_INT==NCO_TYP_INT
                         r_val=gsl_stats_int_correlation(var_arr[0]->val.ip,d1_srd,var_arr[3]->val.ip,d2_srd,sz1 );break;
                       // NC_INT rpresented as long  in nco
		       #else
                         r_val=gsl_stats_long_correlation(var_arr[0]->val.ip,d1_srd,var_arr[3]->val.ip,d2_srd,sz1 );break;
                       #endif
                       break;

       case NC_SHORT:  r_val=gsl_stats_short_correlation( var_arr[0]->val.sp,d1_srd,var_arr[3]->val.sp,d2_srd,sz1 );break;
       case NC_CHAR:   r_val=gsl_stats_char_correlation((const char*)var_arr[0]->val.cp,d1_srd,(const char*)var_arr[3]->val.cp,d2_srd,sz1 );break;
       case NC_BYTE:   r_val=gsl_stats_char_correlation((const char*)var_arr[0]->val.bp,d1_srd,(const char*)var_arr[3]->val.bp,d2_srd,sz1 );break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  r_val=gsl_stats_uchar_correlation((const unsigned char*)var_arr[0]->val.ubp,d1_srd,(const unsigned char*)var_arr[3]->val.ubp,d2_srd,sz1 );break;
       case NC_USHORT: r_val=gsl_stats_ushort_correlation(var_arr[0]->val.usp,d1_srd,var_arr[3]->val.usp,d2_srd,sz1 );break;
       case NC_UINT:   r_val=gsl_stats_uint_correlation(var_arr[0]->val.uip,d1_srd,var_arr[3]->val.uip,d2_srd,sz1 );break;
       case NC_INT64:  r_val=gsl_stats_long_correlation((const long*)var_arr[0]->val.i64p,d1_srd,(const long*)var_arr[3]->val.i64p,d2_srd,sz1 );break;
       case NC_UINT64: r_val=gsl_stats_ulong_correlation((const unsigned long*)var_arr[0]->val.ui64p,d1_srd,(const unsigned long*)var_arr[3]->val.ui64p,d2_srd,sz1);break;
       case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;    
     }  break;  
# endif // NCO_GSL_VERSION < 110
    
   case PS_PVAR:
     switch(var_arr[0]->type){
       case NC_FLOAT:  r_val=gsl_stats_float_pvariance( var_arr[0]->val.fp,d1_srd,sz1,var_arr[3]->val.fp,d2_srd,sz2 );break;
       case NC_DOUBLE: r_val=gsl_stats_pvariance(var_arr[0]->val.dp,d1_srd,sz1,var_arr[3]->val.dp,d2_srd,sz2 );break;

       case NC_INT:    // NC_INT rpresented as int in nco
                       #if NCO_INT==NCO_TYP_INT
                         r_val=gsl_stats_int_pvariance(var_arr[0]->val.ip,d1_srd,sz1,var_arr[3]->val.ip,d2_srd,sz2 );break;
                       // NC_INT rpresented as long  in nco
		       #else
                         r_val=gsl_stats_long_pvariance(var_arr[0]->val.ip,d1_srd,sz1,var_arr[3]->val.ip,d2_srd,sz2 );break;
                       #endif
                       break;

       case NC_SHORT:  r_val=gsl_stats_short_pvariance( var_arr[0]->val.sp,d1_srd,sz1,var_arr[3]->val.sp,d2_srd,sz2 );break;
       case NC_CHAR:   r_val=gsl_stats_char_pvariance((const char*)var_arr[0]->val.cp,d1_srd,sz1,(const char*)var_arr[3]->val.cp,d2_srd,sz2 );break;
       case NC_BYTE:   r_val=gsl_stats_char_pvariance((const char*)var_arr[0]->val.bp,d1_srd,sz1,(const char*)var_arr[3]->val.bp,d2_srd,sz2 );break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  r_val=gsl_stats_uchar_pvariance((const unsigned char*)var_arr[0]->val.ubp,d1_srd,sz1,(const unsigned char*)var_arr[3]->val.ubp,d2_srd,sz2 );break;
       case NC_USHORT: r_val=gsl_stats_ushort_pvariance(var_arr[0]->val.usp,d1_srd,sz1,var_arr[3]->val.usp,d2_srd,sz2 );break;
       case NC_UINT:   r_val=gsl_stats_uint_pvariance(var_arr[0]->val.uip,d1_srd,sz1,var_arr[3]->val.uip,d2_srd,sz2 );break;
       case NC_INT64:  r_val=gsl_stats_long_pvariance((const long*)var_arr[0]->val.i64p,d1_srd,sz1,(const long*)var_arr[3]->val.i64p,d2_srd,sz2 );break;
       case NC_UINT64: r_val=gsl_stats_ulong_pvariance((const unsigned long*)var_arr[0]->val.ui64p,d1_srd,sz1,(const unsigned long*)var_arr[3]->val.ui64p,d2_srd,sz2);break;
       case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;    
     }  break;  
  

  
   case PS_TTST:
     switch(var_arr[0]->type){
       case NC_FLOAT:  r_val=gsl_stats_float_ttest( var_arr[0]->val.fp,d1_srd,sz1,var_arr[3]->val.fp,d2_srd,sz2 );break;
       case NC_DOUBLE: r_val=gsl_stats_ttest(var_arr[0]->val.dp,d1_srd,sz1,var_arr[3]->val.dp,d2_srd,sz2 );break;

       case NC_INT:    // NC_INT rpresented as int in nco
                       #if NCO_INT==NCO_TYP_INT
                         r_val=gsl_stats_int_ttest(var_arr[0]->val.ip,d1_srd,sz1,var_arr[3]->val.ip,d2_srd,sz2 );break;
                       // NC_INT rpresented as long  in nco
		       #else
                         r_val=gsl_stats_long_ttest(var_arr[0]->val.ip,d1_srd,sz1,var_arr[3]->val.ip,d2_srd,sz2 );break;
                       #endif
                       break;

       case NC_SHORT:  r_val=gsl_stats_short_ttest( var_arr[0]->val.sp,d1_srd,sz1,var_arr[3]->val.sp,d2_srd,sz2 );break;
       case NC_CHAR:   r_val=gsl_stats_char_ttest((const char*)var_arr[0]->val.cp,d1_srd,sz1,(const char*)var_arr[3]->val.cp,d2_srd,sz2 );break;
       case NC_BYTE:   r_val=gsl_stats_char_ttest((const char*)var_arr[0]->val.bp,d1_srd,sz1,(const char*)var_arr[3]->val.bp,d2_srd,sz2 );break;
#ifdef ENABLE_NETCDF4
       case NC_UBYTE:  r_val=gsl_stats_uchar_ttest((const unsigned char*)var_arr[0]->val.ubp,d1_srd,sz1,(const unsigned char*)var_arr[3]->val.ubp,d2_srd,sz2 );break;
       case NC_USHORT: r_val=gsl_stats_ushort_ttest(var_arr[0]->val.usp,d1_srd,sz1,var_arr[3]->val.usp,d2_srd,sz2 );break;
       case NC_UINT:   r_val=gsl_stats_uint_ttest(var_arr[0]->val.uip,d1_srd,sz1,var_arr[3]->val.uip,d2_srd,sz2 );break;
       case NC_INT64:  r_val=gsl_stats_long_ttest((const long*)var_arr[0]->val.i64p,d1_srd,sz1,(const long*)var_arr[3]->val.i64p,d2_srd,sz2 );break;
       case NC_UINT64: r_val=gsl_stats_ulong_ttest((const unsigned long*)var_arr[0]->val.ui64p,d1_srd,sz1,(const unsigned long*)var_arr[3]->val.ui64p,d2_srd,sz2);break;
       case NC_STRING: break; /* do nothing */
#endif /* !ENABLE_NETCDF4 */
     default: nco_dfl_case_nc_type_err(); break;    
     }  break;  
  

 
   }// end big switch





     // cast pointer back to void
   (void)cast_nctype_void(var_arr[0]->type,&(var_arr[0]->val)); 
   (void)cast_nctype_void(var_arr[3]->type,&(var_arr[3]->val)); 



 } 
 // end heavy lifting

 // free vars
  for(idx=0 ; idx<6 ; idx++)
    var_arr[idx]=nco_var_free(var_arr[idx]);


  return ncap_sclr_var_mk(SCS("~gsl_stt2_function"),r_val);  


}



//GSL2 Class /******************************************/
  gsl2_cls::gsl2_cls(bool  ){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
      fmc_vtr.push_back( fmc_cls("gsl_rng_min",this,(int)PGSL_RNG_MIN));
      fmc_vtr.push_back( fmc_cls("gsl_rng_max",this,(int)PGSL_RNG_MAX));
      fmc_vtr.push_back( fmc_cls("gsl_rng_name",this,(int)PGSL_RNG_NAME));

    }
  }



  var_sct *gsl2_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl2_cls::fnd");

    int fdx=fmc_obj.fdx();   //index
    int nbr_args;
    unsigned long vlng;
    var_sct *var=NULL_CEWI;
    prs_cls* prs_arg=walker.prs_arg;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string styp;
    RefAST tr;
    std::vector<RefAST> vtr_args; 

    gsl_rng *ncap_rng;


    styp=(expr ? "method":"function");
    nbr_args=0;



    if(expr)
      vtr_args.push_back(expr);

    if((tr=fargs->getFirstChild())) {
      do  
	vtr_args.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
     nbr_args=vtr_args.size();  
      
     if(nbr_args >0) 
       wrn_prn(fnc_nm,styp+" \""+sfnm+"\" requires no arguments"); 



    // If initial scan
    if(prs_arg->ntl_scn){

      switch(fdx) {
       case  PGSL_RNG_MIN:
       case  PGSL_RNG_MAX:
        return ncap_sclr_var_mk(SCS("~gsl2_functions"),(nc_type)NC_DOUBLE,false);        
        break;
      
       case PGSL_RNG_NAME:
        return ncap_var_udf("~gsl2_functions");  
        break;       
    
      }// end switch  
    } 
    
   // initialize 
   ncap_rng=gsl_rng_alloc(gsl_rng_default);  

    
    switch(fdx){ 
           case PGSL_RNG_MIN:
             vlng= gsl_rng_min(ncap_rng);
             var=ncap_sclr_var_mk(SCS("~gsl2_function"),(double)(vlng));
             break;
           case PGSL_RNG_MAX:
             vlng= gsl_rng_max(ncap_rng);
             var=ncap_sclr_var_mk(SCS("~gsl2_function"),(double)(vlng));
             break;
           case PGSL_RNG_NAME:
	    //var=ncap_sclr_var_mk(SCS("~gsl2_functions"),(nco_int)var1->nbr_dim);            
	    break;

    }// end switch        
      



  // free Random Number generator
  gsl_rng_free(ncap_rng);
  return var;		 

}       





//GSL STATISTICS 2  /****************************************/
// gsl statistic functions for floating points only
  gsl_stt2_cls::gsl_stt2_cls(bool  ){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
     fmc_vtr.push_back( fmc_cls("gsl_stats_wmean",this,(int)PWMEAN));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wvariance",this,(int)PWVAR));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wsd",this,(int)PWSD));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wvariance_with_fixed_mean",this,(int)PWVAR_MEAN));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wsd_with_fixed_mean",this,(int)PWSD_MEAN));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wabsdev",this,(int)PWABSDEV));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wskew",this,(int)PWSKEW));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wkurtosis",this,(int)PWKURTOSIS));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wvariance_m",this,(int)PWVAR_M));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wsd_m",this,(int)PWSD_M));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wabsdev_m",this,(int)PWABSDEV_M));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wskew_m_sd",this,(int)PWSKEW_M_SD));
     fmc_vtr.push_back( fmc_cls("gsl_stats_wkurtosis_m_sd",this,(int)PWKURTOSIS_M_SD));

    }
  }



  var_sct *gsl_stt2_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_stt2_cls::fnd");
    int idx;
    int fdx=fmc_obj.fdx();   //index
    int nbr_args=0;
    int in_nbr_args=0;
    double r_val;
    prs_cls* prs_arg=walker.prs_arg;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    RefAST tr;
    std::vector<RefAST> vtr_args; 
    var_sct **var_arr;


    nbr_args=0;
    r_val=0.0; 
     

    if(expr)
      vtr_args.push_back(expr);

    if((tr=fargs->getFirstChild())) {
      do  
	vtr_args.push_back(tr);
      while((tr=tr->getNextSibling()));    
    } 
      
     nbr_args=vtr_args.size();  
  
     switch(fdx){
     
      case PWMEAN:
      case PWVAR:
      case PWSD:
      case PWABSDEV:
      case PWSKEW:
      case PWKURTOSIS:
        susg="usage: double_val="+sfnm+"(var_weight, weight_stride, var_data, data_stride, n)";
	in_nbr_args=5;
        break;

      case PWVAR_MEAN:
      case PWSD_MEAN:
      case PWVAR_M:
      case PWSD_M:
      case PWABSDEV_M:
        susg="usage: double_val="+sfnm+"(var_weight, weight_stride, var_data, data_stride, n, double_mean)";
        in_nbr_args=6;
        break; 
      case PWSKEW_M_SD:
      case PWKURTOSIS_M_SD:
        susg="usage: double_val="+sfnm+"(var_weight, weight_stride, var_data, data_stride, n, double_mean, double_sd)";
        in_nbr_args=7; 
        break;
     }

     if(nbr_args < in_nbr_args) 
       err_prn(sfnm,"Function requires "+nbr2sng(in_nbr_args)+" arguments . You have only supplied "+nbr2sng(nbr_args)+ ".\n"+susg);
     else
       nbr_args=in_nbr_args;   
        


  // init once we now num of args
  var_arr=(var_sct**)nco_malloc(sizeof(var_sct*)*nbr_args);
  
  
 // evaluate args for side effects
  for(idx=0;idx<nbr_args;idx++)
    var_arr[idx]=walker.out(vtr_args[idx]);
   

  
  // deal with intial scan
  if(prs_arg->ntl_scn){
    for(idx=0 ; idx<nbr_args ; idx++)
      var_arr[idx]=nco_var_free(var_arr[idx]);
   
    var_arr=(var_sct**)nco_free(var_arr); 
    // return an empty double 
    return ncap_sclr_var_mk(static_cast< std::string>("~gsl_stt2_function"),(nc_type)NC_DOUBLE,false);  

  }

 //input args:  (weight_var weight_stride data_var data_stride n wmean? wsd ? )
  
 // check weight type and data type 
 
  if(var_arr[0]->type != var_arr[2]->type || (var_arr[2]->type != NC_FLOAT && var_arr[2]->type != NC_DOUBLE  )){   
   ostringstream os;  
   os<<"The data type and the weight type most both be NC_FLOAT or NC_DOUBLE. In your arguments the data is type "<<nco_typ_sng(var_arr[2]->type)<< " and the weight is type "<<nco_typ_sng(var_arr[0]->type);

   err_prn(sfnm,os.str());  
      
 }


 // make weight and data conform
 ncap_var_att_cnf(var_arr[2],var_arr[0]);
    
 {
   bool tflg;

   size_t w_srd;  // weight stride
   size_t d_srd;  // data stride 
   size_t sz;     // array size   

   double dmean=0.0;
   double dsd=0.0;

   
   
   // de-reference -save typing  type is double or float
   tflg=( var_arr[0]->type==NC_DOUBLE ? true:false);

   // 1st arg --the weight
   (void)cast_void_nctype(var_arr[0]->type,&(var_arr[0]->val));  

   // 2nd arg weight stride
   var_arr[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[1]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[1]->val));  
   w_srd=(size_t) var_arr[1]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[1]->val));

   // 3rd arg the data
   (void)cast_void_nctype(var_arr[2]->type,&(var_arr[2]->val));  
  
   // 4th arg data stride
   var_arr[3]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[3]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[3]->val));  
   d_srd=(size_t) var_arr[3]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[3]->val));


   // 5th arg n
   var_arr[4]=nco_var_cnf_typ((nc_type)NC_UINT64,var_arr[4]);
   (void)cast_void_nctype((nc_type)NC_UINT64,&(var_arr[4]->val));  
   sz=(size_t) var_arr[4]->val.ui64p[0];       
   (void)cast_nctype_void((nc_type)NC_UINT64,&(var_arr[4]->val));



   // 5th arg the mean if needed
   if(nbr_args >5){   
     var_arr[5]=nco_var_cnf_typ(NC_DOUBLE,var_arr[5]);
     (void)cast_void_nctype(NC_DOUBLE,&(var_arr[5]->val));  
     dmean=var_arr[5]->val.dp[0];       
     (void)cast_nctype_void(NC_DOUBLE,&(var_arr[5]->val));
   }


   // 6th arg the sd if needed
   if(nbr_args >6){   
     var_arr[6]=nco_var_cnf_typ(NC_DOUBLE,var_arr[6]);
     (void)cast_void_nctype(NC_DOUBLE,&(var_arr[6]->val));  
     dsd=var_arr[6]->val.dp[0];       
     (void)cast_nctype_void(NC_DOUBLE,&(var_arr[6]->val));
   }

   
   // Check hyperslab limits
   if( 1+ (sz-1)*d_srd > (size_t)var_arr[0]->sz){
     err_prn(sfnm,"Requested hyperslab with stride="+nbr2sng(d_srd)+" and n="+ nbr2sng(sz)+" doesn't fit into variable \""+string(var_arr[0]->nm)+"\" with size="+nbr2sng(var_arr[0]->sz)); 
    }

   // the big switch
     switch(fdx){
     
      /********************** user args=5 ********************************************************/    
     case PWMEAN:{ 
       if(tflg) 
	 r_val=gsl_stats_wmean(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz);
       else
         r_val=gsl_stats_float_wmean(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz);              
                
       } break;  

     case PWVAR:{
       if(tflg) 
	 r_val=gsl_stats_wvariance(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz);
       else
         r_val=gsl_stats_float_wvariance(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz);              
                
       } break;  

     case PWSD: {
       if(tflg) 
	 r_val=gsl_stats_wsd(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz);
       else
         r_val=gsl_stats_float_wsd(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz);              
                
       } break;  


     case PWABSDEV: {
       if(tflg) 
	 r_val=gsl_stats_wabsdev(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz);
       else
         r_val=gsl_stats_float_wabsdev(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz);              
                
       } break;  

     case PWSKEW: {
       if(tflg) 
	 r_val=gsl_stats_wskew(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz);
       else
         r_val=gsl_stats_float_wskew(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz);              
                
       } break;  

     case PWKURTOSIS: {
       if(tflg) 
	 r_val=gsl_stats_wkurtosis(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz);
       else
         r_val=gsl_stats_float_wkurtosis(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz);              
                
       } break;  

      /********************** user args=6 ********************************************************/    
     case PWVAR_MEAN: {
       if(tflg) 
	 r_val=gsl_stats_wvariance_with_fixed_mean(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz,dmean);
       else
         r_val=gsl_stats_float_wvariance_with_fixed_mean(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz,dmean);              
                
       } break;  

     case PWSD_MEAN: {
       if(tflg) 
	 r_val=gsl_stats_wsd_with_fixed_mean(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz,dmean);
       else
         r_val=gsl_stats_float_wsd_with_fixed_mean(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz,dmean);              
                
       } break;  

     case PWVAR_M: {
       if(tflg) 
	 r_val=gsl_stats_wvariance_m(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz,dmean);
       else
         r_val=gsl_stats_float_wvariance_m(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz,dmean);              
                
       } break;  


     case PWSD_M: {

       if(tflg) 
	 r_val=gsl_stats_wsd_m(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz,dmean);
       else
         r_val=gsl_stats_float_wsd_m(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz,dmean);              
                
       } break;  



     case PWABSDEV_M:{
       if(tflg) 
	 r_val=gsl_stats_wabsdev_m(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz,dmean);
       else
         r_val=gsl_stats_float_wabsdev_m(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz,dmean);              
                
       } break;  


     /********************** user args=7 ********************************************************/    
     case PWSKEW_M_SD:{
       if(tflg) 
	 r_val=gsl_stats_wskew_m_sd(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz,dmean,dsd);
       else
         r_val=gsl_stats_float_wskew_m_sd(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz,dmean,dsd);              
                
       } break;  


     case PWKURTOSIS_M_SD:{
       if(tflg) 
	 r_val=gsl_stats_wkurtosis_m_sd(var_arr[0]->val.dp,w_srd, var_arr[2]->val.dp,d_srd,sz,dmean,dsd);
       else
         r_val=gsl_stats_float_wkurtosis_m_sd(var_arr[0]->val.fp,w_srd, var_arr[2]->val.fp,d_srd,sz,dmean,dsd);              
                
       } break;  

     

     } // end switch



     // cast pointer back to void
   (void)cast_nctype_void(var_arr[0]->type,&(var_arr[0]->val)); 
   (void)cast_nctype_void(var_arr[2]->type,&(var_arr[2]->val)); 

 } // end inner block

    


    for(idx=0 ; idx<nbr_args ; idx++)
      var_arr[idx]=nco_var_free(var_arr[idx]);
   
    var_arr=(var_sct**)nco_free(var_arr); 




    return ncap_sclr_var_mk(SCS("~gsl_stt2_function"),r_val);  

}


//GSL  /****************************************/
// gsl spline interpolation 
  gsl_spl_cls::gsl_spl_cls(bool  ){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
     fmc_vtr.push_back( fmc_cls("gsl_interp_linear",this,(int)PLINEAR));
     fmc_vtr.push_back( fmc_cls("gsl_interp_polynomial",this,(int)PPOLY));
     fmc_vtr.push_back( fmc_cls("gsl_interp_cspline",this,(int)PCSPLINE));
     fmc_vtr.push_back( fmc_cls("gsl_interp_cspline_periodic",this,(int)PCSPLINE_PER));
     fmc_vtr.push_back( fmc_cls("gsl_interp_akima",this,(int)PAKIMA));
     fmc_vtr.push_back( fmc_cls("gsl_interp_akima_periodic",this,(int)PAKIMA_PER));
     fmc_vtr.push_back( fmc_cls("gsl_spline_eval",this,(int)PEVAL));
    }
  }


  var_sct *gsl_spl_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_spl_cls::fnd");
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
      case PLINEAR:
      case PPOLY:
      case PCSPLINE:
      case PCSPLINE_PER:
      case PAKIMA:
      case PAKIMA_PER:
        return spl_fnd(is_mtd,vtr_args,fmc_obj,walker);  
        break;
      case PEVAL:
        return eval_fnd(is_mtd,vtr_args,fmc_obj,walker);  
        break;
	// 20161205: Always return value to non-void functions: good practice and required by rpmlint
    default:
      return NULL;
      break;
    }// end switch  
    
  }

// nb this method is only call with fdx==PEVAL
var_sct *gsl_spl_cls::eval_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_spl_cls::eval_fnd");
    // int fdx=fmc_obj.fdx();   //index
    int nbr_args;
    int in_nbr_args;
    prs_cls* prs_arg=walker.prs_arg;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    std::string serr;    
    std::string var_nm;

    var_sct *var_xvl;
    vtl_typ lcl_typ;
    NcapVar *Nvar;

 
    nbr_args=args_vtr.size();  
    in_nbr_args=2; 
    susg="usage: var_y_out="+sfnm+"(ram_spline_handle,var_x_vals)";    
    
    if(nbr_args<in_nbr_args){   
      serr="function requires "+ nbr2sng(in_nbr_args)+" arguments. You have only supplied "+nbr2sng(nbr_args)+ " arguments\n"; 
      err_prn(sfnm,serr+susg);
    }



    lcl_typ=expr_typ(args_vtr[0]);          


    // check that first arg is a RAM VARIABLE IDENTIFIER ONLY   
    if(lcl_typ !=VVAR ) {
       serr="The first argument of the function must be a RAM variable identifier only.";
       err_prn(sfnm,serr);
    }

    var_xvl=walker.out(args_vtr[1]);
      
    // convert to doubles
    var_xvl=nco_var_cnf_typ(NC_DOUBLE,var_xvl);   
       	 
    if(prs_arg->ntl_scn)
      return var_xvl;


    var_nm=args_vtr[0]->getText();
    Nvar=prs_arg->var_vtr.find(var_nm);
       
    if(Nvar==NULL)
      err_prn(sfnm,"Unable to find RAM variable "+var_nm); 

    // check if spline flag not set 
    if(!Nvar->flg_spl)
      err_prn(sfnm,"RAM variable "+var_nm+" is not holding a gsl spline interpolation object");   

    
    // do heavy lifting  
    {
       bool us_mss_val=false; // true if missing value is used in output var
       bool has_mss_val;
       long idx;
       long sz;     
       double yval; 
       double mss_val_dbl;
       double *dp;

       gsl_interp_accel *acc;         
       
       acc=gsl_interp_accel_alloc();  

       (void)cast_void_nctype(NC_DOUBLE,&var_xvl->val); 
       dp=var_xvl->val.dp; 
       sz=var_xvl->sz;

      
       if(var_xvl->has_mss_val){
	 has_mss_val=true;
	 (void)cast_void_nctype(NC_DOUBLE,&var_xvl->mss_val);
	 mss_val_dbl=var_xvl->mss_val.dp[0];    
	 (void)cast_nctype_void(NC_DOUBLE,&var_xvl->mss_val);
       }else{
         has_mss_val=false;
	 mss_val_dbl=NC_FILL_DOUBLE;
       }
 
       for(idx=0 ; idx<sz; idx++)
         // nb gsl call return GSL_SUCCESS if no domain error
	 dp[idx]=(  (gsl_spline_eval_e((const gsl_spline*)(Nvar->var->val.vp), dp[idx],acc,&yval))? (us_mss_val=true,mss_val_dbl):yval) ;

 
       (void)cast_nctype_void(NC_DOUBLE,&var_xvl->val);   
       
       // Create missing value for var_xvl if it doesn't have one 
       // and missing value has been used in result;      
       if(!has_mss_val && us_mss_val){
         var_xvl->has_mss_val=true;
         var_xvl->mss_val=nco_mss_val_mk(NC_DOUBLE);
	 (void)cast_void_nctype(NC_DOUBLE,&var_xvl->mss_val);
	 var_xvl->mss_val.dp[0]=mss_val_dbl;    
	(void)cast_nctype_void(NC_DOUBLE,&var_xvl->mss_val);
       }

       gsl_interp_accel_free(acc);

    } // end heavy lifting

  return var_xvl;  


} // end gsl_spl_cls::eval_fnd 


var_sct *gsl_spl_cls::spl_fnd(bool &, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){  
  const std::string fnc_nm("gsl_spl_cls::spl_fnd");
    int fdx=fmc_obj.fdx();   //index
    int nbr_args;
    int in_nbr_args;
    prs_cls* prs_arg=walker.prs_arg;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    std::string serr;    
    std::string var_nm;

    var_sct *var_x;  
    var_sct *var_y;
    var_sct *var_ram;
    std::vector<double> vtr_x;
    std::vector<double> vtr_y;

    
    NcapVar *Nvar;       
    const gsl_interp_type *ts=NULL_CEWI;
    gsl_spline *spline;   
 
    nbr_args=args_vtr.size();  
    in_nbr_args=3; 
    susg="usage: status="+sfnm+"(&ram_spline_handle,var_x_vals,var_y_vals)";    

    if(nbr_args<in_nbr_args){   
      serr="function requires "+ nbr2sng(in_nbr_args)+" arguments. You have only supplied "+nbr2sng(nbr_args)+ " arguments\n"; 
      err_prn(sfnm,serr+susg);
    }

    switch(fdx){
      case PLINEAR:
        ts=gsl_interp_linear;
        break; 
      case PPOLY:
        ts=gsl_interp_polynomial;
        break; 
      case PCSPLINE:
        ts=gsl_interp_cspline;
        break; 
      case PCSPLINE_PER:
        ts=gsl_interp_cspline_periodic;
        break; 
      case PAKIMA:
        ts=gsl_interp_akima;
        break; 
      case PAKIMA_PER:   
        ts=gsl_interp_akima_periodic;
           break; 
    } // end switch 


    if(args_vtr[0]->getType() != CALL_REF ) 
      err_prn(sfnm," first argument must be a call by reference ram variable\n");   

    var_nm=args_vtr[0]->getFirstChild()->getText(); 
       

        
      var_x=walker.out(args_vtr[1]);
      var_y=walker.out(args_vtr[2]);

      // Initial scan          
      if(prs_arg->ntl_scn){
	nco_var_free(var_x);
	nco_var_free(var_y);

        var_ram=ncap_sclr_var_mk(var_nm,NC_CHAR,false);
        // below call frees up var_ram
        prs_arg->ncap_var_write(var_ram,true);     
 
        return ncap_sclr_var_mk("~gsl_spl_cls",NC_INT,false);   
      } 


      var_x=nco_var_cnf_typ(NC_DOUBLE,var_x);    
      var_y=nco_var_cnf_typ(NC_DOUBLE,var_y);    
      // make variables conform 
      (void)ncap_var_att_cnf(var_y,var_x);

      // missing values 
      if(var_x->has_mss_val || var_y->has_mss_val){
        long idx;  
        long sz;
        // a lazy arsed hack - set defaults to double fill value  
	double mss_dbl_y=NC_MIN_DOUBLE; 
	double mss_dbl_x=NC_MIN_DOUBLE;
        
        if(var_x->has_mss_val){   
	 cast_void_nctype(NC_DOUBLE,&var_x->mss_val);
         mss_dbl_x=var_x->mss_val.dp[0];      
         cast_nctype_void(NC_DOUBLE,&var_x->mss_val);      
        }
        
        if(var_y->has_mss_val){  
	 cast_void_nctype(NC_DOUBLE,&var_y->mss_val);
         mss_dbl_y=var_y->mss_val.dp[0];      
         cast_nctype_void(NC_DOUBLE,&var_y->mss_val);      
        } 
         // nb dont check first or last value for missing 
         // so at the very least - vectors contain 2 points 
        vtr_x.push_back(var_x->val.dp[0]);
        vtr_y.push_back(var_y->val.dp[0]);

         sz=var_y->sz-1; 
         for(idx=1; idx<sz;idx++)
	   if(var_x->val.dp[idx]!=mss_dbl_x && var_y->val.dp[idx]!=mss_dbl_y)
             { vtr_x.push_back(var_x->val.dp[idx]);
               vtr_y.push_back(var_y->val.dp[idx]);
             }

        vtr_x.push_back(var_x->val.dp[sz]);
        vtr_y.push_back(var_y->val.dp[sz]);   


      }else{

	vtr_x.insert( vtr_x.begin(), var_x->val.dp,var_x->val.dp+var_x->sz);    
	vtr_y.insert( vtr_y.begin(), var_y->val.dp,var_y->val.dp+var_y->sz);    

      }

      // done with var_x & var_y
      (void)cast_nctype_void(NC_DOUBLE,&var_x->val);   
      (void)cast_nctype_void(NC_DOUBLE,&var_y->val); 
      nco_var_free(var_x);
      nco_var_free(var_y);            
 
   
      Nvar=prs_arg->var_vtr.find(var_nm);  
      if(!Nvar){
          var_ram=ncap_sclr_var_mk(var_nm,NC_CHAR,false);
          Nvar= new NcapVar(var_ram);
          Nvar->flg_mem=true;  
          prs_arg->var_vtr.push_ow(Nvar);
      } 
      // a bit of a hack ?
      Nvar->flg_mem=true;
      Nvar->flg_spl=true;      
      Nvar->flg_stt=2;

      spline=gsl_spline_alloc(ts,vtr_x.size());  
      //gsl_spline_init(spline,var_x->val.dp,var_y->val.dp,var_x->sz);
      gsl_spline_init(spline,&vtr_x[0],&vtr_y[0],vtr_x.size() );

      (void)cast_void_nctype(NC_CHAR,&Nvar->var->val); 
      Nvar->var->val.cp=(char*)spline;  
      (void)cast_nctype_void(NC_CHAR,&Nvar->var->val); 

      // return true
      return ncap_sclr_var_mk("~gsl_spl_cls",(nco_int)1); 

} // end gsl_spl_cls::spl_fnd 

//GSL  /****************************************/
// gsl Least Squares Fitting
  gsl_fit_cls::gsl_fit_cls(bool  ){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
     fmc_vtr.push_back( fmc_cls("gsl_fit_linear",this,(int)PLIN));
     fmc_vtr.push_back( fmc_cls("gsl_fit_wlinear",this,(int)PWLIN));
     fmc_vtr.push_back( fmc_cls("gsl_fit_linear_est",this,(int)PLIN_EST));
     fmc_vtr.push_back( fmc_cls("gsl_fit_mul",this,(int)PMUL));
     fmc_vtr.push_back( fmc_cls("gsl_fit_wmul",this,(int)PWMUL));
     fmc_vtr.push_back( fmc_cls("gsl_fit_mul_est",this,(int)PMUL_EST));
    }
  }


  var_sct *gsl_fit_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){
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
      case PLIN:
      case PWLIN:
      case PMUL:
      case PWMUL:
        return fit_fnd(is_mtd,vtr_args,fmc_obj,walker);  
        break;
      case PLIN_EST:
      case PMUL_EST:
        return fit_est_fnd(is_mtd,vtr_args,fmc_obj,walker);  
        break;
	// 20161205: Always return value to non-void functions: good practice and required by rpmlint
    default:
      return NULL;
      break;
    }

} // end gsl_fit_cls::fnd 


var_sct *gsl_fit_cls::fit_fnd(bool &, std::vector<RefAST> &vtr_args, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_fit_cls::fit_fnd");
    int idx;
    int fdx=fmc_obj.fdx();   //index
    int nbr_args;    // actual nunber of args
    int in_nbr_args=0; // target number of args
    int in_val_nbr_args=0; // number of expressions
    int ret; 
    prs_cls* prs_arg=walker.prs_arg;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    std::string serr;    
    vtl_typ lcl_typ;
    
    var_sct *var_in[13];  
       
    ret=NCO_GSL_SUCCESS;
    
    nbr_args=vtr_args.size();  

    switch(fdx){
      case PLIN:
	in_nbr_args=11;
        in_val_nbr_args=5;
        susg="usage: status="+sfnm+"(data_x,stride_x,data_y,stride_y,n,&co,&c1,&cov00,&cov01,&cov11,&sumsq)";
	break;   
      case PWLIN:
	in_nbr_args=13;
        in_val_nbr_args=7;
        susg="usage: status="+sfnm+"(data_x,stride_x,data_w,stride_w,data_y,stride_y,n,&co,&c1,&cov00,&cov01,&cov11,&chisq)";
	break; 
      case PMUL:
	in_nbr_args=8;
        in_val_nbr_args=5;
        susg="usage: status="+sfnm+"(data_x,stride_x,data_y,stride_y,n,&c1,&cov11,&sumsq)";
	break;   
      case PWMUL:
	in_nbr_args=10;
        in_val_nbr_args=7;
        susg="usage: status="+sfnm+"(data_x,stride_x,data_w,stride_w,stride_y,data_y,n,&c1,&cov11,&sumsq)";
	break;   
	break;
    }   


    if(nbr_args<in_nbr_args){   
      serr="function requires "+ nbr2sng(in_nbr_args)+" arguments. You have only supplied "+nbr2sng(nbr_args)+ " arguments\n"; 
      err_prn(sfnm,serr+susg);
    }
    
    for(idx=0; idx<in_nbr_args  ; idx++){

       lcl_typ=expr_typ(vtr_args[idx]);  

       // deal with regular arguments 
       if(idx<in_val_nbr_args){ 

	 if(lcl_typ == VCALL_REF || lcl_typ == VDIM ){
           serr="function requires that " + nbr2sng(idx)+ " argument be a variable name or an expression";
           err_prn(sfnm,serr+susg);
         }
                 
         var_in[idx] = walker.out(vtr_args[idx]); 
       

       // deal with call-by-ref variables       
       }else{

         var_sct *var_tmp;
	 std::string var_nm;
         NcapVar  *Nvar;

         var_nm=vtr_args[idx]->getFirstChild()->getText();

	 if(lcl_typ != VCALL_REF) {
           serr="function requires that " + nbr2sng(idx)+ " argument be a call by reference variable";
           err_prn(sfnm,serr+susg);
         }

         // initial scan
         if(prs_arg->ntl_scn){

           if(prs_arg->ncap_var_init_chk(var_nm))
             var_tmp=prs_arg->ncap_var_init(var_nm,false);  
           else
	     var_tmp=ncap_sclr_var_mk(var_nm,NC_DOUBLE,false);
           
	 // final scan
	 }else{
	    // we have a problem here - its possible that in the inital scan
            // that some of the call-by-ref variables have been defined but 
            // not populated. Cannot use ncap_var_init() as this will attempt
            // to read var from input as it is unpopulated 
           Nvar=prs_arg->var_vtr.find(var_nm);

           if(Nvar && Nvar->flg_stt==1){
	     var_tmp=Nvar->cpyVarNoData();
             // malloc space for var
             var_tmp->val.vp=nco_malloc(var_tmp->sz * nco_typ_lng(var_tmp->type));   
           }    
           else if(prs_arg->ncap_var_init_chk(var_nm))
             var_tmp=prs_arg->ncap_var_init(var_nm,true);    
           else   
	     var_tmp=ncap_sclr_var_mk(var_nm,NC_DOUBLE,1.0);

	 } //end final scan

         // convert to type double
         if(!var_tmp->undefined)
           var_tmp=nco_var_cnf_typ(NC_DOUBLE,var_tmp);

         var_in[idx]=var_tmp;
           
       } //end call-by-ref vars
        

    } // end for  


    // inital scan --free up  vars and return 
    if(prs_arg->ntl_scn){
      for(idx=0 ; idx<in_nbr_args ;idx++)
        if(idx<in_val_nbr_args) 
	  var_in[idx]=nco_var_free(var_in[idx]);
        else
          // write newly defined call by ref args
          // nb this call frees up var_in[idx] 
          prs_arg->ncap_var_write(var_in[idx],false);
            
      return ncap_sclr_var_mk("~gsl_fit_cls",NC_INT,false);   
    }

    // big switch 
    switch(fdx){

    case PLIN:{
      // recall aguments in order in var_in
      /* 
      0  x_in
      1  x stride
      2  y_in
      3  y stride
      4  n
      5  c0
      6  c1
      7  c00 
      8  c01
      9  c11
      10  sumsq       
      */
      // convert x,y to type double
      var_in[0]=nco_var_cnf_typ(NC_DOUBLE,var_in[0]);
      var_in[2]=nco_var_cnf_typ(NC_DOUBLE,var_in[2]);
    
      // make x,y  conform 
      (void)ncap_var_att_cnf(var_in[2],var_in[0]);
              
      // convert strides to NC_UINT64
      var_in[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[1]);   
      var_in[3]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[3]);   
      var_in[4]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[4]);   

      //cast pointers from void 
      for(idx=0 ; idx< in_nbr_args ;idx++)
        (void)cast_void_nctype(var_in[idx]->type,&var_in[idx]->val);

      // make the call -- 
      ret=gsl_fit_linear(var_in[0]->val.dp,var_in[1]->val.ui64p[0],
                         var_in[2]->val.dp,var_in[3]->val.ui64p[0],     
                         var_in[4]->val.ui64p[0],
                         var_in[5]->val.dp, var_in[6]->val.dp,
                         var_in[7]->val.dp, var_in[8]->val.dp,
                         var_in[9]->val.dp, var_in[10]->val.dp);
        
      // free up or save values 


      } break;   

      case PWLIN:{
      // recall arguments in order in var_in
      /* 
      0  x_in
      1  x stride
      2  weight
      3  weight stride 
      4  y_in
      5  y stride
      6  n
      7  c0
      8  c1
      9  c00 
      10  c01
      11 c11
      12 chisq       
      */
      // convert x,w,y to type double
      var_in[0]=nco_var_cnf_typ(NC_DOUBLE,var_in[0]);
      var_in[2]=nco_var_cnf_typ(NC_DOUBLE,var_in[2]);
      var_in[4]=nco_var_cnf_typ(NC_DOUBLE,var_in[4]);
    
      // make x,w,y all conform 
      { var_sct **var_arr[3];
        var_arr[0]=&var_in[0];    
        var_arr[1]=&var_in[2];    
        var_arr[2]=&var_in[4];    
	(void)ncap_var_att_arr_cnf(false,var_arr,3);
      }        
      // convert strides to NC_UINT64
      var_in[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[1]);   
      var_in[3]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[3]);   
      var_in[5]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[5]);   
      var_in[6]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[6]);   

 
      //cast pointers from void 
      for(idx=0 ; idx< in_nbr_args ;idx++)
        (void)cast_void_nctype(var_in[idx]->type,&var_in[idx]->val);
       

      // make the call -- 
      ret=gsl_fit_wlinear(var_in[0]->val.dp,var_in[1]->val.ui64p[0],
                          var_in[2]->val.dp,var_in[3]->val.ui64p[0],     
                          var_in[4]->val.dp,var_in[5]->val.ui64p[0],     
                          var_in[6]->val.ui64p[0],
                          var_in[7]->val.dp, var_in[8]->val.dp,
                          var_in[9]->val.dp, var_in[10]->val.dp,
                          var_in[11]->val.dp,var_in[12]->val.dp);
        
    
      } break; 

      case PMUL:{
      // recall arguments in order in var_in
      /* 
      0  x_in
      1  x stride
      2  y_in
      3  y stride
      4  n
      5  c1
      6  cov11
      7  sumsq 
      */
      // convert x,y to type double
      var_in[0]=nco_var_cnf_typ(NC_DOUBLE,var_in[0]);
      var_in[2]=nco_var_cnf_typ(NC_DOUBLE,var_in[2]);
    
      // make x,y  conform 
      (void)ncap_var_att_cnf(var_in[2],var_in[0]);
              
      // convert strides to NC_UINT64
      var_in[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[1]);   
      var_in[3]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[3]);   
      var_in[4]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[4]);   

      //cast pointers from void 
      for(idx=0 ; idx< in_nbr_args ;idx++)
        (void)cast_void_nctype(var_in[idx]->type,&var_in[idx]->val);

      // make the call -- 
      ret=gsl_fit_mul(var_in[0]->val.dp,var_in[1]->val.ui64p[0],
                      var_in[2]->val.dp,var_in[3]->val.ui64p[0],     
                      var_in[4]->val.ui64p[0],
                      var_in[5]->val.dp, var_in[6]->val.dp,
                      var_in[7]->val.dp);        


      } break;

      case PWMUL:{
      // recall arguments in order in var_in
      /* 
      0  x_in
      1  x stride
      2  weight
      3  weight stride 
      4  y_in
      5  y stride
      6  n 
      7  c1
      8  cov11
      9  sumsq       
      */
      // convert x,w,y to type double
      var_in[0]=nco_var_cnf_typ(NC_DOUBLE,var_in[0]);
      var_in[2]=nco_var_cnf_typ(NC_DOUBLE,var_in[2]);
      var_in[4]=nco_var_cnf_typ(NC_DOUBLE,var_in[4]);
    
      // make x,w,y all conform 
      { var_sct **var_arr[3];
        var_arr[0]=&var_in[0];    
        var_arr[1]=&var_in[2];    
        var_arr[2]=&var_in[4];    
	(void)ncap_var_att_arr_cnf(false,var_arr,3);
      }        
      // convert strides to NC_UINT64
      var_in[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[1]);   
      var_in[3]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[3]);   
      var_in[5]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[5]);   
      var_in[6]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[6]);   

      //cast pointers from void 
      for(idx=0 ; idx< in_nbr_args ;idx++)
        (void)cast_void_nctype(var_in[idx]->type,&var_in[idx]->val);

      // make the call -- 
      ret=gsl_fit_wmul(var_in[0]->val.dp,var_in[1]->val.ui64p[0],
                          var_in[2]->val.dp,var_in[3]->val.ui64p[0],     
                          var_in[4]->val.dp,var_in[5]->val.ui64p[0],     
                          var_in[6]->val.ui64p[0],
                          var_in[7]->val.dp, var_in[8]->val.dp,
                          var_in[9]->val.dp);

      } break;



    } // end big switch   


    for(idx=0 ; idx< in_nbr_args ;idx++){
      //cast pointers to void
      (void)cast_nctype_void(var_in[idx]->type,&var_in[idx]->val);
      if(idx<in_val_nbr_args)
        nco_var_free(var_in[idx]);
      else
        // nb this write call also frees up pointer  
        prs_arg->ncap_var_write(var_in[idx],false);
    }
    // return status flag
    return ncap_sclr_var_mk("~gsl_fit_cls",(nco_int)ret);   


} // end gsl_fit_cls::fit_fnd 


var_sct *gsl_fit_cls::fit_est_fnd(bool &, std::vector<RefAST> &vtr_args, fmc_cls &fmc_obj, ncoTree &walker){
  const std::string fnc_nm("gsl_fit_cls::fit_est_fnd");
    bool has_mss_val; 
    int idx;
    int fdx=fmc_obj.fdx();   //index
    int nbr_args=0;    // actual nunber of args
    int in_nbr_args=0; // target number of args
    double mss_val_dbl=0.0;

    prs_cls* prs_arg=walker.prs_arg;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    std::string serr;    
    
    var_sct *var_in[12]={NULL_CEWI};  
    var_sct *var_out=NULL_CEWI; 
       
 
    nbr_args=vtr_args.size();  

    switch(fdx){
      case PLIN_EST:
	in_nbr_args=6;
        susg="usage: data_y="+sfnm+"(data_x,c0,c1,cov00,cov01,cov11)";
	break;   
      case PMUL_EST:
	in_nbr_args=3;
        susg="usage: data_y="+sfnm+"(data_x,c1,cov11)";
	break; 

    }   


    if(nbr_args<in_nbr_args){   
      serr="function requires "+ nbr2sng(in_nbr_args)+" arguments. You have only supplied "+nbr2sng(nbr_args)+ " arguments\n"; 
      err_prn(sfnm,serr+susg);
    }
    
    for(idx=0 ; idx<in_nbr_args ;idx++){
      var_in[idx]=walker.out(vtr_args[idx]);
      if(!var_in[idx]->undefined)
	var_in[idx]=nco_var_cnf_typ(NC_DOUBLE,var_in[idx]);
    }


    if(prs_arg->ntl_scn){
      for(idx=1; idx<in_nbr_args; idx++)
	nco_var_free(var_in[idx]);
      return var_in[0]; 
    }

    var_out=nco_var_dpl(var_in[0]);

    // input args now type double -cast them
    for(idx=0 ; idx<in_nbr_args ; idx++)
      (void)cast_void_nctype(var_in[idx]->type,&var_in[idx]->val);       

    (void)cast_void_nctype(var_out->type,&var_out->val);       

    has_mss_val=false;
    if(var_out->has_mss_val){
      has_mss_val=true;
      (void)cast_void_nctype(NC_DOUBLE,&var_out->mss_val);
      mss_val_dbl=var_out->mss_val.dp[0];    
      (void)cast_nctype_void(NC_DOUBLE,&var_out->mss_val);
    }    


    switch(fdx){

      case PLIN_EST: {
        long jdx;
        long sz;
        double y_err;
               
        sz=var_out->sz;
        
        if(has_mss_val){   
          for(jdx=0 ; jdx<sz;jdx++) 
            if(var_out->val.dp[jdx] != mss_val_dbl) 
              gsl_fit_linear_est(var_in[0]->val.dp[jdx],
                                 var_in[1]->val.dp[0], var_in[2]->val.dp[0],   
                                 var_in[3]->val.dp[0], var_in[4]->val.dp[0],   
                                 var_in[5]->val.dp[0], &var_out->val.dp[jdx], &y_err);
        }else{
          for(jdx=0 ; jdx<sz;jdx++) 
              gsl_fit_linear_est(var_in[0]->val.dp[jdx],
                                 var_in[1]->val.dp[0], var_in[2]->val.dp[0],   
                                 var_in[3]->val.dp[0], var_in[4]->val.dp[0],   
                                 var_in[5]->val.dp[0], &var_out->val.dp[jdx], &y_err);
        }                     
        
        } break; 

      case PMUL_EST: {
        long jdx;
        long sz;
        double y_err;
               
        sz=var_out->sz;
 

        if(has_mss_val){   
          for(jdx=0 ; jdx<sz;jdx++) 
            if(var_out->val.dp[jdx] != mss_val_dbl) 
              gsl_fit_mul_est(var_in[0]->val.dp[jdx], var_in[1]->val.dp[0], 
                              var_in[2]->val.dp[0], &var_out->val.dp[jdx], &y_err);
        }else{
          for(jdx=0 ; jdx<sz;jdx++) 
              gsl_fit_mul_est(var_in[0]->val.dp[jdx], var_in[1]->val.dp[0], 
                              var_in[2]->val.dp[0], &var_out->val.dp[jdx], &y_err);
        }                     
        

         } break;        
       
    } // end big switch


    // free up args
    for(idx=0 ; idx<in_nbr_args ; idx++){
      (void)cast_nctype_void(var_in[idx]->type,&var_in[idx]->val);       
      nco_var_free(var_in[idx]);
    }   


    (void)cast_nctype_void(var_out->type,&var_out->val);       
    
    return var_out;  



} // end gsl_fit_cls::fit_est_fnd 

/*
int gsl_fit_cls::rm_miss_arr(
                double *x_in, long long x_stride,
                double *y_in, long long y_stride, 
                double *w_in, long long w_stride,
                long long n)
{


  return 0;
}
*/

  gsl_mfit_cls::gsl_mfit_cls(bool){
    //Populate only on first constructor call
    if(fmc_vtr.empty()){
     fmc_vtr.push_back( fmc_cls("gsl_multifit_linear",this,(int)PMLIN));
     fmc_vtr.push_back( fmc_cls("gsl_multifit_wlinear",this,(int)PMWLIN));
     fmc_vtr.push_back( fmc_cls("gsl_multifit_linear_est",this,(int)PMLIN_EST));

    }
  }

  var_sct * gsl_mfit_cls::fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker){

  const std::string fnc_nm("gsl_mfit_cls::fnd");
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
      case PMLIN:
      case PMWLIN:
        return mfit_fnd(is_mtd,vtr_args,fmc_obj,walker);  
        break;
      case PMLIN_EST:
        return mfit_est_fnd(is_mtd,vtr_args,fmc_obj,walker);  
        break;
	// 20161205: Always return value to non-void functions: good practice and required by rpmlint
    default:
      return (var_sct*)NULL;
      break;
    }

    return (var_sct*)NULL;
    
}
  
  var_sct *gsl_mfit_cls::mfit_fnd(bool &is_mtd, std::vector<RefAST> &vtr_args, fmc_cls &fmc_obj, ncoTree &walker){

  const std::string fnc_nm("gsl_mfit_cls::mfit_fnd");
    int idx;
    int jdx;
    int fdx=fmc_obj.fdx();   //index
    int nbr_args;    // actual nunber of args
    int in_nbr_args=0; // target number of args
    int in_val_nbr_args=0; // number of expressions
    int ret; 
    prs_cls* prs_arg=walker.prs_arg;
    std::string sfnm =fmc_obj.fnm(); //method name
    std::string susg;
    std::string serr;    
    vtl_typ lcl_typ;
    
    var_sct *var_in[13];  
       
    ret=NCO_GSL_SUCCESS;
    
    nbr_args=vtr_args.size();  

    switch(fdx){
      case PMLIN:
	in_nbr_args=5;
        in_val_nbr_args=2;
        susg="usage: status="+sfnm+"(matrix_X, vector_y, &vector_c, &matrix_cov, &chisq)"; 
	break;   
      case PMWLIN:
	in_nbr_args=6;
        in_val_nbr_args=3;
        susg="usage: status="+sfnm+"(matrix_X, vector_w, vector_y, &vector_c, &matrix_cov, &chisq)"; 
	break; 
    }   


    if(nbr_args<in_nbr_args){   
      serr="function requires "+ nbr2sng(in_nbr_args)+" arguments. You have only supplied "+nbr2sng(nbr_args)+ " arguments\n"; 
      err_prn(sfnm,serr+susg);
    }


    // deal with regular arguments
    for(idx=0; idx<in_val_nbr_args  ; idx++){

       lcl_typ=expr_typ(vtr_args[idx]);  

       if(lcl_typ == VCALL_REF || lcl_typ == VDIM ){
           serr="function requires that arg " + nbr2sng(idx+1)+ " be a variable name or an expression\n";
           err_prn(sfnm,serr+susg);
       }
                 
       var_in[idx] = walker.out(vtr_args[idx]);
       // convert up
       var_in[idx]=nco_var_cnf_typ(NC_DOUBLE,var_in[idx]);
     
    }


    // deal with call-by-ref args
    for(idx=in_val_nbr_args; idx<in_nbr_args; idx++) 
    {

         var_sct *var_tmp;
	 std::string var_nm;
         NcapVar  *Nvar;

	 
	 lcl_typ=expr_typ(vtr_args[idx]);  
         var_nm=vtr_args[idx]->getFirstChild()->getText();
         Nvar=prs_arg->var_vtr.find(var_nm);
	 
	 if(lcl_typ != VCALL_REF) {
           serr="function requires that arg" + nbr2sng(idx+1)+ " be a call by reference variable\n";
           err_prn(sfnm,serr+susg);
         }

	 
	 // see if var already defined
         if(prs_arg->ncap_var_init_chk(var_nm))
	 {
	   if(Nvar && Nvar->flg_stt==1)
              var_tmp=Nvar->cpyVarNoData();
	   else
	      var_tmp=prs_arg->ncap_var_init(var_nm, !(prs_arg->ntl_scn));
	 }
	 else
	 {
           switch(idx-in_val_nbr_args)
	   {  

	     // vector_y
	     case 0:
	       {
		 // make a 1 D var using first dim from first arg
		 std::vector<std::string> str_vtr;
		 str_vtr.push_back( var_in[0]->dim[0]->nm );   
	         var_tmp=ncap_cst_mk(str_vtr,prs_arg);
                     
               } 
	       var_tmp->nm=(char*)nco_free(var_tmp->nm);
	       var_tmp->nm=strdup(var_nm.c_str());
	       break;

	     // cov   
	     case 1:
	       {
		 // make a 2 D var using first dim from first arg twice !!
		 std::vector<std::string> str_vtr;
		 str_vtr.push_back( var_in[0]->dim[0]->nm );
		 str_vtr.push_back( var_in[0]->dim[0]->nm );   
	         var_tmp=ncap_cst_mk(str_vtr,prs_arg);
                     
               }
       	       var_tmp->nm=(char*)nco_free(var_tmp->nm);
	       var_tmp->nm=strdup(var_nm.c_str());

               break;

	     // chisq  
	     case 2:   
               var_tmp=ncap_sclr_var_mk(var_nm,NC_DOUBLE, !(prs_arg->ntl_scn) );
	       break;
	   }
	   
	 } //  end switch  

        // remember ncap_cst_mk doesnt create vp 
	if(!prs_arg->ntl_scn && var_tmp->val.vp == NULL)
	   var_tmp->val.vp=(void *)nco_malloc_flg(var_tmp->sz*nco_typ_lng(var_tmp->type));
 
	 
	 var_tmp=nco_var_cnf_typ(NC_DOUBLE,var_tmp);
         var_in[idx]=var_tmp;      
    }     

 
    // inital scan --free up  vars and return 
    if(prs_arg->ntl_scn){
      for(idx=0 ; idx<in_nbr_args ;idx++)
        if(idx<in_val_nbr_args) 
	  var_in[idx]=nco_var_free(var_in[idx]);
        else
          // write newly defined call by ref args
          // nb this call frees up var_in[idx] 
          prs_arg->ncap_var_write(var_in[idx],false);
            
      return ncap_sclr_var_mk("~gsl_mfit_cls",NC_INT,false);   
    }


     // all vars should now be of type NC_DOUBLE, so cast them  
    for(idx=0; idx<in_nbr_args; idx++)  
      (void)cast_void_nctype(var_in[idx]->type,&var_in[idx]->val);
    
    // start with PMLIN
    if( fdx == PMLIN)
    {
      // summary of args
      // I arg 0 matrix X
      // I arg 1 vector y
      // O arg 2 vector c  
      // O arg 3 matrix cov
      // O arg 4 scalar double chisq

      int rows;
      int cols;

      // first arg must be a 2D matrix
      if(var_in[0]->nbr_dim !=2)
	err_prn(sfnm, "The first argument must be a 2D variable - representing a matrix\n"); 

      
      cols=var_in[0]->dim[0]->sz;
      rows=var_in[0]->dim[1]->sz;

      // check that second arg has right number elements
      if( var_in[1]->sz != rows)
	err_prn(sfnm, "Size mismatch the size of the second arg must be equal to the number columns in the first arg\n"); 



      gsl_matrix *mat_x = gsl_matrix_alloc(rows, cols);
      gsl_vector *vec_y = gsl_vector_alloc(rows);
      gsl_vector *vec_c = gsl_vector_alloc(cols);
      gsl_matrix *mat_cov = gsl_matrix_alloc(cols, cols); 
      gsl_multifit_linear_workspace *my_workspace=gsl_multifit_linear_alloc (rows, cols);

      // stuff first arg into matrix
      for(idx=0; idx<cols ; idx++)
        for(jdx=0;jdx<rows;jdx++)   
          gsl_matrix_set(mat_x, idx,jdx, var_in[0]->val.dp[ idx*rows+jdx]);

      // stuff second arg into vector
      for(jdx=0; jdx<rows ; jdx++)
	gsl_vector_set(vec_y, jdx, var_in[1]->val.dp[jdx]);  

      // finally make the big call  
      ret=gsl_multifit_linear(mat_x, vec_y, vec_c, mat_cov,var_in[4]->val.dp, my_workspace );       

      // extract values from vector
      for(jdx=0; jdx<cols ; jdx++){
        var_in[2]->val.dp[jdx]= gsl_vector_get(vec_c, jdx);
      }
      
      // extract values from matrix
      for(idx=0; idx<cols ; idx++)
        for(jdx=0;jdx<cols;jdx++)
	      var_in[3]->val.dp[ idx*cols+jdx] = gsl_matrix_get(mat_cov, idx,jdx); 

      

      gsl_matrix_free(mat_x);
      gsl_vector_free(vec_y);
      gsl_vector_free(vec_c);
      gsl_matrix_free(mat_cov);

      gsl_multifit_linear_free (my_workspace);
      

    }  

    for(idx=0 ; idx< in_nbr_args ;idx++){
      //cast pointers to void
      (void)cast_nctype_void(var_in[idx]->type,&var_in[idx]->val);
      
      if(idx<in_val_nbr_args)
        nco_var_free(var_in[idx]);
      else
        // nb this write call also frees up pointer  
        prs_arg->ncap_var_write(var_in[idx],false);
    }

    return ncap_sclr_var_mk("~gsl_mfit_cls",(nco_int)ret);   
    
}
  
  var_sct *gsl_mfit_cls::mfit_est_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker){



  return (var_sct*)NULL;
}







#else // !ENABLE_GSL

/* Dummy stub function so fmc_gsl_cls.o is not empty when GSL unavailable
   Function should never be called */
int gsl_dmm_stb(void);
int gsl_dmm_stb(void){return 1;}
 
#endif // !ENABLE_GSL

#ifdef ENABLE_GSL
// nco_gsl 
nco_gsl_cls::nco_gsl_cls(bool)
{
  //Populate only on first constructor call
  if(fmc_vtr.empty())
  {
    fmc_vtr.push_back( fmc_cls("nco_gsl_fit_linear",this,(int)NCO_GSL_FUNC1));
  }
}

var_sct *nco_gsl_cls::fnd(RefAST expr,RefAST fargs,fmc_cls &fmc_obj,ncoTree &walker)
{
  const std::string fnc_nm("nco_gsl_cls::fnd");
  bool is_mtd;
  int fdx=fmc_obj.fdx();   //index
  RefAST tr;    
  std::vector<RefAST> vtr_args; 

  // Put args into vector 
  if(expr)
  {
    vtr_args.push_back(expr);
  }

  if((tr=fargs->getFirstChild())) 
  {
    do  
    vtr_args.push_back(tr);
    while((tr=tr->getNextSibling()));    
  }

  is_mtd=(expr ? true: false);

  switch(fdx)
  {
  case NCO_GSL_FUNC1:
    return fit_fnd(is_mtd,vtr_args,fmc_obj,walker);  
    break;
  case NCO_GSL_FUNC2:
    {
      // function pointer
      var_sct* (*hnd_fnc)(HANDLE_ARGS);
      hnd_fnc=gpr_vtr[fdx]._hnd_fnc;
      return hnd_fnc(is_mtd,vtr_args,gpr_vtr[fdx],walker);
    }
    break;
  default:
    assert(0);
    break;
  }
  
  return (var_sct*)NULL;

} // end nco_gsl_cls::fnd 


var_sct *nco_gsl_cls::fit_fnd(bool &,std::vector<RefAST> &vtr_args,fmc_cls &fmc_obj,ncoTree &walker)
{
  const std::string fnc_nm("nco_gsl_cls::fit_fnd");
  int idx;
  int fdx=fmc_obj.fdx();   //index
  int nbr_args;    // actual nunber of args
  int in_nbr_args; // target number of args
  int in_val_nbr_args; // number of expressions
  int ret; 
  prs_cls* prs_arg=walker.prs_arg;
  std::string sfnm =fmc_obj.fnm(); //method name
  std::string susg;
  std::string serr;    
  vtl_typ lcl_typ;
  var_sct *var_in[13];  
  nbr_args=vtr_args.size();  

  switch(fdx)
  {
  case NCO_GSL_FUNC1:
    in_nbr_args=11;
    in_val_nbr_args=5;
    susg="usage: status="+sfnm+"(data_x,stride_x,data_y,stride_y,n,&co,&c1,&cov00,&cov01,&cov11,&sumsq)";
    break;   
  default:
    assert(0);
    break;
  }   

  if(nbr_args<in_nbr_args)
  {   
    serr="function requires "+ nbr2sng(in_nbr_args)+" arguments. You have only supplied "+nbr2sng(nbr_args)+ " arguments\n"; 
    err_prn(sfnm,serr+susg);
  }

  for(idx=0;idx<in_nbr_args;idx++)
  {
    lcl_typ=expr_typ(vtr_args[idx]);  

    // deal with regular arguments 
    if(idx<in_val_nbr_args)
    { 
      if(lcl_typ == VCALL_REF || lcl_typ == VDIM )
      {
        serr="function requires that " + nbr2sng(idx)+ " argument be a variable name or an expression";
        err_prn(sfnm,serr+susg);
      }
      var_in[idx] = walker.out(vtr_args[idx]);        
    }
    else // deal with call-by-ref variables      
    {
      var_sct *var_tmp=NULL_CEWI;
      std::string var_nm;
      NcapVar  *Nvar;
      var_nm=vtr_args[idx]->getFirstChild()->getText();
      if(lcl_typ != VCALL_REF)
      {
        serr="function requires that " + nbr2sng(idx)+ " argument be a call by reference variable";
        err_prn(sfnm,serr+susg);
      }
      // initial scan
      if(prs_arg->ntl_scn)
      {
        if(prs_arg->ncap_var_init_chk(var_nm))
          var_tmp=prs_arg->ncap_var_init(var_nm,false);  
        else
          var_tmp=ncap_sclr_var_mk(var_nm,NC_DOUBLE,false);
      }
      else // final scan
      {
        // we have a problem here - its possible that in the inital scan
        // that some of the call-by-ref variables have been defined but 
        // not populated. Cannot use ncap_var_init() as this will attempt
        // to read var from input as it is unpopulated 
        Nvar=prs_arg->var_vtr.find(var_nm);
        if(Nvar && Nvar->flg_stt==1)
        {
          var_tmp=Nvar->cpyVarNoData();
          // malloc space for var
          var_tmp->val.vp=nco_malloc(var_tmp->sz * nco_typ_lng(var_tmp->type));   
        }    
        else if(prs_arg->ncap_var_init_chk(var_nm))
        {
          var_tmp=prs_arg->ncap_var_init(var_nm,true);
        }
        else   
        {
          var_tmp=ncap_sclr_var_mk(var_nm,NC_DOUBLE,1.0);
        } //end if/else
      } //end final scan
      // convert to type double
      if(!var_tmp->undefined)
      {
        var_tmp=nco_var_cnf_typ(NC_DOUBLE,var_tmp);
      }
      var_in[idx]=var_tmp;
    } //end call-by-ref vars
  } // end for  

  // inital scan --free up  vars and return 
  if(prs_arg->ntl_scn)
  {
    for(idx=0;idx<in_nbr_args;idx++)
    {
      if(idx<in_val_nbr_args)
      {
        var_in[idx]=nco_var_free(var_in[idx]);
      }
      else
      {
        // write newly defined call by ref args
        // nb this call frees up var_in[idx] 
        prs_arg->ncap_var_write(var_in[idx],false);
      }
    }//end for
    return ncap_sclr_var_mk("~nco_gsl_cls",NC_INT,false);   
  }//end if/inital scan

  // big switch 
  switch(fdx)
  {
  case NCO_GSL_FUNC1:
    // recall aguments in order in var_in
    /* 
    0  x_in
    1  x stride
    2  y_in
    3  y stride
    4  n
    5  c0
    6  c1
    7  c00 
    8  c01
    9  c11
    10  sumsq       
    */
    // convert x,y to type double
    var_in[0]=nco_var_cnf_typ(NC_DOUBLE,var_in[0]);
    var_in[2]=nco_var_cnf_typ(NC_DOUBLE,var_in[2]);

    // make x,y  conform 
    (void)ncap_var_att_cnf(var_in[2],var_in[0]);

    // convert strides to NC_UINT64
    var_in[1]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[1]);   
    var_in[3]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[3]);   
    var_in[4]=nco_var_cnf_typ((nc_type)NC_UINT64,var_in[4]);   

    //cast pointers from void 
    for(idx=0 ;idx<in_nbr_args;idx++)
    {
      (void)cast_void_nctype(var_in[idx]->type,&var_in[idx]->val);
    }

    //get fill value from "y_in" only: assumes type conversion to "double"
    if(nco_dbg_lvl_get() == nco_dbg_old)
    {
      if(var_in[2]->mss_val.dp)
      (void)fprintf(stdout,"fill value for %s=%f\n",var_in[2]->nm,var_in[2]->mss_val.dp[0]);
    }

    // make the call -- 
    ret=nco_gsl_fit_linear(var_in[0]->val.dp,
      var_in[1]->val.ui64p[0],
      var_in[2]->val.dp,
      var_in[3]->val.ui64p[0],     
      var_in[4]->val.ui64p[0],
      var_in[5]->val.dp,
      var_in[6]->val.dp,
      var_in[7]->val.dp,
      var_in[8]->val.dp,
      var_in[9]->val.dp,
      var_in[10]->val.dp,
      var_in[2]->mss_val.dp); //fill value "double" for variable "y_in" (var_in[2]); can be NULL

    assert(ret==NCO_GSL_SUCCESS);

    break; //NCO_GSL_FUNC1 
  default:
    assert(0);
  } // end big switch   

  for(idx=0 ; idx< in_nbr_args ;idx++)
  {
    //cast pointers to void
    (void)cast_nctype_void(var_in[idx]->type,&var_in[idx]->val);
    if(idx<in_val_nbr_args)
      nco_var_free(var_in[idx]);
    else
      // nb this write call also frees up pointer  
      prs_arg->ncap_var_write(var_in[idx],false);
  }
  // return status flag
  return ncap_sclr_var_mk("~nco_gsl_cls",(nco_int)ret);   
} // end nco_gsl_cls::fit_fnd 

#endif // !ENABLE_GSL


