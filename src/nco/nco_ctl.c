/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_ctl.c,v 1.10 2002-06-10 02:33:23 zender Exp $ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_ctl.h" /* Program flow control functions */

void
nco_exit /* [fnc] Wrapper for exit() */
(int rcd) /* I [enm] Return code */
{
  /* Purpose: Wrapper for exit() */
  exit(rcd);
} /* nco_exit() */

void 
Exit_gracefully(void) /* [fnc] Clean up timers, file descriptors, then exit */
{
  char *time_bfr_end;
  time_t clock;
  
  /* end the clock */ 
  
  clock=time((time_t *)NULL);
  time_bfr_end=ctime(&clock);  time_bfr_end=time_bfr_end; /* Avoid compiler warning until variable is used for something */
/*  (void)fprintf(stderr,"\tend = %s\n",time_bfr_end);*/

  (void)fclose(stderr);
  (void)fclose(stdin);
  (void)fclose(stdout);

  nco_exit(EXIT_SUCCESS);
} /* end Exit_gracefully() */

bool /* [flg] Program does arithmetic */
is_arithmetic_operator /* [fnc] Query whether program does arithmetic */
(const int prg_id) /* [enm] Program ID */
{
  /* Purpose: Does program do arithmetic? */
  switch(prg_id){
  case ncap: 
  case ncdiff:
  case ncea:
  case ncflint:
  case ncra:
  case ncwa:
    return True;
    break;
  case ncatted: 
  case ncecat: 
  case ncks: 
  case ncrcat: 
  default:
    return False;
    break;
  } /* end switch */
} /* end is_arithmetic_operator() */

void
nco_lib_vrs_prn(void) /* [fnc] Print netCDF library version */
{
  /* Purpose: Print netCDF library version */

  char *lib_sng;
  char *nst_sng;
  char *vrs_sng;
  char *of_ptr;
  char *dlr_ptr;

  size_t vrs_sng_lng;
  size_t nst_sng_lng;

  /* As of netCDF 3.4, nc_inq_libvers() returned strings such as "3.4 of May 16 1998 14:06:16 $" */  
  lib_sng=(char *)strdup(nc_inq_libvers());
  of_ptr=strstr(lib_sng," of ");
  if(of_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lib_vrs_prn() reports of_ptr == NULL\n",prg_nm_get());
  vrs_sng_lng=(size_t)(of_ptr-lib_sng);
  vrs_sng=(char *)nco_malloc(vrs_sng_lng+1);
  strncpy(vrs_sng,lib_sng,vrs_sng_lng);
  vrs_sng[vrs_sng_lng]='\0';

  dlr_ptr=strstr(lib_sng," $");
  if(dlr_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lib_vrs_prn() reports dlr_ptr == NULL\n",prg_nm_get());
  nst_sng_lng=(size_t)(dlr_ptr-of_ptr-4); /* 4 is the length of " of " */
  nst_sng=(char *)nco_malloc(nst_sng_lng+1ul);
  strncpy(nst_sng,of_ptr+4,nst_sng_lng); /* 4 is the length of " of " */
  nst_sng[nst_sng_lng]='\0';

  (void)fprintf(stderr,"Linked to netCDF library version %s, compiled %s\n",vrs_sng,nst_sng);
  (void)fprintf(stdout,"NCO homepage URL is http://nco.sourceforge.net\n");

  vrs_sng=(char *)nco_free(vrs_sng);
  lib_sng=(char *)nco_free(lib_sng);
  nst_sng=(char *)nco_free(nst_sng);
} /* end nco_lib_vrs_prn() */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
prg_prs /* [fnc] Strip program name to stub and return program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const prg) /* O [enm] Enumerated number corresponding to nm_in */
{
  /* Purpose: Set program name and enum */

  char *nm_out;

  /* Get program name (use strrchr() first in case nm_in contains a path) */
  nm_out=(char *)strdup(nm_in);
  if(strrchr(nm_out,'/') != NULL) nm_out=strrchr(nm_out,'/')+1;

  /* Classify calling program */
  if(!strcmp(nm_out,"ncra")){*prg=ncra;}
  else if(!strcmp(nm_out,"ncap")){*prg=ncap;}
  else if(!strcmp(nm_out,"ncea")){*prg=ncea;}
  else if(!strcmp(nm_out,"ncdiff")){*prg=ncdiff;}
  else if(!strcmp(nm_out,"ncflint")){*prg=ncflint;}
  else if(!strcmp(nm_out,"ncwa")){*prg=ncwa;}
  else if(!strcmp(nm_out,"ncrcat")){*prg=ncrcat;}
  else if(!strcmp(nm_out,"ncecat")){*prg=ncecat;}
  else if(!strcmp(nm_out,"ncks")){*prg=ncks;}
  else if(!strcmp(nm_out,"ncrename")){*prg=ncrename;}
  else if(!strcmp(nm_out,"ncatted")){*prg=ncatted;}
  else{
    (void)fprintf(stdout,"%s: ERROR %s not registered in prg_prs()\n",nm_out,nm_out);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  return nm_out;

} /* end prg_prs() */

void 
nco_usg_prn(void)
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
    opt_sng=(char *)strdup("[-A] [-a] [-B] [-b fl_bnr] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-m] [-O] [-p path] [-q] [-R] [-r] [-s format] [-u] [-v ...] [-x] in.nc [out.nc]\n");
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
  if(strstr(opt_sng,"-B")) (void)fprintf(stdout,"-B\t\tWrite data to unformatted binary file\n");
  if(strstr(opt_sng,"-b")) (void)fprintf(stdout,"-b fl_bnr\tUnformatted binary file to write\n");
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
  opt_sng=(char *)nco_free(opt_sng);

} /* end nco_usg_prn() */

