/* $Header$ */

/* Purpose: MPI utilities */

/* NB: Currently this header is only accessed by main() MPI programs 
   Hence it is not (yet) part of libnco.a
   Header assumes its inclusion depends on ENABLE_MPI
   Header is currently mostly for file-scope constants needed by MPI routines 
   No functions (yet) but some will eventually live here after MPI functionalization
   Adding functions will require creating a corresponding *.c file
   *.c files must reside in libnco.a or link separately to MPI executables
   The latter would slightly complicate the build procedure
   Should _any_ MPI dependencies be in libnco.a?
   Cross-platform link weirdness (e.g., AIX) makes MPI dependencies in libnco.a fragile */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_mpi.h" *//* MPI utilities */

#ifndef NCO_MPI_H
#define NCO_MPI_H

/* Standard header files */
#include <signal.h> /* Signal handling */

/* 3rd party vendors */

/* Personal headers */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  const int msg_bfr_lng=3; /* [nbr] Number of elements in msg_bfr */
  const int wrk_id_bfr_lng=1; /* [nbr] Number of elements in wrk_id_bfr */

  /* Sleep interval between successive write token requests
     fxm: TODO nco609 tune or remove this */
  const double tkn_wrt_rqs_ntv=0.04; /* [s] Token request interval */
  
  /* Convention is to assign manager responsibilities to rank 0 process
     NB: Manager rank must be < number of processes */
  const int rnk_mgr=0; /* [idx] Manager rank */

  /* Pass idx_all_wrk_ass in place of variable ID when all variables assigned
     Values for idx_all_wrk_ass must be exclusive of valid variable IDs
     Hence use a negative integer to signify idx_all_wrk_ass */
  const int idx_all_wrk_ass=-1; /* [enm] All variables already assigned */

  /* NB: Message fields must begin in location zero */
  /* fxm: Define message structures rather than int arrays */
  const int msg_lmn_tkn_wrt_rsp=0; /* [idx] Location of response */

  /* Requests for the write token have two possible responses */
  enum tkn_wrt_rsp_val{ /* [enm] Valid responses to write token requests */
    tkn_wrt_rqs_dny, /* Deny request for write token (must wait and re-request) */
    tkn_wrt_rqs_xcp /* Accept request for write token, OK to write */
  }; /* end tkn_wrt_rsp_val enum */

  /* These (and MPI_ANY_TAG) are valid entries for the MPI message tag field 
     Processes may request to filter messages based on any of these tags */
  enum nco_msg_tag_typ{ /* [enm] MPI message tag */
    msg_tag_wrk_done, /* Notification that work is complete */
    msg_tag_wrk_rqs, /* Request for work */
    msg_tag_wrk_rsp, /* Response to work request */
    msg_tag_tkn_wrt_rqs, /* Request for write token */
    msg_tag_tkn_wrt_rsp  /* Response to write token request */
  }; /* end nco_msg_tag_typ enum */

  const int nco_spn_lck_us=100; /* [us] Spinlock sleep interval */
  int nco_spn_lck_brk=0; /* [nbr] Break spin lock, resume execution */

  void nco_cnt_run(int signo); /* Signal handler */
  void nco_cnt_run(int signo){nco_spn_lck_brk=1+0*signo;} /* Signal handler, CEWI */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MPI_H */
