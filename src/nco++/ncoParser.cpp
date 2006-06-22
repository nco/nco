/* $ANTLR 2.7.6 (20060530): "ncoGrammer.g" -> "ncoParser.cpp"$ */
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
	
	try {      // for error handling
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	returnAST = program_AST;
}

void ncoParser::statement() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST statement_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
		case VAR_ID:
		case ATT_ID:
		{
			assign_statement();
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
		case SEMI:
		{
			match(SEMI);
			if ( inputState->guessing==0 ) {
				statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 48 "ncoGrammer.g"
				statement_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(1))->add(astFactory->create(NULL_NODE,"null_stmt"))));
#line 110 "ncoParser.cpp"
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
	returnAST = statement_AST;
}

void ncoParser::block() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST block_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
#line 55 "ncoGrammer.g"
			block_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(BLOCK,"block"))->add(block_AST)));
#line 166 "ncoParser.cpp"
			currentAST.root = block_AST;
			if ( block_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
				block_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
				  currentAST.child = block_AST->getFirstChild();
			else
				currentAST.child = block_AST;
			currentAST.advanceChildToEnd();
		}
		block_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
	returnAST = block_AST;
}

void ncoParser::assign_statement() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_statement_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		if ((LA(1) == VAR_ID || LA(1) == ATT_ID) && (LA(2) == LPAREN || LA(2) == ASSIGN)) {
			hyper_slb();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp4_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp4_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp4_AST);
			}
			match(ASSIGN);
			expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			match(SEMI);
			assign_statement_AST = currentAST.root;
		}
		else if ((LA(1) == VAR_ID || LA(1) == ATT_ID) && (LA(2) == LSQUARE)) {
			cast_slb();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp6_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp6_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp6_AST);
			}
			match(ASSIGN);
			expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			match(SEMI);
			assign_statement_AST = currentAST.root;
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
	returnAST = assign_statement_AST;
}

void ncoParser::if_stmt() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST if_stmt_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp8_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp8_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp8_AST);
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
		if (((LA(1) == ELSE) && (_tokenSet_0.member(LA(2))) && (_tokenSet_3.member(LA(3))))) {
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
		else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2))) && (_tokenSet_4.member(LA(3)))) {
			{
			}
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		if_stmt_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
	returnAST = if_stmt_AST;
}

void ncoParser::expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		lmul_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == LOR)) {
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp11_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp11_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp11_AST);
				}
				match(LOR);
				lmul_expr();
				if (inputState->guessing==0) {
					astFactory->addASTChild( currentAST, returnAST );
				}
			}
			else {
				goto _loop47;
			}
			
		}
		_loop47:;
		} // ( ... )*
		expr_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_5);
		} else {
			throw;
		}
	}
	returnAST = expr_AST;
}

void ncoParser::else_part() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST else_part_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp12_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp12_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, tmp12_AST);
		}
		match(ELSE);
		statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		else_part_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_2);
		} else {
			throw;
		}
	}
	returnAST = else_part_AST;
}

void ncoParser::hyper_slb() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST hyper_slb_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case VAR_ID:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp13_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp13_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp13_AST);
			}
			match(VAR_ID);
			break;
		}
		case ATT_ID:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp14_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp14_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp14_AST);
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
		case RPAREN:
		case ASSIGN:
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
		case COLON:
		case COMMA:
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_6);
		} else {
			throw;
		}
	}
	returnAST = hyper_slb_AST;
}

void ncoParser::cast_slb() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST cast_slb_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
		dmn_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		cast_slb_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_7);
		} else {
			throw;
		}
	}
	returnAST = cast_slb_AST;
}

void ncoParser::lmt_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
				goto _loop55;
			}
			
		}
		_loop55:;
		} // ( ... )*
		match(RPAREN);
		if ( inputState->guessing==0 ) {
			lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 129 "ncoGrammer.g"
			lmt_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT_LIST,"lmt_list"))->add(lmt_list_AST)));
#line 554 "ncoParser.cpp"
			currentAST.root = lmt_list_AST;
			if ( lmt_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
				lmt_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
				  currentAST.child = lmt_list_AST->getFirstChild();
			else
				currentAST.child = lmt_list_AST;
			currentAST.advanceChildToEnd();
		}
		lmt_list_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_6);
		} else {
			throw;
		}
	}
	returnAST = lmt_list_AST;
}

void ncoParser::dmn_list() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		match(LSQUARE);
		{
		switch ( LA(1)) {
		case VAR_ID:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp21_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp21_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp21_AST);
			}
			match(VAR_ID);
			break;
		}
		case DIM_ID:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp22_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp22_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp22_AST);
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
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp24_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp24_AST = astFactory->create(LT(1));
						astFactory->addASTChild(currentAST, tmp24_AST);
					}
					match(VAR_ID);
					break;
				}
				case DIM_ID:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp25_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp25_AST = astFactory->create(LT(1));
						astFactory->addASTChild(currentAST, tmp25_AST);
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
				goto _loop60;
			}
			
		}
		_loop60:;
		} // ( ... )*
		match(RSQUARE);
		if ( inputState->guessing==0 ) {
			dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 136 "ncoGrammer.g"
			dmn_list_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(DMN_LIST,"dmn_list"))->add(dmn_list_AST)));
#line 656 "ncoParser.cpp"
			currentAST.root = dmn_list_AST;
			if ( dmn_list_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
				dmn_list_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
				  currentAST.child = dmn_list_AST->getFirstChild();
			else
				currentAST.child = dmn_list_AST;
			currentAST.advanceChildToEnd();
		}
		dmn_list_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_7);
		} else {
			throw;
		}
	}
	returnAST = dmn_list_AST;
}

void ncoParser::func_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST func_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		switch ( LA(1)) {
		case LPAREN:
		case VAR_ID:
		case ATT_ID:
		case BYTE:
		case SHORT:
		case INT:
		case FLOAT:
		case DOUBLE:
		case NSTRING:
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp27_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp27_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp27_AST);
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
	returnAST = func_exp_AST;
}

void ncoParser::primary_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST primary_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp32_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp32_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp32_AST);
			}
			match(BYTE);
			primary_exp_AST = currentAST.root;
			break;
		}
		case SHORT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp33_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp33_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp33_AST);
			}
			match(SHORT);
			primary_exp_AST = currentAST.root;
			break;
		}
		case INT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp34_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp34_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp34_AST);
			}
			match(INT);
			primary_exp_AST = currentAST.root;
			break;
		}
		case FLOAT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp35_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp35_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp35_AST);
			}
			match(FLOAT);
			primary_exp_AST = currentAST.root;
			break;
		}
		case DOUBLE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp36_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp36_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp36_AST);
			}
			match(DOUBLE);
			primary_exp_AST = currentAST.root;
			break;
		}
		case NSTRING:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp37_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp37_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, tmp37_AST);
			}
			match(NSTRING);
			primary_exp_AST = currentAST.root;
			break;
		}
		case VAR_ID:
		case ATT_ID:
		{
			hyper_slb();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			primary_exp_AST = currentAST.root;
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
	returnAST = primary_exp_AST;
}

void ncoParser::unary_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST unary_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		switch ( LA(1)) {
		case LPAREN:
		case VAR_ID:
		case ATT_ID:
		case FUNC:
		case BYTE:
		case SHORT:
		case INT:
		case FLOAT:
		case DOUBLE:
		case NSTRING:
		{
			func_exp();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			unary_exp_AST = currentAST.root;
			break;
		}
		case LNOT:
		case PLUS:
		case MINUS:
		{
			{
			switch ( LA(1)) {
			case LNOT:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp38_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp38_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp38_AST);
				}
				match(LNOT);
				break;
			}
			case PLUS:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp39_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp39_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp39_AST);
				}
				match(PLUS);
				break;
			}
			case MINUS:
			{
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp40_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp40_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp40_AST);
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
			unary_exp();
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_8);
		} else {
			throw;
		}
	}
	returnAST = unary_exp_AST;
}

void ncoParser::pow_exp() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST pow_exp_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		unary_exp();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		{
		switch ( LA(1)) {
		case CARET:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp41_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
			if ( inputState->guessing == 0 ) {
				tmp41_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, tmp41_AST);
			}
			match(CARET);
			pow_exp();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			break;
		}
		case SEMI:
		case RPAREN:
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
		case COLON:
		case COMMA:
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
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_9);
		} else {
			throw;
		}
	}
	returnAST = pow_exp_AST;
}

void ncoParser::mexpr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST mexpr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp42_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp42_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp42_AST);
					}
					match(TIMES);
					break;
				}
				case DIVIDE:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp43_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp43_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp43_AST);
					}
					match(DIVIDE);
					break;
				}
				case MOD:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp44_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp44_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp44_AST);
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
				goto _loop29;
			}
			
		}
		_loop29:;
		} // ( ... )*
		mexpr_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_10);
		} else {
			throw;
		}
	}
	returnAST = mexpr_AST;
}

void ncoParser::add_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST add_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp45_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp45_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp45_AST);
					}
					match(PLUS);
					break;
				}
				case MINUS:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp46_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp46_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp46_AST);
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
				goto _loop33;
			}
			
		}
		_loop33:;
		} // ( ... )*
		add_expr_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_11);
		} else {
			throw;
		}
	}
	returnAST = add_expr_AST;
}

void ncoParser::rel_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST rel_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp47_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp47_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp47_AST);
					}
					match(LTHAN);
					break;
				}
				case GTHAN:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp48_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp48_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp48_AST);
					}
					match(GTHAN);
					break;
				}
				case GEQ:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp49_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp49_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp49_AST);
					}
					match(GEQ);
					break;
				}
				case LEQ:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp50_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp50_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp50_AST);
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
				goto _loop37;
			}
			
		}
		_loop37:;
		} // ( ... )*
		rel_expr_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_12);
		} else {
			throw;
		}
	}
	returnAST = rel_expr_AST;
}

void ncoParser::eq_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST eq_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
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
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp51_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp51_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp51_AST);
					}
					match(EQ);
					break;
				}
				case NEQ:
				{
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp52_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
					if ( inputState->guessing == 0 ) {
						tmp52_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, tmp52_AST);
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
				goto _loop41;
			}
			
		}
		_loop41:;
		} // ( ... )*
		eq_expr_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_13);
		} else {
			throw;
		}
	}
	returnAST = eq_expr_AST;
}

void ncoParser::lmul_expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmul_expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		eq_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild( currentAST, returnAST );
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == LAND)) {
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp53_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp53_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, tmp53_AST);
				}
				match(LAND);
				eq_expr();
				if (inputState->guessing==0) {
					astFactory->addASTChild( currentAST, returnAST );
				}
			}
			else {
				goto _loop44;
			}
			
		}
		_loop44:;
		} // ( ... )*
		lmul_expr_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_14);
		} else {
			throw;
		}
	}
	returnAST = lmul_expr_AST;
}

void ncoParser::lmt() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		{
		switch ( LA(1)) {
		case LPAREN:
		case VAR_ID:
		case ATT_ID:
		case FUNC:
		case LNOT:
		case PLUS:
		case MINUS:
		case BYTE:
		case SHORT:
		case INT:
		case FLOAT:
		case DOUBLE:
		case NSTRING:
		{
			expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild( currentAST, returnAST );
			}
			break;
		}
		case RPAREN:
		case COLON:
		case COMMA:
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
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp54_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				if ( inputState->guessing == 0 ) {
					tmp54_AST = astFactory->create(LT(1));
					astFactory->addASTChild(currentAST, tmp54_AST);
				}
				match(COLON);
				{
				switch ( LA(1)) {
				case LPAREN:
				case VAR_ID:
				case ATT_ID:
				case FUNC:
				case LNOT:
				case PLUS:
				case MINUS:
				case BYTE:
				case SHORT:
				case INT:
				case FLOAT:
				case DOUBLE:
				case NSTRING:
				{
					expr();
					if (inputState->guessing==0) {
						astFactory->addASTChild( currentAST, returnAST );
					}
					break;
				}
				case RPAREN:
				case COLON:
				case COMMA:
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
				goto _loop52;
			}
			
		}
		_loop52:;
		} // ( ... )*
		if ( inputState->guessing==0 ) {
			lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
#line 123 "ncoGrammer.g"
			lmt_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(astFactory->make((new ANTLR_USE_NAMESPACE(antlr)ASTArray(2))->add(astFactory->create(LMT,"lmt"))->add(lmt_AST)));
#line 1440 "ncoParser.cpp"
			currentAST.root = lmt_AST;
			if ( lmt_AST!=ANTLR_USE_NAMESPACE(antlr)nullAST &&
				lmt_AST->getFirstChild() != ANTLR_USE_NAMESPACE(antlr)nullAST )
				  currentAST.child = lmt_AST->getFirstChild();
			else
				currentAST.child = lmt_AST;
			currentAST.advanceChildToEnd();
		}
		lmt_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_15);
		} else {
			throw;
		}
	}
	returnAST = lmt_AST;
}

void ncoParser::imaginary_token() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST imaginary_token_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp55_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		if ( inputState->guessing == 0 ) {
			tmp55_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, tmp55_AST);
		}
		match(NRootAST);
		imaginary_token_AST = currentAST.root;
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			recover(ex,_tokenSet_1);
		} else {
			throw;
		}
	}
	returnAST = imaginary_token_AST;
}

void ncoParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory )
{
	factory.setMaxNodeType(57);
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
	"LMT",
	"SEMI",
	"LCURL",
	"RCURL",
	"\"if\"",
	"LPAREN",
	"RPAREN",
	"\"else\"",
	"ASSIGN",
	"VAR_ID",
	"ATT_ID",
	"FUNC",
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
	"COLON",
	"COMMA",
	"LSQUARE",
	"DIM_ID",
	"RSQUARE",
	"BYTE",
	"SHORT",
	"INT",
	"FLOAT",
	"DOUBLE",
	"NSTRING",
	"NRootAST",
	"DGT",
	"LPH",
	"LPHDGT",
	"XPN",
	"Whitespace",
	"CPP_COMMENT",
	"C_COMMENT",
	"NUMBER",
	"NUMBER_DOT",
	"VAR_ATT",
	0
};

const unsigned long ncoParser::_tokenSet_0_data_[] = { 797696UL, 0UL, 0UL, 0UL };
// SEMI LCURL "if" VAR_ID ATT_ID 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long ncoParser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long ncoParser::_tokenSet_2_data_[] = { 867330UL, 0UL, 0UL, 0UL };
// EOF SEMI LCURL RCURL "if" "else" VAR_ID ATT_ID 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long ncoParser::_tokenSet_3_data_[] = { 1014786UL, 64UL, 0UL, 0UL };
// EOF SEMI LCURL RCURL "if" LPAREN "else" ASSIGN VAR_ID ATT_ID LSQUARE 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long ncoParser::_tokenSet_4_data_[] = { 16776194UL, 32496UL, 0UL, 0UL };
// EOF SEMI LCURL RCURL "if" LPAREN RPAREN "else" ASSIGN VAR_ID ATT_ID 
// FUNC LNOT PLUS MINUS COLON COMMA LSQUARE DIM_ID BYTE SHORT INT FLOAT 
// DOUBLE NSTRING 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long ncoParser::_tokenSet_5_data_[] = { 33792UL, 48UL, 0UL, 0UL };
// SEMI RPAREN COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long ncoParser::_tokenSet_6_data_[] = { 4290937856UL, 63UL, 0UL, 0UL };
// SEMI RPAREN ASSIGN PLUS MINUS CARET TIMES DIVIDE MOD LTHAN GTHAN GEQ 
// LEQ EQ NEQ LAND LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long ncoParser::_tokenSet_7_data_[] = { 131072UL, 0UL, 0UL, 0UL };
// ASSIGN 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long ncoParser::_tokenSet_8_data_[] = { 4290806784UL, 63UL, 0UL, 0UL };
// SEMI RPAREN PLUS MINUS CARET TIMES DIVIDE MOD LTHAN GTHAN GEQ LEQ EQ 
// NEQ LAND LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long ncoParser::_tokenSet_9_data_[] = { 4274029568UL, 63UL, 0UL, 0UL };
// SEMI RPAREN PLUS MINUS TIMES DIVIDE MOD LTHAN GTHAN GEQ LEQ EQ NEQ LAND 
// LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_9(_tokenSet_9_data_,4);
const unsigned long ncoParser::_tokenSet_10_data_[] = { 4039148544UL, 63UL, 0UL, 0UL };
// SEMI RPAREN PLUS MINUS LTHAN GTHAN GEQ LEQ EQ NEQ LAND LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_10(_tokenSet_10_data_,4);
const unsigned long ncoParser::_tokenSet_11_data_[] = { 4026565632UL, 63UL, 0UL, 0UL };
// SEMI RPAREN LTHAN GTHAN GEQ LEQ EQ NEQ LAND LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_11(_tokenSet_11_data_,4);
const unsigned long ncoParser::_tokenSet_12_data_[] = { 33792UL, 63UL, 0UL, 0UL };
// SEMI RPAREN EQ NEQ LAND LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_12(_tokenSet_12_data_,4);
const unsigned long ncoParser::_tokenSet_13_data_[] = { 33792UL, 60UL, 0UL, 0UL };
// SEMI RPAREN LAND LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_13(_tokenSet_13_data_,4);
const unsigned long ncoParser::_tokenSet_14_data_[] = { 33792UL, 56UL, 0UL, 0UL };
// SEMI RPAREN LOR COLON COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_14(_tokenSet_14_data_,4);
const unsigned long ncoParser::_tokenSet_15_data_[] = { 32768UL, 32UL, 0UL, 0UL };
// RPAREN COMMA 
const ANTLR_USE_NAMESPACE(antlr)BitSet ncoParser::_tokenSet_15(_tokenSet_15_data_,4);


