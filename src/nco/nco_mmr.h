/* $Header$ */

/* Purpose: Memory management */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_mmr.h" *//* Memory management */

#ifndef NCO_MMR_H
#define NCO_MMR_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <errno.h> /* system/library error diagnostics, errno */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, getenv, exit */
#include <string.h> /* strcmp() */
#ifdef MACOSX
# include <sys/time.h> /* machine time (needed by Mac OS X for struct rusage) */ 
#endif /* !MACOSX */
#ifdef HAVE_GETRUSAGE
# include <sys/resource.h> /* Resource usage and limits */
#endif /* !HAVE_GETRUSAGE */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
#endif /* !_MSC_VER */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* http://www.cs.tufts.edu/comp/111/assignments/a3/proc.c */

  /* Number of fields in /proc/PID/stat output, same as
     Number of elements of prc_stt_sct structure */
#define PRC_STT_SCT_NBR 42
  typedef struct{ /* prc_stt_sct */
    int pid;			// %d Member 01
    char comm[256];		// %s Member 02
    char state;			// %c Member 03
    int ppid;			// %d Member 04
    int pgrp;			// %d Member 05
    int session;		// %d Member 06
    int tty_nr;			// %d Member 07
    int tpgid;			// %d Member 08
    unsigned long flags;	// %lu Member 09
    unsigned long minflt;	// %lu Member 10
    unsigned long cminflt;	// %lu Member 11
    unsigned long majflt;	// %lu Member 12
    unsigned long cmajflt;	// %lu Member 13
    unsigned long utime;	// %lu Member 14
    unsigned long stime; 	// %lu Member 15
    long cutime;		// %ld Member 16
    long cstime;		// %ld Member 17
    long priority;		// %ld Member 18
    long nice;			// %ld Member 19
    long num_threads;		// %ld Member 20
    long itrealvalue;		// %ld Member 21
    unsigned long starttime;	// %lu Member 22
    unsigned long vsize;	// %lu Member 23
    long rss;			// %ld Member 24
    unsigned long rlim;		// %lu Member 25
    unsigned long startcode;	// %lu Member 26
    unsigned long endcode;	// %lu Member 27
    unsigned long startstack;	// %lu Member 28
    unsigned long kstkesp;	// %lu Member 29
    unsigned long kstkeip;	// %lu Member 30
    unsigned long signal;	// %lu Member 31
    unsigned long blocked;	// %lu Member 32
    unsigned long sigignore;	// %lu Member 33
    unsigned long sigcatch;	// %lu Member 34
    unsigned long wchan;	// %lu Member 35
    unsigned long nswap;	// %lu Member 36
    unsigned long cnswap;	// %lu Member 37
    int exit_signal;		// %d Member 38
    int processor;		// %d Member 39
    unsigned long rt_priority;	// %lu Member 40
    unsigned long policy;	// %lu  Member 41
    unsigned long long delayacct_blkio_ticks;	// %llu Member 42
  } prc_stt_sct;

  /* Number of fields in /proc/PID/statm output, same as
     Number of elements of prc_stm_sct structure */
#define PRC_STM_SCT_NBR 7
  typedef struct{ /* prc_stm_sct */
    unsigned long size;	// %lu Member 1
    unsigned long resident; // %lu Member 2
    unsigned long share; // %lu Member 3
    unsigned long text;	// %lu Member 4
    unsigned long lib;// %lu Member 5
    unsigned long data;	// %lu Member 6
    unsigned long dt; // %lu Member 7
  } prc_stm_sct;

  int /* Return code */
  nco_prc_stt_get /* [fnc] Read /proc/PID/stat */
  (const int pid, /* [enm] Process ID to read */
   prc_stt_sct *prc_stt); /* [sct] Structure to hold results */

  int /* Return code */
  nco_prc_stm_get /* [fnc] Read /proc/PID/statm */
  (const int pid, /* [enm] Process ID to read */
   prc_stm_sct *prc_stm); /* [sct] Structure to hold results */

void * /* O [ptr] Pointer to calloc'd memory */
nco_calloc /* [fnc] Wrapper for calloc() */
(const size_t lmn_nbr, /* I [nbr] Number of elements to allocate */
 const size_t lmn_sz); /* I [nbr] Size of each element */

void * /* O [ptr] Buffer after free'ing */
nco_free /* [fnc] Wrapper for free() */
(void *vp); /* I/O [ptr] Buffer to free() */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc /* [fnc] Wrapper for malloc() */
(const size_t size); /* I [B] Bytes to allocate */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc_flg /* [fnc] Wrapper for malloc(), forgives ENOMEM errors */
(const size_t size); /* I [B] Bytes to allocate */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc_dbg /* [fnc] Wrapper for malloc(), receives and prints more diagnostics */
(const size_t sz, /* I [B] Bytes to allocate */
 const char *fnc_nm, /* I [sng] Function name */
 const char *msg); /* I [sng] Supplemental error message */

void
nco_malloc_err_hnt_prn /* [fnc] Explain meaning and workarounds for malloc() failures */
(void);

long /* O [B] Maximum resident set size */
nco_mmr_usg_prn /* [fnc] Print rusage memory usage statistics */
(const int rusage_who); /* [enm] RUSAGE_SELF, RUSAGE_CHILDREN, RUSAGE_LWP */

long /* O [nbr] Net memory currently allocated */
nco_mmr_stt /* [fnc] Track memory statistics */
(const nco_mmr_typ_enm nco_mmr_typ, /* I [enm] Memory allocation type */
 const size_t sz); /* I [B] Bytes allocated, deallocated, or reallocated */

const char * /* O [sng] String describing memory type */
nco_mmr_typ_sng /* [fnc] Convert NCO memory management type enum to string */
(const nco_mmr_typ_enm nco_mmr_typ); /* I [enm] NCO memory management type */

void * /* O [ptr] Pointer to re-allocated memory */
nco_realloc /* [fnc] Wrapper for realloc() */
(void *ptr, /* I/O [ptr] Buffer to reallocate */
 const size_t size); /* I [B] Bytes required */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MMR_H */
