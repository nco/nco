header {
    #include <math.h>
    #include <malloc.h>
    #include <assert.h>
    #include <iostream>
    #include <string>
    #include "ncap2.hh"
    #include "NcapVar.hh"
    #include "NcapVarVector.hh"
    ANTLR_USING_NAMESPACE(std);
    ANTLR_USING_NAMESPACE(antlr);
    
}
options {
	language="Cpp";
}




class ncoParser extends Parser;
options {
    k=3;
	genHashLines = true;		// include line number information
	buildAST = true;			// uses CommonAST by default
    //analyzerDebug = true;
    //codeGenDebug= true;
}

tokens {
    NULL_NODE;
    BLOCK;
    ARG_LIST;
    DMN_LIST;
    LMT_LIST;
    LMT;
}

program:
        (statement)*
    ;

statement:
        block
        |assign_statement
        | if_stmt
        //deal with empty statement
        | SEMI! { #statement = #([ NULL_NODE, "null_stmt"]); } 
    ;        


// a bracketed block
block
  : LCURL! (statement)* RCURL!
  { #block = #( [BLOCK, "block"], #block ); }
  ;



if_stmt
  : IF^ LPAREN! expr RPAREN! statement 
    ( (ELSE) => else_part
      | () // nothing 
     )
  ;

else_part
  : ELSE^ statement
  ;



assign_statement:
         hyper_slb  ASSIGN^ expr SEMI!
       | cast_slb   ASSIGN^ expr SEMI!
    ;


hyper_slb: (VAR_ID|ATT_ID) (lmt_list)?
     ;

cast_slb:  VAR_ID dmn_list
     ;

// left association
func_exp: primary_exp | ( FUNC^ LPAREN!  expr RPAREN! )
    ;

// unary right association   
unary_exp:  func_exp | ( LNOT^| PLUS^| MINUS^ ) unary_exp
	;

// right association
pow_exp:    unary_exp (CARET^ pow_exp )? 
    ;

mexpr:
		pow_exp ( (TIMES^ | DIVIDE^ | MOD^) pow_exp)*
	;

add_expr:
		mexpr ( (PLUS^ | MINUS^ ) mexpr)* 
	;

rel_expr:
      add_expr (( LTHAN^ | GTHAN^ | GEQ^ | LEQ^ ) add_expr)*
    ;

eq_expr:
   rel_expr (( EQ^ | NEQ^ ) rel_expr)*
  ;

lmul_expr:
  eq_expr ( LAND^ eq_expr )*
  ;

expr:
   lmul_expr (LOR^ lmul_expr)*
    ;

lmt:
   (expr)? (COLON (expr)?)*
   { #lmt = #( [LMT, "lmt"], #lmt ); }
    ;

lmt_list
  : LPAREN! lmt (COMMA lmt)*  RPAREN!
   { #lmt_list = #( [LMT_LIST, "lmt_list"], #lmt_list ); }
  ;
// Use vars in dimension list so dims in [] can
// be used with or with out $ prefix. ie "$lon" or "lon" 
// So parser is compatible with ncap1
dmn_list
   :LSQUARE! (VAR_ID|DIM_ID) (COMMA! (VAR_ID|DIM_ID))* RSQUARE!
      { #dmn_list = #( [DMN_LIST, "dmn_list"], #dmn_list ); }
    ;
    
primary_exp
    : (LPAREN! expr RPAREN! ) 
    | BYTE
    | SHORT
    | INT
    | FLOAT    
    | DOUBLE
    | hyper_slb  //remember this include VAR_ID & ATT_ID
  ;


	  
imaginary_token
	: NRootAST
    ;

class ncoLexer extends Lexer;
options {
    k = 3;

}


tokens {
// token keywords
    IF ="if";
    ELSE="else";

}
{

private:
    prs_sct *prs_arg;
public:
    // Customized constructor !!
   ncoLexer(ANTLR_USE_NAMESPACE(std)istream& in, prs_sct *prs_in )
   : ANTLR_USE_NAMESPACE(antlr)CharScanner(new ANTLR_USE_NAMESPACE(antlr)CharBuffer(in),true)
   {
        prs_arg=prs_in;
        // This shouldn't really be here 
        // fxm:: should call default constructor
	    initLiterals();
   }



}


ASSIGN: '=';

LPAREN:	'(' ;

RPAREN:	')' ;

LCURL : '{' ;

RCURL : '}' ;

LSQUARE: '[';

RSQUARE: ']';

COMMA: ',' ;

SEMI:	';' ;

COLON: ':' ;

// Operators

CARET: '^' ;

TIMES:	'*' ;

DIVIDE:	'/';

MOD:	'%' ;

PLUS:	'+' ;

MINUS:	'-' ;

EQ: "==" ;

NEQ: "!=" ;

LTHAN:  '<' ;

GTHAN:  '>' ;

LEQ:  "<=" ;

GEQ:  ">=" ;

LAND: "&&" ;

LNOT: '!' ;

LOR: "||" ;


protected DGT:     ('0'..'9');
protected LPH:     ( 'a'..'z' | 'A'..'Z' | '_' );
protected LPHDGT:  ( 'a'..'z' | 'A'..'Z' | '_' | '0'..'9');
protected XPN:     ( 'e' | 'E' ) ( '+' | '-' )? ('0'..'9')+ ;





// Whitespace -- ignored
Whitespace	
	: ( ' ' |'\t' { tab(); } | '\f' |'\n' { newline(); })
		{ $setType(antlr::Token::SKIP);}
	;

CPP_COMMENT: "//" (~'\n')* '\n'
    { $setType(antlr::Token::SKIP); newline(); }
    ;


C_COMMENT:       
		"/*"
 		( { LA(2) != '/' }? '*'
		        | ( '\r' | '\n' )		{newline();}
                | ~( '*'| '\r' | '\n' )			
                )*
                "*/"                    
        { $setType(antlr::Token::SKIP);}
        ;


NUMBER:
	(DGT)+	{ $setType(INT); }
	(
 	|{ LA(2) >= '0' && LA(2) <= '9' }? '.' (DGT)+ (XPN)? { $setType(DOUBLE); }
	|	(XPN) { $setType(DOUBLE); }
    )(
    ('L'|'l')!    { $setType(INT);}
    | ('S'|'s')!    { $setType(SHORT);}
    | ('B'|'b')!    { $setType(BYTE);}
    | ('D'|'d')!    { $setType(DOUBLE);}
    |('F'|'f')!     { $setType(FLOAT);}
    )?        
;

// deal with number like .123, .2e3 ,.123f, 0.23d
NUMBER_DOT:
      '.' (DGT)+ (XPN)? { $setType(DOUBLE); }  
      (('D'|'d')!     { $setType(DOUBLE);}
       |('F'|'f')!     { $setType(FLOAT);}
      )?        
    ;

// Return var or attribute (var_nm@att_nm)
VAR_ATT:  (LPH)(LPH|DGT)*   
            {// check function table
            if( prs_arg->ptr_sym_vtr->find($getText) !=NULL )
               $setType(FUNC);             
             else $setType(VAR_ID); 
           }   
         ('@'(LPH)(LPH|DGT)*  {$setType(ATT_ID); } )?
;


DIM_ID: '$'! (LPH)(LPH|DGT)* {$setType(DIM_ID);}
   ;  

class ncoTree extends TreeParser;


{

private:
    prs_sct *prs_arg;
    bool bcast;
    var_sct* var_cast;;
public:
    void setTable(prs_sct *prs_in){
        prs_arg=prs_in;
    }
    // Customized Constructor
    ncoTree(prs_sct *prs_in){
        prs_arg=prs_in;
        ncoTree();
    }    

}



assign 
{
  var_sct *var;
}
   :(#(ASSIGN  VAR_ID ))=> #(ASSIGN  v:VAR_ID ){

       int dmn_nbr=0;
       char** sbs_lst;
       long *ind_lst;
       long ind_nbr=0;

       var_sct* var_out;
       RefAST aRef;

       bcast=false;  
            
       var_cast=(var_sct*)NULL;
       // Deal with LHS casting 
       if(v->getNextSibling()->getType() == DMN_LIST){
         sbs_lst=(char**)nco_calloc(NC_MAX_DIMS, sizeof(char*));
         aRef=v->getNextSibling()->getFirstChild();
         
         // Get dimension names in list       
         while(aRef) {
           sbs_lst[dmn_nbr++]=strdup(aRef->getText().c_str());
           aRef=aRef->getNextSibling();      
         }
           var_cast=ncap_cast_LHS(sbs_lst,dmn_nbr,prs_arg);     
           bcast=true;     
           var=out(v->getNextSibling()->getNextSibling());
           
           var->nm =strdup(v->getText().c_str());
           (void)ncap_var_write(var,prs_arg);

           bcast=false;
           var_cast=nco_var_free(var_cast); 
           (void)nco_sng_lst_free(sbs_lst,NC_MAX_DIMS);    
                
         }else 
          // deal with hyperslab ( only single indices )
          if(v->getNextSibling()->getType() == ARG_LIST){
            aRef=v->getNextSibling()->getFirstChild();
          ind_lst=(long*)nco_malloc(NC_MAX_DIMS*sizeof(long));
         // Calculate indices & put in list      
         while(aRef) {
           // Calculate indice         
           var_out=out(aRef);
           if(var_out->sz > 1) {
               fprintf(stderr, "Indicies must be single values\n");
               exit(1);
           }
           // convert var to int 
           var_out=nco_var_cnf_typ(NC_INT,var_out);
           (void)cast_void_nctype(NC_INT,&var_out->val);

           ind_lst[ind_nbr++]=var_out->val.lp[0];

           var_out=nco_var_free(var_out);
           printf("id %ld=%ld\n",ind_nbr,ind_lst[ind_nbr-1]);         
           aRef=aRef->getNextSibling();      
         }
         var=out(v->getNextSibling()->getNextSibling());
         var->nm =strdup(v->getText().c_str());
         (void)ncap_var_write(var,prs_arg);           
         }else {

         var_out=out(v->getNextSibling());
         var_out->nm =strdup(v->getText().c_str());
         (void)ncap_var_write(var_out,prs_arg);      
       }
      }
   |   (#(ASSIGN  ATT_ID ))=> #(ASSIGN  a:ATT_ID ){
        
       int dmn_nbr=0;
       char** sbs_lst;
       long *ind_lst;
       long ind_nbr=0;
       
       //token a  WILL contain a "@"
       string sa=a->getText();
       var_sct *var1;
       RefAST aRef;

       bcast=false;  
            
       // Deal with LHS casting 
       switch (a->getNextSibling()->getType()){
                
         case DMN_LIST:
               break;
                
         case ARG_LIST:
               break;
       
         // plain var on RHS can only be dimension 0 or 1;
         default:
             {
           var1=out(a->getNextSibling());
             NcapVar *Nvar=new NcapVar(sa,nco_var_dpl(var1));
             prs_arg->ptr_var_vtr->push_ow(Nvar);       
             (void)nco_var_free(var1);

             }       
           break;
         }
       }    
       ;     
            

out returns [var_sct *var]
{
	var_sct *var1;
    var_sct *var2;
}  
    // arithmetic operators 

    :  (#(PLUS out out)) =>  #( PLUS  var1=out var2=out)  
           { var=ncap_var_var_op(var1,var2, PLUS );}
	|  (#(MINUS out out)) => #( MINUS var1=out var2=out)
            { var=ncap_var_var_op(var1,var2, MINUS );}
	|	#(TIMES var1=out var2=out)
            { var=ncap_var_var_op(var1,var2, TIMES );}	
	|	#(DIVIDE var1=out var2=out)	
            { var=ncap_var_var_op(var1,var2, DIVIDE );}
	|	#(MOD var1=out var2=out)
	
    |   #(CARET var1=out var2=out)
            {var=ncap_var_var_pwr(var1,var2);}

    //unary Operators
    |   #(LNOT var1=out )      
            { var=ncap_var_var_op(var1,(var_sct*)NULL, LNOT );}
	|   #(MINUS var1=out )   
            { var=ncap_var_var_op(var1,(var_sct*)NULL, MINUS );}
    |   #(PLUS var1=out ) // do nothing   
            { var= var1; }
    
    // math functions 
    |  #(m:FUNC var1=out)       
         { 
          sym_sct * sym_ptr;
            
          sym_ptr= prs_arg->ptr_sym_vtr->find(m->getText());
          if(sym_ptr !=NULL)   var=ncap_var_fnc(var1,sym_ptr); 
            else{
              cout << "Function  " << m->getText() << " not found" << endl;
              exit(1);
               } 
          }


    // Logical Operators
    | #(LAND var1=out var2=out)  
            { var=ncap_var_var_op(var1,var2, LAND );}        
    | #(LOR  var1=out var2=out)  
            { var=ncap_var_var_op(var1,var2, LOR );}
    // Comparison Operators
    | #(LTHAN  var1=out var2=out) 
            { var=ncap_var_var_op(var1,var2, LTHAN );}
    | #(GTHAN  var1=out var2=out) 
            { var=ncap_var_var_op(var1,var2, GTHAN );}
    | #(GEQ  var1=out var2=out)   
            { var=ncap_var_var_op(var1,var2, GEQ );}
    | #(LEQ  var1=out var2=out)   
            { var=ncap_var_var_op(var1,var2, LEQ );}
    | #(EQ  var1=out var2=out)    
            { var=ncap_var_var_op(var1,var2, EQ );}
    | #(NEQ  var1=out var2=out)   
            { var=ncap_var_var_op(var1,var2, NEQ );}


    // Naked numbers
	|	c:BYTE		
          {  
            int ival;
            nc_type type=NC_BYTE;
            var=(var_sct *)nco_malloc(sizeof(var_sct));
            /* Set defaults */
            (void)var_dfl_set(var); 
            /* Overwrite with attribute expression information */
            var->nm=strdup("byte");
            var->nbr_dim=0;
            var->sz=1;
            // Get nco type
            ival=atoi(c->getText().c_str());
            var->type=type;
            
            var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
            (void)cast_void_nctype(type,&var->val);
            *var->val.bp = (signed char)ival;
            (void)cast_nctype_void(type,&var->val);
           }

	|	s:SHORT			
          {  
            int ival;
            nc_type type=NC_SHORT;
            var=(var_sct *)nco_malloc(sizeof(var_sct));
            /* Set defaults */
            (void)var_dfl_set(var); 
            /* Overwrite with attribute expression information */
            var->nm=strdup("short");
            var->nbr_dim=0;
            var->sz=1;
            var->type=type;

            ival=atoi(s->getText().c_str());
            var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
            (void)cast_void_nctype(type,&var->val);
            *var->val.sp = ival;
            (void)cast_nctype_void(type,&var->val);

           }
	|	i:INT			
          {  
            int ival;
            nc_type type=NC_INT;
            var=(var_sct *)nco_malloc(sizeof(var_sct));
            /* Set defaults */
            (void)var_dfl_set(var); 
            /* Overwrite with attribute expression information */
            var->nm=strdup("int");
            var->nbr_dim=0;
            var->sz=1;
            // Get nco type
            ival=atoi(i->getText().c_str());
            var->type=type;
            var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
            (void)cast_void_nctype(type,&var->val);
            *var->val.lp = ival;
            (void)cast_nctype_void(type,&var->val);
         }

    |   f:FLOAT        
          {  
            float  fval;
            nc_type type=NC_FLOAT;
            var=(var_sct *)nco_malloc(sizeof(var_sct));
            /* Set defaults */
            (void)var_dfl_set(var); 
            /* Overwrite with attribute expression information */
            var->nm=strdup("float");
            var->nbr_dim=0;
            var->sz=1;
            // Get nco type
            fval=atof(f->getText().c_str());
            var->type=type;
            var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
            (void)cast_void_nctype(type,&var->val);
            *var->val.fp = fval;
            (void)cast_nctype_void(type,&var->val);
           }

    |   d:DOUBLE        
        {  
            double r;
            nc_type type=NC_DOUBLE;
            var=(var_sct *)nco_malloc(sizeof(var_sct));
            /* Set defaults */
            (void)var_dfl_set(var); 
            /* Overwrite with attribute expression information */
            var->nm=strdup("double");
            var->nbr_dim=0;
            var->sz=1;
            // Get nco type
            r=strtod(d->getText().c_str(),(char**)NULL);
            var->type=type;
            var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
            (void)cast_void_nctype(type,&var->val);
            *var->val.dp = r;
            (void)cast_nctype_void(type,&var->val);
         }

    // variables & attributes
	|   v:VAR_ID       
        { 
          
          char *nm;
          var_sct *var_tmp;
          nm =strdup(v->getText().c_str());
          var=ncap_var_init(nm,prs_arg);
          if(var== (var_sct*)NULL){
               nco_exit(EXIT_FAILURE);
            }
          var->undefined=False;
  
          if(prs_arg->ntl_scn == True && bcast){
             var_tmp=nco_var_dpl(var_cast);
             var_tmp->id=var->id;
             var_tmp->nm=(char*)nco_free(var_tmp->nm);
             var_tmp->nm=strdup(nm);
             var_tmp->type=var->type;
             var_tmp->typ_dsk=var->typ_dsk;
             var_tmp->undefined=False;
             var_tmp->val.vp=(void*)NULL;
             var=nco_var_free(var);
             var=var_tmp;
           } /* endif ntl_scn */
  
          if(prs_arg->ntl_scn == False && bcast){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_cnf_dmn(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
           var_tmp=var;
           (void)ncap_var_stretch(&var_tmp,&var_cast);
           if(var_tmp != var) { 
           var=nco_var_free(var); 
           var=var_tmp;
          }
  
          if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching variable %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),var->nm,var_cast->nm,var_cast->nbr_dim,var_cast->sz);
    var->undefined=False;
            } /* endif LHS_cst */

        } /* end action */

    |   att:ATT_ID { 
            // check "output"
            NcapVar *Nvar;
            Nvar=prs_arg->ptr_var_vtr->find(att->getText());
            var=(var_sct*)NULL;    
            if(Nvar !=NULL)
                var=nco_var_dpl(Nvar->var);
            else    
                // Check input file for attribute
                var=ncap_att_init(att->getText().c_str(),prs_arg);

            if(var== (var_sct*)NULL){
                fprintf(stderr,"unable to locate attribute %s in input or output files\n",att->getText().c_str());
                nco_exit(EXIT_FAILURE);
            }
             
        }    
        ;


