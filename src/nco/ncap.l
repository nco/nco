%{
  /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.l,v 1.21 2002-01-14 08:58:59 zender Exp $ -*-C-*- */

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
     Unidata ${DATA}/tmp/netcdf-3.5.0/src/ncgen/ncgen.l 
   */

  /* NB: flex with -Cf or -Cf not -I if this scanner will not be interactive */

  /* Standard header files */
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

#define INT_MIN (-2147483647-1) /* Minimum value of 4 byte integer -2^32 */
#define INT_MAX 2147483647 /* Maximum value of 4 byte integer  2^32 -1 */

/* We want yylex errors to skip current line rather than stop execution
   We do this by provoking error in parser by returning unrecognized token
   This causes parser to read up to next ';'
   To avoid having two error messages we prefix error message to yyerror with '#'
   This causes yyerror to print current error and skip next error
   fxm: Hackish, but I see no other way */ 

extern long int ln_nbr_crr;

/* Re-prototype yylex() to accept re-entrant arguments (Flex p. 12, Bison p. 60) */
#define YY_DECL int yylex(YYSTYPE *lvalp,prs_sct *prs_arg)

char err_sng[200]; 

/* Is lexer on left or right hand side of equals sign?
   LHS should contain only a variable or attribute
   In this case return variable/attribute name to parser
   If on RHS then return attribute value or variable struture */
static bool RHS;
%}

/* Following sections after %% are lexer rules
   Each rule is regular expression followed by C-code for action
   If token matches regular expression then C-code is executed */
%%
\n { 
  /* Ignore new lines */
  ln_nbr_crr++;
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
  strcpy( bfr,&yytext[1]);
  bfr[yyleng-2]='\0';
  
  /* Replace C-language escape codes with actual byte values */
  (void)sng_ascii_trn(bfr);
  lvalp->str=strdup(bfr);       
  if( dbg_lvl_get() > 1){
      printf( "Lexing string: %s\n",yytext);
      printf( "Made   string: %s\n",lvalp->str);
  } /* endif */
  return(STRING);
}

[0-9]*\.[0-9]*([eE][+-]?[0-9]+)?[LlDd]?|[0-9]*([eE][+-]?[0-9]+)[LlDd]? {
  /* Double or long double */
  /* NB: Tempted to prepend lexer expressions for floats and doubles with [+-]? 
     so that unary plus/minus is handled in lexer rather than parser.
     However, this has unintended side effects so let parser handle it for now */
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a double %s\n",yytext); 
  lvalp->attribute.val.d=strtod(yytext,(char **)NULL);
  lvalp->attribute.type=NC_DOUBLE;
  return(ATTRIBUTE);
}

[0-9]*\.[0-9]*([eE][+-]?[0-9]+)?[Ff]|[0-9]*([eE][+-]?[0-9]+)[Ff] {
  /* Float */
  float flt_tkn;
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a float %s\n",yytext); 
  if (sscanf((char*)yytext,"%e",&flt_tkn) != 1){
    sprintf(err_sng,"#Bad float: %s",yytext);
    yyerror(err_sng);
    return(EPROVOKE);
  } /* endif */
  lvalp->attribute.val.f=flt_tkn;
  lvalp->attribute.type=NC_FLOAT;
  return(ATTRIBUTE);    
}

[0-9]+[lL]? {
  /* Long */
  double dbl_tkn;
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing an integer %s\n",yytext); 
  dbl_tkn=strtod((char*)yytext, (char**)(NULL));
  if (dbl_tkn < INT_MIN || dbl_tkn > INT_MAX){
    lvalp->attribute.val.d=dbl_tkn;
    lvalp->attribute.type=NC_DOUBLE;
    return(ATTRIBUTE);
  }else{
    lvalp->attribute.val.l=(nco_long)dbl_tkn;
    lvalp->attribute.type=NC_INT;
    return(ATTRIBUTE);
  } /* end else */
}

[0-9]+[sS] {
  /* Short */
  short sht_tkn;
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a short %s\n",yytext); 	
  if (sscanf((char*)yytext, "%hd", &sht_tkn) != 1){
    sprintf(err_sng,"bad short constant: %s",(char*)yytext);
    yyerror(err_sng);
  }
  lvalp->attribute.val.s=sht_tkn;
  lvalp->attribute.type=NC_SHORT;
  return(ATTRIBUTE);    
}

[0-9]+[Bb] {
  /* Byte */
  int byt_tkn;
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a byte %s\n",yytext); 
  if(sscanf((char*)yytext,"%d",&byt_tkn) != 1){
    sprintf(err_sng,"#Bad byte: %s",yytext);
    yyerror(err_sng);
    return(EPROVOKE);
  }
  if(byt_tkn < -128 || byt_tkn >127){
    lvalp->attribute.val.l=byt_tkn;
    lvalp->attribute.type=NC_INT;
    return(ATTRIBUTE);    
  }else{ 
    lvalp->attribute.val.b=byt_tkn;
    lvalp->attribute.type=NC_BYTE;
    return(ATTRIBUTE);                    
  }/* end else */
}

abs/[ ]*\(     {return ABS;}
atostr/[ ]*\(  {return ATOSTR;}
pow/[ ]*\(     {return POWER;}

[A-Za-z_][A-Za-z0-9_]*/[ ]*\( {
/* Compare input with mathematical function names in table and return
   pointer to structure containing name, double function, float function */
   int i;
   for(i=0;i<prs_arg->sym_tbl_nbr;i++)
     if(!strcmp(yytext,(prs_arg->sym_tbl[i]->nm))){ 
        lvalp->sym=prs_arg->sym_tbl[i];
        return FUNCTION;
     } /* endif */
   (void)sprintf(err_sng,"Warning: Unable to locate function %s.",yytext);
   (void)yyerror(err_sng);       
   return EPROVOKE;             

} /* end functions */

[A-Za-z_][A-Za-z0-9_]*@[A-Za-z_][A-Za-z0-9_]* { 
  /* Recognize variable attributes, e.g., var_nm@att_nm */
  int var_id;
  int rcd=NC_NOERR; /* csz rcd may be used before it is defined */
  int rcd_out;
  int att_idx;
  size_t sng_lng;
  long att_sz;
  char *var_nm;
  char *att_nm;
  char *amp_ptr;
  nc_type type;
  ptr_unn val;
  aed_sct *ptr_aed;
  
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing attribute %s\n",yytext);
  amp_ptr=strchr(yytext,'@');
  sng_lng=amp_ptr-yytext;
  
  var_nm=(char *)nco_malloc((sng_lng+1)*sizeof(char));
  strncpy(var_nm,yytext,sng_lng);
  var_nm[sng_lng]='\0';
  att_nm=strdup(++amp_ptr);
  
  /* Assume global attribute when variable name is "global" */
  if(!strcmp(var_nm,"global")) var_id=NC_GLOBAL; else rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
  
  /* We are on RHS so return value of attribute (if available) to parser
     For initial scan, just return attributes on RHS */
  if(prs_arg->initial_scan){
    if(!RHS){
      lvalp->att.att_nm=strdup(att_nm);
      lvalp->att.var_nm=strdup(var_nm);
      return(OUT_ATT);
    }else{
      return(IGNORE);
    } /* endelse */
  } /* endif */
  
  if(RHS){
    /* First check if attribute has already been saved in symbol table
       If so then obtain and return value, else check input file
       If attribute is present obtain and return value from disk else
       return NC_BYTE of value 0.
       If attribute is NC_CHAR then STRING type is returned
       for others NC_FLOAT, NC_DOUBLE, NC_BYTE , NC_SHORT an ATTRIBUTE
       is returned to parser. */
    att_idx=ncap_aed_lookup(var_nm,att_nm,prs_arg->att_lst,prs_arg->nbr_att,False);
    if(att_idx >= 0){
      ptr_aed=prs_arg->att_lst[att_idx]; 
      if (ptr_aed->type == NC_CHAR){
	(void)cast_void_nctype(ptr_aed->type,&ptr_aed->val);
	lvalp->str=(char *)nco_malloc((ptr_aed->sz+1)*nco_typ_lng(NC_CHAR));
	strncpy(lvalp->str,ptr_aed->val.cp,ptr_aed->sz);
	lvalp->str[ptr_aed->sz]='\0';
	return(STRING);
      } else {
	lvalp->attribute=ncap_ptr_unn_2_attribute(ptr_aed->type,ptr_aed->val);
	return(ATTRIBUTE);  
      } /* end else */        
    } /* end if */
    if(rcd == NC_NOERR){  
      rcd=nco_inq_att_flg(prs_arg->in_id,var_id,att_nm,&type,&att_sz);
      if (rcd == NC_ENOTATT){
	(void)sprintf(err_sng,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
	(void)yyerror(err_sng);
	lvalp->attribute.val.b=0;
	lvalp->attribute.type=NC_BYTE;
	return(ATTRIBUTE);
      } /* end if */
      if(rcd == NC_NOERR){ 
	val.vp=(void*)nco_malloc(att_sz*nco_typ_lng(type));
	rcd=nco_get_att(prs_arg->in_id,var_id,att_nm,val.vp,type);
	if(type == NC_CHAR){
	  (void)cast_void_nctype(type,&val);
	  lvalp->str=(char*)nco_malloc((att_sz+1)*sizeof(char));
	  strncpy(lvalp->str,val.cp,att_sz);
	  lvalp->str[att_sz]='\0'; 
	  return(STRING);
	}else{
	  lvalp->attribute=ncap_ptr_unn_2_attribute(type,val);
	  return(ATTRIBUTE);   
	} /* end else */ 
      } /* end if */
    }else{ /* ...else rcd reported an error */
      /* Attribute is not in table or on disk so return 0-byte  */
      (void)sprintf(err_sng,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
      (void)yyerror(err_sng);
      lvalp->attribute.val.b=0;
      lvalp->attribute.type=NC_BYTE;
      return(ATTRIBUTE);
    } /* end if */
  }/* end if RHS */
  
  /* We are on LHS of = so simply save attribute information for
     later processing by parser
     Atrribute is valid if its associated variable is in input or output file */
  if(!RHS){
    rcd_out=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
    if(rcd == NC_NOERR || rcd_out == NC_NOERR){
      lvalp->att.att_nm=strdup(att_nm);
      lvalp->att.var_nm=strdup(var_nm);
      return(OUT_ATT);
    }else{
      (void)sprintf(err_sng,"#Warning: unable to locate variable %s. So cannot create attribute %s",var_nm,yytext);
      (void)yyerror(err_sng);
      return(EPROVOKE); 
    } /* end else */
  } /* end if */
  
} /* End attributes. Phew! */

[A-Za-z_][A-Za-z0-9_]* {
  /* Recognize variables */
  
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing variable %s\n",yytext);
  if(RHS){
    lvalp->vara=strdup(yytext); 
    return(VAR);
  }else{ 
    lvalp->output_var=strdup(yytext);
    return(OUT_VAR);
  } /* end else */
  
} /* end variables */

= {
  /* Equals */
  RHS=True; return yytext[0];
}

; { 
  /* End of statement indicator */
  RHS=False; return yytext[0]; 
}

. {
  /* Everything not parsed by above falls through to here */
  return  yytext[0];
}

%%

/* Begin user subroutines section */

bool
yywrap()
/* 
   bool yywrap(): output flag whether lexer input is finished
 */
{
  /* Purpose: Routine to replace library-defined yywrap() 
     yywrap() is called when YY_INPUT returns EOF
     Default is to return true (one) when called---this terminates scanner 
     Returning false (zero) means there is more to scan, and the input has
     been redirected to a new source, e.g., yyin points to a new file. */
 return 1;
} /* end yywrap() */

/* End user subroutines section */
