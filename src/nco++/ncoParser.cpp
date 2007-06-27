/* $ANTLR 2.7.6 (20070220): "ncoGrammer.g" -> "ncoParser.cpp"$ */
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
	
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case TIMES:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
	case DIM_ID_SIZE:
	{
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(SEMI);
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 61 "ncoGrammer.g"
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(EXPR,"EXPR"))->add(statement_AST)));
#line 90 "ncoParser.cpp"
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
	case DEFDIM:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp2_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp2_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp2_AST);
		match(DEFDIM);
		match(LPAREN);
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp4_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp4_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp4_AST);
		match(NSTRING);
		match(COMMA);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(RPAREN);
		match(SEMI);
		statement_AST = currentAST.root;
		break;
	}
	case WHILE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp8_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp8_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp8_AST);
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
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp11_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp11_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp11_AST);
		match(BREAK);
		match(SEMI);
		statement_AST = currentAST.root;
		break;
	}
	case CONTINUE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp13_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp13_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp13_AST);
		match(CONTINUE);
		match(SEMI);
		statement_AST = currentAST.root;
		break;
	}
	case SEMI:
	{
		match(SEMI);
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 73 "ncoGrammer.g"
		statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(1))->add(astFactory->create(NULL_NODE,"null_stmt"))));
#line 168 "ncoParser.cpp"
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
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp16_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp16_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp16_AST);
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
	case LCURL:
	{
		block();
		astFactory->addASTChild( currentAST, returnAST );
		statement_AST = currentAST.root;
		break;
	}
	case RAM_DELETE:
	{
		ram_delete();
		astFactory->addASTChild( currentAST, returnAST );
		statement_AST = currentAST.root;
		break;
	}
	case RAM_WRITE:
	{
		ram_write();
		astFactory->addASTChild( currentAST, returnAST );
		statement_AST = currentAST.root;
		break;
	}
	case SET_MISS:
	{
		set_miss();
		astFactory->addASTChild( currentAST, returnAST );
		statement_AST = currentAST.root;
		break;
	}
	case CH_MISS:
	{
		ch_miss();
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
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp20_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp20_AST = astFactory->create(LT(1));
	astFactory->makeASTRoot(currentAST, tmp20_AST);
	match(FOR);
	match(LPAREN);
	{
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case TIMES:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
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
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case TIMES:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
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
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case TIMES:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
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
			goto _loop8;
		}
		
	}
	_loop8:;
	} // ( ... )*
	match(RCURL);
	block_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 95 "ncoGrammer.g"
	block_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(BLOCK,"block"))->add(block_AST)));
#line 425 "ncoParser.cpp"
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

void ncoParser::ram_delete() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST ram_delete_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp27_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp27_AST = astFactory->create(LT(1));
	astFactory->makeASTRoot(currentAST, tmp27_AST);
	match(RAM_DELETE);
	match(LPAREN);
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp29_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp29_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp29_AST);
		match(VAR_ID);
		break;
	}
	case ATT_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp30_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp30_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp30_AST);
		match(ATT_ID);
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
	ram_delete_AST = currentAST.root;
	returnAST = ram_delete_AST;
}

void ncoParser::ram_write() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST ram_write_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp33_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp33_AST = astFactory->create(LT(1));
	astFactory->makeASTRoot(currentAST, tmp33_AST);
	match(RAM_WRITE);
	match(LPAREN);
	{
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp35_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp35_AST = astFactory->create(LT(1));
	astFactory->addASTChild(currentAST, tmp35_AST);
	match(VAR_ID);
	}
	match(RPAREN);
	match(SEMI);
	ram_write_AST = currentAST.root;
	returnAST = ram_write_AST;
}

void ncoParser::set_miss() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST set_miss_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp38_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp38_AST = astFactory->create(LT(1));
	astFactory->makeASTRoot(currentAST, tmp38_AST);
	match(SET_MISS);
	match(LPAREN);
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp40_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp40_AST = astFactory->create(LT(1));
	astFactory->addASTChild(currentAST, tmp40_AST);
	match(VAR_ID);
	match(COMMA);
	expr();
	astFactory->addASTChild( currentAST, returnAST );
	match(RPAREN);
	match(SEMI);
	set_miss_AST = currentAST.root;
	returnAST = set_miss_AST;
}

void ncoParser::ch_miss() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST ch_miss_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp44_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp44_AST = astFactory->create(LT(1));
	astFactory->makeASTRoot(currentAST, tmp44_AST);
	match(CH_MISS);
	match(LPAREN);
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp46_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp46_AST = astFactory->create(LT(1));
	astFactory->addASTChild(currentAST, tmp46_AST);
	match(VAR_ID);
	match(COMMA);
	expr();
	astFactory->addASTChild( currentAST, returnAST );
	match(RPAREN);
	match(SEMI);
	ch_miss_AST = currentAST.root;
	returnAST = ch_miss_AST;
}

void ncoParser::hyper_slb() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST hyper_slb_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp50_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp50_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp50_AST);
		match(VAR_ID);
		break;
	}
	case ATT_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp51_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp51_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp51_AST);
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
	case LPAREN:
	{
		lmt_list();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case LSQUARE:
	{
		dmn_list();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case DOT:
	case INC:
	case DEC:
	case PLUS:
	case MINUS:
	case TIMES:
	case CARET:
	case DIVIDE:
	case MOD:
	case LTHAN:
	case GTHAN:
	case GEQ:
	case LEQ:
	case EQ:
	case NEQ:
	case LAND:
	case LOR:
	case QUESTION:
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
	hyper_slb_AST = currentAST.root;
	returnAST = hyper_slb_AST;
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
			goto _loop79;
		}
		
	}
	_loop79:;
	} // ( ... )*
	match(RPAREN);
	lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 222 "ncoGrammer.g"
	lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT_LIST,"lmt_list"))->add(lmt_list_AST)));
#line 655 "ncoParser.cpp"
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

void ncoParser::dmn_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LSQUARE);
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp56_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp56_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp56_AST);
		match(VAR_ID);
		break;
	}
	case DIM_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp57_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp57_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp57_AST);
		match(DIM_ID);
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
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp59_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp59_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp59_AST);
				match(VAR_ID);
				break;
			}
			case DIM_ID:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp60_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp60_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp60_AST);
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
			goto _loop84;
		}
		
	}
	_loop84:;
	} // ( ... )*
	match(RSQUARE);
	dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 229 "ncoGrammer.g"
	dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(DMN_LIST,"dmn_list"))->add(dmn_list_AST)));
#line 737 "ncoParser.cpp"
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

void ncoParser::arg_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST arg_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case TIMES:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
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
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp62_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp62_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp62_AST);
		match(DIM_ID);
		arg_list_AST = currentAST.root;
		break;
	}
	case DIM_MTD_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp63_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp63_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp63_AST);
		match(DIM_MTD_ID);
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

void ncoParser::dmn_arg_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn_arg_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(DIVIDE);
	{
	switch ( LA(1)) {
	case DIM_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp65_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp65_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp65_AST);
		match(DIM_ID);
		break;
	}
	case DIM_MTD_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp66_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp66_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp66_AST);
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
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp68_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp68_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp68_AST);
				match(DIM_ID);
				break;
			}
			case DIM_MTD_ID:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp69_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp69_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp69_AST);
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
			goto _loop89;
		}
		
	}
	_loop89:;
	} // ( ... )*
	match(DIVIDE);
	dmn_arg_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 235 "ncoGrammer.g"
	dmn_arg_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(DMN_ARG_LIST,"dmn_arg_list"))->add(dmn_arg_list_AST)));
#line 885 "ncoParser.cpp"
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
	case ATT_ID:
	case DIM_ID:
	case DIM_MTD_ID:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case TIMES:
	case DIVIDE:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
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
			goto _loop26;
		}
		
	}
	_loop26:;
	} // ( ... )*
	match(RPAREN);
	func_arg_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 128 "ncoGrammer.g"
	func_arg_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(FUNC_ARG,"func_arg"))->add(func_arg_AST)));
#line 963 "ncoParser.cpp"
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

void ncoParser::func_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST func_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case LPAREN:
	case NSTRING:
	case VAR_ID:
	case ATT_ID:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
	case DIM_ID_SIZE:
	{
		primary_exp();
		astFactory->addASTChild( currentAST, returnAST );
		func_exp_AST = currentAST.root;
		break;
	}
	case FUNC:
	{
		{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp74_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp74_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp74_AST);
		match(FUNC);
		func_arg();
		astFactory->addASTChild( currentAST, returnAST );
		}
		func_exp_AST = currentAST.root;
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = func_exp_AST;
}

void ncoParser::primary_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST primary_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case LPAREN:
	{
		{
		match(LPAREN);
		expr();
		astFactory->addASTChild( currentAST, returnAST );
		match(RPAREN);
		}
		primary_exp_AST = currentAST.root;
		break;
	}
	case FLOAT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp77_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp77_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp77_AST);
		match(FLOAT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case DOUBLE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp78_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp78_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp78_AST);
		match(DOUBLE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case INT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp79_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp79_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp79_AST);
		match(INT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case BYTE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp80_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp80_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp80_AST);
		match(BYTE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case SHORT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp81_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp81_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp81_AST);
		match(SHORT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case USHORT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp82_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp82_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp82_AST);
		match(USHORT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case UINT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp83_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp83_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp83_AST);
		match(UINT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case INT64:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp84_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp84_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp84_AST);
		match(INT64);
		primary_exp_AST = currentAST.root;
		break;
	}
	case UINT64:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp85_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp85_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp85_AST);
		match(UINT64);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NSTRING:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp86_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp86_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp86_AST);
		match(NSTRING);
		primary_exp_AST = currentAST.root;
		break;
	}
	case DIM_ID_SIZE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp87_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp87_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp87_AST);
		match(DIM_ID_SIZE);
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

void ncoParser::meth() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST meth_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	{
	switch ( LA(1)) {
	case PAVG:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp88_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp88_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp88_AST);
		match(PAVG);
		break;
	}
	case PAVGSQR:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp89_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp89_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp89_AST);
		match(PAVGSQR);
		break;
	}
	case PMAX:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp90_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp90_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp90_AST);
		match(PMAX);
		break;
	}
	case PMIN:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp91_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp91_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp91_AST);
		match(PMIN);
		break;
	}
	case PRMS:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp92_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp92_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp92_AST);
		match(PRMS);
		break;
	}
	case PRMSSDN:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp93_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp93_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp93_AST);
		match(PRMSSDN);
		break;
	}
	case PSQR:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp94_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp94_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp94_AST);
		match(PSQR);
		break;
	}
	case ARVG:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp95_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp95_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp95_AST);
		match(ARVG);
		break;
	}
	case PTTL:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp96_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp96_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp96_AST);
		match(PTTL);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	meth_AST = currentAST.root;
	returnAST = meth_AST;
}

void ncoParser::prop_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST prop_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  node = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST node_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	func_exp();
	astFactory->addASTChild( currentAST, returnAST );
	{
	if ((LA(1) == DOT) && ((LA(2) >= PSIZE && LA(2) <= PNDIMS))) {
		node = LT(1);
		node_AST = astFactory->create(node);
		astFactory->makeASTRoot(currentAST, node_AST);
		match(DOT);
#line 147 "ncoGrammer.g"
		node_AST->setType(PROP);
		node_AST->setText("property");
#line 1261 "ncoParser.cpp"
		{
		switch ( LA(1)) {
		case PSIZE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp97_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp97_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp97_AST);
			match(PSIZE);
			break;
		}
		case PTYPE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp98_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp98_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp98_AST);
			match(PTYPE);
			break;
		}
		case PNDIMS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp99_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp99_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp99_AST);
			match(PNDIMS);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	else if ((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	prop_exp_AST = currentAST.root;
	returnAST = prop_exp_AST;
}

void ncoParser::meth_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST meth_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	prop_exp();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == DOT)) {
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp100_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp100_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp100_AST);
			match(DOT);
			meth();
			astFactory->addASTChild( currentAST, returnAST );
			func_arg();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop36;
		}
		
	}
	_loop36:;
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
#line 158 "ncoGrammer.g"
		in_AST->setType(POST_INC);
		in_AST->setText("POST_INC");
#line 1358 "ncoParser.cpp"
		break;
	}
	case DEC:
	{
		de = LT(1);
		de_AST = astFactory->create(de);
		astFactory->makeASTRoot(currentAST, de_AST);
		match(DEC);
#line 160 "ncoGrammer.g"
		de_AST->setType(POST_DEC);
		de_AST->setText("POST_DEC");
#line 1370 "ncoParser.cpp"
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case PLUS:
	case MINUS:
	case TIMES:
	case CARET:
	case DIVIDE:
	case MOD:
	case LTHAN:
	case GTHAN:
	case GEQ:
	case LEQ:
	case EQ:
	case NEQ:
	case LAND:
	case LOR:
	case QUESTION:
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
	case TIMES:
	{
		{
		switch ( LA(1)) {
		case LNOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp101_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp101_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp101_AST);
			match(LNOT);
			break;
		}
		case PLUS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp102_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp102_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp102_AST);
			match(PLUS);
			break;
		}
		case MINUS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp103_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp103_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp103_AST);
			match(MINUS);
			break;
		}
		case INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp104_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp104_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp104_AST);
			match(INC);
			break;
		}
		case DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp105_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp105_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp105_AST);
			match(DEC);
			break;
		}
		case TIMES:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp106_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp106_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp106_AST);
			match(TIMES);
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
	case FUNC:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
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
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp107_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp107_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp107_AST);
		match(CARET);
		pow_exp();
		astFactory->addASTChild( currentAST, returnAST );
		break;
	}
	case SEMI:
	case COMMA:
	case RPAREN:
	case RCURL:
	case PLUS:
	case MINUS:
	case TIMES:
	case DIVIDE:
	case MOD:
	case LTHAN:
	case GTHAN:
	case GEQ:
	case LEQ:
	case EQ:
	case NEQ:
	case LAND:
	case LOR:
	case QUESTION:
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
		if ((LA(1) == TIMES || LA(1) == DIVIDE || LA(1) == MOD)) {
			{
			switch ( LA(1)) {
			case TIMES:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp108_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp108_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp108_AST);
				match(TIMES);
				break;
			}
			case DIVIDE:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp109_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp109_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp109_AST);
				match(DIVIDE);
				break;
			}
			case MOD:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp110_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp110_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp110_AST);
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
			goto _loop46;
		}
		
	}
	_loop46:;
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
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp111_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp111_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp111_AST);
				match(PLUS);
				break;
			}
			case MINUS:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp112_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp112_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp112_AST);
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
			goto _loop50;
		}
		
	}
	_loop50:;
	} // ( ... )*
	add_expr_AST = currentAST.root;
	returnAST = add_expr_AST;
}

void ncoParser::rel_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST rel_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	add_expr();
	astFactory->addASTChild( currentAST, returnAST );
	{ // ( ... )*
	for (;;) {
		if (((LA(1) >= LTHAN && LA(1) <= LEQ))) {
			{
			switch ( LA(1)) {
			case LTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp113_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp113_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp113_AST);
				match(LTHAN);
				break;
			}
			case GTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp114_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp114_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp114_AST);
				match(GTHAN);
				break;
			}
			case GEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp115_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp115_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp115_AST);
				match(GEQ);
				break;
			}
			case LEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp116_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp116_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp116_AST);
				match(LEQ);
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
			goto _loop54;
		}
		
	}
	_loop54:;
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
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp117_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp117_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp117_AST);
				match(EQ);
				break;
			}
			case NEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp118_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp118_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp118_AST);
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
			goto _loop58;
		}
		
	}
	_loop58:;
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp119_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp119_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp119_AST);
			match(LAND);
			eq_expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop61;
		}
		
	}
	_loop61:;
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp120_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp120_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp120_AST);
			match(LOR);
			lmul_expr();
			astFactory->addASTChild( currentAST, returnAST );
		}
		else {
			goto _loop64;
		}
		
	}
	_loop64:;
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
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp121_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp121_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp121_AST);
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp123_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp123_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp123_AST);
			match(ASSIGN);
			break;
		}
		case PLUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp124_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp124_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp124_AST);
			match(PLUS_ASSIGN);
			break;
		}
		case MINUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp125_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp125_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp125_AST);
			match(MINUS_ASSIGN);
			break;
		}
		case TIMES_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp126_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp126_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp126_AST);
			match(TIMES_ASSIGN);
			break;
		}
		case DIVIDE_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp127_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp127_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp127_AST);
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
		case FUNC:
		case INC:
		case DEC:
		case LNOT:
		case PLUS:
		case MINUS:
		case TIMES:
		case FLOAT:
		case DOUBLE:
		case INT:
		case BYTE:
		case SHORT:
		case USHORT:
		case UINT:
		case INT64:
		case UINT64:
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
			goto _loop92;
		}
		
	}
	_loop92:;
	} // ( ... )*
	match(RCURL);
	value_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 240 "ncoGrammer.g"
	value_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(VALUE_LIST,"value_list"))->add(value_list_AST)));
#line 2036 "ncoParser.cpp"
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
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case TIMES:
	case FLOAT:
	case DOUBLE:
	case INT:
	case BYTE:
	case SHORT:
	case USHORT:
	case UINT:
	case INT64:
	case UINT64:
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp131_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			tmp131_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp131_AST);
			match(COLON);
			{
			switch ( LA(1)) {
			case LPAREN:
			case NSTRING:
			case VAR_ID:
			case ATT_ID:
			case FUNC:
			case INC:
			case DEC:
			case LNOT:
			case PLUS:
			case MINUS:
			case TIMES:
			case FLOAT:
			case DOUBLE:
			case INT:
			case BYTE:
			case SHORT:
			case USHORT:
			case UINT:
			case INT64:
			case UINT64:
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
			goto _loop76;
		}
		
	}
	_loop76:;
	} // ( ... )*
	lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 217 "ncoGrammer.g"
	lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT,"lmt"))->add(lmt_AST)));
#line 2151 "ncoParser.cpp"
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

void ncoParser::imaginary_token() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST imaginary_token_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp132_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	tmp132_AST = astFactory->create(LT(1));
	astFactory->addASTChild(currentAST, tmp132_AST);
	match(NRootAST);
	imaginary_token_AST = currentAST.root;
	returnAST = imaginary_token_AST;
}

void ncoParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory )
{
	factory.setMaxNodeType(112);
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
	"DMN_ARG_LIST",
	"LMT_LIST",
	"VALUE_LIST",
	"FUNC_ARG",
	"LMT",
	"EXPR",
	"POST_INC",
	"POST_DEC",
	"SQR2",
	"PROP",
	"FOR2",
	"NORET",
	";",
	"\"defdim\"",
	"(",
	"a string",
	",",
	")",
	"\"while\"",
	"\"break\"",
	"\"continue\"",
	"\"if\"",
	"\"else\"",
	"{",
	"}",
	"\"for\"",
	"\"ram_delete\"",
	"VAR_ID",
	"ATT_ID",
	"\"ram_write\"",
	"\"set_miss\"",
	"\"change_miss\"",
	"DIM_ID",
	"dimension identifier",
	"FUNC",
	"\"avg\"",
	"\"avgsqr\"",
	"\"max\"",
	"\"min\"",
	"\"rms\"",
	"\"rmssdn\"",
	"PSQR",
	"ARVG",
	"\"total\"",
	"dot operator",
	"\"size\"",
	"\"type\"",
	"\"ndims\"",
	"++",
	"--",
	"!",
	"+",
	"-",
	"*",
	"power of operator",
	"/",
	"%",
	"<",
	">",
	">=",
	"<=",
	"==",
	"!=",
	"&&",
	"||",
	"?",
	":",
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"[",
	"]",
	"FLOAT",
	"DOUBLE",
	"INT",
	"BYTE",
	"SHORT",
	"USHORT",
	"UINT",
	"INT64",
	"UINT64",
	"DIM_ID_SIZE",
	"NRootAST",
	"\"<<\"",
	"\">>\"",
	"\"sqravg\"",
	"\\\"",
	"DGT",
	"LPH",
	"LPHDGT",
	"XPN",
	"BLASTOUT",
	"UNUSED_OPS",
	"white space",
	"a C++-style comment",
	"a C-style comment",
	"a floating point number",
	"NUMBER",
	"variable or attribute identifier",
	"VAR_ATT_QT",
	"dimension identifier",
	"LMT_DMN",
	"UBYTE",
	0
};

const unsigned long ncoParser::_tokenSet_0_data_[] = { 3169845248UL, 1056965886UL, 268173312UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// SEMI "defdim" LPAREN NSTRING "while" "break" "continue" "if" LCURL "for" 
// "ram_delete" VAR_ID ATT_ID "ram_write" "set_miss" "change_miss" FUNC 
// INC DEC LNOT PLUS MINUS TIMES FLOAT DOUBLE INT BYTE SHORT USHORT UINT 
// INT64 UINT64 DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_0(_tokenSet_0_data_,8);
const unsigned long ncoParser::_tokenSet_1_data_[] = { 4243587074UL, 4279239935UL, 268303359UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF SEMI "defdim" LPAREN NSTRING "while" "break" "continue" "if" "else" 
// LCURL RCURL "for" "ram_delete" VAR_ID ATT_ID "ram_write" "set_miss" 
// "change_miss" FUNC DOT INC DEC LNOT PLUS MINUS TIMES CARET DIVIDE MOD 
// LTHAN GTHAN GEQ LEQ EQ NEQ LAND LOR QUESTION ASSIGN PLUS_ASSIGN MINUS_ASSIGN 
// TIMES_ASSIGN DIVIDE_ASSIGN LSQUARE FLOAT DOUBLE INT BYTE SHORT USHORT 
// UINT INT64 UINT64 DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_1(_tokenSet_1_data_,8);
const unsigned long ncoParser::_tokenSet_2_data_[] = { 4243587074UL, 1056965887UL, 268173312UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF SEMI "defdim" LPAREN NSTRING "while" "break" "continue" "if" "else" 
// LCURL RCURL "for" "ram_delete" VAR_ID ATT_ID "ram_write" "set_miss" 
// "change_miss" FUNC INC DEC LNOT PLUS MINUS TIMES FLOAT DOUBLE INT BYTE 
// SHORT USHORT UINT INT64 UINT64 DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_2(_tokenSet_2_data_,8);
const unsigned long ncoParser::_tokenSet_3_data_[] = { 4293918722UL, 4294967295UL, 268304383UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF SEMI "defdim" LPAREN NSTRING COMMA RPAREN "while" "break" "continue" 
// "if" "else" LCURL RCURL "for" "ram_delete" VAR_ID ATT_ID "ram_write" 
// "set_miss" "change_miss" DIM_ID DIM_MTD_ID FUNC "avg" "avgsqr" "max" 
// "min" "rms" "rmssdn" PSQR ARVG "total" DOT "size" "type" "ndims" INC 
// DEC LNOT PLUS MINUS TIMES CARET DIVIDE MOD LTHAN GTHAN GEQ LEQ EQ NEQ 
// LAND LOR QUESTION COLON ASSIGN PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN 
// DIVIDE_ASSIGN LSQUARE FLOAT DOUBLE INT BYTE SHORT USHORT UINT INT64 
// UINT64 DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long ncoParser::_tokenSet_4_data_[] = { 51380224UL, 4212129793UL, 65535UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// SEMI COMMA RPAREN RCURL DOT INC DEC PLUS MINUS TIMES CARET DIVIDE MOD 
// LTHAN GTHAN GEQ LEQ EQ NEQ LAND LOR QUESTION COLON ASSIGN PLUS_ASSIGN 
// MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_4(_tokenSet_4_data_,8);
const unsigned long ncoParser::_tokenSet_5_data_[] = { 4293918722UL, 4280287231UL, 268238847UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF SEMI "defdim" LPAREN NSTRING COMMA RPAREN "while" "break" "continue" 
// "if" "else" LCURL RCURL "for" "ram_delete" VAR_ID ATT_ID "ram_write" 
// "set_miss" "change_miss" DIM_ID DIM_MTD_ID FUNC "avg" "avgsqr" "max" 
// "min" "rms" "rmssdn" PSQR ARVG "total" DOT INC DEC LNOT PLUS MINUS TIMES 
// CARET DIVIDE MOD LTHAN GTHAN GEQ LEQ EQ NEQ LAND LOR QUESTION COLON 
// ASSIGN PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN FLOAT DOUBLE 
// INT BYTE SHORT USHORT UINT INT64 UINT64 DIM_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_5(_tokenSet_5_data_,8);


