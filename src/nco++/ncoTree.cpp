/* $ANTLR 2.7.6 (20060903): "ncoGrammer.g" -> "ncoTree.cpp"$ */
#include "ncoTree.hpp"
#include <antlr/Token.hpp>
#include <antlr/AST.hpp>
#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/BitSet.hpp>
#line 1 "ncoGrammer.g"
#line 11 "ncoTree.cpp"
ncoTree::ncoTree()
	: ANTLR_USE_NAMESPACE(antlr)TreeParser() {
}

int  ncoTree::lmt_peek(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 735 "ncoGrammer.g"
	int nbr_dmn=0;
#line 19 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_peek_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		lmt = _t;
		match(_t,LMT_LIST);
		_t = _t->getNextSibling();
		if ( inputState->guessing==0 ) {
#line 737 "ncoGrammer.g"
			
			RefAST aRef;     
			aRef=lmt->getFirstChild();
			nbr_dmn=0;
			while(aRef) {
			if(aRef->getType() == LMT) nbr_dmn++;    
			aRef=aRef->getNextSibling();     
			}   
			
#line 38 "ncoTree.cpp"
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			if ( _t != ANTLR_USE_NAMESPACE(antlr)nullAST )
				_t = _t->getNextSibling();
		} else {
			throw;
		}
	}
	_retTree = _t;
	return nbr_dmn;
}

void ncoTree::statements(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
	ANTLR_USE_NAMESPACE(antlr)RefAST statements_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST blk = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST exp = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST iff = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST def = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 750 "ncoGrammer.g"
	
	var_sct *var;
	
	
#line 65 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case BLOCK:
		{
			blk = _t;
			match(_t,BLOCK);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 755 "ncoGrammer.g"
				
				//cout <<"Num of Children in block="<<blk->getNumberOfChildren()<<endl;
				run_exe(blk->getFirstChild());
				
				
#line 83 "ncoTree.cpp"
			}
			break;
		}
		case EXPR:
		{
			exp = _t;
			match(_t,EXPR);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 761 "ncoGrammer.g"
				
				var=out(exp->getFirstChild());
				var=nco_var_free(var);
				
#line 98 "ncoTree.cpp"
			}
			break;
		}
		case IF:
		{
			iff = _t;
			match(_t,IF);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 767 "ncoGrammer.g"
				
				//if can have only 3 or 5 parts  , 1 node and 2 or 4 siblings
				// IFF LOGICAL_EXP STATEMENT1 ELSE STATEMENT2
				bool br;
				var_sct *var1;
				RefAST dref;
					  //Calculate logical expression
					  var1= out( iff->getFirstChild());
					  br=ncap_var_lgcl(var1);
					  var1=nco_var_free(var1);
				
				dref=iff->getFirstChild()->getNextSibling();
				
				if(br){ 
				// Execute 3rd sibling  
				if(dref->getType()==BLOCK)
				run_exe(dref->getFirstChild());
				else
				statements(dref);     
				
					  }else{ 
				// See if else exists (third sibling)
				dref=iff->getFirstChild()->getNextSibling()->getNextSibling(); 
				if(dref && dref->getType()==ELSE ){
				// Execute 4th sibling
				if(dref->getNextSibling()->getType()==BLOCK)
				run_exe(dref->getNextSibling()->getFirstChild());
				else
				statements(dref->getNextSibling());     
				
				}
				}
				
				var=(var_sct*)NULL;
				
				
#line 145 "ncoTree.cpp"
			}
			break;
		}
		case ELSE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp81_AST_in = _t;
			match(_t,ELSE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 805 "ncoGrammer.g"
				
				
				
#line 159 "ncoTree.cpp"
			}
			break;
		}
		case DEFDIM:
		{
			def = _t;
			match(_t,DEFDIM);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 809 "ncoGrammer.g"
				
				
				const char *dmn_nm;
				long sz;
				
				dmn_nm=def->getFirstChild()->getText().c_str();
				
				var=out(def->getFirstChild()->getNextSibling());    
				var=nco_var_cnf_typ(NC_INT,var);
				
				(void)cast_void_nctype(NC_INT,&var->val);
				sz=*var->val.lp;
				var=(var_sct*)nco_var_free(var);
				(void)ncap_def_dim(dmn_nm,sz,prs_arg);
				
#line 185 "ncoTree.cpp"
			}
			break;
		}
		case NULL_NODE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp82_AST_in = _t;
			match(_t,NULL_NODE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 826 "ncoGrammer.g"
				
				
#line 198 "ncoTree.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(_t);
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			if ( _t != ANTLR_USE_NAMESPACE(antlr)nullAST )
				_t = _t->getNextSibling();
		} else {
			throw;
		}
	}
	_retTree = _t;
}

var_sct * ncoTree::assign_ntl(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 831 "ncoGrammer.g"
	var_sct *var;
#line 223 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_ntl_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 831 "ncoGrammer.g"
	
	const std::string fnc_nm("assign_ntl"); 
	
#line 237 "ncoTree.cpp"
	
	try {      // for error handling
		bool synPredMatched182 = false;
		if (((_t->getType() == ASSIGN))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t182 = _t;
			synPredMatched182 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t181 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp83_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp84_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp85_AST_in = _t;
				match(_t,LMT_LIST);
				_t = _t->getNextSibling();
				_t = __t181;
				_t = _t->getNextSibling();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched182 = false;
			}
			_t = __t182;
			inputState->guessing--;
		}
		if ( synPredMatched182 ) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t183 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp86_AST_in = _t;
			match(_t,ASSIGN);
			_t = _t->getFirstChild();
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			lmt = _t;
			match(_t,LMT_LIST);
			_t = _t->getNextSibling();
			_t = __t183;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 835 "ncoGrammer.g"
				
				
				
				if(dbg_lvl_get() > 0)
				dbg_prn(fnc_nm,vid->getText()+"(limits)");
				
				
				int rcd;
				char *var_nm; 
				var_sct *var_lhs;
				NcapVar *Nvar;              
				
				var_nm=strdup(vid->getText().c_str());
				
				var_lhs=ncap_var_init(var_nm,prs_arg,false);
				if(var_lhs){
				var=nco_var_dpl(var_lhs);
				(void)ncap_var_write(var_lhs,prs_arg);
				} else {
				
				// set var to udf
				var_lhs=ncap_var_udf(var_nm);
				var=nco_var_dpl(var_lhs);
				
				Nvar=new NcapVar(var_lhs);
				(void)prs_arg->ptr_int_vtr->push_ow(Nvar);
				}
				var_nm=(char*)nco_free(var_nm);                                  
				
#line 311 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched186 = false;
			if (((_t->getType() == ASSIGN))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t186 = _t;
				synPredMatched186 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t185 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp87_AST_in = _t;
					match(_t,ASSIGN);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp88_AST_in = _t;
					match(_t,VAR_ID);
					_t = _t->getNextSibling();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp89_AST_in = _t;
					match(_t,DMN_LIST);
					_t = _t->getNextSibling();
					_t = __t185;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched186 = false;
				}
				_t = __t186;
				inputState->guessing--;
			}
			if ( synPredMatched186 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t187 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp90_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				vid1 = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				dmn = _t;
				match(_t,DMN_LIST);
				_t = _t->getNextSibling();
				_t = __t187;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 865 "ncoGrammer.g"
					
					
					int idx;
					const char *var_nm;
					var_sct *var1;
					NcapVector<std::string> str_vtr;
					RefAST  aRef;
					NcapVar *Nvar;
					
					
					if(dbg_lvl_get() > 0)
					dbg_prn(fnc_nm,vid1->getText()+"[dims]");
					
					var_nm=vid1->getText().c_str(); 
					
					// set class wide variables
					bcst=true;  
					var_cst=(var_sct*)NULL;
					
					aRef=dmn->getFirstChild();
					
					// pPut dimension names in vector       
					while(aRef) {
					str_vtr.push(aRef->getText());
					aRef=aRef->getNextSibling();      
					}
					
					//Check that all dims exist
					for(idx=0 ; idx < str_vtr.size() ; idx++)
					if(   prs_arg->ptr_dmn_in_vtr->findi(str_vtr[idx]) ==-1             
					&& prs_arg->ptr_dmn_out_vtr->findi(str_vtr[idx]) ==-1)      
					break;
					
					// return undef if dim missing 
					if( idx <str_vtr.size()){
					var=(var_sct*)NULL;
					
					} else {
					
					// Cast is applied in VAR_ID action in function out()
					var_cst=ncap_cst_mk(str_vtr,prs_arg);
					
					var1=out(dmn->getNextSibling());
					if(var1->undefined) {
					var=(var_sct*)NULL;
					} else {
					var_cst=nco_var_cnf_typ(var1->type,var_cst);
					var_cst->typ_dsk=var1->type;
					var=nco_var_dpl(var_cst);
					
					}
					var1=nco_var_free(var1);
					}
					
					
					if(!var){
					var1=ncap_var_udf(var_nm);
					Nvar=new NcapVar(var1);
					(void)prs_arg->ptr_int_vtr->push_ow(Nvar);
					var=nco_var_dpl(var1);
					} else{
					var->nm=(char*)nco_free(var->nm);
					var->nm=strdup(var_nm);
					var1=nco_var_dpl(var);
					ncap_var_write(var1,prs_arg);
					}
					
					if(var_cst)
					var_cst=nco_var_free(var_cst);
					
					bcst=false;   
					
					
#line 430 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched190 = false;
				if (((_t->getType() == ASSIGN))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t190 = _t;
					synPredMatched190 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t189 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp91_AST_in = _t;
						match(_t,ASSIGN);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp92_AST_in = _t;
						match(_t,VAR_ID);
						_t = _t->getNextSibling();
						_t = __t189;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched190 = false;
					}
					_t = __t190;
					inputState->guessing--;
				}
				if ( synPredMatched190 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t191 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp93_AST_in = _t;
					match(_t,ASSIGN);
					_t = _t->getFirstChild();
					vid2 = _t;
					match(_t,VAR_ID);
					_t = _t->getNextSibling();
					_t = __t191;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 939 "ncoGrammer.g"
						
						
						
						if(dbg_lvl_get() > 0)
						dbg_prn(fnc_nm,vid2->getText());
						
						
						var_sct *var1;
						
						// Set class wide variables           
						bcst=false;
						var_cst=(var_sct*)NULL; 
						
						
						// get shape from RHS
						var1=out(vid2->getNextSibling());
						(void)nco_free(var1->nm);                
						var1->nm =strdup(vid2->getText().c_str());
						
						//Copy return variable
						var=nco_var_dpl(var1);
						
						// Write var to int_vtr
						// if var already in int_vtr or var_vtr then write call does nothing
						(void)ncap_var_write(var1,prs_arg);
						
						
						
#line 498 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched194 = false;
					if (((_t->getType() == ASSIGN))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t194 = _t;
						synPredMatched194 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t193 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp94_AST_in = _t;
							match(_t,ASSIGN);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp95_AST_in = _t;
							match(_t,ATT_ID);
							_t = _t->getNextSibling();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp96_AST_in = _t;
							match(_t,LMT_LIST);
							_t = _t->getNextSibling();
							_t = __t193;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched194 = false;
						}
						_t = __t194;
						inputState->guessing--;
					}
					if ( synPredMatched194 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t195 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp97_AST_in = _t;
						match(_t,ASSIGN);
						_t = _t->getFirstChild();
						att = _t;
						match(_t,ATT_ID);
						_t = _t->getNextSibling();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp98_AST_in = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						_t = __t195;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 968 "ncoGrammer.g"
							
							;
							
#line 547 "ncoTree.cpp"
						}
					}
					else {
						bool synPredMatched198 = false;
						if (((_t->getType() == ASSIGN))) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t198 = _t;
							synPredMatched198 = true;
							inputState->guessing++;
							try {
								{
								ANTLR_USE_NAMESPACE(antlr)RefAST __t197 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp99_AST_in = _t;
								match(_t,ASSIGN);
								_t = _t->getFirstChild();
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp100_AST_in = _t;
								match(_t,ATT_ID);
								_t = _t->getNextSibling();
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp101_AST_in = _t;
								match(_t,LMT_DMN);
								_t = _t->getNextSibling();
								_t = __t197;
								_t = _t->getNextSibling();
								}
							}
							catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
								synPredMatched198 = false;
							}
							_t = __t198;
							inputState->guessing--;
						}
						if ( synPredMatched198 ) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t199 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp102_AST_in = _t;
							match(_t,ASSIGN);
							_t = _t->getFirstChild();
							att1 = _t;
							match(_t,ATT_ID);
							_t = _t->getNextSibling();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp103_AST_in = _t;
							match(_t,DMN_LIST);
							_t = _t->getNextSibling();
							_t = __t199;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 971 "ncoGrammer.g"
								
								;
								
#line 596 "ncoTree.cpp"
							}
						}
						else {
							bool synPredMatched202 = false;
							if (((_t->getType() == ASSIGN))) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t202 = _t;
								synPredMatched202 = true;
								inputState->guessing++;
								try {
									{
									ANTLR_USE_NAMESPACE(antlr)RefAST __t201 = _t;
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp104_AST_in = _t;
									match(_t,ASSIGN);
									_t = _t->getFirstChild();
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp105_AST_in = _t;
									match(_t,ATT_ID);
									_t = _t->getNextSibling();
									_t = __t201;
									_t = _t->getNextSibling();
									}
								}
								catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
									synPredMatched202 = false;
								}
								_t = __t202;
								inputState->guessing--;
							}
							if ( synPredMatched202 ) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t203 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp106_AST_in = _t;
								match(_t,ASSIGN);
								_t = _t->getFirstChild();
								att2 = _t;
								match(_t,ATT_ID);
								_t = _t->getNextSibling();
								_t = __t203;
								_t = _t->getNextSibling();
								if ( inputState->guessing==0 ) {
#line 974 "ncoGrammer.g"
									
									
									//In Initial scan all newly defined atts are flagged as Undefined
									var_sct *var1;
									NcapVar *Nvar;
									
									if(dbg_lvl_get() > 0)
									dbg_prn(fnc_nm,att2->getText());
									
									
									var1=ncap_var_udf(att2->getText().c_str());
									
									Nvar=new NcapVar(var1);
									prs_arg->ptr_int_vtr->push_ow(Nvar);          
									
									// Copy return variable
									var=nco_var_dpl(var1);    
									
									
#line 655 "ncoTree.cpp"
								}
							}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(_t);
		}
		}}}}}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			if ( _t != ANTLR_USE_NAMESPACE(antlr)nullAST )
				_t = _t->getNextSibling();
		} else {
			throw;
		}
	}
	_retTree = _t;
	return var;
}

var_sct * ncoTree::assign(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 997 "ncoGrammer.g"
	var_sct *var;
#line 679 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 997 "ncoGrammer.g"
	
	const std::string fnc_nm("assign"); 
	
#line 693 "ncoTree.cpp"
	
	try {      // for error handling
		bool synPredMatched207 = false;
		if (((_t->getType() == ASSIGN))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t207 = _t;
			synPredMatched207 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t206 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp107_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp108_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp109_AST_in = _t;
				match(_t,LMT_LIST);
				_t = _t->getNextSibling();
				_t = __t206;
				_t = _t->getNextSibling();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched207 = false;
			}
			_t = __t207;
			inputState->guessing--;
		}
		if ( synPredMatched207 ) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t208 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp110_AST_in = _t;
			match(_t,ASSIGN);
			_t = _t->getFirstChild();
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			lmt = _t;
			match(_t,LMT_LIST);
			_t = _t->getNextSibling();
			_t = __t208;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1002 "ncoGrammer.g"
				
				
				int idx;
				int jdx;
				int rcd;
				int nbr_dmn;
				int var_id; 
				char *var_nm;
				
				if(dbg_lvl_get() > 0)
				dbg_prn(fnc_nm,vid->getText()+"(limits)");
				
				
				RefAST lmt_Ref;
				lmt_sct *lmt_ptr;
				var_sct *var_lhs;
				var_sct *var_rhs;
				NcapVector<lmt_sct*> lmt_vtr;          
				
				lmt_Ref=vid->getFirstChild();
				
				bcst=false;
				var_cst=(var_sct*)NULL;
				var=(var_sct*)NULL;
				NcapVar *Nvar; 
				
				
				
				
				var_nm=strdup(vid->getText().c_str());
				
				lmt_Ref=lmt;               
				
				Nvar=prs_arg->ptr_var_vtr->find(var_nm);
				
				
				// if var undefined in O or defined but not populated
				if(Nvar==NULL || ( Nvar && Nvar->flg_stt==1)){              
				// if var isn't in ouptut then copy it there
				//rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
				var_lhs=ncap_var_init(var_nm,prs_arg,true);
				
				// copy atts to output
				(void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
				(void)ncap_var_write(var_lhs,prs_arg);
				}
				
				// Get "new" var_id   
				(void)nco_inq_varid(prs_arg->out_id,var_nm,&var_id);
				
				var_lhs=ncap_var_init(var_nm,prs_arg,false);
				
				nbr_dmn=var_lhs->nbr_dim;
				
				// Now populate lmt_vtr;
				if( lmt_mk(lmt_Ref,lmt_vtr) == 0){
				err_prn(fnc_nm,"Invalid hyperslab limits for variable "+ vid->getText());
				}
				
				if( lmt_vtr.size() != nbr_dmn){
				err_prn(fnc_nm,"Number of hyperslab limits for variable "+ vid->getText()+" doesn't match number of dimensions");
				}
				
				// add dim names to dimension list 
				for(idx=0 ; idx < nbr_dmn;idx++) 
				lmt_vtr[idx]->nm=strdup(var_lhs->dim[idx]->nm);   
				
				
				var_lhs->sz=1;        
				// fill out limit structure
				for(idx=0 ; idx < nbr_dmn ;idx++){
				(void)nco_lmt_evl(prs_arg->out_id,lmt_vtr[idx],0L,prs_arg->FORTRAN_IDX_CNV);
				// Calculate size
				var_lhs->sz *= lmt_vtr[idx]->cnt;
				}
				// Calculate RHS variable                  
				var_rhs=out(lmt->getNextSibling());         
				// Convert to LHS type
				var_rhs=nco_var_cnf_typ(var_lhs->type,var_rhs);             
				
				// deal with scalar on RHS first         
				if(var_rhs->sz == 1){
				// stretch variable to var_lhs->sz                 
				(void)ncap_att_stretch(var_rhs,var_lhs->sz);
				}
				
				// make sure var_lhs and var_rhs are the same size
				// and that they are the same shape (ie they conform!!)          
				if(var_rhs->sz != var_lhs->sz){
				err_prn(fnc_nm, "Hyperslab for "+vid->getText()+" - number of elements on LHS(" +nbr2sng(var_lhs->sz) +  ") doesn't equal number of elements on RHS(" +nbr2sng(var_rhs->sz) +  ")");                                       
				}
				
				
				// Now ready to put values 
				{
				long mult_srd=1L;
				long *dmn_srt;
				long *dmn_cnt;
				long *dmn_srd;
				
				dmn_srt=(long *)nco_malloc(nbr_dmn*sizeof(long));
				dmn_cnt=(long *)nco_malloc(nbr_dmn*sizeof(long));
				dmn_srd=(long *)nco_malloc(nbr_dmn*sizeof(long));
				
				for(idx=0;idx<nbr_dmn;idx++){
				dmn_srt[idx]=lmt_vtr[idx]->srt;
				dmn_cnt[idx]=lmt_vtr[idx]->cnt;
				dmn_srd[idx]=lmt_vtr[idx]->srd;
				mult_srd*=lmt_vtr[idx]->srd;
				} /* end loop over idx */
				
				/* Check for stride */
				if(mult_srd == 1L)
					            (void)nco_put_vara(prs_arg->out_id,var_id,dmn_srt,dmn_cnt,var_rhs->val.vp,var_rhs->type);
				else
					            (void)nco_put_vars(prs_arg->out_id,var_id,dmn_srt,dmn_cnt,dmn_srd,var_rhs->val.vp,var_rhs->type);
				
				dmn_srt=(long *)nco_free(dmn_srt);
				dmn_cnt=(long *)nco_free(dmn_cnt);
				dmn_srd=(long *)nco_free(dmn_srd);
				} // end put block !!
				
				var_lhs=nco_var_free(var_lhs);
				var_rhs=nco_var_free(var_rhs);
				
				//get variable again from disk!! for return value
				var=ncap_var_init(var_nm,prs_arg,true);
				
				
				var_nm=(char*)nco_free(var_nm);
				
				// Empty and free vector 
				for(idx=0 ; idx < nbr_dmn ; idx++)
				(void)nco_lmt_free(lmt_vtr[idx]);
				
				
#line 874 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched211 = false;
			if (((_t->getType() == ASSIGN))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t211 = _t;
				synPredMatched211 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t210 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp111_AST_in = _t;
					match(_t,ASSIGN);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp112_AST_in = _t;
					match(_t,VAR_ID);
					_t = _t->getNextSibling();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp113_AST_in = _t;
					match(_t,DMN_LIST);
					_t = _t->getNextSibling();
					_t = __t210;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched211 = false;
				}
				_t = __t211;
				inputState->guessing--;
			}
			if ( synPredMatched211 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t212 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp114_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				vid1 = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				dmn = _t;
				match(_t,DMN_LIST);
				_t = _t->getNextSibling();
				_t = __t212;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1140 "ncoGrammer.g"
					
					
					var_sct *var1;
					NcapVector<std::string> str_vtr;
					RefAST  aRef;
					
					if(dbg_lvl_get() > 0)
					dbg_prn(fnc_nm,vid1->getText()+"[dims]");
					
					
					// set class wide variables
					bcst=true;  
					var_cst=(var_sct*)NULL;
					
					//aRef=vid->getFirstChild()->getFirstChild();
					aRef=dmn->getFirstChild();
					
					// pPut dimension names in vector       
					while(aRef) {
					str_vtr.push(aRef->getText());
					aRef=aRef->getNextSibling();      
					}
					// Cast is applied in VAR_ID action in function out()
					var_cst=ncap_cst_mk(str_vtr,prs_arg);     
					var1=out(dmn->getNextSibling());
					
					// deal with rhs attribute or rhs hyperslab              
					if( ncap_var_is_att(var1)|| var1->has_dpl_dmn==-1) {
					if(var1->sz ==1 )
					var1=ncap_cst_do(var1,var_cst,prs_arg->ntl_scn);
					else if( var1->sz==var_cst->sz ) {
					ptr_unn val_swp;  // Used to swap values around       
					
					var_cst=nco_var_cnf_typ(var1->type,var_cst);
					(void)ncap_att_stretch(var1,var_cst->sz);
					
					val_swp=var_cst->val; 
					var_cst->val=var1->val;
					var1->val=val_swp;
					
					var1=nco_var_free(var1);
					var1=nco_var_dpl(var_cst);                  
					
					}                                       
					else
					err_prn(fnc_nm, "LHS cast for "+vid1->getText()+" - cannot make RHS attribute "+ std::string(var1->nm) + " conform."); 
					
					// deal with rhs bare number && rhs hyperslab with single element
					} else if(var1->sz ==1 )
					var1=ncap_cst_do(var1,var_cst,prs_arg->ntl_scn);
					
					var1->nm=(char*)nco_free(var1->nm);
					
					var1->nm =strdup(vid1->getText().c_str());
					
					//Copy return variable
					var=nco_var_dpl(var1);
					
					//call to nco_var_get() in ncap_var_init() uses this property
					var1->typ_dsk=var1->type;
					(void)ncap_var_write(var1,prs_arg);
					
					bcst=false;
					var_cst=nco_var_free(var_cst); 
					
					
#line 986 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched215 = false;
				if (((_t->getType() == ASSIGN))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t215 = _t;
					synPredMatched215 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t214 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp115_AST_in = _t;
						match(_t,ASSIGN);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp116_AST_in = _t;
						match(_t,VAR_ID);
						_t = _t->getNextSibling();
						_t = __t214;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched215 = false;
					}
					_t = __t215;
					inputState->guessing--;
				}
				if ( synPredMatched215 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t216 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp117_AST_in = _t;
					match(_t,ASSIGN);
					_t = _t->getFirstChild();
					vid2 = _t;
					match(_t,VAR_ID);
					_t = _t->getNextSibling();
					_t = __t216;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1207 "ncoGrammer.g"
						
						// Set class wide variables
						int var_id;
						int rcd;
						var_sct *var1;
						NcapVar *Nvar;
						
						if(dbg_lvl_get() > 0)
						dbg_prn(fnc_nm,vid2->getText());
						
						
						
						bcst=false;
						var_cst=(var_sct*)NULL; 
						
						
						var1=out(vid2->getNextSibling());
						
						// Save name 
						std::string s_var_rhs(var1->nm);
						(void)nco_free(var1->nm);                
						var1->nm =strdup(vid2->getText().c_str());
						
						// Do attribute propagation only if
						// var doesn't already exist or is defined but NOT
						// populated
						Nvar=prs_arg->ptr_var_vtr->find(vid2->getText());
						//rcd=nco_inq_varid_flg(prs_arg->out_id,var1->nm ,&var_id);
						
						
						if(!Nvar || Nvar && Nvar->flg_stt==1)
						(void)ncap_att_cpy(vid2->getText(),s_var_rhs,prs_arg);
						
						
						//Copy return variable
						var=nco_var_dpl(var1);
						// Write var to disk
						(void)ncap_var_write(var1,prs_arg);
						
						
#line 1066 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched219 = false;
					if (((_t->getType() == ASSIGN))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t219 = _t;
						synPredMatched219 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t218 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp118_AST_in = _t;
							match(_t,ASSIGN);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp119_AST_in = _t;
							match(_t,ATT_ID);
							_t = _t->getNextSibling();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp120_AST_in = _t;
							match(_t,LMT_LIST);
							_t = _t->getNextSibling();
							_t = __t218;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched219 = false;
						}
						_t = __t219;
						inputState->guessing--;
					}
					if ( synPredMatched219 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t220 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp121_AST_in = _t;
						match(_t,ASSIGN);
						_t = _t->getFirstChild();
						att = _t;
						match(_t,ATT_ID);
						_t = _t->getNextSibling();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp122_AST_in = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						_t = __t220;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1248 "ncoGrammer.g"
							
							;
							
#line 1115 "ncoTree.cpp"
						}
					}
					else {
						bool synPredMatched223 = false;
						if (((_t->getType() == ASSIGN))) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t223 = _t;
							synPredMatched223 = true;
							inputState->guessing++;
							try {
								{
								ANTLR_USE_NAMESPACE(antlr)RefAST __t222 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp123_AST_in = _t;
								match(_t,ASSIGN);
								_t = _t->getFirstChild();
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp124_AST_in = _t;
								match(_t,ATT_ID);
								_t = _t->getNextSibling();
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp125_AST_in = _t;
								match(_t,LMT_DMN);
								_t = _t->getNextSibling();
								_t = __t222;
								_t = _t->getNextSibling();
								}
							}
							catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
								synPredMatched223 = false;
							}
							_t = __t223;
							inputState->guessing--;
						}
						if ( synPredMatched223 ) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t224 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp126_AST_in = _t;
							match(_t,ASSIGN);
							_t = _t->getFirstChild();
							att1 = _t;
							match(_t,ATT_ID);
							_t = _t->getNextSibling();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp127_AST_in = _t;
							match(_t,DMN_LIST);
							_t = _t->getNextSibling();
							_t = __t224;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 1251 "ncoGrammer.g"
								
								;
								
#line 1164 "ncoTree.cpp"
							}
						}
						else {
							bool synPredMatched227 = false;
							if (((_t->getType() == ASSIGN))) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t227 = _t;
								synPredMatched227 = true;
								inputState->guessing++;
								try {
									{
									ANTLR_USE_NAMESPACE(antlr)RefAST __t226 = _t;
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp128_AST_in = _t;
									match(_t,ASSIGN);
									_t = _t->getFirstChild();
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp129_AST_in = _t;
									match(_t,ATT_ID);
									_t = _t->getNextSibling();
									_t = __t226;
									_t = _t->getNextSibling();
									}
								}
								catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
									synPredMatched227 = false;
								}
								_t = __t227;
								inputState->guessing--;
							}
							if ( synPredMatched227 ) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t228 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp130_AST_in = _t;
								match(_t,ASSIGN);
								_t = _t->getFirstChild();
								att2 = _t;
								match(_t,ATT_ID);
								_t = _t->getNextSibling();
								_t = __t228;
								_t = _t->getNextSibling();
								if ( inputState->guessing==0 ) {
#line 1254 "ncoGrammer.g"
									
									
									
									var_sct *var1;
									string sa=att2->getText();
									
									if(dbg_lvl_get() > 0)
									dbg_prn(fnc_nm,sa);
									
									var1=out(att2->getNextSibling());
									(void)nco_free(var1->nm);
									var1->nm=strdup(sa.c_str());
									
									//var_nw=nco_var_dpl(var);
									NcapVar *Nvar=new NcapVar(var1,sa);
									prs_arg->ptr_var_vtr->push_ow(Nvar);       
									
									// Copy return variable
									var=nco_var_dpl(var1);    
									
									
#line 1225 "ncoTree.cpp"
								}
							}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(_t);
		}
		}}}}}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			if ( _t != ANTLR_USE_NAMESPACE(antlr)nullAST )
				_t = _t->getNextSibling();
		} else {
			throw;
		}
	}
	_retTree = _t;
	return var;
}

var_sct * ncoTree::out(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 1278 "ncoGrammer.g"
	var_sct *var;
#line 1249 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST qus = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST m = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST pls_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST min_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST tim_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST div_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST prp = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST c = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST s = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST i = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST f = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST d = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST str = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dval = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST v = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vlst = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 1278 "ncoGrammer.g"
	
	const std::string fnc_nm("out"); 
		var_sct *var1;
	var_sct *var2;
	
#line 1277 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case TIMES:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t238 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp131_AST_in = _t;
			match(_t,TIMES);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t238;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1291 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, TIMES );
#line 1298 "ncoTree.cpp"
			}
			break;
		}
		case DIVIDE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t239 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp132_AST_in = _t;
			match(_t,DIVIDE);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t239;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1293 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, DIVIDE );
#line 1317 "ncoTree.cpp"
			}
			break;
		}
		case MOD:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t240 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp133_AST_in = _t;
			match(_t,MOD);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t240;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1295 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, MOD);
#line 1336 "ncoTree.cpp"
			}
			break;
		}
		case CARET:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t241 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp134_AST_in = _t;
			match(_t,CARET);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t241;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1297 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, CARET);
#line 1355 "ncoTree.cpp"
			}
			break;
		}
		case LNOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t242 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp135_AST_in = _t;
			match(_t,LNOT);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t242;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1301 "ncoGrammer.g"
				var=ncap_var_var_op(var1,(var_sct*)NULL, LNOT );
#line 1372 "ncoTree.cpp"
			}
			break;
		}
		case INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t245 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp136_AST_in = _t;
			match(_t,INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t245;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1307 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, INC,prs_arg);
#line 1389 "ncoTree.cpp"
			}
			break;
		}
		case DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t246 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp137_AST_in = _t;
			match(_t,DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t246;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1309 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, DEC,prs_arg );
#line 1406 "ncoTree.cpp"
			}
			break;
		}
		case POST_INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t247 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp138_AST_in = _t;
			match(_t,POST_INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t247;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1312 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, POST_INC,prs_arg);
#line 1423 "ncoTree.cpp"
			}
			break;
		}
		case POST_DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t248 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp139_AST_in = _t;
			match(_t,POST_DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t248;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1314 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, POST_DEC,prs_arg );
#line 1440 "ncoTree.cpp"
			}
			break;
		}
		case QUESTION:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t249 = _t;
			qus = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,QUESTION);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t249;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1317 "ncoGrammer.g"
				
				bool br;
				
				// if initial scan 
				if(prs_arg->ntl_scn){
				var1=nco_var_free(var1);
				var=ncap_var_udf("_question"); 
				return var;
				}
				
				br=ncap_var_lgcl(var1);
				var1=nco_var_free(var1);
				
				if(br) 
				var=out(qus->getFirstChild()->getNextSibling());
				else
				var=out(qus->getFirstChild()->getNextSibling()->getNextSibling());      
				
#line 1474 "ncoTree.cpp"
			}
			break;
		}
		case FUNC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t250 = _t;
			m = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,FUNC);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t250;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1339 "ncoGrammer.g"
				
				sym_sct * sym_ptr;
				
				sym_ptr= prs_arg->ptr_sym_vtr->find(m->getText());
				if(sym_ptr !=NULL)   var=ncap_var_fnc(var1,sym_ptr); 
				else{
				cout << "Function  " << m->getText() << " not found" << endl;
				exit(1);
				} 
				
#line 1500 "ncoTree.cpp"
			}
			break;
		}
		case LAND:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t251 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp140_AST_in = _t;
			match(_t,LAND);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t251;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1353 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LAND );
#line 1519 "ncoTree.cpp"
			}
			break;
		}
		case LOR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t252 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp141_AST_in = _t;
			match(_t,LOR);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t252;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1355 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LOR );
#line 1538 "ncoTree.cpp"
			}
			break;
		}
		case LTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t253 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp142_AST_in = _t;
			match(_t,LTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t253;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1358 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LTHAN );
#line 1557 "ncoTree.cpp"
			}
			break;
		}
		case GTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t254 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp143_AST_in = _t;
			match(_t,GTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t254;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1360 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GTHAN );
#line 1576 "ncoTree.cpp"
			}
			break;
		}
		case GEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t255 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp144_AST_in = _t;
			match(_t,GEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t255;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1362 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GEQ );
#line 1595 "ncoTree.cpp"
			}
			break;
		}
		case LEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t256 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp145_AST_in = _t;
			match(_t,LEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t256;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1364 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LEQ );
#line 1614 "ncoTree.cpp"
			}
			break;
		}
		case EQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t257 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp146_AST_in = _t;
			match(_t,EQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t257;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1366 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, EQ );
#line 1633 "ncoTree.cpp"
			}
			break;
		}
		case NEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t258 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp147_AST_in = _t;
			match(_t,NEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t258;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1368 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, NEQ );
#line 1652 "ncoTree.cpp"
			}
			break;
		}
		case PLUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t259 = _t;
			pls_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,PLUS_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t259;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1372 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN , prs_arg);
#line 1671 "ncoTree.cpp"
			}
			break;
		}
		case MINUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t260 = _t;
			min_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,MINUS_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t260;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1374 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN, prs_arg );
#line 1690 "ncoTree.cpp"
			}
			break;
		}
		case TIMES_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t261 = _t;
			tim_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,TIMES_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t261;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1376 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN, prs_arg );
#line 1709 "ncoTree.cpp"
			}
			break;
		}
		case DIVIDE_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t262 = _t;
			div_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,DIVIDE_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t262;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1378 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN,prs_arg );
#line 1728 "ncoTree.cpp"
			}
			break;
		}
		case ASSIGN:
		{
			asn = _t;
			match(_t,ASSIGN);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1380 "ncoGrammer.g"
				if(prs_arg->ntl_scn)
				var=assign_ntl(asn); 
				else
				var=assign(asn);
				
#line 1744 "ncoTree.cpp"
			}
			break;
		}
		case DOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t263 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp148_AST_in = _t;
			match(_t,DOT);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			prp = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t263;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1387 "ncoGrammer.g"
				
				// de-reference 
				ddra_info_sct ddra_info;            
				
				
				if(var1->undefined){ 
				var=var1;
				// n.b can't use return -- as this results with
				// problems with automagically generated code 
				goto end_dot;
				}
				
				// blow out if unrecognized property
				if(prp->getType()==VAR_ID){
				std::string serr;
				std::string sva(ncap_var_is_att(var1)?"Attribute" :"Variable");
				serr= sva+" " + std::string(var1->nm)+ " has unrecognized property "+ "\""+prp->getText()+"\"";
				err_prn(fnc_nm,serr );
				}
				
				// Initial scan
				if(prs_arg->ntl_scn){
				
				switch(prp->getType()){
				case PSIZE:
				case PTYPE:
				var= ncap_sclr_var_mk("_property",NC_INT,false);
				break;  
				
				case PMIN:
				case PMAX:
				case PSUM:
				case PAVG:
				var=ncap_sclr_var_mk("_property",var1->type,false);
				break;
				} 
				var1=nco_var_free(var1);
				}
				
				// Final scan
				if(!prs_arg->ntl_scn){
				
				switch(prp->getType()){
				case PSIZE:
				var= ncap_sclr_var_mk("_property",var1->sz);
				var1=nco_var_free(var1); 
				break;
				case PTYPE:
				var= ncap_sclr_var_mk("_property",(int)var1->type);
				var1=nco_var_free(var1); 
				break;
				
				case PMIN:
				var=nco_var_avg(var1,var1->dim,var1->nbr_dim,nco_op_min,False,&ddra_info);
				break; 
				case PMAX:
				var=nco_var_avg(var1,var1->dim,var1->nbr_dim,nco_op_max,False,&ddra_info);
				break;
				case PSUM:
				var=nco_var_avg(var1,var1->dim,var1->nbr_dim,nco_op_ttl,False,&ddra_info);
				break;
				case PAVG:
				var=nco_var_avg(var1,var1->dim,var1->nbr_dim,nco_op_avg,False,&ddra_info);
				break;
				
				
				} 
				//var1=nco_var_free(var1);
				// var1 is freed in nco_var_avg()
				
				}
				
				end_dot: ;
				
				
#line 1838 "ncoTree.cpp"
			}
			break;
		}
		case BYTE:
		{
			c = _t;
			match(_t,BYTE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1466 "ncoGrammer.g"
				
				int ival;
				nc_type type=NC_BYTE;
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_byte");
				var->nbr_dim=0;
				var->sz=1;
				var->type=type;
				var->typ_dsk=type;
				// Get nco type
				if(!prs_arg->ntl_scn){
				ival=atoi(c->getText().c_str());
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.bp = (signed char)ival;
				(void)cast_nctype_void(type,&var->val);
				}
				
#line 1870 "ncoTree.cpp"
			}
			break;
		}
		case SHORT:
		{
			s = _t;
			match(_t,SHORT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1489 "ncoGrammer.g"
				
				if(prs_arg->ntl_scn)
				var=ncap_sclr_var_mk("_short",NC_SHORT,false);
				else {
				int ival;
				ival=atoi(s->getText().c_str());
				var=ncap_sclr_var_mk("_short", (short)ival);
				}
				
#line 1890 "ncoTree.cpp"
			}
			break;
		}
		case INT:
		{
			i = _t;
			match(_t,INT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1500 "ncoGrammer.g"
				
				if(prs_arg->ntl_scn)
				var=ncap_sclr_var_mk("_int",NC_INT);
				else {
				int ival;
				ival=atoi(i->getText().c_str());
				var=ncap_sclr_var_mk("_int", ival);
				}
				
#line 1910 "ncoTree.cpp"
			}
			break;
		}
		case FLOAT:
		{
			f = _t;
			match(_t,FLOAT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1511 "ncoGrammer.g"
				
				if(prs_arg->ntl_scn)
				var=ncap_sclr_var_mk("_float",NC_FLOAT,false);
				else {
				float fval;
				fval=atof(f->getText().c_str());
				var=ncap_sclr_var_mk("_float", fval);
				}
				
#line 1930 "ncoTree.cpp"
			}
			break;
		}
		case DOUBLE:
		{
			d = _t;
			match(_t,DOUBLE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1521 "ncoGrammer.g"
				
				if(prs_arg->ntl_scn)
				var=ncap_sclr_var_mk("_double",NC_DOUBLE,false);
				else {
				double dval;
				dval=strtod(d->getText().c_str(),(char**)NULL );
				var=ncap_sclr_var_mk("_double", dval);
				}
				
#line 1950 "ncoTree.cpp"
			}
			break;
		}
		case NSTRING:
		{
			str = _t;
			match(_t,NSTRING);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1531 "ncoGrammer.g"
				
				char *tsng;
				
				tsng=strdup(str->getText().c_str());
				(void)sng_ascii_trn(tsng);            
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_zz@string");
				var->nbr_dim=0;
				var->sz=strlen(tsng);
				var->type=NC_CHAR;
				if(!prs_arg->ntl_scn){
				var->val.vp=(void*)nco_malloc(var->sz*nco_typ_lng(NC_CHAR));
				(void)cast_void_nctype(NC_CHAR,&var->val);
				strncpy(var->val.cp,tsng,(size_t)var->sz);  
				(void)cast_nctype_void(NC_CHAR,&var->val);
				}
				tsng=(char*)nco_free(tsng);      
				
				
#line 1983 "ncoTree.cpp"
			}
			break;
		}
		case DIM_ID_SIZE:
		{
			dval = _t;
			match(_t,DIM_ID_SIZE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1555 "ncoGrammer.g"
				
				string sDim=dval->getText();
				dmn_sct *dmn_fd;
				
				// check output
				if(prs_arg->ntl_scn){
				var=ncap_sclr_var_mk("_dmn",NC_INT,false);
				}else{ 
				// Check output 
				dmn_fd=prs_arg->ptr_dmn_out_vtr->find(sDim);
				// Check input
				if(dmn_fd==NULL)
				dmn_fd=prs_arg->ptr_dmn_in_vtr->find(sDim);
				
				if( dmn_fd==NULL ){
				err_prn(fnc_nm,"Unable to locate dimension " +dval->getText()+ " in input or output files ");
				}
				var=ncap_sclr_var_mk("_dmn",dmn_fd->sz);
				} // end else 
				
#line 2014 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1709 "ncoGrammer.g"
				
				
				NcapVar *Nvar=NULL;
				
				if(prs_arg->ntl_scn)
				Nvar=prs_arg->ptr_int_vtr->find(att->getText());
				
				if(Nvar==NULL) 
				Nvar=prs_arg->ptr_var_vtr->find(att->getText());
				
				var=(var_sct*)NULL;    
				if(Nvar !=NULL)
				var=nco_var_dpl(Nvar->var);
				else    
				// Check input file for attribute
				var=ncap_att_init(att->getText(),prs_arg);
				
				if(!prs_arg->ntl_scn && var==NULL ){
				err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files.");
				}
				
				// if att not found return undefined
				if(prs_arg->ntl_scn && var==NULL )
				var=ncap_var_udf(att->getText().c_str());
				
				
				if(prs_arg->ntl_scn && var->val.vp !=NULL)
				var->val.vp=(void*)nco_free(var->val.vp);
				
				
#line 2055 "ncoTree.cpp"
			}
			break;
		}
		case VALUE_LIST:
		{
			vlst = _t;
			match(_t,VALUE_LIST);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1741 "ncoGrammer.g"
				
				
				char *cp;
				int nbr_lst;
				int idx;
				int tsz;
				
				
				nc_type type=NC_NAT;
				var_sct *var_ret;                        
				RefAST rRef;
				//NcapVector<var_sct*> exp_vtr;
				std::vector<var_sct*> exp_vtr;
				
				rRef=vlst->getFirstChild();
				
				//if Initial scan return undefined
				if(prs_arg->ntl_scn){
				var=ncap_var_udf("_zz@value_list");  
				return var;
				}   
				
				
				while(rRef){
				exp_vtr.push_back(out(rRef));   
				rRef=rRef->getNextSibling();
				}       
				nbr_lst=exp_vtr.size();
				
				// find highest type
				for(idx=0;idx <nbr_lst ;idx++)
				if(exp_vtr[idx]->type > type) type=exp_vtr[idx]->type;     
				
				// convert every element in vector to highest type
				for(idx=0;idx <nbr_lst ;idx++)
				exp_vtr[idx]=nco_var_cnf_typ(type,exp_vtr[idx]);  
				
				var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var_ret); 
				
				/* Overwrite with attribute expression information */
				var_ret->nm=strdup("_zz@value_list");
				var_ret->nbr_dim=0;
				var_ret->sz=nbr_lst;
				var_ret->type=type;
				
				tsz=nco_typ_lng(type);
				var_ret->val.vp=(void*)nco_malloc(nbr_lst*tsz);
				
				for(idx=0;idx <nbr_lst ; idx++){
				cp=(char*)(var_ret->val.vp)+ (ptrdiff_t)(idx*tsz);
				memcpy(cp,exp_vtr[idx]->val.vp,tsz);
				}    
				
				// Free vector        
				for(idx=0 ; idx < nbr_lst ; idx++)
				(void)nco_var_free(exp_vtr[idx]);    
				
				var=var_ret;
				
				
#line 2128 "ncoTree.cpp"
			}
			break;
		}
		default:
			bool synPredMatched232 = false;
			if (((_t->getType() == PLUS))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t232 = _t;
				synPredMatched232 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t231 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp149_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					out(_t);
					_t = _retTree;
					out(_t);
					_t = _retTree;
					_t = __t231;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched232 = false;
				}
				_t = __t232;
				inputState->guessing--;
			}
			if ( synPredMatched232 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t233 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp150_AST_in = _t;
				match(_t,PLUS);
				_t = _t->getFirstChild();
				var1=out(_t);
				_t = _retTree;
				var2=out(_t);
				_t = _retTree;
				_t = __t233;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1287 "ncoGrammer.g"
					var=ncap_var_var_op(var1,var2, PLUS );
#line 2172 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched236 = false;
				if (((_t->getType() == MINUS))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t236 = _t;
					synPredMatched236 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t235 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp151_AST_in = _t;
						match(_t,MINUS);
						_t = _t->getFirstChild();
						out(_t);
						_t = _retTree;
						out(_t);
						_t = _retTree;
						_t = __t235;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched236 = false;
					}
					_t = __t236;
					inputState->guessing--;
				}
				if ( synPredMatched236 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t237 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp152_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					var2=out(_t);
					_t = _retTree;
					_t = __t237;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1289 "ncoGrammer.g"
						var=ncap_var_var_op(var1,var2, MINUS );
#line 2215 "ncoTree.cpp"
					}
				}
				else if ((_t->getType() == MINUS)) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t243 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp153_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					_t = __t243;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1303 "ncoGrammer.g"
						var=ncap_var_var_op(var1,(var_sct*)NULL, MINUS );
#line 2230 "ncoTree.cpp"
					}
				}
				else if ((_t->getType() == PLUS)) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t244 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp154_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					_t = __t244;
					_t = _t->getNextSibling();
				}
				else {
					bool synPredMatched265 = false;
					if (((_t->getType() == VAR_ID))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t265 = _t;
						synPredMatched265 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp155_AST_in = _t;
							match(_t,VAR_ID);
							_t = _t->getNextSibling();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp156_AST_in = _t;
							match(_t,LMT_LIST);
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched265 = false;
						}
						_t = __t265;
						inputState->guessing--;
					}
					if ( synPredMatched265 ) {
						{
						vid = _t;
						match(_t,VAR_ID);
						_t = _t->getNextSibling();
						lmt = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						}
						if ( inputState->guessing==0 ) {
#line 1577 "ncoGrammer.g"
							
							int idx;
							int nbr_dmn;
							char *var_nm;
							var_sct *var_rhs;
							var_sct *var_nw;
							var_sct *var1;
							dmn_sct *dmn_nw;
							RefAST lRef;           
							
							NcapVector<lmt_sct*> lmt_vtr;
							NcapVector<dmn_sct*> dmn_vtr;
							
							
							//if initial scan return undef
							if(prs_arg->ntl_scn){  
							//var=ncap_var_udf(vid->getText().c_str());       
							var=ncap_var_udf("_rhs_undefined");       
							goto end;  // cannot use return var!!
							
							}
							
							var_nm=strdup(vid->getText().c_str()); 
							var_rhs=ncap_var_init(var_nm,prs_arg,false);            
							nbr_dmn=var_rhs->nbr_dim;          
							lRef=lmt;
							
							
							// Now populate lmt_vtr                  
							if( lmt_mk(lRef,lmt_vtr) == 0){
							printf("zero return for lmt_vtr\n");
							nco_exit(EXIT_FAILURE);
							}
							
							if( lmt_vtr.size() != nbr_dmn){
							err_prn(fnc_nm,"Number of hyperslab limits for variable "+ vid->getText()+" doesn't match number of dimensions");
							}
							
							// add dim names to dimension list 
							for(idx=0 ; idx < nbr_dmn;idx++)
							lmt_vtr[idx]->nm=strdup(var_rhs->dim[idx]->nm);   
							
							// fill out limit structure
							for(idx=0 ; idx < nbr_dmn ;idx++)
							(void)nco_lmt_evl(var_rhs->nc_id,lmt_vtr[idx],0L,prs_arg->FORTRAN_IDX_CNV);
							
							// copy lmt_sct to dmn_sct;
							for(idx=0 ;idx <nbr_dmn ; idx++){
							dmn_nw=(dmn_sct*)nco_malloc(sizeof(dmn_sct));
							dmn_nw->nm=strdup(lmt_vtr[idx]->nm);
							
							// Fudge -if the variable is from input then nco_lmt_evl
							// overwrites the dim id's with their input file values
							// we want the dim ids from output  
							dmn_nw->id=var_rhs->dim[idx]->id;
							//dmn_nw->id=lmt_vtr[idx]->id;
							dmn_nw->cnt=lmt_vtr[idx]->cnt;  
							dmn_nw->srt=lmt_vtr[idx]->srt;  
							dmn_nw->end=lmt_vtr[idx]->end;  
							dmn_nw->srd=lmt_vtr[idx]->srd;  
							dmn_vtr.push(dmn_nw);
							}  
							
							// Fudge -- fill out var again -but using dims defined in dmn_vtr
							// We need data in var so that LHS logic in assign can access var shape 
							var_nw=nco_var_fll(var_rhs->nc_id,var_rhs->id,var_nm, dmn_vtr.ptr(0),dmn_vtr.size());
							
							// Now get data from disk - use nco_var_get() 
							(void)nco_var_get(var_nw->nc_id,var_nw); 
							
							/* a hack - we set var->has_dpl_dmn=-1 so we know we are dealing with 
							a hyperslabed var and not a regular var  -- It shouldn't cause 
							any abberant behaviour!! */ 
							
							var_nw->has_dpl_dmn=-1;  
							
							//if variable is scalar -- re-organize in a  new var - loose extraneous material
							if(var_nw->sz ==1) {
							var1=(var_sct *)nco_malloc(sizeof(var_sct));
							/* Set defaults */
							(void)var_dfl_set(var1); 
							var1->nm=strdup(var_nw->nm);
							var1->nbr_dim=0;
							var1->sz=1;
							var1->type=var_nw->type;
							
							var1->val.vp=(void*)nco_malloc(nco_typ_lng(var1->type));
							(void)memcpy( (void*)var1->val.vp,var_nw->val.vp,nco_typ_lng(var1->type));
							var_nw=nco_var_free(var_nw);
							var=var1;
							}else{
							var=var_nw;
							}   
							
							/* Casting a hyperslab --this makes my brain  hurt!!! 
							if(bcst && var->sz >1)
							var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);
							*/
							
							var_rhs=nco_var_free(var_rhs);
							
							//free vectors
							for(idx=0 ; idx < nbr_dmn ; idx++){
							(void)nco_lmt_free(lmt_vtr[idx]);
							(void)nco_dmn_free(dmn_vtr[idx]);
							}    
							var_nm=(char*)nco_free(var_nm);
							
							end: ;
							
							
							
#line 2388 "ncoTree.cpp"
						}
					}
					else if ((_t->getType() == VAR_ID)) {
						v = _t;
						match(_t,VAR_ID);
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1691 "ncoGrammer.g"
							
							
							var=ncap_var_init(v->getText().c_str(), prs_arg,true);
							if(var== NULL){
							if(prs_arg->ntl_scn){
							var=ncap_var_udf(v->getText().c_str());
							return var;
							}else
							nco_exit(EXIT_FAILURE);
							}
							
							// apply cast only if sz >1 
							if(bcst && var->sz >1)
							var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);
							
							
							
#line 2414 "ncoTree.cpp"
						}
					}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(_t);
		}
		}}}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			if ( _t != ANTLR_USE_NAMESPACE(antlr)nullAST )
				_t = _t->getNextSibling();
		} else {
			throw;
		}
	}
	_retTree = _t;
	return var;
}

var_sct * ncoTree::out_asn(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 1809 "ncoGrammer.g"
	var_sct *var;
#line 2438 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_asn_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 1809 "ncoGrammer.g"
	
	const std::string fnc_nm("assign_asn"); 
	
#line 2446 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case VAR_ID:
		{
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1815 "ncoGrammer.g"
				
				int rtyp=0;
				var=ncap_var_init(vid->getText().c_str(),prs_arg,true);
				if(var== (var_sct*)NULL){
				nco_exit(EXIT_FAILURE);
				}
				
				if(vid->getNextSibling())
				rtyp=vid->getNextSibling()->getType();
				if(rtyp==LMT_LIST || rtyp==DMN_LIST) {
				err_prn(fnc_nm,"Invalid Lvalue defintion of variable " +vid->getText() );
				}
				
#line 2472 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1829 "ncoGrammer.g"
				
				// check "output"
				int rtyp;
				NcapVar *Nvar=NULL;
				
				if(prs_arg->ntl_scn)
				Nvar=prs_arg->ptr_int_vtr->find(att->getText());
				
				if(Nvar==NULL) 
				Nvar=prs_arg->ptr_var_vtr->find(att->getText());
				
				var=(var_sct*)NULL;    
				if(Nvar !=NULL)
				var=nco_var_dpl(Nvar->var);
				else    
				var=ncap_att_init(att->getText(),prs_arg);
				
				
				if(!prs_arg->ntl_scn && var==NULL ){
				err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files.");
				}
				
				// if att not found return undefined
				if(prs_arg->ntl_scn && var==NULL )
				var=ncap_var_udf(att->getText().c_str());
				
				
				if(prs_arg->ntl_scn && var->val.vp !=NULL)
				var->val.vp=(void*)nco_free(var->val.vp);
				
				if(att->getNextSibling())
				rtyp=att->getNextSibling()->getType();            
				if(rtyp==LMT_LIST || rtyp==DMN_LIST) {
				err_prn(fnc_nm,"Invalid Lvalue defintion of attribute " +att->getText() );
				
				}
				
				
#line 2521 "ncoTree.cpp"
			}
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(_t);
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		if( inputState->guessing == 0 ) {
			reportError(ex);
			if ( _t != ANTLR_USE_NAMESPACE(antlr)nullAST )
				_t = _t->getNextSibling();
		} else {
			throw;
		}
	}
	_retTree = _t;
	return var;
}

void ncoTree::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* ncoTree::tokenNames[] = {
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
	"\"size\"",
	"\"type\"",
	"\"min\"",
	"\"max\"",
	"\"sum\"",
	"\"avg\"",
	"DOT",
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
	"LMT_DMN",
	0
};



