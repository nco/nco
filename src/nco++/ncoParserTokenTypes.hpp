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
		VALUE_LIST = 9,
		LMT = 10,
		EXPR = 11,
		POST_INC = 12,
		POST_DEC = 13,
		SEMI = 14,
		LCURL = 15,
		RCURL = 16,
		IF = 17,
		LPAREN = 18,
		RPAREN = 19,
		ELSE = 20,
		VAR_ID = 21,
		ATT_ID = 22,
		DEFDIM = 23,
		NSTRING = 24,
		COMMA = 25,
		FUNC = 26,
		INC = 27,
		DEC = 28,
		LNOT = 29,
		PLUS = 30,
		MINUS = 31,
		CARET = 32,
		TIMES = 33,
		DIVIDE = 34,
		MOD = 35,
		LTHAN = 36,
		GTHAN = 37,
		GEQ = 38,
		LEQ = 39,
		EQ = 40,
		NEQ = 41,
		LAND = 42,
		LOR = 43,
		QUESTION = 44,
		COLON = 45,
		ASSIGN = 46,
		PLUS_ASSIGN = 47,
		MINUS_ASSIGN = 48,
		TIMES_ASSIGN = 49,
		DIVIDE_ASSIGN = 50,
		LSQUARE = 51,
		DIM_ID = 52,
		RSQUARE = 53,
		BYTE = 54,
		SHORT = 55,
		INT = 56,
		FLOAT = 57,
		DOUBLE = 58,
		DIM_ID_SIZE = 59,
		ATT_ID_SIZE = 60,
		NRootAST = 61,
		SHIFTL = 62,
		SHIFTR = 63,
		QUOTE = 64,
		DGT = 65,
		LPH = 66,
		LPHDGT = 67,
		XPN = 68,
		BLASTOUT = 69,
		UNUSED_OPS = 70,
		Whitespace = 71,
		CPP_COMMENT = 72,
		C_COMMENT = 73,
		NUMBER_DOT = 74,
		NUMBER = 75,
		VAR_ATT = 76,
		DIM_VAL = 77,
		LMT_DMN = 78,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_ncoParserTokenTypes_hpp_*/
