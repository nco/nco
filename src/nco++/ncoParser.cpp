/* $ANTLR 2.7.6 (20060511): "ncoGrammer.g" -> "ncoParser.cpp"$ */
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
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
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
	case LCURL:
	{
		block();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		statement_AST = currentAST.root;
		break;
	}
	case IF:
	{
		if_stmt();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		statement_AST = currentAST.root;
		break;
	}
	case LPAREN:
	case VAR_ID:
	case ATT_ID:
	case NSTRING:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case BYTE:
	case SHORT:
	case INT:
	case FLOAT:
	case DOUBLE:
	case DIM_ID_SIZE:
	case ATT_ID_SIZE:
	{
		assign_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		statement_AST = currentAST.root;
		break;
	}
	case DEFDIM:
	{
		def_dim();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		statement_AST = currentAST.root;
		break;
	}
	case SEMI:
	{
		match(SEMI);
		if ( inputState->guessing==0 ) {
			statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 57 "ncoGrammer.g"
			statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(1))->add(astFactory->create(NULL_NODE,"null_stmt"))));
#line 123 "ncoParser.cpp"
			currentAST.root = statement_AST;
			if ( statement_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
				statement_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
				  currentAST.child = statement_AST->getFirstChild();
			else
				currentAST.child = statement_AST;
			currentAST.advanceChildToEnd();
		}
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

void ncoParser::block() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST block_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LCURL);
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			statement();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop7;
		}
		
	}
	_loop7:;
	} // ( ... )*
	match(RCURL);
	if ( inputState->guessing==0 ) {
		block_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 64 "ncoGrammer.g"
		block_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(BLOCK,"block"))->add(block_AST)));
#line 169 "ncoParser.cpp"
		currentAST.root = block_AST;
		if ( block_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			block_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = block_AST->getFirstChild();
		else
			currentAST.child = block_AST;
		currentAST.advanceChildToEnd();
	}
	block_AST = currentAST.root;
	returnAST = block_AST;
}

void ncoParser::if_stmt() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST if_stmt_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp4_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	if ( inputState->guessing == 0 ) {
		tmp4_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp4_AST);
	}
	match(IF);
	match(LPAREN);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	match(RPAREN);
	statement();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{
	bool synPredMatched11 = false;
	if (((LA(1) == ELSE) && (_tokenSet_0.member(LA(2))) && (_tokenSet_1.member(LA(3))))) {
		int _m11 = mark();
		synPredMatched11 = true;
		inputState->guessing++;
		try {
			{
			match(ELSE);
			}
		}
		catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
			synPredMatched11 = false;
		}
		rewind(_m11);
		inputState->guessing--;
	}
	if ( synPredMatched11 ) {
		else_part();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
	}
	else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_1.member(LA(2))) && (_tokenSet_3.member(LA(3)))) {
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
	if_stmt_AST = currentAST.root;
	returnAST = if_stmt_AST;
}

void ncoParser::assign_statement() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_statement_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	match(SEMI);
	if ( inputState->guessing==0 ) {
		assign_statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 80 "ncoGrammer.g"
		assign_statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(EXPR,"EXPR"))->add(assign_statement_AST)));
#line 251 "ncoParser.cpp"
		currentAST.root = assign_statement_AST;
		if ( assign_statement_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			assign_statement_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = assign_statement_AST->getFirstChild();
		else
			currentAST.child = assign_statement_AST;
		currentAST.advanceChildToEnd();
	}
	assign_statement_AST = currentAST.root;
	returnAST = assign_statement_AST;
}

void ncoParser::def_dim() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST def_dim_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp8_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	if ( inputState->guessing == 0 ) {
		tmp8_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp8_AST);
	}
	match(DEFDIM);
	match(LPAREN);
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp10_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	if ( inputState->guessing == 0 ) {
		tmp10_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp10_AST);
	}
	match(NSTRING);
	match(COMMA);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	match(RPAREN);
	match(SEMI);
	def_dim_AST = currentAST.root;
	returnAST = def_dim_AST;
}

void ncoParser::expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ass_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	expr_AST = currentAST.root;
	returnAST = expr_AST;
}

void ncoParser::else_part() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST else_part_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp14_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	if ( inputState->guessing == 0 ) {
		tmp14_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, tmp14_AST);
	}
	match(ELSE);
	statement();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	else_part_AST = currentAST.root;
	returnAST = else_part_AST;
}

void ncoParser::hyper_slb() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST hyper_slb_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp15_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp15_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp15_AST);
		}
		match(VAR_ID);
		break;
	}
	case ATT_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp16_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp16_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp16_AST);
		}
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
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		break;
	}
	case SEMI:
	case RCURL:
	case RPAREN:
	case COMMA:
	case INC:
	case DEC:
	case PLUS:
	case MINUS:
	case CARET:
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
	hyper_slb_AST = currentAST.root;
	returnAST = hyper_slb_AST;
}

void ncoParser::lmt_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	match(LPAREN);
	lmt();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			lmt();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop64;
		}
		
	}
	_loop64:;
	} // ( ... )*
	match(RPAREN);
	if ( inputState->guessing==0 ) {
		lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 163 "ncoGrammer.g"
		lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT_LIST,"lmt_list"))->add(lmt_list_AST)));
#line 439 "ncoParser.cpp"
		currentAST.root = lmt_list_AST;
		if ( lmt_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			lmt_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = lmt_list_AST->getFirstChild();
		else
			currentAST.child = lmt_list_AST;
		currentAST.advanceChildToEnd();
	}
	lmt_list_AST = currentAST.root;
	returnAST = lmt_list_AST;
}

void ncoParser::cast_slb() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST cast_slb_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	{
	switch ( LA(1)) {
	case VAR_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp20_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp20_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp20_AST);
		}
		match(VAR_ID);
		break;
	}
	case ATT_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp21_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp21_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp21_AST);
		}
		match(ATT_ID);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	dmn_list();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	cast_slb_AST = currentAST.root;
	returnAST = cast_slb_AST;
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
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp23_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp23_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp23_AST);
		}
		match(VAR_ID);
		break;
	}
	case DIM_ID:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp24_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp24_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp24_AST);
		}
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
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp26_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp26_AST = astFactory->create(LT(1));
					astFactory->addASTChild(currentAST, tmp26_AST);
				}
				match(VAR_ID);
				break;
			}
			case DIM_ID:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp27_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp27_AST = astFactory->create(LT(1));
					astFactory->addASTChild(currentAST, tmp27_AST);
				}
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
			goto _loop69;
		}
		
	}
	_loop69:;
	} // ( ... )*
	match(RSQUARE);
	if ( inputState->guessing==0 ) {
		dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 170 "ncoGrammer.g"
		dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(DMN_LIST,"dmn_list"))->add(dmn_list_AST)));
#line 572 "ncoParser.cpp"
		currentAST.root = dmn_list_AST;
		if ( dmn_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			dmn_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = dmn_list_AST->getFirstChild();
		else
			currentAST.child = dmn_list_AST;
		currentAST.advanceChildToEnd();
	}
	dmn_list_AST = currentAST.root;
	returnAST = dmn_list_AST;
}

void ncoParser::func_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST func_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	switch ( LA(1)) {
	case LPAREN:
	case VAR_ID:
	case ATT_ID:
	case NSTRING:
	case BYTE:
	case SHORT:
	case INT:
	case FLOAT:
	case DOUBLE:
	case DIM_ID_SIZE:
	case ATT_ID_SIZE:
	{
		primary_exp();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		func_exp_AST = currentAST.root;
		break;
	}
	case FUNC:
	{
		{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp29_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp29_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp29_AST);
		}
		match(FUNC);
		match(LPAREN);
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		match(RPAREN);
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
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		match(RPAREN);
		}
		primary_exp_AST = currentAST.root;
		break;
	}
	case BYTE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp34_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp34_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp34_AST);
		}
		match(BYTE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case SHORT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp35_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp35_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp35_AST);
		}
		match(SHORT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case INT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp36_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp36_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp36_AST);
		}
		match(INT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case FLOAT:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp37_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp37_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp37_AST);
		}
		match(FLOAT);
		primary_exp_AST = currentAST.root;
		break;
	}
	case DOUBLE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp38_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp38_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp38_AST);
		}
		match(DOUBLE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case NSTRING:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp39_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp39_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp39_AST);
		}
		match(NSTRING);
		primary_exp_AST = currentAST.root;
		break;
	}
	case DIM_ID_SIZE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp40_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp40_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp40_AST);
		}
		match(DIM_ID_SIZE);
		primary_exp_AST = currentAST.root;
		break;
	}
	case ATT_ID_SIZE:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp41_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp41_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp41_AST);
		}
		match(ATT_ID_SIZE);
		primary_exp_AST = currentAST.root;
		break;
	}
	default:
		if ((LA(1) == VAR_ID || LA(1) == ATT_ID) && (_tokenSet_4.member(LA(2)))) {
			hyper_slb();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			primary_exp_AST = currentAST.root;
		}
		else if ((LA(1) == VAR_ID || LA(1) == ATT_ID) && (LA(2) == LSQUARE)) {
			cast_slb();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			primary_exp_AST = currentAST.root;
		}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = primary_exp_AST;
}

void ncoParser::unaryleft_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST unaryleft_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  in = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST in_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefToken  de = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefAST de_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	func_exp();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{
	switch ( LA(1)) {
	case INC:
	{
		in = LT(1);
		if ( inputState->guessing == 0 ) {
			in_AST = astFactory->create(in);
			astFactory->makeASTRoot(currentAST, in_AST);
		}
		match(INC);
		if ( inputState->guessing==0 ) {
#line 100 "ncoGrammer.g"
			in_AST->setType(POST_INC);
			in_AST->setText("POST_INC");
#line 793 "ncoParser.cpp"
		}
		break;
	}
	case DEC:
	{
		de = LT(1);
		if ( inputState->guessing == 0 ) {
			de_AST = astFactory->create(de);
			astFactory->makeASTRoot(currentAST, de_AST);
		}
		match(DEC);
		if ( inputState->guessing==0 ) {
#line 102 "ncoGrammer.g"
			de_AST->setType(POST_DEC);
			de_AST->setText("POST_DEC");
#line 809 "ncoParser.cpp"
		}
		break;
	}
	case SEMI:
	case RCURL:
	case RPAREN:
	case COMMA:
	case PLUS:
	case MINUS:
	case CARET:
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp42_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp42_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp42_AST);
			}
			match(LNOT);
			break;
		}
		case PLUS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp43_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp43_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp43_AST);
			}
			match(PLUS);
			break;
		}
		case MINUS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp44_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp44_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp44_AST);
			}
			match(MINUS);
			break;
		}
		case INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp45_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp45_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp45_AST);
			}
			match(INC);
			break;
		}
		case DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp46_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp46_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp46_AST);
			}
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
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		unary_exp_AST = currentAST.root;
		break;
	}
	case LPAREN:
	case VAR_ID:
	case ATT_ID:
	case NSTRING:
	case FUNC:
	case BYTE:
	case SHORT:
	case INT:
	case FLOAT:
	case DOUBLE:
	case DIM_ID_SIZE:
	case ATT_ID_SIZE:
	{
		unaryleft_exp();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
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
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{
	switch ( LA(1)) {
	case CARET:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp47_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp47_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp47_AST);
		}
		match(CARET);
		pow_exp();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		break;
	}
	case SEMI:
	case RCURL:
	case RPAREN:
	case COMMA:
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
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if (((LA(1) >= TIMES && LA(1) <= MOD))) {
			{
			switch ( LA(1)) {
			case TIMES:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp48_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp48_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp48_AST);
				}
				match(TIMES);
				break;
			}
			case DIVIDE:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp49_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp49_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp49_AST);
				}
				match(DIVIDE);
				break;
			}
			case MOD:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp50_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp50_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp50_AST);
				}
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
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop31;
		}
		
	}
	_loop31:;
	} // ( ... )*
	mexpr_AST = currentAST.root;
	returnAST = mexpr_AST;
}

void ncoParser::add_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST add_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	mexpr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == PLUS || LA(1) == MINUS)) {
			{
			switch ( LA(1)) {
			case PLUS:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp51_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp51_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp51_AST);
				}
				match(PLUS);
				break;
			}
			case MINUS:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp52_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp52_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp52_AST);
				}
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
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop35;
		}
		
	}
	_loop35:;
	} // ( ... )*
	add_expr_AST = currentAST.root;
	returnAST = add_expr_AST;
}

void ncoParser::rel_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST rel_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	add_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if (((LA(1) >= LTHAN && LA(1) <= LEQ))) {
			{
			switch ( LA(1)) {
			case LTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp53_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp53_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp53_AST);
				}
				match(LTHAN);
				break;
			}
			case GTHAN:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp54_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp54_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp54_AST);
				}
				match(GTHAN);
				break;
			}
			case GEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp55_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp55_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp55_AST);
				}
				match(GEQ);
				break;
			}
			case LEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp56_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp56_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp56_AST);
				}
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
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop39;
		}
		
	}
	_loop39:;
	} // ( ... )*
	rel_expr_AST = currentAST.root;
	returnAST = rel_expr_AST;
}

void ncoParser::eq_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST eq_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	rel_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == EQ || LA(1) == NEQ)) {
			{
			switch ( LA(1)) {
			case EQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp57_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp57_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp57_AST);
				}
				match(EQ);
				break;
			}
			case NEQ:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp58_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp58_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp58_AST);
				}
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
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop43;
		}
		
	}
	_loop43:;
	} // ( ... )*
	eq_expr_AST = currentAST.root;
	returnAST = eq_expr_AST;
}

void ncoParser::lmul_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmul_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	eq_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LAND)) {
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp59_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp59_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp59_AST);
			}
			match(LAND);
			eq_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop46;
		}
		
	}
	_loop46:;
	} // ( ... )*
	lmul_expr_AST = currentAST.root;
	returnAST = lmul_expr_AST;
}

void ncoParser::lor_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lor_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	lmul_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LOR)) {
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp60_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp60_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp60_AST);
			}
			match(LOR);
			lmul_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop49;
		}
		
	}
	_loop49:;
	} // ( ... )*
	lor_expr_AST = currentAST.root;
	returnAST = lor_expr_AST;
}

void ncoParser::cond_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST cond_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	lor_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{
	switch ( LA(1)) {
	case QUESTION:
	{
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp61_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp61_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp61_AST);
		}
		match(QUESTION);
		ass_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		match(COLON);
		cond_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		break;
	}
	case SEMI:
	case RCURL:
	case RPAREN:
	case COMMA:
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
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp63_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp63_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp63_AST);
			}
			match(ASSIGN);
			break;
		}
		case PLUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp64_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp64_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp64_AST);
			}
			match(PLUS_ASSIGN);
			break;
		}
		case MINUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp65_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp65_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp65_AST);
			}
			match(MINUS_ASSIGN);
			break;
		}
		case TIMES_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp66_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp66_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp66_AST);
			}
			match(TIMES_ASSIGN);
			break;
		}
		case DIVIDE_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp67_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp67_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp67_AST);
			}
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
		case VAR_ID:
		case ATT_ID:
		case NSTRING:
		case FUNC:
		case INC:
		case DEC:
		case LNOT:
		case PLUS:
		case MINUS:
		case BYTE:
		case SHORT:
		case INT:
		case FLOAT:
		case DOUBLE:
		case DIM_ID_SIZE:
		case ATT_ID_SIZE:
		{
			ass_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			break;
		}
		case LCURL:
		{
			value_list();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
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
	case RCURL:
	case RPAREN:
	case COMMA:
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
	if (inputState->guessing==0) {
		astFactory->addASTChild( currentAST, returnAST );
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
		}
		else {
			goto _loop72;
		}
		
	}
	_loop72:;
	} // ( ... )*
	match(RCURL);
	if ( inputState->guessing==0 ) {
		value_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 175 "ncoGrammer.g"
		value_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(VALUE_LIST,"value_list"))->add(value_list_AST)));
#line 1559 "ncoParser.cpp"
		currentAST.root = value_list_AST;
		if ( value_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			value_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = value_list_AST->getFirstChild();
		else
			currentAST.child = value_list_AST;
		currentAST.advanceChildToEnd();
	}
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
	case VAR_ID:
	case ATT_ID:
	case NSTRING:
	case FUNC:
	case INC:
	case DEC:
	case LNOT:
	case PLUS:
	case MINUS:
	case BYTE:
	case SHORT:
	case INT:
	case FLOAT:
	case DOUBLE:
	case DIM_ID_SIZE:
	case ATT_ID_SIZE:
	{
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		break;
	}
	case RPAREN:
	case COMMA:
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp71_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp71_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp71_AST);
			}
			match(COLON);
			{
			switch ( LA(1)) {
			case LPAREN:
			case VAR_ID:
			case ATT_ID:
			case NSTRING:
			case FUNC:
			case INC:
			case DEC:
			case LNOT:
			case PLUS:
			case MINUS:
			case BYTE:
			case SHORT:
			case INT:
			case FLOAT:
			case DOUBLE:
			case DIM_ID_SIZE:
			case ATT_ID_SIZE:
			{
				expr();
				if (inputState->guessing==0) {
					astFactory->addASTChild( currentAST, returnAST );
				}
				break;
			}
			case RPAREN:
			case COMMA:
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
			goto _loop61;
		}
		
	}
	_loop61:;
	} // ( ... )*
	if ( inputState->guessing==0 ) {
		lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 158 "ncoGrammer.g"
		lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT,"lmt"))->add(lmt_AST)));
#line 1674 "ncoParser.cpp"
		currentAST.root = lmt_AST;
		if ( lmt_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
			lmt_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
			  currentAST.child = lmt_AST->getFirstChild();
		else
			currentAST.child = lmt_AST;
		currentAST.advanceChildToEnd();
	}
	lmt_AST = currentAST.root;
	returnAST = lmt_AST;
}

void ncoParser::imaginary_token() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST imaginary_token_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	ANTLR_USE_NAMESPACE(antlr)RefAST tmp72_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	if ( inputState->guessing == 0 ) {
		tmp72_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, tmp72_AST);
	}
	match(NRootAST);
	imaginary_token_AST = currentAST.root;
	returnAST = imaginary_token_AST;
}

void ncoParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory )
{
	factory.setMaxNodeType(77);
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
	"LMT_LIST",
	"VALUE_LIST",
	"LMT",
	"EXPR",
	"POST_INC",
	"POST_DEC",
	"SEMI",
	"LCURL",
	"RCURL",
	"\"if\"",
	"LPAREN",
	"RPAREN",
	"\"else\"",
	"VAR_ID",
	"ATT_ID",
	"\"defdim\"",
	"NSTRING",
	"COMMA",
	"FUNC",
	"INC",
	"DEC",
	"LNOT",
	"PLUS",
	"MINUS",
	"CARET",
	"TIMES",
	"DIVIDE",
	"MOD",
	"LTHAN",
	"GTHAN",
	"GEQ",
	"LEQ",
	"EQ",
	"NEQ",
	"LAND",
	"LOR",
	"QUESTION",
	"COLON",
	"ASSIGN",
	"PLUS_ASSIGN",
	"MINUS_ASSIGN",
	"TIMES_ASSIGN",
	"DIVIDE_ASSIGN",
	"LSQUARE",
	"DIM_ID",
	"RSQUARE",
	"BYTE",
	"SHORT",
	"INT",
	"FLOAT",
	"DOUBLE",
	"DIM_ID_SIZE",
	"ATT_ID_SIZE",
	"NRootAST",
	"\"<<\"",
	"\">>\"",
	"QUOTE",
	"DGT",
	"LPH",
	"LPHDGT",
	"XPN",
	"BLASTOUT",
	"UNUSED_OPS",
	"Whitespace",
	"CPP_COMMENT",
	"C_COMMENT",
	"NUMBER_DOT",
	"NUMBER",
	"VAR_ATT",
	"DIM_VAL",
	0
};

const unsigned long ncoParser::_tokenSet_0_data_[] = { 4259758080UL, 532676608UL, 0UL, 0UL };
// SEMI LCURL "if" LPAREN VAR_ID ATT_ID "defdim" NSTRING FUNC INC DEC LNOT 
// PLUS MINUS BYTE SHORT INT FLOAT DOUBLE DIM_ID_SIZE ATT_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long ncoParser::_tokenSet_1_data_[] = { 4260872194UL, 533716991UL, 0UL, 0UL };
// EOF SEMI LCURL RCURL "if" LPAREN "else" VAR_ID ATT_ID "defdim" NSTRING 
// FUNC INC DEC LNOT PLUS MINUS CARET TIMES DIVIDE MOD LTHAN GTHAN GEQ 
// LEQ EQ NEQ LAND LOR QUESTION ASSIGN PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN 
// DIVIDE_ASSIGN LSQUARE BYTE SHORT INT FLOAT DOUBLE DIM_ID_SIZE ATT_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long ncoParser::_tokenSet_2_data_[] = { 4260872194UL, 532676608UL, 0UL, 0UL };
// EOF SEMI LCURL RCURL "if" LPAREN "else" VAR_ID ATT_ID "defdim" NSTRING 
// FUNC INC DEC LNOT PLUS MINUS BYTE SHORT INT FLOAT DOUBLE DIM_ID_SIZE 
// ATT_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long ncoParser::_tokenSet_3_data_[] = { 4294950914UL, 534773759UL, 0UL, 0UL };
// EOF SEMI LCURL RCURL "if" LPAREN RPAREN "else" VAR_ID ATT_ID "defdim" 
// NSTRING COMMA FUNC INC DEC LNOT PLUS MINUS CARET TIMES DIVIDE MOD LTHAN 
// GTHAN GEQ LEQ EQ NEQ LAND LOR QUESTION COLON ASSIGN PLUS_ASSIGN MINUS_ASSIGN 
// TIMES_ASSIGN DIVIDE_ASSIGN LSQUARE DIM_ID BYTE SHORT INT FLOAT DOUBLE 
// DIM_ID_SIZE ATT_ID_SIZE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long ncoParser::_tokenSet_4_data_[] = { 3658301440UL, 524287UL, 0UL, 0UL };
// SEMI RCURL LPAREN RPAREN COMMA INC DEC PLUS MINUS CARET TIMES DIVIDE 
// MOD LTHAN GTHAN GEQ LEQ EQ NEQ LAND LOR QUESTION COLON ASSIGN PLUS_ASSIGN 
// MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_4(_tokenSet_4_data_,4);


