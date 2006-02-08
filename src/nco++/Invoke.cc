/*
 * Different version of the calculator which parses the command line arguments.
 * To do this the argv[] strings are first written to a ostringstream then
 * a istringstream is constructed with the string from the ostringstream and
 * fed to the lexer.
 */
#include <stdio.h>
#include <malloc.h>
#include <fstream>
#include <antlr/AST.hpp>
#include "ncoParserTokenTypes.hpp"
#include "ncoLexer.hpp"
#include "ncoParser.hpp"
#include "ncoTree.hpp"

#include "ncap2.hh"

// Forward Declaration
int switchboard(RefAST t,ncoTree &walker);

struct ncoParserTokenTypes TokenTypes;

int parse_antlr(prs_sct *prs_arg,char* fl_spt_usr,char *cmd_ln_sng)
{

ANTLR_USING_NAMESPACE(std);
ANTLR_USING_NAMESPACE(antlr);

int idx;
char *filename;
ifstream *in;
RefAST t,a;
bool bchk;


    filename=fl_spt_usr;   
    in=new ifstream(filename);

	
	try {
	  ncoLexer lexer(*in, prs_arg);
	  lexer.setFilename(filename);
           
	  ncoParser parser(lexer);
	  parser.setFilename(filename);

	  ASTFactory ast_factory;
	  parser.initializeASTFactory(ast_factory);
	  parser.setASTFactory(&ast_factory);


	  // Parse the input expressions
	  parser.program();
	  a = parser.getAST();
	  t=a;
	  //print the parse tree
	   while( t ) {
		cout << t->toStringTree() << endl;
		t=t->getNextSibling();
	   }
	   printf("Paser tree printed\n");

	}  catch(std::exception& e) {
	cerr << "exception: " << e.what() << endl;
	}

	
	t=a;

	try {   
	  ncoTree walker(prs_arg);
          //walker.setTable(prs_arg);

	  printf("Tree walk completed\n");
	  (void)switchboard(t,walker);
	}  catch(std::exception& e) {
	cerr << "exception: " << e.what() << endl;
	}	
        cout<< "Switchboard complete\n";
        return 1;


}              

	
int switchboard(RefAST t, ncoTree &walker) {
bool br;;
var_sct *var;

RefAST a;
	while(t) {
	  switch (t->getType()) { 

	  case TokenTypes.BLOCK:
	    printf("Type BLOCK\n");
	      switchboard(t->getFirstChild(),walker);
	    break;

	  case TokenTypes.ASSIGN:
	    (void)walker.assign(t); 
	    cout << "Type ASSIGN " <<  t->getFirstChild()->getText() <<endl;;
	    break;


	  case TokenTypes.IF:
	    //Calculate logical expression
	    var= walker.out( t->getFirstChild());
	    br=ncap_var_lgcl(var);
	    var=nco_var_free(var);
	    printf("conditional= %d\n", br);
	    
            // See if else is present
	    a=t->getFirstChild()->getNextSibling()->getNextSibling();

	    if( a && a->getType() == TokenTypes.ELSE) 
	       printf("Else exists\n");

	      
	    if(br) switchboard(t->getFirstChild()->getNextSibling(),walker );
	      else if(a) { switchboard(a->getFirstChild(),walker);}

	    break;
	  

	  case TokenTypes.ELSE:
	    //exit function !
	     return 0;
	     break;

	    // empty statement -do nothing  
	  case TokenTypes.NULL_NODE:
	    printf("Type NULL\n");
	    break;

          default:
	    printf("default %d\n",t->getType() );
	    break;
	  }

	  t=t->getNextSibling();
	  
	}

	return 1;
}

// Function -- convert antlr native types to nctypes;
nc_type a2ntype(int itype)
{
  nc_type nret=NC_NAT;;
  
  switch( itype){

    case TokenTypes.BYTE: 
      nret= NC_BYTE;
      break;

   case TokenTypes.SHORT: 
     nret= NC_SHORT;
      break;

   case TokenTypes.INT: 
     nret= NC_INT;
      break;

   case TokenTypes.FLOAT: 
     nret= NC_FLOAT;
      break;

   case TokenTypes.DOUBLE: 
     nret= NC_DOUBLE;
      break;

  default:
    nret= NC_NAT;
    break;
  }
  return nret;

}



