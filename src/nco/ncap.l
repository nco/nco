%{
  /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.l,v 1.15 2001-12-28 20:09:27 zender Exp $ -*-C-*- */

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

#define INT_MIN (-2147483647-1)  /* Min alue for a 4 byte integer -2^32 */
#define INT_MAX 2147483647       /* Max value for 4 byte integer  2^32 -1 */

/* When we have an error in yylex we want to skip the current line          */
/* Rather than stope execution. We can do this by provoking an error        */
/* in the parser by returning an unrecognized token                         */
/* This causes the parser to read up the next ';'                           */
/* To avoid having two error messages we prefix the error message to        */
/* yyerror with a '#' symbol this causes yyerror to print the current error */
/* and then skip the next one.  Its really a hack but I see no other way    */ 


extern long int line_number;

/* Reprototype yylex() to accept reentrant arguments (flex man p. 12, bison man p. 60) */
#define YY_DECL int yylex(YYSTYPE *lvalp,prs_sct *prs_arg)

char errstr[200]; 

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
\n          {
             /* Ignore new lines */
             line_number++;
             }

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

[0-9]*\.[0-9]*([eE][+-]?[0-9]+)?[LlDd]?|[0-9]*([eE][+-]?[0-9]+)[LlDd]? {
           if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a double %s\n",yytext); 
           
           lvalp->attribute.val.d = strtod(yytext,(char **)NULL );
           lvalp->attribute.type = NC_DOUBLE;
           
           return (ATTRIBUTE);
           
         }

[0-9]*\.[0-9]*([eE][+-]?[0-9]+)?[Ff]|[0-9]*([eE][+-]?[0-9]+)[Ff] {
          float fi;
          if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a float %s\n",yytext); 
          if (sscanf((char*)yytext, "%e", &fi) != 1) {
	    sprintf(errstr,"#Bad float : %s",yytext);
	    yyerror(errstr);
            return (EPROVOKE);
	  }
          lvalp->attribute.val.f = fi;
          lvalp->attribute.type = NC_FLOAT;
	  return (ATTRIBUTE);    
          }

[0-9]+[lL]? {
    	 double fi;

         if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing an integer %s\n",yytext); 
	 fi = strtod((char*)yytext, (char**)(NULL));

         if ( fi  < INT_MIN || fi > INT_MAX){
            lvalp->attribute.val.d = fi;
            lvalp->attribute.type = NC_DOUBLE;
            return (ATTRIBUTE);
           }else{
            lvalp->attribute.val.l=  (nco_long) fi;
            lvalp->attribute.type = NC_INT;
            return (ATTRIBUTE);
           }/* end else */
        }

[0-9]+[sS] {
	    short is;
           if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a short %s\n",yytext); 	
            if (sscanf((char*)yytext, "%hd", &is) != 1) {
		    sprintf(errstr,"bad short constant: %s",(char*)yytext);
		    yyerror(errstr);
	    }
	    
            lvalp->attribute.val.s = is;
            lvalp->attribute.type = NC_SHORT;
	    return (ATTRIBUTE);    
	   }
 
[0-9]+[Bb]  {
                int i;
                if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing a byte %s\n",yytext); 
		if (sscanf((char*)yytext, "%d", &i) != 1) {
		    sprintf(errstr,"#Bad byte : %s",yytext);
		    yyerror(errstr);
                    return (EPROVOKE);
		}
                if ( i < -128 || i >127){
                  lvalp->attribute.val.l = i;
                  lvalp->attribute.type = NC_INT;
		  return (ATTRIBUTE);    
		
		}else{ 
                  lvalp->attribute.val.b = i;
                  lvalp->attribute.type = NC_BYTE;
		  return (ATTRIBUTE);                    
		
                }/* end else */
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

atostr    { return ATOSTR; }

[A-Za-z_][A-Za-z0-9_]*@[A-Za-z_][A-Za-z0-9_]*   { 
        int var_id;
        int rcd=NC_NOERR; /* csz rcd may be used before it is defined */
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
        /* Recognize attributes eg var_nam@att_nm */
        /* -------------------------------------- */        
        
        if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Lexing attribute %s\n",yytext);
        p = strchr(yytext,'@');
        slen = p-yytext;

        var_nm = (char *)nco_malloc((slen+1)*sizeof(char));
        strncpy(var_nm,yytext,slen);
        var_nm[slen] = '\0';
        att_nm = strdup(++p);

/*        rcd = nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id); */
        if(!strcmp(var_nm,"global")) var_id=NC_GLOBAL; else rcd = nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
       
       /* We are on RHS so return value of attribute (if available) to parser */
       /* For initial scan, just return attributes on RHS */
       if(prs_arg->inital_scan){
          if( !RHS ){
            lvalp->att.att_nm = strdup(att_nm);
            lvalp->att.var_nm = strdup(var_nm);
            return (OUT_ATT);
          }else{
            return (IGNORE);
         }
       }    

       if (RHS){
            /* First check if attribute has already been saved in symbol table */
            /* If so then obtain and return value, else check input file */
            /* If attribute is present obtain and return value from disk else */
            /* return NC_BYTE of value 0. */
            /* If attribute is NC_CHAR then STRING type is returned */
            /* for others NC_FLOAT, NC_DOUBLE, NC_BYTE , NC_SHORT an ATTRIBUTE */
            /* is returned to parser. */

            index = ncap_aed_lookup(var_nm,att_nm,prs_arg->att_lst,prs_arg->nbr_att,False);
            
            if( index >= 0){
              ptr_aed = prs_arg->att_lst[index]; 
              if (ptr_aed->type == NC_CHAR){
              (void)cast_void_nctype(ptr_aed->type,&ptr_aed->val);
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
              (void)sprintf(errstr,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
              (void)yyerror(errstr);
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
              } else {
                 /* Attribute is not in table or on disk so return 0-byte  */
                (void)sprintf(errstr,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
                (void)yyerror(errstr);
                lvalp->attribute.val.b = 0;
                lvalp->attribute.type = NC_BYTE;
                return (ATTRIBUTE);
             } /* end if */
       }/* end if RHS */
        
        /* We are on LHS of = so simply save attribute information for */
        /* later processing by parser -- Atrribute is valid if its associated variable is */
        /* in input or output file */

        if(!RHS) {
          rcd_out = nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
          if( rcd == NC_NOERR || rcd_out == NC_NOERR ) {
            lvalp->att.att_nm = strdup(att_nm);
            lvalp->att.var_nm = strdup(var_nm);
            return (OUT_ATT);

            }else{
            (void)sprintf(errstr,"#Warning: unable to locate variable %s. So cannot create attribute %s",var_nm,yytext);
            (void)yyerror(errstr);
            return (EPROVOKE); 
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
