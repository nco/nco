#ifndef INC_ncoParserTokenTypes_hpp_
#define INC_ncoParserTokenTypes_hpp_

/* $ANTLR 2.7.6 (20060511): "ncoGrammer.g" -> "ncoParserTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API ncoParserTokenTypes {
#endif
	enum {
		EOF_ = 1,
		NULL_NODE = 4,
		BLOCK = 5,
		ARG_LIST = 6,
		DMN_LIST = 7,
		LMT_LIST = 8,
		LMT = 9,
		SEMI = 10,
		LCURL = 11,
		RCURL = 12,
		IF = 13,
		LPAREN = 14,
		RPAREN = 15,
		ELSE = 16,
		ASSIGN = 17,
		VAR_ID = 18,
		ATT_ID = 19,
		FUNC = 20,
		LNOT = 21,
		PLUS = 22,
		MINUS = 23,
		CARET = 24,
		TIMES = 25,
		DIVIDE = 26,
		MOD = 27,
		LTHAN = 28,
		GTHAN = 29,
		GEQ = 30,
		LEQ = 31,
		EQ = 32,
		NEQ = 33,
		LAND = 34,
		LOR = 35,
		COLON = 36,
		COMMA = 37,
		LSQUARE = 38,
		DIM_ID = 39,
		RSQUARE = 40,
		BYTE = 41,
		SHORT = 42,
		INT = 43,
		FLOAT = 44,
		DOUBLE = 45,
		NSTRING = 46,
		NRootAST = 47,
		DGT = 48,
		LPH = 49,
		LPHDGT = 50,
		XPN = 51,
		Whitespace = 52,
		CPP_COMMENT = 53,
		C_COMMENT = 54,
		NUMBER = 55,
		NUMBER_DOT = 56,
		VAR_ATT = 57,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_ncoParserTokenTypes_hpp_*/
