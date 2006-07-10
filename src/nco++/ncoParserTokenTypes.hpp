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
		VAR_ID = 17,
		ATT_ID = 18,
		DEFDIM = 19,
		NSTRING = 20,
		COMMA = 21,
		FUNC = 22,
		LNOT = 23,
		PLUS = 24,
		MINUS = 25,
		CARET = 26,
		TIMES = 27,
		DIVIDE = 28,
		MOD = 29,
		LTHAN = 30,
		GTHAN = 31,
		GEQ = 32,
		LEQ = 33,
		EQ = 34,
		NEQ = 35,
		LAND = 36,
		LOR = 37,
		ASSIGN = 38,
		PLUS_ASSIGN = 39,
		MINUS_ASSIGN = 40,
		TIMES_ASSIGN = 41,
		DIVIDE_ASSIGN = 42,
		COLON = 43,
		LSQUARE = 44,
		DIM_ID = 45,
		RSQUARE = 46,
		BYTE = 47,
		SHORT = 48,
		INT = 49,
		FLOAT = 50,
		DOUBLE = 51,
		DIM_ID_VAL = 52,
		NRootAST = 53,
		QUOTE = 54,
		DGT = 55,
		LPH = 56,
		LPHDGT = 57,
		XPN = 58,
		Whitespace = 59,
		CPP_COMMENT = 60,
		C_COMMENT = 61,
		NUMBER = 62,
		NUMBER_DOT = 63,
		VAR_ATT = 64,
		DIM_VAL = 65,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_ncoParserTokenTypes_hpp_*/
