/* $ANTLR 2.7.7 (20200513): "ncoGrammer.g" -> "ncoParser.cpp"$ */
#include "ncoParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 1 "ncoGrammer.g"
#line 8 "ncoParser.cpp"
ncoParser::ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

ncoParser::ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,3)
{
}

ncoParser::ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

ncoParser::ncoParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,3)
{
}

ncoParser::ncoParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,3)
{
}

void ncoParser::program() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST program_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			statement();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop3;
		}
		
	}
	_loop3:;
	} // ( ... )*
	program_AST = currentAST.root;
	returnAST = program_AST;
}

void ncoParser::statement() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST statement_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST e1_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  def1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST def1_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  def2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST def2_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case TIMES:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		expr();
		e1_AST = returnAST;
		astFactory->addASTChild( currentAST, returnAST );
		match(SEMI);
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 99 "ncoGrammer.g"
		
		if( ncap_fnc_srh(e1_AST))  
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(FEXPR,"FEXPR"))->add(statement_AST))); 
		else
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(EXPR,"EXPR"))->add(statement_AST))); 
		
#line 103 "ncoParser.cpp"
		currentAST.root = statement_AST;
		if ( statement_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			statement_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = statement_AST->getFirstChild();
		else
			currentAST.child = statement_AST;
		currentAST.advanceChildToEnd();
		statement_AST = currentAST.root;
		break;
	}
	case DEFDIMA:
	case DEFDIMU:
	{
		{
		switch ( LA(1)) {
		case DEFDIMA:
		{
			def1 = LT(1);
			def1_AST = astFactory->create(def1);
			astFactory->makeASTRoot(currentAST, def1_AST);
			match(DEFDIMA);
			break;
		}
		case DEFDIMU:
		{
			def2 = LT(1);
			def2_AST = astFactory->create(def2);
			astFactory->makeASTRoot(currentAST, def2_AST);
			match(DEFDIMU);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(LPAREN);
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp3_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp3_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp3_AST);
		match(NSTRING);
		match(COMMA);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			expr();
			astFactory->addASTChild( currentAST, returnAST );
			break;
		}
		case RPAREN:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(RPAREN);
		match(SEMI);
#line 109 "ncoGrammer.g"
		
		if( def1_AST ){  def1_AST->setType(DEFDIM);def1_AST->setText("1");}
		if( def2_AST ){  def2_AST->setType(DEFDIM);def2_AST->setText("0");}
		
		
#line 176 "ncoParser.cpp"
		statement_AST = currentAST.root;
		break;
	}
	case EXIT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp8_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp8_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp8_AST);
		match(EXIT);
		match(LPAREN);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(RPAREN);
		match(SEMI);
		statement_AST = currentAST.root;
		break;
	}
	case WHILE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp12_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp12_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp12_AST);
		match(WHILE);
		match(LPAREN);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(RPAREN);
		statement();
		astFactory->addASTChild( currentAST, returnAST );
		statement_AST = currentAST.root;
		break;
	}
	case FOR:
	{
		for_stmt();
		astFactory->addASTChild( currentAST, returnAST );
		statement_AST = currentAST.root;
		break;
	}
	case BREAK:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp15_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp15_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp15_AST);
		match(BREAK);
		match(SEMI);
		statement_AST = currentAST.root;
		break;
	}
	case CONTINUE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp17_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp17_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp17_AST);
		match(CONTINUE);
		match(SEMI);
		statement_AST = currentAST.root;
		break;
	}
	case SEMI:
	{
		match(SEMI);
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 124 "ncoGrammer.g"
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(1))->add(astFactory->create(NULL_NODE,"null_stmt"))));
#line 242 "ncoParser.cpp"
		currentAST.root = statement_AST;
		if ( statement_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			statement_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = statement_AST->getFirstChild();
		else
			currentAST.child = statement_AST;
		currentAST.advanceChildToEnd();
		statement_AST = currentAST.root;
		break;
	}
	case IF:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp20_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp20_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp20_AST);
		match(IF);
		match(LPAREN);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(RPAREN);
		statement();
		astFactory->addASTChild( currentAST, returnAST );
		{
		if ((LA(1) == ELSE) && (_tokenSet_0.member(LA(2))) && (_tokenSet_1.member(LA(3)))) {
			match(ELSE);
			statement();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_1.member(LA(2))) && (_tokenSet_3.member(LA(3)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		statement_AST = currentAST.root;
		break;
	}
	case WHERE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp24_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp24_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp24_AST);
		match(WHERE);
		match(LPAREN);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(RPAREN);
		statement();
		astFactory->addASTChild( currentAST, returnAST );
		{
		if ((LA(1) == ELSEWHERE) && (_tokenSet_0.member(LA(2))) && (_tokenSet_1.member(LA(3)))) {
			match(ELSEWHERE);
			statement();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_1.member(LA(2))) && (_tokenSet_3.member(LA(3)))) {
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		statement_AST = currentAST.root;
		break;
	}
	case LCURL:
	{
		block();
		astFactory->addASTChild( currentAST, returnAST );
		statement_AST = currentAST.root;
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = statement_AST;
}

void ncoParser::expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ass_expr();
	astFactory->addASTChild( currentAST, returnAST );
	expr_AST = currentAST.root;
	returnAST = expr_AST;
}

void ncoParser::for_stmt() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST for_stmt_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST e1_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST e2_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST e3_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST st_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp28_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp28_AST = astFactory->create(LT(1));
	astFactory->makeASTRoot(currentAST, tmp28_AST);
	match(FOR);
	match(LPAREN);
	{
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case TIMES:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		expr();
		e1_AST = returnAST;
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case SEMI:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(SEMI);
	{
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case TIMES:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		expr();
		e2_AST = returnAST;
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case SEMI:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(SEMI);
	{
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case TIMES:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		expr();
		e3_AST = returnAST;
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case RPAREN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(RPAREN);
	statement();
	st_AST = returnAST;
	astFactory->addASTChild( currentAST, returnAST );
	for_stmt_AST = currentAST.root;
	returnAST = for_stmt_AST;
}

void ncoParser::block() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST block_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LCURL);
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			statement();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop11;
		}
		
	}
	_loop11:;
	} // ( ... )*
	match(RCURL);
	block_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 143 "ncoGrammer.g"
	block_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(BLOCK,"block"))->add(block_AST)));
#line 505 "ncoParser.cpp"
	currentAST.root = block_AST;
	if ( block_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		block_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = block_AST->getFirstChild();
	else
		currentAST.child = block_AST;
	currentAST.advanceChildToEnd();
	block_AST = currentAST.root;
	returnAST = block_AST;
}

void ncoParser::lmt() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	{
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case TIMES:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case COMMA:
	case RPAREN:
	case COLON:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COLON)) {
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp35_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp35_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp35_AST);
			match(COLON);
			{
			switch ( LA(1)) {
			case LPAREN:
			case NSTRING:
			case VAR_ID:
			case ATT_ID:
			case TIMES:
			case FUNC:
			case INC:
			case DEC:
			case LNOT:
			case PLUS:
			case MINUS:
			case NCAP_FLOAT:
			case NCAP_DOUBLE:
			case NCAP_INT:
			case NCAP_BYTE:
			case NCAP_UBYTE:
			case NCAP_SHORT:
			case NCAP_USHORT:
			case NCAP_UINT:
			case NCAP_INT64:
			case NCAP_UINT64:
			case N4STRING:
			case DIM_ID_SIZE:
			{
				expr();
				astFactory->addASTChild( currentAST, returnAST );
				break;
			}
			case COMMA:
			case RPAREN:
			case COLON:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		else {
			goto _loop20;
		}
		
	}
	_loop20:;
	} // ( ... )*
	lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 156 "ncoGrammer.g"
	lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT,"lmt"))->add(lmt_AST)));
#line 624 "ncoParser.cpp"
	currentAST.root = lmt_AST;
	if ( lmt_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		lmt_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = lmt_AST->getFirstChild();
	else
		currentAST.child = lmt_AST;
	currentAST.advanceChildToEnd();
	lmt_AST = currentAST.root;
	returnAST = lmt_AST;
}

void ncoParser::lmt_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LPAREN);
	lmt();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			lmt();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop23;
		}
		
	}
	_loop23:;
	} // ( ... )*
	match(RPAREN);
	lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 160 "ncoGrammer.g"
	lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT_LIST,"lmt_list"))->add(lmt_list_AST)));
#line 662 "ncoParser.cpp"
	currentAST.root = lmt_list_AST;
	if ( lmt_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		lmt_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = lmt_list_AST->getFirstChild();
	else
		currentAST.child = lmt_list_AST;
	currentAST.advanceChildToEnd();
	lmt_list_AST = currentAST.root;
	returnAST = lmt_list_AST;
}

void ncoParser::dmn_list_p() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn_list_p_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LSQUARE);
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp40_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp40_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp40_AST);
		match(VAR_ID);
		break;
	}
	case DIM_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp41_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp41_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp41_AST);
		match(DIM_ID);
		break;
	}
	case COMMA:
	case RSQUARE:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			{
			switch ( LA(1)) {
			case VAR_ID:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp43_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp43_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp43_AST);
				match(VAR_ID);
				break;
			}
			case DIM_ID:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp44_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp44_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp44_AST);
				match(DIM_ID);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		else {
			goto _loop28;
		}
		
	}
	_loop28:;
	} // ( ... )*
	match(RSQUARE);
	dmn_list_p_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 168 "ncoGrammer.g"
	dmn_list_p_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(DMN_LIST_P,"dmn_list_p"))->add(dmn_list_p_AST)));
#line 749 "ncoParser.cpp"
	currentAST.root = dmn_list_p_AST;
	if ( dmn_list_p_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		dmn_list_p_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = dmn_list_p_AST->getFirstChild();
	else
		currentAST.child = dmn_list_p_AST;
	currentAST.advanceChildToEnd();
	dmn_list_p_AST = currentAST.root;
	returnAST = dmn_list_p_AST;
}

void ncoParser::dmn_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LPAREN);
	{
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp47_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp47_AST = astFactory->create(LT(1));
	astFactory->addASTChild(currentAST, tmp47_AST);
	match(DIM_ID);
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp49_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp49_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp49_AST);
			match(DIM_ID);
		}
		else {
			goto _loop32;
		}
		
	}
	_loop32:;
	} // ( ... )*
	match(RPAREN);
	dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 173 "ncoGrammer.g"
	dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(DMN_LIST,"dmn_list"))->add(dmn_list_AST)));
#line 793 "ncoParser.cpp"
	currentAST.root = dmn_list_AST;
	if ( dmn_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		dmn_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = dmn_list_AST->getFirstChild();
	else
		currentAST.child = dmn_list_AST;
	currentAST.advanceChildToEnd();
	dmn_list_AST = currentAST.root;
	returnAST = dmn_list_AST;
}

void ncoParser::dmn_arg_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn_arg_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(DIVIDE);
	{
	switch ( LA(1)) {
	case DIM_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp52_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp52_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp52_AST);
		match(DIM_ID);
		break;
	}
	case DIM_MTD_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp53_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp53_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp53_AST);
		match(DIM_MTD_ID);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			{
			switch ( LA(1)) {
			case DIM_ID:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp55_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp55_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp55_AST);
				match(DIM_ID);
				break;
			}
			case DIM_MTD_ID:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp56_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp56_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp56_AST);
				match(DIM_MTD_ID);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		else {
			goto _loop37;
		}
		
	}
	_loop37:;
	} // ( ... )*
	match(DIVIDE);
	dmn_arg_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 179 "ncoGrammer.g"
	dmn_arg_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(DMN_ARG_LIST,"dmn_arg_list"))->add(dmn_arg_list_AST)));
#line 875 "ncoParser.cpp"
	currentAST.root = dmn_arg_list_AST;
	if ( dmn_arg_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		dmn_arg_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = dmn_arg_list_AST->getFirstChild();
	else
		currentAST.child = dmn_arg_list_AST;
	currentAST.advanceChildToEnd();
	dmn_arg_list_AST = currentAST.root;
	returnAST = dmn_arg_list_AST;
}

void ncoParser::value_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST value_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LCURL);
	expr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop40;
		}
		
	}
	_loop40:;
	} // ( ... )*
	match(RCURL);
	value_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 184 "ncoGrammer.g"
	value_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(VALUE_LIST,"value_list"))->add(value_list_AST)));
#line 913 "ncoParser.cpp"
	currentAST.root = value_list_AST;
	if ( value_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		value_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = value_list_AST->getFirstChild();
	else
		currentAST.child = value_list_AST;
	currentAST.advanceChildToEnd();
	value_list_AST = currentAST.root;
	returnAST = value_list_AST;
}

void ncoParser::arg_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST arg_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case TIMES:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		arg_list_AST = currentAST.root;
		break;
	}
	case DIVIDE:
	{
		dmn_arg_list();
		astFactory->addASTChild( currentAST, returnAST );
		arg_list_AST = currentAST.root;
		break;
	}
	case DIM_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp61_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp61_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp61_AST);
		match(DIM_ID);
		arg_list_AST = currentAST.root;
		break;
	}
	case DIM_MTD_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp62_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp62_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp62_AST);
		match(DIM_MTD_ID);
		arg_list_AST = currentAST.root;
		break;
	}
	case CALL_REF:
	{
		call_ref();
		astFactory->addASTChild( currentAST, returnAST );
		arg_list_AST = currentAST.root;
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = arg_list_AST;
}

void ncoParser::call_ref() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST call_ref_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp63_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp63_AST = astFactory->create(LT(1));
	astFactory->makeASTRoot(currentAST, tmp63_AST);
	match(CALL_REF);
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp64_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp64_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp64_AST);
		match(VAR_ID);
		break;
	}
	case ATT_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp65_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp65_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp65_AST);
		match(ATT_ID);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	call_ref_AST = currentAST.root;
	returnAST = call_ref_AST;
}

void ncoParser::func_arg() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST func_arg_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LPAREN);
	{
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case DIM_ID:
	case DIVIDE:
	case DIM_MTD_ID:
	case ATT_ID:
	case CALL_REF:
	case TIMES:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		arg_list();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case COMMA:
	case RPAREN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			arg_list();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop45;
		}
		
	}
	_loop45:;
	} // ( ... )*
	match(RPAREN);
	func_arg_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 191 "ncoGrammer.g"
	func_arg_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(FUNC_ARG,"func_arg"))->add(func_arg_AST)));
#line 1106 "ncoParser.cpp"
	currentAST.root = func_arg_AST;
	if ( func_arg_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
		func_arg_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
		  currentAST.child = func_arg_AST->getFirstChild();
	else
		currentAST.child = func_arg_AST;
	currentAST.advanceChildToEnd();
	func_arg_AST = currentAST.root;
	returnAST = func_arg_AST;
}

void ncoParser::hyper_slb() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST hyper_slb_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp69_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp69_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp69_AST);
		match(VAR_ID);
		break;
	}
	case ATT_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp70_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp70_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp70_AST);
		match(ATT_ID);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case LSQUARE:
	{
		dmn_list_p();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case COLON:
	case DIVIDE:
	case TIMES:
	case DOT:
	case INC:
	case DEC:
	case PLUS:
	case MINUS:
	case CARET:
	case MOD:
	case FLTHAN:
	case FGTHAN:
	case LTHAN:
	case GTHAN:
	case GEQ:
	case LEQ:
	case EQ:
	case NEQ:
	case LAND:
	case LOR:
	case QUESTION:
	case ASSIGN:
	case PLUS_ASSIGN:
	case MINUS_ASSIGN:
	case TIMES_ASSIGN:
	case DIVIDE_ASSIGN:
	{
		break;
	}
	default:
		if ((LA(1) == LPAREN) && (_tokenSet_4.member(LA(2)))) {
			lmt_list();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else if ((LA(1) == LPAREN) && (LA(2) == DIM_ID)) {
			dmn_list();
			astFactory->addASTChild( currentAST, returnAST );
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	hyper_slb_AST = currentAST.root;
	returnAST = hyper_slb_AST;
}

/*************************************************************/
void ncoParser::top_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST top_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  ur = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST ur_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case TIMES:
	{
		ur = LT(1);
		ur_AST = astFactory->create(ur);
		astFactory->makeASTRoot(currentAST, ur_AST);
		match(TIMES);
#line 203 "ncoGrammer.g"
		ur_AST->setType(UTIMES);ur_AST->setText("UTIMES");
#line 1223 "ncoParser.cpp"
		top_exp();
		astFactory->addASTChild( currentAST, returnAST );
		top_exp_AST = currentAST.root;
		break;
	}
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case FUNC:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		primary_exp();
		astFactory->addASTChild( currentAST, returnAST );
		top_exp_AST = currentAST.root;
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = top_exp_AST;
}

void ncoParser::primary_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST primary_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case NCAP_FLOAT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp71_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp71_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp71_AST);
		match(NCAP_FLOAT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_DOUBLE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp72_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp72_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp72_AST);
		match(NCAP_DOUBLE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_INT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp73_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp73_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp73_AST);
		match(NCAP_INT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_BYTE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp74_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp74_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp74_AST);
		match(NCAP_BYTE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_UBYTE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp75_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp75_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp75_AST);
		match(NCAP_UBYTE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_SHORT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp76_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp76_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp76_AST);
		match(NCAP_SHORT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_USHORT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp77_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp77_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp77_AST);
		match(NCAP_USHORT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_UINT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp78_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp78_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp78_AST);
		match(NCAP_UINT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_INT64:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp79_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp79_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp79_AST);
		match(NCAP_INT64);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NCAP_UINT64:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp80_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp80_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp80_AST);
		match(NCAP_UINT64);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NSTRING:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp81_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp81_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp81_AST);
		match(NSTRING);
		primary_exp_AST = currentAST.root;
		break;
	}
	case N4STRING:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp82_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp82_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp82_AST);
		match(N4STRING);
		primary_exp_AST = currentAST.root;
		break;
	}
	case DIM_ID_SIZE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp83_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp83_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp83_AST);
		match(DIM_ID_SIZE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case LPAREN:
	{
		match(LPAREN);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(RPAREN);
		primary_exp_AST = currentAST.root;
		break;
	}
	case FUNC:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp86_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp86_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp86_AST);
		match(FUNC);
		func_arg();
		astFactory->addASTChild( currentAST, returnAST );
		primary_exp_AST = currentAST.root;
		break;
	}
	case VAR_ID:
	case ATT_ID:
	{
		hyper_slb();
		astFactory->addASTChild( currentAST, returnAST );
		primary_exp_AST = currentAST.root;
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = primary_exp_AST;
}

void ncoParser::meth_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST meth_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	top_exp();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == DOT)) {
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp87_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp87_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp87_AST);
			match(DOT);
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp88_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp88_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp88_AST);
			match(FUNC);
			func_arg();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop54;
		}
		
	}
	_loop54:;
	} // ( ... )*
	meth_exp_AST = currentAST.root;
	returnAST = meth_exp_AST;
}

void ncoParser::unaryleft_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST unaryleft_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST in_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  de = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST de_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	meth_exp();
	astFactory->addASTChild( currentAST, returnAST );
	{
	switch ( LA(1)) {
	case INC:
	{
		in = LT(1);
		in_AST = astFactory->create(in);
		astFactory->makeASTRoot(currentAST, in_AST);
		match(INC);
#line 212 "ncoGrammer.g"
		in_AST->setType(POST_INC);
		in_AST->setText("POST_INC");
#line 1473 "ncoParser.cpp"
		break;
	}
	case DEC:
	{
		de = LT(1);
		de_AST = astFactory->create(de);
		astFactory->makeASTRoot(currentAST, de_AST);
		match(DEC);
#line 214 "ncoGrammer.g"
		de_AST->setType(POST_DEC);
		de_AST->setText("POST_DEC");
#line 1485 "ncoParser.cpp"
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case COLON:
	case DIVIDE:
	case TIMES:
	case PLUS:
	case MINUS:
	case CARET:
	case MOD:
	case FLTHAN:
	case FGTHAN:
	case LTHAN:
	case GTHAN:
	case GEQ:
	case LEQ:
	case EQ:
	case NEQ:
	case LAND:
	case LOR:
	case QUESTION:
	case ASSIGN:
	case PLUS_ASSIGN:
	case MINUS_ASSIGN:
	case TIMES_ASSIGN:
	case DIVIDE_ASSIGN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	unaryleft_exp_AST = currentAST.root;
	returnAST = unaryleft_exp_AST;
}

void ncoParser::unary_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST unary_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	{
		{
		switch ( LA(1)) {
		case LNOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp89_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp89_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp89_AST);
			match(LNOT);
			break;
		}
		case PLUS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp90_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp90_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp90_AST);
			match(PLUS);
			break;
		}
		case MINUS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp91_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp91_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp91_AST);
			match(MINUS);
			break;
		}
		case INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp92_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp92_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp92_AST);
			match(INC);
			break;
		}
		case DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp93_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp93_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp93_AST);
			match(DEC);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		unary_exp();
		astFactory->addASTChild( currentAST, returnAST );
		unary_exp_AST = currentAST.root;
		break;
	}
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case TIMES:
	case FUNC:
	case NCAP_FLOAT:
	case NCAP_DOUBLE:
	case NCAP_INT:
	case NCAP_BYTE:
	case NCAP_UBYTE:
	case NCAP_SHORT:
	case NCAP_USHORT:
	case NCAP_UINT:
	case NCAP_INT64:
	case NCAP_UINT64:
	case N4STRING:
	case DIM_ID_SIZE:
	{
		unaryleft_exp();
		astFactory->addASTChild( currentAST, returnAST );
		unary_exp_AST = currentAST.root;
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = unary_exp_AST;
}

void ncoParser::pow_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST pow_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	unary_exp();
	astFactory->addASTChild( currentAST, returnAST );
	{
	switch ( LA(1)) {
	case CARET:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp94_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp94_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp94_AST);
		match(CARET);
		pow_exp();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case COLON:
	case DIVIDE:
	case TIMES:
	case PLUS:
	case MINUS:
	case MOD:
	case FLTHAN:
	case FGTHAN:
	case LTHAN:
	case GTHAN:
	case GEQ:
	case LEQ:
	case EQ:
	case NEQ:
	case LAND:
	case LOR:
	case QUESTION:
	case ASSIGN:
	case PLUS_ASSIGN:
	case MINUS_ASSIGN:
	case TIMES_ASSIGN:
	case DIVIDE_ASSIGN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	pow_exp_AST = currentAST.root;
	returnAST = pow_exp_AST;
}

void ncoParser::mexpr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST mexpr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	pow_exp();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == DIVIDE || LA(1) == TIMES || LA(1) == MOD)) {
			{
			switch ( LA(1)) {
			case TIMES:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp95_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp95_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp95_AST);
				match(TIMES);
				break;
			}
			case DIVIDE:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp96_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp96_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp96_AST);
				match(DIVIDE);
				break;
			}
			case MOD:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp97_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp97_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp97_AST);
				match(MOD);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			pow_exp();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop64;
		}
		
	}
	_loop64:;
	} // ( ... )*
	mexpr_AST = currentAST.root;
	returnAST = mexpr_AST;
}

void ncoParser::add_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST add_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	mexpr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == PLUS || LA(1) == MINUS)) {
			{
			switch ( LA(1)) {
			case PLUS:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp98_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp98_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp98_AST);
				match(PLUS);
				break;
			}
			case MINUS:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp99_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp99_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp99_AST);
				match(MINUS);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			mexpr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop68;
		}
		
	}
	_loop68:;
	} // ( ... )*
	add_expr_AST = currentAST.root;
	returnAST = add_expr_AST;
}

void ncoParser::frel_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST frel_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	add_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == FLTHAN || LA(1) == FGTHAN)) {
			{
			switch ( LA(1)) {
			case FLTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp100_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp100_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp100_AST);
				match(FLTHAN);
				break;
			}
			case FGTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp101_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp101_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp101_AST);
				match(FGTHAN);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			add_expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop72;
		}
		
	}
	_loop72:;
	} // ( ... )*
	frel_expr_AST = currentAST.root;
	returnAST = frel_expr_AST;
}

void ncoParser::rel_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST rel_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	frel_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if (((LA(1) >= LTHAN && LA(1) <= LEQ))) {
			{
			switch ( LA(1)) {
			case LTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp102_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp102_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp102_AST);
				match(LTHAN);
				break;
			}
			case GTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp103_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp103_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp103_AST);
				match(GTHAN);
				break;
			}
			case GEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp104_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp104_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp104_AST);
				match(GEQ);
				break;
			}
			case LEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp105_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp105_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp105_AST);
				match(LEQ);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			frel_expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop76;
		}
		
	}
	_loop76:;
	} // ( ... )*
	rel_expr_AST = currentAST.root;
	returnAST = rel_expr_AST;
}

void ncoParser::eq_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST eq_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	rel_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == EQ || LA(1) == NEQ)) {
			{
			switch ( LA(1)) {
			case EQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp106_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp106_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp106_AST);
				match(EQ);
				break;
			}
			case NEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp107_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp107_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp107_AST);
				match(NEQ);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			rel_expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop80;
		}
		
	}
	_loop80:;
	} // ( ... )*
	eq_expr_AST = currentAST.root;
	returnAST = eq_expr_AST;
}

void ncoParser::lmul_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmul_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	eq_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LAND)) {
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp108_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp108_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp108_AST);
			match(LAND);
			eq_expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop83;
		}
		
	}
	_loop83:;
	} // ( ... )*
	lmul_expr_AST = currentAST.root;
	returnAST = lmul_expr_AST;
}

void ncoParser::lor_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lor_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	lmul_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LOR)) {
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp109_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp109_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp109_AST);
			match(LOR);
			lmul_expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop86;
		}
		
	}
	_loop86:;
	} // ( ... )*
	lor_expr_AST = currentAST.root;
	returnAST = lor_expr_AST;
}

void ncoParser::cond_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST cond_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	lor_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{
	switch ( LA(1)) {
	case QUESTION:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp110_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp110_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp110_AST);
		match(QUESTION);
		ass_expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(COLON);
		cond_expr();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case COLON:
	case ASSIGN:
	case PLUS_ASSIGN:
	case MINUS_ASSIGN:
	case TIMES_ASSIGN:
	case DIVIDE_ASSIGN:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	cond_expr_AST = currentAST.root;
	returnAST = cond_expr_AST;
}

void ncoParser::ass_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST ass_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	cond_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{
	switch ( LA(1)) {
	case ASSIGN:
	case PLUS_ASSIGN:
	case MINUS_ASSIGN:
	case TIMES_ASSIGN:
	case DIVIDE_ASSIGN:
	{
		{
		switch ( LA(1)) {
		case ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp112_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp112_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp112_AST);
			match(ASSIGN);
			break;
		}
		case PLUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp113_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp113_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp113_AST);
			match(PLUS_ASSIGN);
			break;
		}
		case MINUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp114_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp114_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp114_AST);
			match(MINUS_ASSIGN);
			break;
		}
		case TIMES_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp115_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp115_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp115_AST);
			match(TIMES_ASSIGN);
			break;
		}
		case DIVIDE_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp116_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp116_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp116_AST);
			match(DIVIDE_ASSIGN);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		{
		switch ( LA(1)) {
		case LPAREN:
		case NSTRING:
		case VAR_ID:
		case ATT_ID:
		case TIMES:
		case FUNC:
		case INC:
		case DEC:
		case LNOT:
		case PLUS:
		case MINUS:
		case NCAP_FLOAT:
		case NCAP_DOUBLE:
		case NCAP_INT:
		case NCAP_BYTE:
		case NCAP_UBYTE:
		case NCAP_SHORT:
		case NCAP_USHORT:
		case NCAP_UINT:
		case NCAP_INT64:
		case NCAP_UINT64:
		case N4STRING:
		case DIM_ID_SIZE:
		{
			ass_expr();
			astFactory->addASTChild( currentAST, returnAST );
			break;
		}
		case LCURL:
		{
			value_list();
			astFactory->addASTChild( currentAST, returnAST );
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case COLON:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	ass_expr_AST = currentAST.root;
	returnAST = ass_expr_AST;
}

/*************************************************************/
void ncoParser::imaginary_token() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST imaginary_token_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp117_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp117_AST = astFactory->create(LT(1));
	astFactory->addASTChild(currentAST, tmp117_AST);
	match(NRootAST);
	imaginary_token_AST = currentAST.root;
	returnAST = imaginary_token_AST;
}

void ncoParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory )
{
	factory.setMaxNodeType(117);
}
const char* ncoParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"NULL_NODE",
	"BLOCK",
	"ARG_LIST",
	"DMN_LIST",
	"DMN_LIST_P",
	"DMN_ARG_LIST",
	"LMT_LIST",
	"VALUE_LIST",
	"FUNC_ARG",
	"LMT",
	"EXPR",
	"FEXPR",
	"POST_INC",
	"POST_DEC",
	"UTIMES",
	"PROP",
	"FOR2",
	"NORET",
	"ATAN2",
	"WHERE_ASSIGN",
	"MISS2ZERO",
	"VSORT",
	"VRSORT",
	"VABS",
	"VSQR2",
	"DEFDIM",
	";",
	"\"defdim\"",
	"\"defdimunlim\"",
	"(",
	"a string",
	",",
	")",
	"\"exit\"",
	"\"while\"",
	"\"break\"",
	"\"continue\"",
	"\"if\"",
	"\"else\"",
	"\"where\"",
	"\"elsewhere\"",
	"{",
	"}",
	"\"for\"",
	":",
	"[",
	"VAR_ID",
	"DIM_ID",
	"]",
	"/",
	"dimension identifier",
	"ATT_ID",
	"call by reference",
	"*",
	"dot operator",
	"FUNC",
	"++",
	"--",
	"!",
	"+",
	"-",
	"power of operator",
	"%",
	"<<",
	">>",
	"<",
	">",
	">=",
	"<=",
	"==",
	"!=",
	"&&",
	"||",
	"?",
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"NCAP_FLOAT",
	"NCAP_DOUBLE",
	"NCAP_INT",
	"NCAP_BYTE",
	"NCAP_UBYTE",
	"NCAP_SHORT",
	"NCAP_USHORT",
	"NCAP_UINT",
	"NCAP_INT64",
	"NCAP_UINT64",
	"N4STRING",
	"DIM_ID_SIZE",
	"NRootAST",
	"\"<<\"",
	"\">>\"",
	"\\\"",
	"DGT",
	"LPH",
	"LPHDGT",
	"XPN",
	"VAR_NM_QT",
	"BLASTOUT",
	"UNUSED_OPS",
	"white space",
	"a C++-style comment",
	"a C-style comment",
	"a floating point number",
	"NUMBER",
	"variable or function or attribute identifier",
	"VAR_ATT_QT",
	"STR_ATT_QT",
	"DIM_QT",
	"dimension identifier",
	"INCLUDE",
	"LMT_DMN",
	0
};

const unsigned long ncoParser::_tokenSet_0_data_[] = { 3221225472UL, 4202998759UL, 2146959361UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// SEMI "defdim" "defdimunlim" LPAREN NSTRING "exit" "while" "break" "continue" 
// "if" "where" LCURL "for" VAR_ID ATT_ID TIMES FUNC INC DEC LNOT PLUS 
// MINUS NCAP_FLOAT NCAP_DOUBLE NCAP_INT NCAP_BYTE NCAP_UBYTE NCAP_SHORT 
// NCAP_USHORT NCAP_UINT NCAP_INT64 NCAP_UINT64 N4STRING DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_0(_tokenSet_0_data_,8);
const unsigned long ncoParser::_tokenSet_1_data_[] = { 3221225474UL, 4272357351UL, 2147483647UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF SEMI "defdim" "defdimunlim" LPAREN NSTRING "exit" "while" "break" 
// "continue" "if" "else" "where" "elsewhere" LCURL RCURL "for" LSQUARE 
// VAR_ID DIVIDE ATT_ID TIMES DOT FUNC INC DEC LNOT PLUS MINUS CARET MOD 
// FLTHAN FGTHAN LTHAN GTHAN GEQ LEQ EQ NEQ LAND LOR QUESTION ASSIGN PLUS_ASSIGN 
// MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN NCAP_FLOAT NCAP_DOUBLE NCAP_INT 
// NCAP_BYTE NCAP_UBYTE NCAP_SHORT NCAP_USHORT NCAP_UINT NCAP_INT64 NCAP_UINT64 
// N4STRING DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_1(_tokenSet_1_data_,8);
const unsigned long ncoParser::_tokenSet_2_data_[] = { 3221225474UL, 4203020263UL, 2146959361UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF SEMI "defdim" "defdimunlim" LPAREN NSTRING "exit" "while" "break" 
// "continue" "if" "else" "where" "elsewhere" LCURL RCURL "for" VAR_ID 
// ATT_ID TIMES FUNC INC DEC LNOT PLUS MINUS NCAP_FLOAT NCAP_DOUBLE NCAP_INT 
// NCAP_BYTE NCAP_UBYTE NCAP_SHORT NCAP_USHORT NCAP_UINT NCAP_INT64 NCAP_UINT64 
// N4STRING DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long ncoParser::_tokenSet_3_data_[] = { 3221225474UL, 4294967295UL, 2147483647UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF SEMI "defdim" "defdimunlim" LPAREN NSTRING COMMA RPAREN "exit" "while" 
// "break" "continue" "if" "else" "where" "elsewhere" LCURL RCURL "for" 
// COLON LSQUARE VAR_ID DIM_ID RSQUARE DIVIDE DIM_MTD_ID ATT_ID CALL_REF 
// TIMES DOT FUNC INC DEC LNOT PLUS MINUS CARET MOD FLTHAN FGTHAN LTHAN 
// GTHAN GEQ LEQ EQ NEQ LAND LOR QUESTION ASSIGN PLUS_ASSIGN MINUS_ASSIGN 
// TIMES_ASSIGN DIVIDE_ASSIGN NCAP_FLOAT NCAP_DOUBLE NCAP_INT NCAP_BYTE 
// NCAP_UBYTE NCAP_SHORT NCAP_USHORT NCAP_UINT NCAP_INT64 NCAP_UINT64 N4STRING 
// DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long ncoParser::_tokenSet_4_data_[] = { 0UL, 4203020318UL, 2146959361UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// LPAREN NSTRING COMMA RPAREN COLON VAR_ID ATT_ID TIMES FUNC INC DEC LNOT 
// PLUS MINUS NCAP_FLOAT NCAP_DOUBLE NCAP_INT NCAP_BYTE NCAP_UBYTE NCAP_SHORT 
// NCAP_USHORT NCAP_UINT NCAP_INT64 NCAP_UINT64 N4STRING DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_4(_tokenSet_4_data_,8);


