%{ /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.l,v 1.46 2002-05-12 16:32:06 zender Exp $ -*-C-*- */
  /* Everything from here to closing brace is placed at top of lexer */

  /* Purpose: Token generator for ncap parser */

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

  /* Example lexers:
     /data/zender/ora/lexyacc/ch3-05.l
     GCC lexer is hard-coded in C for speed, but is easy to read:
     ../src/gcc-3.x.x/c-lex.c
     Unidata ${DATA}/tmp/netcdf-3.5.0/src/ncgen/ncgen.l */

  /* Usage:
     flex ncap.l
     NB: flex with -Cf or -Cf not -I if this scanner will not be interactive */

  /* Standard header files */
#include <assert.h>  /* assert() debugging macro */
#include <limits.h> /* Integer representation, INT_MIN, INT_MAX... */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strdup */

  /* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

  /* Headers specific to this program */
#include "nco.h" /* NCO definitions */
#include "nco_netcdf.h" /* netCDF3 wrapper functions */
#include "ncap.h" /* Symbol table definition */
#include "ncap.tab.h" /* Symbol definitions from parser */

  /* We want yylex() errors to skip current line rather than stop execution
     We do this by provoking error in parser by returning unrecognized token
     This causes parser to read up to next ';'
     To avoid having two error messages we prefix error message to yyerror with '#'
     This causes yyerror to print current error and skip next error
     fxm: Hackish, but I see no other way */ 

  /* Re-prototype yylex() to accept re-entrant arguments (Flex p. 12, Bison p. 60) */
#define YY_DECL int yylex(YYSTYPE *lval_ptr,prs_sct *prs_arg)

  /* Global variables */
  extern long ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */

  /* File scope variables */
  /* fxm: turn arbitrary size into pre-processor token */
  char err_sng[200]; /* [sng] Buffer for error string */

  /* Is lexer on left or right hand side of equals sign?
     LHS should contain only a variable or attribute
     In this case return variable/attribute name to parser
     If on RHS then return attribute value or variable struture
     Initialize to false, change to true if token is '=', false is token is ';' */
  static bool RHS=False; /* [flg] Is lexer on RHS of '=' sign? */
%}

/* Handle C comments LMB92 p. 172, p. 43
   The %x defines an exclusive start state for CCOMMENT
   When CCOMMENT state is active, other rules do not match
   NB: State names may not contain hyphens */
%x CCOMMENT LHS_SUBSCRIPT RHS_SUBSCRIPT

/* Following sections after %% are lexer rules
   Each rule is regular expression followed by C-code for action
   If token matches regular expression then C-code is executed */
%%

"/*" {
   /* CCOMMENT is exclusive start state for C comments LMB92 p. 172
      When CCOMMENT state is active, only CCOMMENT rules will match 
      (hence an "exclusive start state" rather than "regular start state")
      Thus must (re)-define line number incrementing */
  BEGIN CCOMMENT;
}
<CCOMMENT>\n {
  /* Throw away comment text but increment lines */
  ln_nbr_crr++;
}
<CCOMMENT>. {
  /* Throw away everything except end comment text */
}
<CCOMMENT>"*/" {
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */
  BEGIN INITIAL;
} /* end CCOMMENT start state rules */

\n { 
  /* NB: Remaining rules are default state, also known as INITIAL state, or 0 state */
  /* Ignore new lines */
  ln_nbr_crr++; /* [cnt] Line number incremented in ncap.l */
}

[ \t]+ { 
  /* Eat spaces and tabs */
}

\/\/[^\n]* {
  /* Enable C++ comments */
}

\"[^"\n]*["\n] {
  /* Process quoted strings */
  char *bfr;
                
  bfr=nco_malloc(yyleng*sizeof(char));
  strcpy(bfr,&yytext[1]);
  bfr[yyleng-2]='\0';
  
  /* Replace C-language escape codes with actual byte values */
  (void)sng_ascii_trn(bfr);
  lval_ptr->sng=strdup(bfr);       
  if(dbg_lvl_get() > 3){
    printf("Lexing string: %s\n",yytext);
    printf("Made   string: %s\n",lval_ptr->sng);
  } /* endif */
  return SNG;
} /* end quoted strings */

[0-9]*\.[0-9]*([eE][+-]?[0-9]+)?[LlDd]?|[0-9]*([eE][+-]?[0-9]+)[LlDd]? {
  /* Double or long double */
  /* NB: Tempted to prepend lexer expressions for floats and doubles with [+-]? 
     so that unary plus/minus is handled in lexer rather than parser.
     However, this has unintended side effects so let parser handle it for now */
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a double %s\n",yytext); 
  lval_ptr->scv.val.d=strtod(yytext,(char **)NULL);
  lval_ptr->scv.type=NC_DOUBLE;
  return SCV;
} /* end double or long double */

[0-9]*\.[0-9]*([eE][+-]?[0-9]+)?[Ff]|[0-9]*([eE][+-]?[0-9]+)[Ff] {
  /* Float */
  float flt_tkn;
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a float %s\n",yytext); 
  if (sscanf((char*)yytext,"%e",&flt_tkn) != 1){
    sprintf(err_sng,"#Bad float: %s",yytext);
    yyerror(err_sng);
    return EPROVOKE;
  } /* endif */
  lval_ptr->scv.val.f=flt_tkn;
  lval_ptr->scv.type=NC_FLOAT;
  return SCV;
} /* end float */

[0-9]+[lL]? {
  /* Long */
  double dbl_tkn;
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing an integer %s\n",yytext); 
  dbl_tkn=strtod((char*)yytext, (char**)(NULL));
  if (dbl_tkn < INT_MIN || dbl_tkn > INT_MAX){
    lval_ptr->scv.val.d=dbl_tkn;
    lval_ptr->scv.type=NC_DOUBLE;
    return SCV;
  }else{
    lval_ptr->scv.val.l=(nco_long)dbl_tkn;
    lval_ptr->scv.type=NC_INT;
    return SCV;
  } /* end else */
} /* end long */

[0-9]+[sS] {
  /* Short */
  short sht_tkn;
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a short %s\n",yytext); 	
  if(sscanf((char*)yytext,"%hd",&sht_tkn) != 1){
    sprintf(err_sng,"Bad short constant: %s",(char*)yytext);
    yyerror(err_sng);
  }
  lval_ptr->scv.val.s=sht_tkn;
  lval_ptr->scv.type=NC_SHORT;
  return SCV;
} /* end short */

[0-9]+[Bb] {
  /* Byte */
  int byt_tkn;
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a byte %s\n",yytext); 
  if(sscanf((char*)yytext,"%d",&byt_tkn) != 1){
    sprintf(err_sng,"#Bad byte: %s",yytext);
    yyerror(err_sng);
    return EPROVOKE;
  }
  if(byt_tkn < -128 || byt_tkn > 127){
    lval_ptr->scv.val.l=byt_tkn;
    lval_ptr->scv.type=NC_INT;
    return SCV;    
  }else{ 
    lval_ptr->scv.val.b=byt_tkn;
    lval_ptr->scv.type=NC_BYTE;
    return SCV;                    
  } /* end else */
} /* end byte */

foo/[ ]*\( {return ABS;} /* TOKEN whitespace (: Token followed any amount of whitespace followed by opening parenthesis signifies function name, not variable name.
This disambiguates variable and functional namespaces. */
abs/[ ]*\( {return ABS;}
atostr/[ ]*\( {return ATOSTR;}
min/[ ]*\( {return RDC;}
pck/[ ]*\( {return PACK;}
pack/[ ]*\( {return PACK;}
pow/[ ]*\( {return POWER;}
upk/[ ]*\( {return UNPACK;}
unpack/[ ]*\( {return UNPACK;}

[A-Za-z_][A-Za-z0-9_]*/[ ]*\( {
/* Compare input with mathematical function names in table and return
   pointer to structure containing name, double function, float function */
   int idx;
   for(idx=0;idx<prs_arg->sym_tbl_nbr;idx++)
     if(!strcmp(yytext,(prs_arg->sym_tbl[idx]->nm))){ 
        lval_ptr->sym=prs_arg->sym_tbl[idx];
        return FUNCTION;
     } /* endif */
   (void)sprintf(err_sng,"Warning: Unable to locate function %s. Is this really a variable ?",yytext);
   (void)yyerror(err_sng);       
   return EPROVOKE;             
} /* end functions */

[A-Za-z_][A-Za-z0-9_]*@[A-Za-z_][A-Za-z0-9_]* { 
  /* Recognize variable attributes, e.g., var_nm@att_nm */
  aed_sct *ptr_aed;
  char *amp_ptr;
  char *att_nm;
  char *var_nm;
  int att_idx;
  int rcd=NC_NOERR;
  int rcd_out;
  int var_id;
  long att_sz;
  nc_type type;
  ptr_unn val;
  size_t sng_lng;
  
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing attribute %s\n",yytext);
  amp_ptr=strchr(yytext,'@');
  sng_lng=amp_ptr-yytext;
  
  var_nm=(char *)nco_malloc((sng_lng+1)*sizeof(char));
  strncpy(var_nm,yytext,sng_lng);
  var_nm[sng_lng]='\0';
  att_nm=strdup(++amp_ptr);
  
  /* Assume global attribute when variable name is "global" */
  if(!strcmp(var_nm,"global")) var_id=NC_GLOBAL; else rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
  
  /* We are on RHS so return value of attribute (if available) to parser
     During initial scan, just return attributes on RHS */
  if(prs_arg->initial_scan){
    if(!RHS){
      lval_ptr->aed.att_nm=strdup(att_nm);
      lval_ptr->aed.var_nm=strdup(var_nm);
      return OUT_ATT;
    }else{
      return IGNORE;
    } /* endelse */
  } /* endif */
  
  if(RHS){
    /* First check if attribute has already been saved in symbol table
       If so then obtain and return value, else check input file
       If attribute is present obtain and return value from disk else return NC_BYTE of value 0
       If attribute is NC_CHAR then SNG type is returned
       for others NC_FLOAT, NC_DOUBLE, NC_BYTE, NC_SHORT an SCV
       is returned to parser. */
    att_idx=ncap_aed_lookup(var_nm,att_nm,prs_arg->att_lst,prs_arg->nbr_att,False);
    if(att_idx >= 0){
      ptr_aed=prs_arg->att_lst[att_idx]; 
      if (ptr_aed->type == NC_CHAR){
	(void)cast_void_nctype(ptr_aed->type,&ptr_aed->val);
	lval_ptr->sng=(char *)nco_malloc((ptr_aed->sz+1)*nco_typ_lng(NC_CHAR));
	strncpy(lval_ptr->sng,(char *)(ptr_aed->val.cp),ptr_aed->sz);
	lval_ptr->sng[ptr_aed->sz]='\0';
	return SNG;
      } else {
	lval_ptr->scv=ptr_unn_2_scv(ptr_aed->type,ptr_aed->val);
	return SCV;  
      } /* end else */        
    } /* end if */
    if(rcd == NC_NOERR){  
      rcd=nco_inq_att_flg(prs_arg->in_id,var_id,att_nm,&type,&att_sz);
      if (rcd == NC_ENOTATT){
	(void)sprintf(err_sng,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
	(void)yyerror(err_sng);
	lval_ptr->scv.val.b=0;
	lval_ptr->scv.type=NC_BYTE;
	return SCV;
      } /* end if */
      if(rcd == NC_NOERR){ 
	val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(type));
	rcd=nco_get_att(prs_arg->in_id,var_id,att_nm,val.vp,type);
	if(type == NC_CHAR){
	  (void)cast_void_nctype(type,&val);
	  lval_ptr->sng=(char *)nco_malloc((att_sz+1)*sizeof(char));
	  strncpy(lval_ptr->sng,(char *)val.cp,att_sz);
	  lval_ptr->sng[att_sz]='\0'; 
	  return SNG;
	}else{
	  lval_ptr->scv=ptr_unn_2_scv(type,val);
	  return SCV;   
	} /* end else */ 
      } /* end if */
    }else{ /* ...else rcd reported an error */
      /* Attribute is not in table or on disk so return 0-byte  */
      (void)sprintf(err_sng,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
      (void)yyerror(err_sng);
      lval_ptr->scv.val.b=0;
      lval_ptr->scv.type=NC_BYTE;
      return SCV;
    } /* end if */
  }/* end if RHS */
  
  /* We are on LHS so simply save information for later processing by parser
     Atrribute is valid if its associated variable is in input or output file */
  if(!RHS){
    rcd_out=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
    if(rcd == NC_NOERR || rcd_out == NC_NOERR){
      lval_ptr->aed.att_nm=strdup(att_nm);
      lval_ptr->aed.var_nm=strdup(var_nm);
      return OUT_ATT;
    }else{
      (void)sprintf(err_sng,"#Warning: unable to locate variable %s. So cannot create attribute %s",var_nm,yytext);
      (void)yyerror(err_sng);
      return EPROVOKE; 
    } /* end else */
  } /* end if */
  
} /* End scalar values. Phew! */

[A-Za-z_][A-Za-z0-9_]*/\[ {
  /* Recognize variables with subscripts */
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing variable with subscripts %s\n",yytext);
  if(RHS){
    lval_ptr->var_nm_RHS=strdup(yytext);
    BEGIN RHS_SUBSCRIPT;
    return VAR;
  }else{ 
    lval_ptr->var_nm_LHS=strdup(yytext);
    BEGIN LHS_SUBSCRIPT;
    return OUT_VAR;
  } /* end else */
} /* end variables with subscripts */

[A-Za-z_][A-Za-z0-9_]* {
  /* Recognize plain variables */
  if(dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing variable %s\n",yytext);
  if(RHS){
    lval_ptr->var_nm_RHS=strdup(yytext); 
    return VAR;
  }else{ 
    lval_ptr->var_nm_LHS=strdup(yytext);
    return OUT_VAR;
  } /* end else */
} /* end variables without subscripts */

<LHS_SUBSCRIPT>\[[^\]\n]*[\]\n] {
  /* Recognize subscripted (hyperslabbed) variables
     Hyperslabbed variables for now indicated with square brackets
     Using square brackets so development does not break functions
     Left context is described in LMB92 p. 152
     
     Subscript syntax is taken from Fortran9X specification:
     x[dmn1,dmn2,...,dmnN]
     x[a:b,c:,:d,e] */
  const char *sbs_dlm=","; /* [sng] Subscript delimiter */
  static char *tpl_nm="Internally generated template";
  
  char **sbs_lst; /* [sng] Array of dimension subscripts */
  char *sbs_sng; /* [sng] Subscript */
  
  int dmn_nbr; /* [nbr] Number of dimensions */
  int dmn_id; /* [id] Dimension ID */
  int idx; /* [idx] Counter */
  int jdx; /* [idx] Counter */
  int rcd; /* [enm] Return code */
  int out_nbr=0;
  
  double val=1.0; /* [frc] Value of template */
  
  var_sct *var; /* [sct] Variable */
  
  nm_id_sct *dmn_lst; /* [sng] Dimension name-ID list */
  nm_id_sct *out_lst; /* [sng] Dimension name-ID list */
  dmn_sct **dmn; /* [dmn] Dimension structure list */
  
  /* Valid subscripts must contain alphanumeric string */
  if(yyleng < 3) goto end_LHS_sbs;
  
  /* Copy token into user memory and turn into list of dimension names */
  sbs_sng=(char *)nco_malloc(yyleng*sizeof(char));
  strcpy(sbs_sng,&yytext[1]);
  sbs_sng[yyleng-2]='\0';  
  sbs_lst=lst_prs(sbs_sng,sbs_dlm,&dmn_nbr);

  /* Use dimension names to populate list of dimension structures */
  dmn_lst=(nm_id_sct *)nco_malloc(dmn_nbr*sizeof(nm_id_sct));
  out_lst=(nm_id_sct *)nco_malloc(dmn_nbr*sizeof(nm_id_sct));
  for(idx=0;idx<dmn_nbr;idx++){
    rcd=nco_inq_dimid_flg(prs_arg->in_id,sbs_lst[idx],&dmn_id);
    if(rcd != NC_NOERR){
      (void)sprintf(err_sng,"Warning: Lexer does not recognize user-specified LHS dimension %s\n",sbs_lst[idx]);
      (void)yyerror(err_sng);
      goto end_LHS_sbs;                 
    } /* endif err */
    dmn_lst[idx].nm=sbs_lst[idx];
    dmn_lst[idx].id=dmn_id;

    /* Make another list without any double entries */
    for(jdx=0;jdx<out_nbr;jdx++) /* fxm: What does this do? Is it necessary? */
      if(!strcmp(dmn_lst[idx].nm,out_lst[jdx].nm)) break;
    if(jdx == 0 || jdx == out_nbr) out_lst[out_nbr++]=dmn_lst[idx];
  } /* end loop over dimensions */
  
  /* Build subscript list only once, during initial scan */
  if(prs_arg->initial_scan){
    lval_ptr->sbs_lst=(nm_lst_sct *)nco_malloc(sizeof(nm_lst_sct));
    lval_ptr->sbs_lst->list=out_lst;
    lval_ptr->sbs_lst->nbr=out_nbr;
    /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
    BEGIN INITIAL;
    return LHS_SBS;
  } /* endif */
  
  /* Create private, temporary list of LHS dimensions for current variable
     Each item of list points to public, permanent dimension in global list */
  dmn=(dmn_sct **)nco_malloc(dmn_nbr*sizeof(dmn_sct *));
  for(idx=0;idx<dmn_nbr;idx++){
    for(jdx=0;jdx<prs_arg->nbr_dmn_xtr;jdx++) 
      if(!strcmp(dmn_lst[idx].nm,prs_arg->dmn[jdx]->nm)) break;
    if(jdx == prs_arg->nbr_dmn_xtr){
      (void)sprintf(err_sng,"Warning: Unrecognized dimension %s in LHS subscripts",dmn_lst[idx].nm);
      (void)yyerror(err_sng);
      goto end_LHS_sbs;                 
    } /* end if */
    dmn[idx]=prs_arg->dmn[jdx];
  } /* end for */

  /* Create template variable to cast all RHS expressions */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  /* Overwrite with LHS information */
  var->nm=(char *)strdup(tpl_nm);
  var->type=NC_DOUBLE;
  var->nbr_dim=dmn_nbr;
  var->dim=dmn;
  /* Allocate space for dimension structures */
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;
  
  /* Defensive programming */
  var->sz=1L; 
  /* Attach LHS dimensions to variable */
  for(idx=0;idx<dmn_nbr;idx++){
    var->dim[idx]=dmn[idx];
    var->dmn_id[idx]=dmn[idx]->id;
    var->cnt[idx]=dmn[idx]->cnt;
    var->srt[idx]=dmn[idx]->srt;
    var->end[idx]=dmn[idx]->end;
    var->srd[idx]=dmn[idx]->srd;
    var->sz*=var->cnt[idx];
  } /* end loop over dim */
  
  /* Allocate space for variable values 
     fxm: more efficient and safer to use calloc() and not fill with values? */
  if((var->val.vp=(void *)malloc(var->sz*nco_typ_lng(var->type))) == NULL){
    (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes for var_LHS() in lexer\n",prg_nm_get(),var->sz,nco_typ_lng(var->type));
    nco_exit(EXIT_FAILURE); 
  } /* end if */
  
  /* Copy arbitrary value into variable 
     Placing a uniform value in variable should be unnecessary since variable
     is intended for use solely as dimensional template for var_conform_dim() 
     Nevertheless, copy 1.0 into value for safety */
  { /* Change scope to define convenience variables which reduce dereferencing */
    long var_sz; /* [nbr] Number of elements in variable */
    int var_type_sz; /* [B] Size of single element of variable */
    char *var_val_cp; /* [ptr] Pointer to values */
    
    var_sz=var->sz; /* [nbr] Number of elements in variable */
    var_type_sz=nco_typ_lng(var->type);
    var_val_cp=(char *)var->val.vp;
    for(idx=0;idx<var_sz;idx++) (void)memcpy(var_val_cp+idx,(void *)(&val),var_type_sz);
  } /* end scope */
  
  /* Link variable into yylex() argument for use in parser */
  prs_arg->var_LHS=var;
  
  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Lexer creating LHS cast template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),prs_arg->var_LHS->nm,prs_arg->var_LHS->nbr_dim,prs_arg->var_LHS->sz);
  
  if(0){ /* Remove some compiler warning messages */
    /* Following two statements remove "defined but not used" messages in lex.yy.c */
    (void)yyunput((int)NULL,(char *)NULL);
    (void)yy_flex_realloc((void *)NULL,(size_t)NULL);
    /* Remove warnings which only occur on SGI IRIX cc */
    yy_full_match=yy_full_match;
    yy_full_lp=yy_full_lp;
    yy_full_state=yy_full_state;
  } /* endif False */
  
  /* Free dimension list memory */
  (void)nco_free(sbs_sng);    
  (void)nco_free(dmn_lst);
  
 end_LHS_sbs: /* Errors encountered during LHS processing jump to here */
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  BEGIN INITIAL;
} /* end LHS subscripted variables */

<RHS_SUBSCRIPT>\[[^\]\n]*[\]\n] {
  /* RHS_SUBSCRIPT */
  (void)sprintf(err_sng,"Warning RHS subscripts are not yet implemented");
  (void)yyerror(err_sng);
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  BEGIN INITIAL;
} /* end RHS_SUBSCRIPT */

= {
  /* Equals */
  RHS=True; 
  return yytext[0];
} /* end equals */

; { 
  /* semi-colon indicates end-of-statement */
  RHS=False;
  return yytext[0]; 
} /* end semi-colon */

. {
  /* Everything not parsed by above falls through to here */
  if(dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: Following token reached last lexer rule: %s\n",prg_nm_get(),yytext);
  return yytext[0];
} /* end trickle down */

%%

/* Begin user subroutines section */

bool
yywrap(void)
{
  /* Purpose: Routine to replace library-defined yywrap() 
     yywrap() is called when YY_INPUT returns EOF
     Default is to return true (one) when called---this terminates scanner 
     Returning false (zero) means there is more to scan, and input has
     been redirected to new source, e.g., yyin points to new file. */
 return 1;
} /* end yywrap() */

void 
ncap_initial_scan
(prs_sct *prs_arg,
 char *spt_arg_cat,
 nm_id_sct** xtr_lst_a,
 int *nbr_lst_a,
 nm_id_sct** xtr_lst_b,
 int *nbr_lst_b,
 nm_id_sct** xtr_lst_c,
 int *nbr_lst_c,
 nm_id_sct** xtr_lst_d,
 int *nbr_lst_d)
{
  /* Purpose: Scan command script and return three lists
     list a: RHS variables present in input file
     list b: LHS variables present in input file
     list c: Variables of attributes on LHS which are present in input file
     list d: */
  
  /* 20020510: ncap_initial_scan() was in ncap_utl() but g++ (not gcc) would barf
     g++ complained that since YYSTYPE is defined in ncap.tab.h,
     it is an error to call external function (yylex()) that uses this type
     Placing routine in ncap.l ensures it has access to yylex() prototype,
     and to YYSTYPE definition.
     Since ncap_initial_scan() does call yylex() it is stylistically acceptable
     to keep this routine in ncap.l
     To move routine to other file, #define NCAP_INITIAL_SCAN_NOT_IN_NCAP_L */
#undef NCAP_INITIAL_SCAN_NOT_IN_NCAP_L
#ifdef NCAP_INITIAL_SCAN_NOT_IN_NCAP_L
  /* Get YYSTYPE prior to calling scanner */
#include "ncap.tab.h" /* ncap.tab.h is produced from ncap.y by parser generator */

  extern FILE *yyin; /* [fnc] File handle for script file */
  extern int yylex(YYSTYPE *lval_ptr,prs_sct *prs_arg); /* [fnc] Scanner entrypoint */
  
/* Following declaration gets rid of implicit declaration compiler warning
   It is a condensation of the lexer declaration from lex.yy.c:
   YY_BUFFER_STATE yy_scan_string YY_PROTO(( yyconst char *yy_str )); */
  extern int yy_scan_string(const char *);
#endif /* NCAP_INITIAL_SCAN_NOT_IN_NCAP_L */

  bool match;
  
  char *var_nm;  

  int n_lst_a=0;
  int n_lst_b=0;
  int n_lst_c=0;
  int n_lst_d=0;
  int n_lst_t=0;
  int tkn_crr=-1; /* [tkn] Current token, must initialize to non-zero value */
  int var_id;
  int var_idx;

  nm_id_sct *lst_a=NULL_CEWI;
  nm_id_sct *lst_b=NULL_CEWI;
  nm_id_sct *lst_c=NULL_CEWI;
  nm_id_sct *lst_d=NULL_CEWI;
  nm_id_sct *lst_t=NULL_CEWI;  
  
  YYSTYPE lval; /* [tkn] Token */

  if(spt_arg_cat){
    yy_scan_string(spt_arg_cat);
  }else{
    /* Open script file for reading */
    if((yyin=fopen(prs_arg->fl_spt,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),prs_arg->fl_spt);
      nco_exit(EXIT_FAILURE);
    } /* endif error */
  } /* endif input from script */
  
  /* While there are more tokens... */
  while(tkn_crr != 0){
    /* ...obtain next token from lexer... */
    tkn_crr=yylex(&lval,prs_arg);
    /* ...determine which variables and attributes exist in input file... */
    switch (tkn_crr){
    case IGNORE: break; /* Do nothing  */
    case SCV: break; /* Do nothing  */
    case EPROVOKE: break; /* Do nothing */
    case VAR: 
      /* Search for RHS variables in input file */
      var_nm=lval.var_nm_RHS;
      if(NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id)){
	match=False;
	for(var_idx=0;var_idx<n_lst_a;var_idx++)
	  if(!strcmp(lst_a[var_idx].nm,var_nm)){match=True; break;}
	if(match) break;
	if(n_lst_a==0) lst_a=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_a=(nm_id_sct *)nco_realloc(lst_a,((n_lst_a+1)*sizeof(nm_id_sct)));
	lst_a[n_lst_a].nm=strdup(var_nm);
	lst_a[n_lst_a++].id=var_id;
      } /* endif RHS variable is in input file */
      break; 
    case OUT_VAR: 
      /* Search for LHS variables in input file */
      var_nm=lval.var_nm_LHS;
      if(NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id)){
	match=False;
	for(var_idx=0;var_idx<n_lst_b;var_idx++)
	  if(!strcmp(lst_b[var_idx].nm,var_nm)){match=True; break;}
	if(match) break;
	if(n_lst_b == 0) lst_b=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_b=(nm_id_sct *)nco_realloc(lst_b,((n_lst_b+1)*sizeof(nm_id_sct)));
	lst_b[n_lst_b].nm=strdup(var_nm);
	lst_b[n_lst_b++].id=var_id;
      } /* endif LHS variable is in input file */
      break;
    case OUT_ATT:
      /* Search for LHS attribute's parent variable in input file */
      var_nm=lval.aed.var_nm;     
      if(NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id)){
	match=False;
	for(var_idx=0;var_idx<n_lst_c;var_idx++)
	  if(!strcmp(lst_c[var_idx].nm,var_nm)){match=True;break;}
	if(match) break;
	if(n_lst_c == 0) lst_c=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_c=(nm_id_sct *)nco_realloc(lst_c,((n_lst_c+1)*sizeof(nm_id_sct)));
	lst_c[n_lst_c].nm=strdup(var_nm);
	lst_c[n_lst_c++].id=var_id;
      } /* endif LHS attribute's parent variable is in input file  */
      break;
    case LHS_SBS:
      /* Add dimensions defined in LHS subscripts */
      lst_t=lst_d;n_lst_t=n_lst_t;
      lst_d=var_lst_add(prs_arg->in_id,lst_d,&n_lst_d,lval.sbs_lst->list,lval.sbs_lst->nbr);          
      if(n_lst_t >0) (void)var_lst_free(lst_t,n_lst_t);
      break;
    default: 
      (void)fprintf(stderr,"%s: WARNING Token type %d not handled in ncap_initial_scan()\n",prg_nm_get(),tkn_crr);
      break;
    } /* end switch */
  } /* end while */
  
  if(n_lst_a>0){*xtr_lst_a=lst_a;*nbr_lst_a=n_lst_a;}
  if(n_lst_b>0){*xtr_lst_b=lst_b;*nbr_lst_b=n_lst_b;}
  if(n_lst_c>0){*xtr_lst_c=lst_c;*nbr_lst_c=n_lst_c;}
  if(n_lst_d>0){*xtr_lst_d=lst_d;*nbr_lst_d=n_lst_d;}
  
} /* end ncap_initial_scan() */

/* End user subroutines section */
