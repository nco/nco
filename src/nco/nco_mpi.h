/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mpi.h,v 1.1 2005-09-21 20:06:54 zender Exp $ */

/* Purpose: MPI utilities */

/* NB: Currently this header is only accessed by main() MPI programs 
   Hence it is not (yet) part of libnco.a
   Header assumes its inclusion is already conditional on ENABLE_MPI
   This is currently a header mostly for pre-processor tokens 
   with few odd file scope constants needed by MPI routines 
   No functions yet
   Adding functions would require creating a corresponding *.c file
   Adding a *.c file would require including that in libnco.a or separately linking
   the MPI executables to it, i.e., slightly complicating the build procedure.
   I'm not yet sure I want _any_ MPI dependencies in libnco.a 
   Cross-platform link weirdness (e.g., AIX) makes this something to avoid if possible */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_mpi.h" *//* MPI utilities */

#ifndef NCO_MPI_H
#define NCO_MPI_H

/* Standard header files */

/* 3rd party vendors */

/* Personal headers */

/* Tags used for MPI Communication */
#define TOKEN_ALLOC 1
#define TOKEN_REQUEST 300
#define TOKEN_RESULT 500
#define TOKEN_WAIT 0
#define WORK_ALLOC 400
#define WORK_DONE 600
#define WORK_REQUEST 100

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const int mgr_id=0; /* [ID] Manager ID */
const int NO_MORE_WORK=-1; /* [flg] All MPI variables processed */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MPI_H */
