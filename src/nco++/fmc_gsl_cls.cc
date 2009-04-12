/* $Header: /data/zender/nco_20150216/nco/src/nco++/fmc_gsl_cls.cc,v 1.28 2009-04-12 17:32:22 hmb Exp $ */

/* Purpose: netCDF arithmetic processor class methods for GSL */

/* Copyright (C) 1995--2009 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "fmc_gsl_cls.hh"

// GSL Functions

#ifdef ENABLE_GSL

gsl_cls::gsl_cls(bool flg_dbg){
    // populate gpr_vtr
  gsl_ini_sf();    // Special Functions 
  gsl_ini_cdf();   // Cumulative distribution Functions
  gsl_ini_ran();   // Random Number Generator functions
    
  // Copy into fmc_cls vector  
  for(unsigned idx=0;idx<gpr_vtr.size();idx++) 
   fmc_vtr.push_back(fmc_cls(gpr_vtr[idx].fnm(),this,idx));
 
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

# if NCO_GSL_MINOR_VERSION >= 8
      gpr_vtr.push_back(gpr_cls("gsl_sf_debye_5",f_unn(gsl_sf_debye_5_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_debye_6",f_unn(gsl_sf_debye_6_e),hnd_fnc_x,NC_DOUBLE));
# endif // NCO_GSL_MINOR_VERSION < 8
    
    // Dilogarithm
    // not implemented as all involve complex numbers
    
    // Elementary Operations
    
    // Elliptic Integrals
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Kcomp",f_unn(gsl_sf_ellint_Kcomp_e),hnd_fnc_nd,P1DBLMD));
    gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Ecomp",f_unn(gsl_sf_ellint_Ecomp_e),hnd_fnc_nd,P1DBLMD));

# if NCO_GSL_MINOR_VERSION >= 9 
      gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Pcomp",f_unn(gsl_sf_ellint_Pcomp_e),hnd_fnc_nd,P2DBLMD));
      gpr_vtr.push_back(gpr_cls("gsl_sf_ellint_Dcomp",f_unn(gsl_sf_ellint_Dcomp_e),hnd_fnc_nd,P1DBLMD));
# endif // NCO_GSL_MINOR_VERSION < 9
 
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

# if NCO_GSL_MINOR_VERSION >= 10 
      gpr_vtr.push_back(gpr_cls("gsl_sf_expint_En",f_unn(gsl_sf_expint_En_e),hnd_fnc_xd,NC_INT));
      gpr_vtr.push_back(gpr_cls("gsl_sf_expint_En_scaled",f_unn(gsl_sf_expint_En_scaled_e),hnd_fnc_xd,NC_INT));
# endif // NCO_GSL_MINOR_VERSION < 10

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
    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Plm_array",f_unn(gsl_sf_legendre_Plm_array),hnd_fnc_iidpd,PLEGEND));
    // gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_Plm_deriv_array",f_unn(gsl_sf_legendre_Plm_deriv_array),));
    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm",f_unn(gsl_sf_legendre_sphPlm_e),hnd_fnc_iid));
    gpr_vtr.push_back(gpr_cls("gsl_sf_legendre_sphPlm_array",f_unn(gsl_sf_legendre_sphPlm_array),hnd_fnc_iidpd,PLEGEND));
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
   
# if NCO_GSL_MINOR_VERSION >= 5 
      gpr_vtr.push_back(gpr_cls("gsl_sf_psi_1",f_unn(gsl_sf_psi_1_e),hnd_fnc_x,NC_DOUBLE));
# endif // NCO_GSL_MINOR_VERSION < 5

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

# if NCO_GSL_MINOR_VERSION >= 5 
      gpr_vtr.push_back(gpr_cls("gsl_sf_zetam1",f_unn(gsl_sf_zetam1_e),hnd_fnc_x,NC_DOUBLE));
      gpr_vtr.push_back(gpr_cls("gsl_sf_zetam1_int",f_unn(gsl_sf_zetam1_int_e),hnd_fnc_x,NC_INT));
# endif // NCO_GSL_MINOR_VERSION < 5
    
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

    gpr_vtr.push_back(gpr_cls("gsl_cdf_exppow_P",f_unn(gsl_cdf_exppow_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_exppow_Q",f_unn(gsl_cdf_exppow_Q),hnd_fnc_nd,P3DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_P",f_unn(gsl_cdf_tdist_P),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_Q",f_unn(gsl_cdf_tdist_Q),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_Pinv",f_unn(gsl_cdf_tdist_Pinv),hnd_fnc_nd,P2DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_tdist_Qinv",f_unn(gsl_cdf_tdist_Qinv),hnd_fnc_nd,P2DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_P",f_unn(gsl_cdf_fdist_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_Q",f_unn(gsl_cdf_fdist_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_Pinv",f_unn(gsl_cdf_fdist_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_fdist_Qinv",f_unn(gsl_cdf_fdist_Qinv),hnd_fnc_nd,P3DBLX));

    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_P",f_unn(gsl_cdf_beta_P),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_Q",f_unn(gsl_cdf_beta_Q),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_Pinv",f_unn(gsl_cdf_beta_Pinv),hnd_fnc_nd,P3DBLX));
    gpr_vtr.push_back(gpr_cls("gsl_cdf_beta_Qinv",f_unn(gsl_cdf_beta_Qinv),hnd_fnc_nd,P3DBLX));

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
  //gpr_vtr.push_back(gpr_cls("gsl_ran_bernoulli_pdf",f_unn(gsl_ran_bernoulli_pdf),hnd_fnc_rnd,P1DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_beta",f_unn(gsl_ran_beta),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_beta_pdf",f_unn(gsl_ran_beta_pdf),hnd_fnc_nd,P3DBLX));

  //gpr_vtr.push_back(gpr_cls("gsl_ran_binomial",f_unn(gsl_ran_binomial),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_binomial_knuth",f_unn(gsl_ran_binomial_knuth),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_binomial_tpe",f_unn(gsl_ran_binomial_tpe),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_binomial_pdf",f_unn(gsl_ran_binomial_pdf),hnd_fnc_rnd,P1DBLX));
 
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
  //gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_int",f_unn(gsl_ran_gamma_int),hnd_fnc_udru,NC_UINT));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_pdf",f_unn(gsl_ran_gamma_pdf),hnd_fnc_nd,P3DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_mt",f_unn(gsl_ran_gamma_mt),hnd_fnc_rnd,P2DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gamma_knuth",f_unn(gsl_ran_gamma_knuth),hnd_fnc_rnd,P2DBLX));

  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian",f_unn(gsl_ran_gaussian),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian_ratio_method",f_unn(gsl_ran_gaussian_ratio_method),hnd_fnc_rnd,P1DBLX));
  gpr_vtr.push_back(gpr_cls("gsl_ran_gaussian_ziggurat",f_unn(gsl_ran_gaussian_ziggurat),hnd_fnc_rnd,P1DBLX));
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
  //gpr_vtr.push_back(gpr_cls("gsl_ran_geometric_pdf",f_unn(gsl_ran_geometric_pdf),hnd_fnc_nd,P1DBLX));

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

  //gpr_vtr.push_back(gpr_cls("gsl_ran_pascal",f_unn(gsl_ran_pascal),hnd_fnc_rnd,P1DBLX));
  //gpr_vtr.push_back(gpr_cls("gsl_ran_pascal_pdf",f_unn(gsl_ran_pascal_pdf),hnd_fnc_rnd,P1DBLX));

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
      int sz=var->sz;
      double mss_val_dbl=0.0; 
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
      double mss_val_dbl=0; 
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
    
    return ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nc_type)NC_INT,false); 
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
    
    var_tmp=ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nco_int)status);
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
  var_sct *var_arr[2];
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
    
    return ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nc_type)NC_INT,false);   
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
    
    var_tmp=ncap_sclr_var_mk(static_cast<std::string>("~gsl_function"),(nco_int)status);
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
  nc_type type;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var_ret; 
  var_sct **var_arr;
  var_sct ***var_arr_ptr;
  
  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  type=gpr_obj.type();  
  
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


var_sct *gsl_cls::hnd_fnc_rnd(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
  const std::string fnc_nm("hnd_fnc_rnd");
  int idx;
  int jdx;
  int fdx=gpr_obj.type(); // very important 
  int args_nbr;
  int args_in_nbr(-1); // CEWI
  nc_type type;
  std::string styp=(is_mtd ? "method":"function");
  std::string sfnm=gpr_obj.fnm();
  var_sct *var_ret; 
  var_sct **var_arr;
  var_sct ***var_arr_ptr;
  // GSL random Number stuff
  gsl_rng *ncap_rng;

  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  

  // de-reference 
  prs_cls *prs_arg=walker.prs_arg;
  type=gpr_obj.type();  
  
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
      var=nco_var_cnf_typ((nc_type)NC_UINT64,var);                     
    return var;
  }


  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  
  
  switch(type){ 
    
  case NC_UINT:
    { 
      long sz=var->sz;
      nco_uint64  mss_val_lng=0LL; 
      nco_uint64 *lp;
      
      unsigned (*fnc_int)(const gsl_rng*,unsigned);
      
      fnc_int=gpr_obj.g_args().dru; 
      
      var=nco_var_cnf_typ((nc_type)NC_UINT64,var);                     
      (void)cast_void_nctype((nc_type)NC_UINT64,&(var->val));
      lp=var->val.ui64p;  
      
      if(var->has_mss_val)
        mss_val_lng=var->mss_val.lp[0]; 
       
      
      if(var->has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(lp[idx] != mss_val_lng)
            lp[idx]=(*fnc_int)(ncap_rng,lp[idx]);   

      }else{
	for(idx=0;idx<sz;idx++) 
	  lp[idx]=(*fnc_int)(ncap_rng,lp[idx]);
      }
      
      
      (void)cast_nctype_void((nc_type)NC_UINT64,&(var->val));
      
      
    } 
    break;  
    
  case NC_DOUBLE:
    { 
      bool has_mss_val;
      int sz=var->sz;
      nco_uint64 *lp;
      double mss_val_dbl;
      double *dp;

      var_sct *var_out;
      
      unsigned (*fnc_int)(const gsl_rng*,double);
      
      

      fnc_int=gpr_obj.g_args().drd; 

      // convert to double
      var=nco_var_cnf_typ(NC_DOUBLE,var);                     

      var_out=nco_var_dpl(var);

      var_out=nco_var_cnf_typ((nc_type)NC_UINT64,var_out);                        

      (void)cast_void_nctype((nc_type)NC_UINT64,&(var_out->val));
      lp=var_out->val.ui64p;  
      
      
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
	   lp[idx]=(*fnc_int)(ncap_rng,dp[idx]);
      }else{
	for(idx=0;idx<sz;idx++) 
	  lp[idx]=(*fnc_int)(ncap_rng,dp[idx]);
      }
      
      
      (void)cast_nctype_void(NC_DOUBLE,&(var->val));
      var=nco_var_free(var);
     
      (void)cast_nctype_void((nc_type)NC_UINT64,&(var_out->val));     
      var=var_out; 
  

    } 
    break;
    
  default:
    break; 
    
  } // end big switch 

  gsl_rng_free(ncap_rng);
  
  return var;
} // end hnd_fnc_udrx




var_sct *gsl_cls::hnd_fnc_uerx(bool& is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker ){
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
      var=nco_var_cnf_typ((nc_type)NC_UINT64,var);                     
    return var;
  }


  // initialize 
  ncap_rng=gsl_rng_alloc(gsl_rng_default);  
  
  switch(type){ 
    
  case NC_NAT:
    { 
      long sz=var->sz;
      nco_uint64  mss_val_lng=0LL; 
      nco_uint64 *lp;
      
      unsigned long int (*fnc_int)(const gsl_rng*);
      
      fnc_int=gpr_obj.g_args().er; 
      
      var=nco_var_cnf_typ((nc_type)NC_UINT64,var);                     
      (void)cast_void_nctype((nc_type)NC_UINT64,&(var->val));
      lp=var->val.ui64p;  
      
      /* missing values 
      if(var->has_mss_val)
        mss_val_lng=var->mss_val.lp[0]; 
       
      //  missing values ? 
      
      if(var->has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(lp[idx] != mss_val_lng)
            lp[idx]=(*fnc_int)(ncap_rng,lp[idx]);   

      */
      for(idx=0;idx<sz;idx++) 
	lp[idx]=(*fnc_int)(ncap_rng);
      
      (void)cast_nctype_void((nc_type)NC_UINT64,&(var->val));
      
      
    } 
    break;  
    
  case NC_UINT:
    { 
      bool has_mss_val;
      int sz=var->sz;
      nco_uint64 *lp;
      nco_uint64 mss_val_lng;
      double *dp; 

      unsigned long int(*fnc_int)(const gsl_rng*,unsigned long int);
      
      fnc_int=gpr_obj.g_args().eru; 


      var=nco_var_cnf_typ((nc_type)NC_UINT64,var);                        

      (void)cast_void_nctype((nc_type)NC_UINT64,&(var->val));
      lp=var->val.ui64p;  
      
      
      has_mss_val=false;
      if(var->has_mss_val){
	has_mss_val=true;
	(void)cast_void_nctype((nc_type)NC_UINT64,&(var->mss_val));
	mss_val_lng=var->mss_val.ui64p[0];    
	(void)cast_nctype_void((nc_type)NC_UINT64,&(var->mss_val));
      }
      
      if(has_mss_val){  
	for(idx=0;idx<sz;idx++)
	  if(lp[idx] != mss_val_lng)
	   lp[idx]=(*fnc_int)(ncap_rng,lp[idx]);
      }else{
	for(idx=0;idx<sz;idx++) 
	  lp[idx]=(*fnc_int)(ncap_rng,lp[idx]);
      }
      
      
      (void)cast_nctype_void((nc_type)NC_UINT64,&(var->val));     

  

    } 
    break;
    
  default:
    break; 
    
  } // end big switch 

  gsl_rng_free(ncap_rng);
  
  return var;
} // end hnd_fnc_uerx








//GSL2 Class /******************************************/
  gsl2_cls::gsl2_cls(bool  flg_dbg){
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
    var_sct *var;
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

    if(tr=fargs->getFirstChild()) {
      do  
	vtr_args.push_back(tr);
      while(tr=tr->getNextSibling());    
    } 
      
     nbr_args=vtr_args.size();  
      
     if(nbr_args >0) 
       wrn_prn(fnc_nm,styp+" \""+sfnm+"\" requires no arguments"); 



    // If initial scan
    if(prs_arg->ntl_scn){

      switch(fdx) {
       case  PGSL_RNG_MIN:
       case  PGSL_RNG_MAX:
        return ncap_sclr_var_mk(static_cast<std::string>("~gsl2_functions"),(nc_type)NC_UINT,false);        
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
             var=ncap_sclr_var_mk(static_cast<std::string>("~gsl2_function"),static_cast<double>(vlng));
             break;
           case PGSL_RNG_MAX:
             vlng= gsl_rng_max(ncap_rng);
             var=ncap_sclr_var_mk(static_cast<std::string>("~gsl2_function"),static_cast<double>(vlng));
             break;
           case PGSL_RNG_NAME:
	    //var=ncap_sclr_var_mk(static_cast<std::string>("~gsl2_functions"),(nco_int)var1->nbr_dim);            
	    break;

    }// end switch        
      



  // free Random Number generator
  gsl_rng_free(ncap_rng);
  return var;		 

}       



















#else // !ENABLE_GSL

/* Dummy stub function so fmc_gsl_cls.o is not empty when GSL unavailable
   Function should never be called */
int gsl_dmm_stb(void);
int gsl_dmm_stb(void){return 1;}
 
#endif // !ENABLE_GSL
