/* $Header: /data/zender/nco_20150216/nco/src/nco/nc_utl.c,v 1.4 1998-10-30 07:16:39 zender Exp $ */

/* (c) Copyright 1995--1998University Corporation for Atmospheric Research/
   National Center for Atmospheric Research/
   Climate and Global Dynamics Division

   The file LICENSE contains the full copyright notice, or 
   you may contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance. */

/* Utilities for netCDF operators */ 

/* Coding abbreviations:
   abb = abbreviation
   arg = argument
   ass = associate
   att = attribute
   bool = Boolean
   buf = buffer
   cff = coefficient
   chk = check
   cmd = command
   cmp = compare
   cnt = count
   cp = char *
   cpy = copy
   crd = coordinate
   crr = current
   dbg = debug
   def = definition
   dgt = digit
   dlm = delimiter
   dim = dimension
   dp = double *
   dst = distance
   dup = duplicate
   err = error
   evl = evaluate
   fl = file
   fmt = format
   fnl = final
   fp = float *
   glb = global
   hst = history
   id = identification
   idx = index
   in = input
   inq = inquire
   lcl = local
   len = length
   lft = left
   lim = limit
   lmn = element
   ln = line
   lp = long *
   lst = list
   lvl = level
   max = maximum
   min = minimum
   mlc = molecule
   mod = modulus
   nbr = number
   ncr = increment
   nc = netCDF (file)
   nm = name
   nlm = unlimited
   nlz = analyze
   nrg = energy
   ntl = initial
   op = operation
   out = output
   prc = process
   prg = program
   prn = print
   prs = parse
   pth = path
   ptr = pointer
   rec = record
   rmt = remote
   rgt = right
   sct = structure
   sfx = suffix
   sng = string
   sp = short *
   spec = specified
   ss = subscript
   srt = start/sort
   str = strength
   stt = status,statement
   sys = system
   sz = size
   tm = time
   tmp = temporary/volatile
   unn = union
   usg = usage
   usr = user
   val = value
   var = variable
   vec = vector
   ver = version
   vp = void *
   xcl = exclude
   xrf = cross reference
   xpr = expression
   xtr = extract
   wgt = weight
   wv = wave
   */ 

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 
#include <string.h>             /* strcmp. . . */
#include <sys/stat.h>           /* stat() */
#include <time.h>               /* machine time */
#include <unistd.h>             /* all sorts of POSIX stuff */ 

#include <netcdf.h>             /* netCDF def'ns */
#include "nc.h"                 /* netCDF operator universal def'ns */
/* #include <errno.h> */             /* errno */
/* #include <malloc.h>    */         /* malloc() stuff */
/* #include <assert.h> */            /* assert() debugging macro */ 

char *
nc_type_nm(nc_type type)
/*  
   nc_type type: input netCDF type
   char *nc_type_nm(): output string describing type
*/ 
{
  switch(type){
  case NC_FLOAT:
    return "NC_FLOAT";
  case NC_DOUBLE:
    return "NC_DOUBLE";
  case NC_LONG:
    return "NC_LONG";
  case NC_SHORT:
    return "NC_SHORT";
  case NC_CHAR:
    return "NC_CHAR";
  case NC_BYTE:
    return "NC_BYTE";
  default:
    (void)fprintf(stdout,"%s: ERROR Unknown nc_type %d in nc_type_nm()\n",prg_nm_get(),type);
    exit(EXIT_FAILURE);
    break;
  } /* end switch */ 

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */ 
  return (char *)NULL;
} /* end nc_type_nm() */ 

char *
c_type_nm(nc_type type)
/*  
   nc_type type: input netCDF type
   char *c_type_nm(): output string describing type
*/ 
{
  switch(type){
  case NC_FLOAT:
    return "float";
  case NC_DOUBLE:
    return "double";
  case NC_LONG:
    return "long";
  case NC_SHORT:
    return "short";
  case NC_CHAR:
    return "char";
  case NC_BYTE:
    return "char";
  default:
    (void)fprintf(stdout,"%s: ERROR Unknown nc_type %d in c_type_nm()\n",prg_nm_get(),type);
    exit(EXIT_FAILURE);
    break;
  } /* end switch */ 

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */ 
  return (char *)NULL;
} /* end c_type_nm() */ 

char *
fortran_type_nm(nc_type type)
/*  
   nc_type type: input netCDF type
   char *fortran_type_nm(): output string describing type
*/ 
{
  switch(type){
  case NC_FLOAT:
    return "real";
  case NC_DOUBLE:
    return "double precision";
  case NC_LONG:
    return "integer";
  case NC_SHORT:
    return "integer*2";
  case NC_CHAR:
    return "character";
  case NC_BYTE:
    return "char";
  default:
    (void)fprintf(stdout,"%s: ERROR Unknown nc_type %d in c_type_nm()\n",prg_nm_get(),type);
    exit(EXIT_FAILURE);
    break;
  } /* end switch */ 

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */ 
  return (char *)NULL;
} /* end fortran_type_nm() */ 

void
cast_void_nctype(nc_type type,ptr_unn *ptr)
/*  
   nc_type type: input netCDF type to cast void pointer to
   ptr_unn *ptr: input/output pointer to pointer union whose vp element will be cast to type type
*/ 
{
  /* Routine to cast the generic pointer in the ptr_unn structure from type void
     type to the output netCDF type. */ 

  switch(type){
  case NC_FLOAT:
    ptr->fp=(float *)ptr->vp;
    break;
  case NC_DOUBLE:
    ptr->dp=(double *)ptr->vp;
    break;
  case NC_LONG:
    ptr->lp=(nclong *)ptr->vp;
    break;
  case NC_SHORT:
    ptr->sp=(short *)ptr->vp;
    break;
  case NC_CHAR:
    ptr->cp=(signed char *)ptr->vp;
    break;
  case NC_BYTE:
    ptr->bp=(unsigned char *)ptr->vp;
    break;
  } /* end switch */ 
} /* end cast_void_nctype() */ 

void
cast_nctype_void(nc_type type,ptr_unn *ptr)
/*  
   nc_type type: input netCDF type of pointer
   ptr_unn *ptr: input/output pointer to pointer union which to cast from type type to type void
*/ 
{
  /* Routine to cast the generic pointer in the ptr_unn structure from type type to type void */ 

  switch(type){
  case NC_FLOAT:
    ptr->vp=(void *)ptr->fp;
    break;
  case NC_DOUBLE:
    ptr->vp=(void *)ptr->dp;
    break;
  case NC_LONG:
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
  } /* end switch */ 

} /* end cast_nctype_void() */ 

void 
lim_evl(int nc_id,lim_sct *lim_ptr,bool FORTRAN_STYLE)
/* 
   int nc_id: input netCDF file ID
   lim_sct *lim_ptr: input/ouput structure from lim_prs() to hold dimension limit info.
   bool FORTRAN_STYLE: input switch to determine syntactical interpretation of dimensional indices
 */ 
{
  /* Routine to take a parsed list of dimension names, minima, and
     maxima strings and find the appropriate indices into the
     dimensions for the correct formulation of dimension start and
     count vectors, or fail trying. */ 

  char *cp;
  
  dim_sct dim;

  enum lim_type{
    crd_val, /* 0 */ 
    dim_idx}; /* 1 */ 
  enum monotonic_direction{
    decreasing, /* 0 */ 
    increasing}; /* 1 */ 

  lim_sct lim;

  int idx;
  int min_lim_type;
  int max_lim_type;
  int monotonic_direction;
  
  long dim_sz;

  lim=*lim_ptr;

  /* Initialize limit structure */ 
  lim.srd=1L;
  lim.min_val=0.;
  lim.max_val=0.;

  /* Get dimension ID */ 
  lim.id=ncdimid(nc_id,lim.nm);
  
  /* Get dimension size */ 
  (void)ncdiminq(nc_id,lim.id,(char *)NULL,&dim.sz);
  
  /* Shortcut to avoid indirection */ 
  dim_sz=dim.sz;

  /* Bomb if dim_sz < 1 */ 
  if(dim_sz < 1){
    (void)fprintf(stdout,"%s: ERROR Dimension size for \"%s\" is %li but must be > 0 in order to apply limits.\n",prg_nm_get(),lim.nm,dim_sz);
    exit(EXIT_FAILURE);
  } /* end if */
  
  if(lim.srd_sng != NULL){
    lim.srd=atol(lim.srd_sng);
    if(strchr(lim.srd_sng,'.')){
      (void)fprintf(stdout,"%s: ERROR Requested stride for \"%s\", %s, must be integer\n",prg_nm_get(),lim.nm,lim.srd_sng);
      exit(EXIT_FAILURE);
    } /* end if */
    if(lim.srd < 1){
      (void)fprintf(stdout,"%s: ERROR Stride for \"%s\" is %li\n",prg_nm_get(),lim.nm,lim.srd);
      exit(EXIT_FAILURE);
    } /* end if */
  } /* end if */

  /* According to the netCDF document "NetCDF Operators and Utilities", 
     if there is a decimal point in the limit string, then the limit should be interpreted as a coordinate value.
     Otherwise the limit is interpreted as a zero-based dimension offset. */ 
  
  /* If min_sng and max_sng are both NULL then set type to dim_idx */
  if(lim.min_sng == NULL && lim.max_sng == NULL){
    /* Limiting indices will be set to default extrema later on */
    min_lim_type=max_lim_type=dim_idx;
  }else{
    /* Assume min_sng and max_sng are not both NULL */ 
    if(lim.min_sng != NULL) min_lim_type=(strchr(lim.min_sng,'.') == NULL) ? dim_idx : crd_val;
    if(lim.max_sng != NULL) max_lim_type=(strchr(lim.max_sng,'.') == NULL) ? dim_idx : crd_val;
    
    /* Copy lim_type from defined limit to undefined */ 
    if(lim.min_sng == NULL) min_lim_type=max_lim_type;
    if(lim.max_sng == NULL) max_lim_type=min_lim_type;
  } /* end else */ 
  
  /* Both min_lim_type and max_lim_type are now defined.
     Continue only if both limits are of the same type. */
  if(min_lim_type != max_lim_type){
    (void)fprintf(stdout,"%s: ERROR -d %s,%s,%s\n",prg_nm_get(),lim.nm,lim.min_sng,lim.max_sng);
    (void)fprintf(stdout,"Limits on dimension \"%s\" must be of same numeric type:\n",lim.nm);
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lim.min_sng,(min_lim_type == crd_val) ? "coordinate value" : "zero-based dimension index");
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lim.max_sng,(max_lim_type == crd_val) ? "coordinate value" : "zero-based dimension index");
    (void)fprintf(stdout,"(Limit arguments containing a decimal point are interpreted as coordinate values; arguments without a decimal point are interpreted as zero-based dimensional indices.)\n");
    exit(EXIT_FAILURE);
  } /* end if */
  
  if(min_lim_type == crd_val){
    double *dim_val_dp;

    double dim_max;
    double dim_min;

    long max_idx;
    long min_idx;
    long tmp_idx;
    long dim_srt=0L;

    char *nc_type_nm(nc_type);
    
    /* Get variable ID of coordinate */ 
    dim.cid=ncvarid(nc_id,lim.nm);
    
    /* Get coordinate type */
    (void)ncvarinq(nc_id,dim.cid,(char *)NULL,&dim.type,(int *)NULL,(int *)NULL,(int *)NULL);
    
    /* Warn when coordinate type is weird */ 
    if(dim.type == NC_BYTE || dim.type == NC_CHAR) (void)fprintf(stderr,"\nWARNING: Coordinate %s is type %s. Dimension truncation is unpredictable.\n",lim.nm,nc_type_nm(dim.type));
    
    /* Allocate enough space to hold coordinate */ 
    dim.val.vp=malloc(dim_sz*nctypelen(dim.type));
    
    /* Retrieve coordinate */ 
    ncvarget(nc_id,dim.cid,&dim_srt,&dim_sz,dim.val.vp);
    
    /* Convert coordinate to double-precision if neccessary */ 
    if(dim.type != NC_DOUBLE){
      ptr_unn old_val;

      old_val=dim.val;
      dim.val.vp=malloc(dim_sz*nctypelen(NC_DOUBLE));
      /* Typecast old coordinate pointer union to correct type before access */ 
      (void)cast_void_nctype(dim.type,&old_val);

      /* Shortcut to avoid indirection */ 
      dim_val_dp=dim.val.dp;
      switch(dim.type){
      case NC_FLOAT: for(idx=0L;idx<dim_sz;idx++) {dim_val_dp[idx]=old_val.fp[idx];} break; 
      case NC_DOUBLE: for(idx=0L;idx<dim_sz;idx++) {dim_val_dp[idx]=old_val.dp[idx];} break; 
      case NC_LONG: for(idx=0L;idx<dim_sz;idx++) {dim_val_dp[idx]=old_val.lp[idx];} break;
      case NC_SHORT: for(idx=0L;idx<dim_sz;idx++) {dim_val_dp[idx]=old_val.sp[idx];} break;
      case NC_CHAR: for(idx=0L;idx<dim_sz;idx++) {dim_val_dp[idx]=old_val.cp[idx];} break;
      case NC_BYTE: for(idx=0L;idx<dim_sz;idx++) {dim_val_dp[idx]=old_val.bp[idx];} break;
      } /* end switch */ 

      /* Un-typecast pointer to values after access */ 
      (void)cast_nctype_void(dim.type,&old_val);
    
      /* Free original space allocated for dimension */
      (void)free(old_val.vp);

      /* Officially change type */ 
      dim.type=NC_DOUBLE;
    } /* end type conversion */ 

    /* Shortcut to avoid indirection */ 
    dim_val_dp=dim.val.dp;

    /* NB: Assuming coordinate is monotonic, then direction of monotonicity is determined by first two elements */ 
    if(dim_sz == 1){
      monotonic_direction=increasing;
    }else{
      if(dim_val_dp[0] > dim_val_dp[1]) monotonic_direction=decreasing; else monotonic_direction=increasing;
    } /* end else */ 

    if(monotonic_direction == increasing){
      min_idx=0L;
      max_idx=dim_sz-1L;
    }else{
      min_idx=dim_sz-1L;
      max_idx=0L;
    } /* end else */

    /* Determine min and max values of entire coordinate */ 
    dim_min=dim_val_dp[min_idx];
    dim_max=dim_val_dp[max_idx];
    
    /* Convert user-specified limits into double precision numeric values, or supply defaults */ 
    if(lim.min_sng == NULL) lim.min_val=dim_val_dp[min_idx]; else lim.min_val=atof(lim.min_sng);
    if(lim.max_sng == NULL) lim.max_val=dim_val_dp[max_idx]; else lim.max_val=atof(lim.max_sng);

    /* Warn when min_val > max_val */ 
    /*if(lim.min_val > lim.max_val) (void)fprintf(stderr,"%s: WARNING User-specified minimum \"%s\" coordinate value %g exceeds user-specified maximum coordinate value %g\nWrap-around hyperslabs are not supported yet--Results are unpredictable.\n",prg_nm_get(),lim.nm,lim.min_val,lim.max_val);*/
    
    /* Exit when... */ 
    if(
       /* User did not specify single level, coordinate is not wrapped, and either extrema falls outside valid crd range */
       ((lim.min_val < lim.max_val) && ((lim.min_val > dim_max) || (lim.max_val < dim_min))) ||
       /* User did not specify single level, coordinate is wrapped, and both extrema fall outside valid crd range */
       ((lim.min_val > lim.max_val) && ((lim.min_val > dim_max) && (lim.max_val < dim_min))) ||
       False){
      (void)fprintf(stdout,"%s: ERROR User-specified range %g <= %s <= %g does not fall within valid range %g <= %s <= %g\n",prg_nm_get(),lim.min_val,lim.nm,lim.max_val,dim_min,lim.nm,dim_max);
      exit(EXIT_FAILURE);
    } /* end if */
    
    /* Armed with target coordinate minima and maxima, we are ready to bracket user-specified range */ 
    
    /* If min_sng or max_sng were omitted, use extrema */ 
    if(lim.min_sng == NULL) lim.min_idx=min_idx;
    if(lim.max_sng == NULL) lim.max_idx=max_idx;
    
    /* A single slice requires finding the closest coordinate. */ 
    if(lim.min_val == lim.max_val){
      double dst_new;
      double dst_old;
      
      lim.min_idx=0L;
      dst_old=fabs(lim.min_val-dim_val_dp[0]);
      for(tmp_idx=1L;tmp_idx<dim_sz;tmp_idx++){
	if((dst_new=fabs(lim.min_val-dim_val_dp[tmp_idx])) < dst_old){
	  dst_old=dst_new;
	  lim.min_idx=tmp_idx;
	} /* end if */
      } /* end loop over tmp_idx */
      lim.max_idx=lim.min_idx;
      
    }else{ /* min_val != max_val */
      
      /* Find brackets to specified extrema */ 
      /* Should no coordinate values match the given criteria, flag that index with a -1L.
	 We defined the valid syntax such that a single half range with -1L is not an error.
	 This causes "-d lon,100.,-100." to select [-180.] when lon=[-180.,-90.,0.,90.] because one
	 of the specified half-ranges is valid (there are coordinates < -100.).
	 However, "-d lon,100.,-200." should fail when lon=[-180.,-90.,0.,90.] because both 
	 of the specified half-ranges are invalid (no coordinate is > 100. or < -200.).
	 The -1L flags are replaced with the correct indices (0L or dim_sz-1L) following the search loop block.
	 Overwriting the -1L flags with 0L or dim_sz-1L later is more heuristic than setting them = 0L here,
	 since 0L is a valid search result.
       */ 
      if(monotonic_direction == increasing){
	if(lim.min_sng != NULL){
	  /* Find index of smallest coordinate greater than min_val */ 
	  tmp_idx=0L;
	  while((dim_val_dp[tmp_idx] < lim.min_val) && (tmp_idx < dim_sz)) tmp_idx++;
	  if(tmp_idx != dim_sz) lim.min_idx=tmp_idx; else lim.min_idx=-1L;
	} /* end if */
	if(lim.max_sng != NULL){
	  /* Find index of largest coordinate less than max_val */ 
	  tmp_idx=dim_sz-1L;
	  while((dim_val_dp[tmp_idx] > lim.max_val) && (tmp_idx > -1L)) tmp_idx--;
	  if(tmp_idx != -1L) lim.max_idx=tmp_idx; else lim.max_idx=-1L;
	} /* end if */
	/* end if monotonic_direction == increasing */ 
      }else{ /* monotonic_direction == decreasing */
	if(lim.min_sng != NULL){
	  /* Find index of smallest coordinate greater than min_val */ 
	  tmp_idx=dim_sz-1L;
	  while((dim_val_dp[tmp_idx] < lim.min_val) && (tmp_idx > -1L)) tmp_idx--;
	  if(tmp_idx != -1L) lim.min_idx=tmp_idx; else lim.min_idx=-1L;
	} /* end if */
	if(lim.max_sng != NULL){
	  /* Find index of largest coordinate less than max_val */ 
	  tmp_idx=0L;
	  while((dim_val_dp[tmp_idx] > lim.max_val) && (tmp_idx < dim_sz)) tmp_idx++;
	  if(tmp_idx != dim_sz) lim.max_idx=tmp_idx; else lim.max_idx=-1L;
	} /* end if */
      } /* end else monotonic_direction == decreasing */

      /* The case where both min_idx and max_idx = -1 was flagged as an error above.
	 In the case of a wrapped coordinate, either, but not both, of min_idx or max_idx be flagged with -1.
	 See explanation above. */ 
      if(lim.min_idx == -1L && (lim.min_val > lim.max_val)) lim.min_idx=0L;
      if(lim.max_idx == -1L && (lim.min_val > lim.max_val)) lim.max_idx=dim_sz-1L;
    
    } /* end if min_val != max_val */
    
    /* We now have user-specified ranges bracketed */ 
    
    /* Original tweaking */ 
    /*      if(monotonic_direction == increasing){
	lim.srt=lim.min_idx;
	lim.end=lim.max_idx;
      }else{
	lim.srt=lim.max_idx;
	lim.end=lim.min_idx;
      } */ /* end else */

    /* Convert indices of minima and maxima to srt and end indices */ 
    if(monotonic_direction == increasing){
      lim.srt=lim.min_idx;
      lim.end=lim.max_idx;
    }else{
      lim.srt=lim.max_idx;
      lim.end=lim.min_idx;
    }  /* end else */

    /* Un-typecast pointer to values after access */ 
    (void)cast_nctype_void(NC_DOUBLE,&dim.val);
  
    /* Free space allocated for dimension */
    (void)free(dim.val.vp);
    
  }else{ /* end if limit arguments were coord. values */ 
    
    /* Convert limit strings to zero-based indicial offsets */
    if(lim.min_sng == NULL) lim.min_idx=0L; else lim.min_idx=atol(lim.min_sng);
    if(lim.max_sng == NULL) lim.max_idx=dim_sz-1L; else lim.max_idx=atol(lim.max_sng);
    
    /* Adjust indices if FORTRAN style input was specified */ 
    if(FORTRAN_STYLE){
      lim.min_idx--;
      lim.max_idx--;
    } /* end if */
    
    /* Exit if requested indices are not in valid range */ 
    if(lim.min_idx < 0 || lim.min_idx >= dim_sz || 
       lim.max_idx < 0 || lim.min_idx >= dim_sz){
      (void)fprintf(stdout,"%s: ERROR User-specified range %li <= %s <= %li does not fall within valid range 0 <= %s <= %li\n",prg_nm_get(),lim.min_idx,lim.nm,lim.max_idx,lim.nm,dim_sz-1L);
      (void)fprintf(stdout,"\n");
      exit(EXIT_FAILURE);
    } /* end if */
    
    lim.srt=lim.min_idx;
    lim.end=lim.max_idx;

  } /* end else limit arguments are hyperslab indices */
  
  /* Compute cnt from srt, end, and srd */ 
  if(lim.srd != 1L){
    if(lim.srt <= lim.end) lim.cnt=1L+(lim.end-lim.srt)/lim.srd; else lim.cnt=1L+((dim_sz-lim.srt)+lim.end)/lim.srd;
  }else{
    if(lim.srt <= lim.end) lim.cnt=lim.end-lim.srt+1L; else lim.cnt=dim_sz-lim.srt+lim.end+1L;
  } /* end else */

  /* NB: Degenerate cases of WRP && SRD exist for which dim_cnt_2 == 0
     This occurs when srd is large enough, or max_idx small enough, 
     such that no values are selected in the second read. 
     e.g., "-d lon,60,0,10" if sz(lon)=128 has dim_cnt_2 == 0
     Since netCDF library reports an error reading and writing cnt=0 dimensions, a kludge is necessary.
     Syntax ensures it is always the second read, not the first, which is obviated.
     Therefore we convert these degenerate cases into non-wrapped coordinates to be processed by a single read. 
     For these degenerate cases only, [srt,end] are not a permutation of [min_idx,max_idx].
     */
  if(
     (lim.srd != 1L) && /* SRD */ 
     (lim.srt > lim.end) && /* WRP */ 
     (lim.cnt == (1L+(dim_sz-lim.srt-1L)/lim.srd)) && /* dim_cnt_1 == cnt -> dim_cnt_2 == 0 */ 
     True){
    long greatest_srd_multiplier_1st_hyp_slb; /* greatest integer m such that srt+m*srd < dim_sz */
    long last_good_idx_1st_hyp_slb; /* C index of last valid member of 1st hyperslab (= srt+m*srd) */ 
    long left_over_idx_1st_hyp_slb; /* # elements from first hyperslab to count towards current stride */ 
    long first_good_idx_2nd_hyp_slb; /* C index of first valid member of 2nd hyperslab, if any */ 

    /* NB: Perform these operations with integer arithmatic or else! */ 
    /* Wrapped dimensions with a stride may not start at idx 0 on second read */ 
    greatest_srd_multiplier_1st_hyp_slb=(dim_sz-lim.srt-1L)/lim.srd;
    last_good_idx_1st_hyp_slb=lim.srt+lim.srd*greatest_srd_multiplier_1st_hyp_slb;
    left_over_idx_1st_hyp_slb=dim_sz-last_good_idx_1st_hyp_slb-1L;
    first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lim.srd)%dim_sz;

    /* Conditions causing dim_cnt_2 == 0 */ 
    if(first_good_idx_2nd_hyp_slb > lim.end) lim.end=last_good_idx_1st_hyp_slb;
  } /* end if */ 

  /* Exit when a valid bracketed range contains no coordinates */ 
  if(lim.cnt == 0){
    if(min_lim_type == crd_val) (void)fprintf(stdout,"%s: ERROR Domain %g <= %s <= %g brackets no coordinate values.\n",prg_nm_get(),lim.min_val,lim.nm,lim.max_val); 
    if(min_lim_type == dim_idx) (void)fprintf(stdout,"%s: ERROR Empty domain for %s\n",prg_nm_get(),lim.nm); 
    exit(EXIT_FAILURE);
  } /* end if */
    
  *lim_ptr=lim;
  
  if(dbg_lvl_get() == 5){
    (void)fprintf(stderr,"name = %s\n",lim.nm);
    (void)fprintf(stderr,"min_sng = %s\n",lim.min_sng == NULL ? "NULL" : lim.min_sng);
    (void)fprintf(stderr,"max_sng = %s\n",lim.max_sng == NULL ? "NULL" : lim.max_sng);
    (void)fprintf(stderr,"srd_sng = %s\n",lim.srd_sng == NULL ? "NULL" : lim.srd_sng);
    (void)fprintf(stderr,"min_val = %g\n",lim.min_val);
    (void)fprintf(stderr,"max_val = %g\n",lim.max_val);
    (void)fprintf(stderr,"min_idx = %li\n",lim.min_idx);
    (void)fprintf(stderr,"max_idx = %li\n",lim.max_idx);
    (void)fprintf(stderr,"srt = %li\n",lim.srt);
    (void)fprintf(stderr,"end = %li\n",lim.end);
    (void)fprintf(stderr,"cnt = %li\n",lim.cnt);
    (void)fprintf(stderr,"srd = %li\n",lim.srd);
  } /* end dbg */
  
} /* end lim_evl() */ 

void
rec_var_dbg(int nc_id,char *dbg_sng)
/* 
   int nc_id: input netCDF file ID
   char *dbg_sng: input debugging message to print
*/ 
{
  /* Purpose: Aid in debugging problems with record dimension */ 
  int nbr_dim_fl;
  int nbr_var_fl;
  int rec_dim_id=-1;
  long dim_sz;

  (void)fprintf(stderr,"%s: DBG %s\n",prg_nm_get(),dbg_sng);
  (void)ncinquire(nc_id,&nbr_dim_fl,&nbr_var_fl,(int *)NULL,&rec_dim_id);
  if(rec_dim_id == -1){
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, no record dimension\n",prg_nm_get(),nbr_dim_fl,nbr_var_fl);
  }else{
    (void)ncdiminq(nc_id,rec_dim_id,(char *)NULL,&dim_sz);
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, record dimension size is %d\n",prg_nm_get(),nbr_dim_fl,nbr_var_fl,dim_sz);
  } /* end else */
  (void)fflush(stderr);
} /* end rec_var_dbg() */ 

void 
att_cpy(int in_id,int out_id,int var_in_id,int var_out_id)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   int var_in_id: input netCDF input-variable ID
   int var_out_id: input netCDF output-variable ID
*/ 
{
  /* Routine to copy all the attributes from the input netCDF
     file to the output netCDF file. If var_in_id == NC_GLOBAL, 
     then the global attributes are copied. Otherwise the variable's
     attributes are copied. */

  int idx;
  int nbr_att;
  int rcd;

  if(var_in_id == NC_GLOBAL){
    (void)ncinquire(in_id,(int *)NULL,(int *)NULL,&nbr_att,(int *)NULL);
  }else{
    (void)ncvarinq(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,(int *)NULL,&nbr_att);
  } /* end else */ 
  
  for(idx=0;idx<nbr_att;idx++){
    char att_nm[MAX_NC_NAME];
    
    (void)ncattname(in_id,var_in_id,idx,att_nm);

    ncopts=0; 
    rcd=ncattinq(out_id,var_out_id,att_nm,(nc_type *)NULL,(int *)NULL);
    ncopts=NC_VERBOSE | NC_FATAL; 
      
    /* Are we about to overwrite an existing attribute? */ 
    if(rcd != -1){
      if(var_out_id == -1){
	(void)fprintf(stderr,"%s: WARNING Overwriting global attribute %s\n",prg_nm_get(),att_nm);
      }else{
	char var_nm[MAX_NC_NAME];
	
	(void)ncvarinq(out_id,var_out_id,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,(int *)NULL);
	(void)fprintf(stderr,"%s: WARNING Overwriting attribute %s for output variable %s\n",prg_nm_get(),att_nm,var_nm);
      } /* end else */
    } /* end if */

    (void)ncattcopy(in_id,var_in_id,att_nm,out_id,var_out_id);

  } /* end loop over attributes */
} /* end att_cpy() */ 

var_sct *
var_fll(int nc_id,int var_id,char *var_nm,dim_sct **dim,int nbr_dim)
/* 
   int nc_id: input netCDF file ID
   int var_id: input variable ID
   char *var_nm: input variable name
   dim_sct **dim: input list of pointers to dimension structures
   int nbr_dim: input number of dimensions in list
   var_sct *var_fll(): output variable structure
 */ 
{
  /* Routine to malloc() and return a completed var_sct */ 

  char dim_nm[MAX_NC_NAME];

  int dim_idx;
  int idx;
  int rec_dim_id;

  var_sct *var;

  /* Get the record dimension ID */
  (void)ncinquire(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dim_id);
  
  var=(var_sct *)malloc(sizeof(var_sct));
  var->nm=var_nm;
  var->id=var_id;
  var->nc_id=nc_id;

  /* Get the type and number of dimensions and attributes for the variable. */
  (void)ncvarinq(var->nc_id,var->id,(char *)NULL,&var->type,&var->nbr_dim,(int *)NULL,&var->nbr_att);

  /* Allocate space to hold the dimension information */ 
  var->dim=(dim_sct **)malloc(var->nbr_dim*sizeof(dim_sct *));
  var->dim_id=(int *)malloc(var->nbr_dim*sizeof(int));
  var->cnt=(long *)malloc(var->nbr_dim*sizeof(long));
  var->srt=(long *)malloc(var->nbr_dim*sizeof(long));
  var->end=(long *)malloc(var->nbr_dim*sizeof(long));
  var->srd=(long *)malloc(var->nbr_dim*sizeof(long));

  /* Get the dimension IDs from the input file */
  (void)ncvarinq(var->nc_id,var->id,(char *)NULL,(nc_type *)NULL,(int *)NULL,var->dim_id,(int *)NULL);
  
  /* Set defaults */ 
  var->is_rec_var=False;
  var->is_crd_var=False;
  var->sz=1L;
  var->sz_rec=1L;
  var->cid=-1;
  var->has_mss_val=False;
  var->mss_val.vp=NULL;
  var->val.vp=NULL;
  var->tally=NULL;
  var->xrf=NULL;

  /* Refresh the number of attributes and the missing value attribute, if any */
  var->has_mss_val=mss_val_get(var->nc_id,var);

  for(idx=0;idx<var->nbr_dim;idx++){

    /* What is the name of this dimension? */ 
    (void)ncdiminq(nc_id,var->dim_id[idx],dim_nm,(long *)NULL);

    /* Search the input dimension list for a matching name */ 
    for(dim_idx=0;dim_idx<nbr_dim;dim_idx++){
      if(!strcmp(dim_nm,dim[dim_idx]->nm)) break;
    } /* end for */ 

    if(dim_idx == nbr_dim){
      (void)fprintf(stdout,"%s: ERROR dimension %s is not in input dimension list\n",prg_nm_get(),dim_nm);
      exit(EXIT_FAILURE);
    } /* end if */ 

    var->dim[idx]=dim[dim_idx];
    var->cnt[idx]=dim[dim_idx]->cnt;
    var->srt[idx]=dim[dim_idx]->srt;
    var->end[idx]=dim[dim_idx]->end;
    var->srd[idx]=dim[dim_idx]->srd;

    if(var->dim_id[idx] == rec_dim_id) var->is_rec_var=True; else var->sz_rec*=var->cnt[idx];

    if(var->dim[idx]->is_crd_dim && var->id == var->dim[idx]->cid){
      var->is_crd_var=True;
      var->cid=var->dim_id[idx];
    } /* end if */

    var->sz*=var->cnt[idx];
  } /* end loop over dim */

  return var;
} /* end var_fll() */ 

void
var_refresh(int nc_id,var_sct *var)
/* 
   int nc_id: input netCDF input-file ID
   var_sct *var: input/output variable structure
 */ 
{
  /* Routine to update the ID, number of dimensions, and missing_value attribute for the given variable */

  /* Refresh the ID for this variable */ 
  var->nc_id=nc_id;
  var->id=ncvarid(var->nc_id,var->nm);

  /* Refresh the number of dimensions for the variable. */
  (void)ncvarinq(var->nc_id,var->id,(char *)NULL,(nc_type *)NULL,&var->nbr_dim,(int *)NULL,(int *)NULL);

  /* Refresh the number of attributes and the missing value attribute, if any */
  var->has_mss_val=mss_val_get(var->nc_id,var);

} /* end var_refresh() */ 

int
mss_val_get(int nc_id,var_sct *var)
/* 
   int nc_id: input netCDF input-file ID
   var_sct *var: input/output variable structure
   int mss_val_get(): output flag whether variable has missing value on output or not
 */ 
{
  /* Routine to update the number of attributes and the missing_value attribute for the variable */

  /* NB: has_mss_val is defined typed as int not bool because it is often sent to Fortran routines */ 

  char att_nm[MAX_NC_NAME];
  
  int att_sz;
  int idx;
  
  long att_len;

  nc_type att_type;

  ptr_unn mss_tmp;
  
  /* Refresh the netCDF "mss_val" attribute for this variable */ 
  var->has_mss_val=False;
  if(var->mss_val.vp != NULL){
    (void)free(var->mss_val.vp);
    var->mss_val.vp=NULL;
  } /* end if */ 

  /* Refresh the number of attributes for the variable. */
  (void)ncvarinq(var->nc_id,var->id,(char *)NULL,(nc_type *)NULL,(int *)NULL,(int *)NULL,&var->nbr_att);

  for(idx=0;idx<var->nbr_att;idx++){
    (void)ncattname(var->nc_id,var->id,idx,att_nm);
    if((int)strcasecmp(att_nm,"missing_value") != 0) continue;
    (void)ncattinq(var->nc_id,var->id,att_nm,&att_type,&att_sz);
    if(att_sz != 1 && att_type != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s has %d elements and so will not be used\n",prg_nm_get(),att_nm,var->nm,att_sz);
      continue;
    } /* end if */ 
    /* if(att_type != var->type) (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s will be typecast from %s to %s for arithmetic purposes\n",prg_nm_get(),att_nm,var->nm,nc_type_nm(att_type),nc_type_nm(var->type)); */
    /* If we got this far then try to retrieve the attribute and make sure it conforms to the variable's type */ 
    var->has_mss_val=True;
    /* Oddly, ARM uses NC_CHAR for the type of missing_value, so we must make allowances for this */ 
    att_len=att_sz*nctypelen(att_type);
    mss_tmp.vp=(void *)malloc(att_len);
    (void)ncattget(var->nc_id,var->id,att_nm,mss_tmp.vp);
    if(att_type == NC_CHAR){
      /* NULL-terminate the missing value string */
      if(mss_tmp.cp[att_len-1] != '\0'){
	att_len++;
	mss_tmp.vp=realloc(mss_tmp.vp,att_len);
	mss_tmp.cp[att_len-1]='\0';
	/* Un-typecast the pointer to the values after access */
	(void)cast_nctype_void(att_type,&mss_tmp);
      } /* end if */ 
    } /* end if */ 
    
    /* Convert NC_CHAR to the type of missing_value */ 
    var->mss_val.vp=(void *)malloc(nctypelen(var->type));
    (void)val_conform_type(att_type,mss_tmp,var->type,var->mss_val);

    /* Free the temporary memory */ 
    (void)free(mss_tmp.vp);
    break;
  } /* end loop over att */

  return var->has_mss_val;

} /* end mss_val_get() */ 

dim_sct *
dim_fll(int nc_id,int dim_id,char *dim_nm)
/* 
   int nc_id: input netCDF input-file ID
   int dim_id: input dimension ID
   char *dim_nm: input dimension name
   dim_sct *dim_fll(): pointer to output dimension structure
 */ 
{
  /* Routine to malloc() and return a completed dim_sct */ 

  dim_sct *dim;
  
  int rec_dim_id;
  
  dim=(dim_sct *)malloc(sizeof(dim_sct));
  
  dim->nm=dim_nm;
  dim->id=dim_id;
  dim->nc_id=nc_id;
  dim->xrf=NULL;
  dim->val.vp=NULL;

  dim->is_crd_dim=False;
  (void)ncdiminq(dim->nc_id,dim_id,(char *)NULL,&dim->sz);
  
  /* Get the record dimension ID */
  (void)ncinquire(dim->nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dim_id);
  if(dim->id == rec_dim_id){
    dim->is_rec_dim=True;
  }else{
    dim->is_rec_dim=False;
  } /* end if */
  
  ncopts=0; 
  dim->cid=ncvarid(dim->nc_id,dim_nm);
  ncopts=NC_VERBOSE | NC_FATAL; 
  if(dim->cid != -1){
    dim->is_crd_dim=True;
    /* What type is the coordinate? */
    (void)ncvarinq(dim->nc_id,dim->cid,(char *)NULL,&dim->type,(int *)NULL,(int *)NULL,(int *)NULL);
  } /* end if */
  
  dim->cnt=dim->sz;
  dim->srt=0L;
  dim->end=dim->sz-1L;
  dim->srd=1L;
  
  return dim;
  
} /* end dim_fll() */ 

void
dim_lim_merge(dim_sct **dim,int nbr_dim,lim_sct *lim,int nbr_lim)
/* 
   dim_sct **dim: input list of pointers to dimension structures
   int nbr_dim: input number of dimension structures in structure list
   lim_sct *lim: input structure from lim_evl() holding dimension limit info.
   int nbr_lim: input number of dimensions with user-specified limits
 */ 
{
  /* Routine to merge the limit structure information into the dimension structures */ 

  int idx;
  int lim_idx;

  for(idx=0;idx<nbr_dim;idx++){

    /* Decide whether this dimension has any user-specified limits */ 
    for(lim_idx=0;lim_idx<nbr_lim;lim_idx++){
      if(lim[lim_idx].id == dim[idx]->id){
	dim[idx]->cnt=lim[lim_idx].cnt;
	dim[idx]->srt=lim[lim_idx].srt;
	dim[idx]->end=lim[lim_idx].end;
	dim[idx]->srd=lim[lim_idx].srd;
	break;
      } /* end if */
    } /* end loop over lim_idx */
  } /* end loop over dim */
} /* end dim_lim_merge() */ 

nm_id_sct *
var_lst_mk(int nc_id,int nbr_var,char **var_lst_in,bool PROCESS_ALL_COORDINATES,int *nbr_xtr)
/* 
   int nc_id: input netCDF file ID
   int nbr_var: input total number of variables in input file
   char **var_lst_in: user specified list of variable names
   bool PROCESS_ALL_COORDINATES: input whether to process all coordinates
   int *nbr_xtr: input/output number of variables in current extraction list
   nm_id_sct var_lst_mk(): output extraction list
 */ 
{
  bool err_flg=False;
  int idx;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */

  if(*nbr_xtr > 0){
    /* If the user named variables with the -v option then we
       need to check the validity of the user's list and find the IDs */ 
    xtr_lst=(nm_id_sct *)malloc(*nbr_xtr*sizeof(nm_id_sct));
    ncopts=0; 
    for(idx=0;idx<*nbr_xtr;idx++){
      xtr_lst[idx].nm=var_lst_in[idx];
      xtr_lst[idx].id=ncvarid(nc_id,xtr_lst[idx].nm);
      if(xtr_lst[idx].id == -1){
	(void)fprintf(stdout,"%s: ERROR var_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),xtr_lst[idx].nm);
	err_flg=True;
      } /* endif */ 
    } /* end loop over idx */
    ncopts=NC_VERBOSE | NC_FATAL; 
    if(err_flg) exit(EXIT_FAILURE);
  }else if(!PROCESS_ALL_COORDINATES){
    /* If the user did not specify variables with the -v option,
       and the user did not request automatic processing of all coords,
       then extract all the variables in the file. In this case
       we can assume the variable IDs range from 0..nbr_var-1. */ 
    char var_nm[MAX_NC_NAME];
    
    *nbr_xtr=nbr_var;
    /* SGI64 winterpark core dumps here on ncra -O in.nc foo.nc */ 
    xtr_lst=(nm_id_sct *)malloc(*nbr_xtr*sizeof(nm_id_sct));
    for(idx=0;idx<nbr_var;idx++){
      /* Get the name for each variable. */
      (void)ncvarinq(nc_id,idx,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,(int *)NULL);
      xtr_lst[idx].nm=(char *)strdup(var_nm);
      xtr_lst[idx].id=idx;
    } /* end loop over idx */
  } /* end else */

  return xtr_lst;

} /* end var_lst_mk() */ 

nm_id_sct *
var_lst_xcl(int nc_id,int nbr_var,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: input netCDF file ID
   int nbr_var: input total number of variables in input file
   nm_id_sct *xtr_lst: input/output current extraction list (destroyed)
   int *nbr_xtr: input/output number of variables in current extraction list
   nm_id_sct var_lst_xcl(): output extraction list
 */ 
{
  /* The user wants to extract all the variables except the ones
     currently in the list. Since it's hard to edit the existing
     list, copy the existing extract list into the exclude list,
     and construct a new list extract list from scratch. */ 

  char var_nm[MAX_NC_NAME];

  int idx;
  int lst_idx;
  int nbr_xcl;

  nm_id_sct *xcl_lst;
  
  /* Turn the extract list into the exclude list and reallocate the extract list  */ 
  nbr_xcl=*nbr_xtr;
  *nbr_xtr=0;
  xcl_lst=(nm_id_sct *)malloc(nbr_xcl*sizeof(nm_id_sct));
  (void)memcpy((void *)xcl_lst,(void *)xtr_lst,nbr_xcl*sizeof(nm_id_sct));
  xtr_lst=(nm_id_sct *)realloc((void *)xtr_lst,(nbr_var-nbr_xcl)*sizeof(nm_id_sct));
  
  for(idx=0;idx<nbr_var;idx++){
    /* Get the name and ID for the variable. */
    (void)ncvarinq(nc_id,idx,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,(int *)NULL);
    for(lst_idx=0;lst_idx<nbr_xcl;lst_idx++){
      if(idx == xcl_lst[lst_idx].id) break;
    } /* end loop over lst_idx */
    /* If the variable was not found in the exclusion list then 
       add it to the new list. */ 
    if(lst_idx == nbr_xcl){
      xtr_lst[*nbr_xtr].nm=(char *)strdup(var_nm);
      xtr_lst[*nbr_xtr].id=idx;
      ++*nbr_xtr;
    } /* end if */
  } /* end loop over idx */
  
  /* Free the memory for names in the exclude list before losing the pointers to the names */ 
  /* NB: can't free the memory if the list points to names in argv[] */ 
/*  for(idx=0;idx<nbr_xcl;idx++) (void)free(xcl_lst[idx].nm);*/
  (void)free(xcl_lst);
  
  return xtr_lst;
  
} /* end var_lst_xcl() */ 

nm_id_sct *
var_lst_add_crd(int nc_id,int nbr_var,int nbr_dim,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: input netCDF file ID
   int nbr_var: input total number of variables in input file
   int nbr_dim: input total number of dimensions in input file
   nm_id_sct *xtr_lst: current extraction list (destroyed)
   int *nbr_xtr: input/output number of variables in current extraction list
   nm_id_sct var_lst_add_crd(): output extraction list
 */ 
{
  /* Find all coordinates (dimensions which are also variables) and
     add them to the list if they are not already there. */
  
  char crd_nm[MAX_NC_NAME];

  int crd_id;
  int idx;

  for(idx=0;idx<nbr_dim;idx++){
    (void)ncdiminq(nc_id,idx,crd_nm,(long *)NULL);
    
    /* Does a variable by the same name exist in the input file? */
    ncopts=0; 
    crd_id=ncvarid(nc_id,crd_nm);
    ncopts=NC_VERBOSE | NC_FATAL; 
    if(crd_id != -1){
      /* Dimension is a coordinate. Is it already on the list? */
      int lst_idx;
      
      for(lst_idx=0;lst_idx<*nbr_xtr;lst_idx++){
	if(crd_id == xtr_lst[lst_idx].id) break;
      } /* end loop over lst_idx */
      if(lst_idx == *nbr_xtr){
	/* Coordinate is not already on the list, put it there. */ 
	if(*nbr_xtr == 0) xtr_lst=(nm_id_sct *)malloc((*nbr_xtr+1)*sizeof(nm_id_sct)); else xtr_lst=(nm_id_sct *)realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	/* According to the man page for realloc(), this should work even when xtr_lst == NULL */ 
/*	xtr_lst=(nm_id_sct *)realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));*/
	xtr_lst[*nbr_xtr].nm=(char *)strdup(crd_nm);
	xtr_lst[*nbr_xtr].id=crd_id;
	(*nbr_xtr)++;
      } /* end if */
    } /* end if */
  } /* end loop over idx */
  
  return xtr_lst;
  
} /* end var_lst_add_crd() */ 

lim_sct
lim_dim_mk(int nc_id,int dim_id,lim_sct *lim,int nbr_lim,bool FORTRAN_STYLE)
/* 
   int nc_id: input netCDF file ID
   int dim_id: input ID of the dimension for which to create a limit structure
   lim_sct *lim: input array of limits structures from lim_evl()
   int nbr_lim: input number of limit structures in limit structure array
   bool FORTRAN_STYLE: input switch to determine syntactical interpretation of dimensional indices
   lim_sct lim_dim_mk(): output limit structure for dimension
 */ 
{
  /* Create a stand-alone limit structure just for the given dimension */ 
  
  int idx;
  int rcd;
  
  lim_sct lim_dim;

  /* Decide whether the dimension has any user-specified limits */ 
  for(idx=0;idx<nbr_lim;idx++){
    if(lim[idx].id == dim_id){
      if(lim[idx].max_sng != NULL) lim_dim.max_sng=(char *)strdup(lim[idx].max_sng); else lim_dim.max_sng=NULL;
      if(lim[idx].min_sng != NULL) lim_dim.min_sng=(char *)strdup(lim[idx].min_sng); else lim_dim.min_sng=NULL;
      if(lim[idx].srd_sng != NULL) lim_dim.srd_sng=(char *)strdup(lim[idx].srd_sng); else lim_dim.srd_sng=NULL;
      lim_dim.nm=(char *)strdup(lim[idx].nm);
      break;
    } /* end if */
  } /* end loop over idx */
  if(idx == nbr_lim){
    char dim_nm[MAX_NC_NAME];
    
    /* Fill in the limits with the default parsing info. */ 
    ncopts=0; 
    rcd=ncdiminq(nc_id,dim_id,dim_nm,(long *)NULL);
    ncopts=NC_VERBOSE | NC_FATAL; 

    if(rcd == -1){
      (void)fprintf(stdout,"%s: ERROR attempting to find non-existent dimension with id = %d in lim_dim_mk()\n",prg_nm_get(),dim_id);
      exit(EXIT_FAILURE);
    } /* end if */ 

    lim_dim.nm=(char *)strdup(dim_nm);
    lim_dim.max_sng=NULL;
    lim_dim.srd_sng=NULL;
    if(FORTRAN_STYLE){
      lim_dim.min_sng=(char *)strdup("1");
    }else{
      lim_dim.min_sng=(char *)strdup("0");
    } /* end else */
  } /* end if */
  
  return lim_dim;
  
} /* end lim_dim_mk() */ 

nm_id_sct *
var_lst_crd_xcl(int nc_id,int dim_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: input netCDF file ID
   int dim_id: input dimension ID of the coordinate to eliminate from the extraction list
   nm_id_sct *xtr_lst: current extraction list (destroyed)
   int *nbr_xtr: input/output number of variables in current extraction list
   nm_id_sct var_lst_crd_xcl(): output extraction list
 */ 
{
  /* The following code modifies the extraction list to exclude the coordinate, 
     if any, associated with the given dimension ID */
  
  char crd_nm[MAX_NC_NAME];

  int idx;
  int crd_id=-1;
  
  /* What is the variable ID of the record coordinate, if any? */ 
  (void)ncdiminq(nc_id,dim_id,crd_nm,(long *)NULL);
  ncopts=0; 
  crd_id=ncvarid(nc_id,crd_nm);
  ncopts=NC_VERBOSE | NC_FATAL; 
  
  if(crd_id != -1){
    /* Is the coordinate on the extraction list? */ 
    for(idx=0;idx<*nbr_xtr;idx++){
      if(xtr_lst[idx].id == crd_id) break;
    } /* end loop over idx */
    if(idx != *nbr_xtr){
      nm_id_sct *var_lst_tmp;
      
      var_lst_tmp=(nm_id_sct *)malloc(*nbr_xtr*sizeof(nm_id_sct));
      /* Copy the extract list to the temporary extract list and reallocate the extract list */ 
      (void)memcpy((void *)var_lst_tmp,(void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      (*nbr_xtr)--;
      xtr_lst=(nm_id_sct *)realloc((void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      /* Collapse the temporary extract list into the permanent list by copying 
	 all but the coordinate. NB: the ordering of the list is conserved. */ 
      (void)memcpy((void *)xtr_lst,(void *)var_lst_tmp,idx*sizeof(nm_id_sct));
      (void)memcpy((void *)(xtr_lst+idx),(void *)(var_lst_tmp+idx+1),(*nbr_xtr-idx)*sizeof(nm_id_sct));
      /* Free the memory for coordinate name in the extract list before losing the pointer */
      (void)free(var_lst_tmp[idx].nm);
      (void)free(var_lst_tmp);
    } /* end if */
  } /* end if */
  
  return xtr_lst;
  
} /* end var_lst_crd_xcl() */ 

nm_id_sct *
var_lst_ass_crd_add(int nc_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: input netCDF file ID
   nm_id_sct *xtr_lst: input/output current extraction list (destroyed)
   int *nbr_xtr: input/output number of variables in current extraction list
   nm_id_sct var_lst_ass_crd_add(): output extraction list
 */ 
{
  /* Makes sure all coordinates associated with each of the variables
     to be extracted is also on the list. This helps with making concise
     malloc() calls down the road. */ 

  char dim_nm[MAX_NC_NAME];

  int crd_id;
  int dim_id[MAX_NC_DIMS];
  int idx_dim;
  int idx_var_dim;
  int idx_var;
  int nbr_dim;
  int nbr_var_dim;

  /* Get the number of dimensions */
  (void)ncinquire(nc_id,&nbr_dim,(int *)NULL,(int *)NULL,(int *)NULL);

  /* ...for each dimension in the input file... */ 
  for(idx_dim=0;idx_dim<nbr_dim;idx_dim++){
    /* ...see if it is a coordinate dimension... */ 
    (void)ncdiminq(nc_id,idx_dim,dim_nm,(long *)NULL);
    ncopts=0; 
    crd_id=ncvarid(nc_id,dim_nm);
    ncopts=NC_VERBOSE | NC_FATAL; 
    if(crd_id != -1){
      /* Is this coordinate already on the extraction list? */ 
      for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	if(crd_id == xtr_lst[idx_var].id) break;
      } /* end loop over idx_var */
      if(idx_var == *nbr_xtr){
	/* ...the coordinate is not on the list, is it associated with any of the variables?... */ 
	for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	  /* Get number of dimensions and the dimension IDs for the variable. */
	  (void)ncvarinq(nc_id,xtr_lst[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dim_id,(int *)NULL);
	  for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
	    if(idx_dim == dim_id[idx_var_dim]) break;
	  } /* end loop over idx_var_dim */
	  if(idx_var_dim != nbr_var_dim){
	    /* Add the coordinate to the list */ 
	    xtr_lst=(nm_id_sct *)realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	    xtr_lst[*nbr_xtr].nm=(char *)strdup(dim_nm);
	    xtr_lst[*nbr_xtr].id=crd_id;
	    (*nbr_xtr)++;
	    break;
	  } /* end if */ 
	} /* end loop over idx_var */
      } /* end if coordinate was not already on the list */
    } /* end if dimension is a coordinate */ 
  } /* end loop over idx_dim */
  
  return xtr_lst;
  
} /* end var_lst_ass_crd_add() */ 

nm_id_sct *
lst_heapsort(nm_id_sct *lst,int nbr_lst)
     /* 
   nm_id_sct *lst: current list (destroyed)
   int nbr_lst: input number of members in list
   nm_id_sct lst_heapsort(): output list
 */ 
{
  /* Heapsort the list by ID for fastest I/O */ 

  int *srt_idx;
  int *xtr_id;
  int idx;
  
  nm_id_sct *lst_tmp;
  
  xtr_id=(int *)malloc(nbr_lst*sizeof(int));
  srt_idx=(int *)malloc(nbr_lst*sizeof(int));
  lst_tmp=(nm_id_sct *)malloc(nbr_lst*sizeof(nm_id_sct));
  (void)memcpy((void *)lst_tmp,(void *)lst,nbr_lst*sizeof(nm_id_sct));
  
  for(idx=0;idx<nbr_lst;idx++){
    xtr_id[idx]=lst[idx].id;
  } /* end loop over idx */
  /* NB: Many Numerical Recipes routines, including this one, employ "one-based" arrays */ 
  (void)indexx(nbr_lst,xtr_id-1,srt_idx-1);
  for(idx=0;idx<nbr_lst;idx++){
    lst[idx].id=lst_tmp[srt_idx[idx]-1].id;
    lst[idx].nm=lst_tmp[srt_idx[idx]-1].nm;
  } /* end loop over idx */
  
  (void)free(lst_tmp);
  (void)free(srt_idx);
  (void)free(xtr_id);
  
  return lst;
  
} /* end lst_heapsort() */ 

char *
fl_out_open(char *fl_out,bool FORCE_APPEND,bool FORCE_OVERWRITE,int *out_id)
/* 
   char *fl_out: input name of the file to open
   bool FORCE_APPEND: input flag for appending to existing file, if any
   bool FORCE_OVERWRITE: input flag for overwriting existing file, if any
   int *nc_id: output file ID
   char *fl_out_open(): output name of the temporary file actually opened
 */ 
{
  /* Open the output file subject to availability and user input. 
     In accordance with netCDF philosophy a temporary file (based on fl_out and the process ID)
     is actually opened, so that a fatal error will not create the intended output file. */ 

  char *fl_out_tmp;

  int rcd;

  struct stat stat_sct;
  
  if(FORCE_OVERWRITE && FORCE_APPEND){
    (void)fprintf(stdout,"%s: ERROR FORCE_OVERWRITE and FORCE_APPEND are both set\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* end if */

  fl_out_tmp=(char *)malloc((strlen(fl_out)+strlen(prg_nm_get())+12+1)*sizeof(char));
  (void)sprintf(fl_out_tmp,"%s.%06d.%s.tmp",fl_out,(int)getpid(),prg_nm_get());
  rcd=stat(fl_out_tmp,&stat_sct);

  /* If the temporary file already exists, then prompt the user to remove the temporary files and exit */
  if(rcd != -1){
    (void)fprintf(stdout,"%s: ERROR temporary file %s already exists, remove and try again\n",prg_nm_get(),fl_out_tmp);
    exit(EXIT_FAILURE);
  } /* end if */

  if(FORCE_OVERWRITE){
    *out_id=nccreate(fl_out_tmp,NC_CLOBBER);
    /*    rcd=nc_create(fl_out_tmp,NC_CLOBBER|NC_SHARE,out_id);*/
    return fl_out_tmp;
  } /* end if */

  if(False){
    if(prg_get() == ncrename){
      /* ncrename is different because a single filename is allowed without question */ 
      /* Incur the expense of copying the current file to a temporary file */
      (void)fl_cp(fl_out,fl_out_tmp);
      *out_id=ncopen(fl_out_tmp,NC_WRITE); 
      (void)ncredef(*out_id);
      return fl_out_tmp;
    } /* end if */
  }

  rcd=stat(fl_out,&stat_sct);
  
  /* If the permanent file already exists, then query the user whether to overwrite, append, or exit */
  if(rcd != -1){
    char usr_reply='z';
    short nbr_itr=0;
    
    if(FORCE_APPEND){
      /* Incur the expense of copying the current file to a temporary file */
      (void)fl_cp(fl_out,fl_out_tmp);
      *out_id=ncopen(fl_out_tmp,NC_WRITE); 
      (void)ncredef(*out_id);
      return fl_out_tmp;
    } /* end if */

    while(usr_reply != 'o' && usr_reply != 'a' && usr_reply != 'e'){
      nbr_itr++;
      if(nbr_itr > 10){
	(void)fprintf(stdout,"\n%s: ERROR %hd failed attempts to obtain valid interactive input. Assuming non-interactive shell and exiting.\n",prg_nm_get(),nbr_itr-1);
	exit(EXIT_FAILURE);
      }; /* end if */ 
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
      *out_id=nccreate(fl_out_tmp,NC_CLOBBER);
      /*    rcd=nc_create(fl_out_tmp,NC_CLOBBER|NC_SHARE,out_id);*/
      break;
    case 'a':
      /* Incur the expense of copying the current file to a temporary file */
      (void)fl_cp(fl_out,fl_out_tmp);
      *out_id=ncopen(fl_out_tmp,NC_WRITE); 
      (void)ncredef(*out_id);
      break;
    } /* end switch */
    
  }else{ /* output file does not yet already exist */ 
    *out_id=nccreate(fl_out_tmp,NC_NOCLOBBER);
    /*    rcd=nc_create(fl_out_tmp,NC_NOCLOBBER|NC_SHARE,out_id);*/
  } /* end if output file does not already exist */
  
  return fl_out_tmp;
  
} /* end fl_out_open() */ 

void
fl_out_close(char *fl_out,char *fl_out_tmp,int nc_id)
/* 
   char *fl_out: input name of the permanent output file
   char *fl_out_tmp: input name of the temporary output file to close and move to permanent output file
   int nc_id: input file ID of fl_out_tmp
 */ 
{
  /* Routine to close the temporary output file and move it to the permanent output file */ 
  int rcd;

  rcd=ncclose(nc_id);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR fl_out_close() is unable to ncclose() file %s\n",prg_nm_get(),fl_out_tmp);
    exit(EXIT_FAILURE); 
  } /* end if */ 
  
  (void)fl_mv(fl_out_tmp,fl_out);

} /* end fl_out_close() */ 

void
fl_cmp_err_chk()
/* 
   fl_cmp_err_chk():
*/
{
  /* NB: The goal of the error checking is to guarantee that the user is getting
     the results he thinks he is. The netCDF library calls will fail and flag most
     of the likely errors. However, if T is stored as (lon,lat)=(128,64) in one file,
     and as (lat,lon)=(64,128) in another file, and a hyperslab valid in both files
     is specified, then an error will occur. */
  
} /* end fl_cmp_err_chk() */ 

void
var_val_cpy(int in_id,int out_id,var_sct **var,int nbr_var)
/* 
   int in_id: input netCDF file ID
   int out_id: input netCDF output-file ID
   var_sct **var: input list of pointers to variable structures
   int nbr_var: input number of structures in variable structure list
   var_val_cpy():
*/
{
  /* Copy the variable data for every variable in the input variable structure list
     from the input file to the output file */ 

  int idx;

  for(idx=0;idx<nbr_var;idx++){
    var[idx]->xrf->val.vp=var[idx]->val.vp=(void *)malloc(var[idx]->sz*nctypelen(var[idx]->type));
    if(var[idx]->nbr_dim==0){
      ncvarget1(in_id,var[idx]->id,var[idx]->srt,var[idx]->val.vp);
      ncvarput1(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->val.vp);
    }else{ /* end if variable is a scalar */ 
      ncvarget(in_id,var[idx]->id,var[idx]->srt,var[idx]->cnt,var[idx]->val.vp);
      ncvarput(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->val.vp);
    } /* end if variable is an array */ 
    (void)free(var[idx]->val.vp); var[idx]->xrf->val.vp=var[idx]->val.vp=NULL;
  } /* end loop over idx */

} /* end var_val_cpy() */ 

void
dim_def(char *fl_nm,int nc_id,dim_sct **dim,int nbr_dim)
/* 
   char *fl_nm: input name of the output file
   int nc_id: input netCDF output-file ID
   dim_sct **dim: input list of pointers to dimension structures to be defined in the output file
   int nbr_dim: input number of dimension structures in structure list
*/
{
  int idx;

  for(idx=0;idx<nbr_dim;idx++){

    /* See if the dimension has already been defined */ 
    ncopts=0; 
    dim[idx]->id=ncdimid(nc_id,dim[idx]->nm);
    ncopts=NC_VERBOSE | NC_FATAL; 

    /* If the dimension hasn't been defined, define it */
    if(dim[idx]->id == -1){
      if(dim[idx]->is_rec_dim){
	dim[idx]->id=ncdimdef(nc_id,dim[idx]->nm,NC_UNLIMITED);
      }else{
	dim[idx]->id=ncdimdef(nc_id,dim[idx]->nm,dim[idx]->cnt);
      } /* end else */
    }else{
      (void)fprintf(stderr,"%s: WARNING dimension \"%s\" is already defined in %s\n",prg_nm_get(),dim[idx]->nm,fl_nm);
    } /* end if */
  } /* end loop over idx */
  
} /* end dim_def() */ 

void
var_def(int in_id,char *fl_out,int out_id,var_sct **var,int nbr_var,dim_sct **dim_ncl,int nbr_dim_ncl)
/* 
   int in_id: input netCDF input-file ID
   char *fl_out: input name of the output file
   int out_id: input netCDF output-file ID
   var_sct **var: input list of pointers to variable structures to be defined in the output file
   int nbr_var: input number of variable structures in structure list
   dim_sct **dim_ncl: input list of pointers to dimension structures allowed in the output file
   int nbr_dim_ncl: input number of dimension structures in structure list
*/
{
  /* Define the variables in the output file, and copy their attributes */ 

  /* This function is unusual (for me) in that the dimension arguments are only intended
     to be used by certain programs, those that alter the rank of input variables. If a
     program does not alter the rank (dimensionality) of input variables then it should
     call this function with a NULL dimension list. Otherwise, this routine attempts
     to define the variable correctly in the output file (allowing the variable to be
     defined with only those dimensions that are in the dimension inclusion list) 
     without altering the variable structures. */

  int idx_dim;
  int dim_id_vec[MAX_NC_DIMS];
  int idx;
  
  for(idx=0;idx<nbr_var;idx++){

    /* See if the requested variable is already in the output file. */
    ncopts=0; 
    var[idx]->id=ncvarid(out_id,var[idx]->nm);
    ncopts=NC_VERBOSE | NC_FATAL; 

    /* If the variable has not been defined, define it */
    if(var[idx]->id == -1){
      
      if(dim_ncl != NULL){
	int nbr_var_dim=0;
	int idx_ncl;

	/* The rank of the output variable may have to be reduced. */ 
	for(idx_dim=0;idx_dim<var[idx]->nbr_dim;idx_dim++){
	  /* Is this dimension allowed in the output file? */ 
	  for(idx_ncl=0;idx_ncl<nbr_dim_ncl;idx_ncl++){
	    if(var[idx]->xrf->dim[idx_dim]->id == dim_ncl[idx_ncl]->xrf->id) break;
	  } /* end loop over idx_ncl */
	  if(idx_ncl != nbr_dim_ncl) dim_id_vec[nbr_var_dim++]=var[idx]->dim[idx_dim]->id;
	} /* end loop over idx_dim */
	var[idx]->id=ncvardef(out_id,var[idx]->nm,var[idx]->type,nbr_var_dim,dim_id_vec);

      }else{ /* Straightforward definition */ 
	for(idx_dim=0;idx_dim<var[idx]->nbr_dim;idx_dim++){
	  dim_id_vec[idx_dim]=var[idx]->dim[idx_dim]->id;
	} /* end loop over idx_dim */
	var[idx]->id=ncvardef(out_id,var[idx]->nm,var[idx]->type,var[idx]->nbr_dim,dim_id_vec);
      } /* end else */

    }else{
      (void)fprintf(stderr,"%s: WARNING Using existing definition of variable \"%s\" in %s\n",prg_nm_get(),var[idx]->nm,fl_out);
    } /* end if */

    /* NB: var actually refers to the output variable sct, so var->xrf references the input var. sct */ 
    (void)att_cpy(in_id,out_id,var[idx]->xrf->id,var[idx]->id);
  } /* end loop over idx */
  
} /* end var_def() */ 

void 
hst_att_cat(int out_id,char *hst_sng)
/* 
   int out_id: input netCDF output-file ID
   char *hst_sng: input string to add to history attribute
*/ 
{

/* Routine to add command line and a date stamp to existing history attribute, if any,
   and write them to the specified output file */ 

  char att_nm[MAX_NC_NAME];
  char *ctime_sng;
  char *history_crr=NULL;
  char *history_new;
  char time_stamp_sng[25];
  
  int att_sz=0;
  int idx;
  int nbr_glb_att;

  nc_type att_type;
  
  time_t clock;

  /* Create timestamp string */ 
  clock=time((time_t *)NULL);
  ctime_sng=ctime(&clock);
  /* Get rid of carriage return in ctime_sng */ 
  (void)strncpy(time_stamp_sng,ctime_sng,24);
  time_stamp_sng[24]='\0';

  /* Get number of global attributes in file */
  (void)ncinquire(out_id,(int *)NULL,(int *)NULL,&nbr_glb_att,(int *)NULL);

  for(idx=0;idx<nbr_glb_att;idx++){
    (void)ncattname(out_id,NC_GLOBAL,idx,att_nm);
    if(strcasecmp(att_nm,"history") == 0) break;
  } /* end loop over att */

  /* Fill in the history string */
  if(idx == nbr_glb_att){
    /* history global attribute does not yet exist */

    /* Add 3 for formatting characters */ 
    history_new=(char *)malloc((strlen(hst_sng)+strlen(time_stamp_sng)+3)*sizeof(char));
    (void)sprintf(history_new,"%s: %s",time_stamp_sng,hst_sng);
  }else{ 
    /* history global attribute currently exists */
  
    /* NB: the ncattinq() call, unlike strlen(), counts the terminating NULL for stored NC_CHAR arrays */ 
    (void)ncattinq(out_id,NC_GLOBAL,"history",&att_type,&att_sz);
    if(att_type != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" global attribute is type %s, not %s. Therefore current command line will not be appended to %s in output file.\n",prg_nm_get(),att_nm,nc_type_nm(att_type),nc_type_nm(NC_CHAR),att_nm);
      return;
    } /* end if */

    history_crr=(char *)malloc(att_sz*sizeof(char));
    (void)ncattget(out_id,NC_GLOBAL,"history",(void *)history_crr);
    /* Add 4 for formatting characters */ 
    history_new=(char *)malloc((strlen(history_crr)+strlen(hst_sng)+strlen(time_stamp_sng)+4)*sizeof(char));
    (void)sprintf(history_new,"%s: %s\n%s",time_stamp_sng,hst_sng,history_crr);

  } /* end else */

  (void)ncattput(out_id,NC_GLOBAL,"history",NC_CHAR,strlen(history_new)+1,(void *)history_new);

  if(history_crr != NULL) (void)free(history_crr);
  (void)free(history_new);

} /* end hst_att_cat() */ 

nm_id_sct *
dim_lst_ass_var(int nc_id,nm_id_sct *var,int nbr_var,int *nbr_dim)
/* 
   int nc_id: input netCDF input-file ID
   nm_id_sct *var: input variable list
   int nbr_var: input number of variables in list
   int *nbr_dim: output number of dimensions associated with input variable list
   nm_id_sct *dim_lst_ass_var(): output list of dimensions associated with input variable list
 */
{
  /* Routine to create a list of all the dimensions associated with the input variable list */ 

  bool dim_has_been_placed_on_list;

  char dim_nm[MAX_NC_NAME];

  int dim_id[MAX_NC_DIMS];
  int idx;
  int idx_dim_in;
  int idx_var;
  int idx_var_dim;
  int idx_dim_lst;
  int nbr_dim_in;
  int nbr_var_dim;
  
  nm_id_sct *dim;

  *nbr_dim=0;

  /* Get the number of dimensions */
  (void)ncinquire(nc_id,&nbr_dim_in,(int *)NULL,(int *)NULL,(int *)NULL);

  /* The number of input dimensions is an upper bound on the number of output dimensions */
  dim=(nm_id_sct *)malloc(nbr_dim_in*sizeof(nm_id_sct));
  
  /* ...For each dimension in the file... */ 
  for(idx_dim_in=0;idx_dim_in<nbr_dim_in;idx_dim_in++){
    /* ...begin a search for the dimension in the dimension list by... */ 
    dim_has_been_placed_on_list=False;
    /* ...looking through the set of output variables... */ 
    for(idx_var=0;idx_var<nbr_var;idx_var++){
      /* ...and search each dimension of the output variable... */ 
      (void)ncvarinq(nc_id,var[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dim_id,(int *)NULL);
      for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
	/* ...until an output variable is found which contains the input dimension... */ 
	if(idx_dim_in == dim_id[idx_var_dim]){
	  /* ...then search each member of the output dimension list... */ 
	  for(idx_dim_lst=0;idx_dim_lst<*nbr_dim;idx_dim_lst++){
	    /* ...until the input dimension is found... */ 
	    if(idx_dim_in == dim[idx_dim_lst].id) break; /* ...then search no further... */ 
	  } /* end loop over idx_dim_lst */
	  /* ...and if the dimension was not found on the output dimension list... */ 
	  if(idx_dim_lst == *nbr_dim){
	    /* ...then add the dimension to the output dimension list... */ 
	    (void)ncdiminq(nc_id,idx_dim_in,dim_nm,(long *)NULL);
	    dim[*nbr_dim].id=idx_dim_in;
	    dim[*nbr_dim].nm=(char *)strdup(dim_nm);
	    (*nbr_dim)++;
	  } /* end if dimension was not found in current output dimension list*/
	  /* ...call off the dogs for this input dimension... */ 
	  dim_has_been_placed_on_list=True;
	} /* end if input dimension belongs to this output variable */
	if(dim_has_been_placed_on_list) break; /* break out of idx_var_dim to idx_var */ 
      } /* end loop over idx_var_dim */
      if(dim_has_been_placed_on_list) break; /* break out of idx_var to idx_dim_in */ 
    } /* end loop over idx_var */
  } /* end loop over idx_dim_in */
  
  /* We now have the final list of dimensions to extract. Phew. */
  
  /* Free the unneeded space in the output dimension list */ 
  dim=(nm_id_sct *)realloc((void *)dim,*nbr_dim*sizeof(nm_id_sct));
  
  return dim;

} /* end dim_lst_ass_var() */ 

void
var_srt_zero(var_sct **var,int nbr_var)
/* 
   var_sct **var: input list of pointers to variable structures whose srt elements will be zeroed
   int nbr_var: input number of structures in variable structure list
 */ 
{
  /* Routine to point the srt element of the variable structure to an array of zeroes */ 

  int idx;
  int idx_dim;

  for(idx=0;idx<nbr_var;idx++)
    for(idx_dim=0;idx_dim<var[idx]->nbr_dim;idx_dim++)
      var[idx]->srt[idx_dim]=0L;

} /* end var_srt_zero() */ 

var_sct *
var_dup(var_sct *var)
/* 
   var_sct *var: input variable structure to duplicate
   var_sct *var_dup(): output copy of input variable structure
 */ 
{
  /* Routine to malloc() and return a duplicate of the input var_sct */ 

  int idx;

  var_sct *var_dup;

  var_dup=(var_sct *)malloc(sizeof(var_sct));

  (void)memcpy((void *)var_dup,(void *)var,sizeof(var_sct));

  /* Copy all dyamically allocated arrays that are currently defined in the original */ 
  if(var->val.vp != NULL){
    if((var_dup->val.vp=(void *)malloc(var_dup->sz*nctypelen(var_dup->type))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes for value buffer for variable %s in var_dup()\n",prg_nm_get(),var_dup->sz,nctypelen(var_dup->type),var_dup->nm);
      exit(EXIT_FAILURE); 
    } /* end if */ 
    (void)memcpy((void *)(var_dup->val.vp),(void *)(var->val.vp),var_dup->sz*nctypelen(var_dup->type));
  } /* end if */
  if(var->mss_val.vp != NULL){
    var_dup->mss_val.vp=(void *)malloc(nctypelen(var_dup->type));
    (void)memcpy((void *)(var_dup->mss_val.vp),(void *)(var->mss_val.vp),nctypelen(var_dup->type));
  } /* end if */
  if(var->tally != NULL){
    if((var_dup->tally=(long *)malloc(var_dup->sz*sizeof(long))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in var_dup()\n",prg_nm_get(),var_dup->sz,(long)sizeof(long),var_dup->nm);
      exit(EXIT_FAILURE); 
    } /* end if */ 
    (void)memcpy((void *)(var_dup->tally),(void *)(var->tally),var_dup->sz*sizeof(long));
  } /* end if */
  if(var->dim != NULL){
    var_dup->dim=(dim_sct **)malloc(var_dup->nbr_dim*sizeof(dim_sct *));
    (void)memcpy((void *)(var_dup->dim),(void *)(var->dim),var_dup->nbr_dim*sizeof(var->dim[0]));
  } /* end if */
  if(var->dim_id != NULL){
    var_dup->dim_id=(int *)malloc(var_dup->nbr_dim*sizeof(int));
    (void)memcpy((void *)(var_dup->dim_id),(void *)(var->dim_id),var_dup->nbr_dim*sizeof(var->dim_id[0]));
  } /* end if */
  if(var->cnt != NULL){
    var_dup->cnt=(long *)malloc(var_dup->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dup->cnt),(void *)(var->cnt),var_dup->nbr_dim*sizeof(var->cnt[0]));
  } /* end if */
  if(var->srt != NULL){
    var_dup->srt=(long *)malloc(var_dup->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dup->srt),(void *)(var->srt),var_dup->nbr_dim*sizeof(var->srt[0]));
  } /* end if */
  if(var->end != NULL){
    var_dup->end=(long *)malloc(var_dup->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dup->end),(void *)(var->end),var_dup->nbr_dim*sizeof(var->end[0]));
  } /* end if */

  return var_dup;

} /* end var_dup() */ 

dim_sct *
dim_dup(dim_sct *dim)
/* 
   dim_sct *dim: input dimension structure to duplicate
   dim_sct *dim_dup(): output copy of input dimension structure
 */ 
{
  /* Routine to malloc() and return a duplicate of the input dim_sct */ 

  dim_sct *dim_dup;

  dim_dup=(dim_sct *)malloc(sizeof(dim_sct));

  (void)memcpy((void *)dim_dup,(void *)dim,sizeof(dim_sct));

  return dim_dup;

} /* end dim_dup() */ 

void
var_get(int nc_id,var_sct *var)
/* 
   int nc_id: input netCDF file ID
   var_sct *var: input pointer to variable structure
 */ 
{
  /* Routine to allocate and retrieve the given variable hyperslab from disk memory */ 

  /* This is probably where scale_factor and add_offset unpacking should be done */ 

  if((var->val.vp=(void *)malloc(var->sz*nctypelen(var->type))) == NULL){
    (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes in var_get()\n",prg_nm_get(),var->sz,nctypelen(var->type));
    exit(EXIT_FAILURE); 
  } /* end if */ 
  if(var->sz > 1){
    (void)ncvarget(nc_id,var->id,var->srt,var->cnt,var->val.vp);
  }else{
    (void)ncvarget1(nc_id,var->id,var->srt,var->val.vp);
  } /* end else */
  
} /* end var_get() */ 

var_sct *
var_conform_dim(var_sct *var,var_sct *wgt,var_sct *wgt_crr)
/*  
   var_sct *var: input pointer to variable structure to serve as template
   var_sct *wgt: input pointer to variable structure to make conform to var
   var_sct *wgt_crr: input/output pointer to existing conforming variable structure (if any) (may be destroyed)
   var_sct *var_conform_dim(): output pointer to conforming variable structure
*/
{
  /* Routine to return a copy of the second variable which has been stretched and
     typecast to match the dimensions of the first variable. wgt's dimensions 
     must be a valid subset of var's dimensions to accomplish this. Dimensions
     in var which are not in wgt will be present in the wgt_out, with the values
     replicated from existing dimensions in wgt. */

  /* There are many inelegant ways to accomplish this (without using C++): */   

  /* Perhaps the most efficient method to accomplish this for the general case is to expand the
     weight array until it's the same size as the variable array, and then multiply the two
     together element-by-element in a highly vectorized loop, preferably in fortran. 
     To enhance speed, the (enlarged) weight-values array can be made static, and only destroyed 
     (and then recreated) when the dimensions of one of the incoming variables change.*/ 

  /* Another method for the general case, though an expensive one, is to use C to 
     figure out the multidimensional indices into the one dimensional hyperslab, 
     a la ncks. Knowing these indices, i can loop over the one-dimensional array
     element by element, choosing the appropriate index into the weight array from 
     those same multidimensional indices. This method can also create a static weight-value
     array that is only destroyed when an incoming variable changes dimensions from the
     previous variable. */ 

  /* Yet another method, which is not completely general, but which may be good enough for
     governement work, is to create fortran subroutines which expect variables of a given
     number of dimensions as input. Creating these functions for up to five dimensions would
     satisfy all foreseeable situations. The branch as to which function to call would be
     done based on the number of dimensions, here in the C code. C++ function overloading
     could accomplish some of this interface more elegantly than fortran, probably at a
     sacrifice in performance. */ 

  /* An (untested) simplification to some of these methods would be to copy the 1-D array
     value pointer of the variable and cast it to an N-D array pointer. Then C should be
     able to handle the indexing for me. This method could speed development of a working,
     but non-general, code, and later be replaced by a general method. Still, implementation
     of this method would require ugly branches or hard-to-understand recursive function
     calls. */ 
  
  /* Routine assumes, WLOG, that the weight will never have more dimensions than the variable
     (otherwise which hyperslab of the weight to use would be ill-defined). However, the
     weight may (and often will) have fewer dimensions than the variable. */

  bool CONFORMAL=False;

  int idx;
  int idx_dim;

  var_sct *wgt_out=NULL;

  /* Does the current weight (wgt_crr) conform to the variable's dimensions? */ 
  if(wgt_crr != NULL){
    /* Test rank first because wgt_crr because of 96/02/18 bug (invalid dim_id in old wgt_crr leads to match) */
    if(var->nbr_dim == wgt_crr->nbr_dim){
      for(idx=0;idx<var->nbr_dim;idx++){
	if(wgt_crr->dim_id[idx] != var->dim_id[idx]) break;
      } /* end loop over dimensions */
      if(idx == var->nbr_dim) CONFORMAL=True;
    } /* end if */ 
    if(CONFORMAL){
      wgt_out=wgt_crr;
    }else{
      wgt_crr=var_free(wgt_crr);
      wgt_out=NULL;
    } /* end if */ 
  } /* end if */

  /* Does original weight (wgt) conform to variable's dimensions? */ 
  if(wgt_out == NULL){
    if(var->nbr_dim > 0){
      /* First make sure all dimensions in wgt appear in var */ 
      for(idx=0;idx<wgt->nbr_dim;idx++){
        for(idx_dim=0;idx_dim<var->nbr_dim;idx_dim++){
          if(wgt->dim_id[idx] == var->dim_id[idx_dim]) break;
        } /* end loop over dimensions */
        if(idx_dim == var->nbr_dim){
	  (void)fprintf(stdout,"%s: ERROR the dimensions of %s are not a subset of those in %s\n",prg_nm_get(),wgt->nm,var->nm);
	  exit(EXIT_FAILURE);
        } /* end if */
      } /* end loop over dimensions */
      if(var->nbr_dim == wgt->nbr_dim){
	for(idx=0;idx<var->nbr_dim;idx++){
	  if(wgt->dim_id[idx] != var->dim_id[idx]) break;
	} /* end loop over dimensions */
      }else idx=-1; /* end if */
    }else{
      if(wgt->nbr_dim == 0) idx=0; else idx=1;
    } /* end else */
    if(idx == var->nbr_dim){
      wgt_out=var_dup(wgt);
      (void)var_xrf(wgt,wgt_out);
    } /* end if */ 
  } /* end if */

  if(wgt_out == NULL){
    /* We need to extend the original weight (wgt) to match the size of the current variable */ 

    char *wgt_cp;
    char *wgt_out_cp;

    int idx_wgt_var[MAX_NC_DIMS];
    int idx_var_wgt[MAX_NC_DIMS];
    int wgt_nbr_dim;
    int wgt_type_sz;
    int var_nbr_dim_m1;

    long *var_cnt;
    long dim_ss[MAX_NC_DIMS];
    long dim_var_map[MAX_NC_DIMS];
    long dim_wgt_map[MAX_NC_DIMS];
    long var_lmn;
    long wgt_lmn;
    long var_sz;

    /* Copy the basic attributes of the variable into the current weight */ 
    wgt_out=var_dup(var);
    (void)var_xrf(wgt,wgt_out);

    /* Modify a few elements of the weight array */ 
    wgt_out->nm=wgt->nm;
    wgt_out->id=wgt->id;
    wgt_out->type=wgt->type;
    wgt_out->val.vp=(void *)malloc(wgt_out->sz*nctypelen(wgt_out->type));
    wgt_cp=(char *)wgt->val.vp;
    wgt_out_cp=(char *)wgt_out->val.vp;
    wgt_type_sz=nctypelen(wgt_out->type);

    if(wgt_out->nbr_dim == 0){
      /* The variable (and weight) are scalars, not arrays */

      (void)memcpy(wgt_out_cp,wgt_cp,wgt_type_sz);

    }else{
      /* The variable (and weight) are arrays, not scalars */
      
      /* Create forward and reverse mappings from the variable's dimensions to the weight's:

	 dim_var_map[i] is the number of elements between one value of the i_th 
	 dimension of the variable and the next value of the i_th dimension, i.e., the number
	 of elements in memory between indicial increments in the i_th dimension. This is computed
	 as the product of one (1) times the size of all the dimensions (if any) after the i_th 
	 dimension in the variable.

	 dim_wgt_map[i] contains the analogous information, except for the original weight variable.

	 idx_wgt_var[i] contains the index into the variable's dimensions of the 
	 i_th dimension of the original weight.
	 idx_var_wgt[i] contains the index into the original weight's dimensions of the 
	 i_th dimension of the variable. 

	 NB: Since the weight is a subset of the variable, some of the elements of idx_var_wgt
	 may be "empty", or unused. 

	 NB: Since the mapping arrays (dim_var_map and dim_wgt_map) are ultimately used for a
	 memcpy() operation, they could (read: should) be computed as byte offsets, not type
	 offsets. This is why the netCDF generic hyperslab routines (ncvarputg(), ncvargetg())
	 request the imap vector to specify the offset (imap) vector in bytes.
	 
	 */

      for(idx=0;idx<wgt->nbr_dim;idx++){
	for(idx_dim=0;idx_dim<var->nbr_dim;idx_dim++){
	  if(var->dim_id[idx_dim] == wgt->dim_id[idx]){
	    idx_wgt_var[idx]=idx_dim;
	    idx_var_wgt[idx_dim]=idx;
	    break;
	  } /* end if */
	} /* end loop over variable dimensions */
      } /* end loop over weight dimensions */
      
      /* Figure out the map for each dimension of the variable */ 
      for(idx=0;idx<var->nbr_dim;idx++)	dim_var_map[idx]=1L;
      for(idx=0;idx<var->nbr_dim-1;idx++)
	for(idx_dim=idx+1;idx_dim<var->nbr_dim;idx_dim++)
	  dim_var_map[idx]*=var->cnt[idx_dim];
      
      /* Figure out the map for each dimension of the weight */ 
      for(idx=0;idx<wgt->nbr_dim;idx++)	dim_wgt_map[idx]=1L;
      for(idx=0;idx<wgt->nbr_dim-1;idx++)
	for(idx_dim=idx+1;idx_dim<wgt->nbr_dim;idx_dim++)
	  dim_wgt_map[idx]*=wgt->cnt[idx_dim];
      
      /* Define convenience variables to avoid repetitive indirect addressing */
      wgt_nbr_dim=wgt->nbr_dim;
      var_sz=var->sz;
      var_cnt=var->cnt;
      var_nbr_dim_m1=var->nbr_dim-1;

      /* var_lmn is the offset into the 1-D array corresponding to the N-D indices dim_ss */
      for(var_lmn=0;var_lmn<var_sz;var_lmn++){
	dim_ss[var_nbr_dim_m1]=var_lmn%var_cnt[var_nbr_dim_m1];
	for(idx=0;idx<var_nbr_dim_m1;idx++){
	  dim_ss[idx]=(long)(var_lmn/dim_var_map[idx]);
	  dim_ss[idx]%=var_cnt[idx];
	} /* end loop over dimensions */
	
	/* Map the (shared) N-D array indices into a 1-D index into the original weight data */ 
	wgt_lmn=0L;
	for(idx=0;idx<wgt_nbr_dim;idx++) wgt_lmn+=dim_ss[idx_wgt_var[idx]]*dim_wgt_map[idx];
	
	(void)memcpy(wgt_out_cp+var_lmn*wgt_type_sz,wgt_cp+wgt_lmn*wgt_type_sz,wgt_type_sz);
	
      } /* end loop over var_lmn */
      
    } /* end if the variable (and weight) are arrays, not scalars */
    
  } /* end if we had to stretch the weight to fit the variable */
  
  /* The current weight (wgt_out) now conforms to the current variable */ 
  return wgt_out;
  
} /* end var_wgt */ 

void
var_dim_xrf(var_sct *var)
/*  
   var_sct *var: input pointer to variable structure
*/
{
  /* Switch the pointers to the dimension structures so that var->dim points to var->dim->xrf.
     Useful to make the dim element of a variable structure from var_dup() refer to the counterparts
     of the dimensions directly associated with the variable it was duplicated from. */
  
  int idx;
  
  for(idx=0;idx<var->nbr_dim;idx++) var->dim[idx]=var->dim[idx]->xrf;
  
} /* end var_xrf */ 

void
dim_xrf(dim_sct *dim,dim_sct *dim_dup)
/*  
   dim_sct *dim: input/output pointer to dimension structure
   dim_sct *dim: input/output pointer to dimension structure
*/
{
  /* Make the xrf elements of the dimension structures point to eachother.  */ 

  dim->xrf=dim_dup;
  dim_dup->xrf=dim;

} /* end dim_xrf */ 

void
var_xrf(var_sct *var,var_sct *var_dup)
/*  
   var_sct *var: input/output pointer to variable structure
   var_sct *var: input/output pointer to variable structure
*/
{
  /* Make the xrf elements of the variable structures point to eachother. */ 

  var->xrf=var_dup;
  var_dup->xrf=var;

} /* end var_xrf */ 

var_sct *
var_conform_type(nc_type var_out_type,var_sct *var_in)
/*  
   nc_type *var_out_type: input type to convert variable structure to
   var_sct *var_in: input/output pointer to variable structure (may be destroyed)
   var_sct *var_conform_type(): output point to variable structure of type var_out_type
*/
{
  /* Routine to typecast and copy the values of the variable to the desired type */ 

  long idx;
  long sz;
  
  nc_type var_in_type;
  
  ptr_unn val_in;
  ptr_unn val_out;

  var_sct *var_out;

  /* Do the types already match? */ 
  if(var_in->type == var_out_type) return var_in;

  var_out=var_in;
  
  var_in_type=var_in->type;
  
  /* Simple error-checking and diagnostics */ 
  if(dbg_lvl_get() > 2){
    (void)fprintf(stderr,"%s: WARNING Converting variable %s from type %s to %s\n",prg_nm_get(),var_in->nm,nc_type_nm(var_in_type),nc_type_nm(var_out_type));
  } /* end if */
  
  /* Move the current var values to a swap location */ 
  val_in=var_in->val;
  
  /* Allocate space for the type-conforming values */ 
  var_out->type=var_out_type;
  var_out->val.vp=(void *)malloc(var_out->sz*nctypelen(var_out->type));
  
  /* Define convenience variables to avoid repetitive indirect addressing */
  sz=var_out->sz;
  val_out=var_out->val;
  
  /* Copy and typecast the missing_value attribute, if any */ 
  if(var_out->has_mss_val){
    ptr_unn var_in_mss_val;

    /* NB: The sequence of the following commands is important (copy before overwriting!) */ 
    var_in_mss_val=var_out->mss_val;
    var_out->mss_val.vp=(void *)malloc(nctypelen(var_out->type));
    (void)val_conform_type(var_in_type,var_in_mss_val,var_out_type,var_out->mss_val);
    /* Free the original */ 
    (void)free(var_in_mss_val.vp);
  } /* end if */

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(var_in->type,&val_in);
  (void)cast_void_nctype(var_out->type,&var_out->val);
  
  /* Copy and typecast the entire array of values, using implicit coercion rules of C */ 
  switch(var_out_type){
  case NC_FLOAT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.dp[idx];} break; 
    case NC_LONG: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.bp[idx];} break;
    } break;
  case NC_DOUBLE:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.dp[idx];} break; 
    case NC_LONG: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.bp[idx];} break;
    } break;
  case NC_LONG:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.dp[idx];} break; 
    case NC_LONG: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.bp[idx];} break;
    } break;
  case NC_SHORT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.dp[idx];} break; 
    case NC_LONG: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.bp[idx];} break;
    } break;
  case NC_CHAR:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.dp[idx];} break; 
    case NC_LONG: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.bp[idx];} break;
    } break;
  case NC_BYTE:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.dp[idx];} break; 
    case NC_LONG: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.bp[idx];} break;
    } break;
  } /* end switch */ 
  
  /* Un-typecast the pointer to values after access */
  (void)cast_nctype_void(var_in->type,&val_in);
  (void)cast_nctype_void(var_out->type,&var_out->val);
  
  /* Free input variable data */
  (void)free(val_in.vp);
  
  return var_out;
  
} /* end var_conform_type */ 

void
val_conform_type(nc_type type_in,ptr_unn val_in,nc_type type_out,ptr_unn val_out)
/*  
   nc_type type_in: input type of the input value
   ptr_unn val_in: input pointer to the input value
   nc_type type_out: input type of the output value
   ptr_unn val_out: input pointer to the output value
*/
{
  /* Routine to fill val_out with a copy of val_in that has been typecast from 
     type_in to type_out. The last-referenced state of both value pointers is
     assumed to be .vp, and the val_out union is returned in that state. */

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type_in,&val_in);
  (void)cast_void_nctype(type_out,&val_out);
  
  /* Copy and typecast the single value using the implicit coercion rules of C */
  switch(type_out){
  case NC_FLOAT:
    switch(type_in){
    case NC_FLOAT: *val_out.fp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.fp=*val_in.dp; break; 
    case NC_LONG: *val_out.fp=*val_in.lp; break;
    case NC_SHORT: *val_out.fp=*val_in.sp; break;
    case NC_CHAR: *val_out.fp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.fp=*val_in.bp; break;
    } break;
  case NC_DOUBLE:
    switch(type_in){
    case NC_FLOAT: *val_out.dp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.dp=*val_in.dp; break; 
    case NC_LONG: *val_out.dp=*val_in.lp; break;
    case NC_SHORT: *val_out.dp=*val_in.sp; break;
    case NC_CHAR: *val_out.dp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.dp=*val_in.bp; break;
    } break;
  case NC_LONG:
    switch(type_in){
    case NC_FLOAT: *val_out.lp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.lp=*val_in.dp; break; 
    case NC_LONG: *val_out.lp=*val_in.lp; break;
    case NC_SHORT: *val_out.lp=*val_in.sp; break;
    case NC_CHAR: *val_out.lp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.lp=*val_in.bp; break;
    } break;
  case NC_SHORT:
    switch(type_in){
    case NC_FLOAT: *val_out.sp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.sp=*val_in.dp; break; 
    case NC_LONG: *val_out.sp=*val_in.lp; break;
    case NC_SHORT: *val_out.sp=*val_in.sp; break;
    case NC_CHAR: *val_out.sp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.sp=*val_in.bp; break;
    } break;
  case NC_CHAR:
    switch(type_in){
    case NC_FLOAT: *val_out.cp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.cp=*val_in.dp; break; 
    case NC_LONG: *val_out.cp=*val_in.lp; break;
    case NC_SHORT: *val_out.cp=*val_in.sp; break;
    case NC_CHAR: *val_out.cp=*val_in.cp; break;
    case NC_BYTE: *val_out.cp=*val_in.bp; break;
    } break;
  case NC_BYTE:
    switch(type_in){
    case NC_FLOAT: *val_out.bp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.bp=*val_in.dp; break; 
    case NC_LONG: *val_out.bp=*val_in.lp; break;
    case NC_SHORT: *val_out.bp=*val_in.sp; break;
    case NC_CHAR: *val_out.bp=*val_in.cp; break;
    case NC_BYTE: *val_out.bp=*val_in.bp; break;
    } break;
  } /* end switch */ 
  
  /* NB: There is no need to un-typecast input pointers because they were passed by
     value and are thus purely local to this routine. The only thing changed by this
     routine is the contents of the location pointed to by the pointer to the output value. */
  
} /* end val_conform_type */ 

var_sct *
var_avg(var_sct *var,dim_sct **dim,int nbr_dim)
/*  
   var_sct *var: input/output pointer to variable structure (destroyed)
   dim_sct **dim: input pointer to list of dimension structures
   int nbr_dim: input number of structures in list
   var_sct *var_avg(): output pointer to averaged variable
*/
{
  /* Routine to average given variable over given dimensions. The input variable 
     structure is destroyed and the routine returns the resized, averaged variable. */ 

  /* Create output variable as a duplicate of the input variable, except for the dimensions
     which are to be averaged over */ 

  /* NB: var_avg() overwrites the contents, if any, of the tally array with the number of valid
     averaging operations */ 

  /* Their are three variables to keep track of in this routine, their abbreviations are:
     var: the input variable
     avg: a hyperslab of var that gets averaged
     fix: the output (averaged) variable
   */ 

  dim_sct **dim_avg;
  dim_sct **dim_fix;

  int idx_avg_var[MAX_NC_DIMS];
  int idx_var_avg[MAX_NC_DIMS];
  int idx_fix_var[MAX_NC_DIMS];
  int idx_var_fix[MAX_NC_DIMS];
  int idx;
  int idx_dim;
  int nbr_dim_avg;
  int nbr_dim_fix;
  int nbr_dim_var;

  long avg_sz;
  long fix_sz;
  long var_sz;

  var_sct *fix;

  /* Copy the basic attributes of the input variable into the output (averaged) variable */ 
  fix=var_dup(var);
  (void)var_xrf(fix,var->xrf);

  /* Create lists of the averaging and fixed dimensions (in order of their appearance 
     in the variable). We do not know a priori how many dimensions remain in the 
     ouput (averaged) variable, but nbr_dim_var is an upper bound. Similarly, we do
     not know a priori how many of the dimensions in the input list of averaging 
     dimensions (dim) actually occur in the current variable, so we do not know
     nbr_dim_avg, but nbr_dim is an upper bound on it. */
  nbr_dim_var=var->nbr_dim;
  nbr_dim_fix=0;
  nbr_dim_avg=0;
  dim_avg=(dim_sct **)malloc(nbr_dim*sizeof(dim_sct *));
  dim_fix=(dim_sct **)malloc(nbr_dim_var*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_var;idx++){
    for(idx_dim=0;idx_dim<nbr_dim;idx_dim++){
      if(var->dim_id[idx] == dim[idx_dim]->id){
	dim_avg[nbr_dim_avg]=dim[idx_dim];
	idx_avg_var[nbr_dim_avg]=idx;
	idx_var_avg[idx]=nbr_dim_avg;
	nbr_dim_avg++;
	break;
      } /* end if */ 
    } /* end loop over idx_dim */
    if(idx_dim == nbr_dim){
      dim_fix[nbr_dim_fix]=var->dim[idx];
      idx_fix_var[nbr_dim_fix]=idx;
      idx_var_fix[idx]=nbr_dim_fix;
      nbr_dim_fix++;
    } /* end if */ 
  } /* end loop over idx */

  /* Free the extra list space */ 
  dim_fix=(dim_sct **)realloc(dim_fix,nbr_dim_fix*sizeof(dim_sct *));
  dim_avg=(dim_sct **)realloc(dim_avg,nbr_dim_avg*sizeof(dim_sct *));

  if(nbr_dim_avg == 0){
    (void)fprintf(stderr,"%s: WARNING %s does not contain any averaging dimensions\n",prg_nm_get(),fix->nm);
    return (var_sct *)NULL;
  } /* end if */ 

  /* Get rid of the averaged dimensions */ 
  fix->nbr_dim=nbr_dim_fix;

  avg_sz=1L;
  for(idx=0;idx<nbr_dim_avg;idx++){
    avg_sz*=dim_avg[idx]->cnt;
    fix->sz/=dim_avg[idx]->cnt;
    if(!dim_avg[idx]->is_rec_dim) fix->sz_rec/=dim_avg[idx]->cnt;
  } /* end loop over idx */

  fix->is_rec_var=False;
  for(idx=0;idx<nbr_dim_fix;idx++){
    if(dim_fix[idx]->is_rec_dim) fix->is_rec_var=True;
    fix->dim[idx]=dim_fix[idx];
    fix->dim_id[idx]=dim_fix[idx]->id;
    fix->srt[idx]=var->srt[idx_fix_var[idx]];
    fix->cnt[idx]=var->cnt[idx_fix_var[idx]];
    fix->end[idx]=var->end[idx_fix_var[idx]];
  } /* end loop over idx */
  
  fix->is_crd_var=False;
  if(nbr_dim_fix == 1)
    if(dim_fix[0]->is_crd_dim) 
      fix->is_crd_var=True;

  /* Trim the dimension arrays to their new sizes */ 
  fix->dim=(dim_sct **)realloc(fix->dim,nbr_dim_fix*sizeof(dim_sct *));
  fix->dim_id=(int *)realloc(fix->dim_id,nbr_dim_fix*sizeof(int));
  fix->srt=(long *)realloc(fix->srt,nbr_dim_fix*sizeof(long));
  fix->cnt=(long *)realloc(fix->cnt,nbr_dim_fix*sizeof(long));
  fix->end=(long *)realloc(fix->end,nbr_dim_fix*sizeof(long));
  
  /* If the product of the sizes of all the averaging dimensions is 1, the input and output value arrays 
     should be identical. Since var->val was already copied to fix->val by var_dup() at the beginning
     of this routine, there is only one task remaining, to set fix->tally appropriately. */
  if(avg_sz == 1L){
    long fix_sz;
    long *fix_tally;

    fix_sz=fix->sz;
    fix_tally=fix->tally;

    /* First set the tally field to 1 */ 
    for(idx=0;idx<fix_sz;idx++) fix_tally[idx]=1L;
    /* Next overwrite any missing value locations with zero */ 
    if(fix->has_mss_val){
      int val_sz_byte;

      char *val;
      char *mss_val;

      /* NB: Use char * rather than void * because some compilers (acc) will not do pointer
	 arithmetic on void * */
      mss_val=(char *)fix->mss_val.vp;
      val_sz_byte=nctypelen(fix->type);
      val=(char *)fix->val.vp;
      for(idx=0;idx<fix_sz;idx++,val+=val_sz_byte)
	if(!memcmp(val,mss_val,val_sz_byte)) fix_tally[idx]=0L;
    } /* fix->has_mss_val */
  } /* end if avg_sz == 1L */ 

  /* Starting at the first element of the input hyperslab, add up the next stride elements
     and place the result in the first element of the output hyperslab. */ 
  if(avg_sz != 1L){
    char *avg_cp;
    char *fix_cp;
    char *var_cp;
    
    int type_sz;
    int nbr_dim_var_m1;

    long *var_cnt;
    long avg_off;
    long avg_lmn;
    long fix_lmn;
    long var_lmn;
    long dim_ss[MAX_NC_DIMS];
    long dim_var_map[MAX_NC_DIMS];
    long dim_avg_map[MAX_NC_DIMS];
    long dim_fix_map[MAX_NC_DIMS];

    nc_type avg_type;

    ptr_unn avg_val;

    /* Define convenience variables to avoid repetitive indirect addressing */
    avg_type=fix->type;
    fix_sz=fix->sz;
    nbr_dim_var_m1=nbr_dim_var-1;
    type_sz=nctypelen(fix->type);
    var_cnt=var->cnt;
    var_cp=(char *)var->val.vp;
    var_sz=var->sz;
    
    /* Reuse the existing value buffer (it is of size var_sz, created by var_dup())*/ 
    avg_val=fix->val;
    avg_cp=(char *)avg_val.vp;
    /* Create a new value buffer for the output (averaged) size */ 
    fix->val.vp=(void *)malloc(fix->sz*nctypelen(fix->type));
    fix_cp=(char *)fix->val.vp;
    /* Resize (or just plain allocate) the tally array */ 
    fix->tally=(long *)realloc(fix->tally,fix->sz*sizeof(long));

    /* Re-initialize the value and tally arrays */ 
    (void)zero_long(fix->sz,fix->tally);
    (void)var_zero(fix->type,fix->sz,fix->val);
  
    /* Figure out the map for each dimension of the variable */ 
    for(idx=0;idx<nbr_dim_var;idx++) dim_var_map[idx]=1L;
    for(idx=0;idx<nbr_dim_var-1;idx++)
      for(idx_dim=idx+1;idx_dim<nbr_dim_var;idx_dim++)
	dim_var_map[idx]*=var->cnt[idx_dim];
    
    /* Figure out the map for each dimension of the output variable */ 
    for(idx=0;idx<nbr_dim_fix;idx++) dim_fix_map[idx]=1L;
    for(idx=0;idx<nbr_dim_fix-1;idx++)
      for(idx_dim=idx+1;idx_dim<nbr_dim_fix;idx_dim++)
	dim_fix_map[idx]*=fix->cnt[idx_dim];
    
    /* Figure out the map for each dimension of the averaging buffer */ 
    for(idx=0;idx<nbr_dim_avg;idx++) dim_avg_map[idx]=1L;
    for(idx=0;idx<nbr_dim_avg-1;idx++)
      for(idx_dim=idx+1;idx_dim<nbr_dim_avg;idx_dim++)
	dim_avg_map[idx]*=dim_avg[idx_dim]->cnt;
    
    /* var_lmn is the offset into the 1-D array */
    for(var_lmn=0;var_lmn<var_sz;var_lmn++){

      /* dim_ss are the corresponding indices (subscripts) into the N-D array */
      dim_ss[nbr_dim_var_m1]=var_lmn%var_cnt[nbr_dim_var_m1];
      for(idx=0;idx<nbr_dim_var_m1;idx++){
	dim_ss[idx]=(long)(var_lmn/dim_var_map[idx]);
	dim_ss[idx]%=var_cnt[idx];
      } /* end loop over dimensions */

      /* Map variable's N-D array indices into a 1-D index into the averaged data */ 
      fix_lmn=0L;
      for(idx=0;idx<nbr_dim_fix;idx++) fix_lmn+=dim_ss[idx_fix_var[idx]]*dim_fix_map[idx];
      
      /* Map N-D array indices into a 1-D offset from the offset of its group */ 
      avg_lmn=0L;
      for(idx=0;idx<nbr_dim_avg;idx++) avg_lmn+=dim_ss[idx_avg_var[idx]]*dim_avg_map[idx];
      
      /* Copy current element in the input array into its slot in the sorted avg_val */ 
      (void)memcpy(avg_cp+(fix_lmn*avg_sz+avg_lmn)*type_sz,var_cp+var_lmn*type_sz,type_sz);
      
    } /* end loop over var_lmn */
    
    /* Input data are now sorted and stored (in avg_val) in blocks (of length avg_sz)
       in the same order as the blocks' average values will appear in the output buffer. 
       An averaging routine can take advantage of this by casting avg-val to a two dimensional
       variable and averaging over the inner dimension. */ 
    (void)var_avg_reduce(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,fix->tally,avg_val,fix->val);

    /* Free dynamic memory that held the rearranged input variable values */ 
    (void)free(avg_val.vp);
  } /* end if avg_sz != 1 */
  
  /* Free the input variable */ 
  var=var_free(var);
  (void)free(dim_avg);
  (void)free(dim_fix);

  /* Return the averaged variable */ 
  return fix;

} /* end var_avg() */ 

var_sct *
var_free(var_sct *var)
/*  
   var_sct *var: input pointer to variable structure
*/
{
  /* Routine to free all the space associated with a dynamically allocated variable structure */ 
  
  /* NB: var->nm is not freed because i decided to let names be static memory, and refer to
     the optarg list if available. This assumption needs to be changed before freeing 
     the name pointer. */ 
  
  if(var->val.vp != NULL){(void)free(var->val.vp); var->val.vp=NULL;}
  if(var->mss_val.vp != NULL){(void)free(var->mss_val.vp); var->mss_val.vp=NULL;}
  if(var->tally != NULL){(void)free(var->tally); var->tally=NULL;}
  if(var->dim_id != NULL){(void)free(var->dim_id); var->dim_id=NULL;}
  if(var->dim != NULL){(void)free(var->dim); var->dim=NULL;}
  if(var->srt != NULL){(void)free(var->srt); var->srt=NULL;}
  if(var->end != NULL){(void)free(var->end); var->end=NULL;}
  if(var->cnt != NULL){(void)free(var->cnt); var->cnt=NULL;}

  (void)free(var);

  return NULL;

} /* end var_free */ 

bool
arm_inq(int nc_id)
/*  
   int nc_id: input netCDF file ID
   bool arm_inq(): output whether the file is an ARM data file
*/
{
  /* Routine to check whether the file adheres to ARM time format */
  bool ARM_FORMAT;

  int time_dim_id;
  int base_time_id;
  int time_offset_id;
  
  /* Look for the signature of an ARM file */ 
  ncopts=0;
  time_dim_id=ncdimid(nc_id,"time");
  base_time_id=ncvarid(nc_id,"base_time");
  time_offset_id=ncvarid(nc_id,"time_offset");
  ncopts=NC_VERBOSE | NC_FATAL; 

  if(time_dim_id == -1 || base_time_id == -1 || time_offset_id == -1){
    ARM_FORMAT=False;
  }else{
    (void)fprintf(stderr,"%s: CONVENTION File convention is DOE ARM\n",prg_nm_get()); 
    ARM_FORMAT=True;
  } /* end else */ 

  return ARM_FORMAT;
} /* end arm_inq */ 

nclong
arm_base_time_get(int nc_id)
/*  
   int nc_id: input netCDF file ID
   nclong arm_base_time_get: output value of base_time variable
*/
{
  /* Routine to check whether the file adheres to ARM time format */

  int base_time_id;

  nclong base_time;

  base_time_id=ncvarid(nc_id,"base_time");
  (void)ncvarget1(nc_id,base_time_id,0L,&base_time);

  return base_time;
} /* end arm_base_time_get */ 

void
var_multiply(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: input netCDF type of the operands
  long sz: input size (in elements) of the operands
  int has_mss_val: input flag for missing values
  ptr_unn mss_val: input value of missing value
  ptr_unn op1: input values of first operand
  ptr_unn op2: input/output values of second operand on input, values of product on output
 */ 
{
  /* Routine to multiply the value of the first operand by the value of the second operand 
     and store the result in the second operand. The operands are assumed to have conforming
     dimensions, and to both be of the specified type. The operands' values are 
     assumed to be in memory already. */ 
  
  /* Division is currently defined as op2:=op1*op2 */   

  long idx;
  
  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
#ifndef C_ONLY
    (void)FORTRAN_multiply_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]*=op1.fp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != *mss_val.fp) && (op1.fp[idx] != *mss_val.fp)) op2.fp[idx]*=op1.fp[idx]; else op2.fp[idx]=*mss_val.fp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */
    break;
  case NC_DOUBLE:
#ifndef C_ONLY
    (void)FORTRAN_multiply_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]*=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]*=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */
    break;
  case NC_LONG:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]*=op1.lp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != *mss_val.lp) && (op1.lp[idx] != *mss_val.lp)) op2.lp[idx]*=op1.lp[idx]; else op2.lp[idx]=*mss_val.lp;
      } /* end for */ 
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]*=op1.sp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != *mss_val.sp) && (op1.sp[idx] != *mss_val.sp)) op2.sp[idx]*=op1.sp[idx]; else op2.sp[idx]=*mss_val.sp;
      } /* end for */ 
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */ 
    break;
  case NC_BYTE:
    /* Do nothing */ 
    break;
  } /* end switch */ 

  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 
  
} /* end var_multiply() */ 

void
var_divide(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: input netCDF type of the operands
  long sz: input size (in elements) of the operands
  int has_mss_val: input flag for missing values
  ptr_unn mss_val: input value of missing value
  ptr_unn op1: input values of first operand
  ptr_unn op2: input/outpue values of second operand on input, values of ratio on output
 */ 
{
  /* Routine to divide the value of the first operand by the value of the second operand 
     and store the result in the second operand. The operands are assumed to have conforming
     dimensions, and to both be of the specified type. The operands' values are 
     assumed to be in memory already. */ 

  /* Division is currently defined as op2:=op2/op1 */   

  long idx;
  
  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
#ifndef C_ONLY
    (void)FORTRAN_divide_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]/=op1.fp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != *mss_val.fp) && (op1.fp[idx] != *mss_val.fp)) op2.fp[idx]/=op1.fp[idx]; else op2.fp[idx]=*mss_val.fp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */
    break;
  case NC_DOUBLE:
#ifndef C_ONLY
    (void)FORTRAN_divide_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]/=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]/=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */
    break;
  case NC_LONG:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]/=op1.lp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != *mss_val.lp) && (op1.lp[idx] != *mss_val.lp)) op2.lp[idx]/=op1.lp[idx]; else op2.lp[idx]=*mss_val.lp;
      } /* end for */ 
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]/=op1.sp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != *mss_val.sp) && (op1.sp[idx] != *mss_val.sp)) op2.sp[idx]/=op1.sp[idx]; else op2.sp[idx]=*mss_val.sp;
      } /* end for */ 
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */ 
    break;
  case NC_BYTE:
    /* Do nothing */ 
    break;
  } /* end switch */ 

  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 
  
} /* end var_divide() */ 

void
var_add(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: input netCDF type of the operands
  long sz: input size (in elements) of the operands
  int has_mss_val: input flag for missing values
  ptr_unn mss_val: input value of missing value
  long *tally: input/output counter space
  ptr_unn op1: input values of first operand
  ptr_unn op2: input/outpue values of second operand on input, values of sum on output
 */ 
{
  /* Routine to add the value of the first operand to the value of the second operand 
     and store the result in the second operand. The operands are assumed to have conforming
     dimensions, and be of the specified type. The operands' values are 
     assumed to be in memory already. */ 

  /* Addition is currently defined as op2:=op1+op2 */

  long idx;

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifndef C_ONLY
    (void)FORTRAN_add_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */ 
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != *mss_val.fp) && (op1.fp[idx] != *mss_val.fp)){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */ 
    } /* end else */
#endif /* !F77 */
    break;
  case NC_DOUBLE:
#ifndef C_ONLY
    (void)FORTRAN_add_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */ 
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */ 
    } /* end else */
#endif /* !F77 */
    break;
  case NC_LONG:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.lp[idx]+=op1.lp[idx];
	tally[idx]++;
      } /* end for */ 
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != *mss_val.lp) && (op1.lp[idx] != *mss_val.lp)){
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
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != *mss_val.sp) && (op1.sp[idx] != *mss_val.sp)){
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
  } /* end switch */ 

  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 

} /* end var_add() */ 

void
var_avg_reduce(nc_type type,long sz_op1,long sz_op2,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: input netCDF type of the operands
  long sz_op1: input size (in elements) of op1
  long sz_op2: input size (in elements) of op2
  int has_mss_val: input flag for missing values
  ptr_unn mss_val: input value of missing value
  long *tally: input/output counter space
  ptr_unn op1: input values of first operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2))
  ptr_unn op2: output values resulting from averaging each block of input operand
 */ 
{
  /* Routine to average the value of each contiguous block of the first operand and place the
     value in the corresponding element in the second operand. The operands are assumed to have 
     conforming types, but not dimensions or sizes. */

  long idx_op2;
  long idx_blk;
  long sz_blk;

  sz_blk=sz_op1/sz_op2;

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifndef C_ONLY
    (void)FORTRAN_avg_reduce_real(&sz_blk,&sz_op2,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else
#if ( defined SUN4 ) || ( defined SGI64 )
    /* NB: ANSI compliant branch */ 
    if(True){
      long blk_off;
      long idx_op1;

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
	    if(op1.fp[idx_op1] != *mss_val.fp){
	      op2.fp[idx_op2]+=op1.fp[idx_op1];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */ 
#else /* !SUN4 */ 
    /* NB: Initializes variable-size array. Not ANSI compliant, but more elegant. */ 
    if(True){
      float op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nctypelen(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != *mss_val.fp){
	      op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* !SUN4 */ 
#endif /* !F77 */
    break;
  case NC_DOUBLE:
#ifndef C_ONLY
    (void)FORTRAN_avg_reduce_double_precision(&sz_blk,&sz_op2,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else
#if ( defined SUN4 ) || ( defined SGI64 )
    /* NB: ANSI compliant branch */ 
    if(True){
      long blk_off;
      long idx_op1;

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
	    if(op1.dp[idx_op1] != *mss_val.dp){
	      op2.dp[idx_op2]+=op1.dp[idx_op1];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */ 
#else /* !SUN4 */ 
    /* NB: Initializes variable-size array. Not ANSI compliant, but more elegant. */ 
    if(True){
      double op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nctypelen(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != *mss_val.dp){
	      op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* !SUN4 */ 
#endif /* !F77 */
    break;
  case NC_LONG:
#if ( defined SUN4 ) || ( defined SGI64 )
    /* NB: ANSI compliant branch */ 
    if(True){
      long blk_off;
      long idx_op1;

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
	    if(op1.lp[idx_op1] != *mss_val.lp){
	      op2.lp[idx_op2]+=op1.lp[idx_op1];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */ 
#else /* !SUN4 */ 
    /* NB: Initializes variable-size array. Not ANSI compliant, but more elegant. */ 
    if(True){
      long op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.lp),sz_op1*nctypelen(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.lp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != *mss_val.lp){
	      op2.lp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* !SUN4 */ 
    break;
  case NC_SHORT:
#if ( defined SUN4 ) || ( defined SGI64 )
    /* NB: ANSI compliant branch */ 
    if(True){
      long blk_off;
      long idx_op1;

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
	    if(op1.sp[idx_op1] != *mss_val.sp){
	      op2.sp[idx_op2]+=op1.sp[idx_op1];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */ 
#else /* !SUN4 */ 
    /* NB: Initializes variable-size array. Not ANSI compliant, but more elegant. */ 
    if(True){
      short op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nctypelen(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != *mss_val.sp){
	      op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* !SUN4 */ 
    break;
  case NC_CHAR:
    /* Do nothing */ 
    break;
  case NC_BYTE:
    /* Do nothing */ 
    break;
  } /* end switch */ 
  
  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 

} /* end var_avg_reduce() */ 

void
var_normalize(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1)
/* 
  nc_type type: input netCDF type of the operand
  long sz: input size (in elements) of the operand
  int has_mss_val: input flag for missing values
  ptr_unn mss_val: input value of missing value
  long *tally: input counter space
  ptr_unn op1: input/output values of first operand on input, the normalized result on output
 */ 
{
  /* Routine to normalize the value of the first operand by the count in the tally array 
     and store the result in the first operand. */

  /* Normalization is currently defined as op1:=op1/tally */   

  long idx;

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifndef C_ONLY
    (void)FORTRAN_normalize_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=tally[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if(tally[idx] != 0L) op1.fp[idx]/=tally[idx]; else op1.fp[idx]=*mss_val.fp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */ 
    break;
  case NC_DOUBLE:
#ifndef C_ONLY
    (void)FORTRAN_normalize_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if(tally[idx] != 0L) op1.dp[idx]/=tally[idx]; else op1.dp[idx]=*mss_val.dp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */ 
    break;
  case NC_LONG:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=tally[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if(tally[idx] != 0L) op1.lp[idx]/=tally[idx]; else op1.lp[idx]=*mss_val.lp;
      } /* end for */ 
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if(tally[idx] != 0L) op1.sp[idx]/=tally[idx]; else op1.sp[idx]=*mss_val.sp;
      } /* end for */ 
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */ 
    break;
  case NC_BYTE:
    /* Do nothing */ 
    break;
  } /* end switch */ 

  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 

} /* end var_normalize() */ 

ptr_unn
mss_val_mk(nc_type type)
/* 
  nc_type type: input netCDF type of the operand
  ptr_unn mss_val_mk(): output ptr_unn containing the default missing value for type type
 */ 
{
  /* Routine to return a pointer union containing the default missing value for type type */

  ptr_unn mss_val;

  mss_val.vp=(void *)malloc(nctypelen(type));

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT: *mss_val.fp=FILL_FLOAT; break; 
  case NC_DOUBLE: *mss_val.dp=FILL_DOUBLE; break; 
  case NC_LONG: *mss_val.lp=FILL_LONG; break;
  case NC_SHORT: *mss_val.sp=FILL_SHORT; break;
  case NC_CHAR: *mss_val.cp=FILL_CHAR; break;
  case NC_BYTE: *mss_val.bp=FILL_BYTE; break;
  } /* end switch */ 

  /* Un-typecast the pointer to the values after access */
  (void)cast_nctype_void(type,&mss_val);
  
  return mss_val;

} /* end mss_val_mk() */ 
  
void
var_mask(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,double op1,int op_type,ptr_unn op2,ptr_unn op3)
/* 
  nc_type type: input netCDF type of the operand
  long sz: input size (in elements) of the operand
  int has_mss_val: input flag for missing values
  ptr_unn mss_val: input value of missing value
  double op1: input value of the mask field corresponding to input to be processed
  int op_type: input type of relationship to test for between op2 and op1
  ptr_unn op2: input values of the mask field
  ptr_unn op3: input/output values of second operand on input, the masked values on output
 */ 
{
  /* Routine to mask the third operand by the second operand. Wherever the second operand does not 
     equal the first operand the third operand will be set to its missing value. */

  /* Masking is currently defined as if(op2 !op_type op1) then op3:=mss_val */   

  long idx;

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op2);
  (void)cast_void_nctype(type,&op3);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  /* NB: the explicit coercion when comparing op2 to op1 is necessary */ 
  switch(type){
  case NC_FLOAT:
    switch(op_type){
    case nc_op_eq: for(idx=0;idx<sz;idx++) if(op2.fp[idx] != (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nc_op_ne: for(idx=0;idx<sz;idx++) if(op2.fp[idx] == (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nc_op_lt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >= (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nc_op_gt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <= (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nc_op_le: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >  (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nc_op_ge: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <  (float)op1) op3.fp[idx]=*mss_val.fp; break;
    } /* end switch */ 
    break;
  case NC_DOUBLE:
    switch(op_type){
    case nc_op_eq: for(idx=0;idx<sz;idx++) if(op2.dp[idx] != (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nc_op_ne: for(idx=0;idx<sz;idx++) if(op2.dp[idx] == (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nc_op_lt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >= (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nc_op_gt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <= (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nc_op_le: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >  (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nc_op_ge: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <  (double)op1) op3.dp[idx]=*mss_val.dp; break;
    } /* end switch */ 
    break;
  case NC_LONG:
    switch(op_type){
    case nc_op_eq: for(idx=0;idx<sz;idx++) if(op2.lp[idx] != (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nc_op_ne: for(idx=0;idx<sz;idx++) if(op2.lp[idx] == (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nc_op_lt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >= (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nc_op_gt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <= (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nc_op_le: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >  (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nc_op_ge: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <  (long)op1) op3.lp[idx]=*mss_val.lp; break;
    } /* end switch */ 
    break;
  case NC_SHORT:
    switch(op_type){
    case nc_op_eq: for(idx=0;idx<sz;idx++) if(op2.sp[idx] != (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nc_op_ne: for(idx=0;idx<sz;idx++) if(op2.sp[idx] == (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nc_op_lt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >= (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nc_op_gt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <= (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nc_op_le: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >  (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nc_op_ge: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <  (short)op1) op3.sp[idx]=*mss_val.sp; break;
    } /* end switch */ 
    break;
  case NC_CHAR:
    switch(op_type){
    case nc_op_eq: for(idx=0;idx<sz;idx++) if(op2.cp[idx] != (signed char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nc_op_ne: for(idx=0;idx<sz;idx++) if(op2.cp[idx] == (signed char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nc_op_lt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >= (signed char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nc_op_gt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <= (signed char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nc_op_le: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >  (signed char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nc_op_ge: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <  (signed char)op1) op3.cp[idx]=*mss_val.cp; break;
    } /* end switch */ 
    break;
  case NC_BYTE:
    switch(op_type){
    case nc_op_eq: for(idx=0;idx<sz;idx++) if(op2.bp[idx] != (unsigned char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nc_op_ne: for(idx=0;idx<sz;idx++) if(op2.bp[idx] == (unsigned char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nc_op_lt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >= (unsigned char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nc_op_gt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <= (unsigned char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nc_op_le: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >  (unsigned char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nc_op_ge: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <  (unsigned char)op1) op3.bp[idx]=*mss_val.bp; break;
    } /* end switch */ 
    break;
  } /* end switch */ 

  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 

} /* end var_mask() */ 

void
var_zero(nc_type type,long sz,ptr_unn op1)
/* 
  nc_type type: input netCDF type of the operand
  long sz: input size (in elements) of the operand
  ptr_unn op1: input values of first operand
 */ 
{
  /* Routine to zero the value of the first operand and store the result in the second operand. */

  long idx;

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op1);

  switch(type){
  case NC_FLOAT:
    for(idx=0;idx<sz;idx++) op1.fp[idx]=0.;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=0.;
    break;
  case NC_LONG:
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
  } /* end switch */ 

  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 

} /* end var_zero() */ 

void
zero_long(long sz,long *op1)
/* 
  long sz: input size (in elements) of the operand
  long *op1: input values of first operand
 */ 
{
  /* Routine to zero the value of the first operand and store the result in the first operand. */

  long idx;

  for(idx=0;idx<sz;idx++) op1[idx]=0L;

} /* end zero_long() */ 

void
var_subtract(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: input netCDF type of the operands
  long sz: input size (in elements) of the operands
  int has_mss_val: input flag for missing values
  ptr_unn mss_val: input value of missing value
  ptr_unn op1: input values of first operand
  ptr_unn op2: input/outpue values of second operand on input, values of difference on output
 */ 
{
  /* Routine to difference the value of the first operand from the value of the second operand 
     and store the result in the second operand. The operands are assumed to have conforming
     dimensions, and be of the specified type. The operands' values are 
     assumed to be in memory already. */ 

  /* Subtraction is currently defined as op2:=op2-op1 */ 

  long idx;

  /* Typecast the pointer to the values before access */ 
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
#ifndef C_ONLY
    (void)FORTRAN_subtract_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]-=op1.fp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != *mss_val.fp) && (op1.fp[idx] != *mss_val.fp)) op2.fp[idx]-=op1.fp[idx]; else op2.fp[idx]=*mss_val.fp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */ 
    break;
  case NC_DOUBLE:
#ifndef C_ONLY
    (void)FORTRAN_subtract_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]-=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]-=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */ 
    } /* end else */
#endif /* !F77 */ 
    break;
  case NC_LONG:
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
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != *mss_val.sp) && (op1.sp[idx] != *mss_val.sp)) op2.sp[idx]-=op1.sp[idx]; else op2.sp[idx]=*mss_val.sp;
      } /* end for */ 
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */ 
    break;
  case NC_BYTE:
    /* Do nothing */ 
    break;
  } /* end switch */ 
  
  /* NB: it is not neccessary to un-typecast the pointers to the values after access 
     because we have only operated on local copies of them. */ 
  
} /* end var_subtract() */ 

bool
ncar_csm_inq(int nc_id)
/*  
   int nc_id: input netCDF file ID
   bool ncar_csm_inq(): output whether the file is an NCAR CSM history tape
*/
{
  /* Routine to check whether the file adheres to NCAR CSM history tape format */

  bool NCAR_CSM=False;

  char *att_val;

  int att_sz;
  int rcd;

  nc_type att_type;

  /* Look for the signature of an NCAR CSM format file */ 
  ncopts=0; 
  rcd=ncattinq(nc_id,NC_GLOBAL,"convention",&att_type,&att_sz);
  ncopts=NC_VERBOSE | NC_FATAL; 

  if(rcd != -1 && att_type == NC_CHAR){
    att_val=(char *)malloc(att_sz*nctypelen(att_type));
    (void)ncattget(nc_id,NC_GLOBAL,"convention",att_val);
    if(!strcmp("NCAR-CSM",att_val)) NCAR_CSM=True;
  } /* endif */ 

  if(NCAR_CSM) (void)fprintf(stderr,"%s: CONVENTION File convention is NCAR CSM\n",prg_nm_get()); 

  return NCAR_CSM;
  
} /* end ncar_csm_inq */ 

void
ncar_csm_date(int nc_id,var_sct **var,int nbr_var)
     /*  
	int nc_id: input netCDF file ID
	var_sct **var: input list of pointers to variable structures
	int nbr_var: input number of structures in variable structure list
	*/
{
  /* Routine to fix the date variable in averaged CSM files */ 
  char wrn_sng[1000];

  int day;
  int date_idx;
  int idx;
  int time_idx;
  
  int nbdate_id;
  
  nclong nbdate;
  nclong date;
  
  (void)sprintf(wrn_sng,"Most, but not all, CSM files which are in CCM format contain the fields \"nbdate\", \"time\", and \"date\". When the \"date\" field is present but either \"nbdate\" or \"time\" is missing, then %s is unable to construct a meaningful average \"date\" to store in the output file. Therefore the \"date\" variable in your output file may be meaningless.\n",prg_nm_get());

  /* Find the date variable (NC_LONG: current date as 6 digit integer (YYMMDD)) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"date")) break;
  } /* end loop over idx */
  if(idx == nbr_var) return; else date_idx=idx;
  if(var[date_idx]->type != NC_LONG) return;
  
  /* Find the scalar nbdate variable (NC_LONG: base date date as 6 digit integer (YYMMDD)) */ 
  ncopts=0;
  nbdate_id=ncvarid(nc_id,"nbdate");
  ncopts=NC_VERBOSE | NC_FATAL; 
  if(nbdate_id == -1){
    (void)fprintf(stderr,"%s: WARNING NCAR CSM convention file output variable list contains \"date\" but not \"nbdate\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  } /* endif */ 
  (void)ncvarget1(nc_id,nbdate_id,0L,&nbdate);
  
  /* Find the time variable (NC_DOUBLE: current day) */
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
#ifndef C_ONLY
  date=FORTRAN_newdate(&nbdate,&day);
#else
  /* Debug dbg XXX: Rewrite this routine in C */ 
  date=nbdate;
#endif /* !F77 */ 
  if(var[date_idx]->val.lp != NULL) return; else var[date_idx]->val.lp[0]=date;
  
} /* end ncar_csm_date */ 

double
arm_time_mk(int nc_id,double time_offset)
     /*  
	int nc_id: input netCDF file ID
	double time_offset: input current
	double arm_time_mk: output base_time + current time_offset
	*/
{
  /* Routine to return the time corresponding to the current time offset */ 
  double arm_time;

  int base_time_id;

  nclong base_time;

  /* Find the base_time variable (NC_LONG: base UNIX time of file) */ 
  ncopts=0;
  base_time_id=ncvarid(nc_id,"base_time");
  ncopts=NC_VERBOSE | NC_FATAL; 
  if(base_time_id == -1){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"base_time\", exiting arm_time_mk()...\n",prg_nm_get());
    return -1;
  };
  (void)ncvarget1(nc_id,base_time_id,0L,&base_time);
  arm_time=base_time+time_offset;

  return arm_time;
} /* end arm_time_mk() */ 

void
arm_time_install(int nc_id,nclong base_time_srt)
     /*  
	 int nc_id: input netCDF file ID
	 nclong base_time_srt: input base_time of first input file
	 */
{
  /* Routine to add time variable to concatenated ARM files */ 

  char att_units[]="seconds since 1970/01/01 00:00:00.00";
  char att_long_name[]="UNIX time";

  double *time_offset;

  int rcd;
  int time_id;
  int time_dim_id;
  int time_offset_id;
  
  long idx;
  long srt=0L;
  long cnt;
  long time_sz;

  /* Synchronize output file */ 
  (void)ncsync(nc_id);

  /* Find time_offset variable */ 
  ncopts=0;
  time_offset_id=ncvarid(nc_id,"time_offset");
  ncopts=NC_VERBOSE | NC_FATAL; 
  if(time_offset_id == -1){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"time_offset\", exiting arm_time_install()...\n",prg_nm_get());
    return;
  };

  /* See if the time variable already exists */ 
  ncopts=0;
  time_id=ncvarid(nc_id,"time");
  ncopts=NC_VERBOSE | NC_FATAL; 
  if(time_id != -1){
    (void)fprintf(stderr,"%s: WARNING ARM file already has variable \"time\"\n",prg_nm_get());
    return;
  };

  /* See if the time dimension exists */ 
  time_dim_id=ncdimid(nc_id,"time");
  if(time_dim_id == -1){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have dimension \"time\"\n",prg_nm_get());
    return;
  };
  /* Get dimension size */ 
  (void)ncdiminq(nc_id,time_dim_id,(char *)NULL,&cnt);

  /* If the time coordinate does not already exist, create it */ 
  time_offset=(double *)malloc(cnt*nctypelen(NC_DOUBLE));

  (void)ncvarget(nc_id,time_offset_id,&srt,&cnt,(void *)time_offset);
  for(idx=0L;idx<cnt;idx++) time_offset[idx]+=base_time_srt;

  /* The file must be in define mode */ 
  (void)ncredef(nc_id);
  time_id=ncvardef(nc_id,"time",NC_DOUBLE,1,&time_dim_id);

  /* Add attributes for time variable */ 
  (void)ncattput(nc_id,time_id,"units",NC_CHAR,strlen(att_units)+1,(void *)att_units);
  (void)ncattput(nc_id,time_id,"long_name",NC_CHAR,strlen(att_long_name)+1,(void *)att_long_name);

  /* Catenate a time-stamped reminder to the "history" global attribute */ 
  (void)hst_att_cat(nc_id,"ncrcat added variable time=base_time+time_offset");

  /* Take the file out of define mode */ 
  (void)ncendef(nc_id);

  /* Write the time variable */ 
  (void)ncvarput(nc_id,time_id,&srt,&cnt,(void *)time_offset);

  /* Free the time_offset buffer */ 
  if(time_offset != NULL) (void)free(time_offset);

} /* end arm_time_install */ 

void
var_lst_convert(int nc_id,nm_id_sct *xtr_lst,int nbr_xtr,dim_sct **dim,int nbr_dim_xtr,
	   var_sct ***var_ptr,var_sct ***var_out_ptr)
/*  
   int nc_id: input netCDF file ID
   nm_id_sct *xtr_lst: input current extraction list (destroyed)
   int nbr_xtr: input total number of variables in input file
   dim_sct **dim: input list of pointers to dimension structures associated with input variable list
   int nbr_dim_xtr: input number of dimensions structures in list
   var_sct ***var_ptr: output pointer to list of pointers to variable structures
   var_sct ***var_out_ptr: output pointer to list of pointers to duplicates of variable structures
*/
{
  /* Routine to make a var_sct list from a nm_id list. The var_sct lst is duplicated 
     to be used for an output list. */

  int idx;

  var_sct **var;
  var_sct **var_out;

  var=(var_sct **)malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)malloc(nbr_xtr*sizeof(var_sct *));

  /* Fill in the variable structure list for all the extracted variables */ 
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=var_fll(nc_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dim_xtr);
    var_out[idx]=var_dup(var[idx]);
    (void)var_xrf(var[idx],var_out[idx]);
    (void)var_dim_xrf(var_out[idx]);
  } /* end loop over idx */
  
  *var_ptr=var;
  *var_out_ptr=var_out;

} /* end var_lst_convert */ 

void
var_lst_divide(var_sct **var,var_sct **var_out,int nbr_var,bool NCAR_CSM_FORMAT,
	       dim_sct **dim_xcl,int nbr_dim_xcl,
	       var_sct ***var_fix_ptr,var_sct ***var_fix_out_ptr,int *nbr_var_fix,
	       var_sct ***var_prc_ptr,var_sct ***var_prc_out_ptr,int *nbr_var_prc)
/*  
   var_sct **var: input list of pointers to variable structures
   var_sct **var_out: input list of pointers to duplicates of variable structures
   int nbr_var: input number of variable structures in list
   dim_sct **dim_xcl: input list of pointers to dimensions not allowed in fixed variables
   int nbr_dim_xcl: input number of dimension structures in list
   bool NCAR_CSM_FORMAT: input whether the file is an NCAR CSM history tape
   var_sct ***var_fix_ptr: output pointer to list of pointers to fixed-variable structures
   var_sct ***var_fix_out_ptr: output pointer to list of pointers to duplicates of fixed-variable structures
   int *nbr_var_fix: output number of variable structures in list of fixed variables
   var_sct ***var_prc_ptr: output pointer to list of pointers to processed-variable structures
   var_sct ***var_prc_out_ptr: output pointer to list of pointers to duplicates of processed-variable structures
   int *nbr_var_prc: output number of variable structures in list of processed variables
*/
{
  /* Routine to divide the two input lists of variables into four separate output lists, 
     based on the program type */

  char *var_nm;

  int idx;
  int prg;

  enum op_type{
    fix, /* 0 */ 
    prc /* 1 */ 
  };

  int idx_dim;
  int idx_xcl;
  int var_op_type[MAX_NC_VARS];

  nc_type var_type;

  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_prc;
  var_sct **var_prc_out;

  prg=prg_get();

  /* Allocate space for too many structures, then realloc() at the end, to avoid duplication. */ 
  var_fix=(var_sct **)malloc(MAX_NC_VARS*sizeof(var_sct *));
  var_fix_out=(var_sct **)malloc(MAX_NC_VARS*sizeof(var_sct *));
  var_prc=(var_sct **)malloc(MAX_NC_VARS*sizeof(var_sct *));
  var_prc_out=(var_sct **)malloc(MAX_NC_VARS*sizeof(var_sct *));

  /* Find operation type for each variable: for now this is either fix or prc */ 
  for(idx=0;idx<nbr_var;idx++){
    
    /* Initialize operation type */
    var_op_type[idx]=prc;
    var_nm=var[idx]->nm;
    var_type=var[idx]->type;

    /* Override operation type based depending on both the variable type and program */ 
    switch(prg){
    case ncap:
      var_op_type[idx]=fix;
      break;
    case ncra:
      if(!var[idx]->is_rec_var) var_op_type[idx]=fix;
      break;
    case ncea:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_type[idx]=fix;
      break;
    case ncdiff:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_type[idx]=fix;
      break;
    case ncflint:
      if((var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_type[idx]=fix;
      break;
    case ncwa:
      /* Every variable containing an excluded (averaged) dimension must be processed */
      for(idx_dim=0;idx_dim<var[idx]->nbr_dim;idx_dim++){
	for(idx_xcl=0;idx_xcl<nbr_dim_xcl;idx_xcl++){
	  if(var[idx]->dim[idx_dim]->id == dim_xcl[idx_xcl]->id) break;
	} /* end loop over idx_xcl */
	if(idx_xcl != nbr_dim_xcl){
	  var_op_type[idx]=prc;
	  break;
	} /* end if */ 
      } /* end loop over idx_dim */
      /* If the variable does not contain an excluded (averaged) dimension, it must be fixed */
      if(idx_dim == var[idx]->nbr_dim) var_op_type[idx]=fix;
      break;
    case ncrcat:
      if(!var[idx]->is_rec_var) var_op_type[idx]=fix;
      break;
    case ncecat:
      if(var[idx]->is_crd_var) var_op_type[idx]=fix;
      break;
    } /* end switch */ 
    
    if(NCAR_CSM_FORMAT){
      if(!strcmp(var_nm,"ntrm") || !strcmp(var_nm,"ntrn") || !strcmp(var_nm,"ntrk") || !strcmp(var_nm,"ndbase") || !strcmp(var_nm,"nsbase") || !strcmp(var_nm,"nbdate") || !strcmp(var_nm,"nbsec") || !strcmp(var_nm,"mdt") || !strcmp(var_nm,"mhisf")) var_op_type[idx]=fix;
      if(prg == ncdiff && (!strcmp(var_nm,"hyam") || !strcmp(var_nm,"hybm") || !strcmp(var_nm,"hyai") || !strcmp(var_nm,"hybi") || !strcmp(var_nm,"gw") || !strcmp(var_nm,"ORO") || !strcmp(var_nm,"date") || !strcmp(var_nm,"datesec"))) var_op_type[idx]=fix;
    } /* end if NCAR_CSM_FORMAT */ 

  } /* end loop over var */

  /* Assign the list pointers based on the operation type for the variable */ 
  *nbr_var_prc=*nbr_var_fix=0;
  for(idx=0;idx<nbr_var;idx++){
    if(var_op_type[idx] == fix){
      var_fix[*nbr_var_fix]=var[idx];
      var_fix_out[*nbr_var_fix]=var_out[idx];
      ++*nbr_var_fix;
    }else{
      var_prc[*nbr_var_prc]=var[idx];
      var_prc_out[*nbr_var_prc]=var_out[idx];
      ++*nbr_var_prc;
/*      if(var[idx]->type != NC_FLOAT && var[idx]->type != NC_DOUBLE && prg != ncdiff && prg != ncrcat){
	(void)fprintf(stderr,"%s: WARNING variable \"%s\" will be coerced from %s to NC_FLOAT\n",prg_nm_get(),var[idx]->nm,nc_type_nm(var[idx]->type));
	var_out[idx]->type=NC_FLOAT;
      } */ /* end if */ 
      if(var[idx]->type == NC_CHAR || var[idx]->type == NC_BYTE && prg != ncrcat && prg != ncecat){
	(void)fprintf(stderr,"%s: WARNING Variable %s is of type %s, for which processing (i.e., averaging, differencing) is ill-defined\n",prg_nm_get(),var[idx]->nm,nc_type_nm(var[idx]->type));
      } /* end if */ 
    } /* end else */ 
  } /* end loop over var */ 
  
  /* Sanity check */ 
  if(*nbr_var_prc+*nbr_var_fix != nbr_var){
    (void)fprintf(stdout,"%s: ERROR nbr_var_prc+nbr_var_fix != nbr_var\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* end if */

  /* DBG XXX: remove the ncap exception when we finish the ncap list processing */ 
  if(*nbr_var_prc==0 && prg != ncap){
    (void)fprintf(stdout,"%s: ERROR no variables fit criteria for processing.\n",prg_nm_get());
    switch(prg_get()){
    case ncap:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain some derived fields.\n",prg_nm_get());
    case ncra:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain record variables that are not NC_CHAR or NC_BYTE in order to perform a running average.\n",prg_nm_get());
      break;
    case ncea:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables that are not NC_CHAR or NC_BYTE in order to perform an ensemble average.\n",prg_nm_get());
      break;
    case ncdiff:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables that are not NC_CHAR or NC_BYTE in order to subtract.\n",prg_nm_get());
      break;
    case ncflint:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain variables that are not NC_CHAR or NC_BYTE in order to interpolate.\n",prg_nm_get());
      break;
    case ncwa:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain variables that contain an averaging dimension in order to perform an average.\n",prg_nm_get());
      break;
    case ncrcat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain record variables in order to perform a record concatenation.\n",prg_nm_get());
      break;
    case ncecat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables in order to perform an ensemble concatenation.\n",prg_nm_get());
      break;
    } /* end switch */ 
    exit(EXIT_FAILURE);
  } /* end if */

  /* Free unused space and save the pointers in the output variables */ 
  *var_fix_ptr=(var_sct **)realloc(var_fix,*nbr_var_fix*sizeof(var_sct *));
  *var_fix_out_ptr=(var_sct **)realloc(var_fix_out,*nbr_var_fix*sizeof(var_sct *));
  *var_prc_ptr=(var_sct **)realloc(var_prc,*nbr_var_prc*sizeof(var_sct *));
  *var_prc_out_ptr=(var_sct **)realloc(var_prc_out,*nbr_var_prc*sizeof(var_sct *));

} /* end var_lst_divide */ 

nm_id_sct *
dim_lst_mk(int nc_id,char **dim_lst_in,int nbr_dim)
/* 
   int nc_id: input netCDF file ID
   char **dim_lst_in: user specified list of dimension names
   int nbr_dim: input total number of dimensions in lst
   nm_id_sct dim_lst_mk(): output dimension list
 */ 
{
  int idx;

  nm_id_sct *dim_lst;
  
  dim_lst=(nm_id_sct *)malloc(nbr_dim*sizeof(nm_id_sct));
  for(idx=0;idx<nbr_dim;idx++){
    /* See if the requested dimension is in the input file */
    dim_lst[idx].nm=dim_lst_in[idx];
    dim_lst[idx].id=ncdimid(nc_id,dim_lst[idx].nm);
  } /* end loop over idx */
  
  return dim_lst;

} /* end dim_lst_mk() */ 

void
rec_crd_chk(var_sct *var,char *fl_in,char *fl_out,long idx_rec,long idx_rec_out)
/* 
   var_sct *var: input variable structure of coordinate to check for monotinicity
   char *fl_in: input current input filename
   char *fl_out: input current output filename
   int idx_rec: input current index or record coordinate in input file
   int idx_rec_out: input current index or record coordinate in output file
 */ 
{
  /* Routine to check for monotonicity of coordinate values */ 

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
  case NC_LONG: rec_crd_val_crr=var->val.lp[0]; break;
  case NC_SHORT: rec_crd_val_crr=var->val.sp[0]; break;
  case NC_CHAR: rec_crd_val_crr=var->val.cp[0]; break;
  case NC_BYTE: rec_crd_val_crr=var->val.bp[0]; break;
  } /* end switch */ 
  
  if(idx_rec_out > 1){
    if(((rec_crd_val_crr > rec_crd_val_lst) && monotonic_direction == decreasing) ||
       ((rec_crd_val_crr < rec_crd_val_lst) && monotonic_direction == increasing))
      (void)fprintf(stderr,"%s: WARNING Record coordinate \"%s\" does not monotonically %s between (input file %s record indices: %ld, %ld) (output file %s record indices %ld, %ld) record coordinate values %lg, %lg\n",prg_nm_get(),var->nm,(monotonic_direction == decreasing ? "decrease" : "increase"),fl_in,idx_rec-1,idx_rec,fl_out,idx_rec_out-1,idx_rec_out,rec_crd_val_lst,rec_crd_val_crr);
  }else if(idx_rec_out == 1){
    if(rec_crd_val_crr > rec_crd_val_lst) monotonic_direction=increasing; else monotonic_direction=decreasing;
  } /* end if */ 
    
  rec_crd_val_lst=rec_crd_val_crr;

} /* end rec_crd_chk() */ 

char **
fl_lst_mk(char **argv,int argc,int arg_crr,int *nbr_fl,char **fl_out)
/* 
   char **argv: input argument list
   int argc: input argument count
   int arg_crr: input index of current argument
   int *nbr_fl: output number of files in input file list
   char **fl_out: output name of the output file
   char **fl_lst_mk(): output list of user-specified filenames
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
    fl_lst_in=(char **)malloc(sizeof(char *));
    fl_lst_in[(*nbr_fl)++]=argv[arg_crr++];
    if(arg_crr == argc-1) *fl_out=argv[arg_crr]; else *fl_out=NULL;
    return fl_lst_in;
    /*    break;*/ /* NB: putting break after return in case statement causes warning on SGI cc */ 
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

  /* Fill in the file list and output file */ 
  fl_lst_in=(char **)malloc((argc-arg_crr-1)*sizeof(char *));
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
   char *nm_in: input name of program to categorize, e.g., argv[0] (might include a path prefix)
   int *prg: output enumeration number corresponding to nm_in
   char *prg_prs(): output nm_in stripped of any path (i.e., program name stub)
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
   char *err_msg: input the formatted error message to print
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
/* Routine to print the correct command-line usage of the programs (currently to stdout) */ 

  char *opt_sng;

  int prg;

  prg=prg_get();

  switch(prg){
  case ncra:
  case ncea:
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
    opt_sng=(char *)strdup("[-A] -a ... [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-m mask] [-M val] [-N] [-n] [-O] [-o op_type] [-p path] [-R] [-r] [-v ...] [-W] [-w wgt] [-x] in.nc out.nc\n");
    break;
  case ncap:
    opt_sng=(char *)strdup("[-A] -a ... [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-m mask] [-M val] [-o op_type] [-O] [-p path] [-R] [-r] [-v ...] [-w wgt] [-x] in.nc out.nc\n");
    break;
  case ncks:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-m] [-O] [-p path] [-R] [-r] [-s format] [-u] [-v ...] [-x] in.nc [out.nc]\n");
    break;
  case ncatted:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-h] [-l path] [-O] [-p path] [-R] [-r] in.nc [out.nc]\n");
    break;
  case ncrename:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-d ...] [-h] [-l path] [-O] [-p path] [-R] [-r] [-v ...] in.nc [out.nc]\n");
    break;
  } /* end switch */
  
  /* Public service announcement */ 
  (void)fprintf(stdout,"Visit NCO's homepage at http://www.cgd.ucar.edu/cms/nco for the complete online User's Guide\n");

  /* We now have the command-specific command line option string */ 
  (void)fprintf(stdout,"%s %s\n",prg_nm_get(),opt_sng);

  if(strstr(opt_sng,"-A")) (void)fprintf(stdout,"-A\t\tAppend to existing output file, if any\n");
  if(strstr(opt_sng,"-a")){
    if(prg == ncrename) (void)fprintf(stdout,"-a old_att,new_att Attribute's old and new names\n");
    if(prg == ncwa) (void)fprintf(stdout,"-a avg_dim1[,avg_dim2[...]] Averaging dimensions\n");
    if(prg == ncatted) (void)fprintf(stdout,"-a att_nm,var_nm,mode,att_type,att_val Attribute specification:\n\t\tmode = a,c,d,m,o and att_type = f,d,l,s,c,b\n");
  } /* end if */
  if(strstr(opt_sng,"-c")) (void)fprintf(stdout,"-c\t\tCoordinate variables will all be processed\n");
  if(strstr(opt_sng,"-C")) (void)fprintf(stdout,"-C\t\tAssociated coordinate variables should not be processed\n");
  if(strstr(opt_sng,"-D")) (void)fprintf(stdout,"-D dbg_lvl\tDebugging level\n");
  if(strstr(opt_sng,"-d")){
    if(prg == ncrename) (void)fprintf(stdout,"-d old_dim,new_dim Dimension's old and new names\n");
    if(prg != ncrename && prg != ncks) (void)fprintf(stdout,"-d dim,[min][,[max]] Dimension's limits in hyperslab\n");
    if(prg == ncks) (void)fprintf(stdout,"-d dim,[min][,[max]][,[stride]] Dimension's limits and stride in hyperslab\n");
  } /* end if */
  if(strstr(opt_sng,"-F")) (void)fprintf(stdout,"-F\t\tFortran indexing conventions (1-based) for I/O\n");
  if(strstr(opt_sng,"-H")) (void)fprintf(stdout,"-H\t\tPrint data\n");
  if(strstr(opt_sng,"-h")){
    if(prg == ncatted) (void)fprintf(stdout,"-h\t\tDo not append to \"history\" global attribute\n");
  } /* end if */
  if(strstr(opt_sng,"-i")) (void)fprintf(stdout,"-i var,val\tInterpolant and value\n");
  if(strstr(opt_sng,"-l")) (void)fprintf(stdout,"-l path\t\tLocal storage path for remotely-retrieved files\n");
  if(strstr(opt_sng,"-M")){
    if(prg == ncwa) (void)fprintf(stdout,"-M val\tMasking value (default is 1.)\n");
    if(prg == ncks) (void)fprintf(stdout,"-M\t\tPrint global metadata\n");
  } /* end if */ 
  if(strstr(opt_sng,"-m")){
    if(prg == ncwa) (void)fprintf(stdout,"-m mask\t\tMasking variable name\n");
    if(prg == ncks) (void)fprintf(stdout,"-m\t\tPrint variable metadata\n");
  } /* end if */ 
  if(strstr(opt_sng,"-N")) (void)fprintf(stdout,"-N\t\tNo normalization (by tally or weight)\n");
  if(strstr(opt_sng,"-n")){
    if(prg == ncwa) (void)fprintf(stdout,"-n\t\tNormalize by tally but not weight\n");
    if(prg != ncwa) (void)fprintf(stdout,"-n nbr_files,[nbr_numeric_chars[,increment]] NINTAP-style abbreviation of file list\n");
  } /* end if */
  if(strstr(opt_sng,"-o")) (void)fprintf(stdout,"-o op_type\tRelational operator for masking (eq,ne,ge,le,gt,lt)\n");
  if(strstr(opt_sng,"-O")) (void)fprintf(stdout,"-O\t\tOverwrite existing output file, if any\n");
  if(strstr(opt_sng,"-p")) (void)fprintf(stdout,"-p path\t\tPath prefix for all input filenames\n");
  if(strstr(opt_sng,"-R")) (void)fprintf(stdout,"-R\t\tRetain remotely-retrieved files after use\n");
  if(strstr(opt_sng,"-r")) (void)fprintf(stdout,"-r\t\tProgram version and copyright notice\n");
  if(strstr(opt_sng,"-s")) (void)fprintf(stdout,"-s format\tString format for text output\n");
  if(strstr(opt_sng,"-u")) (void)fprintf(stdout,"-u\t\tUnits of variables, if any, will be printed\n");
  if(strstr(opt_sng,"-v")){
    if(prg == ncrename) (void)fprintf(stdout,"-v old_var,new_var Variable's old and new names\n");
    if(prg != ncrename) (void)fprintf(stdout,"-v var1[,var2[...]] Variables to process\n");
  } /* end if */
  if(strstr(opt_sng,"-W")) (void)fprintf(stdout,"-W\t\tNormalize by weight but not tally\n");
  if(strstr(opt_sng,"-w")){
    if(prg == ncwa) (void)fprintf(stdout,"-w wgt\tWeighting variable name\n");
    if(prg == ncflint) (void)fprintf(stdout,"-w wgt_1[,wgt_2] Weight(s) of file(s)\n");
  } /* end if */
  if(strstr(opt_sng,"-x")) (void)fprintf(stdout,"-x\t\tExtract all variables EXCEPT those specified with -v\n");
  if(strstr(opt_sng,"in.nc")) (void)fprintf(stdout,"in.nc\t\tInput file name(s)\n");
  if(strstr(opt_sng,"out.nc")) (void)fprintf(stdout,"out.nc\t\tOutput file name\n");
/*  if(strstr(opt_sng,"-")) (void)fprintf(stdout,"-\n");*/

  /* Free the space holding the string */ 
  (void)free(opt_sng);

} /* end usg_prn() */

int
op_prs(char *op_sng)
/* 
   char *op_sng: input string containing Fortran representation of a reltional operator ("eq","lt"...)
 */ 
{
  /* Routine to parse the Fortran abbreviation for a relational operator into a unique numeric value
     representing that relation */

  /* Classify the relation */
  if(!strcmp(op_sng,"eq")){
    return nc_op_eq;
  }else if(!strcmp(op_sng,"ne")){
    return nc_op_ne;
  }else if(!strcmp(op_sng,"lt")){
    return nc_op_lt;
  }else if(!strcmp(op_sng,"gt")){
    return nc_op_gt;
  }else if(!strcmp(op_sng,"le")){
    return nc_op_le;
  }else if(!strcmp(op_sng,"ge")){
    return nc_op_ge;
  }else{
    (void)fprintf(stdout,"%s: ERROR %s not registered in op_prs()\n",prg_nm_get(),op_sng);
    exit(EXIT_FAILURE);
  } /* end else */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */ 
  return 1;
} /* end op_prs() */ 
