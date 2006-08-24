#ifndef INC_ncoParserTokenTypes_hpp_
#define INC_ncoParserTokenTypes_hpp_

/* $ANTLR 2.7.5 (20050629): "ncoGrammer.g" -> "ncoParserTokenTypes.hpp"$ */

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
		EXPR = 10,
		POST_INC = 11,
		POST_DEC = 12,
		SEMI = 13,
		LCURL = 14,
		RCURL = 15,
		IF = 16,
		LPAREN = 17,
		RPAREN = 18,
		ELSE = 19,
		VAR_ID = 20,
		ATT_ID = 21,
		DEFDIM = 22,
		NSTRING = 23,
		COMMA = 24,
		FUNC = 25,
		INC = 26,
		DEC = 27,
		LNOT = 28,
		PLUS = 29,
		MINUS = 30,
		CARET = 31,
		TIMES = 32,
		DIVIDE = 33,
		MOD = 34,
		LTHAN = 35,
		GTHAN = 36,
		GEQ = 37,
		LEQ = 38,
		EQ = 39,
		NEQ = 40,
		LAND = 41,
		LOR = 42,
		QUESTION = 43,
		COLON = 44,
		ASSIGN = 45,
		PLUS_ASSIGN = 46,
		MINUS_ASSIGN = 47,
		TIMES_ASSIGN = 48,
		DIVIDE_ASSIGN = 49,
		LSQUARE = 50,
		DIM_ID = 51,
		RSQUARE = 52,
		BYTE = 53,
		SHORT = 54,
		INT = 55,
		FLOAT = 56,
		DOUBLE = 57,
		DIM_ID_VAL = 58,
		NRootAST = 59,
		QUOTE = 60,
		DGT = 61,
		LPH = 62,
		LPHDGT = 63,
		XPN = 64,
		BLASTOUT = 65,
		Whitespace = 66,
		CPP_COMMENT = 67,
		C_COMMENT = 68,
		NUMBER = 69,
		NUMBER_DOT = 70,
		VAR_ATT = 71,
		DIM_VAL = 72,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_ncoParserTokenTypes_hpp_*/
