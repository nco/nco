 %{
/* Begin C declarations section */

/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.y,v 1.14 2001-11-16 12:28:59 hmb Exp $ -*-C-*- */

/* Purpose: Grammar parser for ncap */

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

  /* Example yacc text:
     /data/zender/ora/lexyacc/ch3-05.y
     /home/thibaud/usr/local/src/gcc-2.7.2/c-parse.y
     /data/zender/gcc-2.7.2/c-parse.y
     parser_build_binary_op is in /data/zender/gcc-2.7.2/c-typeck.c
     unidata ncgen.y
  */

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */
#include <string.h>             /* strcmp. . . */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */

#include <netcdf.h>             /* netCDF definitions */
#include "nco.h"                 /* NCO definitions */
#include "nco_netcdf.h"         /* netCDF3 wrapper calls */
#include "ncap.h"               /* symbol table definition */


/* define a an ATTRIBUTE to contain a -1 */ 

/* Turn on parser debugging option (bison man p. 85) */
#define YYDEBUG 1
int yydebug=1;


/* Turns on more verbose errors than just plain "parse error" when yyerror() is called by parser */
#define YYERROR_VERBOSE 1



/* Bison manual p. 60 describes how to call yyparse() with arguments */
/* prs_sct must be consistent between ncap.y and ncap.c 
   fxm: Is there a way to define prs_sct in only one place? */

#define YYPARSE_PARAM prs_arg
#define YYLEX_PARAM prs_arg 
 int rcd; /* [enm] Return value for function calls */

/* End C declarations section */
%}
/* Begin parser declaration section */

/* Request pure, reentrant parser, so we can pass a structure to parser */
%pure_parser

/* NB: "terminal symbol" is just a fancy name for token produced by lexer 
   Symbols defined on LHS of rules are called "non-terminal symbols" or "non-terminals" 
   Examples of non-terminals are xpr, stt, stt_lst
   Examples of terminal symbols, or tokens, are NAME, NUMBER
   Convention is to make token names all uppercase, and non-terminals lowercase */

/* Define YYSTYPE union (type of lex variable yylval) */
%union{
  char *str;
  char *output_var;
  char *vara;
  aed_sct att;
  sym_sct *sym;
  parse_sct attribute;
  var_sct *var;
}

/* Tell parser which kind of values each token takes */
%token <str> STRING
%token <attribute> ATTRIBUTE
%token <vara> VAR
%token <output_var> OUT_VAR
%token <att> OUT_ATT
%token <sym> FUNCTION
%token POWER ABS ITOSTR

%type <attribute> a_exp
%type <str> string_exp
%type <var> var_exp
%type <output_var> out_var_exp
%type <att> out_att_exp

%left  '+' '-'
%left  '*' '/' '%'
%left  '^'
%nonassoc UMINUS

/* "type" declaration sets type for non-terminal symbols which otherwise need no declaration */
/*%type <val_double> xpr*/


/* End parser declaration section */
%%
/* Begin Rules section */
/* Format is rule: action */


program:           statement_list
                   ;

statement_list:     statement_list statement ';'
                  | statement ';'
                  ;


statement:     out_att_exp '=' a_exp
                { 
		  int index; 
                  aed_sct *ptr_aed;
                  
		  index=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct*)prs_arg)->att_lst,((prs_sct*)prs_arg)->nbr_att,True);
                  ptr_aed=((prs_sct*)prs_arg)->att_lst[index];                               
                  ptr_aed->val=ncap_attribute_2_ptr_unn($3);
                  ptr_aed->type=$3.type;
                  ptr_aed->sz = (long)nco_typ_lng($3.type);
                  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
                  
                 if(dbg_lvl_get() > 1) {
                  (void)fprintf(stderr,"Saving in array attribute %s:%s=",$1.var_nm,$1.att_nm);
                  switch($3.type){
                    case NC_BYTE:  (void)fprintf(stderr,"%d\n",$3.val.b); break;
                    case NC_SHORT: (void)fprintf(stderr,"%d\n",$3.val.s); break;
                    case NC_INT:   (void)fprintf(stderr,"%d\n",$3.val.l); break;
		    case NC_FLOAT: (void)fprintf(stderr,"%G\n",$3.val.f); break;		  
    		    case NC_DOUBLE:  (void)fprintf(stderr,"%.5G\n",$3.val.d);break;
		    default: break;
                  }/* end switch */
		 } /* end if */
                  (void)free($1.var_nm);
                  (void)free($1.att_nm);
                }
              | out_att_exp '=' string_exp 
                {
	 	  int index; 
                  int slen;
                  aed_sct *ptr_aed;
                  
                  slen =strlen($3);
		  index=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct*)prs_arg)->att_lst,((prs_sct*)prs_arg)->nbr_att,True);
                  ptr_aed=((prs_sct*)prs_arg)->att_lst[index];
                  ptr_aed->type=NC_CHAR;
                  ptr_aed->sz = (long)((slen+1)*nco_typ_lng(NC_CHAR));
                  ptr_aed->val.cp = (char *)nco_malloc((slen+1)*nco_typ_lng(NC_CHAR));
                  strcpy(ptr_aed->val.cp,$3);
                  (void)cast_nctype_void(NC_CHAR,&ptr_aed->val);    

                 if(dbg_lvl_get() > 1) (void)fprintf(stderr,"Saving in array,attribute %s:%s=%s\n",$1.var_nm,$1.att_nm,$3);
                 (void)free($1.var_nm);
                 (void)free($1.att_nm);
                 (void)free($3);
                }
              | out_att_exp '=' var_exp
                { 
                  /* Its OK to store 0 dimensional variables in an attribute */ 
                  int index;
                  aed_sct *ptr_aed;

                  if( $3->nbr_dim == 0  ){
                    index=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct*)prs_arg)->att_lst,((prs_sct*)prs_arg)->nbr_att,True);
                    ptr_aed=((prs_sct*)prs_arg)->att_lst[index];
                    ptr_aed->sz = nco_typ_lng($3->type)*($3->sz);
                    ptr_aed->val.vp = (void*)nco_malloc(ptr_aed->sz);
		    (void)var_copy($3->type,$3->sz,ptr_aed->val,$3->val);
		    ptr_aed->type = $3->type;
                    cast_nctype_void($3->type,&ptr_aed->val); 
		  }else{
                   (void)fprintf(stderr,"Warning: Cannot store a multi-dimensional variable in attribute %s:%s\n",$1.var_nm,$1.att_nm );
                  }
		  (void)free($1.var_nm);
                  (void)free($1.att_nm);
                  (void)var_free($3); 
		}   
              |  out_var_exp '=' var_exp 
                 {
		  int rcd;
                  int var_id;
                  $3->nm = strdup($1);
                  /* check to see if variable is already in output file */
                  rcd = nco_inq_varid_flg(((prs_sct*)prs_arg)->out_id,$3->nm,&var_id);
                  if( rcd == NC_NOERR ) {
                   (void)fprintf(stderr,"Warning: Variable %s has aleady been saved in %s\n", $3->nm,((prs_sct*)prs_arg)->fl_out);
                  }else{  
                    (void)ncap_var_write($3,(prs_sct*)prs_arg);
                    (void)fprintf(stderr,"Saving variable %s to  %s\n", $3->nm,((prs_sct*)prs_arg)->fl_out);
		  } /* end else */
		  (void)free($1);
                  (void)var_free($3);
                 }
              | out_var_exp '=' a_exp
                 {
		  int rcd;
                  int var_id;
                  var_sct *var;
                  rcd = nco_inq_varid_flg(((prs_sct*)prs_arg)->out_id,$1,&var_id);
                  if( rcd == NC_NOERR ) {
                       (void)fprintf(stderr,"Warning: Variable %s has aleady been saved in %s\n", $1,((prs_sct*)prs_arg)->fl_out);
                  }else{  
	    	   
                   var = (var_sct*)calloc(1,sizeof(var_sct));
		   var->nm = strdup($1);
                   var->nbr_dim = 0;
                   var->dmn_id = (int *)NULL;
                   var->sz = 1;
                   var->val = ncap_attribute_2_ptr_unn($3);
                   var->type = $3.type;
                   (void)ncap_var_write(var,(prs_sct*)prs_arg);
                   (void)var_free(var);
                   (void)fprintf(stderr,"Saving variable %s to  %s\n", $1,((prs_sct*)prs_arg)->fl_out);
		   }
		  (void)free($1);

		  }
               | out_var_exp '=' string_exp
                 {

                  int rcd;
                  int var_id;
                  var_sct *var;
                  rcd = nco_inq_varid_flg(((prs_sct*)prs_arg)->out_id,$1,&var_id);
                  if( rcd == NC_NOERR ) {
                       (void)fprintf(stderr,"Warning: Variable %s has aleady been saved in %s\n", $1,((prs_sct*)prs_arg)->fl_out);
                  }else{  
                   var = (var_sct*)calloc(1,sizeof(var_sct));
		   var->nm = strdup($1);
                   var->nbr_dim = 0;
                   var->dmn_id = (int *)NULL;
                   var->sz = strlen($3)+1;
                   var->val.cp=strdup($3);
                   var->type = NC_CHAR;
                   (void)cast_nctype_void(NC_CHAR,&var->val);
                   (void)ncap_var_write(var,(prs_sct*)prs_arg);
                   (void)var_free(var);
                   (void)fprintf(stderr,"Saving variable %s to  %s\n", $1,((prs_sct*)prs_arg)->fl_out);
		   }
		  (void)free($1);
                  (void)free($3);
                 }
              ;                    


a_exp:                 a_exp '+' a_exp   {
                                  (void)ncap_retype(&$1,&$3);
                                  $$=ncap_attribute_calc($1,'+',$3);                                
                           }
                     |  a_exp '-' a_exp        {
                                 (void)ncap_retype(&$1,&$3); 
                                 $$=ncap_attribute_calc($1,'-',$3);
                           }
                     | a_exp '*' a_exp        {
                                  (void)ncap_retype(&$1,&$3);
                                  $$=ncap_attribute_calc($1,'*',$3);
                                  }
                    | a_exp '/' a_exp        {
		                 (void)ncap_retype(&$1,&$3); 
                                 $$=ncap_attribute_calc($1,'/',$3);  
                                 }
                    | a_exp '%' a_exp {
                                 (void)ncap_retype(&$1,&$3);
                           
                                 $$=ncap_attribute_calc($1,'%',$3);  
		                 }
                    | '-' a_exp  %prec UMINUS        {
		                   (void)ncap_attribute_minus(&$2);
                                   $$=$2;
                         	}
                    | a_exp '^' a_exp        {
		                (void)ncap_attribute_conform_type(NC_DOUBLE,&$1);
		                (void)ncap_attribute_conform_type(NC_DOUBLE,&$3);
                                $$.val.d = pow($1.val.d,$3.val.d);
                                $$.type = NC_DOUBLE; 
                                }
                    | POWER '(' a_exp ',' a_exp ')' {
		                (void)ncap_attribute_conform_type(NC_DOUBLE,&$3);
		                (void)ncap_attribute_conform_type(NC_DOUBLE,&$5);
                                $$.val.d = pow($3.val.d,$5.val.d);
                                $$.type = NC_DOUBLE; 
		                }
		    
                    | ABS '(' a_exp ')' {
		               $$ = ncap_attribute_abs($3);
		               }
                    | FUNCTION '(' a_exp ')' {
		               (void)ncap_attribute_conform_type(NC_DOUBLE,&$3);
                               $$.val.d = (*($1->fnc))($3.val.d);
                               $$.type = NC_DOUBLE;
                               (void)free($1->nm);
                               (void)free($1);
		               }
                    | '(' a_exp ')'   {$$ = $2;}
                    | ATTRIBUTE   {$$ = $1; }
            ;



out_var_exp:   OUT_VAR { $$ = $1 }
               ;


out_att_exp:   OUT_ATT { $$ = $1 }
               ;

      
string_exp:    string_exp '+' string_exp {
                 size_t len;

                 len  = strlen($1) + strlen($3);
                 $$ = (char*)nco_malloc((len + 1 )*sizeof(char));

                 strcpy($$,$1);
                 strcat($$,$3);
                 (void)free($1);
                 (void)free($3);
               
                } 
              
              | ITOSTR '(' a_exp ')' {

		char buf[50];

                switch ($3.type){

                 case  NC_DOUBLE: sprintf(buf,"%.10G",$3.val.d); break;
                 case  NC_FLOAT:  sprintf(buf,"%G",$3.val.f); break;
                 case  NC_INT:    sprintf(buf,"%d",$3.val.l); break;
                 case  NC_SHORT:  sprintf(buf,"%d",$3.val.s); break;
                 case  NC_BYTE:   sprintf(buf,"%d",$3.val.b); break;
                 default:  break;
                } /* end switch */
	       $$ = strdup(buf);      
	      }

              | ITOSTR '(' a_exp ',' string_exp ')' {
	       
               char buf[150];
              	      
              /* format string according to info in string exp */
              /* Its up to the user to work out which format corresponds with which type */
              switch ($3.type){
                case  NC_DOUBLE: sprintf(buf,$5,$3.val.d); break;
                case  NC_FLOAT:  sprintf(buf,$5,$3.val.f); break;
                case  NC_INT:    sprintf(buf,$5,$3.val.l); break;
                case  NC_SHORT:  sprintf(buf,$5,$3.val.s); break;
                case  NC_BYTE:   sprintf(buf,$5,$3.val.b); break;
                  default:  break;
                } /* end switch */

	       (void)free($5);
	       $$ = strdup(buf);      
	      }

             |  STRING { $$=$1;}
               ;

var_exp:       var_exp '+' var_exp   { 
               $$=ncap_var_var_add($1,$3); 
               var_free($1); var_free($3);
              }
            | var_exp '+' a_exp {
               $$=ncap_var_attribute_add($1,$3);
               var_free($1);
              }            
            | a_exp '+' var_exp {
               $$=ncap_var_attribute_add($3,$1);
               var_free($3);
              }            
            | var_exp '-' var_exp  { 
                $$=ncap_var_var_sub($1,$3);
                var_free($1); 
                var_free($3);
             }
            | a_exp '-' var_exp { 
               var_sct *var1 ;
               parse_sct minus;
               minus.val.b = -1;
               minus.type= NC_BYTE;
               (void)ncap_attribute_conform_type($3->type,&minus);
               var1 = ncap_var_attribute_sub($3,$1);
               $$ = ncap_var_attribute_multiply(var1,minus);
               var_free(var1);
               var_free($3);
              }
            | var_exp '-' a_exp {
               $$=ncap_var_attribute_sub($1,$3);
               var_free($1);
               }
            | var_exp '*' var_exp  {
                $$ = ncap_var_var_multiply($1,$3); 
                var_free($1); var_free($3); 
             }
            | var_exp '*' a_exp {
                $$ = ncap_var_attribute_multiply($1,$3);
                var_free($1);
              }
            | var_exp '%' a_exp {
              $$=ncap_var_attribute_modulus($1,$3);
              var_free($1);
	    }
            | a_exp '*' var_exp {
                $$ = ncap_var_attribute_multiply($3,$1);
                var_free($3);
              }
            | var_exp '/' a_exp {
                $$=ncap_var_attribute_divide($1,$3);
              var_free($1);
              }
            | var_exp '^' a_exp {
               $$=ncap_var_attribute_power($1,$3);
	       var_free($1);
              }
            | POWER '(' var_exp ',' a_exp ')' {
               $$=ncap_var_attribute_power($3,$5);
	       var_free($3);
              }
            | '-' var_exp  %prec UMINUS { 
               parse_sct minus;
               minus.val.b = -1;
               minus.type = NC_BYTE;
               (void)ncap_attribute_conform_type($2->type,&minus);
               $$ = ncap_var_attribute_multiply($2,minus);
               var_free($2);      
              }
              | ABS '(' var_exp ')' {
		$$=ncap_var_abs($3);
                var_free($3);
              } 
              | FUNCTION '(' var_exp')' {
	       $$ = ncap_var_function($3,$1->fnc);
               var_free($3);
              (void)free($1->nm);
              (void)free($1);
 	      }  
            | '(' var_exp ')'        {
              $$ = var_dpl($2);
               var_free($2);
              }
            | VAR  { 
	      $$=ncap_var_init($1,((prs_sct*)prs_arg));
              }
	    
            ;
/* End Rules section */
%%
/* Begin User Subroutines section */

int
ncap_aed_lookup(char *var_nm,char *att_nm,aed_sct **att_lst,int *nbr_att, bool update)
{
  int i;
  
  for(i=0; i < *nbr_att ; i++)
    if (!strcmp(att_lst[i]->att_nm,att_nm) && !strcmp(att_lst[i]->var_nm,var_nm)) {   
        return i;
      } /* end if */

  if (!update) return -1;
  
  att_lst[*nbr_att] = (aed_sct *)nco_malloc(sizeof(aed_sct));
  
  att_lst[*nbr_att]->var_nm = strdup(var_nm);
  att_lst[*nbr_att]->att_nm = strdup(att_nm);
  
  return (*nbr_att)++;
}







