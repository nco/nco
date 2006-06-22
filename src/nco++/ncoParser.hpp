#ifndef INC_ncoParser_hpp_
#define INC_ncoParser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.6 (20060530): "ncoGrammer.g" -> "ncoParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "ncoParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 1 "ncoGrammer.g"

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
    

#line 26 "ncoParser.hpp"
class CUSTOM_API ncoParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public ncoParserTokenTypes
{
#line 1 "ncoGrammer.g"
#line 30 "ncoParser.hpp"
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
	public: void block();
	public: void assign_statement();
	public: void if_stmt();
	public: void expr();
	public: void else_part();
	public: void hyper_slb();
	public: void cast_slb();
	public: void lmt_list();
	public: void dmn_list();
	public: void func_exp();
	public: void primary_exp();
	public: void unary_exp();
	public: void pow_exp();
	public: void mexpr();
	public: void add_expr();
	public: void rel_expr();
	public: void eq_expr();
	public: void lmul_expr();
	public: void lmt();
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
	static const int NUM_TOKENS = 58;
#else
	enum {
		NUM_TOKENS = 58
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
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_11;
	static const unsigned long _tokenSet_12_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_12;
	static const unsigned long _tokenSet_13_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_13;
	static const unsigned long _tokenSet_14_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_14;
	static const unsigned long _tokenSet_15_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_15;
};

#endif /*INC_ncoParser_hpp_*/
