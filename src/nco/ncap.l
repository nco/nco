%{
  /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.l,v 1.11 2001-11-16 12:27:54 hmb Exp $ -*-C-*- */

  /* Purpose: Token generator for ncap parser */

/* Copyright (C) 1995--2001 Charlie Zender

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

  /* Example Lex text:
     /data/zender/ora/lexyacc/ch3-05.l
     GCC lexer is hard-coded in C for speed, but is easy to read:
     /home/thibaud/usr/local/src/gcc-2.7.2/c-lex.c
     unidata ncgen.l 
   */

  /* NB: flex with -Cf or -Cf not -I if this scanner will not be interactive */

  /* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */
#include <string.h>             /* strdup */
  /* 3rd party vendors */
#include <netcdf.h>             /* netCDF definitions */

  /* Headers specific to this program */
#include "nco.h"                 /* NCO definitions */
#include "nco_netcdf.h"          /* netcdf3 wrapper functions */
#include "ncap.h"               /* symbol table definition */
#include "ncap.tab.h"           /* symbol definitions from parser */



/* Reprototype yylex() to accept reentrant arguments (flex man p. 12, bison man p. 60) */
#define YY_DECL int yylex(YYSTYPE *lvalp,prs_sct *prs_arg)
 

/* Simple hack to tell wheather we are on the left or right hand side of */
/* of an equals sign. On LHS should only be a variable or attribute     */
/* So return the var/attrib name to the parser                           */
/* If on RHS then return the attribute value or a variable struture */
static bool RHS;
%}

/* Following sections after %% are lexer rules
   Each rule is regular expression followed by C-code for action
   If token matches regular expression then C-code is executed */
%%
\n          { /* Ignore new lines */ }

[ \t]+      { /* eat up spaces and tabs */  }

\/\/[^\n]* {
            /* ------------------- */
            /* enable c++ comments */
            /* ------------------- */
        
    }


\"[^"\n]*["\n] {
           /* ------------------------ */
           /* Process quoted strings   */
           /* ------------------------ */

        char *buf;
                
        buf = nco_malloc(yyleng*sizeof(char));
        strcpy( buf,&yytext[1]);
        buf[yyleng-2] = '\0';

        /* Replace any C language escape codes with their actual byte value */
 
        (void)sng_ascii_trn(buf);
 
        lvalp->str = strdup(buf);       

        if( dbg_lvl_get() > 1 )
        {
            printf( "Lexing string: %s\n",yytext );
            printf( "Made   string: %s\n",lvalp->str );
        }
        return (STRING);

    }

              


(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))((e[+-]?[0-9]+)?) {
           if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a double %s\n",yytext); 
           lvalp->attribute.val.d = strtod(yytext,(char **)NULL );
           lvalp->attribute.type = NC_DOUBLE;
           
           return (ATTRIBUTE);
           }

[0-9]+   { 
        /* ------------------ */
        /* recognize integers */
        /* ------------------ */
        
        if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing integer %s\n",yytext);

          lvalp->attribute.val.l =atoi(yytext);
          lvalp->attribute.type= NC_INT;
          return (ATTRIBUTE); 
        }

         /* Now define the double mathematical functions */

sin       { lvalp->sym = ncap_sym_init(yytext,sin); return FUNCTION; }

cos       { lvalp->sym = ncap_sym_init(yytext,cos); return FUNCTION; }

tan       { lvalp->sym = ncap_sym_init(yytext,tan); return FUNCTION; }

asin      { lvalp->sym = ncap_sym_init(yytext,asin); return FUNCTION; }

acos      { lvalp->sym = ncap_sym_init(yytext,acos); return FUNCTION; }

atan      { lvalp->sym = ncap_sym_init(yytext,atan); return FUNCTION; }

sqrt      { lvalp->sym = ncap_sym_init(yytext,sqrt); return FUNCTION; }
                  
log       { lvalp->sym = ncap_sym_init(yytext,log); return FUNCTION;  }

log10     { lvalp->sym = ncap_sym_init(yytext,log10); return FUNCTION; }

exp       { lvalp->sym = ncap_sym_init(yytext,exp); return FUNCTION; }

pow       { return POWER; }

abs       { return ABS; }

itostr    { return ITOSTR; }

[A-Za-z_][A-Za-z0-9_]*:[A-Za-z_][A-Za-z0-9_]*   { 
        int var_id;
        int rcd;
        int rcd_out;
        int index;
        size_t slen;
        long att_sz;
        char *var_nm;
        char *att_nm;
        char *p;
        nc_type type;
        ptr_unn val;
        aed_sct *ptr_aed;

        /* -------------------------------------- */
        /* Recognize attributes eg var_nam:att_nm */
        /* -------------------------------------- */        
        
        if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing attribute %s\n",yytext);
        p = strchr(yytext,':');
        slen = p-yytext;

        var_nm = (char *)nco_malloc((slen+1)*sizeof(char));
        strncpy(var_nm,yytext,slen);
        var_nm[slen] = '\0';
        att_nm = strdup(++p);
    

        rcd = nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
        
       
            /* We are on the RHS of the = sign so we need to return to the parser the value */
            /* of the attribue (if its available)                                           */
       
       if (RHS) {
            
            /* first check to see if attribute is already been saved in symbol table */
            /* if it is then obtain and return the value, else check the input file */
            /* if attribute is present obtain and return value from disk else       */
            /* return an NC_BYTE of value 0.                                        */
            /* if attribute is NC_CHAR then a STRING type is returned               */
            /* for the rest NC_FLOAT, NC_DOUBLE, NC_BYTE , NC_SHORT an ATTRIBUTE    */
            /* is returned to the parser.                                           */

            index = ncap_aed_lookup(var_nm,att_nm,prs_arg->att_lst,prs_arg->nbr_att,False);
            
            if( index >= 0) {
              ptr_aed = prs_arg->att_lst[index]; 
              if (ptr_aed->type == NC_CHAR){
              (void)cast_void_nctype(type,&ptr_aed->val);
              lvalp->str = (char *)nco_malloc((ptr_aed->sz+1)*nco_typ_lng(NC_CHAR));
              strncpy(lvalp->str,ptr_aed->val.cp,ptr_aed->sz);
              lvalp->str[ptr_aed->sz]='\0';
              return (STRING);
 
              } else {
              lvalp->attribute = ncap_ptr_unn_2_attribute(ptr_aed->type,ptr_aed->val);
              return (ATTRIBUTE);  
              
              } /* end else */        
            } /* end if */
            if ( rcd == NC_NOERR ) {  
              rcd = nco_inq_att_flg(prs_arg->in_id,var_id,att_nm,&type,&att_sz);
              if (rcd == NC_ENOTATT) {
              (void)fprintf(stderr,"WARNING: Unable to locate attribute %s in %s. Returning a 0 byte.\n",yytext,prs_arg->fl_in);
              lvalp->attribute.val.b = 0;
              lvalp->attribute.type = NC_BYTE;
              return (ATTRIBUTE);

              } /* end if */
              if(rcd == NC_NOERR) { 
                val.vp = (void*)nco_malloc(att_sz*nco_typ_lng(type));
                rcd = nco_get_att(prs_arg->in_id,var_id,att_nm,val.vp,type);

          
                if ( type == NC_CHAR ) {
                  (void)cast_void_nctype(type,&val);
                  lvalp->str = (char*)nco_malloc((att_sz+1)*sizeof(char));
                  strncpy(lvalp->str,val.cp,att_sz);
                  lvalp->str[att_sz] = '\0'; 
                  return (STRING);
 
                  }else{
                  lvalp->attribute = ncap_ptr_unn_2_attribute(type,val);
                  return (ATTRIBUTE);   
             
                  } /* end else */ 
         
                } /* end if */
              } /* end if */
       }/* end if RHS */
        
        /* we are on the LHS of the = sign so we simply save the attribute information for    */
        /* later processing by the parser -- Atrribute is valid if its associated variable is  */
        /* in input or output file */

        if(!RHS) {
          rcd_out = nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
          if( rcd == NC_NOERR || rcd_out == NC_NOERR ) {
            lvalp->att.att_nm = strdup(att_nm);
            lvalp->att.var_nm = strdup(var_nm);
            return (OUT_ATT);

            }else{
            (void)fprintf(stderr,"WARNING: unable to locate variable %s. So cannot create attribute %s\n",var_nm,yytext);
           } /* end else */
        } /* end if */


} /* end attributes. Phew ! */


[A-Za-z_][A-Za-z0-9_]*  {
       
        /* ------------------- */
        /* Recognize variables */
        /* ------------------- */
        
        if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing variable %s\n",yytext);
        if(RHS) {
          lvalp->vara = strdup(yytext); 
          return (VAR);
          }else{ 
            lvalp->output_var = strdup(yytext);
            //  lvalp->output_var = (char *)nco_malloc(strlen(yytext)+1); strcpy(lvalp->output_var,yytext);
            return (OUT_VAR);
         } /* end else */

} /* end variables */



=       {
        RHS = True; return yytext[0];
         }

;       { 
        RHS = False; return yytext[0]; 
        }



.   {
        /* ---------------- */
        /* all the rest ... */
        /* ---------------- */

        
        
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






