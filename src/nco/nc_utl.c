/* $Header: /data/zender/nco_20150216/nco/src/nco/nc_utl.c,v 1.128 2002-01-29 08:40:19 zender Exp $ */

/* Purpose: netCDF-dependent utilities for NCO netCDF operators */

/* Copyright (C) 1995--2002 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the NetCDF and HDF libraries 
   and distribute the resulting executables under the terms of the GPL, 
   but in addition obeying the extra stipulations of the netCDF and 
   HDF library licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   The file LICENSE contains the GNU General Public License, version 2
   It may be viewed interactively by typing, e.g., ncks -L

   The author of this software, Charlie Zender, would like to receive
   your suggestions, improvements, bug-reports, and patches for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
 */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* POSIX stuff */
#include <assert.h> /* assert() debugging macro */
/* #include <errno.h> */            /* errno */
/* #include <malloc.h>    */        /* malloc() stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netcdf3.x wrappers */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* not _OPENMP */

/* Personal headers */
#include "nco.h" /* netCDF operator universal def'ns */

void
cast_void_nctype(nc_type type,ptr_unn *ptr)
/*  
   nc_type type: I netCDF type to cast void pointer to
   ptr_unn *ptr: I/O pointer to pointer union whose vp element will be cast to type type
*/
{
  /* Cast generic pointer in ptr_unn structure from type void to output netCDF type */
  switch(type){
  case NC_FLOAT:
    ptr->fp=(float *)ptr->vp;
    break;
  case NC_DOUBLE:
    ptr->dp=(double *)ptr->vp;
    break;
  case NC_INT:
    ptr->lp=(nco_long *)ptr->vp;
    break;
  case NC_SHORT:
    ptr->sp=(short *)ptr->vp;
    break;
  case NC_CHAR:
    ptr->cp=(unsigned char *)ptr->vp;
    break;
  case NC_BYTE:
    ptr->bp=(signed char *)ptr->vp;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end cast_void_nctype() */

void
cast_nctype_void(nc_type type,ptr_unn *ptr)
/*  
   nc_type type: I netCDF type of pointer
   ptr_unn *ptr: I/O pointer to pointer union which to cast from type type to type void
*/
{
  /* Cast generic pointer in ptr_unn structure from type type to type void */
  switch(type){
  case NC_FLOAT:
    ptr->vp=(void *)ptr->fp;
    break;
  case NC_DOUBLE:
    ptr->vp=(void *)ptr->dp;
    break;
  case NC_INT:
    ptr->vp=(void *)ptr->lp;
    break;
  case NC_SHORT:
    ptr->vp=(void *)ptr->sp;
    break;
  case NC_CHAR:
    ptr->vp=(void *)ptr->cp;
    break;
  case NC_BYTE:
    ptr->vp=(void *)ptr->bp;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end cast_nctype_void() */

void 
lmt_evl(int nc_id,lmt_sct *lmt_ptr,long cnt_crr,bool FORTRAN_STYLE)
/* 
   int nc_id: I [idx] netCDF file ID
   lmt_sct *lmt_ptr: I/O [sct] Structure from lmt_prs() or from lmt_sct_mk() to hold dimension limit information
   long cnt_crr: I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators)
   bool FORTRAN_STYLE: I [flg] Switch to determine syntactical interpretation of dimensional indices
 */
{
  /* Threads: Routine is thread-unsafe */
  /* Purpose: Take a parsed list of dimension names, minima, and
     maxima strings and find appropriate indices into dimensions 
     for formulation of dimension start and count vectors, or fail trying. */

  bool flg_no_data=False; /* True if file contains no data for hyperslab */
  bool rec_dmn_and_mlt_fl_opr=False; /* True if record dimension in multi-file operator */

  dmn_sct dim;

  enum monotonic_direction{
    decreasing, /* 0 */
    increasing}; /* 1 */

  lmt_sct lmt;

  int min_lmt_typ=int_CEWI;
  int max_lmt_typ=int_CEWI;
  int monotonic_direction;
  int prg_id; /* Program ID */
  int rcd=NC_NOERR; /* [enm] Return code */
  int rec_dmn_id; /* [idx] Variable ID of record dimension, if any */

  long idx;
  long dmn_sz;
  long cnt_rmn_crr=-1L; /* Records to extract from current file */
  long cnt_rmn_ttl=-1L; /* Total records remaining to be read from this and all remaining files */
  long rec_skp_vld_prv_dgn=-1L; /* Records skipped at end of previous valid file (diagnostic only) */

  lmt=*lmt_ptr;

  prg_id=prg_get(); /* Program ID */

  /* Initialize limit structure */
  lmt.srd=1L;
  lmt.min_val=0.0;
  lmt.max_val=0.0;

  /* Get dimension ID */
  rcd=nco_inq_dimid_flg(nc_id,lmt.nm,&lmt.id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR dimension %s is not in input file\n",prg_nm_get(),lmt.nm);
    exit(EXIT_FAILURE);
  } /* endif */

  /* Logic on whether to allow skipping current file depends on whether limit
     is specified for record dimension in multi-file operators.
     This information is not used in single-file operators, but whether
     the limit is a record limit may be tested.
     Best to program defensively and define this flag in all cases. */
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  if(lmt.id == rec_dmn_id) lmt.is_rec_dmn=True; else lmt.is_rec_dmn=False;
  if(lmt.is_rec_dmn && (prg_id == ncra || prg_id == ncrcat)) rec_dmn_and_mlt_fl_opr=True; else rec_dmn_and_mlt_fl_opr=False;

  /* Get dimension size */
  (void)nco_inq_dimlen(nc_id,lmt.id,&dim.sz);
  
  /* Shortcut to avoid indirection */
  dmn_sz=dim.sz;

  /* Bomb if dmn_sz < 1 */
  if(dmn_sz < 1){
    (void)fprintf(stdout,"%s: ERROR Size of dimension \"%s\" is %li in input file, but must be > 0 in order to apply limits.\n",prg_nm_get(),lmt.nm,dmn_sz);
    exit(EXIT_FAILURE);
  } /* end if */
  
  if(lmt.srd_sng != NULL){
    if(strchr(lmt.srd_sng,'.') || strchr(lmt.srd_sng,'e') || strchr(lmt.srd_sng,'E') || strchr(lmt.srd_sng,'d') || strchr(lmt.srd_sng,'D')){
      (void)fprintf(stdout,"%s: ERROR Requested stride for \"%s\", %s, must be integer\n",prg_nm_get(),lmt.nm,lmt.srd_sng);
      exit(EXIT_FAILURE);
    } /* end if */
    lmt.srd=strtol(lmt.srd_sng,(char **)NULL,10);
    if(lmt.srd < 1){
      (void)fprintf(stdout,"%s: ERROR Stride for \"%s\" is %li but must be > 0\n",prg_nm_get(),lmt.nm,lmt.srd);
      exit(EXIT_FAILURE);
    } /* end if */
  } /* end if */

  /* If min_sng and max_sng are both NULL then set type to lmt_dmn_idx */
  if(lmt.min_sng == NULL && lmt.max_sng == NULL){
    /* Limiting indices will be set to default extrema a bit later */
    min_lmt_typ=max_lmt_typ=lmt_dmn_idx;
  }else{
    /* min_sng and max_sng are not both NULL */
    /* Limit is coordinate value if string contains decimal point or is in exponential format 
     Otherwise limit is interpreted as zero-based dimension offset */
  
    if(lmt.min_sng != NULL) min_lmt_typ=(strchr(lmt.min_sng,'.') || strchr(lmt.min_sng,'e') || strchr(lmt.min_sng,'E') || strchr(lmt.min_sng,'d') || strchr(lmt.min_sng,'D')) ? lmt_crd_val : lmt_dmn_idx;
    if(lmt.max_sng != NULL) max_lmt_typ=(strchr(lmt.max_sng,'.') || strchr(lmt.max_sng,'e') || strchr(lmt.max_sng,'E') || strchr(lmt.max_sng,'d') || strchr(lmt.max_sng,'D')) ? lmt_crd_val : lmt_dmn_idx;
    
    /* Copy lmt_typ from defined limit to undefined */
    if(lmt.min_sng == NULL) min_lmt_typ=max_lmt_typ;
    if(lmt.max_sng == NULL) max_lmt_typ=min_lmt_typ;
  } /* end else */
  
  /* Both min_lmt_typ and max_lmt_typ are now defined
     Continue only if both limits are of the same type */
  if(min_lmt_typ != max_lmt_typ){
    (void)fprintf(stdout,"%s: ERROR -d %s,%s,%s\n",prg_nm_get(),lmt.nm,lmt.min_sng,lmt.max_sng);
    (void)fprintf(stdout,"Limits on dimension \"%s\" must be of same numeric type:\n",lmt.nm);
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.min_sng,(min_lmt_typ == lmt_crd_val) ? "coordinate value" : "zero-based dimension index");
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.max_sng,(max_lmt_typ == lmt_crd_val) ? "coordinate value" : "zero-based dimension index");
    (void)fprintf(stdout,"(Limit arguments containing a decimal point are interpreted as coordinate values; arguments without a decimal point are interpreted as zero-based dimensional indices.)\n");
    exit(EXIT_FAILURE);
  } /* end if */
  lmt.lmt_typ=min_lmt_typ;
  
  if(lmt.lmt_typ == lmt_crd_val){
    double *dmn_val_dp;

    double dmn_max;
    double dmn_min;

    long max_idx;
    long min_idx;
    long tmp_idx;
    long dmn_srt=0L;

    /* Get variable ID of coordinate */
    rcd=nco_inq_varid(nc_id,lmt.nm,&dim.cid);
    
    /* Get coordinate type */
    (void)nco_inq_vartype(nc_id,dim.cid,&dim.type);
    
    /* Warn when coordinate type is weird */
    if(dim.type == NC_BYTE || dim.type == NC_CHAR) (void)fprintf(stderr,"\nWARNING: Coordinate %s is type %s. Dimension truncation is unpredictable.\n",lmt.nm,nco_typ_sng(dim.type));
    
    /* Allocate enough space to hold coordinate */
    dim.val.vp=(void *)nco_malloc(dmn_sz*nco_typ_lng(dim.type));
    
    /* Retrieve coordinate */
    nco_get_vara(nc_id,dim.cid,&dmn_srt,&dmn_sz,dim.val.vp,dim.type);
    
    /* Convert coordinate to double-precision if neccessary */
    if(dim.type != NC_DOUBLE){
      ptr_unn old_val;

      old_val=dim.val;
      dim.val.vp=(void *)nco_malloc(dmn_sz*nco_typ_lng(NC_DOUBLE));
      /* Typecast old coordinate pointer union to correct type before access */
      (void)cast_void_nctype(dim.type,&old_val);

      /* Shortcut to avoid indirection */
      dmn_val_dp=dim.val.dp;
      switch(dim.type){
      case NC_FLOAT: for(idx=0L;idx<dmn_sz;idx++) {dmn_val_dp[idx]=old_val.fp[idx];} break; 
      case NC_DOUBLE: for(idx=0L;idx<dmn_sz;idx++) {dmn_val_dp[idx]=old_val.dp[idx];} break; 
      case NC_INT: for(idx=0L;idx<dmn_sz;idx++) {dmn_val_dp[idx]=old_val.lp[idx];} break;
      case NC_SHORT: for(idx=0L;idx<dmn_sz;idx++) {dmn_val_dp[idx]=old_val.sp[idx];} break;
      case NC_CHAR: for(idx=0L;idx<dmn_sz;idx++) {dmn_val_dp[idx]=old_val.cp[idx];} break;
      case NC_BYTE: for(idx=0L;idx<dmn_sz;idx++) {dmn_val_dp[idx]=old_val.bp[idx];} break;
      default: nco_dfl_case_nctype_err(); break;
      } /* end switch */

      /* Un-typecast pointer to values after access */
      (void)cast_nctype_void(dim.type,&old_val);
    
      /* Free original space allocated for dimension */
      old_val.vp=nco_free(old_val.vp);

      /* Officially change type */
      dim.type=NC_DOUBLE;
    } /* end type conversion */

    /* Shortcut to avoid indirection */
    dmn_val_dp=dim.val.dp;

    /* Assuming coordinate is monotonic, direction of monotonicity is determined by first two elements */
    if(dmn_sz == 1){
      monotonic_direction=increasing;
    }else{
      if(dmn_val_dp[0] > dmn_val_dp[1]) monotonic_direction=decreasing; else monotonic_direction=increasing;
    } /* end else */

    if(monotonic_direction == increasing){
      min_idx=0L;
      max_idx=dmn_sz-1L;
    }else{
      min_idx=dmn_sz-1L;
      max_idx=0L;
    } /* end else */

    /* Determine min and max values of entire coordinate */
    dmn_min=dmn_val_dp[min_idx];
    dmn_max=dmn_val_dp[max_idx];
    
    /* Convert user-specified limits into double precision numeric values, or supply defaults */
    if(lmt.min_sng == NULL) lmt.min_val=dmn_val_dp[min_idx]; else lmt.min_val=strtod(lmt.min_sng,(char **)NULL);
    if(lmt.max_sng == NULL) lmt.max_val=dmn_val_dp[max_idx]; else lmt.max_val=strtod(lmt.max_sng,(char **)NULL);

    /* Warn when min_val > max_val (i.e., wrapped coordinate)*/
    if(lmt.min_val > lmt.max_val) (void)fprintf(stderr,"%s: WARNING Interpreting hyperslab specifications as wrapped coordinates [%s <= %g] and [%s >= %g]\n",prg_nm_get(),lmt.nm,lmt.max_val,lmt.nm,lmt.min_val);
    
    /* Fail when... */
    if(
       /* Following condition added 20000508, changes behavior of single point 
	  hyperslabs depending on whether hyperslab occurs in record dimension
	  for a multi-file operator.
	  Altered behavior of single point hyperslabs so that single point
	  hyperslabs in the record coordinate (i.e., -d time,1.0,1.0) may be
	  treated differently than single point hyperslabs in other
	  coordinates. Multifile operators will skip files if single point
	  hyperslabs in record coordinate lays outside record coordinate
	  range of file. For non-record coordinates (and for all operators
	  besides ncra and ncrcat on record coordinates), single point
	  hyperslabs will choose the closest value rather than skip the file
	  (I believe). This should be verified. */
       /* User specified single point, coordinate is not wrapped, and both extrema fall outside valid crd range */
       (rec_dmn_and_mlt_fl_opr && (lmt.min_val == lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
       /* User did not specify single point, coordinate is not wrapped, and either extrema falls outside valid crd range */
       ((lmt.min_val < lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
       /* User did not specify single point, coordinate is wrapped, and both extrema fall outside valid crd range */
       ((lmt.min_val > lmt.max_val) && ((lmt.min_val > dmn_max) && (lmt.max_val < dmn_min))) ||
       False){
      /* Allow for possibility that current file is superfluous */
      if(rec_dmn_and_mlt_fl_opr){
	flg_no_data=True;
	goto no_data;
      }else{
	(void)fprintf(stdout,"%s: ERROR User-specified coordinate value range %g <= %s <= %g does not fall within valid coordinate range %g <= %s <= %g\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val,dmn_min,lmt.nm,dmn_max);
	exit(EXIT_FAILURE);
      } /* end else */
    } /* end if */

    /* Armed with target coordinate minima and maxima, we are ready to bracket user-specified range */
    
    /* If min_sng or max_sng were omitted, use extrema */
    if(lmt.min_sng == NULL) lmt.min_idx=min_idx;
    if(lmt.max_sng == NULL) lmt.max_idx=max_idx;
    
    /* Single slice requires finding the closest coordinate */
    if(lmt.min_val == lmt.max_val){
      double dst_new;
      double dst_old;
      
      lmt.min_idx=0L;
      dst_old=fabs(lmt.min_val-dmn_val_dp[0]);
      for(tmp_idx=1L;tmp_idx<dmn_sz;tmp_idx++){
	if((dst_new=fabs(lmt.min_val-dmn_val_dp[tmp_idx])) < dst_old){
	  dst_old=dst_new;
	  lmt.min_idx=tmp_idx;
	} /* end if */
      } /* end loop over tmp_idx */
      lmt.max_idx=lmt.min_idx;
      
    }else{ /* min_val != max_val */
      
      /* Find brackets to specified extrema */
      /* Should no coordinate values match the given criteria, flag that index with a -1L
	 We defined the valid syntax such that a single half range with -1L is not an error
	 This causes "-d lon,100.0,-100.0" to select [-180.0] when lon=[-180.0,-90.0,0.0,90.0] because one
	 of the specified half-ranges is valid (there are coordinates < -100.0).
	 However, "-d lon,100.0,-200.0" should fail when lon=[-180.0,-90.0,0.0,90.0] because both 
	 of the specified half-ranges are invalid (no coordinate is > 100.0 or < -200.0).
	 The -1L flags are replaced with the correct indices (0L or dmn_sz-1L) following the search loop block
	 Overwriting the -1L flags with 0L or dmn_sz-1L later is more heuristic than setting them = 0L here,
	 since 0L is a valid search result.
       */
      if(monotonic_direction == increasing){
	if(lmt.min_sng != NULL){
	  /* Find index of smallest coordinate greater than min_val */
	  tmp_idx=0L;
	  while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx < dmn_sz)) tmp_idx++;
	  if(tmp_idx != dmn_sz) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
	} /* end if */
	if(lmt.max_sng != NULL){
	  /* Find index of largest coordinate less than max_val */
	  tmp_idx=dmn_sz-1L;
	  while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx > -1L)) tmp_idx--;
	  if(tmp_idx != -1L) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
	} /* end if */
	/* end if monotonic_direction == increasing */
      }else{ /* monotonic_direction == decreasing */
	if(lmt.min_sng != NULL){
	  /* Find index of smallest coordinate greater than min_val */
	  tmp_idx=dmn_sz-1L;
	  while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx > -1L)) tmp_idx--;
	  if(tmp_idx != -1L) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
	} /* end if */
	if(lmt.max_sng != NULL){
	  /* Find index of largest coordinate less than max_val */
	  tmp_idx=0L;
	  while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx < dmn_sz)) tmp_idx++;
	  if(tmp_idx != dmn_sz) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
	} /* end if */
      } /* end else monotonic_direction == decreasing */

      /* Case where both min_idx and max_idx = -1 was flagged as error above
	 Case of wrapped coordinate: Either, but not both, of min_idx or max_idx will be flagged with -1
	 See explanation above */
      if(lmt.min_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.min_idx=0L;
      if(lmt.max_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.max_idx=dmn_sz-1L;
    
    } /* end if min_val != max_val */
    
    /* User-specified ranges are now bracketed */
    
    /* Convert indices of minima and maxima to srt and end indices */
    if(monotonic_direction == increasing){
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{
      lmt.srt=lmt.max_idx;
      lmt.end=lmt.min_idx;
    }  /* end else */

    /* Un-typecast pointer to values after access */
    (void)cast_nctype_void(NC_DOUBLE,&dim.val);
  
    /* Free space allocated for dimension */
    dim.val.vp=nco_free(dim.val.vp);
    
  }else{ /* end if limit arguments were coordinate values */
    /* Convert limit strings to zero-based indicial offsets */
    
    /* Specifying stride alone, but not min or max, is legal, e.g., -d time,,,2
       Thus is_usr_spc_lmt may be True, even though one or both of min_sng, max_sng is NULL
       Furthermore, both min_sng and max_sng are artifically created by lmt_sct_mk()
       for record dimensions when the user does not explicitly specify limits.
       In this case, min_sng_and max_sng are non-NULL though no limits were specified
       In fact, min_sng and max_sng are set to the minimum and maximum string
       values of the first file processed.
       However, we can tell if these strings were artificially generated because 
       lmt_sct_mk() sets the is_usr_spc_lmt flag to False in such cases.
       Subsequent files may have different numbers of records, but lmt_sct_mk()
       is only called once.
       Thus we must update min_idx and max_idx here for each file
       This causes min_idx and max_idx to be out of sync with min_sng and max_sng, 
       which are only set in lmt_sct_mk() for the first file.
       In hindsight, artificially generating min_sng and max_sng may be a bad idea
    */
    /* Following logic is messy, but hard to simplify */
    if(lmt.min_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available--generate minimal dimension index */
      if(FORTRAN_STYLE) lmt.min_idx=1L; else lmt.min_idx=0L;
    }else{
      /* Use user-specified limit when available */
      lmt.min_idx=strtol(lmt.min_sng,(char **)NULL,10);
    } /* end if */
    if(lmt.max_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available--generate maximal dimension index */
      if(FORTRAN_STYLE) lmt.max_idx=dmn_sz; else lmt.max_idx=dmn_sz-1L;
    }else{
      /* Use user-specified limit when available */
      lmt.max_idx=strtol(lmt.max_sng,(char **)NULL,10);
    } /* end if */

    /* Adjust indices if FORTRAN style input was specified */
    if(FORTRAN_STYLE){
      lmt.min_idx--;
      lmt.max_idx--;
    } /* end if */
    
    /* Exit if requested indices are always invalid for all operators... */
    if(lmt.min_idx < 0 || lmt.max_idx < 0 || 
       /* ...or are invalid for non-record dimensions or single file operators */
       (!rec_dmn_and_mlt_fl_opr && lmt.min_idx >= dmn_sz)){
      (void)fprintf(stdout,"%s: ERROR User-specified dimension index range %li <= %s <= %li does not fall within valid dimension index range 0 <= %s <= %li\n",prg_nm_get(),lmt.min_idx,lmt.nm,lmt.max_idx,lmt.nm,dmn_sz-1L);
      (void)fprintf(stdout,"\n");
      exit(EXIT_FAILURE);
    } /* end if */
    
    /* Logic depends on whether this is record dimension in multi-file operator */
    if(!rec_dmn_and_mlt_fl_opr || !lmt.is_usr_spc_lmt){
      /* For non-record dimensions and for record dimensions where limit 
	 was automatically generated (to include whole file), starting
	 and ending indices are simply minimum and maximum indices already 
	 in structure */
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{

      /* Allow for possibility initial files are superfluous in multi-file hyperslab */
      if(rec_dmn_and_mlt_fl_opr && cnt_crr == 0L && lmt.min_idx >= dmn_sz+lmt.rec_skp_nsh_spf) flg_no_data=True;
      
      /* Initialize rec_skp_vld_prv to 0L on first call to lmt_evl() 
	 This is necessary due to intrinsic hysterisis of rec_skp_vld_prv
	 rec_skp_vld_prv is used only by multi-file operators
	 rec_skp_vld_prv counts records skipped at end of previous valid file
	 cnt_crr and rec_skp_nsh_spf are both zero only for first file
	 No records were skipped in previous files */
      if(cnt_crr == 0L && lmt.rec_skp_nsh_spf == 0L) lmt.rec_skp_vld_prv=0L;
      
      /* This if statement is required to avoid an ugly goto statment */
      if(!flg_no_data){
	
	/* For record dimensions with user-specified limit, allow for possibility 
	   that limit pertains to record dimension of a multi-file operator.
	   Then user-specified maximum index may exceed number of records in any one file
	   Thus lmt.srt does not necessarily equal lmt.min_idx and 
	   lmt.end does not necessarily equal lmt.max_idx */
	/* Stride is officially supported for ncks (all dimensions)
	   and for ncra and ncrcat (record dimension only) */
	if(lmt.srd != 1L && prg_id != ncks && !lmt.is_rec_dmn) (void)fprintf(stderr,"%s: WARNING Stride argument for a non-record dimension is only supported by ncks, use at your own risk...\n",prg_nm_get());
	
	/* Note that following block assumes flg_no_data is false
	   Harm would be done if this block were entered while flg_no_data were true
	   What would happen is lmt.end would be set, causing lmt.rec_skp_vld_prv 
	   to be incorrectly set at the end of this block 
	   Note, however, that flg_no_data may be set in two place within this block
	   These occurances are safe because they indicate all data have been read (all following files will be superfluous)
	   Therefore any hinky exit values will not affect future reads
	*/
	if(lmt.is_usr_spc_min && lmt.is_usr_spc_max){
	  /* cnt_rmn_ttl is determined only when both min and max are known */
	  cnt_rmn_ttl=-cnt_crr+1L+(lmt.max_idx-lmt.min_idx)/lmt.srd;
	  if(cnt_rmn_ttl == 0L) flg_no_data=True; /* Safe since all data have been read */
	  if(cnt_crr == 0L){
	    /* Start index is min_idx adjusted for any skipped initial superfluous files */
	    lmt.srt=lmt.min_idx-lmt.rec_skp_nsh_spf;
	    if(lmt.srd == 1L){
	      /* With unity stride, end index is lesser of number of remaining records to read and number of records in this file */
	      lmt.end=(lmt.max_idx < lmt.rec_skp_nsh_spf+dmn_sz) ? lmt.max_idx-lmt.rec_skp_nsh_spf : dmn_sz-1L;
	    }else{
	      cnt_rmn_crr=1L+(dmn_sz-1L-lmt.srt)/lmt.srd;
	      cnt_rmn_crr=(cnt_rmn_ttl < cnt_rmn_crr) ? cnt_rmn_ttl : cnt_rmn_crr;
	      lmt.end=lmt.srt+lmt.srd*(cnt_rmn_crr-1L);
	    } /* end else */
	  }else{
	    /* Records have been read from previous file(s) */
	    if(lmt.srd == 1L){
	      /* Start index is zero since contiguous records are requested */
	      lmt.srt=0L;
	      /* End index is lesser of number of records to read from all remaining files (including this one) and number of records in this file */
	      lmt.end=(cnt_rmn_ttl < dmn_sz) ? cnt_rmn_ttl-1L : dmn_sz-1L;
	    }else{
	      /* Start index will be non-zero if all previous file sizes (in records) were not evenly divisible by stride */
	      lmt.srt=lmt.srd-lmt.rec_skp_vld_prv-1L;
	      cnt_rmn_crr=1L+(dmn_sz-1L-lmt.srt)/lmt.srd;
	      cnt_rmn_crr=(cnt_rmn_ttl < cnt_rmn_crr) ? cnt_rmn_ttl : cnt_rmn_crr;
	      lmt.end=lmt.srt+lmt.srd*(cnt_rmn_crr-1L);
	    } /* end else */
	  } /* endif user-specified records have already been read */
	}else if(lmt.is_usr_spc_min){
	  /* If min was user specified but max was not, then we know which record to 
	     start with and we read every subsequent file */
	  if(cnt_crr == 0L){
	    /* Start index is min_idx adjusted for any skipped initial superfluous files */
	    lmt.srt=lmt.min_idx-lmt.rec_skp_nsh_spf;
	    if(lmt.srd == 1L){
	      lmt.end=dmn_sz-1L;
	    }else{
	      cnt_rmn_crr=1L+(dmn_sz-1L-lmt.srt)/lmt.srd;
	      lmt.end=lmt.srt+lmt.srd*(cnt_rmn_crr-1L);
	    } /* end else */
	  }else{
	    /* Records have been read from previous file(s) */
	    if(lmt.srd == 1L){
	      /* Start index is zero since contiguous records are requested */
	      lmt.srt=0L;
	      lmt.end=dmn_sz-1L;
	    }else{
	      /* Start index will be non-zero if all previous file sizes (in records) were not evenly divisible by stride */
	      lmt.srt=lmt.srd-lmt.rec_skp_vld_prv-1L;
	      cnt_rmn_crr=1L+(dmn_sz-1L-lmt.srt)/lmt.srd;
	      lmt.end=lmt.srt+lmt.srd*(cnt_rmn_crr-1L);
	    } /* end else */
	  } /* endif user-specified records have already been read */
	}else if(lmt.is_usr_spc_max){
	  /* If max was user specified but min was not, then we know which index to 
	     end with and we read record (modulo srd) until we get there */
	  if(cnt_crr == 0L){
	    /* Start index is min_idx = 0L for first file (no initial files are skipped in this case)*/
	    lmt.srt=lmt.min_idx;
	    if(lmt.srd == 1L){
	      /* With unity stride, end index is lesser of number of remaining records to read and number of records in this file */
	      lmt.end=(lmt.max_idx < dmn_sz) ? lmt.max_idx : dmn_sz-1L;
	    }else{
	      /* Record indices in the first file are global record indices */
	      lmt.end=(dmn_sz-(dmn_sz%lmt.srd));
	      lmt.end=(lmt.max_idx < lmt.end) ? lmt.max_idx : lmt.end;
	    } /* end else */
	  }else{
	    /* Records have been read from previous file(s) 
	       We must now account for "index shift" from previous files */
	    
	    long rec_idx_glb_off; /* Global index of first record in this file */
	    long max_idx_lcl; /* User-specified max index in "local" file */
	    
	    /* Global index of first record in this file */
	    rec_idx_glb_off=(cnt_crr-1L)*lmt.srd+lmt.rec_skp_vld_prv+1L;
	    /* Convert user-specified max index to "local" index in current file */
	    max_idx_lcl=lmt.max_idx-rec_idx_glb_off;
	    if(max_idx_lcl < 0) flg_no_data=True; /* Safe since all data have been read */
	    if(lmt.srd == 1L){
	      /* Start index is zero since contiguous records are requested */
	      lmt.srt=0L;
	      lmt.end=(max_idx_lcl < dmn_sz) ? max_idx_lcl : dmn_sz-1L;
	    }else{
	      /* Start index will be non-zero if all previous file sizes (in records) were not evenly divisible by stride */
	      lmt.srt=lmt.srd-lmt.rec_skp_vld_prv-1L;
	      lmt.end=lmt.srt;
	      while(lmt.end < dmn_sz-lmt.srd && lmt.end < max_idx_lcl-lmt.srd){
		lmt.end+=lmt.srd;
	      } /* end while */
	    } /* end else */
	  } /* endif user-specified records have already been read */
	}else if(!lmt.is_usr_spc_min && !lmt.is_usr_spc_max){
	  /* If stride was specified without min or max, then we read in all records
	     (modulo the stride) from every file */
	  if(cnt_crr == 0L){
	    /* Start index is min_idx = 0L for first file (no initial files are skipped in this case)*/
	    lmt.srt=lmt.min_idx;
	    if(lmt.srd == 1L){
	      lmt.end=dmn_sz-1L;
	    }else{
	      lmt.end=(dmn_sz > lmt.srd) ? dmn_sz-1L-(dmn_sz-1L)%lmt.srd : 0L;
	    } /* end else */
	  }else{
	    /* Records have been read from previous file(s) */
	    if(lmt.srd == 1L){
	      /* Start index is zero since contiguous records are requested */
	      lmt.srt=0L;
	      lmt.end=dmn_sz-1L;
	    }else{
	      /* Start index will be non-zero if all previous file sizes (in records) were not evenly divisible by stride */
	      lmt.srt=lmt.srd-lmt.rec_skp_vld_prv-1L;
	      cnt_rmn_crr=1L+(dmn_sz-1L-lmt.srt)/lmt.srd;
	      lmt.end=lmt.srt+lmt.srd*(cnt_rmn_crr-1L);
	    } /* end else */
	  } /* endif user-specified records have already been read */
	} /* end if srd but not min or max was user-specified */
	
	/* Compute diagnostic count for this file only */
	cnt_rmn_crr=1L+(lmt.end-lmt.srt)/lmt.srd;
	/* Save current rec_skp_vld_prv for diagnostics */
	rec_skp_vld_prv_dgn=lmt.rec_skp_vld_prv;
	/* rec_skp_vld_prv for next file is stride minus number of unused records
	   at end of this file (dmn_sz-1L-lmt.end) minus one */
	lmt.rec_skp_vld_prv=dmn_sz-1L-lmt.end;
	/*      assert(lmt.rec_skp_vld_prv >= 0);*/
	
      } /* endif !flg_no_data */
      
    } /* endif user-specified limits to record dimension */
    
  } /* end else limit arguments are hyperslab indices */
  
  /* Compute cnt from srt, end, and srd
     This is fine for multi-file record dimensions since those operators read in one
     record at a time and thus never actually use lmt.cnt for the record dimension.
   */
  if(lmt.srd == 1L){
    if(lmt.srt <= lmt.end) lmt.cnt=lmt.end-lmt.srt+1L; else lmt.cnt=dmn_sz-lmt.srt+lmt.end+1L;
  }else{
    if(lmt.srt <= lmt.end) lmt.cnt=1L+(lmt.end-lmt.srt)/lmt.srd; else lmt.cnt=1L+((dmn_sz-lmt.srt)+lmt.end)/lmt.srd;
  } /* end else */

  /* NB: Degenerate cases of WRP && SRD exist for which dmn_cnt_2 == 0
     This occurs when srd is large enough, or max_idx small enough, 
     such that no values are selected in the second read. 
     e.g., "-d lon,60,0,10" if sz(lon)=128 has dmn_cnt_2 == 0
     Since netCDF library reports an error reading and writing cnt=0 dimensions, a kludge is necessary
     Syntax ensures it is always the second read, not the first, which is obviated
     Therefore we convert these degenerate cases into non-wrapped coordinates to be processed by a single read 
     For these degenerate cases only, [srt,end] are not a permutation of [min_idx,max_idx]
     */
  if(
     (lmt.srd != 1L) && /* SRD */
     (lmt.srt > lmt.end) && /* WRP */
     (lmt.cnt == (1L+(dmn_sz-lmt.srt-1L)/lmt.srd)) && /* dmn_cnt_1 == cnt -> dmn_cnt_2 == 0 */
     True){
    long greatest_srd_multiplier_1st_hyp_slb; /* greatest integer m such that srt+m*srd < dmn_sz */
    long last_good_idx_1st_hyp_slb; /* C index of last valid member of 1st hyperslab (= srt+m*srd) */
    /* long left_over_idx_1st_hyp_slb;*/ /* # elements from first hyperslab to count towards current stride */
    long first_good_idx_2nd_hyp_slb; /* C index of first valid member of 2nd hyperslab, if any */

    /* NB: Perform these operations with integer arithmatic or else! */
    /* Wrapped dimensions with a stride may not start at idx 0 on second read */
    greatest_srd_multiplier_1st_hyp_slb=(dmn_sz-lmt.srt-1L)/lmt.srd;
    last_good_idx_1st_hyp_slb=lmt.srt+lmt.srd*greatest_srd_multiplier_1st_hyp_slb;
    /*    left_over_idx_1st_hyp_slb=dmn_sz-last_good_idx_1st_hyp_slb-1L;*/
    first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lmt.srd)%dmn_sz;

    /* Conditions causing dmn_cnt_2 == 0 */
    if(first_good_idx_2nd_hyp_slb > lmt.end) lmt.end=last_good_idx_1st_hyp_slb;
  } /* end if */

  /* Exit when valid bracketed range contains no coordinates */
  if(lmt.cnt == 0){
    if(lmt.lmt_typ == lmt_crd_val) (void)fprintf(stdout,"%s: ERROR Domain %g <= %s <= %g brackets no coordinate values.\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val); 
    if(lmt.lmt_typ == lmt_dmn_idx) (void)fprintf(stdout,"%s: ERROR Empty domain for %s\n",prg_nm_get(),lmt.nm); 
    exit(EXIT_FAILURE);
  } /* end if */
    
  /* Coordinate-valued limits that bracket no values in current file jump here with goto
     Index-valued limits with no values in current file flow here naturally */
 no_data: /* end goto */
  if(flg_no_data){
    /* File is superfluous (contributes no data) to specified hyperslab
       Set output parameters to well-defined state
       This state must not cause ncra or ncrcat to retrieve any data
       Since ncra and ncrcat use loops for the record dimension, this
       may be accomplished by returning loop control values that cause
       loop always to be skipped, never entered, e.g., lmt_rec.srt > lmt_rec.end */
    lmt.srt=-1L;
    lmt.end=lmt.srt-1L;
    lmt.cnt=-1L;
    /* Keep track of number of records skipped in initial superfluous files */
    if(cnt_crr == 0L) lmt.rec_skp_nsh_spf+=dmn_sz;
    /* Set variables to preserve utility of diagnostic routines at end of routine */
    cnt_rmn_crr=rec_skp_vld_prv_dgn=lmt.rec_skp_vld_prv=0L;
  } /* endif */
  
  /* Place contents of working structure in location of returned structure */
  *lmt_ptr=lmt;

  if(dbg_lvl_get() == 5){
    (void)fprintf(stderr,"Dimension hyperslabber lmt_evl() diagnostics:\n");
    (void)fprintf(stderr,"Dimension name = %s\n",lmt.nm);
    (void)fprintf(stderr,"Limit type is %s\n",(min_lmt_typ == lmt_crd_val) ? "coordinate value" : "zero-based dimension index");
    (void)fprintf(stderr,"Limit %s user-specified\n",(lmt.is_usr_spc_lmt) ? "is" : "is not");
    (void)fprintf(stderr,"Limit %s record dimension\n",(lmt.is_rec_dmn) ? "is" : "is not");
    (void)fprintf(stderr,"Current file %s specified hyperslab, data %s be read\n",(flg_no_data) ? "is superfluous to" : "is required by",(flg_no_data) ? "will not" : "will");
    if(rec_dmn_and_mlt_fl_opr) (void)fprintf(stderr,"Records skipped in initial superfluous files = %li \n",lmt.rec_skp_nsh_spf);
    if(rec_dmn_and_mlt_fl_opr) (void)fprintf(stderr,"Records read from previous files = %li\n",cnt_crr);
    if(cnt_rmn_ttl != -1L) (void)fprintf(stderr,"Total records to be read from this and all following files = %li\n",cnt_rmn_ttl);
    if(cnt_rmn_crr != -1L) (void)fprintf(stderr,"Records to be read from this file = %li\n",cnt_rmn_crr);
    if(rec_skp_vld_prv_dgn != -1L) (void)fprintf(stderr,"rec_skp_vld_prv_dgn (previous file, if any) = %li \n",rec_skp_vld_prv_dgn);
    if(rec_skp_vld_prv_dgn != -1L) (void)fprintf(stderr,"rec_skp_vld_prv (this file) = %li \n",lmt.rec_skp_vld_prv);
    (void)fprintf(stderr,"min_sng = %s\n",lmt.min_sng == NULL ? "NULL" : lmt.min_sng);
    (void)fprintf(stderr,"max_sng = %s\n",lmt.max_sng == NULL ? "NULL" : lmt.max_sng);
    (void)fprintf(stderr,"srd_sng = %s\n",lmt.srd_sng == NULL ? "NULL" : lmt.srd_sng);
    (void)fprintf(stderr,"min_val = %g\n",lmt.min_val);
    (void)fprintf(stderr,"max_val = %g\n",lmt.max_val);
    (void)fprintf(stderr,"min_idx = %li\n",lmt.min_idx);
    (void)fprintf(stderr,"max_idx = %li\n",lmt.max_idx);
    (void)fprintf(stderr,"srt = %li\n",lmt.srt);
    (void)fprintf(stderr,"end = %li\n",lmt.end);
    (void)fprintf(stderr,"cnt = %li\n",lmt.cnt);
    (void)fprintf(stderr,"srd = %li\n\n",lmt.srd);
  } /* end dbg */
  
  if(lmt.srt > lmt.end && !flg_no_data){
    if(prg_id != ncks) (void)fprintf(stderr,"WARNING: Possible instance of Schweitzer data hole requiring better diagnostics TODO #148\n");
    if(prg_id != ncks) (void)fprintf(stderr,"HINT: If operation fails, try hyperslabbing wrapped dimension using ncks instead of %s\n",prg_nm_get());
  } /* end dbg */

} /* end lmt_evl() */

void
rec_var_dbg(int nc_id,char *dbg_sng)
/* 
   int nc_id: I netCDF file ID
   char *dbg_sng: I debugging message to print
*/
{
  /* Purpose: Aid in debugging problems with record dimension */
  /* Usage: if(dbg_lvl == 73) rec_var_dbg(out_id,"After ncvarput()"); */
  int nbr_dmn_fl;
  int nbr_var_fl;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  long dmn_sz;

  (void)fprintf(stderr,"%s: DBG %s\n",prg_nm_get(),dbg_sng);
  (void)nco_inq(nc_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  if(rec_dmn_id == NCO_REC_DMN_UNDEFINED){
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, no record dimension\n",prg_nm_get(),nbr_dmn_fl,nbr_var_fl);
  }else{
    (void)nco_inq_dimlen(nc_id,rec_dmn_id,&dmn_sz);
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, record dimension size is %li\n",prg_nm_get(),nbr_dmn_fl,nbr_var_fl,dmn_sz);
  } /* end else */
  (void)fflush(stderr);
} /* end rec_var_dbg() */

void 
att_cpy(int in_id,int out_id,int var_in_id,int var_out_id)
/* 
   int in_id: I netCDF input-file ID
   int out_id: I netCDF output-file ID
   int var_in_id: I netCDF input-variable ID
   int var_out_id: I netCDF output-variable ID
*/
{
  /* Purpose: Copy attributes from input netCDF file to output netCDF file
     If var_in_id == NC_GLOBAL, then global attributes are copied. 
     Otherwise only indicated variable's attributes are copied */

  int idx;
  int nbr_att;
  int rcd; /* [enm] Return code */

  if(var_in_id == NC_GLOBAL){
    (void)nco_inq_natts(in_id,&nbr_att);
  }else{
    (void)nco_inq_varnatts(in_id,var_in_id,&nbr_att);
  } /* end else */
  
  for(idx=0;idx<nbr_att;idx++){
    char att_nm[NC_MAX_NAME];
    
    (void)nco_inq_attname(in_id,var_in_id,idx,att_nm);
    rcd=nco_inq_att_flg(out_id,var_out_id,att_nm,(nc_type *)NULL,(long *)NULL);
      
    /* Are we about to overwrite an existing attribute? */
    if(rcd == NC_NOERR){
      if(var_out_id == NC_GLOBAL){
	(void)fprintf(stderr,"%s: WARNING Overwriting global attribute %s\n",prg_nm_get(),att_nm);
      }else{
	char var_nm[NC_MAX_NAME];
	
	(void)nco_inq_varname(out_id,var_out_id,var_nm);
	(void)fprintf(stderr,"%s: WARNING Overwriting attribute %s for output variable %s\n",prg_nm_get(),att_nm,var_nm);
      } /* end else */
    } /* end if */

    (void)nco_copy_att(in_id,var_in_id,att_nm,out_id,var_out_id);

  } /* end loop over attributes */
} /* end att_cpy() */

var_sct *
var_fll(int nc_id,int var_id,char *var_nm,dmn_sct **dim,int nbr_dim)
/* 
   int nc_id: I netCDF file ID
   int var_id: I variable ID
   char *var_nm: I variable name
   dmn_sct **dim: I list of pointers to dimension structures
   int nbr_dim: I number of dimensions in list
   var_sct *var_fll(): O variable structure
 */
{
  /* Purpose: nco_malloc() and return a completed var_sct */
  char dmn_nm[NC_MAX_NAME];

  int dmn_idx;
  int idx;
  int rec_dmn_id;

  var_sct *var;

  /* Get record dimension ID */
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  
  /* Allocate space for variable structure */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */

  /* Fill in known fields */
  var->nm=var_nm;
  var->id=var_id;
  var->nc_id=nc_id;

  /* Get type and number of dimensions and attributes for variable */
  (void)nco_inq_var(var->nc_id,var->id,(char *)NULL,&var->typ_dsk,&var->nbr_dim,(int *)NULL,&var->nbr_att);

  /* Allocate space for dimension information */
  if(var->nbr_dim > 0) var->dim=(dmn_sct **)nco_malloc(var->nbr_dim*sizeof(dmn_sct *)); else var->dim=(dmn_sct **)NULL;
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(var->nc_id,var->id,var->dmn_id);
  
  /* Type in memory begins as same type as on disk */
  var->type=var->typ_dsk; /* Type of variable in RAM */
  /* Type of packed data on disk */
  var->typ_pck=var->type;  /* Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */

  /* Refresh number of attributes and missing value attribute, if any */
  var->has_mss_val=mss_val_get(var->nc_id,var);

  /* Size defaults to 1 in var_dfl_set(), but set to 1 here for safety */
  var->sz=1L; 
  for(idx=0;idx<var->nbr_dim;idx++){
    (void)nco_inq_dimname(nc_id,var->dmn_id[idx],dmn_nm);
    /* Search input dimension list for matching name */
    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
      if(!strcmp(dmn_nm,dim[dmn_idx]->nm)) break;
    } /* end for */
    if(dmn_idx == nbr_dim){
      (void)fprintf(stdout,"%s: ERROR dimension %s is not in input dimension list\n",prg_nm_get(),dmn_nm);
      exit(EXIT_FAILURE);
    } /* end if */

    /* fxm: hmb, what is this for? */
    /* re-define dim_id so that if dim is the dimension list from the output file
       then we get the correct dim_id. Should not affect normal running of the
       routine as most the time, dim is the dimension from the input file */
    var->dmn_id[idx]=dim[dmn_idx]->id;

    var->dim[idx]=dim[dmn_idx];
    var->cnt[idx]=dim[dmn_idx]->cnt;
    var->srt[idx]=dim[dmn_idx]->srt;
    var->end[idx]=dim[dmn_idx]->end;
    var->srd[idx]=dim[dmn_idx]->srd;

    if(var->dmn_id[idx] == rec_dmn_id) var->is_rec_var=True; else var->sz_rec*=var->cnt[idx];

    if(var->dim[idx]->is_crd_dmn && var->id == var->dim[idx]->cid){
      var->is_crd_var=True;
      var->cid=var->dmn_id[idx];
    } /* end if */

    /* NB: This assumes default var->sz begins as 1 */
    var->sz*=var->cnt[idx];
  } /* end loop over dim */

  if(dbg_lvl_get() == 3){
    if(prg_get() == ncra || prg_get() == ncea){
      /* Packing/Unpacking */
      (void)pck_dsk_inq(nc_id,var);
    } /* endif arithemetic operator with packing capability */
  } /* endif debug */

  return var;
} /* end var_fll() */

void
var_refresh(int nc_id,var_sct *var)
/* 
   int nc_id: I netCDF input-file ID
   var_sct *var: I/O variable structure
 */
{
  /* Threads: Routine contains thread-unsafe calls protected by critical regions */
  /* Purpose: Update variable ID, number of dimensions, and missing_value attribute for given variable
     var_refresh() is called in file loop in multi-file operators because each new file may have 
     different variable ID and missing_value for same variable.
     This is necessary, for example, if a computer model runs for awhile on one machine, e.g., SGI,
     and then the run is restarted on another, e.g., Cray. 
     Since internal floating point representations differ betwee these architectures, the missing_value
     representation may differ. 
     Variable IDs may changes whenever someone fiddles with original model output in some files, 
     but not others, and then processes all files in a batch.
     NCO is one of the only tool I know of which makes all of this transparent to the user
     Thus this capability is very important to maintain
   */

  /* Refresh variable ID */
  var->nc_id=nc_id;

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
  { /* begin OpenMP critical */
    (void)nco_inq_varid(var->nc_id,var->nm,&var->id);
    
    /* fxm: Not sure if/why it is necessary refresh number of dimensions...but it should not hurt */
    /* Refresh number of dimensions in variable */
    (void)nco_inq_varndims(var->nc_id,var->id,&var->nbr_dim);
    
    /* Refresh number of attributes and missing value attribute, if any */
    var->has_mss_val=mss_val_get(var->nc_id,var);
  } /* end OpenMP critical */

} /* end var_refresh() */

int
mss_val_get(int nc_id,var_sct *var)
/* 
   int nc_id: I netCDF input-file ID
   var_sct *var: I/O variable structure
   int mss_val_get(): O flag whether variable has missing value on output or not
 */
{
  /* Purpose: Update number of attributes and missing_value attribute of variable
     No matter what type of missing_value is as stored on disk, this routine
     ensures that the copy in mss_val in the var_sct is stored in type as
     the host variable.
     Routine does not allow output missing_value to have more than one element */

  /* has_mss_val is defined typed as int not bool because it is often sent to Fortran routines */

  char att_nm[NC_MAX_NAME];
  
  long att_sz;
  int idx;
  
  long att_len;

  nc_type att_typ;

  ptr_unn mss_tmp;
  
  /* Refresh netCDF "mss_val" attribute for this variable */
  var->has_mss_val=False;
  var->mss_val.vp=nco_free(var->mss_val.vp);

  /* Refresh number of attributes for variable */
  (void)nco_inq_varnatts(var->nc_id,var->id,&var->nbr_att);

  for(idx=0;idx<var->nbr_att;idx++){
    (void)nco_inq_attname(var->nc_id,var->id,idx,att_nm);
    if((int)strcasecmp(att_nm,"missing_value") != 0) continue;
    (void)nco_inq_att(var->nc_id,var->id,att_nm,&att_typ,&att_sz);
    if(att_sz != 1 && att_typ != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s has %li elements and so will not be used\n",prg_nm_get(),att_nm,var->nm,att_sz);
      continue;
    } /* end if */
    /* if(att_typ != var->type) (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s will be typecast from %s to %s for arithmetic purposes\n",prg_nm_get(),att_nm,var->nm,nco_typ_sng(att_typ),nco_typ_sng(var->type)); */
    /* If we got this far then try to retrieve attribute and make sure it conforms to variable's type */
    var->has_mss_val=True;
    /* Oddly, ARM uses NC_CHAR for type of missing_value, so we must make allowances for this */
    att_len=att_sz*nco_typ_lng(att_typ);
    mss_tmp.vp=(void *)nco_malloc(att_len);
    (void)nco_get_att(var->nc_id,var->id,att_nm,mss_tmp.vp,att_typ);
    if(att_typ == NC_CHAR){
      /* NUL-terminate missing value string */
      if(mss_tmp.cp[att_len-1] != '\0'){
	att_len++;
	mss_tmp.vp=(void *)nco_realloc(mss_tmp.vp,att_len);
	mss_tmp.cp[att_len-1]='\0';
	/* Un-typecast pointer to values after access */
	(void)cast_nctype_void(att_typ,&mss_tmp);
      } /* end if */
    } /* end if */
    
    /* Ensure mss_val in memory is stored as same type as variable */
    var->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    (void)val_conform_type(att_typ,mss_tmp,var->type,var->mss_val);

    /* Release temporary memory */
    mss_tmp.vp=nco_free(mss_tmp.vp);
    break;
  } /* end loop over att */

  return var->has_mss_val;

} /* end mss_val_get() */

dmn_sct *
dmn_fll(int nc_id,int dmn_id,char *dmn_nm)
/* 
   int nc_id: I netCDF input-file ID
   int dmn_id: I dimension ID
   char *dmn_nm: I dimension name
   dmn_sct *dmn_fll(): pointer to output dimension structure
 */
{
  /* Routine to nco_malloc() and return a completed dmn_sct */

  dmn_sct *dim;
  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id;
  
  dim=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
  
  dim->nm=dmn_nm;
  dim->id=dmn_id;
  dim->nc_id=nc_id;
  dim->xrf=NULL;
  dim->val.vp=NULL;

  dim->is_crd_dmn=False;
  (void)nco_inq_dimlen(dim->nc_id,dmn_id,&dim->sz);
  
  /* Get the record dimension ID */
  (void)nco_inq(dim->nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  if(dim->id == rec_dmn_id){
    dim->is_rec_dmn=True;
  }else{
    dim->is_rec_dmn=False;
  } /* end if */
   
  rcd=nco_inq_varid_flg(dim->nc_id,dmn_nm,&dim->cid);
  if(rcd == NC_NOERR){
    dim->is_crd_dmn=True;
    /* What type is coordinate? */
    (void)nco_inq_vartype(dim->nc_id,dim->cid,&dim->type);
  } /* end if */
  
  dim->cnt=dim->sz;
  dim->srt=0L;
  dim->end=dim->sz-1L;
  dim->srd=1L;
  
  return dim;
} /* end dmn_fll() */

void
dmn_lmt_mrg(dmn_sct **dim,int nbr_dim,lmt_sct *lmt,int lmt_nbr)
/* 
   dmn_sct **dim: I list of pointers to dimension structures
   int nbr_dim: I number of dimension structures in structure list
   lmt_sct *lmt: I structure from lmt_evl() holding dimension limit info.
   int lmt_nbr: I number of dimensions with user-specified limits
 */
{
  /* Routine to merge the limit structure information into dimension structures */

  int idx;
  int lmt_idx;

  for(idx=0;idx<nbr_dim;idx++){

    /* Decide whether this dimension has any user-specified limits */
    for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
      if(lmt[lmt_idx].id == dim[idx]->id){
	dim[idx]->cnt=lmt[lmt_idx].cnt;
	dim[idx]->srt=lmt[lmt_idx].srt;
	dim[idx]->end=lmt[lmt_idx].end;
	dim[idx]->srd=lmt[lmt_idx].srd;
	break;
      } /* end if */
    } /* end loop over lmt_idx */
  } /* end loop over dim */
} /* end dmn_lmt_mrg() */

nm_id_sct *
var_lst_mk(int nc_id,int nbr_var,char **var_lst_in,bool PROCESS_ALL_COORDINATES,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   int nbr_var: I total number of variables in input file
   char **var_lst_in: user specified list of variable names
   bool PROCESS_ALL_COORDINATES: I whether to process all coordinates
   int *nbr_xtr: I/O number of variables in current extraction list
   nm_id_sct var_lst_mk(): O extraction list
 */
{
  bool err_flg=False;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int idx;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */

  if(*nbr_xtr > 0){
    /* If user named variables with -v option then check validity of user's list and find IDs */
    xtr_lst=(nm_id_sct *)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
     
    for(idx=0;idx<*nbr_xtr;idx++){
      xtr_lst[idx].nm=var_lst_in[idx];
      rcd=nco_inq_varid_flg(nc_id,xtr_lst[idx].nm,&xtr_lst[idx].id);
      if(rcd != NC_NOERR){
	(void)fprintf(stdout,"%s: ERROR var_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),xtr_lst[idx].nm);
	err_flg=True;
      } /* endif */
    } /* end loop over idx */
    
    if(err_flg) exit(EXIT_FAILURE);
  }else if(!PROCESS_ALL_COORDINATES){
    /* If the user did not specify variables with the -v option,
       and the user did not request automatic processing of all coords,
       then extract all variables in file. In this case
       we can assume variable IDs range from 0..nbr_var-1. */
    char var_nm[NC_MAX_NAME];
    
    *nbr_xtr=nbr_var;
    xtr_lst=(nm_id_sct *)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
    for(idx=0;idx<nbr_var;idx++){
      /* Get name of each variable. */
      (void)nco_inq_varname(nc_id,idx,var_nm);
      xtr_lst[idx].nm=(char *)strdup(var_nm);
      xtr_lst[idx].id=idx;
    } /* end loop over idx */
  } /* end else */

  return xtr_lst;

} /* end var_lst_mk() */

nm_id_sct *
var_lst_xcl(int nc_id,int nbr_var,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   int nbr_var: I total number of variables in input file
   nm_id_sct *xtr_lst: I/O current extraction list (destroyed)
   int *nbr_xtr: I/O number of variables in current extraction list
   nm_id_sct var_lst_xcl(): O extraction list
 */
{
  /* The user wants to extract all variables except the ones
     currently in the list. Since it's hard to edit the existing
     list, copy the existing extract list into the exclude list,
     and construct a new list extract list from scratch. */

  char var_nm[NC_MAX_NAME];

  int idx;
  int lst_idx;
  int nbr_xcl;

  nm_id_sct *xcl_lst;
  
  /* Turn the extract list into the exclude list and reallocate the extract list  */
  nbr_xcl=*nbr_xtr;
  *nbr_xtr=0;
  xcl_lst=(nm_id_sct *)nco_malloc(nbr_xcl*sizeof(nm_id_sct));
  (void)memcpy((void *)xcl_lst,(void *)xtr_lst,nbr_xcl*sizeof(nm_id_sct));
  xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(nbr_var-nbr_xcl)*sizeof(nm_id_sct));
  
  for(idx=0;idx<nbr_var;idx++){
    /* Get name and ID of variable */
    (void)nco_inq_varname(nc_id,idx,var_nm);
    for(lst_idx=0;lst_idx<nbr_xcl;lst_idx++){
      if(idx == xcl_lst[lst_idx].id) break;
    } /* end loop over lst_idx */
    /* If variable was not found in the exclusion list then 
       add it to the new list. */
    if(lst_idx == nbr_xcl){
      xtr_lst[*nbr_xtr].nm=(char *)strdup(var_nm);
      xtr_lst[*nbr_xtr].id=idx;
      ++*nbr_xtr;
    } /* end if */
  } /* end loop over idx */
  
  /* Free memory for names in exclude list before losing pointers to names */
  /* NB: cannot free memory if list points to names in argv[] */
  /* for(idx=0;idx<nbr_xcl;idx++) xcl_lst[idx].nm=nco_free(xcl_lst[idx].nm);*/
  xcl_lst=nco_free(xcl_lst);
  
  return xtr_lst;
} /* end var_lst_xcl() */

nm_id_sct *
var_lst_add_crd(int nc_id,int nbr_var,int nbr_dim,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   int nbr_var: I total number of variables in input file
   int nbr_dim: I total number of dimensions in input file
   nm_id_sct *xtr_lst: current extraction list (destroyed)
   int *nbr_xtr: I/O number of variables in current extraction list
   nm_id_sct var_lst_add_crd(): O extraction list
 */
{
  /* Find all coordinates (dimensions which are also variables) and
     add them to the list if they are not already there. */
  
  char crd_nm[NC_MAX_NAME];

  int crd_id;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

  for(idx=0;idx<nbr_dim;idx++){
    (void)nco_inq_dimname(nc_id,idx,crd_nm);
    
    /* Does variable of same name exist in input file? */
    rcd=nco_inq_varid_flg(nc_id,crd_nm,&crd_id);
    if(rcd == NC_NOERR){
      /* Dimension is coordinate. Is it already on list? */
      int lst_idx;
      
      for(lst_idx=0;lst_idx<*nbr_xtr;lst_idx++){
	if(crd_id == xtr_lst[lst_idx].id) break;
      } /* end loop over lst_idx */
      if(lst_idx == *nbr_xtr){
	/* Coordinate is not already on the list, put it there. */
	if(*nbr_xtr == 0) xtr_lst=(nm_id_sct *)nco_malloc((*nbr_xtr+1)*sizeof(nm_id_sct)); else xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	/* According to the man page for realloc(), this should work even when xtr_lst == NULL */
/*	xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));*/
	xtr_lst[*nbr_xtr].nm=(char *)strdup(crd_nm);
	xtr_lst[*nbr_xtr].id=crd_id;
	(*nbr_xtr)++;
      } /* end if */
    } /* end if */
  } /* end loop over idx */
  
  return xtr_lst;
  
} /* end var_lst_add_crd() */

lmt_sct
lmt_sct_mk(int nc_id,int dmn_id,lmt_sct *lmt,int lmt_nbr,bool FORTRAN_STYLE)
/* 
   int nc_id: I [idx] netCDF file ID
   int dmn_id: I [idx] ID of dimension for which to create a limit structure
   lmt_sct *lmt: I [sct] Array of limits structures from lmt_evl()
   int lmt_nbr: I [nbr] Number of limit structures in limit structure array
   bool FORTRAN_STYLE: I [flg] switch to determine syntactical interpretation of dimensional indices
   lmt_sct lmt_sct_mk(): O [sct] Limit structure for dimension
 */
{
  /* Purpose: Create stand-alone limit structure just for given dimension 
     lmt_sct_mk() is called by ncra() to generate limit structure for record dimension */
  
  int idx;
  int rcd; /* [rcd] Return code */
  
  lmt_sct lmt_dim;

  /* Initialize defaults to False, override later if warranted */
  lmt_dim.is_usr_spc_lmt=False; /* True if any part of limit is user-specified, else False */
  lmt_dim.is_usr_spc_max=False; /* True if user-specified, else False */
  lmt_dim.is_usr_spc_min=False; /* True if user-specified, else False */
  /* rec_skp_nsh_spf is used for record dimension in multi-file operators */
  lmt_dim.rec_skp_nsh_spf=0L; /* Number of records skipped in initial superfluous files */

  for(idx=0;idx<lmt_nbr;idx++){
    /* Copy user-specified limits, if any */
    if(lmt[idx].id == dmn_id){
      lmt_dim.is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
      if(lmt[idx].max_sng == NULL){
	lmt_dim.max_sng=NULL;
      }else{
	lmt_dim.max_sng=(char *)strdup(lmt[idx].max_sng);
	lmt_dim.is_usr_spc_max=True; /* True if user-specified, else False */
      } /* end if */
      if(lmt[idx].min_sng == NULL){
	lmt_dim.min_sng=NULL;
      }else{
	lmt_dim.min_sng=(char *)strdup(lmt[idx].min_sng);
	lmt_dim.is_usr_spc_min=True; /* True if user-specified, else False */
      } /* end if */
      if(lmt[idx].srd_sng != NULL) lmt_dim.srd_sng=(char *)strdup(lmt[idx].srd_sng); else lmt_dim.srd_sng=NULL;
      lmt_dim.nm=(char *)strdup(lmt[idx].nm);
      break;
    } /* end if */
  } /* end loop over idx */

  /* If this limit was not user-specified, then ... */
  if(idx == lmt_nbr){
    /* Create default limits to look as though user-specified them */
    char dmn_nm[NC_MAX_NAME];
    long cnt;
    int max_sng_sz;
    
    /* Fill in limits with default parsing information */
    rcd=nco_inq_dim_flg(nc_id,dmn_id,dmn_nm,&cnt);

    if(rcd == NC_EBADDIM){
      (void)fprintf(stdout,"%s: ERROR attempting to find non-existent dimension with id = %d in lmt_sct_mk()\n",prg_nm_get(),dmn_id);
      exit(EXIT_FAILURE);
    } /* end if */
		
    lmt_dim.nm=(char *)strdup(dmn_nm);
    lmt_dim.srd_sng=NULL;
    /* Generate min and max strings to look as if user had specified them
       Adjust accordingly if FORTRAN_STYLE was requested for other dimensions
       These sizes will later be decremented in lmt_evl() where all information
       is converted internally to C based indexing representation.
       Ultimately this problem arises because I want lmt_evl() to think the
       user always did specify this dimension's hyperslab.
       Otherwise, problems arise when FORTRAN_STYLE is specified by the user 
       along with explicit hypersalbs for some dimensions excluding the record
       dimension.
       Then, when lmt_sct_mk() creates the record dimension structure, it must
       be created consistently with the FORTRAN_STYLE flag for the other dimensions.
       In order to do that, I must fill in the max_sng, min_sng, and srd_sng
       arguments with strings as if they had been read from the keyboard.
       Another solution would be to add a flag to lmt_sct indicating whether this
       limit struct had been automatically generated and then act accordingly.
    */
    /* Decrement cnt to C index value if necessary */
    if(!FORTRAN_STYLE) cnt--; 
    if(cnt < 0L){
      (void)fprintf(stdout,"%s: cnt < 0 in lmt_sct_mk()\n",prg_nm_get());
      exit(EXIT_FAILURE);
    } /* end if */
    /* cnt < 10 covers negative numbers and SIGFPE from log10(cnt==0) 
       Adding 1 is required for cnt=10,100,1000... */
    if(cnt < 10L) max_sng_sz=1; else max_sng_sz=1+(int)ceil(log10((double)cnt));
    /* Add one for NUL terminator */
    lmt_dim.max_sng=(char *)nco_malloc(sizeof(char)*(max_sng_sz+1));
    (void)sprintf(lmt_dim.max_sng,"%ld",cnt);
    if(FORTRAN_STYLE){
      lmt_dim.min_sng=(char *)strdup("1");
    }else{
      lmt_dim.min_sng=(char *)strdup("0");
    } /* end else */
  } /* end if user did not explicity specify limits for this dimension */
  
  return lmt_dim;
  
} /* end lmt_sct_mk() */

nm_id_sct *
var_lst_crd_xcl(int nc_id,int dmn_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   int dmn_id: I dimension ID of the coordinate to eliminate from extraction list
   nm_id_sct *xtr_lst: current extraction list (destroyed)
   int *nbr_xtr: I/O number of variables in current extraction list
   nm_id_sct var_lst_crd_xcl(): O extraction list
 */
{
  /* The following code modifies extraction list to exclude the coordinate, 
     if any, associated with the given dimension ID */
  
  char crd_nm[NC_MAX_NAME];

  int idx;
  int crd_id=-1;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  /* What is variable ID of record coordinate, if any? */
  (void)nco_inq_dimname(nc_id,dmn_id,crd_nm);
   
  rcd=nco_inq_varid_flg(nc_id,crd_nm,&crd_id);
  if(rcd == NC_NOERR){
    /* Is coordinate on extraction list? */
    for(idx=0;idx<*nbr_xtr;idx++){
      if(xtr_lst[idx].id == crd_id) break;
    } /* end loop over idx */
    if(idx != *nbr_xtr){
      nm_id_sct *var_lst_tmp;
      
      var_lst_tmp=(nm_id_sct *)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
      /* Copy the extract list to the temporary extract list and reallocate the extract list */
      (void)memcpy((void *)var_lst_tmp,(void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      (*nbr_xtr)--;
      xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      /* Collapse the temporary extract list into the permanent list by copying 
	 all but the coordinate. NB: the ordering of the list is conserved. */
      (void)memcpy((void *)xtr_lst,(void *)var_lst_tmp,idx*sizeof(nm_id_sct));
      (void)memcpy((void *)(xtr_lst+idx),(void *)(var_lst_tmp+idx+1),(*nbr_xtr-idx)*sizeof(nm_id_sct));
      /* Free the memory for coordinate name in the extract list before losing the pointer */
      var_lst_tmp[idx].nm=nco_free(var_lst_tmp[idx].nm);
      var_lst_tmp=nco_free(var_lst_tmp);
    } /* end if */
  } /* end if */
  
  return xtr_lst;
  
} /* end var_lst_crd_xcl() */

nm_id_sct *
var_lst_ass_crd_add(int nc_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   nm_id_sct *xtr_lst: I/O current extraction list (destroyed)
   int *nbr_xtr: I/O number of variables in current extraction list
   nm_id_sct var_lst_ass_crd_add(): O extraction list
 */
{
  /* Makes sure all coordinates associated with each of variables
     to be extracted is also on the list. This helps with making concise
     nco_malloc() calls down the road. */

  char dmn_nm[NC_MAX_NAME];

  int crd_id;
  int dmn_id[NC_MAX_DIMS];
  int idx_dmn;
  int idx_var_dim;
  int idx_var;
  int nbr_dim;
  int nbr_var_dim;
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dim,(int *)NULL,(int *)NULL,(int *)NULL);

  /* ...for each dimension in input file... */
  for(idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
    /* ...see if it is a coordinate dimension... */
    (void)nco_inq_dimname(nc_id,idx_dmn,dmn_nm);
     
    rcd=nco_inq_varid_flg(nc_id,dmn_nm,&crd_id);
    if(rcd == NC_NOERR){
      /* Is this coordinate already on extraction list? */
      for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	if(crd_id == xtr_lst[idx_var].id) break;
      } /* end loop over idx_var */
      if(idx_var == *nbr_xtr){
	/* ...the coordinate is not on the list, is it associated with any variables?... */
	for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	  /* Get number of dimensions and dimension IDs for variable. */
	  (void)nco_inq_var(nc_id,xtr_lst[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dmn_id,(int *)NULL);
	  for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
	    if(idx_dmn == dmn_id[idx_var_dim]) break;
	  } /* end loop over idx_var_dim */
	  if(idx_var_dim != nbr_var_dim){
	    /* Add the coordinate to the list */
	    xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	    xtr_lst[*nbr_xtr].nm=(char *)strdup(dmn_nm);
	    xtr_lst[*nbr_xtr].id=crd_id;
	    (*nbr_xtr)++;
	    break;
	  } /* end if */
	} /* end loop over idx_var */
      } /* end if coordinate was not already on the list */
    } /* end if dimension is a coordinate */
  } /* end loop over idx_dmn */
  
  return xtr_lst;
  
} /* end var_lst_ass_crd_add() */

nm_id_sct * /* O [sct] Sorted output list */
lst_heapsort /* [fnc] Heapsort input lists numerically or alphabetically */
(nm_id_sct *lst, /* I/O [sct] Current list (destroyed) */
 int nbr_lst, /* I [nbr] number of members in list */
 bool ALPHABETIZE_OUTPUT) /* I [flg] Alphabetize extraction list */
{
  /* Purpose: Sort extraction lists numerically or alphabetically */
  int *srt_idx; /* List to store sorted key map */
  int idx; /* Counting index */
  nm_id_sct *lst_tmp; /* Temporary copy of original extraction list */
  
  srt_idx=(int *)nco_malloc(nbr_lst*sizeof(int));
  lst_tmp=(nm_id_sct *)nco_malloc(nbr_lst*sizeof(nm_id_sct));
  (void)memcpy((void *)lst_tmp,(void *)lst,nbr_lst*sizeof(nm_id_sct));
  
  /* indexx() and relative assume "one-based" arrays 
     Use pointer arithmetic to spoof zero-based arrays, i.e.,
     xtr_nm[0] in calling routine becomes xtr_nm[1] in sorting routine  */
  if(ALPHABETIZE_OUTPUT){
    /* Alphabetize list by variable name
       This produces easy-to-read screen output with ncks */
    char **xtr_nm;
    xtr_nm=(char **)nco_malloc(nbr_lst*sizeof(char *));
    for(idx=0;idx<nbr_lst;idx++) xtr_nm[idx]=lst[idx].nm;
    (void)index_alpha(nbr_lst,xtr_nm-1,srt_idx-1);
    xtr_nm=nco_free(xtr_nm);
  }else{
    /* Heapsort the list by variable ID 
       This theoretically allows the fastest I/O when creating output file */
    int *xtr_id;
    xtr_id=(int *)nco_malloc(nbr_lst*sizeof(int));
    for(idx=0;idx<nbr_lst;idx++) xtr_id[idx]=lst[idx].id;
    (void)indexx(nbr_lst,xtr_id-1,srt_idx-1);
    xtr_id=nco_free(xtr_id);
  } /* end else */

  /* indexx and relatives employ "one-based" arrays 
     Thus min(srt_idx) == 1 and max(srt_idx) == nbr_lst */
  for(idx=0;idx<nbr_lst;idx++){
    lst[idx].id=lst_tmp[srt_idx[idx]-1].id;
    lst[idx].nm=lst_tmp[srt_idx[idx]-1].nm;
  } /* end loop over idx */
  lst_tmp=nco_free(lst_tmp);
  srt_idx=nco_free(srt_idx);
  
  return lst;
  
} /* end lst_heapsort() */

char *
fl_out_open(char *fl_out,bool FORCE_APPEND,bool FORCE_OVERWRITE,int *out_id)
/* 
   char *fl_out: I Name of file to open
   bool FORCE_APPEND: I Flag for appending to existing file, if any
   bool FORCE_OVERWRITE: I Flag for overwriting existing file, if any
   int *out_id: O File ID
   char *fl_out_open(): O Name of temporary file actually opened
 */
{
  /* Open output file subject to availability and user input 
     In accord with netCDF philosophy a temporary file (based on fl_out and process ID)
     is actually opened, so that errors can not infect intended output file */

  char *fl_out_tmp;
  char tmp_sng_1[]="pid"; /* Extra string appended to temporary filenames */
  char tmp_sng_2[]="tmp"; /* Extra string appended to temporary filenames */
  char *pid_sng; /* String containing decimal representation of PID */

  int rcd; /* [rcd] Return code */

  long fl_out_tmp_lng; /* [nbr] Length of temporary file name */
  long pid_sng_lng; /* [nbr] Theoretical length of decimal representation of this PID */
  long pid_sng_lng_max; /* [nbr] Maximum length of decimal representation of any PID */

  pid_t pid; /* Process ID */

  struct stat stat_sct;
  
  if(FORCE_OVERWRITE && FORCE_APPEND){
    (void)fprintf(stdout,"%s: ERROR FORCE_OVERWRITE and FORCE_APPEND are both set\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* end if */

  /* Generate unique temporary file name
     System routines tempnam(), tmpname(), mktemp() perform a similar function, but are OS dependent
     Maximum length of PID depends on pid_t
     Until about 1995 most OSs set pid_t = short = 16 or 32 bits
     Now some OSs have /usr/include/sys/types.h set pid_t = long = 32 or 64 bits
     20000126: Use sizeof(pid_t) rather than hardcoded size to fix longstanding bug on SGIs
  */
  /* Maximum length of decimal representation of PID is number of bits in PID times log10(2) */
  pid_sng_lng_max=(long)ceil(8*sizeof(pid_t)*log10(2.0));
  pid_sng=(char *)nco_malloc((pid_sng_lng_max+1)*sizeof(char));
  pid=getpid();
  (void)sprintf(pid_sng,"%ld",(long)pid);
  /* Theoretical length of decimal representation of PID is 1+ceil(log10(PID)) where the 1 is required iff PID is an exact power of 10 */
  pid_sng_lng=1L+(long)ceil(log10((double)pid));
  /* NCO temporary file name is user-specified file name + "." + tmp_sng_1 + PID + "." + prg_nm + "." + tmp_sng_2 + NUL */
  fl_out_tmp_lng=strlen(fl_out)+1L+strlen(tmp_sng_1)+strlen(pid_sng)+1L+strlen(prg_nm_get())+1L+strlen(tmp_sng_2)+1L;
  fl_out_tmp=(char *)nco_malloc(fl_out_tmp_lng*sizeof(char));
  (void)sprintf(fl_out_tmp,"%s.%s%s.%s.%s",fl_out,tmp_sng_1,pid_sng,prg_nm_get(),tmp_sng_2);
  if(dbg_lvl_get() > 5) (void)fprintf(stdout,"%s: fl_out_open() reports sizeof(pid_t) = %d bytes, pid = %ld, pid_sng_lng = %ld bytes, strlen(pid_sng) = %ld bytes, fl_out_tmp_lng = %ld bytes, strlen(fl_out_tmp) = %ld, fl_out_tmp = %s\n",prg_nm_get(),(int)sizeof(pid_t),(long)pid,pid_sng_lng,(long)strlen(pid_sng),fl_out_tmp_lng,(long)strlen(fl_out_tmp),fl_out_tmp);
  rcd=stat(fl_out_tmp,&stat_sct);

  /* Free temporary memory */
  pid_sng=nco_free(pid_sng);

  if(dbg_lvl_get() == 8){
  /* Use builtin system routines to generate temporary filename
     This allows file to be built in fast directory like /tmp rather than local
     directory which could be a slow, NFS-mounted directories like /fs/cgd

     There are many options:
     tmpnam() uses P_tmpdir, does not allow specfication of drc
     tempnam(const char *drc, const char *pfx) uses writable $TMPDIR, else drc, else P_tmpdir, else /tmp and prefixes returned name with up to five characters from pfx, if supplied
     mkstemp(char *tpl) generates a filename and creates file in mode 0600

     Many sysadmins do not make /tmp large enough for huge temporary data files 
     tempnam(), however, allows $TMPDIR or drc to be set to override /tmp
     We tried tempnam() but as of 20001010 GCC 2.96 this causes a warning: "the use of `tempnam' is dangerous, better use `mkstemp'"
   */
    int fl_out_hnd; /* Temporary file */
    char *fl_out_tmp_sys; /* System-generated unique temporary filename */
    fl_out_tmp_sys=(char *)nco_malloc((strlen(fl_out)+7)*sizeof(char));
    fl_out_tmp_sys[0]='\0'; /* NUL-terminate */
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,fl_out);
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,"XXXXXX");
    fl_out_hnd=mkstemp(fl_out_tmp_sys);
    fl_out_hnd=fl_out_hnd; /* Removes compiler warning on SGI */
    if(dbg_lvl_get() > 2) (void)fprintf(stdout,"%s: fl_out_open() reports strlen(fl_out_tmp_sys) = %ld, fl_out_tmp_sys = %s, \n",prg_nm_get(),(long)strlen(fl_out_tmp_sys),fl_out_tmp_sys);
  } /* endif dbg */

  /* If temporary file already exists, prompt user to remove temporary files and exit */
  if(rcd != -1){
    (void)fprintf(stdout,"%s: ERROR temporary file %s already exists, remove and try again\n",prg_nm_get(),fl_out_tmp);
    exit(EXIT_FAILURE);
  } /* end if */

  if(FORCE_OVERWRITE){
    rcd=nco_create(fl_out_tmp,NC_CLOBBER,out_id);
    /*    rcd=nc_create(fl_out_tmp,NC_CLOBBER|NC_SHARE,out_id);*/
    return fl_out_tmp;
  } /* end if */

  if(False){
    if(prg_get() == ncrename){
      /* ncrename is different because a single filename is allowed without question */
      /* Incur expense of copying current file to temporary file */
      (void)fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id); 
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */
  } /* end if false */

  rcd=stat(fl_out,&stat_sct);
  
  /* If permanent file already exists, query user whether to overwrite, append, or exit */
  if(rcd != -1){
    char usr_reply='z';
    short nbr_itr=0;
    
    if(FORCE_APPEND){
      /* Incur expense of copying current file to temporary file */
      (void)fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id); 
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */

    while(usr_reply != 'o' && usr_reply != 'a' && usr_reply != 'e'){
      nbr_itr++;
      if(nbr_itr > 10){
	(void)fprintf(stdout,"\n%s: ERROR %hd failed attempts to obtain valid interactive input. Assuming non-interactive shell and exiting.\n",prg_nm_get(),nbr_itr-1);
	exit(EXIT_FAILURE);
      } /* end if */
      if(nbr_itr > 1) (void)fprintf(stdout,"%s: ERROR Invalid response.\n",prg_nm_get());
      (void)fprintf(stdout,"%s: %s exists---`o'verwrite, `a'ppend/replace, or `e'xit (o/a/e)? ",prg_nm_get(),fl_out);
      (void)fflush(stdout);
      usr_reply=(char)fgetc(stdin);
      /* Allow one carriage return per response free of charge */
      if(usr_reply == '\n') usr_reply=(char)fgetc(stdin);
      (void)fflush(stdin);
    } /* end while */
    
    switch(usr_reply){
    case 'e':
      exit(EXIT_SUCCESS);
      break;
    case 'o':
      rcd=nco_create(fl_out_tmp,NC_CLOBBER,out_id);
      /*    rcd=nc_create(fl_out_tmp,NC_CLOBBER|NC_SHARE,out_id);*/
      break;
    case 'a':
      /* Incur expense of copying current file to temporary file */
      (void)fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id); 
      (void)nco_redef(*out_id);
      break;
    } /* end switch */
    
  }else{ /* output file does not yet already exist */
    rcd=nco_create(fl_out_tmp,NC_NOCLOBBER,out_id);
    /*    rcd=nc_create(fl_out_tmp,NC_NOCLOBBER|NC_SHARE,out_id);*/
  } /* end if output file does not already exist */
  
  return fl_out_tmp;
  
} /* end fl_out_open() */

void
fl_out_cls(char *fl_out,char *fl_out_tmp,int nc_id)
/* 
   char *fl_out: I name of permanent output file
   char *fl_out_tmp: I name of temporary output file to close and move to permanent output file
   int nc_id: I file ID of fl_out_tmp
 */
{
  /* Routine to close the temporary output file and move it to the permanent output file */
  int rcd; /* [rcd] Return code */

  rcd=nco_close(nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR fl_out_cls() is unable to ncclose() file %s\n",prg_nm_get(),fl_out_tmp);
    exit(EXIT_FAILURE); 
  } /* end if */
  
  (void)fl_mv(fl_out_tmp,fl_out);

} /* end fl_out_cls() */

void
fl_cmp_err_chk()
/* 
   fl_cmp_err_chk():
*/
{
  /* Purpose: Perform error checking on file */
} /* end fl_cmp_err_chk() */

void
var_val_cpy(int in_id,int out_id,var_sct **var,int nbr_var)
/* 
   int in_id: I netCDF file ID
   int out_id: I netCDF output-file ID
   var_sct **var: I list of pointers to variable structures
   int nbr_var: I number of structures in variable structure list
   var_val_cpy():
*/
{
  /* Copy variable data for every variable in the input variable structure list
     from input file to output file */

  int idx;

  for(idx=0;idx<nbr_var;idx++){
    var[idx]->xrf->val.vp=var[idx]->val.vp=(void *)nco_malloc(var[idx]->sz*nco_typ_lng(var[idx]->type));
    if(var[idx]->nbr_dim==0){
      nco_get_var1(in_id,var[idx]->id,var[idx]->srt,var[idx]->val.vp,var[idx]->type);
      nco_put_var1(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->val.vp,var[idx]->type);
    }else{ /* end if variable is a scalar */
      nco_get_vara(in_id,var[idx]->id,var[idx]->srt,var[idx]->cnt,var[idx]->val.vp,var[idx]->type);
      nco_put_vara(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->val.vp,var[idx]->type);
    } /* end if variable is an array */
    var[idx]->val.vp=nco_free(var[idx]->val.vp); var[idx]->xrf->val.vp=var[idx]->val.vp=NULL;
  } /* end loop over idx */

} /* end var_val_cpy() */

void
dmn_dfn(char *fl_nm,int nc_id,dmn_sct **dim,int nbr_dim)
/* 
   char *fl_nm: I name of output file
   int nc_id: I netCDF output-file ID
   dmn_sct **dim: I list of pointers to dimension structures to be defined in output file
   int nbr_dim: I number of dimension structures in structure list
*/
{
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

  for(idx=0;idx<nbr_dim;idx++){

    /* Has dimension already been defined? */
    rcd=nco_inq_dimid_flg(nc_id,dim[idx]->nm,&dim[idx]->id);

    /* If dimension has not been defined yet, define it */
    if(rcd != NC_NOERR){
      if(dim[idx]->is_rec_dmn){
	(void)nco_def_dim(nc_id,dim[idx]->nm,NC_UNLIMITED,&(dim[idx]->id));
      }else{
	(void)nco_def_dim(nc_id,dim[idx]->nm,dim[idx]->cnt,&(dim[idx]->id));
      } /* end else */
    }else{
      (void)fprintf(stderr,"%s: WARNING dimension \"%s\" is already defined in %s\n",prg_nm_get(),dim[idx]->nm,fl_nm);
    } /* end if */
  } /* end loop over idx */
  
} /* end dmn_dfn() */

void 
var_copy(nc_type var_typ,long sz,ptr_unn op1,ptr_unn op2)
{
  /* Purpose: Copy hyperslab variables of type var_typ from op1 to op2
     Assumes memory area in op2 has already been malloc()'d */
  (void)memcpy((void *)(op2.vp),(void *)(op1.vp),sz*nco_typ_lng(var_typ));
} /* end var_copy() */

void
var_dfn(int in_id,char *fl_out,int out_id,var_sct **var,int nbr_var,dmn_sct **dmn_ncl,int nbr_dmn_ncl)
/* 
   int in_id: I netCDF input-file ID
   char *fl_out: I name of output file
   int out_id: I netCDF output-file ID
   var_sct **var: I list of pointers to variable structures to be defined in output file
   int nbr_var: I number of variable structures in structure list
   dmn_sct **dmn_ncl: I list of pointers to dimension structures allowed in output file
   int nbr_dmn_ncl: I number of dimension structures in structure list
*/
{
  /* Define variables in output file, and copy their attributes */

  /* This function is unusual (for me) in that dimension arguments are only intended
     to be used by certain programs, those that alter the rank of input variables. If a
     program does not alter the rank (dimensionality) of input variables then it should
     call this function with a NULL dimension list. Otherwise, this routine attempts
     to define variable correctly in output file (allowing variable to be
     defined with only those dimensions that are in dimension inclusion list) 
     without altering variable structures. 

     The other unusual thing about this function is that it is intended to be called with var_prc_out
     So the local variable var usually refers to var_prc_out in the calling function 
     That is why many of the names look reversed in this function, and why xrf is frequently used */

  int idx_dmn;
  int dmn_id_vec[NC_MAX_DIMS];
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  for(idx=0;idx<nbr_var;idx++){

    /* Is requested variable already in output file? */
    rcd=nco_inq_varid_flg(out_id,var[idx]->nm,&var[idx]->id);

    /* If variable has not been defined, define it */
    if(rcd != NC_NOERR){
      
      /* TODO #116: There is a problem here in that var_out[idx]->nbr_dim is never explicitly set to the actual number of output dimensions, rather, it is simply copied from var[idx]. When var_out[idx] actually has 0 dimensions, the loop executes once anyway, and an erroneous index into the dmn_out[idx] array is attempted. Fix is to explicitly define var_out[idx]->nbr_dim. Until this is done, anything in ncwa that explicitly depends on var_out[idx]->nbr_dim is suspect. The real problem is that, in ncwa, var_avg() expects var_out[idx]->nbr_dim to contain the input, rather than output, number of dimensions. The routine, var_dfn() was designed to call the simple branch when dmn_ncl == 0, i.e., for operators besides ncwa. However, when ncwa averages all dimensions in output file, nbr_dmn_ncl == 0 so the wrong branch would get called unless we specifically use this branch whenever ncwa is calling. */
      if(dmn_ncl != NULL || prg_get() == ncwa){
	int nbr_var_dim=0;
	int idx_ncl;

	/* Rank of output variable may have to be reduced */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  /* Is dimension allowed in output file? */
	  for(idx_ncl=0;idx_ncl<nbr_dmn_ncl;idx_ncl++){
	    if(var[idx]->xrf->dim[idx_dmn]->id == dmn_ncl[idx_ncl]->xrf->id) break;
	  } /* end loop over idx_ncl */
	  if(idx_ncl != nbr_dmn_ncl) dmn_id_vec[nbr_var_dim++]=var[idx]->dim[idx_dmn]->id;
	} /* end loop over idx_dmn */
	(void)nco_def_var(out_id,var[idx]->nm,var[idx]->type,nbr_var_dim,dmn_id_vec,&var[idx]->id);

      }else{ /* Straightforward definition */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  dmn_id_vec[idx_dmn]=var[idx]->dim[idx_dmn]->id;
	} /* end loop over idx_dmn */
	(void)nco_def_var(out_id,var[idx]->nm,var[idx]->type,var[idx]->nbr_dim,dmn_id_vec,&var[idx]->id);
      } /* end else */
      
      /* endif if variable had not yet been defined in output file */
    }else{
      /* Variable is already in output file---use existing definition
	 This branch is executed, e.g., by operators in append mode */
      (void)fprintf(stderr,"%s: WARNING Using existing definition of variable \"%s\" in %s\n",prg_nm_get(),var[idx]->nm,fl_out);
    } /* end if */

    /* Always copy all attributes of a variable except in cases where packing/unpacking is involved

       0. Variable is unpacked on input, unpacked on output
       --> Copy all attributes
       1. Variable is packed on input, is not altered, and remains packed on output
       --> Copy all attributes
       2. Variable is packed on input, is unpacked for some reason, and will be unpacked on output
       --> Copy all attributes except scale_factor and add_offset
       3. Variable is packed on input, is unpacked for some reason, and will be packed on output (possibly with new packing attributes)
       --> Copy all attributes, but scale_factor and add_offset must be overwritten later with new values
       4. Variable is not packed on input, packing is performed, and output is packed
       --> Copy all attributes, define scale_factor and add_offset now, write their values later
    */

#undef FALSE
#ifdef FALSE
    /* Set nco_pck_typ based on program */
    switch(prg){
    case ncea:
      nco_pck_typ=nco_pck_all_new_att;
      break;
    case ncap:
    case ncra:
    case ncdiff:
    case ncflint:
    case ncwa:
    case ncrcat:
    case ncecat:
    default:
      nco_pck_typ=nco_pck_nil;
      break;
    } /* end switch */

    switch(nco_pck_typ){
    case nco_pck_all_xst_att:
    case nco_pck_all_new_att:
      break;
    case nco_pck_xst_xst_att:
    case nco_pck_xst_new_att:
      break;
    case nco_pck_upk:
      break;
    default:
      break;
    } /* end switch */
#endif /* not FALSE */

    /* var refers to output variable structure, var->xrf refers to input variable structure */
    (void)att_cpy(in_id,out_id,var[idx]->xrf->id,var[idx]->id);
  } /* end loop over idx */
  
} /* end var_dfn() */

void 
hst_att_cat(int out_id,char *hst_sng)
/* 
   int out_id: I netCDF output-file ID
   char *hst_sng: I string to add to history attribute
*/
{
/* Purpose: Add command line and date stamp to existing history attribute, if any,
   and write them to specified output file */

#define TIME_STAMP_SNG_LNG 25 

  char att_nm[NC_MAX_NAME];
  char *ctime_sng;
  char *history_crr=NULL;
  char *history_new;
  char time_stamp_sng[TIME_STAMP_SNG_LNG];
  
  int idx;
  int nbr_glb_att;

  long att_sz=0;

  nc_type att_typ;
  
  time_t clock;

  /* Create timestamp string */
  clock=time((time_t *)NULL);
  ctime_sng=ctime(&clock);
  /* NUL-terminate time_stamp_sng */
  time_stamp_sng[TIME_STAMP_SNG_LNG-1]='\0';
  /* Get rid of carriage return in ctime_sng */
  (void)strncpy(time_stamp_sng,ctime_sng,TIME_STAMP_SNG_LNG-1);

  /* Get number of global attributes in file */
  (void)nco_inq(out_id,(int *)NULL,(int *)NULL,&nbr_glb_att,(int *)NULL);

  for(idx=0;idx<nbr_glb_att;idx++){
    (void)nco_inq_attname(out_id,NC_GLOBAL,idx,att_nm);
    if(strcasecmp(att_nm,"history") == 0) break;
  } /* end loop over att */

  /* Fill in history string */
  if(idx == nbr_glb_att){
    /* history global attribute does not yet exist */

    /* Add 3 for formatting characters */
    history_new=(char *)nco_malloc((strlen(hst_sng)+strlen(time_stamp_sng)+3)*sizeof(char));
    (void)sprintf(history_new,"%s: %s",time_stamp_sng,hst_sng);
  }else{ 
    /* history global attribute currently exists */
  
    /* NB: ncattinq(), unlike strlen(), counts terminating NUL for stored NC_CHAR arrays */
    (void)nco_inq_att(out_id,NC_GLOBAL,"history",&att_typ,&att_sz);
    if(att_typ != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" global attribute is type %s, not %s. Therefore current command line will not be appended to %s in output file.\n",prg_nm_get(),att_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),att_nm);
      return;
    } /* end if */

    /* Allocate and NUL-terminate space for current history attribute
       If history attribute is of size zero then ensure strlen(history_crr) = 0 */
    history_crr=(char *)nco_malloc((att_sz+1)*sizeof(char));
    history_crr[att_sz]='\0';
    if(att_sz > 0) (void)nco_get_att(out_id,NC_GLOBAL,"history",(void *)history_crr,NC_CHAR);

    /* Add 4 for formatting characters */
    history_new=(char *)nco_malloc((strlen(history_crr)+strlen(hst_sng)+strlen(time_stamp_sng)+4)*sizeof(char));
    (void)sprintf(history_new,"%s: %s\n%s",time_stamp_sng,hst_sng,history_crr);
  } /* endif history global attribute currently exists */

  (void)nco_put_att(out_id,NC_GLOBAL,"history",NC_CHAR,strlen(history_new)+1,(void *)history_new);

  history_crr=nco_free(history_crr);
  history_new=nco_free(history_new);

} /* end hst_att_cat() */

nm_id_sct *
dmn_lst_ass_var(int nc_id,nm_id_sct *var,int nbr_var,int *nbr_dim)
/* 
   int nc_id: I netCDF input-file ID
   nm_id_sct *var: I variable list
   int nbr_var: I number of variables in list
   int *nbr_dim: O number of dimensions associated with input variable list
   nm_id_sct *dmn_lst_ass_var(): O list of dimensions associated with input variable list
 */
{
  /* Purpose: Create a list of all dimensions associated with input variable list */

  bool dmn_has_been_placed_on_list;

  char dmn_nm[NC_MAX_NAME];

  int dmn_id[NC_MAX_DIMS];
  int idx_dmn_in;
  int idx_var;
  int idx_var_dim;
  int idx_dmn_lst;
  int nbr_dmn_in;
  int nbr_var_dim;
  
  nm_id_sct *dim;

  *nbr_dim=0;

  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dmn_in,(int *)NULL,(int *)NULL,(int *)NULL);

  /* Number of input dimensions is upper bound on number of output dimensions */
  dim=(nm_id_sct *)nco_malloc(nbr_dmn_in*sizeof(nm_id_sct));
  
  /* ...For each dimension in file... */
  for(idx_dmn_in=0;idx_dmn_in<nbr_dmn_in;idx_dmn_in++){
    /* ...begin search for dimension in dimension list by... */
    dmn_has_been_placed_on_list=False;
    /* ...looking through the set of output variables... */
    for(idx_var=0;idx_var<nbr_var;idx_var++){
      /* ...and searching each dimension of each output variable... */
      (void)nco_inq_var(nc_id,var[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dmn_id,(int *)NULL);
      for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
	/* ...until output variable is found which contains input dimension... */
	if(idx_dmn_in == dmn_id[idx_var_dim]){
	  /* ...then search each member of output dimension list... */
	  for(idx_dmn_lst=0;idx_dmn_lst<*nbr_dim;idx_dmn_lst++){
	    /* ...until input dimension is found... */
	    if(idx_dmn_in == dim[idx_dmn_lst].id) break; /* ...then search no further... */
	  } /* end loop over idx_dmn_lst */
	  /* ...and if dimension was not found on output dimension list... */
	  if(idx_dmn_lst == *nbr_dim){
	    /* ...then add dimension to output dimension list... */
	    (void)nco_inq_dimname(nc_id,idx_dmn_in,dmn_nm);
	    dim[*nbr_dim].id=idx_dmn_in;
	    dim[*nbr_dim].nm=(char *)strdup(dmn_nm);
	    (*nbr_dim)++;
	  } /* end if dimension was not found in current output dimension list */
	  /* ...call off the dogs for this input dimension... */
	  dmn_has_been_placed_on_list=True;
	} /* end if input dimension belongs to this output variable */
	if(dmn_has_been_placed_on_list) break; /* break out of idx_var_dim to idx_var */
      } /* end loop over idx_var_dim */
      if(dmn_has_been_placed_on_list) break; /* break out of idx_var to idx_dmn_in */
    } /* end loop over idx_var */
  } /* end loop over idx_dmn_in */
  
  /* We now have final list of dimensions to extract. Phew. */
  
  /* Free unused space in output dimension list */
  dim=(nm_id_sct *)nco_realloc((void *)dim,*nbr_dim*sizeof(nm_id_sct));
  
  return dim;

} /* end dmn_lst_ass_var() */

void
var_srt_zero(var_sct **var,int nbr_var)
/* 
   var_sct **var: I list of pointers to variable structures whose srt elements will be zeroed
   int nbr_var: I number of structures in variable structure list
 */
{
  /* Purpose: Point srt element of variable structure to array of zeroes */

  int idx;
  int idx_dmn;

  for(idx=0;idx<nbr_var;idx++)
    for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++)
      var[idx]->srt[idx_dmn]=0L;

} /* end var_srt_zero() */

var_sct *
var_dpl(var_sct *var)
/* 
   var_sct *var: I variable structure to duplicate
   var_sct *var_dpl(): O copy of input variable structure
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: nco_malloc() and return duplicate of input var_sct */

  var_sct *var_dpl;

  var_dpl=(var_sct *)nco_malloc(sizeof(var_sct));

  (void)memcpy((void *)var_dpl,(void *)var,sizeof(var_sct));

  /* Copy all dyamically allocated arrays currently defined in original */
  if(var->val.vp != NULL){
    if((var_dpl->val.vp=(void *)malloc(var_dpl->sz*nco_typ_lng(var_dpl->type))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes for value buffer for variable %s in var_dpl()\n",prg_nm_get(),var_dpl->sz,nco_typ_lng(var_dpl->type),var_dpl->nm);
      exit(EXIT_FAILURE); 
    } /* end if */
    (void)memcpy((void *)(var_dpl->val.vp),(void *)(var->val.vp),var_dpl->sz*nco_typ_lng(var_dpl->type));
  } /* end if */
  if(var->mss_val.vp != NULL){
    var_dpl->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_dpl->type));
    (void)memcpy((void *)(var_dpl->mss_val.vp),(void *)(var->mss_val.vp),nco_typ_lng(var_dpl->type));
  } /* end if */
  if(var->tally != NULL){
    if((var_dpl->tally=(long *)malloc(var_dpl->sz*sizeof(long))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in var_dpl()\n",prg_nm_get(),var_dpl->sz,(long)sizeof(long),var_dpl->nm);
      exit(EXIT_FAILURE); 
    } /* end if */
    (void)memcpy((void *)(var_dpl->tally),(void *)(var->tally),var_dpl->sz*sizeof(long));
  } /* end if */
  if(var->dim != NULL){
    var_dpl->dim=(dmn_sct **)nco_malloc(var_dpl->nbr_dim*sizeof(dmn_sct *));
    (void)memcpy((void *)(var_dpl->dim),(void *)(var->dim),var_dpl->nbr_dim*sizeof(var->dim[0]));
  } /* end if */
  if(var->dmn_id != NULL){
    var_dpl->dmn_id=(int *)nco_malloc(var_dpl->nbr_dim*sizeof(int));
    (void)memcpy((void *)(var_dpl->dmn_id),(void *)(var->dmn_id),var_dpl->nbr_dim*sizeof(var->dmn_id[0]));
  } /* end if */
  if(var->cnt != NULL){
    var_dpl->cnt=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->cnt),(void *)(var->cnt),var_dpl->nbr_dim*sizeof(var->cnt[0]));
  } /* end if */
  if(var->srt != NULL){
    var_dpl->srt=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->srt),(void *)(var->srt),var_dpl->nbr_dim*sizeof(var->srt[0]));
  } /* end if */
  if(var->end != NULL){
    var_dpl->end=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->end),(void *)(var->end),var_dpl->nbr_dim*sizeof(var->end[0]));
  } /* end if */
  if(var->srd != NULL){
    var_dpl->srd=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->srd),(void *)(var->srd),var_dpl->nbr_dim*sizeof(var->srd[0]));
  } /* end if */
  if(var->scl_fct.vp != NULL){
    var_dpl->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var_dpl->typ_upk));
    (void)memcpy((void *)(var_dpl->scl_fct.vp),(void *)(var->scl_fct.vp),nco_typ_lng(var_dpl->typ_upk));
  } /* end if */
  if(var->add_fst.vp != NULL){
    var_dpl->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var_dpl->typ_upk));
    (void)memcpy((void *)(var_dpl->add_fst.vp),(void *)(var->add_fst.vp),nco_typ_lng(var_dpl->typ_upk));
  } /* end if */

  return var_dpl;

} /* end var_dpl() */

dmn_sct *
dmn_dpl(dmn_sct *dim)
/* 
   dmn_sct *dim: I dimension structure to duplicate
   dmn_sct *dmn_dpl(): O copy of input dimension structure
 */
{
  /* Purpose: nco_malloc() and return duplicate of input dmn_sct */

  dmn_sct *dmn_dpl;

  dmn_dpl=(dmn_sct *)nco_malloc(sizeof(dmn_sct));

  (void)memcpy((void *)dmn_dpl,(void *)dim,sizeof(dmn_sct));

  return dmn_dpl;

} /* end dmn_dpl() */

void
var_get(int nc_id,var_sct *var)
/* 
   int nc_id: I netCDF file ID
   var_sct *var: I pointer to variable structure
 */
{
  /* Threads: Routine contains thread-unsafe calls protected by critical regions */
  /* Purpose: Allocate and retrieve given variable hyperslab from disk into memory
     If variable is packed on disk then inquire about scale_factor and add_offset */

  if((var->val.vp=(void *)malloc(var->sz*nco_typ_lng(var->typ_dsk))) == NULL){
    (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes in var_get()\n",prg_nm_get(),var->sz,nco_typ_lng(var->type));
    exit(EXIT_FAILURE); 
  } /* end if */

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
  { /* begin OpenMP critical */
    if(var->sz > 1){
      (void)nco_get_vara(nc_id,var->id,var->srt,var->cnt,var->val.vp,var->typ_dsk);
    }else{
      (void)nco_get_var1(nc_id,var->id,var->srt,var->val.vp,var->typ_dsk);
    } /* end else */
  } /* end OpenMP critical */

  /* Type in memory is now same as type on disk */
  var->type=var->typ_dsk; /* Type of variable in RAM */
  
  if(dbg_lvl_get() == 3){
    if(prg_get() == ncra || prg_get() == ncea){
      /* Packing/Unpacking */
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
      if(var->pck_dsk) var=var_upk(var);
    } /* endif arithemetic operator with packing capability */
  } /* endif debug */

} /* end var_get() */

var_sct * /* O [sct] wgt_out that conforms to var  */
var_conform_dim(var_sct *var,var_sct *wgt,var_sct *wgt_crr,bool MUST_CONFORM,bool *DO_CONFORM)
     /* fxm: TODO #114. Fix var_conform_dim() so returned weight always has same size tally array as template variable */
/*  
   var_sct *var: I [ptr] Pointer to variable structure to serve as template
   var_sct *wgt: I [ptr] Pointer to variable structure to make conform to var
   var_sct *wgt_crr: I/O [ptr] pointer to existing conforming variable structure, if any (destroyed when does not conform to var)
   bool MUST_CONFORM; I [flg] Must wgt and var must conform?
   bool *DO_CONFORM; O [flg] Did wgt and var conform?
   wgt_out var_sct *var_conform_dim(): O [sct] Pointer to conforming variable structure
*/
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Stretch second variable to match dimensions of first variable
     Dimensions in var which are not in wgt will be present in wgt_out, with values
     replicated from existing dimensions in wgt.
     By default, wgt's dimensions must be a subset of var's dimensions (MUST_CONFORM=true)
     If it is permissible for wgt not to conform to var then set MUST_CONFORM=false before calling this routine
     In this case when wgt and var do not conform then then var_conform_dim sets *DO_CONFORM=False and returns a copy of var with all values set to 1.0
     The calling procedure can then decide what to do with the output
     MUST_CONFORM is True for ncdiff: Variables of like name to be differenced must be same rank
     MUST_CONFORM is False false for ncap, ncflint, ncwa: Variables to be averaged may may be */

  /* There are many inelegant ways to accomplish this (without using C++): */  

  /* Perhaps most efficient method to accomplish this in general case is to expand 
     weight array until it is same size as variable array, and then multiply these
     together element-by-element in highly vectorized loop, preferably in Fortran. 
     To enhance speed, (enlarged) weight-values array could be static, only remade
     when dimensions of incoming variables change. */

  /* Another method for the general case, though an expensive one, is to use C to 
     figure out the multidimensional indices into the one dimensional hyperslab, 
     a la ncks. Knowing these indices, one can loop over the one-dimensional array
     element by element, choosing the appropriate index into the weight array from 
     those same multidimensional indices. This method can also create a static weight-value
     array that is only destroyed when an incoming variable changes dimensions from the
     previous variable. */

  /* Yet another method, which is not completely general, but which may be good enough for
     governement work, is to create fortran subroutines which expect variables of a given
     number of dimensions as input. Creating these functions for up to five dimensions would
     satisfy all foreseeable situations. The branch as to which function to call would be
     done based on number of dimensions, here in the C code. C++ or Fortran9x overloading
     could accomplish some of this interface more elegantly. */

  /* An (untested) simplification to some of these methods is to copy the 1-D array
     value pointer of variable and cast it to an N-D array pointer
     Then C could handle indexing 
     This method easily produce working, but non-general code
     Implementation would require ugly branches or hard-to-understand recursive function calls */
  
  /* Routine assumes weight will never have more dimensions than variable
     (otherwise which hyperslab of weight to use would be ill-defined). 
     However, weight may (and often will) have fewer dimensions than variable */

  bool CONFORMABLE=False; /* Whether wgt can be made to conform to var */
  bool USE_DUMMY_WGT=False; /* Whether to fool NCO into thinking wgt conforms to var */

  int idx; /* Counting index */
  int idx_dmn; /* Dimension index */
  int wgt_var_dmn_shr_nbr=0; /* Number of dimensions shared by wgt and var */

  var_sct *wgt_out=NULL;

  /* Initialize flag to false. Overwrite by true after successful conformance */
  *DO_CONFORM=False;
  
  /* Does current weight (wgt_crr) conform to variable's dimensions? */
  if(wgt_crr != NULL){
    /* Test rank first because wgt_crr because of 19960218 bug (invalid dmn_id in old wgt_crr leads to match) */
    if(var->nbr_dim == wgt_crr->nbr_dim){
      /* Test whether all wgt and var dimensions match in sequence */
      for(idx=0;idx<var->nbr_dim;idx++){
	/*	if(wgt_crr->dmn_id[idx] != var->dmn_id[idx]) break;*/
	if(!strstr(wgt_crr->dim[idx]->nm,var->dim[idx]->nm)) break;
      } /* end loop over dimensions */
      if(idx == var->nbr_dim) *DO_CONFORM=True;
    } /* end if */
    if(*DO_CONFORM){
      wgt_out=wgt_crr;
    }else{
      wgt_crr=var_free(wgt_crr);
      wgt_out=NULL;
    } /* end if */
  } /* end if */

  /* Does original weight (wgt) conform to variable's dimensions? */
  if(wgt_out == NULL){
    if(var->nbr_dim > 0){
      /* Test that all dimensions in wgt appear in var */
      for(idx=0;idx<wgt->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(strstr(wgt->dim[idx]->nm,var->dim[idx_dmn]->nm)){
	    wgt_var_dmn_shr_nbr++; /* wgt and var share this dimension */
	    break;
	  } /* endif */
        } /* end loop over var dimensions */
      } /* end loop over wgt dimensions */
      /* Decide whether wgt and var dimensions conform, are mutually exclusive, or are partially exclusive (an error) */ 
      if(wgt_var_dmn_shr_nbr == wgt->nbr_dim){
	/* wgt and var conform */
	CONFORMABLE=True;
      }else if(wgt_var_dmn_shr_nbr == 0){
	/* Dimensions in wgt and var are mutually exclusive */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",prg_nm_get(),wgt->nm,var->nm);
	  exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"\n%s: DEBUG %s and template %s share no dimensions: Not broadcasting %s to %s\n",prg_nm_get(),wgt->nm,var->nm,wgt->nm,var->nm);
	  USE_DUMMY_WGT=True;
	} /* endif */
      }else if(wgt->nbr_dim > var->nbr_dim){
	/* wgt is larger rank than var---no possibility of conforming */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %s is rank %d but template %s is rank %d: Impossible to broadcast\n",prg_nm_get(),wgt->nm,wgt->nbr_dim,var->nm,var->nbr_dim);
	  exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"\n%s: DEBUG %s is rank %d but template %s is rank %d: Not broadcasting %s to %s\n",prg_nm_get(),wgt->nm,wgt->nbr_dim,var->nm,var->nbr_dim,wgt->nm,var->nm);
	  USE_DUMMY_WGT=True;
	} /* endif */
      }else if(wgt_var_dmn_shr_nbr > 0 && wgt_var_dmn_shr_nbr < wgt->nbr_dim){
	/* Some, but not all, of wgt dimensions are in var */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",prg_nm_get(),wgt_var_dmn_shr_nbr,wgt->nm,var->nm,wgt->nbr_dim-wgt_var_dmn_shr_nbr);
	  exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s\n",prg_nm_get(),wgt_var_dmn_shr_nbr,wgt->nm,var->nm,wgt->nbr_dim-wgt_var_dmn_shr_nbr,wgt->nm,var->nm);
	  USE_DUMMY_WGT=True;
	} /* endif */
      } /* end if */
      if(USE_DUMMY_WGT){
	/* Variables do not truly conform, but this might be OK, depending on the application, so set DO_CONFORM flag to false and ... */
	*DO_CONFORM=False;
	/* ... return a dummy weight of 1.0, which allows program logic to pretend variable is weighted, but does not change answers */ 
	wgt_out=var_dpl(var);
	(void)vec_set(wgt_out->type,wgt_out->sz,wgt_out->val,1.0);
      } /* endif */
      if(CONFORMABLE){
	if(var->nbr_dim == wgt->nbr_dim){
	  /* var and wgt conform and are same rank */
	  /* Test whether all wgt and var dimensions match in sequence */
	  for(idx=0;idx<var->nbr_dim;idx++){
	    if(!strstr(wgt->dim[idx]->nm,var->dim[idx]->nm)) break;
	       /*	    if(wgt->dmn_id[idx] != var->dmn_id[idx]) break;*/
	  } /* end loop over dimensions */
	  /* If so, take shortcut and copy wgt to wgt_out */
	  if(idx == var->nbr_dim) *DO_CONFORM=True;
	}else{
	  /* var and wgt conform but are not same rank, set flag to proceed to generic conform routine */
	  *DO_CONFORM=False;
	} /* end else */
      } /* endif CONFORMABLE */
    }else{
      /* var is scalar, if wgt is also then set flag to copy wgt to wgt_out else proceed to generic conform routine */
      if(wgt->nbr_dim == 0) *DO_CONFORM=True; else *DO_CONFORM=False;
    } /* end else */
    if(CONFORMABLE && *DO_CONFORM){
      wgt_out=var_dpl(wgt);
      (void)var_xrf(wgt,wgt_out);
    } /* end if */
  } /* end if */

  if(wgt_out == NULL){
    /* Expand original weight (wgt) to match size of current variable */
    char *wgt_cp;
    char *wgt_out_cp;

    int idx_wgt_var[NC_MAX_DIMS];
    /*    int idx_var_wgt[NC_MAX_DIMS];*/
    int wgt_nbr_dim;
    int wgt_type_sz;
    int var_nbr_dmn_m1;

    long *var_cnt;
    long dmn_ss[NC_MAX_DIMS];
    long dmn_var_map[NC_MAX_DIMS];
    long dmn_wgt_map[NC_MAX_DIMS];
    long var_lmn;
    long wgt_lmn;
    long var_sz;

    /* Copy main attributes of variable into current weight */
    wgt_out=var_dpl(var);
    (void)var_xrf(wgt,wgt_out);

    /* Modify a few elements of weight array */
    wgt_out->nm=wgt->nm;
    wgt_out->id=wgt->id;
    wgt_out->type=wgt->type;
    wgt_out->val.vp=(void *)nco_malloc(wgt_out->sz*nco_typ_lng(wgt_out->type));
    wgt_cp=(char *)wgt->val.vp;
    wgt_out_cp=(char *)wgt_out->val.vp;
    wgt_type_sz=nco_typ_lng(wgt_out->type);

    if(wgt_out->nbr_dim == 0){
      /* Variable (and weight) are scalars, not arrays */

      (void)memcpy(wgt_out_cp,wgt_cp,wgt_type_sz);

    }else{
      /* Variable (and weight) are arrays, not scalars */
      
      /* Create forward and reverse mappings from variable's dimensions to weight's dimensions:

	 dmn_var_map[i] is number of elements between one value of i_th 
	 dimension of variable and next value of i_th dimension, i.e., 
	 number of elements in memory between indicial increments in i_th dimension. 
	 This is computed as product of one (1) times size of all dimensions (if any) after i_th 
	 dimension in variable.

	 dmn_wgt_map[i] contains analogous information, except for original weight variable

	 idx_wgt_var[i] contains index into variable's dimensions of i_th dimension of original weight
	 idx_var_wgt[i] contains index into original weight's dimensions of i_th dimension of variable 

	 Since weight is a subset of variable, some elements of idx_var_wgt may be "empty", or unused

	 Since mapping arrays (dmn_var_map and dmn_wgt_map) are ultimately used for a
	 memcpy() operation, they could (read: should) be computed as byte offsets, not type offsets. 
	 This is why netCDF generic hyperslab routines (ncvarputg(), ncvargetg())
	 request imap vector to specify offset (imap) vector in bytes.
      */

      for(idx=0;idx<wgt->nbr_dim;idx++){
	for(idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(strstr(var->dim[idx_dmn]->nm,wgt->dim[idx]->nm)){
	    idx_wgt_var[idx]=idx_dmn;
	    /*	    idx_var_wgt[idx_dmn]=idx;*/
	    break;
	  } /* end if */
	  /* Sanity check */
	  if(idx_dmn == var->nbr_dim-1){
	    (void)fprintf(stdout,"%s: ERROR wgt %s has dimension %s but var %s does not deep in var_conform_dim()\n",prg_nm_get(),wgt->nm,wgt->dim[idx]->nm,var->nm);
	    exit(EXIT_FAILURE);
	  } /* end if err */
	} /* end loop over variable dimensions */
      } /* end loop over weight dimensions */
      
      /* Figure out map for each dimension of variable */
      for(idx=0;idx<var->nbr_dim;idx++)	dmn_var_map[idx]=1L;
      for(idx=0;idx<var->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<var->nbr_dim;idx_dmn++)
	  dmn_var_map[idx]*=var->cnt[idx_dmn];
      
      /* Figure out map for each dimension of weight */
      for(idx=0;idx<wgt->nbr_dim;idx++)	dmn_wgt_map[idx]=1L;
      for(idx=0;idx<wgt->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<wgt->nbr_dim;idx_dmn++)
	  dmn_wgt_map[idx]*=wgt->cnt[idx_dmn];
      
      /* Define convenience variables to avoid repetitive indirect addressing */
      wgt_nbr_dim=wgt->nbr_dim;
      var_sz=var->sz;
      var_cnt=var->cnt;
      var_nbr_dmn_m1=var->nbr_dim-1;

      /* var_lmn is offset into 1-D array corresponding to N-D indices dmn_ss */
      for(var_lmn=0;var_lmn<var_sz;var_lmn++){
	dmn_ss[var_nbr_dmn_m1]=var_lmn%var_cnt[var_nbr_dmn_m1];
	for(idx=0;idx<var_nbr_dmn_m1;idx++){
	  dmn_ss[idx]=(long)(var_lmn/dmn_var_map[idx]);
	  dmn_ss[idx]%=var_cnt[idx];
	} /* end loop over dimensions */
	
	/* Map (shared) N-D array indices into 1-D index into original weight data */
	wgt_lmn=0L;
	for(idx=0;idx<wgt_nbr_dim;idx++) wgt_lmn+=dmn_ss[idx_wgt_var[idx]]*dmn_wgt_map[idx];
	
	(void)memcpy(wgt_out_cp+var_lmn*wgt_type_sz,wgt_cp+wgt_lmn*wgt_type_sz,wgt_type_sz);
	
      } /* end loop over var_lmn */
      
    } /* end if variable (and weight) are arrays, not scalars */
    
    *DO_CONFORM=True;
  } /* end if we had to stretch weight to fit variable */
  
  if(*DO_CONFORM == -1){
    (void)fprintf(stdout,"%s: ERROR *DO_CONFORM == -1 on exit from var_conform_dim()\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* endif */
  
  /* Current weight (wgt_out) now conforms to current variable */
  return wgt_out;
  
} /* end var_conform_dim() */

void
var_dmn_xrf(var_sct *var)
/*  
   var_sct *var: I pointer to variable structure
*/
{
  /* Purpose: Switch pointers to dimension structures so var->dim points to var->dim->xrf.
     Routine makes dim element of variable structure from var_dpl() refer to counterparts
     of dimensions directly associated with variable it was duplicated from */
  
  int idx;
  
  for(idx=0;idx<var->nbr_dim;idx++) var->dim[idx]=var->dim[idx]->xrf;
  
} /* end var_xrf() */

void
dmn_xrf(dmn_sct *dim,dmn_sct *dmn_dpl)
/*  
   dmn_sct *dim: I/O pointer to dimension structure
   dmn_sct *dim: I/O pointer to dimension structure
*/
{
  /* Make xrf elements of dimension structures point to eachother */

  dim->xrf=dmn_dpl;
  dmn_dpl->xrf=dim;

} /* end dmn_xrf() */

void
var_xrf(var_sct *var,var_sct *var_dpl)
/*  
   var_sct *var: I/O pointer to variable structure
   var_sct *var_dpl: I/O pointer to variable structure
*/
{
  /* Make xrf elements of variable structures point to eachother */

  var->xrf=var_dpl;
  var_dpl->xrf=var;

} /* end var_xrf() */

var_sct *
var_conform_type(nc_type var_out_type,var_sct *var_in)
/*  
   nc_type *var_out_type: I type to convert variable structure to
   var_sct *var_in: I/O pointer to variable structure (may be destroyed)
   var_sct *var_conform_type(): O pointer to variable structure of type var_out_type
*/
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Return copy of input variable typecast to a desired type */

  long idx;
  long sz;
  
  nc_type var_in_type;
  
  ptr_unn val_in;
  ptr_unn val_out;

  var_sct *var_out;

  /* Do types of variable AND its missing value already match? */
  if(var_in->type == var_out_type) return var_in;

  var_out=var_in;
  
  var_in_type=var_in->type;
  
  /* Simple error-checking and diagnostics */
  if(dbg_lvl_get() > 2){
    (void)fprintf(stderr,"%s: DEBUG Converting variable %s from type %s to type %s\n",prg_nm_get(),var_in->nm,nco_typ_sng(var_in_type),nco_typ_sng(var_out_type));
  } /* end if */
  
  /* Move the current var values to swap location */
  val_in=var_in->val;
  
  /* Allocate space for type-conforming values */
  var_out->type=var_out_type;
  var_out->val.vp=(void *)nco_malloc(var_out->sz*nco_typ_lng(var_out->type));
  
  /* Define convenience variables to avoid repetitive indirect addressing */
  sz=var_out->sz;
  val_out=var_out->val;
  
  /* Copy and typecast missing_value attribute, if any */
  if(var_out->has_mss_val){
    ptr_unn var_in_mss_val;

    /* Sequence of following commands is important (copy before overwriting!) */
    var_in_mss_val=var_out->mss_val;
    var_out->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_out->type));
    (void)val_conform_type(var_in_type,var_in_mss_val,var_out_type,var_out->mss_val);
    /* Free original */
    var_in_mss_val.vp=nco_free(var_in_mss_val.vp);
  } /* end if */

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(var_in->type,&val_in);
  (void)cast_void_nctype(var_out->type,&var_out->val);
  
  /* Copy and typecast entire array of values, using implicit coercion rules of C */
  switch(var_out_type){
  case NC_FLOAT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_DOUBLE:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_INT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=(long)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=(long)val_in.dp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_SHORT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short)(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_CHAR:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(unsigned char)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(unsigned char)val_in.dp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_BYTE:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(signed char)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(signed char)val_in.dp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(var_in->type,&val_in);
  (void)cast_nctype_void(var_out->type,&var_out->val);
  
  /* Free input variable data */
  val_in.vp=nco_free(val_in.vp);
  
  return var_out;
  
} /* end var_conform_type() */

void
val_conform_type(nc_type type_in,ptr_unn val_in,nc_type type_out,ptr_unn val_out)
/*  
   nc_type type_in: I type of input value
   ptr_unn val_in: I pointer to input value
   nc_type type_out: I type of output value
   ptr_unn val_out: I pointer to output value
*/
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Fill val_out with a copy of val_in that has been typecast from type_in to type_out
     Last-referenced state of both value pointers is assumed to be .vp, and the val_out union is returned in that state */

  /* val_out must hold enough space (one element of type type_out) to hold output */

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type_in,&val_in);
  (void)cast_void_nctype(type_out,&val_out);
  
  /* Copy and typecast single value using implicit coercion rules of C */
  switch(type_out){
  case NC_FLOAT:
    switch(type_in){
    case NC_FLOAT: *val_out.fp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.fp=*val_in.dp; break; 
    case NC_INT: *val_out.fp=*val_in.lp; break;
    case NC_SHORT: *val_out.fp=*val_in.sp; break;
    case NC_CHAR: *val_out.fp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.fp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_DOUBLE:
    switch(type_in){
    case NC_FLOAT: *val_out.dp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.dp=*val_in.dp; break; 
    case NC_INT: *val_out.dp=*val_in.lp; break;
    case NC_SHORT: *val_out.dp=*val_in.sp; break;
    case NC_CHAR: *val_out.dp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.dp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_INT:
    switch(type_in){
    case NC_FLOAT: *val_out.lp=(long)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.lp=(long)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.lp=*val_in.lp; break;
    case NC_SHORT: *val_out.lp=*val_in.sp; break;
    case NC_CHAR: *val_out.lp=(long)strtod((const char *)val_in.cp,(char **)NULL); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_BYTE: *val_out.lp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_SHORT:
    switch(type_in){
    case NC_FLOAT: *val_out.sp=(short)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.sp=(short)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.sp=*val_in.lp; break;
    case NC_SHORT: *val_out.sp=*val_in.sp; break;
    case NC_CHAR: *val_out.sp=(short)strtod((const char *)val_in.cp,(char **)NULL); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_BYTE: *val_out.sp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_CHAR:
    switch(type_in){
    case NC_FLOAT: *val_out.cp=(unsigned char)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.cp=(unsigned char)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.cp=*val_in.lp; break;
    case NC_SHORT: *val_out.cp=*val_in.sp; break;
    case NC_CHAR: *val_out.cp=*val_in.cp; break;
    case NC_BYTE: *val_out.cp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_BYTE:
    switch(type_in){
    case NC_FLOAT: *val_out.bp=(signed char)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.bp=(signed char)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.bp=*val_in.lp; break;
    case NC_SHORT: *val_out.bp=*val_in.sp; break;
    case NC_CHAR: *val_out.bp=*val_in.cp; break;
    case NC_BYTE: *val_out.bp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: There is no need to un-typecast input pointers because they were passed by
     value and are thus purely local to this routine. The only thing changed by this
     routine is the contents of the location pointed to by the pointer to the output value. */
  
} /* end val_conform_type */

var_sct *
var_avg(var_sct *var,dmn_sct **dim,int nbr_dim,int nco_op_typ)
/*  
   var_sct *var: I/O pointer to variable structure (destroyed)
   dmn_sct **dim: I pointer to list of dimension structures
   int nbr_dim: I number of structures in list
   var_sct *var_avg(): O pointer to PARTIALLY (non-normalized) averaged variable
   nco_op_typ : average,min,max,ttl operation to perform, default is average
*/
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Routine to reduce given variable over specified dimensions. 
     "Reduce" means to reduce the rank of variable by performing an arithmetic operation
     The default operation is to average, but nco_op_typ can also be min, max, etc.
     The input variable structure is destroyed and the routine returns the resized, partially reduced variable
     For some operations, such as min, max, ttl, the variable returned by var_avg() is complete and need not be further processed
     But to complete the averaging operation, the output variable must be normalized by its tally array
     In other words, var_normalize() should be called subsequently if normalization is desired
     Normalization is not done internally to var_avg() in order to allow the user more flexibility
  */ 

  /* Create output variable as a duplicate of the input variable, except for dimensions which are to be averaged over */

  /* var_avg() overwrites contents, if any, of tally array with number of valid reduction operations */

  /* There are three variables to keep track of in this routine, their abbreviations are:
     var: Input variable (already hyperslabbed)
     avg: A contiguous arrangement of all elements of var that contribute to a single element of fix (a quasi-hyperslab)
     fix: Output (averaged) variable
   */

  dmn_sct **dmn_avg;
  dmn_sct **dmn_fix;

  int idx_avg_var[NC_MAX_DIMS];
  /*  int idx_var_avg[NC_MAX_DIMS];*/
  int idx_fix_var[NC_MAX_DIMS];
  /*  int idx_var_fix[NC_MAX_DIMS];*/
  int idx;
  int idx_dmn;
  int nbr_dmn_avg;
  int nbr_dmn_fix;
  int nbr_dmn_var;

  long avg_sz;
  long fix_sz;
  long var_sz;

  var_sct *fix;

  /* Copy basic attributes of input variable into output (averaged) variable */
  fix=var_dpl(var);
  (void)var_xrf(fix,var->xrf);

  /* Create lists of averaging and fixed dimensions (in order of their appearance 
     in the variable). We do not know a priori how many dimensions remain in the 
     output (averaged) variable, but nbr_dmn_var is an upper bound. Similarly, we do
     not know a priori how many of the dimensions in the input list of averaging 
     dimensions (dim) actually occur in the current variable, so we do not know
     nbr_dmn_avg, but nbr_dim is an upper bound on it. */
  nbr_dmn_var=var->nbr_dim;
  nbr_dmn_fix=0;
  nbr_dmn_avg=0;
  dmn_avg=(dmn_sct **)nco_malloc(nbr_dim*sizeof(dmn_sct *));
  dmn_fix=(dmn_sct **)nco_malloc(nbr_dmn_var*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_var;idx++){
    for(idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
      if(var->dmn_id[idx] == dim[idx_dmn]->id){
	dmn_avg[nbr_dmn_avg]=dim[idx_dmn];
	idx_avg_var[nbr_dmn_avg]=idx;
	/*	idx_var_avg[idx]=nbr_dmn_avg;*/ /* Variable is unused but instructive anyway */
	nbr_dmn_avg++;
	break;
      } /* end if */
    } /* end loop over idx_dmn */
    if(idx_dmn == nbr_dim){
      dmn_fix[nbr_dmn_fix]=var->dim[idx];
      idx_fix_var[nbr_dmn_fix]=idx;
      /*      idx_var_fix[idx]=nbr_dmn_fix;*/ /* Variable is unused but instructive anyway */
      nbr_dmn_fix++;
    } /* end if */
  } /* end loop over idx */

  /* Free extra list space */
  if(nbr_dmn_fix > 0) dmn_fix=(dmn_sct **)nco_realloc(dmn_fix,nbr_dmn_fix*sizeof(dmn_sct *)); else dmn_fix=(dmn_sct **)NULL;
  if(nbr_dmn_avg > 0) dmn_avg=(dmn_sct **)nco_realloc(dmn_avg,nbr_dmn_avg*sizeof(dmn_sct *)); else dmn_avg=(dmn_sct **)NULL;

  if(nbr_dmn_avg == 0){
    (void)fprintf(stderr,"%s: WARNING %s does not contain any averaging dimensions\n",prg_nm_get(),fix->nm);
    return (var_sct *)NULL;
  } /* end if */

  /* Get rid of averaged dimensions */
  fix->nbr_dim=nbr_dmn_fix;

  avg_sz=1L;
  for(idx=0;idx<nbr_dmn_avg;idx++){
    avg_sz*=dmn_avg[idx]->cnt;
    fix->sz/=dmn_avg[idx]->cnt;
    if(!dmn_avg[idx]->is_rec_dmn) fix->sz_rec/=dmn_avg[idx]->cnt;
  } /* end loop over idx */

  fix->is_rec_var=False;
  for(idx=0;idx<nbr_dmn_fix;idx++){
    if(dmn_fix[idx]->is_rec_dmn) fix->is_rec_var=True;
    fix->dim[idx]=dmn_fix[idx];
    fix->dmn_id[idx]=dmn_fix[idx]->id;
    fix->srt[idx]=var->srt[idx_fix_var[idx]];
    fix->cnt[idx]=var->cnt[idx_fix_var[idx]];
    fix->end[idx]=var->end[idx_fix_var[idx]];
  } /* end loop over idx */
  
  fix->is_crd_var=False;
  if(nbr_dmn_fix == 1)
    if(dmn_fix[0]->is_crd_dmn) 
      fix->is_crd_var=True;

  /* Trim dimension arrays to their new sizes */
  if(nbr_dmn_fix > 0) fix->dim=(dmn_sct **)nco_realloc(fix->dim,nbr_dmn_fix*sizeof(dmn_sct *)); else fix->dim=NULL;
  if(nbr_dmn_fix > 0) fix->dmn_id=(int *)nco_realloc(fix->dmn_id,nbr_dmn_fix*sizeof(int)); else fix->dmn_id=NULL;
  if(nbr_dmn_fix > 0) fix->srt=(long *)nco_realloc(fix->srt,nbr_dmn_fix*sizeof(long)); else fix->srt=NULL;
  if(nbr_dmn_fix > 0) fix->cnt=(long *)nco_realloc(fix->cnt,nbr_dmn_fix*sizeof(long)); else fix->cnt=NULL;
  if(nbr_dmn_fix > 0) fix->end=(long *)nco_realloc(fix->end,nbr_dmn_fix*sizeof(long)); else fix->end=NULL;
  
  /* If product of sizes of all averaging dimensions is 1, input and output value arrays should be identical 
     Since var->val was already copied to fix->val by var_dpl() at the beginning
     of this routine, only one task remains, to set fix->tally appropriately. */
  if(avg_sz == 1L){
    long fix_sz;
    long *fix_tally;

    fix_sz=fix->sz;
    fix_tally=fix->tally;

    /* First set tally field to 1 */
    for(idx=0;idx<fix_sz;idx++) fix_tally[idx]=1L;
    /* Next overwrite any missing value locations with zero */
    if(fix->has_mss_val){
      int val_sz_byte;

      char *val;
      char *mss_val;

      /* NB: Use char * rather than void * because some compilers (acc) will not do pointer
	 arithmetic on void * */
      mss_val=(char *)fix->mss_val.vp;
      val_sz_byte=nco_typ_lng(fix->type);
      val=(char *)fix->val.vp;
      for(idx=0;idx<fix_sz;idx++,val+=val_sz_byte)
	if(!memcmp(val,mss_val,val_sz_byte)) fix_tally[idx]=0L;
    } /* fix->has_mss_val */
  } /* end if avg_sz == 1L */

  /* Starting at first element of input hyperslab, add up next stride elements
     and place result in first element of output hyperslab. */
  if(avg_sz != 1L){
    char *avg_cp;
    char *var_cp;
    
    int type_sz;
    int nbr_dmn_var_m1;

    long *var_cnt;
    long avg_lmn;
    long fix_lmn;
    long var_lmn;
    long dmn_ss[NC_MAX_DIMS];
    long dmn_var_map[NC_MAX_DIMS];
    long dmn_avg_map[NC_MAX_DIMS];
    long dmn_fix_map[NC_MAX_DIMS];

    ptr_unn avg_val;

    /* Define convenience variables to avoid repetitive indirect addressing */
    fix_sz=fix->sz;
    nbr_dmn_var_m1=nbr_dmn_var-1;
    type_sz=nco_typ_lng(fix->type);
    var_cnt=var->cnt;
    var_cp=(char *)var->val.vp;
    var_sz=var->sz;
    
    /* Reuse the existing value buffer (it is of size var_sz, created by var_dpl())*/
    avg_val=fix->val;
    avg_cp=(char *)avg_val.vp;
    /* Create a new value buffer for output (averaged) size */
    fix->val.vp=(void *)nco_malloc(fix->sz*nco_typ_lng(fix->type));
    /* Resize (or just plain allocate) the tally array */
    fix->tally=(long *)nco_realloc(fix->tally,fix->sz*sizeof(long));

    /* Re-initialize value and tally arrays */
    (void)zero_long(fix->sz,fix->tally);
    (void)var_zero(fix->type,fix->sz,fix->val);
  
    /* Compute map for each dimension of variable */
    for(idx=0;idx<nbr_dmn_var;idx++) dmn_var_map[idx]=1L;
    for(idx=0;idx<nbr_dmn_var-1;idx++)
      for(idx_dmn=idx+1;idx_dmn<nbr_dmn_var;idx_dmn++)
	dmn_var_map[idx]*=var->cnt[idx_dmn];
    
    /* Compute map for each dimension of output variable */
    for(idx=0;idx<nbr_dmn_fix;idx++) dmn_fix_map[idx]=1L;
    for(idx=0;idx<nbr_dmn_fix-1;idx++)
      for(idx_dmn=idx+1;idx_dmn<nbr_dmn_fix;idx_dmn++)
	dmn_fix_map[idx]*=fix->cnt[idx_dmn];
    
    /* Compute map for each dimension of averaging buffer */
    for(idx=0;idx<nbr_dmn_avg;idx++) dmn_avg_map[idx]=1L;
    for(idx=0;idx<nbr_dmn_avg-1;idx++)
      for(idx_dmn=idx+1;idx_dmn<nbr_dmn_avg;idx_dmn++)
	dmn_avg_map[idx]*=dmn_avg[idx_dmn]->cnt;
    
    /* var_lmn is the offset into 1-D array */
    for(var_lmn=0;var_lmn<var_sz;var_lmn++){

      /* dmn_ss are corresponding indices (subscripts) into N-D array */
      dmn_ss[nbr_dmn_var_m1]=var_lmn%var_cnt[nbr_dmn_var_m1];
      for(idx=0;idx<nbr_dmn_var_m1;idx++){
	dmn_ss[idx]=(long)(var_lmn/dmn_var_map[idx]);
	dmn_ss[idx]%=var_cnt[idx];
      } /* end loop over dimensions */

      /* Map variable's N-D array indices into a 1-D index into averaged data */
      fix_lmn=0L;
      for(idx=0;idx<nbr_dmn_fix;idx++) fix_lmn+=dmn_ss[idx_fix_var[idx]]*dmn_fix_map[idx];
      
      /* Map N-D array indices into a 1-D offset from offset of its group */
      avg_lmn=0L;
      for(idx=0;idx<nbr_dmn_avg;idx++) avg_lmn+=dmn_ss[idx_avg_var[idx]]*dmn_avg_map[idx];
      
      /* Copy current element in input array into its slot in sorted avg_val */
      (void)memcpy(avg_cp+(fix_lmn*avg_sz+avg_lmn)*type_sz,var_cp+var_lmn*type_sz,type_sz);
      
    } /* end loop over var_lmn */
    
    /* Input data are now sorted and stored (in avg_val) in blocks (of length avg_sz)
       in same order as blocks' average values will appear in output buffer. 
       An averaging routine can take advantage of this by casting avg_val to a two dimensional
       variable and averaging over inner dimension. 
       This is where tally array is actually set */
    switch(nco_op_typ){
    case nco_op_max:
      (void)var_avg_reduce_max(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,avg_val,fix->val);
      break;
    case nco_op_min:
      (void)var_avg_reduce_min(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,avg_val,fix->val);
      break;
    case nco_op_avg: /* Operations: Previous=none, Current=sum, Next=normalize and root */
    case nco_op_sqravg: /* Operations: Previous=none, Current=sum, Next=normalize and square */
    case nco_op_avgsqr: /* Operations: Previous=square, Current=sum, Next=normalize */
    case nco_op_rms: /* Operations: Previous=square, Current=sum, Next=normalize and root */
    case nco_op_rmssdn: /* Operations: Previous=square, Current=sum, Next=normalize and root */
    case nco_op_ttl: /* Operations: Previous=none, Current=sum, Next=none */
    default:
      (void)var_avg_reduce_ttl(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,fix->tally,avg_val,fix->val);	  		
      break;
    } /* end case */

    /* Free dynamic memory that held rearranged input variable values */
    avg_val.vp=nco_free(avg_val.vp);
  } /* end if avg_sz != 1 */
  
  /* Free input variable */
  var=var_free(var);
  dmn_avg=nco_free(dmn_avg);
  dmn_fix=nco_free(dmn_fix);

  /* Return averaged variable */
  return fix;
} /* end var_avg() */

var_sct *
var_free(var_sct *var)
/*  
   var_sct *var: I pointer to variable structure
*/
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with a dynamically allocated variable structure */
  
  /* NB: var->nm is not freed because I decided to let names be static memory, and refer to
     the optarg list if available. This assumption needs to be changed before freeing 
     the name pointer. */
  
  var->val.vp=nco_free(var->val.vp);
  var->mss_val.vp=nco_free(var->mss_val.vp);
  var->tally=nco_free(var->tally);
  var->dmn_id=nco_free(var->dmn_id);
  var->dim=nco_free(var->dim);
  var->srt=nco_free(var->srt);
  var->end=nco_free(var->end);
  var->cnt=nco_free(var->cnt);
  var->srd=nco_free(var->srd);
  var->scl_fct.vp=nco_free(var->scl_fct.vp);
  var->add_fst.vp=nco_free(var->add_fst.vp);

  /* Free structure pointer only after all dynamic elements of structure have been freed */
  var=nco_free(var);

  return NULL;

} /* end var_free */

bool
arm_inq(int nc_id)
/*  
   int nc_id: I netCDF file ID
   bool arm_inq(): O whether file is an ARM data file
*/
{
  /* Routine to check whether file adheres to ARM time format */
  bool ARM_FORMAT;

  int time_dmn_id;
  int base_time_id;
  int time_offset_id;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  /* Look for the signature of an ARM file */
  
  rcd+=nco_inq_dimid_flg(nc_id,"time",&time_dmn_id);
  rcd+=nco_inq_varid_flg(nc_id,"base_time",&base_time_id);
  rcd+=nco_inq_varid_flg(nc_id,"time_offset",&time_offset_id);
  
  /* All three IDs must be valid to handle ARM format */
  if(rcd != NC_NOERR){
    ARM_FORMAT=False;
  }else{
    (void)fprintf(stderr,"%s: CONVENTION File convention is DOE ARM\n",prg_nm_get()); 
    ARM_FORMAT=True;
  } /* end else */

  return ARM_FORMAT;
} /* end arm_inq */

nco_long
arm_base_time_get(int nc_id)
/*  
   int nc_id: I netCDF file ID
   nco_long arm_base_time_get: O value of base_time variable
*/
{
  /* Routine to check whether file adheres to ARM time format */
  int base_time_id;

  nco_long base_time;

  (void)nco_inq_varid(nc_id,"base_time",&base_time_id);
  (void)nco_get_var1(nc_id,base_time_id,0L,&base_time,NC_INT);

  return base_time;
} /* end arm_base_time_get */

void
var_max_bnr(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
   nc_type type: I netCDF type of operands
   long sz: I size (in elements) of operands
   int has_mss_val: I flag for missing values
   ptr_unn mss_val: I value of missing value
   ptr_unn op1: I values of first operand
   ptr_unn op2: I/O values of second operand on input, values of maximium on output
*/
{
  /* Routine to find maximium value(s) of the two operands
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and to both be of the specified type. Operands' values are 
     assumed to be in memory already. */
  
  long idx;
  
  /* Typecast pointer to values before access */
  /* It is not necessary to untype-cast pointer types after using them as we have 
     operated on local copies of them */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.fp[idx] < op1.fp[idx]) op2.fp[idx]=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] == mss_val_flt) 
	  op2.fp[idx]=op1.fp[idx];
	else if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] < op1.fp[idx]))
	  op2.fp[idx]=op1.fp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.dp[idx] < op1.dp[idx]) op2.dp[idx]=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] == mss_val_dbl) 
	  op2.dp[idx]=op1.dp[idx];
	else if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] < op1.dp[idx]))
	  op2.dp[idx]=op1.dp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.lp[idx] < op1.lp[idx]) 
	  op2.lp[idx]=op1.lp[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.lp[idx] == mss_val_lng) 
	  op2.lp[idx]=op1.lp[idx];
	else if((op1.lp[idx] != mss_val_lng) && (op2.lp[idx] < op1.lp[idx]))
	  op2.lp[idx]=op1.lp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] < op1.sp[idx])
	  op2.sp[idx]=op1.sp[idx];
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_shrt) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_shrt) && (op2.sp[idx] < op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end var_max_bnr() */

void
var_min_bnr(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand
  ptr_unn op2: I/O values of second operand on input, values of maximium on output
*/
{
  /* Purpose: Find minimium value(s) of two operands and store result in second operand 
     Operands are assumed to have conforming dimensions, and to both be of the specified type
     Operands' values are assumed to be in memory already */
  long idx;
  
  /* Typecast pointer to values before access */
  /* It is not necessary to uncast pointer types after using them as we have 
     operated on local copies of them */
  
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.fp[idx] > op1.fp[idx]) op2.fp[idx]=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] == mss_val_flt) 
	  op2.fp[idx]=op1.fp[idx];
	else if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] > op1.fp[idx]))
	  op2.fp[idx]=op1.fp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.dp[idx] > op1.dp[idx]) op2.dp[idx]=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] == mss_val_dbl) 
	  op2.dp[idx]=op1.dp[idx];
	else if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] > op1.dp[idx]))
	  op2.dp[idx]=op1.dp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.lp[idx] > op1.lp[idx]) 
	  op2.lp[idx]=op1.lp[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.lp[idx] == mss_val_lng) 
	  op2.lp[idx]=op1.lp[idx];
	else if((op1.lp[idx] != mss_val_lng) && (op2.lp[idx] > op1.lp[idx]))
	  op2.lp[idx]=op1.lp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] > op1.sp[idx])  op2.sp[idx]=op1.sp[idx];
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_shrt) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_shrt) && (op2.sp[idx] > op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end var_min_bnr() */

void
var_multiply(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
	ptr_unn op2: I/O values of second operand on input, values of product on output
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: multiply value of first operand by value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and to both be of the specified type. Operands' values are 
     assumed to be in memory already. */
  
  /* Multiplication is currently defined as op2:=op1*op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_multiply_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]*=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]*=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_multiply_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]*=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]*=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]*=op1.lp[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]*=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]*=op1.sp[idx];
    }else{
      long mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_shrt) && (op1.sp[idx] != mss_val_shrt)) op2.sp[idx]*=op1.sp[idx]; else op2.sp[idx]=mss_val_shrt;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_multiply() */

void
var_divide(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand
  ptr_unn op2: I/O values of second operand on input, values of quotient on output
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide value of first operand by value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and to both be of specified type. Operands' values are 
     assumed to be in memory already. */

  /* Division is currently defined as op2:=op2/op1 */  

  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_divide_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]/=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]/=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_divide_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]/=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]/=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]/=op1.lp[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]/=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]/=op1.sp[idx];
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_shrt) && (op1.sp[idx] != mss_val_shrt)) op2.sp[idx]/=op1.sp[idx]; else op2.sp[idx]=mss_val_shrt;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_divide() */

void
var_add(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I/O counter space
  ptr_unn op1: I values of first operand
  ptr_unn op2: I/O values of second operand on input, values of sum on output
 */
{
  /* Routine to add value of first operand to value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and be of the specified type. Operands' values are 
     assumed to be in memory already. */

  /* Addition is currently defined as op2:=op1+op2 */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_add_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_add_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.lp[idx]+=op1.lp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)){
	  op2.lp[idx]+=op1.lp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]+=op1.sp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_shrt) && (op1.sp[idx] != mss_val_shrt)){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_add() */

void
var_sqrt(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I/O counter space
  ptr_unn op1: I values of first operand
  ptr_unn op2: O squareroot of first operand
 */
{
  /* Purpose: Place squareroot of first operand in value of second operand 
     Operands are assumed to have conforming dimensions, and be of specified type 
     Operands' values are assumed to be in memory already */

  /* Square root is currently defined as op2:=sqrt(op1) */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_sqrt_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]=sqrt(op1.fp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt){
	  op2.fp[idx]=sqrt(op1.fp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_sqrt_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]=sqrt(op1.dp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl){
	  op2.dp[idx]=sqrt(op1.dp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.lp[idx]=(long)sqrt(op1.lp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng){
	  op2.lp[idx]=(long)sqrt(op1.lp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]=(short)sqrt(op1.sp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt){
	  op2.sp[idx]=(short)sqrt(op1.sp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_sqrt() */

void
var_avg_reduce_ttl(nc_type type,long sz_op1,long sz_op2,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz_op1: I size (in elements) of op1
  long sz_op2: I size (in elements) of op2
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I/O counter space
  ptr_unn op1: I values of first operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2))
  ptr_unn op2: O values resulting from averaging each block of input operand
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Perform arithmetic operation on values in each contiguous block of first operand and place
     result in corresponding element in second operand. 
     Currently the arithmetic operation performed is a simple summation of the elements in op1
     Inpute operands are assumed to have conforming types, but not dimensions or sizes
     var_avg_reduce() knows nothing about dimensions, it is purely a one dimensional array operator
     acting serially on each element of the input buffer op1.
     The calling rouine knows exactly how the rank of the output, op2, is reduced from the rank of the input
     This routine currently only does summing rathering than averaging in order to remain flexible
     It is expected that operations which require normalization, e.g., averaging, will call var_normalize() 
     or var_divide() to divide the sum set in this routine by the tally set in this routine. */

  /* There is a GNUC, a non-GNUC, and a Fortran block for each operation
     GNUC: Utilize (non-ANSI-compliant) compiler support for local automatic arrays
     This results in more elegent loop structure and, theoretically, in faster performance
     non-GNUC: Fully ANSI-compliant structure
     Fortran: Same structure as GNUC blocks (g77 and all Fortran90 compilers support local autmatic arrays) */

#ifndef __GNUC__
  long blk_off;
  long idx_op1;
#endif /* !__GNUC__ */
  long idx_op2;
  long idx_blk;
  long sz_blk;
#ifndef USE_FORTRAN_ARITHMETIC
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  unsigned char mss_val_chr;
  signed char mss_val_byt;
#endif /* USE_FORTRAN_ARITHMETIC */
  nco_long mss_val_lng=nco_long_CEWI;
  short mss_val_shrt=short_CEWI;

  sz_blk=sz_op1/sz_op2;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

#ifndef USE_FORTRAN_ARITHMETIC
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_shrt=*mss_val.sp; break;
    case NC_INT: mss_val_lng=*mss_val.lp; break;
    case NC_BYTE: mss_val_byt=*mss_val.bp; break;
    case NC_CHAR: mss_val_chr=*mss_val.cp; break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
  } /* endif */
#endif /* USE_FORTRAN_ARITHMETIC */

  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_avg_reduce_real(&sz_blk,&sz_op2,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.fp[idx_op2]+=op1.fp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.fp[idx_op1] != mss_val_flt){
	    op2.fp[idx_op2]+=op1.fp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_flt){
	      op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_avg_reduce_double_precision(&sz_blk,&sz_op2,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.dp[idx_op2]+=op1.dp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.dp[idx_op1] != mss_val_dbl){
	    op2.dp[idx_op2]+=op1.dp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
	      op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.dp[idx_op2]=mss_val_dbl;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.lp[idx_op2]+=op1.lp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.lp[idx_op1] != mss_val_lng){
	    op2.lp[idx_op2]+=op1.lp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.lp[idx_op2]=mss_val_lng;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.lp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.lp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_lng){
	      op2.lp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.lp[idx_op2]=mss_val_lng;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.sp[idx_op2]+=op1.sp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.sp[idx_op1] != mss_val_shrt){
	    op2.sp[idx_op2]+=op1.sp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.sp[idx_op2]=mss_val_shrt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      short op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_shrt){
	      op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.sp[idx_op2]=mss_val_shrt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Do nothing except avoid compiler warnings */
    mss_val_chr=mss_val_chr;
    break;
  case NC_BYTE:
    /* Do nothing except avoid compiler warnings */
    mss_val_byt=mss_val_byt;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_avg_reduce_ttl() */

void
var_avg_reduce_min(nc_type type,long sz_op1,long sz_op2,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz_op1: I size (in elements) of op1
  long sz_op2: I size (in elements) of op2
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2))
  ptr_unn op2: O minimum value of each block of input operand
 */
{
  /* Routine to find minium values in each contiguous block of first operand and place
     result in corresponding element in second operand. Operands are assumed to have
     conforming types, but not dimensions or sizes. */

  /* var_avg_reduce_min() is derived from var_avg_reduce_ttl()
     Routines are very similar but tallies are not incremented
     See var_avg_reduce_ttl() for more algorithmic documentation
     var_avg_reduce_max() is derived from var_avg_reduce_min() */

#ifndef __GNUC__
  long blk_off;
  long idx_op1;
#endif /* !__GNUC__ */
  long idx_op2;
  long idx_blk;
  long sz_blk;

  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_long mss_val_lng=nco_long_CEWI;
  short mss_val_shrt=short_CEWI;
  unsigned char mss_val_chr;
  signed char mss_val_byt;
  
  bool flg_mss=False;
  
  sz_blk=sz_op1/sz_op2;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_shrt=*mss_val.sp; break;
    case NC_INT: mss_val_lng=*mss_val.lp; break;
    case NC_BYTE: mss_val_byt=*mss_val.bp; break;
    case NC_CHAR: mss_val_chr=*mss_val.cp; break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
  } /* endif */
  
  switch(type){
  case NC_FLOAT:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.fp[idx_op2]=op1.fp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.fp[idx_op2] > op1.fp[blk_off+idx_blk]) op2.fp[idx_op2]=op1.fp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.fp[idx_op1] != mss_val_flt) {
	    if(!flg_mss || op2.fp[idx_op2] > op1.fp[idx_op1]) op2.fp[idx_op2]=op1.fp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.fp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.fp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_flt) {
	      if(!flg_mss || op2.fp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    
    break;
  case NC_DOUBLE:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.dp[idx_op2]=op1.dp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.dp[idx_op2] > op1.dp[blk_off+idx_blk]) op2.dp[idx_op2]=op1.dp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.dp[idx_op1] != mss_val_dbl) {
	    if(!flg_mss || (op2.dp[idx_op2] > op1.dp[idx_op1])) op2.dp[idx_op2]=op1.dp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.dp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.dp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk] ;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
	      if(!flg_mss || (op2.dp[idx_op2] > op1_2D[idx_op2][idx_blk])) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk];	    
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.lp[idx_op2]=op1.lp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.lp[idx_op2] > op1.lp[blk_off+idx_blk]) op2.lp[idx_op2]=op1.lp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.lp[idx_op1] != mss_val_lng){
	    if(!flg_mss || op2.lp[idx_op2] > op1.lp[idx_op1]) op2.lp[idx_op2]=op1.lp[idx_op1];
	    flg_mss= True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.lp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.lp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.lp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_lng){
	      if(!flg_mss || op2.lp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.sp[idx_op2]=op1.sp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.sp[idx_op2] > op1.sp[blk_off+idx_blk]) op2.sp[idx_op2]=op1.sp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.sp[idx_op1] != mss_val_shrt){
	    if(!flg_mss || op2.sp[idx_op2] > op1.sp[idx_op1]) op2.sp[idx_op2]=op1.sp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.sp[idx_op2]=mss_val_shrt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      short op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.sp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.sp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_shrt){
	      if(!flg_mss  || op2.sp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.sp[idx_op2]=mss_val_shrt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Do nothing except avoid compiler warnings */
    mss_val_chr=mss_val_chr;
    break;
  case NC_BYTE:
    /* Do nothing except avoid compiler warnings */
    mss_val_byt=mss_val_byt;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end  switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_avg_reduce_min() */

void
var_avg_reduce_max(nc_type type,long sz_op1,long sz_op2,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz_op1: I size (in elements) of op1
  long sz_op2: I size (in elements) of op2
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2))
  ptr_unn op2: O maximum value of each block of input operand
 */
{
  /* Routine to find maximium values in each contiguous block of first operand and place
     result in corresponding element in second operand. Operands are assumed to have
     conforming types, but not dimensions or sizes. */
  
  /* var_avg_reduce_min() is derived from var_avg_reduce_ttl()
     Routines are very similar but tallies are not incremented
     var_avg_reduce_max() is identical to var_avg_reduce_min() except the ">" have been swapped for "<" 
     See var_avg_reduce_ttl() for more algorithmic documentation
  */

#ifndef __GNUC__
  long blk_off;
  long idx_op1;
#endif /* !__GNUC__ */
  long idx_op2;
  long idx_blk;
  long sz_blk;
  
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_long mss_val_lng=nco_long_CEWI;
  short mss_val_shrt=short_CEWI;
  unsigned char mss_val_chr;
  signed char mss_val_byt;
  
  bool flg_mss=False;
  
  sz_blk=sz_op1/sz_op2;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_shrt=*mss_val.sp; break;
    case NC_INT: mss_val_lng=*mss_val.lp; break;
    case NC_BYTE: mss_val_byt=*mss_val.bp; break;
    case NC_CHAR: mss_val_chr=*mss_val.cp; break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
  } /* endif */
  
  switch(type){
  case NC_FLOAT:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.fp[idx_op2]=op1.fp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.fp[idx_op2] < op1.fp[blk_off+idx_blk]) op2.fp[idx_op2]=op1.fp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.fp[idx_op1] != mss_val_flt) {
	    if(!flg_mss || op2.fp[idx_op2] < op1.fp[idx_op1]) op2.fp[idx_op2]=op1.fp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.fp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.fp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_flt) {
	      if(!flg_mss || op2.fp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    
    break;
  case NC_DOUBLE:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.dp[idx_op2]=op1.dp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.dp[idx_op2] < op1.dp[blk_off+idx_blk]) op2.dp[idx_op2]=op1.dp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.dp[idx_op1] != mss_val_dbl) {
	    if(!flg_mss || (op2.dp[idx_op2] < op1.dp[idx_op1])) op2.dp[idx_op2]=op1.dp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.dp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.dp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk] ;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
	      if(!flg_mss || (op2.dp[idx_op2] < op1_2D[idx_op2][idx_blk])) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk];	    
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.lp[idx_op2]=op1.lp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.lp[idx_op2] < op1.lp[blk_off+idx_blk]) op2.lp[idx_op2]=op1.lp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.lp[idx_op1] != mss_val_lng){
	    if(!flg_mss || op2.lp[idx_op2] < op1.lp[idx_op1]) op2.lp[idx_op2]=op1.lp[idx_op1];
	    flg_mss= True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.lp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.lp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.lp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_lng){
	      if(!flg_mss || op2.lp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.sp[idx_op2]=op1.sp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.sp[idx_op2] < op1.sp[blk_off+idx_blk]) op2.sp[idx_op2]=op1.sp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.sp[idx_op1] != mss_val_shrt){
	    if(!flg_mss || op2.sp[idx_op2] < op1.sp[idx_op1]) op2.sp[idx_op2]=op1.sp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.sp[idx_op2]=mss_val_shrt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      short op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.sp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.sp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_shrt){
	      if(!flg_mss  || op2.sp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.sp[idx_op2]=mss_val_shrt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Do nothing except avoid compiler warnings */
    mss_val_chr=mss_val_chr;
    break;
  case NC_BYTE:
    /* Do nothing except avoid compiler warnings */
    mss_val_byt=mss_val_byt;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end  switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_avg_reduce_max() */

void 
nco_opr_drv(int cnt,int nco_op_typ,var_sct *var_prc_out, var_sct *var_prc)
{
  /* Purpose: Perform appropriate ncra/ncea operation (avg, min, max, ttl, ...) on operands
     nco_opr_drv() is called within the record loop of ncra, and within file loop of ncea
     These operations perform part, but not all, of the necessary operations for each procedure
     Most arithmetic operations require additional procedures such as normalization be performed after all files/records have been processed */
  
  /* var_prc_out->type and var_prc->type should be equal and thus interchangeable
     var_prc_out->sz and var_prc->sz should be equal and thus interchangeable */
  switch (nco_op_typ){
  case nco_op_min: /* Minimum */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(cnt == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else	  
      (void)var_min_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;
  case nco_op_max: /* Maximium */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(cnt == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else
      (void)var_max_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_avg: /* Average */
  case nco_op_sqrt: /* Squareroot will produce the squareroot of the mean */
  case nco_op_ttl: /* Total */
  case nco_op_sqravg: /* Square of the mean */
    (void)var_add(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  case nco_op_rms: /* Root mean square */
  case nco_op_rmssdn: /* Root mean square normalized by N-1 */
  case nco_op_avgsqr: /* Mean square */
    /* Square values in var_prc first */
    var_multiply(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc->val);
    /* Sum the squares */
    (void)var_add(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  } /* end switch */
} /* end nco_opr_drv() */

void
var_normalize(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I counter space
  ptr_unn op1: I/O values of first operand on input, normalized result on output
*/
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Normalize value of first operand by count in tally array 
     and store result in first operand. */

  /* Normalization is currently defined as op1:=op1/tally */  

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_normalize_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=tally[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.fp[idx]/=tally[idx]; else op1.fp[idx]=mss_val_flt;
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_normalize_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.dp[idx]/=tally[idx]; else op1.dp[idx]=mss_val_dbl;
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=tally[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.lp[idx]/=tally[idx]; else op1.lp[idx]=mss_val_lng;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx];
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.sp[idx]/=tally[idx]; else op1.sp[idx]=mss_val_shrt;
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_normalize() */

void
var_normalize_sdn(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I counter space
  ptr_unn op1: I/O values of first operand on input, normalized result on output
*/
{
  /* Purpose: Normalize value of first operand by count-1 in tally array 
     and store result in first operand. */

  /* Normalization is currently defined as op1:=op1/(--tally) */  

  /* var_normalize_sdn() is based on var_normalize() and algorithms should be kept consistent with eachother */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=(tally[idx]-1);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.fp[idx]/=(tally[idx]-1); else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=(tally[idx]-1);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.dp[idx]/=(tally[idx]-1); else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=(tally[idx]-1);
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.lp[idx]/=(tally[idx]-1); else op1.lp[idx]=mss_val_lng;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=(tally[idx]-1);
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.sp[idx]/=(tally[idx]-1); else op1.sp[idx]=mss_val_shrt;
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end of var_normalize_sdn */

ptr_unn
mss_val_mk(nc_type type)
/* 
  nc_type type: I netCDF type of operand
  ptr_unn mss_val_mk(): O ptr_unn containing default missing value for type type
 */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Routine to return a pointer union containing default missing value for type type */

  ptr_unn mss_val;

  mss_val.vp=(void *)nco_malloc(nco_typ_lng(type));

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT: *mss_val.fp=NC_FILL_FLOAT; break; 
  case NC_DOUBLE: *mss_val.dp=NC_FILL_DOUBLE; break; 
  case NC_INT: *mss_val.lp=NC_FILL_INT; break;
  case NC_SHORT: *mss_val.sp=NC_FILL_SHORT; break;
  case NC_CHAR: *mss_val.cp=NC_FILL_CHAR; break;
  case NC_BYTE: *mss_val.bp=NC_FILL_BYTE; break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(type,&mss_val);
  
  return mss_val;

} /* end mss_val_mk() */
  
void
mss_val_cp(var_sct *var1,var_sct *var2)
/* 
  var_sct *var1: I variable structure with template missing value to copy
  var_sct *var2: I/O variable structure with missing value to fill in/overwrite
  mss_val_cp(): 
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Copy missing value from var1 to var2
     On exit, var2 contains has_mss_val, and mss_val identical to var1
     Type of mss_val in var2 will agree with type of var2
     This maintains assumed consistency between type of variable and
     type of mss_val in all var_sct's
   */

  if(!var1->has_mss_val){
    var2->has_mss_val=False;
    if(var2->mss_val.vp != NULL) free(var2->mss_val.vp);
  }else{ /* endif no mss_val in var1 */
    var2->mss_val.vp=(void *)nco_realloc(var2->mss_val.vp,nco_typ_lng(var2->type));
    (void)val_conform_type(var1->type,var1->mss_val,var2->type,var2->mss_val);
    var2->has_mss_val=True;
  } /* endif var1 has mss_val */

} /* end mss_val_cp() */
  
void
var_mask(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,double op1,int op_typ_rlt,ptr_unn op2,ptr_unn op3)
/* 
  nc_type type: I netCDF type of operand op3
  long sz: I size (in elements) of operand op3
  int has_mss_val: I flag for missing values (basically assumed to be true)
  ptr_unn mss_val: I value of missing value
  double op1: I Target value against which mask field will be compared (i.e., argument of -M)
  int op_typ_rlt: I type of relationship to test for between op2 and op1
  ptr_unn op2: I Value of mask field
  ptr_unn op3: I/O values of second operand on input, masked values on output
 */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Routine to mask third operand by second operand. Wherever second operand does not 
     equal first operand the third operand will be set to its missing value. */

  /* Masking is currently defined as if(op2 !op_typ_rlt op1) then op3:=mss_val */  

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  (void)cast_void_nctype(type,&op3);
  if(has_mss_val){
    (void)cast_void_nctype(type,&mss_val);
  }else{
    (void)fprintf(stdout,"%s: ERROR has_mss_val is inconsistent with purpose of var_ask(), i.e., has_mss_val is not True\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* end else */

  /* NB: Explicit coercion when comparing op2 to op1 is necessary */
  switch(type){
  case NC_FLOAT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.fp[idx] != (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.fp[idx] == (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >= (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <= (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >  (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <  (float)op1) op3.fp[idx]=*mss_val.fp; break;
    } /* end switch */
    break;
  case NC_DOUBLE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.dp[idx] != (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.dp[idx] == (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >= (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <= (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >  (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <  (double)op1) op3.dp[idx]=*mss_val.dp; break;
    } /* end switch */
    break;
  case NC_INT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.lp[idx] != (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.lp[idx] == (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >= (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <= (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >  (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <  (long)op1) op3.lp[idx]=*mss_val.lp; break;
    } /* end switch */
    break;
  case NC_SHORT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.sp[idx] != (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.sp[idx] == (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >= (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <= (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >  (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <  (short)op1) op3.sp[idx]=*mss_val.sp; break;
    } /* end switch */
    break;
  case NC_CHAR:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.cp[idx] != (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.cp[idx] == (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >= (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <= (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >  (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <  (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    } /* end switch */
    break;
  case NC_BYTE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.bp[idx] != (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.bp[idx] == (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >= (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <= (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >  (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <  (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    } /* end switch */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* It is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_mask() */

void
var_zero(nc_type type,long sz,ptr_unn op1)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  ptr_unn op1: I values of first operand
 */
{
  /* Routine to zero value of first operand and store result in second operand. */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);

  switch(type){
  case NC_FLOAT:
    for(idx=0;idx<sz;idx++) op1.fp[idx]=0.0;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=0.0;
    break;
  case NC_INT:
    for(idx=0;idx<sz;idx++) op1.lp[idx]=0L;
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=0;
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_zero() */

void
vec_set(nc_type type,long sz,ptr_unn op1,double op2)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  ptr_unn op1: I values of first operand
  double op2: I value to fill vector with
 */
{
  /* Routine to fill every value of first operand with value of second operand */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);

  switch(type){
  case NC_FLOAT:
    for(idx=0;idx<sz;idx++) op1.fp[idx]=op2;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=op2;
    break;
  case NC_INT:
    for(idx=0;idx<sz;idx++) op1.lp[idx]=(long)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=(short)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end vec_set() */

void
zero_long(long sz,long *op1)
/* 
  long sz: I size (in elements) of operand
  long *op1: I values of first operand
 */
{
  /* Routine to zero value of first operand and store result in first operand. */

  long idx;
  if(op1 != NULL){
    for(idx=0;idx<sz;idx++) op1[idx]=0L;
  }else{
    (void)fprintf(stdout,"%s: ERROR zero_long() asked to zero NULL pointer\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* endif */

} /* end zero_long() */

void
var_subtract(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I [type] netCDF type of operands
  long sz: I [nbr] Size (in elements) of operands
  int has_mss_val: I [flg] Flag for missing values
  ptr_unn mss_val: I [flg] Value of missing value
  ptr_unn op1: I [val] Values of first operand
  ptr_unn op2: I/O [val] Values of second operand on input, values of difference on output
 */
{
  /* Purpose: Subtract value of first operand from value of second operand 
     and store result in second operand. 
     Operands are assumed to have conforming dimensions, and be of specified type. 
     Operands' values are assumed to be in memory already. */

  /* Subtraction is currently defined as op2:=op2-op1 */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_subtract_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]-=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]-=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_subtract_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]-=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]-=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]-=op1.lp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != *mss_val.lp) && (op1.lp[idx] != *mss_val.lp)) op2.lp[idx]-=op1.lp[idx]; else op2.lp[idx]=*mss_val.lp;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]-=op1.sp[idx];
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_shrt) && (op1.sp[idx] != mss_val_shrt)) op2.sp[idx]-=op1.sp[idx]; else op2.sp[idx]=mss_val_shrt;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_subtract() */

void
var_add_no_tally(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* nc_type type: I [type] netCDF type of operands
  long sz: I [nbr] Size (in elements) of operands
  int has_mss_val: I [flg] Flag for missing values
  ptr_unn mss_val: I [flg] Value of missing value
  ptr_unn op1: I [val] Values of first operand
  ptr_unn op2: I/O [val] Values of second operand on input, values of sum on output */
{
  /* Routine to add value of first operand to value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and be of the specified type. Operands' values are 
     assumed to be in memory already. */

  /* Addition is currently defined as op2:=op1+op2 */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]+=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]+=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]+=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]+=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]+=op1.lp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != *mss_val.lp) && (op1.lp[idx] != *mss_val.lp)) op2.lp[idx]+=op1.lp[idx]; else op2.lp[idx]=*mss_val.lp;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]+=op1.sp[idx];
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_shrt) && (op1.sp[idx] != mss_val_shrt)) op2.sp[idx]+=op1.sp[idx]; else op2.sp[idx]=mss_val_shrt;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_add_no_tally() */

bool
ncar_csm_inq(int nc_id)
/*  
   int nc_id: I netCDF file ID
   bool ncar_csm_inq(): O whether file is an NCAR CSM history tape
*/
{
  /* Routine to check whether file adheres to NCAR CSM history tape format */

  bool NCAR_CSM=False;

  char *att_val;

  char cnv_sng[]="Conventions"; /* Unidata standard string */
  
  int rcd; /* [rcd] Return code */
  
  long att_sz;

  nc_type att_typ;

  /* Look for signature of an NCAR CSM format file */
  rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);

  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NULL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    if(strstr(att_val,"NCAR-CSM") != NULL) NCAR_CSM=True;
    if(NCAR_CSM) (void)fprintf(stderr,"%s: CONVENTION File convention is %s\n",prg_nm_get(),att_val);
    att_val=nco_free(att_val);
  } /* endif */

  return NCAR_CSM;
  
} /* end ncar_csm_inq */

void
ncar_csm_date(int nc_id,var_sct **var,int nbr_var)
     /*  
	int nc_id: I netCDF file ID
	var_sct **var: I list of pointers to variable structures
	int nbr_var: I number of structures in variable structure list
	*/
{
  /* Routine to fix date variable in averaged CSM files */
  char wrn_sng[1000];

  int day;
  int date_idx;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int time_idx;
  
  int nbdate_id;
  
  nco_long nbdate;
  nco_long date;
  
  (void)sprintf(wrn_sng,"Most, but not all, CSM files which are in CCM format contain the fields \"nbdate\", \"time\", and \"date\". When the \"date\" field is present but either \"nbdate\" or \"time\" is missing, then %s is unable to construct a meaningful average \"date\" to store in the output file. Therefore the \"date\" variable in your output file may be meaningless.\n",prg_nm_get());

  /* Find the date variable (NC_INT: current date as 6 digit integer (YYMMDD)) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"date")) break;
  } /* end loop over idx */
  if(idx == nbr_var) return; else date_idx=idx;
  if(var[date_idx]->type != NC_INT) return;
  
  /* Find scalar nbdate variable (NC_INT: base date date as 6 digit integer (YYMMDD)) */
  rcd=nco_inq_varid_flg(nc_id,"nbdate",&nbdate_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING NCAR CSM convention file output variable list contains \"date\" but not \"nbdate\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  } /* endif */
  (void)nco_get_var1(nc_id,nbdate_id,0L,&nbdate,NC_INT);
  
  /* Find time variable (NC_DOUBLE: current day) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"time")) break;
  } /* end loop over idx */
  if(idx == nbr_var){
    (void)fprintf(stderr,"%s: WARNING NCAR CSM convention file output variable list contains \"date\" but not \"time\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  }else{
    time_idx=idx;
  } /* endif */
  
  /* Assign the current day to the averaged day number */
  day=(int)(var[time_idx]->val.dp[0]);
  
  /* Recompute the date variable based on the new (averaged) day number */
#ifdef USE_FORTRAN_ARITHMETIC
  date=FORTRAN_newdate(&nbdate,&day);
#else /* !USE_FORTRAN_ARITHMETIC */
  date=newdate(nbdate,day);
#endif /* !USE_FORTRAN_ARITHMETIC */
  if(var[date_idx]->val.lp != NULL) return; else var[date_idx]->val.lp[0]=date;
  
} /* end ncar_csm_date */

double
arm_time_mk(int nc_id,double time_offset)
     /*  
	int nc_id: I netCDF file ID
	double time_offset: I current
	double arm_time_mk: O base_time + current time_offset
	*/
{
  /* Routine to return the time corresponding to the current time offset */
  double arm_time;

  int base_time_id;
  int rcd=NC_NOERR; /* [rcd] Return code */

  nco_long base_time;

  /* Find the base_time variable (NC_INT: base UNIX time of file) */
  rcd=nco_inq_varid_flg(nc_id,"base_time",&base_time_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"base_time\", exiting arm_time_mk()...\n",prg_nm_get());
    return -1;
  } /* end if */
  (void)nco_get_var1(nc_id,base_time_id,0L,&base_time,NC_INT);
  arm_time=base_time+time_offset;

  return arm_time;
} /* end arm_time_mk() */

void
arm_time_install(int nc_id,nco_long base_time_srt)
     /*  
	 int nc_id: I netCDF file ID
	 nco_long base_time_srt: I base_time of first input file
	 */
{
  /* Routine to add time variable to concatenated ARM files */

  char att_units[]="seconds since 1970/01/01 00:00:00.00";
  char att_long_name[]="UNIX time";

  double *time_offset;

  int rcd=NC_NOERR; /* [rcd] Return code */
  int time_id;
  int time_dmn_id;
  int time_offset_id;
  
  long idx;
  long srt=0L;
  long cnt;

  /* Synchronize output file */
  (void)nco_sync(nc_id);

  /* Find time_offset variable */
  rcd=nco_inq_varid_flg(nc_id,"time_offset",&time_offset_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"time_offset\", exiting arm_time_install()...\n",prg_nm_get());
    return;
  } /* endif */

  /* See if time variable already exists */
  rcd=nco_inq_varid_flg(nc_id,"time",&time_id);
  if(rcd == NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file already has variable \"time\"\n",prg_nm_get());
    return;
  } /* endif */

  /* See if time dimension exists */
  rcd=nco_inq_dimid_flg(nc_id,"time",&time_dmn_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have dimension \"time\"\n",prg_nm_get());
    return;
  } /* endif */
  /* Get dimension size */
  (void)nco_inq_dimlen(nc_id,time_dmn_id,&cnt);

  /* If the time coordinate does not already exist, create it */
  time_offset=(double *)nco_malloc(cnt*nco_typ_lng(NC_DOUBLE));

  (void)nco_get_vara(nc_id,time_offset_id,&srt,&cnt,(void *)time_offset,NC_DOUBLE);
  for(idx=0L;idx<cnt;idx++) time_offset[idx]+=base_time_srt;

  /* File must be in define mode */
  (void)nco_redef(nc_id);
  (void)nco_def_var(nc_id,"time",NC_DOUBLE,1,&time_dmn_id,&time_id);

  /* Add attributes for time variable */
  (void)nco_put_att(nc_id,time_id,"units",NC_CHAR,strlen(att_units)+1,(void *)att_units);
  (void)nco_put_att(nc_id,time_id,"long_name",NC_CHAR,strlen(att_long_name)+1,(void *)att_long_name);

  /* Catenate time-stamped reminder onto "history" global attribute */
  (void)hst_att_cat(nc_id,"ncrcat added variable time=base_time+time_offset");

  /* Take file out of define mode */
  (void)nco_enddef(nc_id);

  /* Write time variable */
  (void)nco_put_vara(nc_id,time_id,&srt,&cnt,(void *)time_offset,NC_DOUBLE);

  /* Free time_offset buffer */
  time_offset=nco_free(time_offset);

} /* end arm_time_install */

void
var_lst_convert(int nc_id,nm_id_sct *xtr_lst,int nbr_xtr,dmn_sct **dim,int nbr_dmn_xtr,
	   var_sct ***var_ptr,var_sct ***var_out_ptr)
/*  
   int nc_id: I netCDF file ID
   nm_id_sct *xtr_lst: I current extraction list (destroyed)
   int nbr_xtr: I total number of variables in input file
   dmn_sct **dim: I list of pointers to dimension structures associated with input variable list
   int nbr_dmn_xtr: I number of dimensions structures in list
   var_sct ***var_ptr: O pointer to list of pointers to variable structures
   var_sct ***var_out_ptr: O pointer to list of pointers to duplicates of variable structures
*/
{
  /* Routine to make a var_sct list from a nm_id list. The var_sct lst is duplicated 
     to be used for an output list. */

  int idx;

  var_sct **var;
  var_sct **var_out;

  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));

  /* Fill in variable structure list for all extracted variables */
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=var_fll(nc_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=var_dpl(var[idx]);
    (void)var_xrf(var[idx],var_out[idx]);
    (void)var_dmn_xrf(var_out[idx]);
  } /* end loop over idx */
  
  *var_ptr=var;
  *var_out_ptr=var_out;

} /* end var_lst_convert */

void
var_lst_divide(var_sct **var,var_sct **var_out,int nbr_var,bool NCAR_CSM_FORMAT,
	       dmn_sct **dmn_xcl,int nbr_dmn_xcl,
	       var_sct ***var_fix_ptr,var_sct ***var_fix_out_ptr,int *nbr_var_fix,
	       var_sct ***var_prc_ptr,var_sct ***var_prc_out_ptr,int *nbr_var_prc)
/*  
   var_sct **var: I list of pointers to variable structures
   var_sct **var_out: I list of pointers to duplicates of variable structures
   int nbr_var: I number of variable structures in list
   dmn_sct **dmn_xcl: I list of pointers to dimensions not allowed in fixed variables
   int nbr_dmn_xcl: I number of dimension structures in list
   bool NCAR_CSM_FORMAT: I whether file is an NCAR CSM history tape
   var_sct ***var_fix_ptr: O pointer to list of pointers to fixed-variable structures
   var_sct ***var_fix_out_ptr: O pointer to list of pointers to duplicates of fixed-variable structures
   int *nbr_var_fix: O number of variable structures in list of fixed variables
   var_sct ***var_prc_ptr: O pointer to list of pointers to processed-variable structures
   var_sct ***var_prc_out_ptr: O pointer to list of pointers to duplicates of processed-variable structures
   int *nbr_var_prc: O number of variable structures in list of processed variables
*/
{
  /* Routine to divide the two input lists of variables into four separate output lists, 
     based on the program type */

  char *var_nm=NULL_CEWI;

  int idx;
  int prg; /* Program key */

  enum op_typ{
    fix, /* 0 */
    prc /* 1 */
  };

  int idx_dmn;
  int idx_xcl;
  int var_op_typ[NC_MAX_VARS];

  nc_type var_type=NC_NAT; /* NC_NAT present in netcdf.h version netCDF 3.5+ */

  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_prc;
  var_sct **var_prc_out;

  prg=prg_get(); /* Program key */

  /* Allocate space for too many structures, then realloc() at the end, to avoid duplication. */
  var_fix=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_fix_out=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_prc=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_prc_out=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));

  /* Find operation type for each variable: for now this is either fix or prc */
  for(idx=0;idx<nbr_var;idx++){
    
    /* Initialize operation type */
    var_op_typ[idx]=prc;
    var_nm=var[idx]->nm;
    var_type=var[idx]->type;

    /* Override operation type based depending on both variable type and program */
    switch(prg){
    case ncap:
      var_op_typ[idx]=fix;
      break;
    case ncra:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncea:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_typ[idx]=fix;
      break;
    case ncdiff:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_typ[idx]=fix;
      break;
    case ncflint:
      if((var_type == NC_CHAR) || (var_type == NC_BYTE) || (var[idx]->is_crd_var && !var[idx]->is_rec_var)) var_op_typ[idx]=fix;
      break;
    case ncwa:
      /* Process every variable containing an excluded (averaged) dimension */
      for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	for(idx_xcl=0;idx_xcl<nbr_dmn_xcl;idx_xcl++){
	  if(var[idx]->dim[idx_dmn]->id == dmn_xcl[idx_xcl]->id) break;
	} /* end loop over idx_xcl */
	if(idx_xcl != nbr_dmn_xcl){
	  var_op_typ[idx]=prc;
	  break;
	} /* end if */
      } /* end loop over idx_dmn */
      /* Variables which do not contain an excluded (averaged) dimension must be fixed */
      if(idx_dmn == var[idx]->nbr_dim) var_op_typ[idx]=fix;
      break;
    case ncrcat:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncecat:
      if(var[idx]->is_crd_var) var_op_typ[idx]=fix;
      break;
    } /* end switch */
    
    if(NCAR_CSM_FORMAT){
      if(!strcmp(var_nm,"ntrm") || !strcmp(var_nm,"ntrn") || !strcmp(var_nm,"ntrk") || !strcmp(var_nm,"ndbase") || !strcmp(var_nm,"nsbase") || !strcmp(var_nm,"nbdate") || !strcmp(var_nm,"nbsec") || !strcmp(var_nm,"mdt") || !strcmp(var_nm,"mhisf")) var_op_typ[idx]=fix;
      if(prg == ncdiff && (!strcmp(var_nm,"hyam") || !strcmp(var_nm,"hybm") || !strcmp(var_nm,"hyai") || !strcmp(var_nm,"hybi") || !strcmp(var_nm,"gw") || !strcmp(var_nm,"ORO") || !strcmp(var_nm,"date") || !strcmp(var_nm,"datesec"))) var_op_typ[idx]=fix;
    } /* end if NCAR_CSM_FORMAT */

  } /* end loop over var */

  /* Assign list pointers based on operation type for each variable */
  *nbr_var_prc=*nbr_var_fix=0;
  for(idx=0;idx<nbr_var;idx++){
    if(var_op_typ[idx] == fix){
      var_fix[*nbr_var_fix]=var[idx];
      var_fix_out[*nbr_var_fix]=var_out[idx];
      ++*nbr_var_fix;
    }else{
      var_prc[*nbr_var_prc]=var[idx];
      var_prc_out[*nbr_var_prc]=var_out[idx];
      ++*nbr_var_prc;
/*      if(var[idx]->type != NC_FLOAT && var[idx]->type != NC_DOUBLE && prg != ncdiff && prg != ncrcat){
	(void)fprintf(stderr,"%s: WARNING variable \"%s\" will be coerced from %s to NC_FLOAT\n",prg_nm_get(),var[idx]->nm,nco_typ_sng(var[idx]->type));
	var_out[idx]->type=NC_FLOAT;
      } *//* end if */
      if(((var[idx]->type == NC_CHAR) || (var[idx]->type == NC_BYTE)) && ((prg != ncrcat) && (prg != ncecat))){
	(void)fprintf(stderr,"%s: WARNING Variable %s is of type %s, for which processing (i.e., averaging, differencing) is ill-defined\n",prg_nm_get(),var[idx]->nm,nco_typ_sng(var[idx]->type));
      } /* end if */
    } /* end else */
  } /* end loop over var */
  
  /* Sanity check */
  if(*nbr_var_prc+*nbr_var_fix != nbr_var){
    (void)fprintf(stdout,"%s: ERROR nbr_var_prc+nbr_var_fix != nbr_var\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* end if */

  /* fxm: Remove ncap exception when finished with ncap list processing */
  if(*nbr_var_prc==0 && prg != ncap){
    (void)fprintf(stdout,"%s: ERROR no variables fit criteria for processing\n",prg_nm_get());
    switch(prg_get()){
    case ncap:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain some derived fields\n",prg_nm_get());
    case ncra:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain record variables that are not NC_CHAR or NC_BYTE in order to perform a running average\n",prg_nm_get());
      break;
    case ncea:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables that are not NC_CHAR or NC_BYTE in order to perform an ensemble average\n",prg_nm_get());
      break;
    case ncdiff:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables that are not NC_CHAR or NC_BYTE in order to subtract\n",prg_nm_get());
      break;
    case ncflint:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain variables that are not NC_CHAR or NC_BYTE in order to interpolate\n",prg_nm_get());
      break;
    case ncwa:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain variables that contain an averaging dimension in order to perform an average\n",prg_nm_get());
      break;
    case ncrcat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain record variables in order to perform a record concatenation\n",prg_nm_get());
      break;
    case ncecat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables in order to perform an ensemble concatenation\n",prg_nm_get());
      break;
    } /* end switch */
    exit(EXIT_FAILURE);
  } /* end if */

  /* Free unused space and save pointers in output variables */
  if(*nbr_var_fix > 0) *var_fix_ptr=(var_sct **)nco_realloc(var_fix,*nbr_var_fix*sizeof(var_sct *)); else *var_fix_ptr=NULL;
  if(*nbr_var_fix > 0) *var_fix_out_ptr=(var_sct **)nco_realloc(var_fix_out,*nbr_var_fix*sizeof(var_sct *)); else *var_fix_out_ptr=NULL;
  if(*nbr_var_prc > 0) *var_prc_ptr=(var_sct **)nco_realloc(var_prc,*nbr_var_prc*sizeof(var_sct *)); else *var_prc_ptr=NULL;
  if(*nbr_var_prc > 0) *var_prc_out_ptr=(var_sct **)nco_realloc(var_prc_out,*nbr_var_prc*sizeof(var_sct *)); else *var_prc_out_ptr=NULL;

} /* end var_lst_divide */

nm_id_sct *
dmn_lst_mk(int nc_id,char **dmn_lst_in,int nbr_dim)
/* 
   int nc_id: I netCDF file ID
   char **dmn_lst_in: user specified list of dimension names
   int nbr_dim: I total number of dimensions in lst
   nm_id_sct dmn_lst_mk(): O dimension list
 */
{
  int idx;

  nm_id_sct *dmn_lst;
  
  dmn_lst=(nm_id_sct *)nco_malloc(nbr_dim*sizeof(nm_id_sct));
  for(idx=0;idx<nbr_dim;idx++){
    /* See if requested dimension is in input file */
    dmn_lst[idx].nm=dmn_lst_in[idx];
    (void)nco_inq_dimid(nc_id,dmn_lst[idx].nm,&dmn_lst[idx].id);
  } /* end loop over idx */
  
  return dmn_lst;
} /* end dmn_lst_mk() */

void
rec_crd_chk(var_sct *var,char *fl_in,char *fl_out,long idx_rec,long idx_rec_out)
/* 
   var_sct *var: I variable structure of coordinate to check for monotinicity
   char *fl_in: I current input filename
   char *fl_out: I current output filename
   int idx_rec: I current index or record coordinate in input file
   int idx_rec_out: I current index or record coordinate in output file
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Check for monotonicity of coordinate values */

  enum monotonic_direction{
    decreasing, /* 0 */
    increasing}; /* 1 */

  static double rec_crd_val_lst;
  static double rec_crd_val_crr;

  static int monotonic_direction;

  /* Use implicit type conversion */
  switch(var->type){
  case NC_FLOAT: rec_crd_val_crr=var->val.fp[0]; break; 
  case NC_DOUBLE: rec_crd_val_crr=var->val.dp[0]; break; 
  case NC_INT: rec_crd_val_crr=var->val.lp[0]; break;
  case NC_SHORT: rec_crd_val_crr=var->val.sp[0]; break;
  case NC_CHAR: rec_crd_val_crr=var->val.cp[0]; break;
  case NC_BYTE: rec_crd_val_crr=var->val.bp[0]; break;
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  if(idx_rec_out > 1){
    if(((rec_crd_val_crr > rec_crd_val_lst) && monotonic_direction == decreasing) ||
       ((rec_crd_val_crr < rec_crd_val_lst) && monotonic_direction == increasing))
      (void)fprintf(stderr,"%s: WARNING Record coordinate \"%s\" does not monotonically %s between (input file %s record indices: %ld, %ld) (output file %s record indices %ld, %ld) record coordinate values %f, %f\n",prg_nm_get(),var->nm,(monotonic_direction == decreasing ? "decrease" : "increase"),fl_in,idx_rec-1,idx_rec,fl_out,idx_rec_out-1,idx_rec_out,rec_crd_val_lst,rec_crd_val_crr);
  }else if(idx_rec_out == 1){
    if(rec_crd_val_crr > rec_crd_val_lst) monotonic_direction=increasing; else monotonic_direction=decreasing;
  } /* end if */
    
  rec_crd_val_lst=rec_crd_val_crr;

} /* end rec_crd_chk() */

char **
fl_lst_mk(char **argv,int argc,int arg_crr,int *nbr_fl,char **fl_out)
/* 
   char **argv: I argument list
   int argc: I argument count
   int arg_crr: I index of current argument
   int *nbr_fl: O number of files in input file list
   char **fl_out: O name of output file
   char **fl_lst_mk(): O list of user-specified filenames
 */
{
  /* Routine to parse the positional arguments on the command line.
     The name of the calling program plays a role in this. */

  /* Command-line switches are expected to have been digested already (e.g., by getopt()),
     and argv[arg_crr] points to the first argument on the command line following all the
     switches. */

  char **fl_lst_in;

  int idx;
  int fl_nm_sz_warning=80;

  /* Are there any remaining arguments that could be filenames? */
  if(arg_crr >= argc){
    (void)fprintf(stdout,"%s: ERROR must specify filename(s)\n",prg_nm_get());
    (void)usg_prn();
    exit(EXIT_FAILURE);
  } /* end if */

  /* See if there appear to be problems with any of the specified files */
  for(idx=arg_crr;idx<argc;idx++){
    if((int)strlen(argv[idx]) >= fl_nm_sz_warning) (void)fprintf(stderr,"%s: WARNING filename %s is very long (%ld characters)\n",prg_nm_get(),argv[idx],(long)strlen(argv[idx]));
  } /* end loop over idx */

  switch(prg_get()){
  case ncks:
  case ncatted:
  case ncrename:
    if(argc-arg_crr > 2){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need no more than 2\n",prg_nm_get(),argc-arg_crr);
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end if */
    fl_lst_in=(char **)nco_malloc(sizeof(char *));
    fl_lst_in[(*nbr_fl)++]=argv[arg_crr++];
    if(arg_crr == argc-1) *fl_out=argv[arg_crr]; else *fl_out=NULL;
    return fl_lst_in;
    /*    break;*//* NB: putting break after return in case statement causes warning on SGI cc */
  case ncdiff:
  case ncflint:
    if(argc-arg_crr != 3){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly 3\n",prg_nm_get(),argc-arg_crr);
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncap:
  case ncwa:
    if(argc-arg_crr != 2){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly 2\n",prg_nm_get(),argc-arg_crr);
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncra:
  case ncea:
  case ncrcat:
  case ncecat:
    if(argc-arg_crr < 2){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need at least 2\n",prg_nm_get(),argc-arg_crr);
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end if */
    break;
  default:
    break;
  } /* end switch */

  /* Fill in file list and output file */
  fl_lst_in=(char **)nco_malloc((argc-arg_crr-1)*sizeof(char *));
  while(arg_crr < argc-1) fl_lst_in[(*nbr_fl)++]=argv[arg_crr++];
  if(*nbr_fl == 0){
    (void)fprintf(stdout,"%s: ERROR Must specify input filename.\n",prg_nm_get());
    (void)usg_prn();
    exit(EXIT_FAILURE);
  } /* end if */
  *fl_out=argv[argc-1];

  return fl_lst_in;

} /* end fl_lst_mk() */

char *
prg_prs(char *nm_in,int *prg)
/* 
   char *nm_in: I name of program to categorize, e.g., argv[0] (might include a path prefix)
   int *prg: O enumeration number corresponding to nm_in
   char *prg_prs(): O nm_in stripped of any path (i.e., program name stub)
 */
{
  /* Routine to set program name and enum */

  char *nm_out;

  /* Get program name (use strrchr() first in case nm_in contains a path) */
  if((nm_out=strrchr(nm_in,'/')) == NULL) nm_out=nm_in; else nm_out++;

  /* Classify calling program */
  if(!strcmp(nm_out,"ncra")){*prg=ncra;
  }else if(!strcmp(nm_out,"ncap")){*prg=ncap;
  }else if(!strcmp(nm_out,"ncea")){*prg=ncea;
  }else if(!strcmp(nm_out,"ncdiff")){*prg=ncdiff;
  }else if(!strcmp(nm_out,"ncflint")){*prg=ncflint;
  }else if(!strcmp(nm_out,"ncwa")){*prg=ncwa;
  }else if(!strcmp(nm_out,"ncrcat")){*prg=ncrcat;
  }else if(!strcmp(nm_out,"ncecat")){*prg=ncecat;
  }else if(!strcmp(nm_out,"ncks")){*prg=ncks;
  }else if(!strcmp(nm_out,"ncrename")){*prg=ncrename;
  }else if(!strcmp(nm_out,"ncatted")){*prg=ncatted;
  }else{
    (void)fprintf(stdout,"%s: ERROR %s not registered in prg_prs()\n",nm_out,nm_out);
    exit(EXIT_FAILURE);
  } /* end else */

  return nm_out;

} /* end prg_prs() */

void
err_prn(char *err_msg)
/* 
   char *err_msg: I the formatted error message to print
 */
{
  /* Routine to print an error message (currently only to stdout) approximately in GNU style,
     i.e., "program_name: ERROR error message....\n". This routine is intended to make error
     messages convenient to use in routines without the routine itself needing access to a copy
     of program. Therefore a copy of this routine should be placed in each main.c file, just
     like the program's usg_prn() routine. */

  /* This routine is more difficult to implement practically than it first seemed, because 
     it assumes all the calling routines have their error messages in a single string, when
     in reality, they often utilize the var_args capabilities of fprintf() themselves and
     have multiple formatted inputs. Better hold off for now. */

  (void)fprintf(stdout,"%s: %s",prg_nm_get(),err_msg);

} /* end err_prn() */

void 
usg_prn(void)
{
  /* Purpose: Print correct command-line usage of host program (currently to stdout) */

  char *opt_sng=NULL_CEWI;

  int prg;

  prg=prg_get();

  switch(prg){
  case ncra:
  case ncea:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-n ...] [-O] [-p path] [-R] [-r] [-v ...] [-x] [-y op_typ] in.nc [...] out.nc\n");
    break;
  case ncrcat:
  case ncecat:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-n ...] [-O] [-p path] [-R] [-r] [-v ...] [-x] in.nc [...] out.nc\n");
    break;
  case ncdiff:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-n ...] [-O] [-p path] [-R] [-r] [-v ...] [-x] in_1.nc in_2.nc out.nc\n");
    break;
  case ncflint:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-i var,val] [-l path] [-n ...] [-O] [-p path] [-R] [-r] [-v ...] [-x] [-w wgt_1[,wgt_2]] in_1.nc in_2.nc out.nc\n");
    break;
  case ncwa:
    opt_sng=(char *)strdup("[-A] [-a ...] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-I] [-l path] [-m mask] [-M val] [-N] [-O] [-o op_typ] [-p path] [-R] [-r] [-v ...] [-w wgt] [-x] [-y op_typ] in.nc out.nc\n");
    break;
  case ncap:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-O] [-p path] [-R] [-r] [-s algebra] [-S fl.nco] [-v] in.nc out.nc\n");
    break;
  case ncks:
    opt_sng=(char *)strdup("[-A] [-a] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-m] [-O] [-p path] [-q] [-R] [-r] [-s format] [-u] [-v ...] [-x] in.nc [out.nc]\n");
    break;
  case ncatted:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-h] [-l path] [-O] [-p path] [-R] [-r] in.nc [out.nc]\n");
    break;
  case ncrename:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-d ...] [-h] [-l path] [-O] [-p path] [-R] [-r] [-v ...] in.nc [out.nc]\n");
    break;
  } /* end switch */
  
  /* Public service announcements */
  (void)fprintf(stdout,"NCO homepage at http://nco.sourceforge.net has complete online User's Guide\n");
  (void)fprintf(stdout,"Post questions, suggestions, patches at http://sourceforge.net/projects/nco\n");

  /* We now have command-specific command line option string */
  (void)fprintf(stdout,"%s %s\n",prg_nm_get(),opt_sng);

  if(strstr(opt_sng,"-A")) (void)fprintf(stdout,"-A\t\tAppend to existing output file, if any\n");
  if(strstr(opt_sng,"-a")){
    if(prg == ncrename) (void)fprintf(stdout,"-a old_att,new_att Attribute's old and new names\n");
    if(prg == ncwa) (void)fprintf(stdout,"-a avg_dim1[,avg_dim2[...]] Averaging dimensions\n");
    if(prg == ncks) (void)fprintf(stdout,"-a\t\tDisable alphabetization of extracted variables\n");
    if(prg == ncatted) (void)fprintf(stdout,"-a att_nm,var_nm,mode,att_typ,att_val Attribute specification:\n\t\tmode = a,c,d,m,o and att_typ = f,d,l,s,c,b\n");
  } /* end if */
  if(strstr(opt_sng,"-c")) (void)fprintf(stdout,"-c\t\tCoordinate variables will all be processed\n");
  if(strstr(opt_sng,"-C")) (void)fprintf(stdout,"-C\t\tAssociated coordinate variables should not be processed\n");
  if(strstr(opt_sng,"-D")) (void)fprintf(stdout,"-D dbg_lvl\tDebugging level\n");
  if(strstr(opt_sng,"-d")){
    if(prg == ncrename) (void)fprintf(stdout,"-d old_dim,new_dim Dimension's old and new names\n");
    else if(prg == ncks) (void)fprintf(stdout,"-d dim,[min][,[max]][,[stride]] Dimension's limits and stride in hyperslab\n");
    else if(prg == ncra || prg == ncrcat) (void)fprintf(stdout,"-d dim,[min][,[max]][,[stride]] Dimension's limits (any dimension) and stride (record dimension only) in hyperslab\n");
    else (void)fprintf(stdout,"-d dim,[min][,[max]] Dimension's limits in hyperslab\n");
  } /* end if -d */
  if(strstr(opt_sng,"-F")) (void)fprintf(stdout,"-F\t\tFortran indexing conventions (1-based) for I/O\n");
  if(strstr(opt_sng,"-H")) (void)fprintf(stdout,"-H\t\tPrint data\n");
  if(strstr(opt_sng,"-h")){
    if(prg == ncatted) (void)fprintf(stdout,"-h\t\tDo not append to \"history\" global attribute\n");
  } /* end if */
  if(strstr(opt_sng,"-i")) (void)fprintf(stdout,"-i var,val\tInterpolant and value\n");
  if(strstr(opt_sng,"-I")) (void)fprintf(stdout,"-I \t\tDo not weight or mask coordinate variables\n");
  if(strstr(opt_sng,"-l")) (void)fprintf(stdout,"-l path\t\tLocal storage path for remotely-retrieved files\n");
  if(strstr(opt_sng,"-M")){
    if(prg == ncwa) (void)fprintf(stdout,"-M val\t\tMasking value (default is 1.0)\n");
    if(prg == ncks) (void)fprintf(stdout,"-M\t\tPrint global metadata\n");
  } /* end if */
  if(strstr(opt_sng,"-m")){
    if(prg == ncwa) (void)fprintf(stdout,"-m mask\t\tMasking variable name\n");
    if(prg == ncks) (void)fprintf(stdout,"-m\t\tPrint variable metadata\n");
  } /* end if */
  if(strstr(opt_sng,"-N")) (void)fprintf(stdout,"-N\t\tNo normalization\n");
  if(strstr(opt_sng,"-n")){
    /*    if(prg == ncwa) (void)fprintf(stdout,"-n\t\tNormalize by tally but not weight\n");*/
    if(prg != ncwa) (void)fprintf(stdout,"-n nbr_files,[nbr_numeric_chars[,increment]] NINTAP-style abbreviation of file list\n");
  } /* end if */
  if(strstr(opt_sng,"-o")) (void)fprintf(stdout,"-o op_typ\tRelational operator for masking: eq,ne,ge,le,gt,lt\n");
  if(strstr(opt_sng,"-O")) (void)fprintf(stdout,"-O\t\tOverwrite existing output file, if any\n");
  if(strstr(opt_sng,"-p")) (void)fprintf(stdout,"-p path\t\tPath prefix for all input filenames\n");
  if(strstr(opt_sng,"-q")) (void)fprintf(stdout,"-q\t\tToggle printing of dimension indices and coordinate values\n");
  if(strstr(opt_sng,"-R")) (void)fprintf(stdout,"-R\t\tRetain remotely-retrieved files after use\n");
  if(strstr(opt_sng,"-r")) (void)fprintf(stdout,"-r\t\tProgram version and copyright notice\n");
  if(strstr(opt_sng,"-s")){
    if(prg != ncap) (void)fprintf(stdout,"-s format\tString format for text output\n");
    if(prg == ncap) (void)fprintf(stdout,"-s algebra\tAlgebraic command defining single output variable\n");
  } /* end if */
  if(strstr(opt_sng,"-S")) (void)fprintf(stdout,"-S fl.nco\tScript file containing multiple algebraic commands\n");
  if(strstr(opt_sng,"-u")) (void)fprintf(stdout,"-u\t\tUnits of variables, if any, will be printed\n");
  if(strstr(opt_sng,"-v")){
    if(prg == ncrename) (void)fprintf(stdout,"-v old_var,new_var Variable's old and new names\n");
	if(prg == ncap) (void)fprintf(stdout,"-v\t\tOutput file includes ONLY user-defined variables\n");
    if(prg != ncrename && prg != ncap) (void)fprintf(stdout,"-v var1[,var2[...]] Variables to process\n");
  } /* end if */
  /*  if(strstr(opt_sng,"-W")) (void)fprintf(stdout,"-W\t\tNormalize by weight but not tally\n");*/
  if(strstr(opt_sng,"-w")){
    if(prg == ncwa) (void)fprintf(stdout,"-w wgt\t\tWeighting variable name\n");
    if(prg == ncflint) (void)fprintf(stdout,"-w wgt_1[,wgt_2] Weight(s) of file(s)\n");
  } /* end if */
  if(strstr(opt_sng,"-x")) (void)fprintf(stdout,"-x\t\tExtract all variables EXCEPT those specified with -v\n");
  if(strstr(opt_sng,"-y")) (void)fprintf(stdout,"-y op_typ\tArithmetic operation: avg,min,max,ttl,sqravg,avgsqr,sqrt,rms,rmssdn\n");
  if(strstr(opt_sng,"in.nc")) (void)fprintf(stdout,"in.nc\t\tInput file name(s)\n");
  if(strstr(opt_sng,"out.nc")) (void)fprintf(stdout,"out.nc\t\tOutput file name\n");
/*  if(strstr(opt_sng,"-")) (void)fprintf(stdout,"-\n");*/

  /* Free the space holding the string */
  opt_sng=nco_free(opt_sng);

} /* end usg_prn() */

int
nco_op_typ_get(char *nco_op_sng)
{
  /* Purpose: Process '-y' command line argument
     Convert user-specified string to enumerated operation type 
     Return nco_op_avg by default */
  if(!strcmp(nco_op_sng,"min")) return nco_op_min;
  if(!strcmp(nco_op_sng,"max")) return nco_op_max;
  if(!strcmp(nco_op_sng,"total") || !strcmp(nco_op_sng,"ttl")) return nco_op_ttl;
  if(!strcmp(nco_op_sng,"sqravg")) return nco_op_sqravg;
  if(!strcmp(nco_op_sng,"avgsqr")) return nco_op_avgsqr;  
  if(!strcmp(nco_op_sng,"sqrt")) return nco_op_sqrt;
  if(!strcmp(nco_op_sng,"rms")) return nco_op_rms;
  if(!strcmp(nco_op_sng,"rmssdn")) return nco_op_rmssdn;

  return nco_op_avg;
} /* end nco_op_typ_get() */

int
nco_pck_typ_get(char *nco_pck_sng)
{
  /* Purpose: Process '-P' command line argument
     Convert user-specified string to packing operation type 
     Return nco_pck_nil by default */
  /* fxm: add the rest of types */
  if(!strcmp(nco_pck_sng,"all")) return nco_pck_all_xst_att;

  return nco_pck_nil;
} /* end nco_pck_typ_get() */

int
op_prs_rlt(char *op_sng)
/* 
   char *op_sng: I string containing Fortran representation of a reltional operator ("eq","lt"...)
 */
{
  /* Routine to parse the Fortran abbreviation for a relational operator into a unique numeric value
     representing that relation */

  /* Classify the relation */
  if(!strcmp(op_sng,"eq")){
    return nco_op_eq;
  }else if(!strcmp(op_sng,"ne")){
    return nco_op_ne;
  }else if(!strcmp(op_sng,"lt")){
    return nco_op_lt;
  }else if(!strcmp(op_sng,"gt")){
    return nco_op_gt;
  }else if(!strcmp(op_sng,"le")){
    return nco_op_le;
  }else if(!strcmp(op_sng,"ge")){
    return nco_op_ge;
  }else{
    (void)fprintf(stdout,"%s: ERROR %s not registered in op_prs_rlt()\n",prg_nm_get(),op_sng);
    exit(EXIT_FAILURE);
  } /* end else */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return 1;
} /* end op_prs_rlt() */

int nd2endm(int mth,int day)
{
  /* Purpose: Returns number of days to the end of the month  
     This number added to the input arguement day gives the last day of month mth
     Original fortran: Brian Eaton cal_util.F:nd2endm()
     C version: Charlie Zender
 */
  int nbr_day_2_mth_end;
  int mdays[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if(mth < 1 || mth > 12 || day < 0){
    (void)fprintf(stdout,"%s: ERROR nd2endm() reports mth = %d, day = %d\n",prg_nm_get(),mth,day);
    exit(EXIT_FAILURE);
  } /* end if */

  nbr_day_2_mth_end=mdays[mth-1]-day;

  return nbr_day_2_mth_end;
} /* nd2endm */

nco_long newdate(nco_long date,int day_srt) 
{
  /* Purpose: Find date a specified number of days (possibly negative) from given date 
     Original fortran: Brian Eaton cal_util.F:newdate()
     C version: Charlie Zender
  */
  /* Local */
  int date_srt; /* Initial value of date (may change sign) */
  int day_nbr_2_eom; /* Days to end of month */
  int day_crr; /* Day of date */
  int day_ncr; /* Running count of days to increment date by */
  int mth_crr; /* Month of date */
  int mth_idx; /* Index */
  int mth_srt; /* Save the initial value of month */
  int mth_tmp; /* Current month as we increment date */
  int yr_crr; /* Year of date */
  int mth_day_nbr[]= /* Number of days in each month */
  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  nco_long newdate_YYMMDD; /* New date in YYMMDD format */

  if(day_srt == 0) return date;

  date_srt=date;
  yr_crr=date_srt/10000;
  if(date_srt < 0) date_srt=-date_srt;
  mth_crr=(date_srt%10000)/100;
  mth_srt=mth_crr;
  day_crr=date_srt%100;
      
  if(day_srt > 0){
    day_ncr=day_srt;
    yr_crr+=day_ncr/365;
    day_ncr=day_ncr%365;
    for(mth_idx=mth_srt;mth_idx<=mth_srt+12;mth_idx++){
      mth_tmp=mth_idx;
      if(mth_idx > 12) mth_tmp=mth_idx-12;
      day_nbr_2_eom=nd2endm(mth_tmp,day_crr);
      if(day_ncr > day_nbr_2_eom){
	mth_crr++;
	if(mth_crr > 12){
	  mth_crr=1;
	  yr_crr++;
	} /* end if */
	day_crr=1;
	day_ncr-=day_nbr_2_eom+1;
	if(day_ncr == 0) break;
      }else{
	day_crr=day_crr+day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  }else if(day_srt < 0){
    day_ncr=-day_srt;
    yr_crr=yr_crr-day_ncr/365;
    day_ncr=day_ncr%365;
    mth_srt=mth_crr;
    for(mth_idx=mth_srt+12;mth_idx>=mth_srt;mth_idx--){
      if(day_ncr >= day_crr){
	mth_crr--;
	if(mth_crr < 1){
	  mth_crr=12;
	  yr_crr--;
	} /* end if */
	day_ncr-=day_crr;
	day_crr=mth_day_nbr[mth_crr-1];
	if(day_ncr == 0) break;
      }else{
	day_crr-=day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  } /* end if */

  if(yr_crr >= 0){
    newdate_YYMMDD=yr_crr*10000+mth_crr*100+day_crr;
  }else{
    newdate_YYMMDD=-yr_crr*10000+mth_crr*100+day_crr;
    newdate_YYMMDD=-newdate_YYMMDD;
  } /* end if */

  return newdate_YYMMDD;
} /* end newdate() */

var_sct * /* O [var] Variable after (possible) conversion */
nco_typ_cnv_rth  /* [fnc] Convert char, short, long, int types to doubles before arithmetic */
(var_sct *var, /* I/O [var] Variable to be considered for conversion */
 int nco_op_typ) /* I [enm] Operation type */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Convert char, short, long, int types to doubles for arithmetic
     Conversions are performed unless arithmetic operation type is min or max
     Floats (and doubles, of course) are not converted for performance reason 
     This routine is usually called 
     Remember to convert back after weighting and arithmetic are complete! */

  if(var->type != NC_FLOAT && var->type != NC_DOUBLE && nco_op_typ != nco_op_min && nco_op_typ != nco_op_max) var=var_conform_type(NC_DOUBLE,var);
  
  return var;
} /* nco_typ_cnv_rth() */

var_sct * /* O [sct] Variable reverted to on-disk type */
nco_cnv_var_typ_dsk  /* [fnc] Revert variable to on-disk type */
(var_sct *var) /* I [sct] Variable to be reverted */
{
  /* Purpose: Revert variable to on-disk type */
  
  if(var->type != var->typ_dsk) var=var_conform_type(var->typ_dsk,var);
  
  return var;
} /* nco_cnv_var_typ_dsk() */

int /* [enm] Return code */
var_dfl_set /* [fnc] Set defaults for each member of variable structure */
(var_sct *var){ /* [sct] Pointer to variable strucutre to initialize to defaults */
  /* Purpose: Set defaults for each member of variable structure
     var_dfl_set() should be called by any routine that creates a variable structure */

  int rcd=0; /* [enm] Return code */

  /* Set defaults to be overridden by available information */
  var->nm=NULL;
  var->id=-1;
  var->nc_id=-1;
  var->type=NC_NAT; /* Type of variable in RAM */ /* fxm: should use nc_type enum */
  var->typ_dsk=NC_NAT; /* Type of variable on disk */ /* fxm: should use nc_type enum */
  var->typ_pck=NC_NAT; /* Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  var->typ_upk=NC_NAT; /* Type of variable when unpacked (expanded) (in memory) */
  var->is_rec_var=False;
  var->is_crd_var=False;
  /* Size of 1 is assumed in var_fll() */
  var->sz=1L;
  var->sz_rec=1L;
  var->cid=-1;
  var->has_mss_val=False;
  var->mss_val.vp=NULL;
  var->val.vp=NULL;
  var->tally=NULL;
  var->xrf=NULL;
  var->nbr_dim=-1;
  var->nbr_att=-1;
  var->dim=(dmn_sct **)NULL;
  var->dmn_id=(int *)NULL;
  var->cnt=(long *)NULL;
  var->srt=(long *)NULL;
  var->end=(long *)NULL;
  var->srd=(long *)NULL;

  /* Members related to packing */
  var->has_scl_fct=False; /* [flg] Valid scale_factor attribute exists */
  var->has_add_fst=False; /* [flg] Valid add_offset attribute exists */
  var->pck_dsk=False; /* [flg] Variable is packed on disk */
  var->pck_ram=False; /* [flg] Variable is packed in memory */
  var->scl_fct.vp=NULL; /* [ptr] Value of scale_factor attribute, if any */
  var->add_fst.vp=NULL; /* [ptr] Value of add_offset attribute, if any */

  return rcd; /* [enm] Return code */
} /* end var_dfl_set() */

var_sct *
scl_dbl_mk_var(double val)
/* double val: I double precision value to turn into netCDF variable
   scl_dbl_mk_var: O netCDF variable structure representing val */
{
  /* Purpose: Turn a scalar double into a netCDF variable
     Routine duplicates most functions of var_fll() 
     Both functions should share as much initialization code as possible */

  static char *var_nm="Internally generated variable";

  var_sct *var;

  var=(var_sct *)nco_malloc(sizeof(var_sct));

  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */

  /* Overwrite defaults with values appropriate for artificial variable */
  var->nm=(char *)strdup(var_nm);
  var->nbr_dim=0;
  var->type=NC_DOUBLE;
  var->val.vp=(void *)nco_malloc(nco_typ_lng(var->type));
  (void)memcpy((void *)var->val.vp,(void *)(&val),nco_typ_lng(var->type));

  return var;
} /* end scl_dbl_mk_var() */

var_sct * /* [sct] Output netCDF variable structure representing val */
scl_mk_var /* [fnc] Convert scalar value of any type into NCO variable */
(val_unn val, /* I [frc] Scalar value to turn into netCDF variable */
 nc_type val_typ) /* I [enm] netCDF type of value */
{
  /* Purpose: Turn scalar value of any type into NCO variable
     Routine is just a wrapper for scl_ptr_mk_var()
     This routine creates the void * argument needed for scl_ptr_mk_var(),
     calls, scl_ptr_mk_var(), then passes back the result */

  var_sct *var;
  ptr_unn val_ptr_unn; /* [ptr] void pointer to value */
  
  switch(val_typ){
  case NC_FLOAT: val_ptr_unn.fp=&val.f; break; 
  case NC_DOUBLE: val_ptr_unn.dp=&val.d; break; 
  case NC_INT: val_ptr_unn.lp=&val.l; break;
  case NC_SHORT: val_ptr_unn.sp=&val.s; break;
  case NC_CHAR: val_ptr_unn.cp=&val.c; break;
  case NC_BYTE: val_ptr_unn.bp=&val.b; break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(val_typ,&val_ptr_unn);

  var=scl_ptr_mk_var(val_ptr_unn,val_typ);

  return var;
} /* end scl_mk_var() */

var_sct * /* [sct] Output NCO variable structure representing value */
scl_ptr_mk_var /* [fnc] Convert void pointer to scalar of any type into NCO variable */
(ptr_unn val_ptr_unn, /* I [unn] Pointer union to scalar value to turn into netCDF variable */
 nc_type val_typ) /* I [enm] netCDF type of existing pointer/value */
{
  /* Purpose: Convert void pointer to scalar of any type into NCO variable
     Routine duplicates many functions of var_fll() 
     Both functions should share as much initialization code as possible */

  static char *var_nm="Internally generated variable";

  var_sct *var;
  
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  
  /* Overwrite defaults with values appropriate for artificial variable */
  var->nm=(char *)strdup(var_nm);
  var->nbr_dim=0;
  var->type=val_typ;
  /* Allocate new space here so that variable can eventually be deleted 
     and associated memory free()'d */
  /* free(val_ptr_unn.vp) is unpredictable since val_ptr_unn may point to constant data, e.g.,
     a constant in scl_mk_var */
  var->val.vp=(void *)nco_malloc(nco_typ_lng(var->type));

  /* Copy value into variable structure */
  (void)memcpy((void *)var->val.vp,val_ptr_unn.vp,nco_typ_lng(var->type)); 

  return var;
} /* end scl_ptr_mk_var() */

double /* O [frc] Double precision representation of var->val.?p[0] */
ptr_unn_2_scl_dbl /* [fnc] Convert first element of NCO variable to a scalar double */
(ptr_unn val, /* I [sct] Pointer union to variable values */
 nc_type type) /* I [enm] Type of values pointed to by pointer union */
{
  /* Purpose: Return first element of NCO variable converted to a scalar double */

  double scl_dbl; /* [sct] Double precision value of scale_factor */

  ptr_unn ptr_unn_scl_dbl; /* [unn] Pointer union to double precision value of first element */

  /* Variable must be in memory already */
  if(val.vp == NULL){ 
    (void)fprintf(stdout,"%s: ERROR ptr_unn_2_scl_dbl() called with empty val.vp\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* endif */
  
  /* Valid memory address exists */
  ptr_unn_scl_dbl.vp=(void *)nco_malloc(nco_typ_lng(NC_DOUBLE)); /* [unn] Pointer union to double precision value of first element */
  (void)val_conform_type(type,val,NC_DOUBLE,ptr_unn_scl_dbl);
  scl_dbl=ptr_unn_scl_dbl.dp[0];
  ptr_unn_scl_dbl.vp=nco_free(ptr_unn_scl_dbl.vp);

  return scl_dbl;

} /* end ptr_unn_2_scl_dbl() */

void
nco_lib_vrs_prn()
{
  /* Purpose: Print netCDF library version */

  char *lib_sng;
  char *nst_sng;
  char *vrs_sng;
  char *of_ptr;
  char *dlr_ptr;

  int vrs_sng_len;
  int nst_sng_len;

  /* As of netCDF 3.4, nc_inq_libvers() returned strings such as "3.4 of May 16 1998 14:06:16 $" */  
  lib_sng=(char *)strdup(nc_inq_libvers());
  of_ptr=strstr(lib_sng," of ");
  if(of_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lib_vrs_prn() reports of_ptr == NULL\n",prg_nm_get());
  vrs_sng_len=(int)(of_ptr-lib_sng);
  vrs_sng=(char *)nco_malloc(vrs_sng_len+1);
  strncpy(vrs_sng,lib_sng,vrs_sng_len);
  vrs_sng[vrs_sng_len]='\0';

  dlr_ptr=strstr(lib_sng," $");
  if(dlr_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lib_vrs_prn() reports dlr_ptr == NULL\n",prg_nm_get());
  nst_sng_len=(int)(dlr_ptr-of_ptr-4); /* 4 is the length of " of " */
  nst_sng=(char *)nco_malloc(nst_sng_len+1);
  strncpy(nst_sng,of_ptr+4,nst_sng_len); /* 4 is the length of " of " */
  nst_sng[nst_sng_len]='\0';

  (void)fprintf(stderr,"Linked to netCDF library version %s, compiled %s\n",vrs_sng,nst_sng);
  vrs_sng=nco_free(vrs_sng);
  lib_sng=nco_free(lib_sng);
  nst_sng=nco_free(nst_sng);
  (void)fprintf(stdout,"NCO homepage URL is http://nco.sourceforge.net\n");
} /* end nco_lib_vrs_prn() */

void
aed_prc(int nc_id,int var_id,aed_sct aed)
     /* 
	int nc_id: input netCDF file ID
	int var_id: input ID of variable on which to perform attribute editing 
	aed_sct aed: input structure containing information necessary to edit
     */
{
  /* Process a single attribute edit on a single variable */
  
  /* If var_id == NC_GLOBAL ( = -1) then a global attribute will be edited */
  
  char var_nm[NC_MAX_NAME];
  
  /* fxm: netCDF 2 specifies att_sz should be type int, netCDF 3 uses size_t */
  int nbr_att;
  int rcd=NC_NOERR; /* [rcd] Return code */
  long att_sz;
  
  nc_type att_typ;
  
  void *att_val_new=NULL;
  
  if(var_id == NC_GLOBAL){
    /* Get number of global attributes in file */
    (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,&nbr_att,(int *)NULL);
    (void)strcpy(var_nm,"Global");
  }else{
    /* Get name and number of attributes for variable */
    (void)nco_inq_var(nc_id,var_id,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,&nbr_att);
  } /* end else */

  rcd=nco_inq_att_flg(nc_id,var_id,aed.att_nm,&att_typ,&att_sz);
  /* Before changing metadata, change missing values to new missing value if warranted 
     This capability is an add on feature and is not implemented very cleanly or efficiently
     If, for example, every variable has a "missing_value" attribute and it is changed
     globally, then this routine will go into and out of define mode for each variable,
     rather than collecting all the information in one pass and then replacing all the 
     data in a second pass.
     This is because ncatted was originally designed to change only metadata and so was
     architected differently from other NCO operators.
   */
  if(
     strcmp(aed.att_nm,"missing_value") == 0 /* Current attribute is "missing_value" */
     && var_id != NC_GLOBAL /* Current attribute is not global */
     && (aed.mode == aed_modify || aed.mode == aed_overwrite)  /* Modifying or overwriting existing value */
     && rcd == NC_NOERR /* Only when existing missing_value attribute is modified */
     && att_sz == 1L /* Old missing_value attribute must be of size 1 */
     && aed.sz == 1L /* New missing_value attribute must be of size 1 */
     ){

    int *dmn_id;
    long *dmn_sz;
    long *dmn_srt;
    long idx;
    long var_sz=long_CEWI;
    ptr_unn mss_val_crr;
    ptr_unn mss_val_new;
    ptr_unn var_val;
    var_sct *var=NULL_CEWI;

    (void)fprintf(stdout,"%s: WARNING Replacing missing value data in variable %s\n",prg_nm_get(),var_nm);

    /* Take file out of define mode */
    (void)nco_enddef(nc_id);
  
    /* Initialize (partially) variable structure */
    var=(var_sct *)nco_malloc(sizeof(var_sct));
    var->nc_id=nc_id;
    var->id=var_id;
    var->sz=1L;

    /* Get type of variable and number of dimensions */
    (void)nco_inq_var(var->nc_id,var->id,(char *)NULL,&var->type,&var->nbr_dim,(int *)NULL,(int *)NULL);
    dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int));
    dmn_sz=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    dmn_srt=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    (void)nco_inq_vardimid(var->nc_id,var->id,dmn_id);

    /* Get dimension sizes and construct variable size */
    for(idx=0;idx<var->nbr_dim;idx++){
      (void)nco_inq_dimlen(var->nc_id,dmn_id[idx],dmn_sz+idx);
      var->sz*=dmn_sz[idx];
      dmn_srt[idx]=0L;
    } /* end loop over dim */
    var->dmn_id=dmn_id;
    var->cnt=dmn_sz;
    var->srt=dmn_srt;
      
    /* Place var_get() code inline since var struct is not truly complete */
    if((var->val.vp=(void *)malloc(var->sz*nco_typ_lng(var->type))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes in aed_prc()\n",prg_nm_get(),var->sz,nco_typ_lng(var->type));
      exit(EXIT_FAILURE); 
    } /* end if */
    if(var->sz > 1){
      (void)nco_get_vara(var->nc_id,var->id,var->srt,var->cnt,var->val.vp,var->type);
    }else{
      (void)nco_get_var1(var->nc_id,var->id,var->srt,var->val.vp,var->type);
    } /* end else */
    
    /* Get current missing value attribute */
    var->mss_val.vp=NULL;
    var->has_mss_val=mss_val_get(var->nc_id,var);

    /* Sanity check */
    if(var->has_mss_val == False){
      (void)fprintf(stdout,"%s: ERROR \"missing_value\" attribute does not exist in aed_prc()\n",prg_nm_get());
      exit(EXIT_FAILURE);
    } /* end if */

    /* Shortcuts to avoid indirection */
    var_val=var->val;
    var_sz=var->sz;

    /* Get new and old missing values in same type as variable */
    mss_val_crr.vp=(void *)nco_malloc(att_sz*nco_typ_lng(var->type));
    mss_val_new.vp=(void *)nco_malloc(aed.sz*nco_typ_lng(var->type));
    (void)val_conform_type(att_typ,var->mss_val,var->type,mss_val_crr);
    (void)val_conform_type(aed.type,aed.val,var->type,mss_val_new);

    /* Typecast pointer to values before access */
    (void)cast_void_nctype(var->type,&var_val);
    (void)cast_void_nctype(var->type,&mss_val_crr);
    (void)cast_void_nctype(var->type,&mss_val_new);
  
    switch(var->type){
    case NC_FLOAT: for(idx=0L;idx<var_sz;idx++) {if(var_val.fp[idx] == *mss_val_crr.fp) var_val.fp[idx]=*mss_val_new.fp;} break;
    case NC_DOUBLE: for(idx=0L;idx<var_sz;idx++) {if(var_val.dp[idx] == *mss_val_crr.dp) var_val.dp[idx]=*mss_val_new.dp;} break;
    case NC_INT: for(idx=0L;idx<var_sz;idx++) {if(var_val.lp[idx] == *mss_val_crr.lp) var_val.lp[idx]=*mss_val_new.lp;} break;
    case NC_SHORT: for(idx=0L;idx<var_sz;idx++) {if(var_val.sp[idx] == *mss_val_crr.sp) var_val.sp[idx]=*mss_val_new.sp;} break;
    case NC_CHAR: for(idx=0L;idx<var_sz;idx++) {if(var_val.cp[idx] == *mss_val_crr.cp) var_val.cp[idx]=*mss_val_new.cp;} break;
    case NC_BYTE: for(idx=0L;idx<var_sz;idx++) {if(var_val.bp[idx] == *mss_val_crr.bp) var_val.bp[idx]=*mss_val_new.bp;} break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */

    /* Un-typecast the pointer to values after access */
    (void)cast_nctype_void(var->type,&var_val);
    (void)cast_nctype_void(var->type,&mss_val_crr);
    (void)cast_nctype_void(var->type,&mss_val_new);

    /* Write to disk */
    if(var->nbr_dim == 0){
      (void)nco_put_var1(nc_id,var->id,var->srt,var->val.vp,var->type);
    }else{ /* end if variable is a scalar */
      (void)nco_put_vara(nc_id,var->id,var->srt,var->cnt,var->val.vp,var->type);
    } /* end else */

    /* Free memory */
    mss_val_crr.vp=nco_free(mss_val_crr.vp);
    mss_val_new.vp=nco_free(mss_val_new.vp);
    var->mss_val.vp=nco_free(var->mss_val.vp);
    var->val.vp=nco_free(var->val.vp);
    var->dmn_id=nco_free(var->dmn_id);
    var->srt=nco_free(var->srt);
    var->cnt=nco_free(var->cnt);

    /* Put file back in define mode */
    (void)nco_redef(nc_id);
  } /* end if replacing missing value data */

  /* Change metadata (as written, this must be done after missing_value data is replaced) */
  switch(aed.mode){
  case aed_append:	
    if(rcd == NC_NOERR){
      /* Append to existing attribute value */
      if(aed.type != att_typ){
	(void)fprintf(stdout,"%s: ERROR %s attribute %s is of type %s not %s, unable to append\n",prg_nm_get(),var_nm,aed.att_nm,nco_typ_sng(att_typ),nco_typ_sng(aed.type));
	exit(EXIT_FAILURE);
      } /* end if */
      att_val_new=(void *)nco_malloc((att_sz+aed.sz)*nco_typ_lng(aed.type));
      (void)nco_get_att(nc_id,var_id,aed.att_nm,(void *)att_val_new,aed.type);
      /* NB: Following assumes sizeof(char) = 1 byte */
      (void)memcpy((void *)((char *)att_val_new+att_sz*nco_typ_lng(aed.type)),
		   (void *)aed.val.vp,
		   aed.sz*nco_typ_lng(aed.type));
      (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,att_sz+aed.sz,att_val_new);
      att_val_new=nco_free(att_val_new);
    }else{
      /* Create new attribute */
      (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);
    } /* end else */
    break;
  case aed_create:	
    if(rcd != NC_NOERR) (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);  
    break;
  case aed_delete:	
    if(rcd == NC_NOERR) (void)nco_del_att(nc_id,var_id,aed.att_nm);
    break;
  case aed_modify:	
    if(rcd == NC_NOERR) (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);
    break;
  case aed_overwrite:	
    (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);  
    break;
  default: 
    break;
  } /* end switch */
  
} /* end aed_prc() */

