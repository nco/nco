header {
    #include <math.h>
    #include <malloc.h>
    #include <assert.h>
    #include <ctype.h>
    #include <iostream>
    #include <sstream>
    #include <string>
    #include <algorithm>
    #include "ncap2.hh"
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
    SQR2;
    PROP; // used to differenciate properties & methods
    
}

program:
        (statement)*
    ;

statement:
        block
        | if_stmt
        | assign_statement
        | def_dim
        //deal with empty statement
        | SEMI! { #statement = #([ NULL_NODE, "null_stmt"]); } 
   ;        


// a bracketed block
block:
    LCURL! (statement)* RCURL!
    { #block = #( [BLOCK, "block"], #block ); }
    ;


if_stmt:
    IF^ LPAREN! expr RPAREN! statement 
   ( //standard if-else ambiguity
    options {
             warnWhenFollowAmbig = false;
                        } :
   ELSE statement )?
     ;


assign_statement:
         expr SEMI!
       {#assign_statement = #(#[EXPR,"EXPR"],#assign_statement); }

     ; 

hyper_slb: (VAR_ID^ |ATT_ID ^) (lmt_list|dmn_list)?
     ;

//cast_slb:  (VAR_ID^ |ATT_ID^ ) dmn_list
//     ;

def_dim:   DEFDIM^ LPAREN! NSTRING COMMA! expr RPAREN! SEMI! 
     ;        

arg_list: expr|dmn_arg_list|DIM_ID|DIM_MTD_ID
     ;

func_arg:  LPAREN! (arg_list)? (COMMA! arg_list)*   RPAREN! 
      { #func_arg = #( [FUNC_ARG, "func_arg"], #func_arg ); }
     ;   


// left association
//func_exp: primary_exp | ( FUNC^ LPAREN!  expr RPAREN! )
//    ;

func_exp: primary_exp | ( FUNC^ func_arg )
      ;



// dot operator -- properties
// not sure this is best place for this rule
meth: ( PAVG|PAVGSQR|PMAX|PMIN|PRMS|PRMSSDN|PSQR|ARVG|PTTL)
    ;


prop_exp: func_exp (node:DOT^ {#node->setType(PROP);
                               #node->setText("property");}(PSIZE|PTYPE|PNDIMS))?
    ;

// Method call all methods must in include () otherwise
// they will be treated as a property 
meth_exp: prop_exp ( DOT^ meth func_arg )*
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
unary_exp:  ( LNOT^| PLUS^| MINUS^ |INC^ | DEC^ | TIMES^ ) unary_exp
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

expr:   ass_expr
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
    
primary_exp
    : (LPAREN! expr RPAREN! ) 
    | FLOAT    
    | DOUBLE
    | INT
    | BYTE
    | SHORT
    | USHORT
    | UINT
    | INT64
    | UINT64
    | NSTRING    
    | DIM_ID_SIZE
    | hyper_slb  //remember this includes VAR_ID & ATT_ID
  ;


	  
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

// Properties 
    PSIZE="size";
    PTYPE="type";
    PNDIMS="ndims";
// Methods
    PAVG="avg";
    PAVGSQR="avgsqr";
    PMAX="max";
    PMIN="min";
    PRMS="rms";
    PRMSSDN="rmssdn";
    PSQRAVG="sqravg";
    PTTL="total";
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
            {// check function table
            sym_sct *sym_srh;
            sym_srh=ncap_sym_init($getText.c_str(),cos,cosf); 

            if( prs_arg->ptr_sym_vtr->bsearch(sym_srh))
               $setType(FUNC);             
             else $setType(VAR_ID); 
             
            (void)nco_free(sym_srh->nm);
           (void)nco_free(sym_srh);

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
    prs_sct *prs_arg;
    bool bcst;
    var_sct* var_cst;
public:
    void setTable(prs_sct *prs_in){
        prs_arg=prs_in;
    }
    // Customized Constructor
    ncoTree(prs_sct *prs_in){
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
         (void)cast_void_nctype(NC_INT,&var_out->val);

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



public:
    void run(RefAST tr){
        while(tr) {
          (void)statements(tr);   
          tr=tr->getNextSibling();   
        }
    }

public:
    void run_dbl(RefAST tr,int icnt){
     int idx=0;
     RefAST ntr=tr;
      
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
    (void)ncap_def_ntl_scn(prs_arg);
    (void)nco_enddef(prs_arg->out_id);  


    // Sort expressions - MPI preparation
    if(prs_arg->NCAP_MPI_SORT){  
       prs_arg->ntl_scn=False;
      // nb A vector of vectors
      std::vector< std::vector<RefAST> > all_ast_vtr;

      // Populate  vector 
      (void)ncap_mpi_srt(tr,icnt,all_ast_vtr);

      // Evaluate expressions
      for(unsigned vtr_idx=0 ; vtr_idx<all_ast_vtr.size(); vtr_idx++)
        for(unsigned jdx=0 ; jdx<all_ast_vtr[vtr_idx].size(); jdx++)
	     (void)statements(all_ast_vtr[vtr_idx][jdx]);
        
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
    void run_exe(RefAST tr){
    // number of statements in block
    int nbr_stmt=0;
    int idx;
    int icnt=0;
    int gtyp;
    
    RefAST etr=ANTLR_USE_NAMESPACE(antlr)nullAST;
    RefAST ntr;
   
    ntr=tr;
    do nbr_stmt++; 
    while(ntr=ntr->getNextSibling());        
            
     
    
    if(nbr_stmt <4){
        prs_arg->ntl_scn=False;
        ntr=tr;
        do (void)statements(ntr);   
        while(ntr=ntr->getNextSibling());   
        goto exit;
    }
  
    ntr=tr;

    for(idx=0 ; idx < nbr_stmt; idx++){
      gtyp=ntr->getType();
      // we have hit an IF or a code block
      if(gtyp==BLOCK || gtyp==IF ||gtyp==DEFDIM ) {
        if(icnt>0) 
         (void)run_dbl(etr,icnt);
        icnt=0;
        etr=ANTLR_USE_NAMESPACE(antlr)nullAST;; 
        prs_arg->ntl_scn=False;
        (void)statements(ntr);      
       }

       if(gtyp==EXPR || gtyp== NULL_NODE) 
        if(icnt++==0) etr=ntr;
        
       
      ntr=ntr->getNextSibling();
      
    } // end for
    if(icnt >0)
       (void)run_dbl(etr,icnt);      

      
exit: ;     
            

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



statements 
{
var_sct *var;

}
    : blk:BLOCK { 
       //cout <<"Num of Children in block="<<blk->getNumberOfChildren()<<endl;
       run_exe(blk->getFirstChild());
            
                }

    | exp:EXPR {
       var=out(exp->getFirstChild());
       var=nco_var_free(var);
      }
              

    | iff:IF {
    //if can have only 3 or 5 parts  , 1 node and 2 or 4 siblings
    // IFF LOGICAL_EXP STATEMENT1 ELSE STATEMENT2
      bool br;
      var_sct *var1;
      RefAST dref;
	  //Calculate logical expression
	  var1= out( iff->getFirstChild());
	  br=ncap_var_lgcl(var1);
	  var1=nco_var_free(var1);
      
      dref=iff->getFirstChild()->getNextSibling();

      if(br){ 
         // Execute 3rd sibling  
         if(dref->getType()==BLOCK)
           run_exe(dref->getFirstChild());
         else
           statements(dref);     
            
	  }else{ 
      // See if else exists (third sibling)
         dref=iff->getFirstChild()->getNextSibling()->getNextSibling(); 
         if(dref && dref->getType()==ELSE ){
           // Execute 4th sibling
           if(dref->getNextSibling()->getType()==BLOCK)
             run_exe(dref->getNextSibling()->getFirstChild());
           else
             statements(dref->getNextSibling());     
             
         }
     }
 
      var=NULL_CEWI;
      
    }// end action


    | ELSE {

      }
    
    | def:DEFDIM {
            
        const char *dmn_nm;
        long sz;
            
        dmn_nm=def->getFirstChild()->getText().c_str();
            
        var=out(def->getFirstChild()->getNextSibling());    
        var=nco_var_cnf_typ(NC_INT,var);

        (void)cast_void_nctype(NC_INT,&var->val);
        sz=*var->val.lp;
        var=(var_sct*)nco_var_free(var);
        (void)ncap_def_dim(dmn_nm,sz,prs_arg);
     }
             

    | NULL_NODE {
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

               var_lhs=ncap_var_init(vid->getText(),prs_arg,false);
               if(var_lhs){
                 var=nco_var_dpl(var_lhs);
                 (void)ncap_var_write(var_lhs,bram,prs_arg);
               } else {

                 // set var to udf
                 var_lhs=ncap_var_udf(var_nm);
                 var=nco_var_dpl(var_lhs);
 
                 Nvar=new NcapVar(var_lhs);
                 (void)prs_arg->ptr_int_vtr->push_ow(Nvar);
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
                if(   prs_arg->ptr_dmn_in_vtr->findi(str_vtr[idx]) ==-1             
                   && prs_arg->ptr_dmn_out_vtr->findi(str_vtr[idx]) ==-1)      
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
                (void)prs_arg->ptr_int_vtr->push_ow(Nvar);
                var=nco_var_dpl(var1);
              } else{
                var->nm=(char*)nco_free(var->nm);
                var->nm=strdup(var_nm);
                var1=nco_var_dpl(var);
                ncap_var_write(var1,bram,prs_arg);
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
               (void)ncap_var_write(var1,bram,prs_arg);
               
        
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
        prs_arg->ptr_int_vtr->push_ow(Nvar);          

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
               const char *var_nm;
               
              if(dbg_lvl_get() > 0)
                dbg_prn(fnc_nm,vid->getText()+"(limits)");


               RefAST lmt_Ref;
               var_sct *var_lhs;
               var_sct *var_rhs;
               NcapVector<lmt_sct*> lmt_vtr;          
               
               lmt_Ref=vid->getFirstChild();

               bcst=false;
               var_cst=NULL_CEWI;
               var=NULL_CEWI;
               NcapVar *Nvar; 
               
              

              
               var_nm=vid->getText().c_str();

               lmt_Ref=lmt;               

               Nvar=prs_arg->ptr_var_vtr->find(var_nm);
               //set ram flag to flag in var_vtr 
               if(Nvar) bram=Nvar->flg_mem;


              // if var undefined in O or defined but not populated
               if(Nvar && !bram && Nvar->flg_stt==1){              
                  // if var isn't in ouptut then copy it there
                 var_lhs=ncap_var_init(vid->getText(),prs_arg,true);

                 // copy atts to output
                 (void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
                 (void)ncap_var_write(var_lhs,false,prs_arg);
               }

               if(Nvar==NULL && !bram){ 
                 var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
                 // copy atts to output
                 (void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
                 (void)ncap_var_write(var_lhs,false,prs_arg);
               }

                if(Nvar==NULL && bram){
                  std::cout<<"Nvar==NULL && bram)\n";
                  var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
                  (void)ncap_var_write(var_lhs,true,prs_arg);
                }
               

               // Get "new" var_id   
               if(!bram){
                 (void)nco_inq_varid(prs_arg->out_id,var_nm,&var_id);
                 var_lhs=ncap_var_init(vid->getText(),prs_arg,false);
               }

               if(bram)
                 var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
                   
               nbr_dmn=var_lhs->nbr_dim;

               // Now populate lmt_vtr;
               if( lmt_mk(lmt_Ref,lmt_vtr) == 0){
                  err_prn(fnc_nm,"Invalid hyperslab limits for variable "+ vid->getText());
               }

               if( lmt_vtr.size() != nbr_dmn){
                  err_prn(fnc_nm,"Number of hyperslab limits for variable "+ vid->getText()+" doesn't match number of dimensions");
               }

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

          
              // Now ready to put values 
              
              
             // Put RAM var
              if(bram){
                var_sct *var_tst;
                var_tst=ncap_var_init(vid->getText(),prs_arg,true);
                (void)nco_put_var_mem(var_tst,var_rhs,lmt_vtr);
                (void)ncap_var_write(var_tst,true,prs_arg); 
              }
              

              // Regular var
              if(!bram){ 
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
              var_rhs=nco_var_free(var_rhs);

              //get variable again from disk!! for return value
              var=ncap_var_init(vid->getText(),prs_arg,true);

               
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
              
              // deal with rhs attribute or rhs hyperslab              
              if( ncap_var_is_att(var1)|| var1->has_dpl_dmn==-1) {
                if(var1->sz ==1 )
                  var1=ncap_cst_do(var1,var_cst,prs_arg->ntl_scn);
                else if( var1->sz==var_cst->sz ) {
                  ptr_unn val_swp;  // Used to swap values around       
              
                  var_cst=nco_var_cnf_typ(var1->type,var_cst);
                  (void)ncap_att_stretch(var1,var_cst->sz);
 
                  val_swp=var_cst->val; 
                  var_cst->val=var1->val;
                  var1->val=val_swp;
                  
                  var1=nco_var_free(var1);
                  var1=nco_var_dpl(var_cst);                  
                  
                  }                                       
                else
                  err_prn(fnc_nm, "LHS cast for "+vid1->getText()+" - cannot make RHS attribute "+ std::string(var1->nm) + " conform."); 
              
              // deal with rhs bare number && rhs hyperslab with single element
              } else if(var1->sz ==1 )
                  var1=ncap_cst_do(var1,var_cst,prs_arg->ntl_scn);
     
              var1->nm=(char*)nco_free(var1->nm);

              var1->nm =strdup(vid1->getText().c_str());

              //Copy return variable<
              var=nco_var_dpl(var1);
              
              //call to nco_var_get() in ncap_var_init() uses this property
              var1->typ_dsk=var1->type;
              (void)ncap_var_write(var1,bram,prs_arg);

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
               Nvar=prs_arg->ptr_var_vtr->find(vid2->getText());
               //rcd=nco_inq_varid_flg(prs_arg->out_id,var1->nm ,&var_id);

              
               if(!Nvar || Nvar && Nvar->flg_stt==1)
                 (void)ncap_att_cpy(vid2->getText(),s_var_rhs,prs_arg);
                               

               //Copy return variable
               var=nco_var_dpl(var1);
               // Write var to disk
               (void)ncap_var_write(var1,bram,prs_arg);
                         
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
            prs_arg->ptr_var_vtr->push_ow(Nvar);       

            // Copy return variable
            var=nco_var_dpl(var1);    
                  
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
    |  (#(TIMES #(POST_INC out)))=> #( TIMES #(POST_INC var1=out_asn)){
             var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,true,prs_arg);      
       } 
    |  (#(TIMES #(POST_DEC out)))=> #( TIMES #(POST_DEC var1=out_asn)){
             var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,true,prs_arg);      
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

    //ternary Operator
    |   #( qus:QUESTION var1=out) {
           bool br;
            
           // if initial scan 
           if(prs_arg->ntl_scn){
               var1=nco_var_free(var1);
               var=ncap_var_udf("~question"); 
               return var;
           }

           br=ncap_var_lgcl(var1);
           var1=nco_var_free(var1);
           
           if(br) 
             var=out(qus->getFirstChild()->getNextSibling());
           else
             var=out(qus->getFirstChild()->getNextSibling()->getNextSibling());      
         }   
    

    // math functions 
    |  #(m:FUNC #(FUNC_ARG var1=out))      
         { 
          sym_sct * sym_ptr;
            
          sym_ptr= prs_arg->ptr_sym_vtr->find(m->getText());
          if(sym_ptr ==NULL) { 
              cout << "Function  " << m->getText() << " not found" << endl;
              exit(1);
           } 
           // Catch delete function
           if(m->getText() == "delete"){
               ;
    
           } else     
           var=ncap_var_fnc(var1,sym_ptr);

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
       var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN ,(pls_asn->getType()==TIMES), prs_arg);
       }
	| #(MINUS_ASSIGN min_asn:. var2=out){
       var1=out_asn(min_asn);
       var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN ,(min_asn->getType()==TIMES), prs_arg);
       }
	| #(TIMES_ASSIGN tim_asn:. var2=out){       
       var1=out_asn(tim_asn);
       var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN ,(tim_asn->getType()==TIMES), prs_arg);
       }
	| #(DIVIDE_ASSIGN div_asn:. var2=out){	
       var1=out_asn(div_asn);
       var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN ,(div_asn->getType()==TIMES), prs_arg);
       }

    | #(ASSIGN asn:. ) 
            {
              // Check for RAM variable - if present 
              // change tree - for example from:
              //     ( EXPR ( = ( * n1 ) ( + four four ) ) )
              // to  ( EXPR ( = n1 ( + four four ) ) )
             RefAST tr;
             bool bram;
             if(asn->getType()==TIMES){
               tr=asn->getFirstChild();
               tr->setNextSibling(asn->getNextSibling());
               bram=true;
             } else { 
               tr=asn; 
               bram=false;
             }

             if(prs_arg->ntl_scn)
               var=assign_ntl(tr,bram); 
             else
               var=assign(tr,bram);
               
            }  

    // The following properties are shared by vars & atts
    | #(PROP var1=out prp:.) {

       if(prs_arg->ntl_scn){
         var1=nco_var_free(var1);
         var=ncap_sclr_var_mk(static_cast<std::string>("~property"),NC_INT,false);        
       } else { 

         switch(prp->getType()){ 
           case PSIZE:
             var=ncap_sclr_var_mk(static_cast<std::string>("~property"),(nco_int)var1->sz);
             break;
           case PTYPE:
             var=ncap_sclr_var_mk(static_cast<std::string>("~property"),(nco_int)var1->type);
             break;
           case PNDIMS:
             var=ncap_sclr_var_mk(static_cast<std::string>("~property"),(nco_int)var1->nbr_dim);            

          } // end switch
         var1=nco_var_free(var1); 
      }
    } 

    // These methods are only for vars
    | #(DOT var1=out mtd:. args:FUNC_ARG) {
      
            int nbr_arg;
            int idx;
            int nbr_dim;
            dmn_sct **dim;
           
            RefAST aRef;
            std::vector<std::string> str_vtr;
            NcapVector<dmn_sct*> dmn_vtr;
            // de-reference 
            ddra_info_sct ddra_info;        
            
            // blow out if unrecognized method
            if(mtd->getType()==VAR_ID){
              std::string serr;
              std::string sva(ncap_var_is_att(var1)?"Attribute" :"Variable");
              serr= sva+" " + std::string(var1->nm)+ " has unrecognized method "+ "\""+mtd->getText()+"\"";
              err_prn(fnc_nm,serr );
            }
            
            // Process method arguments if any exist !! 
            if(args && (nbr_arg=args->getNumberOfChildren()) >0){  
                aRef=args->getFirstChild();
                while(aRef){
                    
                    switch(aRef->getType()){
                    case DIM_ID: 
                    case DIM_MTD_ID:{  
                            str_vtr.push_back(aRef->getText());
                            break;    
                        }
                        
                        // This is garanteed to contain at least one DIM_ID or DIM_MTD  
                        // and NOTHING else --no need to type check!!
                    case DMN_ARG_LIST: 
                        { RefAST bRef=aRef->getFirstChild();
                            while(bRef){
                                str_vtr.push_back(bRef->getText());
                                bRef=bRef->getNextSibling();
                            }  
                            break;
                        } 
                        
                        // ignore expr type argument
                    default:
                        std::string serr;
                        serr="Argument \""+aRef->getText()+"\" to method "+mtd->getText()+" is not a dimension";      
                        wrn_prn(fnc_nm,serr);
                        break;
                        
                    } // end switch
                    aRef=aRef->getNextSibling();
                } // end while
                
                dmn_vtr=ncap_dmn_mtd(var1, str_vtr);
            }           
            
            // Initial scan 
            if(prs_arg->ntl_scn){
                nbr_dim=var1->nbr_dim;
                
                if(var1->undefined)
                var=ncap_var_udf("~dot_methods");  
                // deal with average over all dims or scalar var
                else if( nbr_dim==0 || dmn_vtr.size()== 0 || dmn_vtr.size()==nbr_dim)  
                var=ncap_sclr_var_mk(static_cast<std::string>("~dot_methods"),var1->type,false);    
                else {
                    // cast a variable with the correct dims in the correct order
                    dim=var1->dim;
                    std::vector<std::string> cst_vtr;              
                    
                    for(idx=0 ; idx < nbr_dim ; idx++){
                        std::string sdm(dim[idx]->nm);    
                        if( dmn_vtr.findi(sdm) == -1)
                        cst_vtr.push_back(sdm);       
                    }                
                    
                    var=ncap_cst_mk(cst_vtr,prs_arg);
                    var=nco_var_cnf_typ(var1->type,var);
                }
                
                var1=nco_var_free(var1);
                // NB: cannot use return -- as this results with
                // problems with automagically generated code 
                goto end_dot;
            }
            
            if(dmn_vtr.size() >0){
                dim=&dmn_vtr[0];
                nbr_dim=dmn_vtr.size();                           
            } else {
                dim=var1->dim;
                nbr_dim=var1->nbr_dim; 
            }    
            
            // Final scan
            if(!prs_arg->ntl_scn){
                
                switch(mtd->getType()){
                    
                case PAVG:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_avg,False,&ddra_info);
                    // Use tally to normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    break;
                    
                case PAVGSQR:
                    var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_avgsqr,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    break;
                    
                case PMAX:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_max,False,&ddra_info);
                    break;
                    
                case PMIN:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_min,False,&ddra_info);
                    break; 
                    
                case PRMS:
                    var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_rms,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Take root
                    (void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
                    break;
                    
                case PRMSSDN:
                    var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_rmssdn,False,&ddra_info);
                    // Normalize
                    (void)nco_var_nrm_sdn(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Take root
                    (void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
                    break;
                    
                case PSQRAVG:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_sqravg,False,&ddra_info);
                    // Normalize 
                    (void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
                    // Square mean
                    (void)nco_var_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,var->val);
                    break;
                    
                case PTTL:
                    var=nco_var_avg(var1,dim,nbr_dim,nco_op_ttl,False,&ddra_info);
                    break;
                } 
                // var1 is freed in nco_var_avg()
            }
            
            end_dot: ;
            
        } // end action
        
        // Naked numbers: Cast is not applied to these numbers
    |   val_float:FLOAT        
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~float"),NC_FLOAT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~float"),static_cast<float>(std::strtod(val_float->getText().c_str(),(char **)NULL)));} // end FLOAT
    |   val_double:DOUBLE        
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~double"),NC_DOUBLE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~double"),strtod(val_double->getText().c_str(),(char **)NULL));} // end DOUBLE
	|	val_int:INT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int"),NC_INT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int"),static_cast<nco_int>(std::strtol(val_int->getText().c_str(),(char **)NULL,10)));} // end INT
	|	val_short:SHORT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~short"),NC_SHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~short"),static_cast<nco_short>(std::strtol(val_short->getText().c_str(),(char **)NULL,10)));} // end SHORT
    |	val_byte:BYTE			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),NC_BYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),static_cast<nco_byte>(std::strtol(val_byte->getText().c_str(),(char **)NULL,10)));} // end BYTE
// #ifdef ENABLE_NETCDF4
	|	val_ubyte:UBYTE			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),NC_UBYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),static_cast<nco_ubyte>(std::strtoul(val_ubyte->getText().c_str(),(char **)NULL,10)));} // end UBYTE
	|	val_ushort:USHORT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),NC_USHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),static_cast<nco_ushort>(std::strtoul(val_ushort->getText().c_str(),(char **)NULL,10)));} // end USHORT
	|	val_uint:UINT			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),NC_UINT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),static_cast<nco_uint>(std::strtoul(val_uint->getText().c_str(),(char **)NULL,10)));} // end UINT
	|	val_int64:INT64			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),NC_INT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),static_cast<nco_int64>(std::strtoll(val_int64->getText().c_str(),(char **)NULL,10)));} // end INT64
	|	val_uint64:UINT64			
        {if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),NC_UINT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),static_cast<nco_uint64>(std::strtoull(val_uint64->getText().c_str(),(char **)NULL,10)));} // end UINT64
// #endif /* !ENABLE_NETCDF4 */
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
                (void)cast_void_nctype(NC_CHAR,&var->val);
                strncpy(var->val.cp,tsng,(size_t)var->sz);  
                (void)cast_nctype_void(NC_CHAR,&var->val);
            }
            tsng=(char*)nco_free(tsng);      
        }
    |   dval:DIM_ID_SIZE
        {
            string sDim=dval->getText();
            dmn_sct *dmn_fd;
            
            // check output
            if(prs_arg->ntl_scn){
                var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),NC_INT,false);
            }else{ 
                // Check output 
                dmn_fd=prs_arg->ptr_dmn_out_vtr->find(sDim);
                // Check input
                if(dmn_fd==NULL_CEWI)
                dmn_fd=prs_arg->ptr_dmn_in_vtr->find(sDim);
                
                if( dmn_fd==NULL_CEWI ){
                    err_prn(fnc_nm,"Unable to locate dimension " +dval->getText()+ " in input or output files ");
                }
                var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),dmn_fd->sz);
            } // end else 
        } // end action 
        
        // Variable with argument list 
    | (#(VAR_ID LMT_LIST)) => #( vid:VAR_ID lmt:LMT_LIST) {
            bool bram;   // Check for a RAM variable
            int idx;
            int nbr_dmn;
            const char *var_nm;
            var_sct *var_rhs;
          var_sct *var_nw;
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
          var_rhs=ncap_var_init(vid->getText(),prs_arg,false);            
          nbr_dmn=var_rhs->nbr_dim;          
          lRef=lmt;

          // Check for RAM variable
          Nvar=prs_arg->ptr_var_vtr->find(var_nm);
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
              var_nw=nco_get_var_mem(var_rhs,dmn_vtr);
                //var_nw=nco_var_dpl(var_rhs);
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
    // plain Variable
	|   v:VAR_ID       
        { 

          var=ncap_var_init(v->getText(), prs_arg,true);
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
              Nvar=prs_arg->ptr_int_vtr->find(att->getText());

            if(Nvar==NULL) 
              Nvar=prs_arg->ptr_var_vtr->find(att->getText());

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

     // Value list -- stuff values into var which is attribute
     |   vlst:VALUE_LIST {

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


// Return a var or att WITHOUT applying a cast 
// and checks that the operand is a valid Lvalue
// ie that the var or att has NO children!!
out_asn returns [var_sct *var]
{
const std::string fnc_nm("assign_asn");
var=NULL_CEWI; 
}

   : #(TIMES vid1:VAR_ID)
        { 
          if(vid1->getFirstChild())
               err_prn(fnc_nm,"Invalid Lvalue " +vid1->getText() );

          var=ncap_var_init(vid1->getText(),prs_arg,true);
          if(var== NULL_CEWI){
               nco_exit(EXIT_FAILURE);
          }

        }
	|   vid:VAR_ID       
        { 
          if(vid->getFirstChild())
               err_prn(fnc_nm,"Invalid Lvalue " +vid->getText() );

          var=ncap_var_init(vid->getText(),prs_arg,true);
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
              Nvar=prs_arg->ptr_int_vtr->find(att->getText());

            if(Nvar==NULL) 
              Nvar=prs_arg->ptr_var_vtr->find(att->getText());

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
