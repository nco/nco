header {
/* $Header: /data/zender/nco_20150216/nco/src/nco++/ncoGrammer.g,v 1.117 2007-11-16 12:11:13 hmb Exp $ */

/* Purpose: ANTLR Grammar and support files for ncap2 */

/* Copyright (C) 2005--2007 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

    #include <algorithm>
    #include <iostream>
    #include <sstream>
    #include <string>
    #include <assert.h>
    #include <ctype.h>
    #include <malloc.h>
    #include <math.h>
    #if !(defined __xlC__) && !(defined SGIMP64) // C++ compilers that do not allow stdint.h
    #include <stdint.h> // Required by g++ for LLONG_MAX, ULLONG_MAX, by icpc for int64_t    
    #endif // C++ compilers that do not allow stdint.h
    #include "libnco++.hh"
    #include "NcapVar.hh"
    #include "NcapVarVector.hh"
    #include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()
    #include "NcapVector.hh"
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
    defaultErrorHandler=false;
    //analyzerDebug = true;
    //codeGenDebug= true;
}

tokens {
    NULL_NODE;
    BLOCK;
    ARG_LIST;
    DMN_LIST;
    DMN_ARG_LIST;
    LMT_LIST;
    VALUE_LIST;
    FUNC_ARG;
    LMT;
    EXPR;
    POST_INC;
    POST_DEC;
    UTIMES;
    SQR2;
    PROP; // used to differenciate properties & methods
    FOR2;    
    NORET;
}

program:
        (statement)*
    ;

statement:
        // assign/expression_statement
        expr SEMI! {#statement = #(#[EXPR,"EXPR"],#statement); }
        
        //Define Dim statement
        | DEFDIM^ LPAREN! NSTRING COMMA! expr RPAREN! SEMI!
        // while loop
        | WHILE^ LPAREN! expr RPAREN! statement 
        // for statement
        | for_stmt 
        // Jump Statements
        | BREAK SEMI!
        | CONTINUE SEMI!        
        //deal with empty statement
        | SEMI! { #statement = #([ NULL_NODE, "null_stmt"]); } 

        // if statement
        | IF^ LPAREN! expr RPAREN! statement 
         ( //standard if-else ambiguity
         options {warnWhenFollowAmbig = false;} 
        : ELSE! statement )?
        // print statement
        | PRINT^ LPAREN! (VAR_ID|ATT_ID|NSTRING) (COMMA! NSTRING)?  RPAREN! SEMI! 
        // Code block
        | block

   ;        



// a bracketed block
block:
    LCURL! (statement)* RCURL!
    { #block = #( [BLOCK, "block"], #block ); }
    ;


for_stmt:
     FOR^ LPAREN! (e1:expr)? SEMI! (e2:expr)? SEMI! (e3:expr)? RPAREN! st:statement
      /*   { if(#e1==NULL)  #e1 = #([ NULL_NODE, "null_stmt"]); 
           if(#e2==NULL)  #e2 = #([ NULL_NODE, "null_stmt"]); 
           if(#e3==NULL)  #e3 = #([ NULL_NODE, "null_stmt"]); 
           #for_stmt=#(FOR,e1,e2,e3,st);
         }  */
     ;


lmt:    (expr)? (COLON (expr)?)*
        { #lmt = #( [LMT, "lmt"], #lmt ); }
   ;



lmt_list: LPAREN! lmt (COMMA! lmt)*  RPAREN!
          { #lmt_list = #( [LMT_LIST, "lmt_list"], #lmt_list ); }
  ;


// Use vars in dimension list so dims in [] can
// be used with or with out $ prefix. ie "$lon" or "lon" 
// So parser is compatible with ncap1
dmn_list:
   LSQUARE! (VAR_ID|DIM_ID) (COMMA! (VAR_ID|DIM_ID))* RSQUARE!
      { #dmn_list = #( [DMN_LIST, "dmn_list"], #dmn_list ); }
    ;

// list of dims eg /$Lat,$time,$0,$1/
dmn_arg_list:
    DIVIDE! (DIM_ID|DIM_MTD_ID) (COMMA! (DIM_ID|DIM_MTD_ID))*  DIVIDE!
      { #dmn_arg_list = #( [DMN_ARG_LIST, "dmn_arg_list"], #dmn_arg_list ); }
    ;        

value_list:
      LCURL! expr (COMMA! expr)* RCURL!  
      { #value_list = #( [VALUE_LIST, "value_list"], #value_list ); }
    ;        

arg_list: expr|dmn_arg_list|DIM_ID|DIM_MTD_ID
     ;

func_arg:  LPAREN! (arg_list)? (COMMA! arg_list)*   RPAREN! 
      { #func_arg = #( [FUNC_ARG, "func_arg"], #func_arg ); }
     ;   

hyper_slb: (VAR_ID^ |ATT_ID ^) (lmt_list|dmn_list)?
     ;


/*************************************************************/
/* start  expressions */


meth_exp: primary_exp (DOT^ FUNC func_arg)*
     ;

// unary left association   
unaryleft_exp: meth_exp (
                          in:INC^ {#in->setType(POST_INC);
                                   #in->setText("POST_INC");}
                        | de:DEC^ {#de->setType(POST_DEC);
                                   #de->setText("POST_DEC");}
                          )?
    ;


// unary right association   
/*
unary_exp:  ( LNOT^| PLUS^| MINUS^ |INC^ | DEC^ | TIMES^ ) unary_exp
            | unaryleft_exp
	;
*/

unary_exp:  ( LNOT^| PLUS^| MINUS^ |INC^ | DEC^ 
             | ur:TIMES^ {#ur->setType(UTIMES);#ur->setText("UTIMES");} ) unary_exp
             | unaryleft_exp
    ;    



// right association
pow_exp: unary_exp (CARET^ pow_exp )? 
    ;

mexpr:
		 pow_exp ( (TIMES^ | DIVIDE^ | MOD^ ) pow_exp)*
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

lor_expr:
        lmul_expr (LOR^ lmul_expr)*
    ;

cond_expr: lor_expr ( QUESTION^ ass_expr COLON! cond_expr)?
    ;


ass_expr: cond_expr ( ( ASSIGN^   
                    | PLUS_ASSIGN^
                    | MINUS_ASSIGN^ 
                    | TIMES_ASSIGN^ 
                    | DIVIDE_ASSIGN^
                    ) (ass_expr|value_list) )?
    ;

// The mother of all expressions !!
expr:   ass_expr
    ;    
        
    
primary_exp
    : FLOAT    
    | DOUBLE
    | INT
    | BYTE
    | UBYTE    
    | SHORT
    | USHORT
    | UINT
    | INT64
    | UINT64
    | NSTRING    
    | DIM_ID_SIZE
    | LPAREN! expr RPAREN!  
    | FUNC^ func_arg
    | hyper_slb  //remember this includes VAR_ID & ATT_ID
  ;


/* End  expressions */
/*************************************************************/

	  
imaginary_token
	: NRootAST
    ;

class ncoLexer extends Lexer;
options {
    k = 4;
    defaultErrorHandler=false;
    filter=BLASTOUT;
    testLiterals=false;
}


tokens {
// token keywords
    IF ="if";
    ELSE="else";
    DEFDIM="defdim";
    SHIFTL="<<";
    SHIFTR=">>";

    BREAK="break";
    CONTINUE="continue";
    WHILE="while";    
    FOR="for";

    PRINT="print";  
 
    /*
    RAM_DELETE="ram_delete";
    RAM_WRITE="ram_write";
    SET_MISS="set_miss";
    CH_MISS="change_miss";
    */

}


{

private:
    prs_cls *prs_arg;
public:
    // Customized constructor !!
   ncoLexer(ANTLR_USE_NAMESPACE(std)istream& in, prs_cls *prs_in )
   : ANTLR_USE_NAMESPACE(antlr)CharScanner(new ANTLR_USE_NAMESPACE(antlr)CharBuffer(in),true)
   {
        prs_arg=prs_in;
        // This shouldn't really be here 
        // fxm:: should call default constructor
	    initLiterals();
   }



}


ASSIGN options { paraphrase="="; } : '=';

PLUS_ASSIGN options { paraphrase="+="; }  : "+=";

MINUS_ASSIGN options { paraphrase="-=";}  : "-=";

TIMES_ASSIGN options { paraphrase="*=";}  : "*=";

DIVIDE_ASSIGN options { paraphrase="/=";} : "/=";

INC options { paraphrase="++"; }: "++";

DEC options { paraphrase="--"; }: "--";

QUESTION options { paraphrase="?";} : '?';

LPAREN options { paraphrase="("; } :	'(' ;

RPAREN options { paraphrase=")";} :	')' ;

LCURL options { paraphrase="{"; } : '{' ;

RCURL options { paraphrase="}";} : '}' ;

LSQUARE options { paraphrase="["; } : '[';

RSQUARE options { paraphrase="]"; } : ']';

COMMA options { paraphrase=",";} : ',' ;

QUOTE options { paraphrase="\""; }: '"';

SEMI options { paraphrase=";";} :	';' ;

COLON options { paraphrase=":";}:  ':' ;

// Operators

CARET options { paraphrase="power of operator";} : '^' ;

TIMES options { paraphrase="*";} :	'*' ;

DIVIDE options { paraphrase="/";} :	'/';

MOD options { paraphrase="%"; } :	'%' ;

PLUS options {paraphrase="+";} :	'+' ;

MINUS options { paraphrase="-";} :	'-' ;

EQ options { paraphrase="=="; }  : "==" ;

NEQ options { paraphrase="!=";} : "!=" ;

LTHAN options { paraphrase="<";} :  '<' ;

GTHAN options { paraphrase=">"; } :  '>' ;

LEQ options { paraphrase="<="; }:  "<=" ;

GEQ options { paraphrase=">=";} :  ">=" ;

LAND options { paraphrase="&&";}: "&&" ;

LNOT options { paraphrase="!";} : '!' ;

LOR options { paraphrase="||";}: "||" ;

DOT options {paraphrase="dot operator";} : '.';

protected DGT:     ('0'..'9');
protected LPH:     ( 'a'..'z' | 'A'..'Z' | '_' );
protected LPHDGT:  ( 'a'..'z' | 'A'..'Z' | '_' | '0'..'9');
protected XPN:     ( 'e' | 'E' ) ( '+' | '-' )? ('0'..'9')+ ;


protected BLASTOUT: .
         {
          // blast out of lexer & parser
          // Can't use RecognitionException() as 
          // this is caught in lexer -- use TokenStreamRecognitionException()
          // instead -- This takes us back to try block in Invoke          

          ostringstream os;
          char ch=LA(0);
          os << getFilename() << " line " << getLine() << ", column "<< getColumn() 
             << ": unexpected character '" << ch << "'" << endl;

          ANTLR_USE_NAMESPACE(antlr)RecognitionException re(os.str());
          throw  ANTLR_USE_NAMESPACE(antlr)TokenStreamRecognitionException(re);
         }  
    ;     

	
UNUSED_OPS: ( "%=" | "^=" | "&=" | "|=" ) {
  
          ostringstream os;
          os << getFilename() << " line " << getLine() << ", column "<< getColumn() 
             << ": unused operator '" << getText() << "'" << endl;

          ANTLR_USE_NAMESPACE(antlr)RecognitionException re(os.str());
          throw  ANTLR_USE_NAMESPACE(antlr)TokenStreamRecognitionException(re);
         }  
    ;    


// Whitespace -- ignored
Whitespace options {paraphrase="white space"; } 	
	: ( ' ' |'\t' { tab(); } | '\f' |'\n' { newline(); })
		{ $setType(antlr::Token::SKIP);}
	;



CXX_COMMENT options {paraphrase="a C++-style comment"; } 
    : "//" (~'\n')* '\n'
    { $setType(antlr::Token::SKIP); newline(); }
    ;


C_COMMENT options {paraphrase="a C-style comment"; } 
        :       
		"/*"
 		( { LA(2) != '/' }? '*'
		        | ( '\r' | '\n' )		{newline();}
                | ~( '*'| '\r' | '\n' )			
                )*
                "*/"                    
        { $setType(antlr::Token::SKIP);}
        ;

// Numbers like .123, .2e3 ,.123f, 0.23d
// csz: Treat "l" or "L" following decimal point as "long double" as per C++
NUMBER_DOT options {paraphrase="a floating point number"; } 
     :
      '.' (DGT)+ (XPN)? { $setType(DOUBLE); }  
      ( ('D'|'d')!     {  $setType(DOUBLE);}
       |('F'|'f')!     {  $setType(FLOAT);}
       |('L'|'l')!     {  $setType(DOUBLE);}
      )?        
    ;

NUMBER:
	(DGT)+	{ $setType(INT); }
	( ( '.'  (DGT)* ((XPN)? | ('L'|'l')! )) { $setType(DOUBLE); } // 3.14e0, 3.14L, 3.14l
       | (XPN)         { $setType(DOUBLE);} // 3e0
       | ('L'|'l')!    { $setType(INT);   } // 3l, 3L
       | ('S'|'s')!    { $setType(SHORT); } // 3s, 3S
       | ('B'|'b')!    { $setType(BYTE);  } // 3b, 3B
       | ("UB"|"ub")!  { $setType(UBYTE); } // 3ub, 3UB
       | ("US"|"us")!  { $setType(USHORT); } // 3us, 3US
       | ('U'|'u'|"UL"|"ul")!    { $setType(UINT); } // 3u, 3U, 3ul, 3UL
       | ("LL"|"ll")!  { $setType(INT64); } // 3ll, 3LL
       | ("ULL"|"ull")!  { $setType(UINT64); } // 3ull, 3ULL
    )?
    (    ('F'|'f')!    { $setType(FLOAT); } // 3F, 3f
       | ('D'|'d')!    { $setType(DOUBLE);} // 3D, 3d
    )?        
;



// Return var or att (var_nm@att_nm)
VAR_ATT options {testLiterals=true; paraphrase="variable or attribute identifier"; } 
        :  (LPH)(LPH|DGT)*   
            {
            // check function/method vector
            if( std::binary_search(prs_arg->fmc_vtr.begin(),prs_arg->fmc_vtr.end(),fmc_cls($getText)))
               $setType(FUNC);             
            else 
               $setType(VAR_ID); 

           }   
           ('@'(LPH)(LPH|DGT)*  {$setType(ATT_ID); })?
   ;



// Return a quoted dim, var or att (var_nm@att_nm)
VAR_ATT_QT :( '\''!)
            (
             (
               ((LPHDGT|'-'|'.')+      {$setType(VAR_ID);})
               ( '@' (LPHDGT|'-'|'.')+ {$setType(ATT_ID);})?
               ('\''!)
              )|
              ( ('$'! (LPHDGT|'-'|'.')+ {$setType(DIM_ID);})
               ('\''!) ( ".size"! { $setType(DIM_ID_SIZE);})?
              )? 
             ) 
   ;

DIM_VAL options { paraphrase="dimension identifier"; } 
        : '$'! (LPH)(LPH|DGT)* 
            {$setType(DIM_ID);}
         ( ".size"!  
            { $setType(DIM_ID_SIZE);}
         )? 
   ;  

// Shorthand for naming dims in method e.g $0,$1, $2 etc
DIM_MTD_ID 
  options{paraphrase="dimension identifier";} 
  : '$'! (DGT)+
   ;            


NSTRING
  options{paraphrase="a string";} 
  : '"'! ( ~('"'|'\n'))* '"'! {$setType(NSTRING);}
   ;

class ncoTree extends TreeParser;
{

private:
    //prs_cls *prs_arg;
    bool bcst;
    var_sct* var_cst;
public:
    prs_cls *prs_arg;
    ASTFactory myFactory;




     //Structure to hold AST pointers to indices in hyperslabs -only temporary 
     typedef struct{
        ANTLR_USE_NAMESPACE(antlr)RefAST ind[3];
     } ast_lmt_sct;   

    void setTable(prs_cls *prs_in){
        prs_arg=prs_in; 

    }
   // Customized Constructor
    ncoTree(prs_cls *prs_in){
        prs_arg=prs_in;
        // default is NO. Casting variable set to true 
        // causes casting in function out(). var_cst must 
        // then be defined 
        bcst=false; 
        ncoTree();
    }

int 
lmt_init(
RefAST aRef, 
vector<ast_lmt_sct> &ast_lmt_vtr) 
{
      const std::string fnc_nm("lmt_init"); // [sng] Function name   

      int idx;
      int nbr_dmn;   
      int nbr_cln; // Number of colons in limit
      RefAST lRef;
      RefAST eRef;
      RefAST cRef;
      ast_lmt_sct hyp;

      if(aRef->getType() != LMT_LIST)
         return 0;
    
      lRef=aRef->getFirstChild();

      nbr_dmn=lmt_peek(aRef);      
      //nbr_dmn=lRef->getNumberOfChildren();

      for(idx=0 ; idx < nbr_dmn ; idx++){
         hyp.ind[0]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[1]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[2]=ANTLR_USE_NAMESPACE(antlr)nullAST;

             
       if(lRef->getType()!=LMT) 
            return 0;
       
        eRef=lRef->getFirstChild();
        nbr_cln=0;
        
       while(eRef) {
          if(eRef->getType() == COLON){
            cRef=eRef;        
            nbr_cln++;
          }
           eRef=eRef->getNextSibling();
        }
      
      // Initialise  to default markers
       switch(nbr_cln) {
          case 0: 
             break;
                
         case 1: hyp.ind[0]=cRef;
                 hyp.ind[1]=cRef;
                 break;

         case 2: hyp.ind[0]=cRef;
                 hyp.ind[1]=cRef;
                 hyp.ind[2]=cRef;
                 break;

         default: err_prn(fnc_nm,"Too many hyperslab indices");
                  break;  
        }

       eRef=lRef->getFirstChild();
       // point inidices to any expressions that exist
        nbr_cln=0;
       while(eRef) {
          if(eRef->getType() == COLON) 
             nbr_cln++; 
           else   
             hyp.ind[nbr_cln]=eRef;
           
           eRef=eRef->getNextSibling();
       }
       // save indices 
       ast_lmt_vtr.push_back(hyp);

       lRef=lRef->getNextSibling();
     }
     return nbr_dmn;

} 

int 
lmt_mk(
RefAST lmt,
NcapVector<lmt_sct*> &lmt_vtr ) 
{   
int nbr_dmn;
int idx;
int jdx;
long lcl_ind[3];
char *buff;

var_sct *var_out;
lmt_sct *lmt_ptr;
RefAST aRef;

vector<ast_lmt_sct> ast_lmt_vtr;


// populate ast_lmt_vtr
nbr_dmn=lmt_init(lmt,ast_lmt_vtr);

  for(idx=0 ; idx <nbr_dmn ; idx++){


     lcl_ind[0]=-2; lcl_ind[1]=-2; lcl_ind[2]=0; 

    for(jdx=0 ; jdx <3 ; jdx++){

     aRef=ast_lmt_vtr[idx].ind[jdx];

     if(!aRef)
        continue; //do nothing - use default lcl_ind values     
     else if( aRef->getType() == COLON){
       if(jdx <2) lcl_ind[jdx]=-1;
     }else{
         // Calculate number using out()
         var_out=out(aRef);

         // convert result to type int
          var_out=nco_var_cnf_typ(NC_INT,var_out);    
         (void)cast_void_nctype((nc_type)NC_INT,&var_out->val);

          // only interested in the first value.
         lcl_ind[jdx]=var_out->val.lp[0];

         var_out=nco_var_free(var_out);
        }
     }// end jdx
         
     // fill out lmt structure
     // use same logic as nco_lmt_prs 
     lmt_ptr=(lmt_sct*)nco_calloc((size_t)1,sizeof(lmt_sct));

     lmt_ptr->nm=NULL;
     //lmt_ptr->lmt_typ=-1;
     lmt_ptr->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
     lmt_ptr->min_sng=NULL;
     lmt_ptr->max_sng=NULL;
     lmt_ptr->srd_sng=NULL;
     /* rec_skp_nsh_spf is used for record dimension in multi-file operators */
     lmt_ptr->rec_skp_nsh_spf=0L; /* Number of records skipped in initial superfluous files */
    
    
    /* Fill in structure */
    if( lcl_ind[0] >= 0) {
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[0]);
           lmt_ptr->min_sng=buff; 
    }    

    /* Fill in structure */
    if( lcl_ind[1] >= 0) {
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[1]);
           lmt_ptr->max_sng=buff;
    }    

    /* Fill in structure */
    if( lcl_ind[2] > 0) {
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[2]);
           lmt_ptr->srd_sng=buff;
    }    

    /* need to deal with situation where only start is defined -- ie picking only a single value */
    if(lcl_ind[0] >=0 && lcl_ind[1]==-2){
          buff=(char*)nco_malloc(20*sizeof(char));
          (void)sprintf(buff, "%ld",lcl_ind[0]);
          lmt_ptr->max_sng=buff; 
    }    


    if(lmt_ptr->max_sng == NULL) lmt_ptr->is_usr_spc_max=False; else lmt_ptr->is_usr_spc_max=True;
    if(lmt_ptr->min_sng == NULL) lmt_ptr->is_usr_spc_min=False; else lmt_ptr->is_usr_spc_min=True;

    lmt_vtr.push_back(lmt_ptr);


   } // end idx

   return nbr_dmn;

} /* end lmt_mk */


/* Legacy run -- will remove soon
public:
    void run(RefAST tr){
        while(tr) {
          (void)statements(tr);   
          tr=tr->getNextSibling();   
        }
    }
*/

public:
    void run_dbl(RefAST tr,int icnt){
     int idx=0;
     RefAST ntr=tr;

     extern int      
     ncap_mpi_exe(
     std::vector< std::vector<RefAST> > &all_ast_vtr,
     ncoTree** wlk_ptr,
     int nbr_wlk);


      
     //small list dont bother with double parsing     
     if(icnt <4) goto small;

     //Initial scan
     prs_arg->ntl_scn=True;
     while(idx++ < icnt){
         (void)statements(ntr);   
       ntr=ntr->getNextSibling();   
     }

     //Define variables in output
    (void)nco_redef(prs_arg->out_id);  
    (void)prs_arg->ncap_def_ntl_scn();
    (void)nco_enddef(prs_arg->out_id);  


    // Sort expressions - MPI preparation
    if(prs_arg->NCAP_MPI_SORT){  
       prs_arg->ntl_scn=False;
      // nb A vector of vectors
      std::vector< std::vector<RefAST> > all_ast_vtr;
      ncoTree **wlk_vtr=(ncoTree**)NULL; 

      // Populate and sort  vector 
      (void)ncap_mpi_srt(tr,icnt,all_ast_vtr,prs_arg);
       
      // Evaluate expressions (execute)
      (void)ncap_mpi_exe(all_ast_vtr,wlk_vtr,0);  
      
      /*  
      for(unsigned vtr_idx=0 ; vtr_idx<all_ast_vtr.size(); vtr_idx++)
        for(unsigned jdx=0 ; jdx<all_ast_vtr[vtr_idx].size(); jdx++)
	     (void)statements(all_ast_vtr[vtr_idx][jdx]);
      */
      
    goto end;
    } //end if

   


small: 
     idx=0;
     ntr=tr;
     //Final scan
     prs_arg->ntl_scn=False;
     while(idx++ < icnt){
       (void)statements(ntr);   
       ntr=ntr->getNextSibling();   
     }
end: ;
   }

public:
    int run_exe(RefAST tr, int nbr_dpt){
    // number of statements in block
    int nbr_stmt=0;
    int idx;
    int icnt=0;
    int ntyp;
    int iret=0;
    
    RefAST etr=ANTLR_USE_NAMESPACE(antlr)nullAST;
    RefAST ntr;
   
    ntr=tr;
    do nbr_stmt++; 
    while(ntr=ntr->getNextSibling());        
            
     
    
    if(nbr_stmt <4 || nbr_dpt>0 ){
        prs_arg->ntl_scn=False;
        ntr=tr;
        do{ 
          iret=statements(ntr);
          // break if jump statement   
          if(iret==BREAK || iret==CONTINUE) 
           break; 
        } while(ntr=ntr->getNextSibling());   
        goto exit;
    }
  
    ntr=tr;

    for(idx=0 ; idx < nbr_stmt; idx++){
      ntyp=ntr->getType();
      // we have hit an IF or a basic block
      if(ntyp==BLOCK || ntyp==IF ||ntyp==DEFDIM || ntyp==WHILE ||ntyp==FOR) {
        if(icnt>0) 
         (void)run_dbl(etr,icnt);
        icnt=0;
        etr=ANTLR_USE_NAMESPACE(antlr)nullAST;; 
        prs_arg->ntl_scn=False;
        iret=statements(ntr);      
      }else{
        if(icnt++==0) etr=ntr;
       }        

     ntr=ntr->getNextSibling();
      
    } // end for

    if(icnt >0){
       iret=0;
       (void)run_dbl(etr,icnt);      
    }
      
exit: return iret;     
            

    } // end run_exe


} // end native block




// Return the number of dimensions in lmt subscript
lmt_peek returns [int nbr_dmn=0]

  : lmt:LMT_LIST{
    RefAST aRef;     
    aRef=lmt->getFirstChild();
    nbr_dmn=0;
    while(aRef) {
      if(aRef->getType() == LMT) nbr_dmn++;    
        aRef=aRef->getNextSibling();     
    }   
  }
  ;



statements returns [int iret=0] 
{
var_sct *var;
const std::string fnc_nm("statements");
// list of while/for loops entered n.b depth is lpp_vtr.size()
// Temporary fix so call run_exe only does a single parse in the
// nested block
static std::vector<std::string> lpp_vtr;
}
    : blk:BLOCK { 
       //cout <<"Num of Children in block="<<blk->getNumberOfChildren()<<endl;
       iret=run_exe(blk->getFirstChild(),lpp_vtr.size() );
            
                }

    | #(exp:EXPR ass:.) {
       RefAST tr;
       RefAST ntr;  

       if(ass->getType()==ASSIGN && !prs_arg->ntl_scn ){
         ntr=ass->getFirstChild();
         if(ntr->getType()==TIMES) 
           ntr=ntr->getFirstChild();
        
         if(ntr->getType() == VAR_ID || ntr->getType() ==ATT_ID){
           ntr->addChild( astFactory->create(NORET,"no_ret") );
           // std::cout << "Modified assign "<<exp->toStringTree()<<std::endl;      
         }
       } 


       var=out(exp->getFirstChild());
       if(var != (var_sct*)NULL)
         var=nco_var_free(var);
       iret=EXPR;
     }
              

    | #(IF var=out stmt:. ) {
    //if can have only 3 or 4 parts  , 1 node and 2 or 3 siblings
    // IF LOGICAL_EXP STATEMENT1 STATEMENT2
      bool br;
	  //Calculate logical expression
	  br=ncap_var_lgcl(var);
	  var=nco_var_free(var);

      if(br){ 
         // Execute 2nd sibling  
         if(stmt->getType()==BLOCK)
           iret=run_exe(stmt,lpp_vtr.size());
         else
           iret=statements(stmt);     
      }

      // See if else stmt exists (3rd sibling)       
	  if(!br && (stmt=stmt->getNextSibling()) ){
         if(stmt->getType()==BLOCK)
           iret=run_exe(stmt,lpp_vtr.size());
         else
           iret=statements(stmt);     
             
      }
 
      var=NULL_CEWI;
      
    }// end action

    | #(WHILE lgcl:. stmt1:.){

      bool br;
      var_sct *var_tf;  
      
      var_tf=out(lgcl);
      br=ncap_var_lgcl(var_tf);
      var_tf=nco_var_free(var_tf);
       
      lpp_vtr.push_back("while");

      while(br){ 
        if(stmt1->getType()==BLOCK)
          iret=run_exe(stmt1,lpp_vtr.size());
        else
         iret=statements(stmt1);     
       
        if(iret==BREAK) break;
        var_tf=out(lgcl);
        br=ncap_var_lgcl(var_tf); 
        var_tf=nco_var_free(var_tf);       

      }
      lpp_vtr.pop_back(); 
      iret=WHILE;
      var=NULL_CEWI; 

    }
   
    |#(FOR e1:. e2:. e3:. stmt2:.) {
      bool b1,b2,b3,br;
      var_sct *var_f1;
      var_sct *var_f2;
      var_sct *var_f3;

      b1=(e1->getType()!=NULL_NODE ? true:false);
      b2=(e2->getType()!=NULL_NODE ? true:false);
      b3=(e3->getType()!=NULL_NODE ? true:false);

      lpp_vtr.push_back("for");

      if(b1){
       var_f1=out(e1);
       var_f1=nco_var_free(var_f1);
      }          

      if(b2){
       var_f2=out(e2);
       br=ncap_var_lgcl(var_f2);
       var_f2=nco_var_free(var_f2);
      } else br=true;

      while(br){

        if(stmt2->getType()==BLOCK)
          iret=run_exe(stmt2,lpp_vtr.size());
        else
         iret=statements(stmt2);     
       
        if(iret==BREAK) break;


        if(b3){
          var_f3=out(e3);
          var_f3=nco_var_free(var_f3);
        }

  
        if(b2){
         var_f2=out(e2);
         br=ncap_var_lgcl(var_f2);
         var_f2=nco_var_free(var_f2);
        } 

      } // end while

      lpp_vtr.pop_back();
      iret=FOR;
      var=NULL_CEWI;                
        
    } // end  for action


    | ELSE { iret=ELSE;}
    | BREAK { iret=BREAK;}
    | CONTINUE {iret=CONTINUE;} 
    | NULL_NODE { iret=NULL_NODE; }
   
    |#(DEFDIM def:NSTRING  var=out){
            
        const char *dmn_nm;
        long sz;
            
        dmn_nm=def->getText().c_str();
            
        var=nco_var_cnf_typ(NC_INT,var);
        iret=DEFDIM;

        (void)cast_void_nctype((nc_type)NC_INT,&var->val);
        sz=*var->val.lp;
        var=(var_sct*)nco_var_free(var);
        (void)ncap_def_dim(dmn_nm,sz,prs_arg);
     }


    // All the following functions have iret=0
    | (#(PRINT VAR_ID))=> #(PRINT pvid:VAR_ID){

          int var_id;
          int fl_id=-1;
          char *fmt_sng;
          std::string va_nm(pvid->getText());
          NcapVar *Nvar;
          
          
          if(prs_arg->ntl_scn) goto end2;
          Nvar=prs_arg->var_vtr.find(va_nm);

          if(Nvar->flg_mem){ 
            wrn_prn(fnc_nm,"Cannot print out RAM variables at the moment!");
            goto end2;
          }

          if(pvid->getNextSibling() && pvid->getNextSibling()->getType()==NSTRING)
            fmt_sng=strdup(pvid->getNextSibling()->getText().c_str());
          else 
            fmt_sng=(char*)NULL; 


 
          if(Nvar)
           fl_id=prs_arg->out_id;
          else{
           /* Check input file for var */  
           if(NC_NOERR==nco_inq_varid_flg(prs_arg->in_id,va_nm.c_str(),&var_id))
            fl_id=prs_arg->in_id;
          }
           
   
          if( fl_id >=0)
           (void)nco_prn_var_val_lmt(fl_id,va_nm.c_str(),(lmt_sct*)NULL,0L,fmt_sng,prs_arg->FORTRAN_IDX_CNV,False,False);
            

          if(fmt_sng)
            fmt_sng=(char*)nco_free(fmt_sng); 
         

        end2: ;

    }
    | (#(PRINT ATT_ID))=> #(PRINT patt:ATT_ID){
             ;
    }

    | (#(PRINT NSTRING))=> #(PRINT pstr:NSTRING){
       char *prn_sng;
       
       if(!prs_arg->ntl_scn){
        prn_sng=strdup(pstr->getText().c_str());
        (void)sng_ascii_trn(prn_sng);            

        fprintf(stdout,prn_sng);
        prn_sng=(char*)nco_free(prn_sng);
      }    
    }



    ;

// Parse assign statement - Initial Scan
assign_ntl [bool bram] returns [var_sct *var]
{
const std::string fnc_nm("assign_ntl"); 
var=NULL_CEWI;
}
   : (#(VAR_ID LMT_LIST ))=> #(vid:VAR_ID lmt:LMT_LIST){


              if(dbg_lvl_get() > 0)
                dbg_prn(fnc_nm,vid->getText()+"(limits)");


               const char *var_nm; 
               var_sct *var_lhs;
               NcapVar *Nvar;              

               var_nm=vid->getText().c_str();

               var_lhs=prs_arg->ncap_var_init(vid->getText(),false);
               if(var_lhs){
                 var=nco_var_dpl(var_lhs);
                 (void)prs_arg->ncap_var_write(var_lhs,bram);
               } else {

                 // set var to udf
                 var_lhs=ncap_var_udf(var_nm);
                 var=nco_var_dpl(var_lhs);
 
                 Nvar=new NcapVar(var_lhs);
                 (void)prs_arg->int_vtr.push_ow(Nvar);
               }
        }                    
 
        | (#(VAR_ID DMN_LIST ))=> #(vid1:VAR_ID dmn:DMN_LIST){   
                               
              int idx;
              const char *var_nm;
              var_sct *var1;
              std::vector<std::string> str_vtr;
              RefAST  aRef;
              NcapVar *Nvar;
              int str_vtr_sz;
              

              if(dbg_lvl_get() > 0)
                dbg_prn(fnc_nm,vid1->getText()+"[dims]");

              var_nm=vid1->getText().c_str(); 

              // set class wide variables
              bcst=true;  
              var_cst=NULL_CEWI;

              aRef=dmn->getFirstChild();
         
              // pPut dimension names in vector       
              while(aRef) {
                str_vtr.push_back(aRef->getText());
                aRef=aRef->getNextSibling();      
              }
              
              //Check that all dims exist
              str_vtr_sz=str_vtr.size();
              for(idx=0 ; idx < str_vtr_sz ; idx++)
                if(   prs_arg->dmn_in_vtr.findi(str_vtr[idx]) ==-1             
                   && prs_arg->dmn_out_vtr.findi(str_vtr[idx]) ==-1)      
                  break;

               // return undef if dim missing 
               if( idx <str_vtr_sz){
                  var=NULL_CEWI;

               } else {

                 // Cast is applied in VAR_ID action in function out()
                 var_cst=ncap_cst_mk(str_vtr,prs_arg);

                 var1=out(vid1->getNextSibling());
                 if(var1->undefined) {
                    var=NULL_CEWI;
                 } else {
                   var_cst=nco_var_cnf_typ(var1->type,var_cst);
                   var_cst->typ_dsk=var1->type;
                   var=nco_var_dpl(var_cst);

                   }
                 var1=nco_var_free(var1);
              }
              

              if(!var){
                var1=ncap_var_udf(var_nm);
                Nvar=new NcapVar(var1);
                (void)prs_arg->int_vtr.push_ow(Nvar);
                var=nco_var_dpl(var1);
              } else{
                var->nm=(char*)nco_free(var->nm);
                var->nm=strdup(var_nm);
                var1=nco_var_dpl(var);
                prs_arg->ncap_var_write(var1,bram);
             }

              if(var_cst)
                var_cst=nco_var_free(var_cst);
 
              bcst=false;   
              
            }

          | vid2:VAR_ID {   
              

              if(dbg_lvl_get() > 0)
                dbg_prn(fnc_nm,vid2->getText());
             

               var_sct *var1;
               
               // Set class wide variables           
               bcst=false;
               var_cst=NULL_CEWI; 
             

               // get shape from RHS
               var1=out(vid2->getNextSibling());
               (void)nco_free(var1->nm);                
               var1->nm =strdup(vid2->getText().c_str());

               //Copy return variable
               var=nco_var_dpl(var1);
                
               // Write var to int_vtr
               // if var already in int_vtr or var_vtr then write call does nothing
               (void)prs_arg->ncap_var_write(var1,bram);
               //(void)ncap_var_write_omp(var1,bram,prs_arg);
        
           } // end action
       
   |   (#(ATT_ID LMT_LIST))=> #(att:ATT_ID LMT_LIST){
        ;
        } 
   |   (#(ATT_ID LMT_DMN))=>  #(att1:ATT_ID DMN_LIST){
        ;
        } 
   | att2:ATT_ID {
       
        //In Initial scan all newly defined atts are flagged as Undefined
        var_sct *var1;
        NcapVar *Nvar;

        if(dbg_lvl_get() > 0)
          dbg_prn(fnc_nm,att2->getText());

      
        var1=ncap_var_udf(att2->getText().c_str());

        Nvar=new NcapVar(var1);
        prs_arg->int_vtr.push_ow(Nvar);          

        // Copy return variable
        var=nco_var_dpl(var1);    

       } //end action

    ; // end assign block


assign [bool bram] returns [var_sct *var]
{
const std::string fnc_nm("assign"); 
var=NULL_CEWI;
}

   :   (#(VAR_ID LMT_LIST ))=> #(vid:VAR_ID lmt:LMT_LIST){

               int idx;
               int nbr_dmn;
               int var_id; 
               int slb_sz;
               const char *var_nm;
               
              if(dbg_lvl_get() > 0)
                dbg_prn(fnc_nm,vid->getText()+"(limits)");


               RefAST lmt_Ref;
               var_sct *var_lhs=NULL_CEWI;
               var_sct *var_rhs=NULL_CEWI;
               NcapVector<lmt_sct*> lmt_vtr;          
               
               lmt_Ref=vid->getFirstChild();

               bcst=false;
               var_cst=NULL_CEWI;
               var=NULL_CEWI;
               NcapVar *Nvar; 
               
              

              
               var_nm=vid->getText().c_str();

               lmt_Ref=lmt;               

              Nvar=prs_arg->var_vtr.find(var_nm);
 
              // Overwrite bram possibly 
              if(Nvar) 
                bram=Nvar->flg_mem;

               
              // Deal with RAM variables
              if(bram) {

                  
                 if(Nvar && Nvar->flg_stt==1){
                    var_sct *var_ini;
                    var_ini=prs_arg->ncap_var_init(vid->getText(),true);       
                    Nvar->var->val.vp=var_ini->val.vp;
                    var_ini->val.vp=(void*)NULL;
                    var_ini=nco_var_free(var_ini);
                    Nvar->flg_stt=2; 
                 }

                 if(Nvar && Nvar->flg_stt==2)
                    var_lhs=Nvar->var;    
                   
                 
                 if(!Nvar)
                    var_lhs=prs_arg->ncap_var_init(vid->getText(),true);       
                 
                    
                  nbr_dmn=var_lhs->nbr_dim;

                  // Now populate lmt_vtr;
                  if( lmt_mk(lmt_Ref,lmt_vtr) == 0)
                    err_prn(fnc_nm,"Invalid hyperslab limits for variable "+ vid->getText());
                  

                 if( lmt_vtr.size() != nbr_dmn)
                    err_prn(fnc_nm,"Number of hyperslab limits for variable "+ vid->getText()+" doesn't match number of dimensions");
                 

                 // add dim names to dimension list 
                 for(idx=0 ; idx < nbr_dmn;idx++) 
                    lmt_vtr[idx]->nm=strdup(var_lhs->dim[idx]->nm);   
        
                
                 slb_sz=1;        
                // fill out limit structure
                for(idx=0 ; idx < nbr_dmn ;idx++){
                   (void)nco_lmt_evl(prs_arg->out_id,lmt_vtr[idx],0L,prs_arg->FORTRAN_IDX_CNV);
                    // Calculate size
                   slb_sz *= lmt_vtr[idx]->cnt;
                }
                 // Calculate RHS variable                  
                 var_rhs=out(vid->getNextSibling());         
                 // Convert to LHS type
                 var_rhs=nco_var_cnf_typ(var_lhs->type,var_rhs);             
               
                 // deal with scalar on RHS first         
                 if(var_rhs->sz == 1)
                   (void)ncap_att_stretch(var_rhs,slb_sz);


                 // make sure var_lhs and var_rhs are the same size
                 // and that they are the same shape (ie they conform!!)          
                 if(var_rhs->sz != slb_sz){
                   err_prn(fnc_nm, "Hyperslab for "+vid->getText()+" - number of elements on LHS(" +nbr2sng(slb_sz) +  ") doesn't equal number of elements on RHS(" +nbr2sng(var_rhs->sz) +  ")");                                       
                 }

                (void)nco_put_var_mem(var_rhs,var_lhs,lmt_vtr);
                if(Nvar==NULL)
                   (void)prs_arg->ncap_var_write(var_lhs,true); 
             


              // deal with Regular Vars
              } else {                 


              // if var undefined in O or defined but not populated
               if(!Nvar || ( Nvar && Nvar->flg_stt==1)){              
                  // if var isn't in ouptut then copy it there
                 //rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
                 var_lhs=prs_arg->ncap_var_init(vid->getText(),true);

                 // copy atts to output
                 (void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
                 (void)prs_arg->ncap_var_write(var_lhs,false);
               }
 
               // Get "new" var_id   
               (void)nco_inq_varid(prs_arg->out_id,var_nm,&var_id);

               var_lhs=prs_arg->ncap_var_init(vid->getText(),false);

               nbr_dmn=var_lhs->nbr_dim;

               // Now populate lmt_vtr;
               if( lmt_mk(lmt_Ref,lmt_vtr) == 0)
                  err_prn(fnc_nm,"Invalid hyperslab limits for variable "+ vid->getText());
               

               if( lmt_vtr.size() != nbr_dmn)
                  err_prn(fnc_nm,"Number of hyperslab limits for variable "+ vid->getText()+" doesn't match number of dimensions");
               

                // add dim names to dimension list 
               for(idx=0 ; idx < nbr_dmn;idx++) 
                   lmt_vtr[idx]->nm=strdup(var_lhs->dim[idx]->nm);   
        
                
                var_lhs->sz=1;        
                // fill out limit structure
                for(idx=0 ; idx < nbr_dmn ;idx++){
                   (void)nco_lmt_evl(prs_arg->out_id,lmt_vtr[idx],0L,prs_arg->FORTRAN_IDX_CNV);
                    // Calculate size
                   var_lhs->sz *= lmt_vtr[idx]->cnt;
                }
               // Calculate RHS variable                  
               var_rhs=out(vid->getNextSibling());         
               // Convert to LHS type
               var_rhs=nco_var_cnf_typ(var_lhs->type,var_rhs);             
               
               // deal with scalar on RHS first         
               if(var_rhs->sz == 1){
                 // stretch variable to var_lhs->sz                 
                 (void)ncap_att_stretch(var_rhs,var_lhs->sz);
                }

               // make sure var_lhs and var_rhs are the same size
               // and that they are the same shape (ie they conform!!)          
               if(var_rhs->sz != var_lhs->sz){
                 err_prn(fnc_nm, "Hyperslab for "+vid->getText()+" - number of elements on LHS(" +nbr2sng(var_lhs->sz) +  ") doesn't equal number of elements on RHS(" +nbr2sng(var_rhs->sz) +  ")");                                       
                 }

              //put block              
              { 
               long mult_srd=1L;
               std::vector<long> dmn_srt_vtr;
               std::vector<long> dmn_cnt_vtr;
               std::vector<long> dmn_srd_vtr;
    
               for(idx=0;idx<nbr_dmn;idx++){
                 dmn_srt_vtr.push_back(lmt_vtr[idx]->srt);
                 dmn_cnt_vtr.push_back(lmt_vtr[idx]->cnt);
                 dmn_srd_vtr.push_back(lmt_vtr[idx]->srd);  
                 mult_srd*=lmt_vtr[idx]->srd;
               } /* end loop over idx */
    
               /* Check for stride */
               if(mult_srd == 1L)
	            (void)nco_put_vara(prs_arg->out_id,var_id,&dmn_srt_vtr[0],&dmn_cnt_vtr[0],var_rhs->val.vp,var_rhs->type);
               else
	            (void)nco_put_vars(prs_arg->out_id,var_id,&dmn_srt_vtr[0],&dmn_cnt_vtr[0],&dmn_srd_vtr[0],var_rhs->val.vp,var_rhs->type);
               
              } // end put block !!

                 
              var_lhs=nco_var_free(var_lhs);

             } // end else


              var_rhs=nco_var_free(var_rhs);
              
              // See If we have to return something
              if(lmt->getNextSibling() && lmt->getNextSibling()->getType()==NORET)
                var=NULL_CEWI;
              else 
                var=prs_arg->ncap_var_init(vid->getText(),true);

               
               // Empty and free vector 
              for(idx=0 ; idx < nbr_dmn ; idx++)
                (void)nco_lmt_free(lmt_vtr[idx]);

        } // end action


        // Deal with LHS casting 
        | (#(VAR_ID DMN_LIST ))=> #(vid1:VAR_ID dmn:DMN_LIST){   

              var_sct *var1;
              std::vector<std::string> str_vtr;
              RefAST  aRef;
              
              
              if(dbg_lvl_get() > 0)
                dbg_prn(fnc_nm,vid1->getText()+"[dims]");


              // set class wide variables
              bcst=true;  
              var_cst=NULL_CEWI;


              //aRef=vid->getFirstChild()->getFirstChild();
              aRef=dmn->getFirstChild();
         
              // pPut dimension names in vector       
              while(aRef) {
                str_vtr.push_back(aRef->getText());
                aRef=aRef->getNextSibling();      
              }

              // Cast is applied in VAR_ID action in function out()
              var_cst=ncap_cst_mk(str_vtr,prs_arg);     
              var1=out(vid1->getNextSibling());

              (void)nco_free(var_cst->val.vp);
              var_cst->val.vp=(void*)NULL;

              bool br1=(var_cst->sz >1 && var1->sz==1);
              bool br2=(var_cst->sz==var1->sz &&  ( ncap_var_is_att(var1) ||var1->has_dpl_dmn==-1 ));
              
               
              if( br1 || br2){
                var_sct *var_nw;
                void *vp_swp;

                var_nw=nco_var_dpl(var_cst);
                var_nw=nco_var_cnf_typ(var1->type,var_nw);
                if(br1)
                   (void)ncap_att_stretch(var1,var_nw->sz);
                 
                vp_swp=var_nw->val.vp;
                var_nw->val.vp=var1->val.vp;
                var1->val.vp=vp_swp;

                var1=nco_var_free(var1);
                var1=var_nw;
             }
               
               //blow out if vars not the same size      
             if(var1->sz != var_cst->sz) 
                  err_prn(fnc_nm, "LHS cast for "+vid1->getText()+" - cannot make RHS "+ std::string(var1->nm) + " conform.");               
     
              var1->nm=(char*)nco_free(var1->nm);

              var1->nm =strdup(vid1->getText().c_str());



              // See If we have to return something
              if(dmn->getNextSibling() && dmn->getNextSibling()->getType()==NORET)
                var=NULL_CEWI;
              else 
                var=nco_var_dpl(var1);               ;


              
              //call to nco_var_get() in ncap_var_init() uses this property
              var1->typ_dsk=var1->type;
              (void)prs_arg->ncap_var_write(var1,bram);

              bcst=false;
              var_cst=nco_var_free(var_cst); 


          } // end action
           
          | vid2:VAR_ID {   
               // Set class wide variables
               var_sct *var1;
               NcapVar *Nvar;
                      

              if(dbg_lvl_get() > 0)
                dbg_prn(fnc_nm,vid2->getText());


               
               bcst=false;
               var_cst=NULL_CEWI; 
              
              
               var1=out(vid2->getNextSibling());
               
               // Save name 
               std::string s_var_rhs(var1->nm);
               (void)nco_free(var1->nm);                
               var1->nm =strdup(vid2->getText().c_str());

               // Do attribute propagation only if
               // var doesn't already exist or is defined but NOT
               // populated
               Nvar=prs_arg->var_vtr.find(vid2->getText());
               //rcd=nco_inq_varid_flg(prs_arg->out_id,var1->nm ,&var_id);

              
               if(!Nvar || Nvar && Nvar->flg_stt==1)
                 (void)ncap_att_cpy(vid2->getText(),s_var_rhs,prs_arg);
                 
                           
 
               // See If we have to return something
               if(vid2->getFirstChild() && vid2->getFirstChild()->getType()==NORET)
                 var=NULL_CEWI;
               else 
                 var=nco_var_dpl(var1);               ;


               // Write var to disk
               (void)prs_arg->ncap_var_write(var1,bram);
               //(void)ncap_var_write_omp(var1,bram,prs_arg);

                         
       } // end action
 
   |   (#(ATT_ID LMT_LIST)) => #(att:ATT_ID LMT_LIST){
        ;
        } 
   |   (#(ATT_ID LMT_DMN))=> #(att1:ATT_ID DMN_LIST){
        ;
        } 
   |  att2:ATT_ID {
       
            var_sct *var1;
            string sa=att2->getText();

            if(dbg_lvl_get() > 0)
              dbg_prn(fnc_nm,sa);
 
            var1=out(att2->getNextSibling());
            (void)nco_free(var1->nm);
            var1->nm=strdup(sa.c_str());

            //var_nw=nco_var_dpl(var);
            NcapVar *Nvar=new NcapVar(var1,sa);
            prs_arg->var_vtr.push_ow(Nvar);       


               // See If we have to return something
            if(att2->getFirstChild() && att2->getFirstChild()->getType()==NORET)
              var=NULL_CEWI;
            else 
              var=nco_var_dpl(var1);               ;
                  
       } // end action
   ;
               

out returns [var_sct *var]
{
    const std::string fnc_nm("out"); 
	var_sct *var1;
    var_sct *var2;
    var=NULL_CEWI;
}  
    // arithmetic operators 

    :  (#(PLUS out out)) =>  #( PLUS  var1=out var2=out)  
           { var=ncap_var_var_op(var1,var2, PLUS );}
	|  (#(MINUS out out)) => #( MINUS var1=out var2=out)
            { var=ncap_var_var_op(var1,var2, MINUS );}
    |  (#(UTIMES #(POST_INC out)))=> #( UTIMES #(POST_INC var1=out_asn)){
             var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,true,prs_arg);      
       } 
    |  (#(UTIMES #(POST_DEC out)))=> #( UTIMES #(POST_DEC var1=out_asn)){
             var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,true,prs_arg);      
       } 
    |  (#(UTIMES #(INC out)))=> #( UTIMES #(INC var1=out_asn)){
             var=ncap_var_var_inc(var1,NULL_CEWI,INC,true,prs_arg);      
       } 
    |  (#(UTIMES #(DEC out)))=> #( UTIMES #(DEC var1=out_asn)){
             var=ncap_var_var_inc(var1,NULL_CEWI,DEC,true,prs_arg);      
       } 

	|	#(TIMES var1=out var2=out)
            { var=ncap_var_var_op(var1,var2, TIMES );}	

	|	#(DIVIDE var1=out var2=out)	
            { var=ncap_var_var_op(var1,var2, DIVIDE );}
	|	#(MOD var1=out var2=out)
	         {var=ncap_var_var_op(var1,var2, MOD);}
    |   #(CARET var1=out var2=out)
            {var=ncap_var_var_op(var1,var2, CARET);}

    //unary Operators
    |   #(LNOT var1=out )      
            { var=ncap_var_var_op(var1,NULL_CEWI, LNOT );}
	|   #(MINUS var1=out )   
            { var=ncap_var_var_op(var1,NULL_CEWI, MINUS );}
    |   #(PLUS var1=out ) // do nothing   

    |   #(INC var1=out_asn )      
            { var=ncap_var_var_inc(var1,NULL_CEWI,INC,false,prs_arg);}
    |   #(DEC var1=out_asn )      
            { var=ncap_var_var_inc(var1,NULL_CEWI, DEC,false,prs_arg );}
    
    |   #(POST_INC var1=out_asn ){
            var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,false,prs_arg);
        }
    |   #(POST_DEC var1=out_asn ){      
            var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,false,prs_arg);
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

    // Assign Operators 
    | #(PLUS_ASSIGN pls_asn:. var2=out) {
       var1=out_asn(pls_asn);
       var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN ,(pls_asn->getType()==UTIMES), prs_arg);
       }
	| #(MINUS_ASSIGN min_asn:. var2=out){
       var1=out_asn(min_asn);
       var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN ,(min_asn->getType()==UTIMES), prs_arg);
       }
	| #(TIMES_ASSIGN tim_asn:. var2=out){       
       var1=out_asn(tim_asn);
       var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN ,(tim_asn->getType()==UTIMES), prs_arg);
       }
	| #(DIVIDE_ASSIGN div_asn:. var2=out){	
       var1=out_asn(div_asn);
       var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN ,(div_asn->getType()==UTIMES), prs_arg);
       }

    | #(ASSIGN asn:. ) 
            {
              // Check for RAM variable - if present 
              // change tree - for example from:
              //     ( EXPR ( = ( * n1 ) ( + four four ) ) )
              // to  ( EXPR ( = n1 ( + four four ) ) )
             RefAST tr;
             bool bram;
             NcapVar *Nvar;

             if(asn->getType()==UTIMES){
               tr=asn->getFirstChild();
               tr->setNextSibling(asn->getNextSibling());
               bram=true;
             } else { 
               tr=asn; 
               bram=false;
             }
             
             // Die if attempting to create a RAM var 
             // from an existing disk var   
             Nvar= prs_arg->var_vtr.find(tr->getText());

             if(bram && tr->getType()==VAR_ID && Nvar && Nvar->flg_mem==false){
              std::string serr;
              serr= "It is impossible to recast disk variable: \"" + tr->getText() +"\" as a RAM variable.";
              err_prn(fnc_nm,serr );       
              }                



             if(prs_arg->ntl_scn)
               var=assign_ntl(tr,bram); 
             else
               var=assign(tr,bram);
               
            }  

    //ternary Operator
    |   #(QUESTION var1=out qus:.) {
           bool br;
            
           // if initial scan 
           if(prs_arg->ntl_scn){
               var=ncap_var_udf("~question"); 
           } else {

             br=ncap_var_lgcl(var1);
             if(br) 
               var=out(qus);
             else
               var=out(qus->getNextSibling());      
           }   
           var1=nco_var_free(var1);
    } 
           

    // Functions 
    |  #(m:FUNC args:FUNC_ARG) {
          RefAST tr;
          std::string sfnm(m->getText());
          std::vector<fmc_cls>::iterator we=std::lower_bound(prs_arg->fmc_vtr.begin(),prs_arg->fmc_vtr.end(),fmc_cls(sfnm));
          // see if string found
          if( we->fnm() == sfnm){
            //Call function
            var=we->vfnc()->fnd(tr ,args, *we,*this); 
          } else { 
              cout << "Function  " << sfnm << " not found" << endl;
              exit(1);
          }
     }

    // Deal with methods 
    | #(DOT mtd:. mfnc:FUNC  margs:FUNC_ARG ){
          std::string sfnm(mfnc->getText());
          std::vector<fmc_cls>::iterator we=std::lower_bound(prs_arg->fmc_vtr.begin(),prs_arg->fmc_vtr.end(),fmc_cls(sfnm));
          // see if string found
          if( we->fnm() == sfnm){
            //Call function
            var=we->vfnc()->fnd(mtd ,margs, *we,*this); 
          } else { 
              cout << "Method  " << sfnm << " not found" << endl;
              exit(1);
          }
     }

    |   dval:DIM_ID_SIZE
        {
            string sDim=dval->getText();
            dmn_sct *dmn_fd;
            
            // Check output
            if(prs_arg->ntl_scn){
                var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),(nc_type)NC_INT,false);
            }else{ 
                // Check output 
                dmn_fd=prs_arg->dmn_out_vtr.find(sDim);
                // Check input
                if(dmn_fd==NULL_CEWI)
                dmn_fd=prs_arg->dmn_in_vtr.find(sDim);
                
                if( dmn_fd==NULL_CEWI ){
                    err_prn(fnc_nm,"Unable to locate dimension " +dval->getText()+ " in input or output files ");
                }
                var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),dmn_fd->sz);
            } // end else 
        } // end action 
        
     // Variable with argument list 
    | (#(VAR_ID LMT_LIST)) => #( vid:VAR_ID LMT_LIST) {
            var=var_lmt(vid);

    }

    // plain Variable
	|   v:VAR_ID       
        { 

          var=prs_arg->ncap_var_init(v->getText(),true);
          if(var== NULL){
               if(prs_arg->ntl_scn){
                 var=ncap_var_udf(v->getText().c_str());
                 return var;
               }else
                 nco_exit(EXIT_FAILURE);
          }

          // apply cast only if sz >1 
          if(bcst && var->sz >1)
            var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);


        } /* end action */

    // PLain attribute
    |   att:ATT_ID { 

            NcapVar *Nvar=NULL;
         
            if(prs_arg->ntl_scn)
              Nvar=prs_arg->int_vtr.find(att->getText());

            if(Nvar==NULL) 
              Nvar=prs_arg->var_vtr.find(att->getText());

            var=NULL_CEWI;    
            if(Nvar !=NULL)
                var=nco_var_dpl(Nvar->var);
            else    
                // Check input file for attribute
                var=ncap_att_init(att->getText(),prs_arg);

            if(!prs_arg->ntl_scn && var==NULL_CEWI ){
                err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files.");
            }
            
            // if att not found return undefined
            if(prs_arg->ntl_scn && var==NULL_CEWI )
                var=ncap_var_udf(att->getText().c_str());
            

            if(prs_arg->ntl_scn && var->val.vp !=NULL)
                var->val.vp=(void*)nco_free(var->val.vp);
              
        }

     // Value list -- stuff values into an attribute
     |   vlst:VALUE_LIST {
          var=value_list(vlst);
         }

    |   str:NSTRING
        {
            char *tsng;
            
            tsng=strdup(str->getText().c_str());
            (void)sng_ascii_trn(tsng);            
            var=(var_sct *)nco_malloc(sizeof(var_sct));
            /* Set defaults */
            (void)var_dfl_set(var); 
            /* Overwrite with attribute expression information */
            var->nm=strdup("~zz@string");
            var->nbr_dim=0;
            var->sz=strlen(tsng);
            var->type=NC_CHAR;
            if(!prs_arg->ntl_scn){
                var->val.vp=(void*)nco_malloc(var->sz*nco_typ_lng(NC_CHAR));
                (void)cast_void_nctype((nc_type)NC_CHAR,&var->val);
                strncpy(var->val.cp,tsng,(size_t)var->sz);  
                (void)cast_nctype_void((nc_type)NC_CHAR,&var->val);
            }
            tsng=(char*)nco_free(tsng);      
        }

        // Naked numbers: Cast is not applied to these numbers
    |   val_float:FLOAT        
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~float"),(nc_type)NC_FLOAT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~float"),static_cast<float>(std::strtod(val_float->getText().c_str(),(char **)NULL)));} // end FLOAT
    |   val_double:DOUBLE        
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~double"),(nc_type)NC_DOUBLE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~double"),strtod(val_double->getText().c_str(),(char **)NULL));} // end DOUBLE
	|	val_int:INT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int"),(nc_type)NC_INT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int"),static_cast<nco_int>(std::strtol(val_int->getText().c_str(),(char **)NULL,10)));} // end INT
	|	val_short:SHORT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~short"),(nc_type)NC_SHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~short"),static_cast<nco_short>(std::strtol(val_short->getText().c_str(),(char **)NULL,10)));} // end SHORT
    |	val_byte:BYTE			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),(nc_type)NC_BYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),static_cast<nco_byte>(std::strtol(val_byte->getText().c_str(),(char **)NULL,10)));} // end BYTE
// fxm TODO nco851: How to add ENABLE_NETCDF4 #ifdefs to ncoGrammer.g?
// Workaround (permanent?) is to add stub netCDF4 forward compatibility prototypes to netCDF3 libnco
// #ifdef ENABLE_NETCDF4
	|	val_ubyte:UBYTE			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),(nc_type)NC_UBYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),static_cast<nco_ubyte>(std::strtoul(val_ubyte->getText().c_str(),(char **)NULL,10)));} // end UBYTE
        // NB: sng2nbr converts "255" into nco_ubtye=2. This is not good.
        //        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),(nc_type)NC_UBYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),sng2nbr(val_ubyte->getText(),nco_ubyte_CEWI));} // end UBYTE
	|	val_ushort:USHORT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),(nc_type)NC_USHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),static_cast<nco_ushort>(std::strtoul(val_ushort->getText().c_str(),(char **)NULL,10)));} // end USHORT
	|	val_uint:UINT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),(nc_type)NC_UINT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),static_cast<nco_uint>(std::strtoul(val_uint->getText().c_str(),(char **)NULL,10)));} // end UINT
	|	val_int64:INT64			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),(nc_type)NC_INT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),sng2nbr(val_int64->getText(),nco_int64_CEWI));} // end INT64
        // std::strtoll() and std::strtoull() are not (yet) ISO C++ standard
        //{if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),(nc_type)NC_INT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),static_cast<nco_int64>(std::strtoll(val_int64->getText().c_str(),(char **)NULL,10)));} // end INT64
	|	val_uint64:UINT64
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),(nc_type)NC_UINT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),sng2nbr(val_uint64->getText(),nco_uint64_CEWI));} // end UINT64
        // std::strtoll() and std::strtoull() are not (yet) ISO C++ standard
        // {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),(nc_type)NC_UINT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),static_cast<nco_uint64>(std::strtoull(val_uint64->getText().c_str(),(char **)NULL,10)));} // end UINT64
// #endif /* !ENABLE_NETCDF4 */

;


// Return a var or att WITHOUT applying a cast 
// and checks that the operand is a valid Lvalue
// ie that the var or att has NO children!!
out_asn returns [var_sct *var]
{
const std::string fnc_nm("assign_asn");
var=NULL_CEWI; 
}

   : #(UTIMES vid1:VAR_ID)
        { 
          if(vid1->getFirstChild())
               err_prn(fnc_nm,"Invalid Lvalue " +vid1->getText() );

          var=prs_arg->ncap_var_init(vid1->getText(),true);
          if(var== NULL_CEWI){
               nco_exit(EXIT_FAILURE);
          }

        }
	|   vid:VAR_ID       
        { 
          if(vid->getFirstChild())
               err_prn(fnc_nm,"Invalid Lvalue " +vid->getText() );

          var=prs_arg->ncap_var_init(vid->getText(),true);
          if(var== NULL_CEWI){
               nco_exit(EXIT_FAILURE);
          }
         


        } /* end action */
    // Plain attribute
    |   att:ATT_ID { 
            // check "output"
            NcapVar *Nvar=NULL;
         
            if(att->getFirstChild())
                err_prn(fnc_nm,"Invalid Lvalue " +att->getText() );


            if(prs_arg->ntl_scn)
              Nvar=prs_arg->int_vtr.find(att->getText());

            if(Nvar==NULL) 
              Nvar=prs_arg->var_vtr.find(att->getText());

            var=NULL_CEWI;    
            if(Nvar !=NULL)
                var=nco_var_dpl(Nvar->var);
            else    
                var=ncap_att_init(att->getText(),prs_arg);


            if(!prs_arg->ntl_scn && var==NULL_CEWI ){
                err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files.");
            }
            
            // if att not found return undefined
            if(prs_arg->ntl_scn && var==NULL_CEWI )
                var=ncap_var_udf(att->getText().c_str());
            

            if(prs_arg->ntl_scn && var->val.vp !=NULL)
                var->val.vp=(void*)nco_free(var->val.vp);


       }// end action    
;


value_list returns [var_sct *var]
{
const std::string fnc_nm("var_lmt");
var=NULL_CEWI; 
}
 :(vlst:VALUE_LIST) {

         char *cp;
         int nbr_lst;
         int idx;
         int tsz;
         

         nc_type type=NC_NAT;
         var_sct *var_ret;                        
         RefAST rRef;
         //NcapVector<var_sct*> exp_vtr;
         std::vector<var_sct*> exp_vtr;
         
         rRef=vlst->getFirstChild();
         //rRef=vlst->getNextSibling();

         /*
         if(prs_arg->ntl_scn){
              var=ncap_var_udf("~zz@value_list");  
              return var;
         }   
         */

         while(rRef){
           exp_vtr.push_back(out(rRef));   
           rRef=rRef->getNextSibling();
         }       
         nbr_lst=exp_vtr.size();
         
         // find highest type
         for(idx=0;idx <nbr_lst ;idx++)
           if(exp_vtr[idx]->type > type) type=exp_vtr[idx]->type;     


         // Inital Scan
         if(prs_arg->ntl_scn){

           //skip loop if highest type is double 
           if(type==NC_DOUBLE) idx=nbr_lst;

           for(idx=0 ; idx <nbr_lst ; idx++) 
            if(exp_vtr[idx]->undefined) break;

           // Exit if an element in the list is "undefined" 
           if(idx < nbr_lst){ 
             var_ret=ncap_var_udf("~zz@value_list");  
             goto end_val;  
           }

           var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
           /* Set defaults */
           (void)var_dfl_set(var_ret); 

           /* Overwrite with attribute expression information */
           var_ret->nm=strdup("~zz@value_list");
           var_ret->nbr_dim=0;
           var_ret->sz=nbr_lst;
           var_ret->type=type;
           
           goto end_val;          

         } // end initial scan

         // convert every element in vector to highest type
         for(idx=0;idx <nbr_lst ;idx++)
            exp_vtr[idx]=nco_var_cnf_typ(type,exp_vtr[idx]);  

         var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
         /* Set defaults */
         (void)var_dfl_set(var_ret); 

         /* Overwrite with attribute expression information */
         var_ret->nm=strdup("~zz@value_list");
         var_ret->nbr_dim=0;
         var_ret->sz=nbr_lst;
         var_ret->type=type;

         tsz=nco_typ_lng(type);
         var_ret->val.vp=(void*)nco_malloc(nbr_lst*tsz);
           
         for(idx=0;idx <nbr_lst ; idx++){
          cp=(char*)(var_ret->val.vp)+ (ptrdiff_t)(idx*tsz);
          memcpy(cp,exp_vtr[idx]->val.vp,tsz);
         }    
         
         // Free vector        
        end_val: for(idx=0 ; idx < nbr_lst ; idx++)
           (void)nco_var_free(exp_vtr[idx]);    

         var=var_ret;

     }

;


//Calculate var with limits
var_lmt returns [var_sct *var]
{
const std::string fnc_nm("var_lmt");
var=NULL_CEWI; 
}
  :#(vid:VAR_ID lmt:LMT_LIST) {
            bool bram;   // Check for a RAM variable
            int idx;
            int nbr_dmn;
            const char *var_nm;
            var_sct *var_rhs;
          var_sct *var_nw=NULL_CEWI;
          var_sct *var1=NULL_CEWI;
          dmn_sct *dmn_nw;
 
          
          NcapVar *Nvar;
          RefAST lRef;           

          NcapVector<lmt_sct*> lmt_vtr;
          NcapVector<dmn_sct*> dmn_vtr;


          //if initial scan return undef
          if(prs_arg->ntl_scn){  
            //var=ncap_var_udf(vid->getText().c_str());       
              var=ncap_var_udf("~rhs_undefined");       
              goto end;  // cannot use return var!!
            
          }

     

          var_nm=vid->getText().c_str(); 
          var_rhs=prs_arg->ncap_var_init(vid->getText(),false);            
          nbr_dmn=var_rhs->nbr_dim;          
          lRef=lmt;

          // Check for RAM variable
          Nvar=prs_arg->var_vtr.find(var_nm);
          if(Nvar && Nvar->flg_mem){ 
             bram=true;
             var_rhs=nco_var_free(var_rhs);
              
             var_rhs=Nvar->cpyVar();
             var_rhs->nc_id=prs_arg->out_id;
           }else{
             bram=false;
           }

          // Now populate lmt_vtr                  
          if( lmt_mk(lRef,lmt_vtr) == 0){
            printf("zero return for lmt_vtr\n");
            nco_exit(EXIT_FAILURE);
          }

         if( lmt_vtr.size() != nbr_dmn){
            err_prn(fnc_nm,"Number of hyperslab limits for variable "+ vid->getText()+" doesn't match number of dimensions");
         }

          // add dim names to dimension list 
          for(idx=0 ; idx < nbr_dmn;idx++)
            lmt_vtr[idx]->nm=strdup(var_rhs->dim[idx]->nm);   
                        
          // fill out limit structure
           for(idx=0 ; idx < nbr_dmn ;idx++)
            (void)nco_lmt_evl(var_rhs->nc_id,lmt_vtr[idx],0L,prs_arg->FORTRAN_IDX_CNV);
           
           // copy lmt_sct to dmn_sct;
           for(idx=0 ;idx <nbr_dmn ; idx++){
              dmn_nw=(dmn_sct*)nco_malloc(sizeof(dmn_sct));
              dmn_nw->nm=strdup(lmt_vtr[idx]->nm);

              // Fudge -if the variable is from input then nco_lmt_evl
              // overwrites the dim id's with their input file values
              // we want the dim ids from output  
              dmn_nw->id=var_rhs->dim[idx]->id;
              //dmn_nw->id=lmt_vtr[idx]->id;
              dmn_nw->cnt=lmt_vtr[idx]->cnt;  
              dmn_nw->srt=lmt_vtr[idx]->srt;  
              dmn_nw->end=lmt_vtr[idx]->end;  
              dmn_nw->srd=lmt_vtr[idx]->srd;  
              dmn_vtr.push_back(dmn_nw);
           }  
 

           if(!bram){
            // Fudge -- fill out var again -but using dims defined in dmn_vtr
            // We need data in var so that LHS logic in assign can access var shape 
            var_nw=nco_var_fll(var_rhs->nc_id,var_rhs->id,var_nm, &dmn_vtr[0],dmn_vtr.size());

            // Now get data from disk - use nco_var_get() 
            (void)nco_var_get(var_nw->nc_id,var_nw); 
           }
            
           // Ram variable -do an in memory get  
           if(bram){

              //Do an in memory get 
              (void)nco_get_var_mem(var_rhs,dmn_vtr);
              var_nw=nco_var_dpl(var_rhs);
           }
                
           
          /* a hack - we set var->has_dpl_dmn=-1 so we know we are dealing with 
             a hyperslabed var and not a regular var  -- It shouldn't cause 
             any abberant behaviour!! */ 

           var_nw->has_dpl_dmn=-1;  

           //if variable is scalar -- re-organize in a  new var - loose extraneous material
           if(var_nw->sz ==1) {
             var1=(var_sct *)nco_malloc(sizeof(var_sct));
             /* Set defaults */
             (void)var_dfl_set(var1); 
             var1->nm=strdup(var_nw->nm);
             var1->nbr_dim=0;
             var1->sz=1;
             var1->type=var_nw->type;

             var1->val.vp=(void*)nco_malloc(nco_typ_lng(var1->type));
             (void)memcpy( (void*)var1->val.vp,var_nw->val.vp,nco_typ_lng(var1->type));
             var_nw=nco_var_free(var_nw);
             var=var1;
            }else{
             var=var_nw;
            }   
            
          /* Casting a hyperslab --this makes my brain  hurt!!! 
          if(bcst && var->sz >1)
            var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);
          */
          
          var_rhs=nco_var_free(var_rhs);

          //free vectors
          for(idx=0 ; idx < nbr_dmn ; idx++){
            (void)nco_lmt_free(lmt_vtr[idx]);
            (void)nco_dmn_free(dmn_vtr[idx]);
          }    
          end: ;

    }
;
