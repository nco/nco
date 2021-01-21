#ifndef INC_ncoParser_hpp_
#define INC_ncoParser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20200513): "ncoGrammer.g" -> "ncoParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "ncoParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 1 "ncoGrammer.g"

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

#line 57 "ncoParser.hpp"
class CUSTOM_API ncoParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public ncoParserTokenTypes
{
#line 87 "ncoGrammer.g"


public:
   std::vector<std::string> inc_vtr;
#line 61 "ncoParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	ncoParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return ncoParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return ncoParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return ncoParser::tokenNames;
	}
	public: void program();
	public: void statement();
	public: void expr();
	public: void for_stmt();
	public: void block();
	public: void lmt();
	public: void lmt_list();
	public: void dmn_list_p();
	public: void dmn_list();
	public: void dmn_arg_list();
	public: void value_list();
	public: void arg_list();
	public: void call_ref();
	public: void func_arg();
	public: void hyper_slb();
	public: void top_exp();
	public: void primary_exp();
	public: void meth_exp();
	public: void unaryleft_exp();
	public: void unary_exp();
	public: void pow_exp();
	public: void mexpr();
	public: void add_expr();
	public: void frel_expr();
	public: void rel_expr();
	public: void eq_expr();
	public: void lmul_expr();
	public: void lor_expr();
	public: void cond_expr();
	public: void ass_expr();
	public: void imaginary_token();
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 118;
#else
	enum {
		NUM_TOKENS = 118
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
};

#endif /*INC_ncoParser_hpp_*/
