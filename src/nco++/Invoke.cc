/*
 * Different version of the calculator which parses the command line arguments.
 * To do this the argv[] strings are first written to a ostringstream then
 * a istringstream is constructed with the string from the ostringstream and
 * fed to the lexer.
 */
#include <stdio.h>
#include <malloc.h>
#include <fstream>
#include <sstream>
#include <antlr/AST.hpp>
#include "ncoParserTokenTypes.hpp"
#include "ncoLexer.hpp"
#include "ncoParser.hpp"
#include "ncoTree.hpp"
#include <antlr/CharStreamException.hpp>
#include <antlr/TokenStreamException.hpp>
#include "ncap2.hh"


int parse_antlr(prs_sct *prs_arg,char* fl_spt_usr,char *cmd_ln_sng)
{

ANTLR_USING_NAMESPACE(std);
ANTLR_USING_NAMESPACE(antlr);

int idx;
char *filename;
 
istringstream *sin=NULL;
ifstream *in=NULL;

ncoLexer *lexer=NULL;
ncoParser *parser=NULL;

RefAST t,a;
bool bchk;



filename=fl_spt_usr;   

     try {
        
       if( cmd_ln_sng ){
	 sin= new  istringstream(cmd_ln_sng);
	 lexer= new ncoLexer( *sin, prs_arg);
       }else {
         in=new ifstream(filename);          
	 lexer= new ncoLexer( *in, prs_arg);
       }     


	  lexer->setFilename(filename);
           
	  parser= new ncoParser(*lexer);
	  parser->setFilename(filename);

	  ASTFactory ast_factory;
	  parser->initializeASTFactory(ast_factory);
	  parser->setASTFactory(&ast_factory);


	  // Parse the input expressions
	  parser->program();
	  a = parser->getAST();
	  t=a;
	  //print the parse tree
	   while( t ) {
		cout << t->toStringTree() << endl;
		t=t->getNextSibling();
	   }
	   printf("Paser tree printed\n");

	}  

            catch (RecognitionException& pe) {
              parser->reportError(pe);
              // bomb out
              nco_exit(EXIT_FAILURE);
	   }

             catch (TokenStreamException& te) {
	      cerr << te.getMessage();
              // bomb out
              nco_exit(EXIT_FAILURE);
	   }

             catch (CharStreamException& ce) {
               cerr << ce.getMessage();
              // bomb out
              nco_exit(EXIT_FAILURE);
	   }

	t=a;

    try {   
	  ncoTree walker(prs_arg);
	  printf("Walker initialized\n");
          // Run script
	  walker.run(t);

	}  catch(std::exception& e) {
	cerr << "exception: " << e.what() << endl;
	}	


        cout<< "run complete\n";
      
        delete lexer;
        delete parser;        
        if(sin) delete sin;
        if(in) delete in;
       
        return 1;


}              
