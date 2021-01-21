header {
/* $Header$ */

/* Purpose: ANTLR Grammar and support files for ncap2 */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

    // C Standard Headers
    #include <assert.h>
    #include <ctype.h>
	#include <stdlib.h>
    #include <math.h>
    #if !(defined __xlC__) && !(defined SGIMP64) // C++ compilers that do not allow stdint.h
    #include <stdint.h> // Required by g++ for LLONG_MAX, ULLONG_MAX, by icpc for int64_t    
    #endif // C++ compilers that do not allow stdint.h

    // C++ Standard Headers 
    #include <algorithm>
    #include <iostream>
    #include <sstream>
    #include <fstream>
    #include <string>
  
    // custom exception -used for "exit" function
    #include "ExitException.hpp" 
    // Custom Headers
    #include "prs_cls.hh"
    #include "ncap2_utl.hh"
    #include "ncap2_att.hh"
    #include "fmc_cls.hh"
    #include "NcapVar.hh"
    #include "NcapVarVector.hh"
    #include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()
    #include "NcapVector.hh"
    #include "antlr/TokenStreamSelector.hpp"
    #include "ncoParser.hpp"
    #include "Invoke.hh"

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
    DMN_LIST_P;
    DMN_ARG_LIST;
    LMT_LIST;
    VALUE_LIST;
    FUNC_ARG;
    LMT;
    EXPR;
    FEXPR;
    POST_INC;
    POST_DEC;
    UTIMES;
    PROP; // used to differenciate properties & methods
    FOR2;    
    NORET;
    ATAN2;  //Used indirectly
    WHERE_ASSIGN;
    MISS2ZERO; //used only in VarOp -sets all missing values to zero
    VSORT;     // ascending sort only used outside of grammer to sort in VarOp  
    VRSORT;     // reverse sort only used outside of grammer to sort in VarOp 
    VABS;      // imaginary token used in VarOp to return absolute value 
    VSQR2;     // imaginary token used in VarOp to return square of number 
    DEFDIM;
}
{

public:
   std::vector<std::string> inc_vtr;
}

program:
        (statement)*
    ;

statement:
        // assign/expression_statement
        e1:expr SEMI! { 
          if( ncap_fnc_srh(#e1))  
            #statement = #(#[FEXPR,"FEXPR"],#statement); 
          else
            #statement = #(#[EXPR,"EXPR"],#statement); 
        }
        
        // Define DEFDIM statement 
        // DEFDIMA (regular call - limited or unlimited ) DEFDIM(1)
        // DEFDIMU (explicitly unlimited)                 DEFDIM(0)
        | (def1:DEFDIMA^ |def2:DEFDIMU^  ) LPAREN! NSTRING COMMA! expr (COMMA! expr)?  RPAREN! SEMI! { 
             if( #def1 ){  #def1->setType(DEFDIM);#def1->setText("1");}
             if( #def2 ){  #def2->setType(DEFDIM);#def2->setText("0");}
 
        }
        //exit statement 
        | EXIT^ LPAREN! expr RPAREN! SEMI!
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
        // if statement
        | WHERE^ LPAREN! expr RPAREN! statement 
         ( //standard if-else ambiguity
         options {warnWhenFollowAmbig = false;} 
        : ELSEWHERE! statement )?
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
dmn_list_p:
   LSQUARE! (VAR_ID|DIM_ID)? (COMMA! (VAR_ID|DIM_ID))* RSQUARE!
      { #dmn_list_p = #( [DMN_LIST_P, "dmn_list_p"], #dmn_list_p ); }
    ;

dmn_list:
   LPAREN! (DIM_ID) (COMMA! DIM_ID)* RPAREN!
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

arg_list: expr|dmn_arg_list|DIM_ID|DIM_MTD_ID|call_ref
     ;

func_arg:  LPAREN! (arg_list)? (COMMA! arg_list)*   RPAREN! 
      { #func_arg = #( [FUNC_ARG, "func_arg"], #func_arg ); }
     ;   

hyper_slb: (VAR_ID^ |ATT_ID ^) (lmt_list|dmn_list|dmn_list_p)?
     ;

// call by reference - prefix &
call_ref: CALL_REF^(VAR_ID|ATT_ID)
     ;

/*************************************************************/

top_exp: ur:TIMES^ {#ur->setType(UTIMES);#ur->setText("UTIMES");}  top_exp
             | primary_exp
     ;  
             
meth_exp: top_exp (DOT^ FUNC func_arg)*
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
unary_exp:  ( LNOT^| PLUS^| MINUS^ |INC^ | DEC^ ) unary_exp
            | unaryleft_exp
	;

/*
unary_exp:  ( LNOT^| PLUS^| MINUS^ |INC^ | DEC^ 
             | ur:TIMES^ {#ur->setType(UTIMES);#ur->setText("UTIMES");} ) unary_exp
             | unaryleft_exp
    ;    
*/
// right association
pow_exp: unary_exp (CARET^ pow_exp )? 
    ;

mexpr:
		 pow_exp ( (TIMES^ | DIVIDE^ | MOD^ ) pow_exp)*
	;

add_expr:
		 mexpr ( (PLUS^ | MINUS^ ) mexpr)* 
	;

frel_expr:
        add_expr (( FLTHAN^ | FGTHAN^ ) add_expr)*
    ;


rel_expr:
        frel_expr (( LTHAN^ | GTHAN^ | GEQ^ | LEQ^ ) frel_expr)*
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
        
// 20151021 csz Prefixed some tokens with NCO_ to prevent namespace conflicts with MSVC definitions in windef.h    
primary_exp
    : NCAP_FLOAT
    | NCAP_DOUBLE
    | NCAP_INT
    | NCAP_BYTE
    | NCAP_UBYTE    
    | NCAP_SHORT
    | NCAP_USHORT
    | NCAP_UINT
    | NCAP_INT64
    | NCAP_UINT64
    | NSTRING
    | N4STRING    
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
    charVocabulary = '\u0000'..'\u00FF';
}

tokens {
// token keywords
    IF ="if";
    ELSE="else";
    WHERE="where";
    ELSEWHERE="elsewhere";
    SHIFTL="<<";
    SHIFTR=">>";

    BREAK="break";
    CONTINUE="continue";
    WHILE="while";    
    FOR="for";
   
    DEFDIMA="defdim";
    DEFDIMU="defdimunlim";
    EXIT="exit";
 
    /*


    LIMITED="LIMITED";
    UNLIMITED="UNLIMITED";

    RAM_DELETE="ram_delete";
    RAM_WRITE="ram_write";
    SET_MISS="set_miss";
    CH_MISS="change_miss";
    */
}

{
private:
    prs_cls *prs_arg;
    std::vector<std::string> paths_vtr;      

public:

    // Customized constructor !!
   ncoLexer(ANTLR_USE_NAMESPACE(std)istream& in, prs_cls *prs_in )
   : ANTLR_USE_NAMESPACE(antlr)CharScanner(new ANTLR_USE_NAMESPACE(antlr)CharBuffer(in),true)
   {    
        char *spaths;

        /* a list of include paths delimited by ':' */   
        /* if nco NCO_PATH then NULL */
        spaths=getenv("NCO_PATH");  
        if( spaths &&  strlen(spaths) >0  ) 
          paths_vtr=ncap_make_include_paths(spaths);

        prs_arg=prs_in;
        // This shouldn't really be here 
        // fxm:: should call default constructor
	    initLiterals();
   }

public:
	void uponEOF() /*throws TokenStreamException, CharStreamException*/ {
		if ( selector.getCurrentStream() != lexer ) {
            // Do not allow EOF until main lexer 
            // Force selector to retry for another token
            parser->inc_vtr.pop_back();

            if(nco_dbg_lvl_get() >= 1)
               std::cout<<"Setting parser(filename)=" <<parser->inc_vtr.back()<<std::endl; 

            parser->setFilename(parser->inc_vtr.back());
			selector.pop(); // return to old lexer/stream
			selector.retry();
		}
		// else ANTLR_USE_NAMESPACE(std)cout << "Hit EOF of main file" << ANTLR_USE_NAMESPACE(std)endl;
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

FLTHAN options { paraphrase="<<";} :  "<<" ;

FGTHAN options { paraphrase=">>"; } :  ">>" ;

LAND options { paraphrase="&&";}: "&&" ;

LNOT options { paraphrase="!";} : '!' ;

LOR options { paraphrase="||";}: "||" ;

DOT options {paraphrase="dot operator";} : '.';

CALL_REF options {paraphrase="call by reference";} : '&';



protected DGT:     ('0'..'9');
protected LPH:     ( 'a'..'z' | 'A'..'Z' | '_' );
protected LPHDGT:  ( 'a'..'z' | 'A'..'Z' | '_' | '0'..'9');
protected XPN:     ( 'e' | 'E' ) ( '+' | '-' )? ('0'..'9')+ ;
protected VAR_NM_QT: (LPHDGT|'-'|'+'|'.'|'('|')'|':'|' ' )+  ;      

protected BLASTOUT: .
         {
          // blast out of lexer & parser
          // Can't use RecognitionException() as 
          // this is caught in lexer -- use TokenStreamRecognitionException()
          // instead -- This takes us back to try block in Invoke          

          ostringstream os;
          char ch=LA(0);
          os << getFilename() << " line " << getLine() << ", column "<< getColumn() 
             << ": unexpected character '" << ch << "'" <<std::endl;

          ANTLR_USE_NAMESPACE(antlr)RecognitionException re(os.str());
          throw  ANTLR_USE_NAMESPACE(antlr)TokenStreamRecognitionException(re);
         }  
    ;     
	
UNUSED_OPS: ( "%=" | "^=" | "&=" | "|=" ) {
  
          ostringstream os;
          os << getFilename() << " line " << getLine() << ", column "<< getColumn() 
             << ": unused operator '" << getText() << "'" <<std::endl;

          ANTLR_USE_NAMESPACE(antlr)RecognitionException re(os.str());
          throw  ANTLR_USE_NAMESPACE(antlr)TokenStreamRecognitionException(re);
         }  
    ;    

// Whitespace -- ignored
WS  options {paraphrase="white space"; } 	
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
      '.' (DGT)+ (XPN)? { $setType(NCAP_DOUBLE); }  
      ( ('D'|'d')!     {  $setType(NCAP_DOUBLE);}
       |('F'|'f')!     {  $setType(NCAP_FLOAT);}
       |('L'|'l')!     {  $setType(NCAP_DOUBLE);}
      )?        
    ;

NUMBER:
	(DGT)+	{ $setType(NCAP_INT); }
	( ( '.'  (DGT)* ((XPN)? | ('L'|'l')! )) { $setType(NCAP_DOUBLE); } // 3.14e0, 3.14L, 3.14l
       | (XPN)         { $setType(NCAP_DOUBLE);} // 3e0
       | ('L'|'l')!    { $setType(NCAP_INT);   } // 3l, 3L
       | ('S'|'s')!    { $setType(NCAP_SHORT); } // 3s, 3S
       | ('B'|'b')!    { $setType(NCAP_BYTE);  } // 3b, 3B
       | ("UB"|"ub")!  { $setType(NCAP_UBYTE); } // 3ub, 3UB
       | ("US"|"us")!  { $setType(NCAP_USHORT); } // 3us, 3US
       | ('U'|'u'|"UL"|"ul")!    { $setType(NCAP_UINT); } // 3u, 3U, 3ul, 3UL
       | ("LL"|"ll")!  { $setType(NCAP_INT64); } // 3ll, 3LL
       | ("ULL"|"ull")!  { $setType(NCAP_UINT64); } // 3ull, 3ULL
    )?
    (    ('F'|'f')!    { $setType(NCAP_FLOAT); } // 3F, 3f
       | ('D'|'d')!    { $setType(NCAP_DOUBLE);} // 3D, 3d
    )?        
;




// Return var or att (var_nm@att_nm)
VAR_ATT options {testLiterals=true; paraphrase="variable or function or attribute identifier"; } 
     :  (LPH)(LPH|DGT)*   
            {
             // try to intelligently guess the type to avoid un-necessary function search    
            bool bDoSearch;
            switch( LA(1) ){
               case ' ': 
               case '\t':
               case '(':
                 bDoSearch=true;
                 $setType(VAR_ID); 
                 break;
               case '@':
                 bDoSearch=false;
                 $setType(ATT_ID);
                 break;    
               case '[':
                 bDoSearch=false;
                 $setType(VAR_ID); 
                 break;   
               default: 
                 bDoSearch=false;
                 $setType(VAR_ID);
                 break;
            }  
            if(bDoSearch){   
               string fnc_nm=$getText; 
               std::vector<fmc_cls>::iterator we=std::lower_bound(prs_arg->fmc_vtr.begin(),prs_arg->fmc_vtr.end(),fmc_cls(fnc_nm));   
               if(we!=prs_arg->fmc_vtr.end() && we->fnm()==fnc_nm){
                 int idx=we-prs_arg->fmc_vtr.begin();
                 char buff[10]; 
                 sprintf(buff,"%d",idx);
                 // VERY IMPORTANT - append the index in fmc_vtr to the function name - (name#idx)
                 $setText(fnc_nm+"#"+buff);    
                 $setType(FUNC);
               }             
            } 
           }  
           ('@'(LPH)(LPH|DGT)*  {$setType(ATT_ID); })?
;


// Return a quoted var or att (var_nm@att_nm)
VAR_ATT_QT :( '\''!)
                VAR_NM_QT  {$setType(VAR_ID);}
                ( '@' VAR_NM_QT {$setType(ATT_ID);})?
             ('\''!)
   ;     

// return a STR_ATT_ID
ATT_ID:  '@'(LPH)(LPH|DGT)* {string an=$getText;$setText("global"+an);$setType(ATT_ID);}
;

// Return a quoted STR_ATT_ID
STR_ATT_QT :( '\''!)
              '@' VAR_NM_QT {$setType(STR_ATT_QT);}
             ('\''!)
;     


//Return a quoted dim
DIM_QT: ( '\''!)
           ('$'! VAR_NM_QT {$setType(DIM_ID);})
        ('\''!) 
           ( ".size"! { $setType(DIM_ID_SIZE);})?
   ;

DIM_VAL options { paraphrase="dimension identifier"; } 
        : '$'! (LPH)(LPH|DGT)* 
            {$setType(DIM_ID);}
         ( ".size"!  
            { $setType(DIM_ID_SIZE);}
         )? 
   ;  


// Shorthand for naming dims in method e.g., $0,$1, $2 etc
DIM_MTD_ID 
  options{paraphrase="dimension identifier";} 
  : '$'! (DGT)+
   ;            

NSTRING
  options{paraphrase="a string";} 
  : '"'! ( ~('"'|'\n'))* '"'! 
        {$setType(NSTRING);}
       ('s'! {$setType(N4STRING);} )? 
   ;   

INCLUDE
	:	"#include" (WS)? f:NSTRING
		{
		// ANTLR_USING_NAMESPACE(std)
		// create lexer to handle include
        int idx; 
        int sz=paths_vtr.size(); 
		std::string f_nm=f->getText();

		std::ifstream* input=new std::ifstream(f_nm.c_str());
        // if(*input==NULL){ // 20150413: Trips clang 6.0 MACOSX Yosemite warning from -Wnull-arithmetic and subsequent error "invalid operands to binary expression" 
		if(!(*input)){
          // only search include paths if f_nm NOT an absolute path  
          // add include paths and stop if opened ok  
          if( sz==0 || f_nm[0]=='/')  
              err_prn("Lexer cannot find include file \""+f_nm+"\""); 
             
          for(idx=0;idx<sz;idx++)
          {   
              input=new std::ifstream( (paths_vtr[idx] + f_nm).c_str()); 
              if(*input)
                break;  
          }
          if(idx==sz) 
             err_prn("Lexer cannot find the include file \""+f_nm+ "\" in the locations specified in the env-var \"NCO_PATH\""); 

		}
		ncoLexer* sublexer = new ncoLexer(*input,prs_arg);
		// make sure errors are reported in right file
		sublexer->setFilename(f_nm);
		parser->setFilename(f_nm);
        // save the filename - so we can pop it back later if needed
        // so parser gets the right filename
        parser->inc_vtr.push_back(f_nm);
		// you can't just call nextToken of sublexer
		// because you need a stream of tokens to
		// head to the parser.  The only way is
		// to blast out of this lexer and reenter
		// the nextToken of the sublexer instance
		// of this class.

		selector.push(sublexer);
		// ignore this as whitespace; ask selector to try
		// to get another token.  It will call nextToken()
		// of the new instance of this lexer.
		selector.retry(); // throws TokenStreamRetryException
		}
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

      for(idx=0 ; idx < nbr_dmn ; idx++)
      {
         hyp.ind[0]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[1]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[2]=ANTLR_USE_NAMESPACE(antlr)nullAST;
             
       if(lRef->getType()!=LMT) 
            return 0;
       
       eRef=lRef->getFirstChild();
       nbr_cln=0;
        
       while(eRef) 
       {
          if(eRef->getType() == COLON)
          {
            cRef=eRef;        
            nbr_cln++;
          }
           eRef=eRef->getNextSibling();
        }
      
       // Initialise  to default markers
       switch(nbr_cln) 
       {
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
       while(eRef) 
       {
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

bool
lmt_var_mk(
int nbr_dmn,
RefAST lmt,
NcapVector<lmt_sct*> &lmt_vtr ) 
{
	long idx;
	long jdx;
	long sz;   
    long dmn_sz;     
	var_sct *var;
	std::string fnc_nm="lmt_var_mk"; 
	
	
	// calculate variables
	var=out(lmt->getFirstChild()->getFirstChild());
	// convert result to type int
	var=nco_var_cnf_typ(NC_INT64,var);    
	(void)cast_void_nctype((nc_type)NC_INT64,&var->val);
	sz=var->sz;
	dmn_sz=var->sz / nbr_dmn;
    
    if(nco_dbg_lvl_get() >= nco_dbg_scl)
       dbg_prn("lmt_var_mk","using hyperslab indices from a single var"); 

    // shape of var must be (nbr_dmn) or (nbr_dmn,2) or (nbr_dmn,3) 
    if( dmn_sz * nbr_dmn != sz )
	{
	  var=nco_var_free(var);
      return false;  
    }

    
	
    for(idx=0; idx<sz;idx+=dmn_sz)
	 {

		// fill out lmt structure
		// use same logic as nco_lmt_prs 
		lmt_sct* lmt_ptr=(lmt_sct*)nco_calloc((size_t)1,sizeof(lmt_sct));
		lmt_ptr->nm=NULL;
		//lmt_ptr->lmt_typ=-1;
		lmt_ptr->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
		lmt_ptr->min_sng=NULL;
		lmt_ptr->max_sng=NULL;
		lmt_ptr->srd_sng=NULL;
		lmt_ptr->is_usr_spc_min=False;
		lmt_ptr->is_usr_spc_max=False;
		/* rec_skp_ntl_spf is used for record dimension in multi-file operators */
		lmt_ptr->rec_skp_ntl_spf=0L; /* Number of records skipped in initial superfluous files */

	   
       for(jdx=0;jdx<dmn_sz;jdx++)
		{     
		  
         nco_int64 ival= var->val.i64p[idx+jdx];
         switch(jdx){
           case 0: 
             lmt_ptr->is_usr_spc_min=True;
             lmt_ptr->srt=ival;
             break;
           case 1: //end
             lmt_ptr->is_usr_spc_max=True;
             lmt_ptr->end=ival;             break;
           case 2: //srd
             lmt_ptr->srd_sng=strdup("~fill_in");
             lmt_ptr->srd=ival;
             break;			
	       }

		}
		/* need to deal with situation where only start is defined -- ie picking only a single value */
		if( lmt_ptr->is_usr_spc_min==True && lmt_ptr->is_usr_spc_max==False && lmt_ptr->srd_sng==NULL)
		{
			lmt_ptr->is_usr_spc_max=True;
			lmt_ptr->end=lmt_ptr->srt; 
       }    

       lmt_vtr.push_back(lmt_ptr);		
	}
	 
	cast_nctype_void((nc_type)NC_INT64,&var->val);
	var=nco_var_free(var);  
	
   return true;

}



bool 
lmt_mk(
int nbr_dmn,
RefAST lmt,
NcapVector<lmt_sct*> &lmt_vtr ) 
{   

int idx;
int jdx;
long ldx=0L;
var_sct *var_out;
lmt_sct *lmt_ptr;
RefAST aRef;
vector<ast_lmt_sct> ast_lmt_vtr;

// deal with a single expression containing all indicies
if(lmt->getNumberOfChildren()==1 && 
   lmt->getFirstChild()->getNumberOfChildren()==1 &&
   lmt->getFirstChild()->getFirstChild()->getType() != COLON)
{
   return lmt_var_mk(nbr_dmn,lmt,lmt_vtr);   ;            
}


// populate ast_lmt_vtr
if(nbr_dmn != lmt_init(lmt,ast_lmt_vtr))
    return false;	

for(idx=0;idx<nbr_dmn;idx++){

    // fill out lmt structure
    // use same logic as nco_lmt_prs 
    lmt_ptr=(lmt_sct*)nco_calloc((size_t)1,sizeof(lmt_sct));
    lmt_ptr->nm=NULL;
    //lmt_ptr->lmt_typ=-1;
    lmt_ptr->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
    lmt_ptr->min_sng=NULL;
    lmt_ptr->max_sng=NULL;
    lmt_ptr->srd_sng=NULL;
    lmt_ptr->is_usr_spc_min=False;
    lmt_ptr->is_usr_spc_max=False;
    /* rec_skp_ntl_spf is used for record dimension in multi-file operators */
    lmt_ptr->rec_skp_ntl_spf=0L; /* Number of records skipped in initial superfluous files */

    // for a var limit  with a single index (or var) -(1D or nD) this case is handled by lmt_var_mk
    //  we are here when  there is a single index for one of an nD specification
    // a single limit mean a single index -by the parser this is 
    // guaranteed to be non null  (that is ast_lmt_vtr[idx].ind[0] not null) 
    if( ast_lmt_vtr[idx].ind[1]==ANTLR_USE_NAMESPACE(antlr)nullAST)
    {  
      var_out=out( ast_lmt_vtr[idx].ind[0] );
      // convert result to type int
      var_out=nco_var_cnf_typ(NC_INT64,var_out);    
      (void)cast_void_nctype((nc_type)NC_INT64,&var_out->val);
      // only interested in the first value.
      ldx=var_out->val.i64p[0];
      var_out=nco_var_free(var_out);

      lmt_ptr->is_usr_spc_min=True;
      lmt_ptr->srt=ldx;
            
      lmt_ptr->is_usr_spc_max=True;
      lmt_ptr->end=ldx; 

    } 
    else 

    for(jdx=0 ; jdx <3 ; jdx++)
    {

      aRef=ast_lmt_vtr[idx].ind[jdx];

      if(!aRef || aRef->getType() == COLON)
        continue;

      // Calculate number using out()
      var_out=out(aRef);
      // convert result to type int
      var_out=nco_var_cnf_typ(NC_INT64,var_out);    
      (void)cast_void_nctype((nc_type)NC_INT64,&var_out->val);
         // only interested in the first value.
      ldx=var_out->val.i64p[0];
      var_out=nco_var_free(var_out);

      // switch jdx 0-srt,1-end,2-srd
      switch(jdx)
      {
          //srt
          case 0: 
              lmt_ptr->is_usr_spc_min=True;
              lmt_ptr->srt=ldx;
              break;
          // end
          case 1: 
              lmt_ptr->is_usr_spc_max=True;
              lmt_ptr->end=ldx;
              break;
          //srd 
          case 2: 
              lmt_ptr->srd_sng=strdup("~fill_in");
              lmt_ptr->srd=ldx;         
              break;
      }

    }  

    lmt_vtr.push_back(lmt_ptr);
  } // end idx

   return true;;
} /* end lmt_mk */


    void run_dbl(RefAST tr,int icnt){
     int idx=0;
     RefAST ntr=tr;

     extern int      
     ncap_omp_exe(
     std::vector< std::vector<RefAST> > &all_ast_vtr,
     ncoTree** wlk_ptr,
     int wlk_nbr);

     if(tr== ANTLR_USE_NAMESPACE(antlr)nullAST)
        err_prn("run_dbl"," REPORTS given a null AST Reference\n");
            
     //small list dont bother with double parsing     
     // just do a final parse
     if(icnt <4){
       //Final scan
       prs_arg->ntl_scn=False;
       while(idx++ < icnt){
          (void)statements(ntr);   
          ntr=ntr->getNextSibling();   
       }
       return; 
     }
     
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

    // see if below does anything ? 
    (void)nco_sync(prs_arg->out_id); 

    //Final scan
    if(!prs_arg->NCAP_MPI_SORT)
    { 
      idx=0;
      ntr=tr;
      prs_arg->ntl_scn=False;
      while(idx++ < icnt){
        (void)statements(ntr);   
        ntr=ntr->getNextSibling();   
      }
    }
    else
    {  
    // Sort expressions - MPI preparation
       prs_arg->ntl_scn=False;
      // nb A vector of vectors
      std::vector< std::vector<RefAST> > all_ast_vtr;
      ncoTree **wlk_vtr=(ncoTree**)NULL; 

      // Populate and sort  vector 
      (void)ncap_mpi_srt(tr,icnt,all_ast_vtr,prs_arg);   
      // Evaluate expressions (execute)
      (void)ncap_omp_exe(all_ast_vtr,wlk_vtr,0);  

      /*  
      for(unsigned vtr_idx=0 ; vtr_idx<all_ast_vtr.size(); vtr_idx++)
        for(unsigned jdx=0 ; jdx<all_ast_vtr[vtr_idx].size(); jdx++)
	     (void)statements(all_ast_vtr[vtr_idx][jdx]);
      */
    }
    return; 

    }
 
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
    while((ntr=ntr->getNextSibling())); // 20101027 csz "while result of assignment is true" (not an equality comparison) 
    
    if(nbr_stmt <4 || nbr_dpt>0 ){
        prs_arg->ntl_scn=False;
        ntr=tr;
        do{ 
          iret=statements(ntr);
          // break if jump statement   
          if(iret==BREAK || iret==CONTINUE) 
           break; 
        } while((ntr=ntr->getNextSibling())); // 20101027 csz "while result of assignment is true" (not an equality comparison) 
        goto exit;
    }
  
    ntr=tr;

    for(idx=0 ; idx < nbr_stmt; idx++){
      ntyp=ntr->getType();
      // we have hit an IF or a basic block
      if(ntyp==BLOCK || ntyp==IF ||ntyp==DEFDIM || ntyp==WHILE ||ntyp==FOR || ntyp==FEXPR ||ntyp==WHERE || ntyp==EXIT) {
      //  if(ntyp != EXPR ){ 
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

RefAST nco_dupList(RefAST tr){
      RefAST otr;  
      // nb astFactory is protected- must call from within class
      otr=astFactory->dupList(tr);      
      //otr->setNextSibling( ANTLR_USE_NAMESPACE(antlr)nullAST ) ;
      /*  
      if(otr->getNextSibling()!= ANTLR_USE_NAMESPACE(antlr)ASTNULL )     
        err_prn("nco_dupList", "NON NULL AST SIBLING\n");
      */ 
      return otr; 
     }
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
var_sct *var=NULL;
var_sct *var2=NULL;
const std::string fnc_nm("statements");
// list of while/for loops entered n.b depth is lpp_vtr.size()
// Temporary fix so call run_exe only does a single parse in the
// nested block
static std::vector<std::string> lpp_vtr;
}
    : blk:BLOCK { 
       //std::cout <<"Num of Children in block="<<blk->getNumberOfChildren()<<endl;
       iret=run_exe(blk->getFirstChild(),lpp_vtr.size() );
            
                }

    | #(exp:EXPR ass:.) 
     {
       int aType; 
       
       aType=ass->getType(); 

     
      
      if( aType==ASSIGN || aType==POST_INC || aType==POST_DEC || aType==INC || aType==DEC||aType==PLUS_ASSIGN || aType==MINUS_ASSIGN || aType==TIMES_ASSIGN || aType==DIVIDE_ASSIGN )
            ass->setText("NO_RET"); 

       var=out(exp->getFirstChild());

       if(var != (var_sct*)NULL)
         var=nco_var_free(var);
       iret=EXPR;
    }

    // These expressions excute in their own basic blocks
    // Any expressions  which use the utility functions
    // are flagged as FEXPR by the parser.
    | #(FEXPR fss:.) {

       var=out(fss);
       if(var != (var_sct*)NULL)
         var=nco_var_free(var);
       iret=FEXPR;

       dbg_prn(fnc_nm,"executed FEXPR "+ fss->getText());    
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
         if(stmt->getType()==BLOCK ) {
           if(stmt->getFirstChild()) 
             iret=run_exe(stmt->getFirstChild(),lpp_vtr.size());
         }else
           iret=statements(stmt);     
      }

      // See if else stmt exists (3rd sibling)       
	  if(!br && (stmt=stmt->getNextSibling()) ){
         if(stmt->getType()==BLOCK ){
           if(stmt->getFirstChild()) 
           iret=run_exe(stmt->getFirstChild(),lpp_vtr.size());
         }else
           iret=statements(stmt);     
      }
 
      var=NULL_CEWI;
      
    }// end action

    | #(WHERE var=out stmt3:.) { 
      // convert mask to short 
      RefAST tr; 
      var=nco_var_cnf_typ(NC_SHORT,var);

      //change missing values to zero
      if(var->has_mss_val){
       var=ncap_var_var_stc(var,NULL_CEWI,MISS2ZERO);
       var->has_mss_val=False;
       var->mss_val.vp=(void*)nco_free(var->mss_val.vp);
      }         

      //deal with block
      if(stmt3->getType()==BLOCK){
        tr=stmt3->getFirstChild();
        while(tr) {
          (void)where_assign(tr,var);       
          tr=tr->getNextSibling();
        } 
      } else 
        where_assign(stmt3,var);
    
      // deal with else-where
      if((tr=stmt3->getNextSibling())!=ANTLR_USE_NAMESPACE(antlr)nullAST) {

        //invert mask
        var=ncap_var_var_stc(var,NULL_CEWI,LNOT); 

        if(tr->getType()==BLOCK){
          tr=tr->getFirstChild();
          while(tr) {
           (void)where_assign(tr,var);       
           tr=tr->getNextSibling();
          } 
        } else 
        where_assign(tr,var);

      }

      if(var != (var_sct*)NULL)
         var=nco_var_free(var);
      iret=WHERE;      
    }
  
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

    // throw a custom ExitException 
    | #(EXIT var=out ) { 
      int sret;
      ostringstream os;       
      
      // convert to INT   
      nco_var_cnf_typ(NC_INT,var);  
      cast_void_nctype(NC_INT,&var->val);
      sret=var->val.ip[0];
      cast_nctype_void(NC_INT,&var->val);
      var=nco_var_free(var); 
      
      iret=EXIT; 
      os<<sret;
      throw  ANTLR_USE_NAMESPACE(antlr)ExitException(os.str());

    }
    | ELSE { iret=ELSE;}
    | BREAK { iret=BREAK;}
    | CONTINUE {iret=CONTINUE;} 
    | NULL_NODE { iret=NULL_NODE; }

    |#(dtyp:DEFDIM def:NSTRING  var=out (var2=out)? ){

        bool bunlimited=false;      
        int ityp;  
        int dCall;  
        long sz;
         
        // 0 - specific - UNLIMITED    
        // 1 - regular LIMITED or UNLIMITED
        dCall=atoi(dtyp->getText().c_str());    
        iret=DEFDIM;
 
        var=nco_var_cnf_typ((nc_type)NC_INT64,var);
        (void)cast_void_nctype((nc_type)NC_INT64,&var->val);
        sz=var->val.i64p[0];
        var=(var_sct*)nco_var_free(var);

        if( dCall==0 ){ 
          bunlimited =true;
        } else    
        if(dCall==1 && var2){
           // nb var2 is in upper scope - defined at statement action start
           var2=nco_var_cnf_typ(NC_INT,var2);
           (void)cast_void_nctype(NC_INT,&var2->val);
           ityp=var2->val.ip[0]; 
           (void)cast_nctype_void(NC_INT,&var2->val);
           var2=nco_var_free(var2);

           if( ityp==0)
              bunlimited=true;
           else if( ityp==1 )  
              bunlimited=false;
           else    
              err_prn(fnc_nm,"defdim for "+ def->getText() + ". Third argument must be 0 for \"UNLIMITED\" or 1 for \"LIMITED\" or void"); 

        } 

        (void)ncap_def_dim(def->getText(),sz,bunlimited,prs_arg);
      }
    ;

// Parse assign statement - Initial Scan
assign_ntl [bool bram,bool bret] returns [var_sct *var]
{
const std::string fnc_nm("assign_ntl"); 
var=NULL_CEWI;
}
   : (#(VAR_ID LMT_LIST ))=> #(vid:VAR_ID lmt:LMT_LIST){

               std::string var_nm; 
               var_sct *var_lhs;
               var_sct *var_rhs;
               NcapVar *Nvar;              

               var_nm=vid->getText();

               if(nco_dbg_lvl_get() >= nco_dbg_scl) dbg_prn(fnc_nm,var_nm+"(limits)");

              // evaluate rhs for side effects eg new dims or lvalues 
               var_rhs=out(vid->getNextSibling());         
               var_rhs=nco_var_free(var_rhs);               

               var_lhs=prs_arg->ncap_var_init(var_nm,false);
               if(var_lhs)
               {
                 var=nco_var_dpl(var_lhs);
                 (void)prs_arg->ncap_var_write(var_lhs,bram);
               } 
               else 
               {
                 // set var to udf
                 var_lhs=ncap_var_udf(var_nm.c_str());
                 var=nco_var_dpl(var_lhs);
 
                 Nvar=new NcapVar(var_lhs);
                 (void)prs_arg->int_vtr.push_ow(Nvar);
               }
        }                    
 
        | (#(VAR_ID (DMN_LIST|DMN_LIST_P) ))=> #(vid1:VAR_ID dmn:.){   
                               
              int idx;
              std::string var_nm;
              var_sct *var1;
              std::vector<std::string> str_vtr;
              RefAST  aRef;
              NcapVar *Nvar;
              int str_vtr_sz;

              var_nm=vid1->getText();               

              if(nco_dbg_lvl_get() >= nco_dbg_var) dbg_prn(fnc_nm,var_nm+"[dims]");

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
              if( idx <str_vtr_sz)
                 var1=ncap_var_udf(var_nm.c_str());
              else 
              {
                 // Cast is applied in VAR_ID action in function out()
                 var_cst=ncap_cst_mk(str_vtr,prs_arg);
                 var1=out(vid1->getNextSibling());

                 if(!var1->undefined) 
                 {
                   var_cst=nco_var_cnf_typ(var1->type,var_cst);
                   var_cst->typ_dsk=var1->type;
                   var1=nco_var_free(var1); 
                   var1=nco_var_dpl(var_cst);
                 }

              }
              
              if(var1->undefined)
              {
                var1=nco_var_free(var1);   
                var1=ncap_var_udf(var_nm.c_str());
                Nvar=new NcapVar(var1);
                (void)prs_arg->int_vtr.push_ow(Nvar);
                var=nco_var_dpl(var1);
              } 
              else
              {
                var1->nm=(char*)nco_free(var1->nm);
                var1->nm=strdup(var_nm.c_str());
                var=nco_var_dpl(var1);
                prs_arg->ncap_var_write(var1,bram);
             }

              if(var_cst)
                var_cst=nco_var_free(var_cst);
 
              bcst=false;   
            }

          | vid2:VAR_ID {   

              var_sct *var_rhs=NULL_CEWI;
              var_sct *var_lhs=NULL_CEWI;
              var_sct *var_ret=NULL_CEWI;
              std::string var_nm;
              NcapVar *Nvar;

              // Set class wide variables
              bcst=false;
              var_cst=NULL_CEWI;

              var_nm=vid2->getText();

              if(nco_dbg_lvl_get() >= nco_dbg_var) dbg_prn(fnc_nm,var_nm);

              // get shape from RHS
              var_rhs=out(vid2->getNextSibling());

              // deal with special case where RHS is a NC_CHAR
              if(var_rhs->type==NC_CHAR)
              {

                Nvar=prs_arg->int_vtr.find(var_nm);
                if(!Nvar)
                   Nvar=prs_arg->var_vtr.find(var_nm);

                // var is unread, is it in Input ?
                if(!Nvar && prs_arg->ncap_var_init_chk(var_nm)){
                    var_lhs=prs_arg->ncap_var_init(var_nm,false);
                    if(var_lhs->type !=NC_CHAR)
                        var_lhs=nco_var_free(var_lhs);
                }
               }

               if(var_lhs)
               {
                 var_rhs=(var_sct*)nco_var_free(var_rhs);
                 var_ret=var_lhs;

               }
               else if(var_rhs)
               {
                  (void)nco_free(var_rhs->nm);
                  var_rhs->nm =strdup(var_nm.c_str());
                  var_ret=var_rhs;
                }

                //Copy return variable
                if(bret)
                  var=nco_var_dpl(var_ret);
                else
                   var=(var_sct*)NULL;

                (void)prs_arg->ncap_var_write(var_ret,bram);



        } // end action
       
   |   (#(ATT_ID LMT_LIST))=> #(att:ATT_ID LMT_LIST){

            //In Initial scan all newly defined atts are flagged as Undefined
            var_sct *var1;
            NcapVar *Nvar;

            if(nco_dbg_lvl_get() > nco_dbg_var) dbg_prn(fnc_nm,att->getText());
            
            var1=ncap_var_udf(att->getText().c_str());

            Nvar=new NcapVar(var1);
            prs_arg->int_vtr.push_ow(Nvar);          

            // Copy return variable
            if(bret)
              var=nco_var_dpl(var1);    
            else
              var=(var_sct*)NULL; 

       } //end action

   |   (#(ATT_ID LMT_DMN))=>  #(att1:ATT_ID DMN_LIST){
        ;
        } 
   | att2:ATT_ID {
       
        //In Initial scan all newly defined atts are flagged as Undefined
        var_sct *var1;
        NcapVar *Nvar;

        if(nco_dbg_lvl_get() > nco_dbg_var) dbg_prn(fnc_nm,att2->getText());
      
        var1=ncap_var_udf(att2->getText().c_str());

        Nvar=new NcapVar(var1);
        prs_arg->int_vtr.push_ow(Nvar);          

        // Copy return variable
        if(bret)
          var=nco_var_dpl(var1);    
        else
          var=(var_sct*)NULL;    

    } //end action

    ; // end assign block

assign [bool bram,bool bret] returns [var_sct *var]
{
const std::string fnc_nm("assign"); 
var=NULL_CEWI;
}

   :   (#(VAR_ID LMT_LIST ))=> #(vid:VAR_ID lmt:LMT_LIST){

               int idx;
               int nbr_dmn;
               int var_id; 
               int slb_sz;
               std::string var_nm;

               RefAST lmt_Ref;
               var_sct *var_lhs=NULL_CEWI;
               var_sct *var_rhs=NULL_CEWI;
               NcapVector<lmt_sct*> lmt_vtr;          
               
               lmt_Ref=vid->getFirstChild();

               bcst=false;
               var_cst=NULL_CEWI;
               var=NULL_CEWI;
               NcapVar *Nvar; 
              
               var_nm=vid->getText();
               
               if(nco_dbg_lvl_get() >= nco_dbg_var) dbg_prn(fnc_nm,var_nm+"(limits)");

               // check to see if we are dealing with a single
               // index in limit -- i.e hyperslab a mult-dimensional var
               // with a single index 
              
               if(lmt->getNumberOfChildren()==1 &&
                  lmt->getFirstChild()->getNumberOfChildren()==1 &&
                  lmt->getFirstChild()->getFirstChild()->getType() != COLON
                 ){
                  var=var_lmt_one_lhs(vid,bram);
                  goto end0;
                } 
              
               lmt_Ref=lmt;               

              Nvar=prs_arg->var_vtr.find(var_nm);
 
              // Overwrite bram possibly 
              if(Nvar) 
                bram=Nvar->flg_mem;
               
              // Deal with RAM variables
              if(bram) {
                  
                 if(Nvar && Nvar->flg_stt==1){
                    var_sct *var_ini;
                    var_ini=prs_arg->ncap_var_init(var_nm,true);
                    Nvar->var->val.vp=var_ini->val.vp;
                    var_ini->val.vp=(void*)NULL;
                    var_ini=nco_var_free(var_ini);
                    Nvar->flg_stt=2;
                 }

                 if(Nvar && Nvar->flg_stt==2)
                    var_lhs=Nvar->var;    
                   
                 if(!Nvar)
                    var_lhs=prs_arg->ncap_var_init(var_nm,true);       
                    
                  nbr_dmn=var_lhs->nbr_dim;

                  // Now populate lmt_vtr;
                  if( lmt_mk(nbr_dmn, lmt_Ref,lmt_vtr) == false)
                    err_prn(fnc_nm,"Invalid hyperslab limits for variable "+ var_nm);
                  
                 if( lmt_vtr.size() != nbr_dmn)
                    err_prn(fnc_nm,"Number of hyperslab limits for variable "+ var_nm+" doesn't match number of dimensions");

                 // add dim names to dimension list 
                 for(idx=0 ; idx < nbr_dmn;idx++) 
                    lmt_vtr[idx]->nm=strdup(var_lhs->dim[idx]->nm);   
                
                 slb_sz=1;        
                // fill out limit structure
                for(idx=0 ; idx < nbr_dmn ;idx++){
                   (void)ncap_lmt_evl(prs_arg->out_id,lmt_vtr[idx],-1,prs_arg);
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
                   err_prn(fnc_nm, "Hyperslab for \""+var_nm+"\": LHS number of elements = "+nbr2sng(slb_sz)+" does not equal RHS number of elements = "+nbr2sng(var_rhs->sz));
                 }
                    
                (void)nco_put_var_mem(var_rhs,var_lhs,lmt_vtr);
                if(Nvar==NULL)
                   (void)prs_arg->ncap_var_write(var_lhs,true); 

              // deal with Regular Vars
              }else{                 

              // if var undefined in O or defined but not populated
               if(!Nvar || ( Nvar && Nvar->flg_stt==1)){              
                  // if var isn't in ouptut then copy it there
                 //rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
                 
                 var_lhs=prs_arg->ncap_var_init(var_nm,true);

                 // copy atts to output
                 (void)ncap_att_cpy(var_nm,var_nm,prs_arg);
                 (void)prs_arg->ncap_var_write(var_lhs,false);

                 if(nco_dbg_lvl_get() >= nco_dbg_std) nfo_prn(fnc_nm,"Variable \""+var_nm+"\" being read and written in ASSIGN"); 
               }
 
               // Get "new" var_id   
               (void)nco_inq_varid(prs_arg->out_id,var_nm.c_str(),&var_id);

               var_lhs=prs_arg->ncap_var_init(var_nm,false);

               nbr_dmn=var_lhs->nbr_dim;

               // Now populate lmt_vtr;
               if( lmt_mk(nbr_dmn, lmt_Ref,lmt_vtr) == false)
                  err_prn(fnc_nm,"Invalid hyperslab limits for variable "+ var_nm);
               
               if( lmt_vtr.size() != nbr_dmn)
                  err_prn(fnc_nm,"Number of hyperslab limits for variable "+ var_nm+" doesn't match number of dimensions");

                // add dim names to dimension list 
               for(idx=0 ; idx < nbr_dmn;idx++) 
                   lmt_vtr[idx]->nm=strdup(var_lhs->dim[idx]->nm);   
                
                var_lhs->sz=1;        
                // fill out limit structure
                for(idx=0 ; idx < nbr_dmn ;idx++){
                   (void)ncap_lmt_evl(prs_arg->out_id,lmt_vtr[idx],-1,prs_arg);
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
                 err_prn(fnc_nm, "Hyperslab for "+var_nm+" - number of elements on LHS(" +nbr2sng(var_lhs->sz) +  ") doesn't equal number of elements on RHS(" +nbr2sng(var_rhs->sz) +  ")");                                       
                 }
                
              // swap values about
              var_lhs->val.vp=var_rhs->val.vp; 
              var_rhs->val.vp=(void*)NULL; 

              //write block              
              { 
               for(idx=0;idx<nbr_dmn;idx++){
                 var_lhs->srt[idx]=lmt_vtr[idx]->srt; 
                 var_lhs->cnt[idx]=lmt_vtr[idx]->cnt; 
                 var_lhs->srd[idx]=lmt_vtr[idx]->srd; 
               } /* end loop over idx */
    

               // write slab to O contains call to Open MP critical region
               // routine also frees up calling var    
               (void)prs_arg->ncap_var_write_slb(var_lhs);     
                 

              } // end put block !!

              } // end else if regular var

             var_rhs=nco_var_free(var_rhs);
              
               // Empty and free vector 
              for(idx=0 ; idx < nbr_dmn ; idx++)
                (void)nco_lmt_free(lmt_vtr[idx]);

              // See If we have to return something
end0:         if(bret)
                var=prs_arg->ncap_var_init(var_nm,true);
              else 
                var=NULL_CEWI;

               
    } // end action

        // Deal with LHS casting 
        | (#(VAR_ID (DMN_LIST|DMN_LIST_P) ))=> #(vid1:VAR_ID dmn:.){   

              var_sct *var1;
              std::vector<std::string> str_vtr;
              RefAST  aRef;
              std::string var_nm;              

              var_nm=vid1->getText();
              
              if(nco_dbg_lvl_get() >= nco_dbg_var) dbg_prn(fnc_nm,var_nm+"[dims]");

              // set class wide variables
              bcst=true;  
              var_cst=NULL_CEWI;



              if(dmn)
              { 

                  aRef=dmn->getFirstChild();                  
                  // pPut dimension names in vector       
                  while(aRef) 
                  {
                    str_vtr.push_back(aRef->getText());
                    aRef=aRef->getNextSibling();      
                  }

                  // Cast is applied in VAR_ID action in function out()
                  // Note cast is NOT applied to an attribute, an irregular hyperslab
                  // or any entity which has a size of one.
                  var_cst=ncap_cst_mk(str_vtr,prs_arg);     
                  var1=out(vid1->getNextSibling());
              }  
              // deal with an empty hyperslab ie casting a scalar
              else
              {  
                 var1=out(vid1->getNextSibling());    
                 var_cst=ncap_sclr_var_mk("Internally generated template",var1->type,false);   
              }    



             // do attribute propagation only  from RHS -if not an att
             if( !ncap_var_is_att(var1))  
                 ncap_att_gnrl(var_nm,var1->nm,1,prs_arg);



              // If the RHS has size one or is an attribute or an irregular hyperslab
              // then we neet to use the var_cst as the shape of the written variable.
              // It is possible for the cast on the LHS to have a size of one and the RHS 
              // to have a size of one e.g., if the dim(s) in the list have a size of one   
              bool br1=(var_cst->sz >=1 && var1->sz==1);
              bool br2=(var_cst->sz==var1->sz &&  ( ncap_var_is_att(var1) ||var1->has_dpl_dmn==-1 ));

              // NB var_cst->val.vp always now set to null 
              // The code rebuilds var1 with the shape from the casting variable  
              if( br1 || br2){
                var_sct *var_nw;

                if(br1)
                   (void)ncap_att_stretch(var1,var_cst->sz);

                var_nw=nco_var_dpl(var_cst);
                (void)nco_var_cnf_typ(var1->type,var_nw);
                 
                var_nw->val.vp=var1->val.vp;
                var1->val.vp=(void*)NULL;

                var1=nco_var_free(var1);
                var1=var_nw;
             }
             

 

               //blow out if vars not the same size      
             if(var1->sz != var_cst->sz) 
                  err_prn(fnc_nm, "LHS cast for "+var_nm+" - cannot make RHS "+ std::string(var1->nm) + " conform.");          
              var1->nm=(char*)nco_free(var1->nm);
              var1->nm =strdup(var_nm.c_str());

              // See If we have to return something
              if(bret)
                var=nco_var_dpl(var1);     
              else 
                var=NULL_CEWI;

              //call to nco_var_get() in ncap_var_init() uses this property
              var1->typ_dsk=var1->type;
              (void)prs_arg->ncap_var_write(var1,bram);

              bcst=false;
              var_cst=nco_var_free(var_cst); 

    } // end action
           
          | vid2:VAR_ID {

               // Set class wide variables
               var_sct *var_rhs=NULL_CEWI; ;
               var_sct *var_shp=NULL_CEWI; ;
               NcapVar *Nvar;
               std::string var_nm;
 
               var_nm=vid2->getText();       


              if(nco_dbg_lvl_get() >= nco_dbg_var) dbg_prn(fnc_nm,var_nm);
               
               bcst=false;
               var_cst=NULL_CEWI; 
              
               var_rhs=out(vid2->getNextSibling());
               
               // Save name 
               std::string s_var_rhs(var_rhs->nm);

               // Do attribute propagation only if
               // var doesn't already exist or is defined but NOT
               // populated
               Nvar=prs_arg->var_vtr.find(var_nm);
               //rcd=nco_inq_varid_flg(prs_arg->out_id,var_rhs->nm ,&var_id);

               if(!Nvar || (Nvar && Nvar->flg_stt==1))
                 (void)ncap_att_cpy(var_nm,s_var_rhs,prs_arg);
               
                // var is defined and populated &  RHS is scalar -then stretch var to match
               // if(Nvar && Nvar->flg_stt==2)
               if(Nvar )
               {
                  var_shp=Nvar->cpyVarNoData();
                  nco_var_cnf_typ(var_shp->type,var_rhs);

                  long n_sz=var_shp->sz;

                  if(var_rhs->sz ==1 && n_sz >1)
                  {
                    (void)ncap_att_stretch(var_rhs,n_sz);
                    
                    // this is a special case -- if the RHS scalar has
                    // no missing value then retain LHS missing value
                    // else LHS missing value gets over written by RHS
                    //if(!var_rhs->has_mss_val)
                    // (void)nco_mss_val_cp(Nvar->var,var_rhs);
                  }

                  // deal with NC_CHAR on RHS as special case - if too short then pad out with nulls else trucate
                  else if(var_rhs->sz>1 &&  n_sz != var_rhs->sz &&  var_shp->type==NC_CHAR  )
                      ncap_att_char_stretch(var_rhs, n_sz);

                  else if( var_rhs->sz >1 && n_sz >1 && var_rhs->sz != n_sz && n_sz % var_rhs->sz ==0)
                      ncap_var_cnf_dmn(&Nvar->var,&var_rhs); 

                  if(var_rhs->sz != n_sz)
                   err_prn(fnc_nm, "size miss-match in simple assign between \""+ var_nm +"\""+ " size="+nbr2sng(Nvar->var->sz) + "var_rhs expr size="+nbr2sng(var_rhs->sz) );

                   var_shp->val.vp=var_rhs->val.vp;
                   var_rhs->val.vp=(void*)NULL;

                   if(var_rhs->has_mss_val)
                        (void)nco_mss_val_cp(var_rhs,var_shp);

                   if(var_rhs->has_add_fst)
                   {
                     var_shp->has_add_fst=1;  
                     var_shp->add_fst.vp=var_rhs->add_fst.vp;
                     var_rhs->has_add_fst=0;
                     var_rhs->add_fst.vp=(void*)NULL;
                   }

                   if(var_rhs->has_scl_fct)
                   {
                     var_shp->has_scl_fct=1;  
                     var_shp->scl_fct.vp=var_rhs->scl_fct.vp;
                     var_rhs->has_scl_fct=0;
                     var_rhs->scl_fct.vp=(void*)NULL;
                   }
                   var_rhs->pck_ram=False;
                   
                   if(var_shp->has_add_fst || var_shp->has_scl_fct)
                       var_shp->pck_ram=True;
                   else
                       var_shp->pck_ram=False;
                                            
                   var_rhs=nco_var_free(var_rhs);
                   var_rhs=var_shp;

               }
               // var is undefined in Output and special treatment of NC_CHAR on RHS
               else if(!Nvar && var_rhs->type ==NC_CHAR)
               {
                 if( prs_arg->ncap_var_init_chk(var_nm))
                    var_shp=prs_arg->ncap_var_init(var_nm,false);

                 if(var_shp->type==NC_CHAR && var_shp->sz != var_rhs->sz)
                 {
                     ncap_att_char_stretch(var_rhs, var_shp->sz);
                     var_shp->val.vp=var_rhs->val.vp;
                     var_rhs->val.vp=(void*)NULL;
                     if(var_rhs->has_mss_val)
                        (void)nco_mss_val_cp(var_rhs,var_shp);

                     var_rhs=nco_var_free(var_rhs);
                     var_rhs=var_shp;

                 }

                }


               // finally add new name before write  
               (void)nco_free(var_rhs->nm);                
               var_rhs->nm =strdup(var_nm.c_str());

               // Write var to disk
               (void)prs_arg->ncap_var_write(var_rhs,bram);


                // See If we have to return something
               if(bret)
                 var=prs_arg->ncap_var_init(var_nm,true);   
               else 
                 var=NULL_CEWI;

   
    } // end action
 
   |   (#(ATT_ID LMT_LIST)) => #(att:ATT_ID lmta:LMT_LIST){

            long srt,end,cnt,srd;  
            std::string att_nm=att->getText();
            std::string fnc_nm("att_lmt"); 
            NcapVar *Nvar=NULL;
            NcapVector<lmt_sct*> lmt_vtr;

            var_sct *var_lhs=NULL_CEWI; 
            var_sct *var_rhs=NULL_CEWI; 
            
            if(prs_arg->ntl_scn)
            { 
              // only final scan for this attribute hyperslab 
              var=ncap_var_udf(att_nm.c_str());
              // can't return here -- have to use goto
              goto att_end; 
            } 
  
            Nvar=prs_arg->var_vtr.find(att_nm);

            if(Nvar!=NULL)
                var_lhs=nco_var_dpl(Nvar->var);
            else    
                var_lhs=ncap_att_init(att_nm,prs_arg);
            

            if(var_lhs==NULL_CEWI )
                err_prn(fnc_nm,"Unable to locate attribute " +att_nm+ " in input or output files.");
            
		    lmt_mk(1L,lmta,lmt_vtr);
           

            if( lmt_vtr.size() != 1)
                err_prn(fnc_nm,"Number of hyperslab limits for an attribute "+ att_nm+" must be one ");


            lmt_vtr[0]->nm=strdup("zz@attribute");    
            (void)ncap_lmt_evl(1,lmt_vtr[0],var_lhs->sz,prs_arg); 
            
            if(lmt_vtr[0]->cnt==0)
               err_prn(fnc_nm,"Hyperslab limits for attribute "+ att_nm+" doesn't include any elements");   

            srt=lmt_vtr[0]->srt;
            cnt=lmt_vtr[0]->cnt;
            srd=lmt_vtr[0]->srd; 
            end=lmt_vtr[0]->end; 

            // get rhs can be anything as long as it is of size cnt 
            var_rhs=out(att->getNextSibling());


            
            if( var_lhs->type==NC_STRING)
            { 
              long szn; 
              long idx; 

              char buffer[NC_MAX_ATTRS];  
               
              buffer[0]='\0';
              cnt=0L;
              cast_void_nctype(NC_STRING, &var_lhs->val);    
              cast_void_nctype(var_rhs->type, &var_rhs->val);     
               
              if( var_rhs->type==NC_STRING)
              {
                // check size
               if(  var_rhs->sz!=1L &&  var_rhs->sz != cnt)   
                  err_prn(fnc_nm,"Hyperslab limits for attribute "+att_nm + " on LHS size="+nbr2sng(cnt)+ " doesn't match RHS size=" + nbr2sng(var_rhs->sz));

                   
                szn=(var_rhs->sz >1 ? 1: 0);     
                for(idx=srt; idx<=end;idx+=srd)  
                {
                   var_lhs->val.sngp[idx]=strdup(var_rhs->val.sngp[cnt]);    
                   cnt+=szn;
                }


              } 
                  
              else if( var_rhs->type==NC_CHAR )
              { 
               // if RHS is a char string then it reduces to  single NC_STRING regardless of size  
                strncpy(buffer, var_rhs->val.cp , var_rhs->sz);   
                buffer[var_rhs->sz]='\0';  

                for(idx=srt; idx<=end;idx+=srd)  
                   var_lhs->val.sngp[idx]=strdup(buffer);    
              }
              else 
              { 
                err_prn(fnc_nm,"To hyperslab into a text NC_STRING The RHS type must alo be a text string of type NC_CHAR or NC_STRING"); 
              } 

              cast_nctype_void(var_rhs->type, &var_rhs->val);    
              cast_nctype_void(var_lhs->type, &var_lhs->val);    

  

            }
            

            // deal with regular types   
            if(var_lhs->type !=NC_STRING)
            {  
               char *cp_in;
               char *cp_out;  
               long idx;
               long szn; 

               size_t slb_sz=nco_typ_lng(var_lhs->type); 

               // check size
               if(  var_rhs->sz!=1L &&  var_rhs->sz != cnt)   
                  err_prn(fnc_nm,"Hyperslab limits for attribute "+att_nm + " on LHS size="+nbr2sng(cnt)+ " doesn't match RHS size=" + nbr2sng(var_rhs->sz));

 
               nco_var_cnf_typ(var_lhs->type,var_rhs);               

               // if rhs size is one then simply copy across possibly multiple times
               szn=(var_rhs->sz >1 ? 1: 0);   

               cp_in=(char*)(var_rhs->val.vp); 
              
               //cp_out=(char*)var_lhs->val.vp; 

               for(idx=srt; idx<=end;idx+=srd) 
               { 
                cp_out=(char*)(var_lhs->val.vp)+slb_sz*idx;     
                memcpy(cp_out,cp_in,slb_sz);  
                cp_in+= szn;
               }

            }

            nco_var_free(var_rhs);  
            nco_lmt_free(lmt_vtr[0]); 
                  
            Nvar=new NcapVar(var_lhs,att_nm);

            prs_arg->var_vtr.push_ow(Nvar);       

               // See If we have to return something
            if(bret)
              var=nco_var_dpl(var_lhs);             
            else 
              var=NULL_CEWI; 

           att_end: ; 

        
    }

   |   (#(ATT_ID DMN_LIST))=> #(att1:ATT_ID DMN_LIST){
        ;
        } 
   |  att2:ATT_ID {
       
            var_sct *var1;
            string sa=att2->getText();

            if(nco_dbg_lvl_get() >= nco_dbg_var) dbg_prn(fnc_nm,sa);
 
            var1=out(att2->getNextSibling());
            // we dont need missing values in ATT right !!
            if(var1->has_mss_val){
              var1->has_mss_val=False;
	          var1->mss_val.vp=(void*)nco_free(var1->mss_val.vp);
            }   

            // if RHS is a non scalar variable then loose superfluous dimension data 
            if( var1->nbr_dim >0){
              var_sct *var_tmp;
              var_tmp=ncap_sclr_var_mk(sa,var1->type,false);   
              var_tmp->sz=var1->sz;
              var_tmp->val.vp=var1->val.vp;     
              var1->val.vp =(void*)NULL;
              var1=nco_var_free(var1);
              var1=var_tmp;
            }else{   
              (void)nco_free(var1->nm);
               var1->nm=strdup(sa.c_str());
            }

            NcapVar *Nvar=new NcapVar(var1,sa);
            prs_arg->var_vtr.push_ow(Nvar);       

               // See If we have to return something
            if(bret)
              var=nco_var_dpl(var1);               
            else 
              var=NULL_CEWI;

                  
    } // end action
   ;
               
out returns [var_sct *var]
{
    bool bret=true;   
    const std::string fnc_nm("out"); 
	var_sct *var1;
    var_sct *var2;
    var=NULL_CEWI;
  
    // on the following tokens if text is set to NO_RET then make sure out return var=NULL
    // the check is done in statements / EXPR
    // out_AST_in declared an inialized  to the first arg of out() (see ncoTree.cpp/out() for actual code )
    if( out_AST_in != ANTLR_USE_NAMESPACE(antlr)nullAST && out_AST_in->getText()=="NO_RET" )
    {
       int aType=out_AST_in->getType();
       if(aType==ASSIGN ||aType==POST_INC || aType==POST_DEC || aType==INC || aType==DEC||aType==PLUS_ASSIGN || aType==MINUS_ASSIGN || aType==DIVIDE_ASSIGN || aType==TIMES_ASSIGN)
          bret=false;
    }      
}  
    // arithmetic operators 

    :  (#(PLUS out out)) =>  #( PLUS  var1=out var2=out)  
           { var=ncap_var_var_op(var1,var2, PLUS );}
	|  (#(MINUS out out)) => #( MINUS var1=out var2=out)
            { var=ncap_var_var_op(var1,var2, MINUS );}
    |  (#(POST_INC #(UTIMES ATT_ID)))=> #( POST_INC #(UTIMES aposti:ATT_ID)){
             var1=out(att2var(aposti));     
             var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,false,bret,prs_arg);      
       } 
    |  (#(POST_DEC #(UTIMES ATT_ID)))=> #( POST_DEC #(UTIMES apostd:ATT_ID)){
             var1=out(att2var(apostd));     
             var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,false,bret,prs_arg);      
       } 
    |  (#(INC #(UTIMES ATT_ID)))=> #( INC #(UTIMES aprei:ATT_ID)){
             var1=out(att2var(aprei));     
             var=ncap_var_var_inc(var1,NULL_CEWI,INC,false,bret,prs_arg);      
       } 
    |  (#(DEC #(UTIMES ATT_ID)))=> #( DEC #(UTIMES apred:ATT_ID)){
             var1=out(att2var(apred));     
             var=ncap_var_var_inc(var1,NULL_CEWI,DEC,false,bret,prs_arg);      
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
            { var=ncap_var_var_inc(var1,NULL_CEWI,INC,false,bret,prs_arg);}

    |   #(DEC var1=out_asn )      
            {var=ncap_var_var_inc(var1,NULL_CEWI, DEC,false,bret,prs_arg );}

    |   #(POST_INC var1=out_asn ){
            var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,false,bret,prs_arg);
        }
    |   #(POST_DEC var1=out_asn ){      
            var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,false,bret,prs_arg);
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
    // Fortran style Comparison Operators
    | #(FLTHAN  var1=out var2=out) 
            { var=ncap_var_var_op(var1,var2, FLTHAN );}
    | #(FGTHAN  var1=out var2=out) 
            { var=ncap_var_var_op(var1,var2, FGTHAN );}
    // Assign Operators 
    // | #(PLUS_ASSIGN pls_asn:. var2=out) {
    //    var1=out_asn(pls_asn);
    //    var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN ,(pls_asn->getType()==UTIMES), prs_arg);
    //    }
    // Assign Operators 
    
    | ( #(PLUS_ASSIGN  (VAR_ID|ATT_ID)  var2=out)) => #(PLUS_ASSIGN var1=out  var2=out)  {
       var=ncap_var_var_inc(var1,var2,PLUS_ASSIGN ,false, bret,prs_arg);
       }

    | (#(PLUS_ASSIGN  #(UTIMES VAR_ID) var2=out)) => #(PLUS_ASSIGN  #(UTIMES var1=out)  var2=out)  {
       var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN ,true, bret,prs_arg);
       }

    | (#(PLUS_ASSIGN  #(UTIMES ATT_ID) var2=out)) => #(PLUS_ASSIGN  #(UTIMES atp:ATT_ID)  var2=out)  {

       var1=out(att2var(atp));     
       var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN ,false,bret, prs_arg);
       }


    | ( #(MINUS_ASSIGN  (VAR_ID|ATT_ID)  var2=out)) => #(MINUS_ASSIGN  var1=out  var2=out)  {
       var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN ,false, bret,prs_arg);
       }

    | (#(MINUS_ASSIGN #(UTIMES VAR_ID) var2=out)) => #(MINUS_ASSIGN #(UTIMES var1=out)  var2=out)  {
       var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN ,true, bret,prs_arg);
       }

    | (#(MINUS_ASSIGN #(UTIMES ATT_ID) var2=out)) => #(MINUS_ASSIGN #(UTIMES atm:ATT_ID)  var2=out)  {
       var1=out(att2var(atm));     
       var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN ,false, bret,prs_arg);
       }

    | (#(TIMES_ASSIGN  (VAR_ID|ATT_ID)  var2=out)) => #(TIMES_ASSIGN  var1=out  var2=out)  {
       var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN ,false, bret,prs_arg);
       }

    | (#(TIMES_ASSIGN #(UTIMES VAR_ID) var2=out)) => #(TIMES_ASSIGN #(UTIMES var1=out)  var2=out)  {
       var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN ,true, bret,prs_arg);
       }

    | (#(TIMES_ASSIGN #(UTIMES ATT_ID) var2=out)) => #(TIMES_ASSIGN #(UTIMES attm:ATT_ID)  var2=out)  {
       var1=out(att2var(attm));     
       var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN ,false, bret,prs_arg);
       }

    | (#(DIVIDE_ASSIGN  (VAR_ID|ATT_ID)  var2=out)) => #(DIVIDE_ASSIGN  var1=out  var2=out)  {
       var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN ,false, bret,prs_arg);
       }

    | (#(DIVIDE_ASSIGN #(UTIMES VAR_ID) var2=out)) => #(DIVIDE_ASSIGN #(UTIMES var1=out)  var2=out)  {
       var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN ,true, bret,prs_arg);
       }

    | (#(DIVIDE_ASSIGN #(UTIMES ATT_ID) var2=out)) => #(DIVIDE_ASSIGN #(UTIMES atd:ATT_ID)  var2=out)  {
       var1=out(att2var(atd));        
       var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN ,false, bret,prs_arg);
       }


    | (#(ASSIGN (VAR_ID|ATT_ID))) => #(ASSIGN asn:.) {
             if(prs_arg->ntl_scn)
               var=assign_ntl(asn,false,bret); 
             else
               var=assign(asn,false,bret);
    }

    // dealing with a variable pointer  
    | (#(ASSIGN #(UTIMES ATT_ID) )) =>  #(ASSIGN #(atta:UTIMES ATT_ID)) {

             RefAST tr;
             tr=atta->getFirstChild();
             tr->setNextSibling(atta->getNextSibling());
             
             // remember tr siblings and children are  duplicated here   
             tr=att2var(tr);   

             if(prs_arg->ntl_scn)
               var=assign_ntl(tr,false,bret); 
             else
               var=assign(tr,false,bret);
    }  
    
    //  memory var - regular and pointer
    | #(ASSIGN #(asn2:UTIMES (VAR_ID| #(UTIMES  asn2a:ATT_ID)) )) {
              // Check for RAM variable - if present 
              // change tree - for example from:
              //     ( EXPR ( = ( * n1 ) ( + four four ) ) )
              // to  ( EXPR ( = n1 ( + four four ) ) )

             RefAST tr;
             NcapVar *Nvar;          

             // deal with ATT_ID 
             if(asn2a)  
             { 
               // remember tr and siblings and children are  duplicated here
               tr=att2var(asn2a);   
               tr->setNextSibling(asn2->getNextSibling());

             } 
             // must be VAR_ID 
             else 
             { 
                  
               tr=asn2->getFirstChild();
               tr->setNextSibling(asn2->getNextSibling());

               // Die if attempting to create a RAM var 
               // from an existing disk var   
                Nvar= prs_arg->var_vtr.find(tr->getText());

                if(Nvar && Nvar->flg_mem==false)
                {
                  std::string serr;
                  serr= "It is impossible to recast disk variable: \"" + tr->getText() +"\" as a RAM variable.";
                  err_prn(fnc_nm,serr );       
                
                }                
             }

             if(prs_arg->ntl_scn)
               var=assign_ntl(tr,true,bret); 
             else
               var=assign(tr, true,bret);
               
        }
   


    | #(UTIMES attz:ATT_ID) {
          var=out(att2var(attz));    
    }



     | #(WHERE_ASSIGN wasn:. ) {

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
          // The lexer has appended the index of the function to the function name m - (name#index)
          //  the index is into fmc_vtr  
         string sm(m->getText()); 
         string sdx(sm,sm.find("#")+1,sm.length()-1) ;
         int idx=atoi(sdx.c_str());
         RefAST tr;  
         var=prs_arg->fmc_vtr[idx].vfnc()->fnd(tr ,args, prs_arg->fmc_vtr[idx],*this); 
        }

    // Deal with methods 
    | #(DOT mtd:. mfnc:FUNC  margs:FUNC_ARG ){
          // The lexer has appended the index of the function to the function name m - (name#index)
          //  the index is into fmc_vtr  
         string sm(mfnc->getText()); 
         string sdx(sm,sm.find("#")+1,sm.length()-1) ;
         int idx=atoi(sdx.c_str());
         var=prs_arg->fmc_vtr[idx].vfnc()->fnd(mtd ,margs, prs_arg->fmc_vtr[idx],*this); 
     }

    |   dval:DIM_ID_SIZE
        {
            string sDim=dval->getText();
            dmn_sct *dmn_fd;

            // Check output 
            dmn_fd=prs_arg->dmn_out_vtr.find(sDim);
            // Check input
            if(dmn_fd==NULL_CEWI)
              dmn_fd=prs_arg->dmn_in_vtr.find(sDim);

            // nb there is  a problem with the below code 
            // if we have a netcdf4 file and on the intial scan dim is undefined (dmn_fd==NULL)     
            // and on the final scan  size >NC_MAX_INT then there is a conflict between 
            // the initial returned  type (NC_INT) and the final returned type (NC_INT64)

// csz 20100119 Workaround dependence on NC_MAX_INT which DNE in netcdf.h until ~2009
#ifndef NC_MAX_INT
# define NC_MAX_INT 2147483647
#endif
            //Initial Scan
            if(prs_arg->ntl_scn){  
                if( (dmn_fd==NULL_CEWI )|| (dmn_fd->sz <= NC_MAX_INT) )
                   var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),(nc_type)NC_INT,false);
                else 
                   var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),(nc_type)NC_INT64,false);

            }else{ 

                if( dmn_fd==NULL_CEWI )
                    err_prn(fnc_nm,"Unable to locate dimension " +sDim+ " in input or output files ");


                if(dmn_fd->sz <= NC_MAX_INT )
                  var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),(nco_int)dmn_fd->sz);
                else
                  var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),(nco_int64)dmn_fd->sz); 
            } 
                  
        }  // end action 
        
     // Variable with argument list 
    | (#(VAR_ID LMT_LIST)) => #( vid:VAR_ID lmt:LMT_LIST) {
		
		    // fxm: 4 Oct 2015
			/*
			  the following is valid syntx var_nm(array_var)-
			  array_var now contains all the indices - that is 
			  srt vectors  
			  srt & end vectors
			  srt,end,srd vectors
			
			  so for now indexing into a multi-dmensional var using a single index has
			  temporarily been disabled
			
           // see if hyperslab limit is a single value
           if(lmt->getNumberOfChildren()==1 && 
              lmt->getFirstChild()->getNumberOfChildren()==1 &&
              lmt->getFirstChild()->getFirstChild()->getType() != COLON)
                
              var=var_lmt_one(vid);
           else   
               var=var_lmt(vid);
           */
           var=var_lmt(vid);  
     
             
    }

     // attribute with argument list 
    | (#(ATT_ID LMT_LIST)) => #( attl:ATT_ID lmtl:LMT_LIST) {

            std::string att_nm=attl->getText();
            std::string fnc_nm("att_lmt"); 
            NcapVar *Nvar=NULL;
            NcapVector<lmt_sct*> lmt_vtr;

            var_sct *var_att=NULL_CEWI; 

            
            if(prs_arg->ntl_scn)
            { 
              // only final scan for this attribute hyperslab 
              var=ncap_var_udf(att_nm.c_str());
              // can't return here -- have to use goto
              goto attl_end; 
            } 
  
            Nvar=prs_arg->var_vtr.find(attl->getText());

            if(Nvar!=NULL)
                var_att=nco_var_dpl(Nvar->var);
            else    
                var_att=ncap_att_init(att_nm,prs_arg);
            

            if(var_att==NULL_CEWI )
                err_prn(fnc_nm,"Unable to locate attribute " +att_nm+ " in input or output files.");
            
		    lmt_mk(1L,lmtl,lmt_vtr);
           

            if( lmt_vtr.size() != 1)
                err_prn(fnc_nm,"Number of hyperslab limits for an attribute "+ att_nm+"must be one ");

            // fxm - very soon  
            lmt_vtr[0]->nm=strdup("zz@attribute");    
            (void)ncap_lmt_evl(1,lmt_vtr[0],var_att->sz,prs_arg); 
             
            // we have something to hyperslab
            if( lmt_vtr[0]->cnt > 0)
            {  
               char *cp_in;
               char *cp_out;  
               long idx;
               long jdx;  
               long srt=lmt_vtr[0]->srt;
               long cnt=lmt_vtr[0]->cnt;
               long srd=lmt_vtr[0]->srd; 
               long end=lmt_vtr[0]->end; 
               size_t slb_sz=nco_typ_lng(var_att->type); 
                
               /* create output att */
                 
               if( var_att->type ==NC_STRING )  
               {
                   var=ncap_sclr_var_mk(att_nm,var_att->type,false);                
                   var->val.vp=(void*)nco_malloc(slb_sz*cnt);  
                   var->sz=cnt;       
                   (void)cast_void_nctype((nc_type)NC_STRING,&var->val);                 
                   (void)cast_void_nctype((nc_type)NC_STRING,&var_att->val);                  
                   
                   jdx=0;  
                   for(idx=srt;idx<=end;idx+=srd)  
                     var->val.sngp[jdx++]=strdup(var_att->val.sngp[idx]);

                   (void)cast_nctype_void((nc_type)NC_STRING,&var->val); 
                   (void)cast_nctype_void((nc_type)NC_STRING,&var_att->val); 
               
               }
               else
               {     
                   var=ncap_sclr_var_mk(att_nm,var_att->type,true);                 
                   (void)ncap_att_stretch(var,cnt);     

                   cp_in=(char*)( var_att->val.vp); 
                   cp_in+= (ptrdiff_t)slb_sz*srt;
                   cp_out=(char*)var->val.vp; 
                   
                   idx=0;
                   
                   while(idx++ < cnt )
                   { 
                       memcpy(cp_out, cp_in, slb_sz);                   
                       cp_in+=srd*slb_sz;
                       cp_out+=slb_sz; 
                   } 

               }               

            }
            else
            {
              err_prn(fnc_nm,"Hyperslab limits for attribute "+ att_nm+" doesn't include any elements");  
            }

            nco_lmt_free(lmt_vtr[0]);  
            nco_var_free(var_att);    

            attl_end: ; 
        }



    // plain Variable
	|   v:VAR_ID       
        { 
          //dbg_prn(fnc_nm,"getting regular var in out "+v->getText());
   
          var=prs_arg->ncap_var_init(v->getText(),true);

         // initial scan 
         if(prs_arg->ntl_scn)  
         {  
           if(var==NULL)     
              var=ncap_var_udf(v->getText().c_str());
           else if(bcst && var_cst && var->sz >1)      
              var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);  
         } 
         // final scan  
         else
         { 
           if(var==NULL)
             err_prn(fnc_nm,"Could not read var "+ v->getText());
  
           if(bcst && var_cst && var->sz >1)
           {    
             if(var_cst->nbr_dim==var->nbr_dim)     
               ncap_var_cnf_dmn(&var,&var_cst);
             else    
               var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);
           }   
         }


    } /* end action */

    // PLain attribute
    |   att:ATT_ID { 
   
            var=att_plain(att);  
            /*  
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
            */
              
        }

     // Value list -- stuff values into an attribute
     |   vlst:VALUE_LIST {
          var=value_list(vlst);
         }

    |   str:NSTRING
        {
            /* use malloc here rather than strdup(str->getText().c_str()) as this causes 
               an invalid-read when using GCC compiler */ 
            char *tsng;
            tsng=(char*)nco_malloc(str->getText().size()+1);    
            strcpy(tsng, str->getText().c_str());
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




    |   str1:N4STRING
        {
            char *tsng;
            tsng=(char*)nco_malloc(str1->getText().size()+1);    
            strcpy(tsng, str1->getText().c_str());
            (void)sng_ascii_trn(tsng);            
            var=(var_sct *)nco_malloc(sizeof(var_sct));
            /* Set defaults */
            (void)var_dfl_set(var); 
            /* Overwrite with attribute expression information */
            var->nm=strdup("~zz@string");
            var->nbr_dim=0;
            var->sz=1;

            var->type=(nc_type)NC_STRING;
            if(!prs_arg->ntl_scn){

             // nb sngp is type char** a ragged array of chars
             // each string terminated by a (char*)NULL  
             var->val.vp=(void*)nco_malloc(nco_typ_lng((nc_type)NC_STRING));
             (void)cast_void_nctype((nc_type)NC_STRING,&var->val);

             var->val.sngp[0]=strdup(tsng);   

             (void)cast_nctype_void((nc_type)NC_STRING,&var->val);
            }
            tsng=(char*)nco_free(tsng);      
        }

        // Naked numbers: Cast is not applied to these numbers
    |   val_float:NCAP_FLOAT  
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~float"),(nc_type)NC_FLOAT,false); else var=ncap_sclr_var_mk(SCS("~float"),static_cast<float>(std::strtod(val_float->getText().c_str(),(char **)NULL)));} // end FLOAT
    |   val_double:NCAP_DOUBLE        
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~double"),(nc_type)NC_DOUBLE,false); else var=ncap_sclr_var_mk(SCS("~double"),strtod(val_double->getText().c_str(),(char **)NULL));} // end DOUBLE
	|	val_int:NCAP_INT
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~int"),(nc_type)NC_INT,false); else var=ncap_sclr_var_mk(SCS("~int"),static_cast<nco_int>(std::strtol(val_int->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end INT
	|	val_short:NCAP_SHORT
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~short"),(nc_type)NC_SHORT,false); else var=ncap_sclr_var_mk(SCS("~short"),static_cast<nco_short>(std::strtol(val_short->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end SHORT
    |	val_byte:NCAP_BYTE		
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~byte"),(nc_type)NC_BYTE,false); else var=ncap_sclr_var_mk(SCS("~byte"),static_cast<nco_byte>(std::strtol(val_byte->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end BYTE
// fxm TODO nco851: How to add ENABLE_NETCDF4 #ifdefs to ncoGrammer.g?
// Workaround (permanent?) is to add stub netCDF4 forward compatibility prototypes to netCDF3 libnco
// #ifdef ENABLE_NETCDF4
	|	val_ubyte:NCAP_UBYTE
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~ubyte"),(nc_type)NC_UBYTE,false); else var=ncap_sclr_var_mk(SCS("~ubyte"),static_cast<nco_ubyte>(std::strtoul(val_ubyte->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end UBYTE
        // NB: sng2nbr converts "255" into nco_ubtye=2. This is not good.
        //        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~ubyte"),(nc_type)NC_UBYTE,false); else var=ncap_sclr_var_mk(SCS("~ubyte"),sng2nbr(val_ubyte->getText(),nco_ubyte_CEWI));} // end UBYTE
	|	val_ushort:NCAP_USHORT
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~ushort"),(nc_type)NC_USHORT,false); else var=ncap_sclr_var_mk(SCS("~ushort"),static_cast<nco_ushort>(std::strtoul(val_ushort->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end USHORT
	|	val_uint:NCAP_UINT	
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~uint"),(nc_type)NC_UINT,false); else var=ncap_sclr_var_mk(SCS("~uint"),static_cast<nco_uint>(std::strtoul(val_uint->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end UINT
	|	val_int64:NCAP_INT64
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~int64"),(nc_type)NC_INT64,false); else var=ncap_sclr_var_mk(SCS("~int64"),sng2nbr(val_int64->getText(),nco_int64_CEWI));} // end INT64
        // std::strtoll() and std::strtoull() are not (yet) ISO C++ standard
        //{if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~int64"),(nc_type)NC_INT64,false); else var=ncap_sclr_var_mk(SCS("~int64"),static_cast<nco_int64>(std::strtoll(val_int64->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end INT64
	|	val_uint64:NCAP_UINT64
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~uint64"),(nc_type)NC_UINT64,false); else var=ncap_sclr_var_mk(SCS("~uint64"),sng2nbr(val_uint64->getText(),nco_uint64_CEWI));} // end UINT64
        // std::strtoll() and std::strtoull() are not (yet) ISO C++ standard
        // {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(SCS("~uint64"),(nc_type)NC_UINT64,false); else var=ncap_sclr_var_mk(SCS("~uint64"),static_cast<nco_uint64>(std::strtoull(val_uint64->getText().c_str(),(char **)NULL,NCO_SNG_CNV_BASE10)));} // end UINT64
// #endif /* !ENABLE_NETCDF4 */

;


// Return a var or att WITHOUT applying a cast 
// and checks that the operand is a valid Lvalue
// ie that the var or att has NO children!!
out_asn returns [var_sct *var]
{
const std::string fnc_nm("assign_asn");
var=NULL_CEWI; 
string var_nm_s; 
NcapVar *Nvar;
   
}

   : #(UTIMES vid1:VAR_ID)
        { 
          if(vid1->getFirstChild())
               err_prn(fnc_nm,"Invalid Lvalue " +vid1->getText() );

          //do attribute inheritance 
          var_nm_s=vid1->getText(); 
 
          Nvar=prs_arg->var_vtr.find(var_nm_s);
          if( !Nvar || Nvar->flg_stt==1 ) 
               ncap_att_cpy(var_nm_s,var_nm_s,prs_arg);
  
          var=prs_arg->ncap_var_init(var_nm_s,true);
          if(var== NULL_CEWI){
               nco_exit(EXIT_FAILURE);
          }

        }
	|   vid:VAR_ID       
        { 
          var_nm_s=vid->getText();  
          if(vid->getFirstChild())
               err_prn(fnc_nm,"Invalid Lvalue " +vid->getText() );

          //do attribute inheritance

          Nvar=prs_arg->var_vtr.find(var_nm_s);
          if( !Nvar || Nvar->flg_stt==1 ) 
             ncap_att_cpy(var_nm_s,var_nm_s,prs_arg);

          var=prs_arg->ncap_var_init(var_nm_s,true);



          if(var== NULL_CEWI){
               nco_exit(EXIT_FAILURE);
          }
         
        } /* end action */
    // Plain attribute
    |   att:ATT_ID { 
            // check "output"

         
            if(att->getFirstChild())
                err_prn(fnc_nm,"Invalid Lvalue " +att->getText() );

            var=att_plain(att);

            /* 
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
            */ 
              

       }// end action    
;



att_plain returns [var_sct *var]
{
const std::string fnc_nm("att_plain");
var=NULL_CEWI; 
string att_nm; 

   
}

: att:ATT_ID 
      { 
            // check "output"
            NcapVar *Nvar=NULL;
             
            att_nm=att->getText();       

            if(prs_arg->ntl_scn)
              Nvar=prs_arg->int_vtr.find(att_nm);

            if(Nvar==NULL) 
              Nvar=prs_arg->var_vtr.find(att_nm);

            var=NULL_CEWI;    
            if(Nvar !=NULL)
                var=nco_var_dpl(Nvar->var);
            else    
                var=ncap_att_init(att_nm,prs_arg);

            if(prs_arg->ntl_scn==False  && var==NULL_CEWI )
                err_prn(fnc_nm,"Unable to locate attribute " +att_nm+ " in input or output files.");

            
            // if att not found return undefined
            if(prs_arg->ntl_scn && var==NULL_CEWI )
                var=ncap_var_udf(att_nm.c_str());

            if(prs_arg->ntl_scn && var->val.vp !=NULL)
                var->val.vp=(void*)nco_free(var->val.vp);

       }// end action    
;





// takes an ATT_ID pointer and converts it to a VAR_ID
att2var returns [ RefAST tr ]
{
  var_sct *var=NULL_CEWI; 
  NcapVar *Nvar;
  std::string sn;
  std::string att_nm;
  std::string fnc_nm("att2var");

}

: (att:ATT_ID) 
   {
    /* sn can be empty on 1st Parse but not 2nd */
   att_nm=att->getText();
   
   if(nco_dbg_lvl_get() >= nco_dbg_scl) 
      dbg_prn(fnc_nm,"att_nm="+att_nm);
       
    sn=ncap_att2var(prs_arg,att_nm); 
   
    if(prs_arg->ntl_scn && sn.empty())     
    {
       sn="_empty_"+att_nm;   
       var=ncap_var_udf(sn.c_str());
       Nvar=new NcapVar(var);
       prs_arg->int_vtr.push_ow(Nvar);            
    }
    

    tr=nco_dupList(att);
    
    if(sn.find('@') != std::string::npos)
      tr->setType(ATT_ID);       
    else
      tr->setType(VAR_ID);       

    tr->setText(sn);

    }
;




value_list returns [var_sct *var]
{
const std::string fnc_nm("value_list");
var=NULL_CEWI; 
}
 :(vlst:VALUE_LIST) {
         char *cp;  
         int nbr_lst;
         int idx;
         int tsz;

         nc_type type=NC_NAT;
         var_sct *var_ret;                        
         var_sct *var_int; 
         RefAST rRef;

         rRef=vlst->getFirstChild();

         nbr_lst=vlst->getNumberOfChildren(); 

         /* get type of first element */ 
         var_int=out(rRef);       

         /* first element undefined */  
         if(var_int->undefined)
          {   
            var_ret=ncap_var_udf("~zz@value_list");  
            goto end_val; 
          } 
          
          type=var_int->type;   

          if(type==NC_STRING) 
          { 
           var_ret=value_list_string(vlst); 
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

          /* deal with initial scan */   
          if(prs_arg->ntl_scn)
             goto end_val;

          /* create some space for output */
          tsz=nco_typ_lng(type);
          var_ret->val.vp=(void*)nco_malloc(nbr_lst*tsz);

          /* copy first value over */
          memcpy(var_ret->val.vp, var_int->val.vp, tsz);  
          var_int=nco_var_free(var_int); 
          rRef=rRef->getNextSibling();

          /* rest of values */
          for(idx=1;idx<nbr_lst;idx++) 
          {
            var_int=out(rRef);   
            nco_var_cnf_typ(type,var_int);  
            cp=(char*)(var_ret->val.vp)+ (ptrdiff_t)(idx*tsz);
            memcpy(cp,var_int->val.vp,tsz);
 
            var_int=nco_var_free(var_int); 
            rRef=rRef->getNextSibling();
          }
          
          end_val: if(var_int)
                      nco_var_free(var_int);  
          var=var_ret;

    } // end action
;


value_list_string returns [var_sct *var]
{
const std::string fnc_nm("value_list");
var=NULL_CEWI; 
}
 :(vlst:VALUE_LIST) {


         int nbr_lst;
         int idx;
         int tsz;

         nc_type type=NC_NAT;
         var_sct *var_ret;                        
         var_sct *var_int; 
         RefAST rRef;
         
         rRef=vlst->getFirstChild();

         nbr_lst=vlst->getNumberOfChildren(); 
         
         type=NC_STRING;   

         var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
         /* Set defaults */
         (void)var_dfl_set(var_ret); 

         /* Overwrite with attribute expression information */
         var_ret->nm=strdup("~zz@value_list");
         var_ret->nbr_dim=0;
         var_ret->sz=nbr_lst;
         var_ret->type=type;

         /* deal with initial scan */   
         if(prs_arg->ntl_scn)
             goto end_val;

         /* create some space for output */
         tsz=nco_typ_lng(type);
         var_ret->val.vp=(void*)nco_malloc(nbr_lst*tsz);
         (void)cast_void_nctype((nc_type)NC_STRING,&var_ret->val);

         for(idx=0;idx<nbr_lst;idx++) 
             {
                 var_int=out(rRef);   
                 if(var_int->type != NC_STRING)
                     err_prn(fnc_nm," error processing value list string: to successfully parse value list of strings all elements must be of type NC_STRING");    

                 (void)cast_void_nctype((nc_type)NC_STRING,&var_int->val);
                 var_ret->val.sngp[idx]=strdup(var_int->val.sngp[0]); 
                 // cast pointer back
                 (void)cast_nctype_void((nc_type)NC_STRING,&var_int->val);

                 nco_var_free(var_int);  
                 rRef=rRef->getNextSibling();
             }
         (void)cast_nctype_void((nc_type)NC_STRING,&var_ret->val);

    end_val: var=var_ret;


    } // end action
;



value_list_old returns [var_sct *var]
{
const std::string fnc_nm("value_list");
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
         std::vector<var_sct*> exp_vtr;
         
         rRef=vlst->getFirstChild();

         while(rRef){
           exp_vtr.push_back(out(rRef));   
           rRef=rRef->getNextSibling();
         }       
         nbr_lst=exp_vtr.size();
         

         // if any types are NC_STRING then call value_list_string() action 
         for(idx=0;idx <nbr_lst ;idx++)
           if(exp_vtr[idx]->type == NC_STRING){
             var_ret=value_list_string_old(rRef,exp_vtr);
             goto end_val;
           }
      
         // find highest type
         for(idx=0;idx <nbr_lst ;idx++)
           type=ncap_typ_hgh(type,exp_vtr[idx]->type);
             //(void)ncap_var_retype(exp_vtr[0], exp_vtr[idx]);  

         // Inital Scan
         if(prs_arg->ntl_scn){

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

        } // end action
;





// Deal here with a value list of strings
// Called only from value_list
value_list_string_old[ std::vector<var_sct*> &exp_vtr] returns [var_sct *var]
{
const std::string fnc_nm("value_list_string");
var=NULL_CEWI; 
}
    :
        {
         int idx;
         int nbr_lst;
         int tsz;
         nco_string *cp;         
         var_sct *var_ret;                        

         nbr_lst=exp_vtr.size();
         var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
         /* Set defaults */
         (void)var_dfl_set(var_ret); 

         /* Overwrite with attribute expression information */
         var_ret->nm=strdup("~zz@value_list");
         var_ret->nbr_dim=0;
         var_ret->sz=nbr_lst;
         var_ret->type=(nc_type)NC_STRING;

         // Inital Scan
         if(prs_arg->ntl_scn)
           goto end_val;          

         // check all var types in vector must be type NC_STRING
         for(idx=0; idx<nbr_lst ; idx++)
            if( exp_vtr[idx]->type != NC_STRING)
              err_prn(fnc_nm," error processing value list string: to successfully parse value list of strings all elements must be of type NC_STRING");

         // from here on deal with final scan
         tsz=nco_typ_lng((nc_type)NC_STRING);
         var_ret->val.vp=(void*)nco_malloc(nbr_lst*tsz);

         (void)cast_void_nctype((nc_type)NC_STRING,&var_ret->val);
         cp=var_ret->val.sngp; 

         for(idx=0 ; idx<nbr_lst;idx++){
           // dereference 
           var_sct *var_in=exp_vtr[idx];
           (void)cast_void_nctype((nc_type)NC_STRING,&var_in->val);
           cp[idx]=strdup(var_in->val.sngp[0]); 
           // cast pointer back
           (void)cast_nctype_void((nc_type)NC_STRING,&var_in->val);
         } // end loop      

         (void)cast_nctype_void((nc_type)NC_STRING,&var_ret->val);
         
         end_val: var=var_ret;

}// end action
;

//where calculate 
where_assign [var_sct *var_msk] returns [bool bret=false]
{
const std::string fnc_nm("where_assign");
var_sct *var_lhs;
var_sct *var_rhs;

}
  :#(EXPR #(ASSIGN vid:. var_rhs=out)) {
//:#(EXPR #(ASSIGN var_lhs=out var_rhs=out)) {
    
   bool bfr=false;
   nco_bool DO_CONFORM;
   //std::string var_nm=vid->getText();
   std::string var_nm;
   NcapVar *Nvar;

   bret=false;

   if(vid->getType()==VAR_ID &&  vid->getFirstChild()  && vid->getFirstChild()->getType()==LMT_LIST )  
     err_prn(fnc_nm, "the \"where statement\" cannot handle a variable with limits on the Left-Hand-Side of an assign statement. Please  remove the limts and try again\nThe variable involved is \""+ vid->getText()+"\"");
                     
   //var_lhs=prs_arg->ncap_var_init(var_nm,true);
   var_lhs=out(vid);
   if(var_lhs==NULL_CEWI) 
     nco_exit(EXIT_FAILURE);
            
   var_nm=std::string(var_lhs->nm); 

   var_rhs=nco_var_cnf_typ(var_lhs->type,var_rhs);         
   if(var_rhs->sz >1L && var_rhs->sz != var_lhs->sz) {
     var_sct *var_tmp=NULL_CEWI;
     //  try and make RHS conform to LHS
     var_tmp=nco_var_cnf_dmn(var_lhs,var_rhs,var_tmp,True,&DO_CONFORM);

     if(DO_CONFORM==False) {
       std::ostringstream os;
       os<<"Cannot make variable:"<<var_lhs->nm <<" and  variable "<<var_rhs->nm <<" conform in where statement.";
       err_prn(fnc_nm,os.str()); 
     }

     if(var_rhs != var_tmp){
       var_rhs=nco_var_free(var_rhs);
       var_rhs=var_tmp;
     }  
   }
  
   // Make mask conform
   if(var_msk->sz != var_lhs->sz){
     var_sct *var_tmp=NULL_CEWI;
     var_tmp=nco_var_cnf_dmn(var_lhs,var_msk,var_tmp,True,&DO_CONFORM);

     if(DO_CONFORM==False) {
       std::ostringstream os;
       os<<"Cannot make variable:"<<var_lhs->nm <<" and where mask variable "<<var_msk->nm <<" conform. ";
       err_prn(fnc_nm,os.str()); 
     }

     if(var_msk != var_tmp){
       //var_msk=nco_var_free(var_msk);
       bfr=true;
       var_msk=var_tmp;
     }
   }

    char *cp_in;
    char *cp_out;
    short *sp;
    long idx;
    long sz;
    size_t slb_sz;

    sz=var_lhs->sz;
    slb_sz=nco_typ_lng(var_lhs->type);

    (void)cast_void_nctype(NC_SHORT,&var_msk->val);
    //Dereference 
    sp=var_msk->val.sp; 

    cp_out=( char*)(var_lhs->val.vp);
    cp_in=( char*)(var_rhs->val.vp);


    if(var_rhs->sz==1L){ 
      for(idx=0; idx<sz; idx++) {
        if(sp[ idx])
          (void)memcpy(cp_out,cp_in,slb_sz);       
        cp_out+=slb_sz;
        } 
    }else{  
        for(idx=0; idx<sz; idx++) {
          if(sp[idx])
            (void)memcpy(cp_out,cp_in,slb_sz);      
          cp_out+=slb_sz;
         cp_in+=slb_sz;
        }
    } 
                    

   (void)cast_nctype_void(NC_SHORT,&var_msk->val); 

   // free "local" copy of var_msk if necessary
   if(bfr)
      var_msk=nco_var_free(var_msk);           

   // Do attribute propagation if LHS is new
   Nvar=prs_arg->var_vtr.find(var_nm);
   if(!Nvar)
      (void)ncap_att_cpy(var_nm,std::string(var_rhs->nm),prs_arg);
              
   var_rhs=nco_var_free(var_rhs);

   prs_arg->ncap_var_write(var_lhs,false);
   bret=true;
        }
;

//Calculate scalar hyperslab where there is a single limit for a possibly
// multi-dimensional variable
var_lmt_one returns [var_sct *var]
{
const std::string fnc_nm("var_lmt_one");
var=NULL_CEWI; 
var_sct *var_nbr;
}
  :#(vid:VAR_ID #(LMT_LIST #(LMT var_nbr=out))) {

            int idx;
            var_sct *var_rhs;
            std::string var_nm;
           
            var_nm=vid->getText(); 
            var_rhs=prs_arg->ncap_var_init(var_nm,false);            
         
            if(var_rhs->undefined){
              var=ncap_var_udf("~rhs_undefined");       
              goto end0;  // cannot use return var!!
            }

            if(prs_arg->ntl_scn){
             var=ncap_sclr_var_mk(var_nm,(nc_type)(var_rhs->type),false);
            }else{
              bool bram;
              int fl_id;
              int nbr_dim=var_rhs->nbr_dim;
              long srt; 
              long srt1[NC_MAX_VAR_DIMS];   
              long sz_dim=1;
              NcapVar *Nvar;

              // create var with space for value
              var=ncap_sclr_var_mk(var_nm,(nc_type)(var_rhs->type),true);

              // we have already checked limit is a single value
              // in the calling action earlier  in the tree parser
              //var_nbr=out(lmt->getFirstChild());

              var_nbr=nco_var_cnf_typ(NC_INT,var_nbr); 
              (void)cast_void_nctype(NC_INT,&var_nbr->val);
              srt=var_nbr->val.ip[0];
              (void)cast_nctype_void(NC_INT,&var_nbr->val);

              // fortran index convention   
              if(prs_arg->FORTRAN_IDX_CNV)
                srt--;
              else if ( srt<0) 
                srt+=var_rhs->sz; // deal with negative index 
 
              // do some bounds checking
              if(srt >= var_rhs->sz || srt<0 )
               err_prn(fnc_nm,"Limit of "+ nbr2sng(srt) +" for variable \""+ var_nm+"\" with size="+nbr2sng(var_rhs->sz)+" is out of bounds\n"); 
            
              // check for RAM variable  
              Nvar=prs_arg->var_vtr.find(var_nm);
              if(Nvar && Nvar->flg_mem)
                bram=true;
              else
               bram=false;
 
              if(bram){
                // deal with RAM variable
                size_t slb_sz;
                slb_sz=nco_typ_lng(var_rhs->type);     

                (void)memcpy(var->val.vp,(const char*)Nvar->var->val.vp+(ptrdiff_t)(srt*slb_sz),slb_sz);
              }else{ 

                // variable in output and defined
                if(Nvar && Nvar->flg_stt==2) {
#ifdef _OPENMP
                  fl_id=( omp_in_parallel() ? prs_arg->out_id_readonly : prs_arg->out_id );
#else    
                  fl_id=prs_arg->out_id;  
#endif      
                // variable output but undefined        
                }else if(Nvar && Nvar->flg_stt==1) {
                  fl_id=prs_arg->in_id;
                  (void)nco_inq_varid(fl_id,var_nm.c_str(),&var_rhs->id);

                // variable in input
                }else{
                  fl_id=prs_arg->in_id; 
                }

                // convert srt into multiple indices  
                for(idx=0;idx<nbr_dim;idx++)
                  sz_dim*= var_rhs->cnt[idx]; 

                for(idx=0; idx<nbr_dim; idx++){                   
                   sz_dim/=var_rhs->cnt[idx];
                   srt1[idx]=srt/sz_dim; 
                   srt-=srt1[idx]*sz_dim;
                }
                (void)nco_get_var1(fl_id,var_rhs->id,srt1,var->val.vp,var_rhs->type);  

               } // end else !bram
 
               // copy missing value if any over             
               nco_mss_val_cp(var_rhs,var);

            } // end else !prs_arg->ntl_scn 

end0:       var_nbr=nco_var_free(var_nbr);
            var_rhs=nco_var_free(var_rhs);   
}
;

//Calculate scalar LHS hyperslab where there is a single limit for a possibly
// multi-dimensional variable

var_lmt_one_lhs[bool bram] returns [var_sct *var]
{
const std::string fnc_nm("var_lmt_one_lhs");
var=NULL_CEWI; 
var_sct *var_nbr;
}
  :#(vid:VAR_ID #(LMT_LIST #(LMT var_nbr=out))) {
               int idx; 
               int slb_sz;
               long srt;

               std::string var_nm;

               var_sct *var_lhs=NULL_CEWI;
               var_sct *var_rhs=NULL_CEWI;
               NcapVar *Nvar; 
           
               var_nm=vid->getText(); 

               if(nco_dbg_lvl_get() > nco_dbg_var) dbg_prn(fnc_nm,var_nm+"(limit)");
           
               Nvar=prs_arg->var_vtr.find(var_nm);

               // calculate single hyperslab limit
               var_nbr=nco_var_cnf_typ(NC_INT,var_nbr); 
               (void)cast_void_nctype(NC_INT,&var_nbr->val);
               srt=var_nbr->val.ip[0];
               (void)cast_nctype_void(NC_INT,&var_nbr->val);
 
              // Overwrite bram possibly 
              if(Nvar) 
                bram=Nvar->flg_mem;
             
              // Deal with RAM variables
              if(bram){
               
                 if(Nvar){
                   //defined but not-populated 
                   if(Nvar->flg_stt==1){ 
                    var_sct *var_ini;
                    var_ini=prs_arg->ncap_var_init(var_nm,true);       
                    Nvar->var->val.vp=var_ini->val.vp;
                    var_ini->val.vp=(void*)NULL;
                    var_ini=nco_var_free(var_ini);
                    Nvar->flg_stt=2; 
                   } 
                   //defined and populated 
                   if(Nvar->flg_stt==2)
                     var_lhs=Nvar->var;                        

                 }else{
                    var_lhs=prs_arg->ncap_var_init(var_nm,true);       
                 }
                  
               // fortran index convention   
               if(prs_arg->FORTRAN_IDX_CNV)
                srt--;
               else if(srt<0) srt+=var_lhs->sz; //deal with negative index convention 
                 
                 // do some bounds checking on single limits
                 if(srt >= var_lhs->sz || srt<0 )
                   err_prn(fnc_nm,"Limit of "+ nbr2sng(srt) +" for variable \""+ var_nm+"\" with size="+nbr2sng(var_lhs->sz)+" is out of bounds\n"); 
      
                 //calculate rhs
                 var_rhs=out(vid->getNextSibling());   

                 // we are only hyperslabbing a single value 
                 if(var_rhs->sz !=1)
                   err_prn(fnc_nm, "Hyperslab for "+var_nm+" - number of elements on LHS(1) doesn't equal number of elements on RHS(" +nbr2sng(var_rhs->sz) +  ")");                                       


                 // Convert to LHS type
                 var_rhs=nco_var_cnf_typ(var_lhs->type,var_rhs);             
               
                 slb_sz=nco_typ_lng(var_lhs->type);     
                 (void)memcpy((char*)var_lhs->val.vp+(ptrdiff_t)(srt*slb_sz),var_rhs->val.vp,slb_sz);
                 
                 if(var_lhs->type==NC_STRING)
                    (void)ncap_sngcpy((char*)var_lhs->val.vp+(ptrdiff_t)(srt*slb_sz),slb_sz);


                 if(!Nvar)
                   (void)prs_arg->ncap_var_write(var_lhs,true); 

              // deal with regular vars 
              }else{

                // if var undefined in O or defined but not populated
                if(!Nvar || ( Nvar && Nvar->flg_stt==1)){              
                  // if var isn't in ouptut then copy it there
                  var_lhs=prs_arg->ncap_var_init(var_nm,true);

                 // copy atts to output
                 (void)ncap_att_cpy(var_nm,var_nm,prs_arg);
                 (void)prs_arg->ncap_var_write(var_lhs,false);
                }
 
                var_lhs=prs_arg->ncap_var_init(var_nm,false);

                    
               // fortran index convention   
               if(prs_arg->FORTRAN_IDX_CNV)
                srt--;
               else if(srt<0) srt+=var_lhs->sz; //deal with negative index convention 
              
                // do some bounds checking on single limits
                if(srt >= var_lhs->sz || srt<0 )
                   err_prn(fnc_nm,"Limit of "+ nbr2sng(srt) +" for variable \""+ var_nm+"\" with size="+nbr2sng(var_lhs->sz)+" is out of bounds\n"); 

                // Grab RHS
                var_rhs=out(vid->getNextSibling());   

                // we are only hyperslabbing a single value 
                if(var_rhs->sz !=1)
                  err_prn(fnc_nm, "Hyperslab for "+var_nm+" - number of elements on LHS(1) doesn't equal number of elements on RHS(" +nbr2sng(var_rhs->sz) +  ")");                                       

                // Convert to LHS type
                var_rhs=nco_var_cnf_typ(var_lhs->type,var_rhs);             

                // swap values about
                var_lhs->val.vp=var_rhs->val.vp; 
                var_rhs->val.vp=(void*)NULL; 

                // write block
                { 
                 int nbr_dim=var_lhs->nbr_dim;
                 long srt1[NC_MAX_VAR_DIMS];   
                 long sz_dim=1; 

                 var_lhs->sz=1;
                 // convert srt into multiple indices  
                 for(idx=0;idx<nbr_dim;idx++)
                   sz_dim*= var_lhs->cnt[idx]; 

                 for(idx=0; idx<nbr_dim; idx++){                   
                   sz_dim/=var_lhs->cnt[idx];
                   srt1[idx]=srt/sz_dim; 
                   srt-=srt1[idx]*sz_dim;
                 }
                
                 for(idx=0;idx<nbr_dim;idx++){
                   var_lhs->srt[idx]=srt1[idx]; 
                   var_lhs->cnt[idx]=1L; 
                   var_lhs->srd[idx]=1L; 
                 } /* end loop over idx */

                 // write slab to O contains call to Open MP critical region
                 //  routine also frees up var_lhs
                 (void)prs_arg->ncap_var_write_slb(var_lhs);     

                }//end write block 
              }     
                   var_rhs=nco_var_free(var_rhs); 
                   var_nbr=nco_var_free(var_nbr); 
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
            bool bnrm;
            int idx;
            int nbr_dmn;
            int fl_id;

            var_sct *var1;
            var_sct *var_rhs;

            std::string var_nm;
           
            NcapVar *Nvar;
            RefAST lRef;           

            NcapVector<lmt_sct*> lmt_vtr;
            NcapVector<dmn_sct*> dmn_vtr;
            NcapVector<std::string> dmn_nrm_vtr;  // list of dimension names

            var_nm=vid->getText(); 
            var_rhs=prs_arg->ncap_var_init(var_nm,false);            
         
            if(var_rhs->undefined){
              var=ncap_var_udf("~rhs_undefined");       
              goto end2;  // cannot use return var!!
            }

            nbr_dmn=var_rhs->nbr_dim;          
            lRef=lmt;

          if(prs_arg->ntl_scn){
            // check limit only contains numbers or dim_id.size()
            std::vector<std::string> str_vtr;
            (void)ncap_mpi_get_id(lRef,str_vtr);
            if(str_vtr.size()>0){
              var=ncap_var_udf("~rhs_undefined");       
              goto end2;  // cannot use return var!!
            }
            // Temporarily change mode 
            prs_arg->ntl_scn=False;
            lmt_mk(nbr_dmn,lRef,lmt_vtr);
            prs_arg->ntl_scn=True; 

          }else{
            lmt_mk(nbr_dmn,lRef,lmt_vtr);
           }

         if( lmt_vtr.size() != nbr_dmn)
            err_prn(fnc_nm,"Number of hyperslab limits for variable "+ var_nm+" doesn't match number of dimensions");

          // add dim names to dimension list 
          for(idx=0 ; idx < nbr_dmn;idx++)
            lmt_vtr[idx]->nm=strdup(var_rhs->dim[idx]->nm);   
                        
          // fill out limit structure
           for(idx=0 ; idx < nbr_dmn ;idx++)
            (void)ncap_lmt_evl(var_rhs->nc_id,lmt_vtr[idx],-1,prs_arg);

          // See if var can be normalized
           for(idx=0; idx<nbr_dmn ; idx++){
             if(lmt_vtr[idx]->cnt==1) continue;

             if(lmt_vtr[idx]->cnt == var_rhs->dim[idx]->cnt) 
               dmn_nrm_vtr.push_back(std::string(lmt_vtr[idx]->nm));
             else
               break;
           } 

           bnrm= (idx==nbr_dmn ? true:false);       

           // deal more with inital scan 
           if(prs_arg->ntl_scn){

             if(bnrm){   
               
               var=ncap_cst_mk(dmn_nrm_vtr,prs_arg);

               (void)nco_free(var->nm);
               var->nm=strdup(var_nm.c_str());

               var=nco_var_cnf_typ(var_rhs->type,var);
                
               // apply LHS cast if necessary 
               if(var->sz>1 && bcst && var_cst) 
                 var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);
              }else{
                var=ncap_var_udf("~rhs_undefined");             
              }
             goto end1;
           }           

           /**** From here on we are dealing with a final scan  ****/
           // copy lmt_sct to dmn_sct;
           for(idx=0 ;idx <nbr_dmn ; idx++){
              dmn_sct *dmn_nw;
              dmn_nw=(dmn_sct*)nco_calloc(1,sizeof(dmn_sct));
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

          Nvar=prs_arg->var_vtr.find(var_nm);
          if(Nvar && Nvar->flg_mem)
            bram=true;
          else
            bram=false;
 
          // Ram variable -do an in memory get  
          if(bram){
            var=prs_arg->ncap_var_init(var_nm,true);                         
            //Do an in memory get 
            (void)nco_get_var_mem(var,dmn_vtr);
            // put values from dmn_vtr back into var
            // nb above call has already calculated correct value for
            // var->sz;
            for(idx=0;idx<nbr_dmn ;idx++){
                var->srt[idx]=dmn_vtr[idx]->srt;
                var->end[idx]=dmn_vtr[idx]->end;
                var->cnt[idx]=dmn_vtr[idx]->cnt;
                var->srd[idx]=dmn_vtr[idx]->srd;
            }
          // regular variable
          }else{
 
            // variable in output 
           if(Nvar){
#ifdef _OPENMP
             fl_id=( omp_in_parallel() ? prs_arg->out_id_readonly : prs_arg->out_id);
#else    
             fl_id=prs_arg->out_id;  
#endif      
            // variable in input         
            }else{ 
             fl_id=prs_arg->in_id;
            }
 
            // Fudge -- fill out var again -but using dims defined in dmn_vtr
            // We need data in var so that LHS logic in assign can access var shape 
            var=nco_var_fll(fl_id,var_rhs->id,var_nm.c_str(), &dmn_vtr[0],dmn_vtr.size()); 
            //var->sz*=2;
            // Now get data from disk - use nco_var_get() 
            (void)nco_var_get(fl_id,var); 

           } // end if(nbram)
           
           // copy missing value over
           nco_mss_val_cp(var_rhs,var);
           
          /* a hack - we set var->has_dpl_dmn=-1 so we know we are dealing with 
             a hyperslabbed var and not a regular var  -- It shouldn't cause 
             any abberant behaviour!! */ 
           var->has_dpl_dmn=-1;  

            // if variable is scalar re-organize in a new var 
           // loose extraneous material so it looks like a
           // plain scalar variable
           if(var->sz ==1) {
              
             var1=ncap_sclr_var_mk(var_nm,var->type,true);
             
             (void)memcpy( (void*)var1->val.vp,var->val.vp,nco_typ_lng(var1->type));
             if(var1->type==NC_STRING)
                 (void)ncap_sngcpy((char*)var1->val.vp, nco_typ_lng(var1->type) );
             /*
             if(var->type==NC_STRING){
                 cast_void_nctype(NC_STRING, &var->val); 
                 var1->val.sngp[0]=(nco_string)strdup(var->val.sngp[0]);
                 cast_nctype_void(NC_STRING, &var->val); 
             }else{
             
           }
           */  
             
             // copy missing value if any from var_rhs to var1
             nco_mss_val_cp(var_rhs,var1);
           
             // free main var
             var=nco_var_free(var);

             var=var1;

            // if hyperslab -nomalizable 
            // nb the returned var is just like a regular var 
            }else if(bnrm) {

              // nb dmn_nrm_vtr was populated much earlier in function
              var1=ncap_cst_mk(dmn_nrm_vtr,prs_arg);
              (void)nco_free(var1->nm);
                
              var1->nm=strdup(var_nm.c_str());
              var1=nco_var_cnf_typ(var_rhs->type,var1);

              // copy missing value if any from var_rhs to var
              nco_mss_val_cp(var_rhs,var1);
                
              // swap values about in var1 & var 
              var1->val.vp=var->val.vp;

              var->val.vp=(void*)NULL;       

              // free var  
              (void)nco_var_free(var);    
                
              /* Casting a hyperslab --this makes my brain  hurt!!! 
              if the var is already the correct size then do nothing 
               what not even dimension reordering ?  */  
              if(bcst && var_cst &&  var1->sz != var_cst->sz)
                 var1=ncap_cst_do(var1,var_cst,prs_arg->ntl_scn);
              
              var=var1;
                
           }else{

           /* A sophisticated hack, var->dim currently contains 
              the local dims from dmn_vtr . We cannot leave them in 
              place as they will never be freed up, and if the var is 
              copied then complications will result. 
              So we replace them with the regular dims in
              prs_arg->dmn_out_vtr.

              Currently only one function explicitly references these dims
              This is the agg_cls::fnd -- The  Aggregate Functions
              
              Unpredicable behaviour will result when this hyperslabbed var 
              is passed to other functions
               
           */                      
           for(idx=0 ; idx<nbr_dmn; idx++)
              var->dim[idx]=prs_arg->dmn_out_vtr.find(dmn_vtr[idx]->nm);  

           }   

          //free vectors
          for(idx=0 ; idx < nbr_dmn ; idx++)
             (void)nco_dmn_free(dmn_vtr[idx]); 
               ;

          end1: ;
          for(idx=0 ; idx < nbr_dmn ; idx++)
            (void)nco_lmt_free(lmt_vtr[idx]);
          
          end2: var_rhs=nco_var_free(var_rhs); 
    }
;





