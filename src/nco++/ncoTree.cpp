/* $ANTLR 2.7.6 (20070220): "ncoGrammer.g" -> "ncoTree.cpp"$ */
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
#line 837 "ncoGrammer.g"
	int nbr_dmn=0;
#line 19 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_peek_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		lmt = _t;
		match(_t,LMT_LIST);
		_t = _t->getNextSibling();
		if ( inputState->guessing==0 ) {
#line 839 "ncoGrammer.g"
			
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
#line 852 "ncoGrammer.g"
	
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
#line 857 "ncoGrammer.g"
				
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
#line 863 "ncoGrammer.g"
				
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
#line 869 "ncoGrammer.g"
				
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
				
				var=NULL_CEWI;
				
				
#line 145 "ncoTree.cpp"
			}
			break;
		}
		case ELSE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp98_AST_in = _t;
			match(_t,ELSE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 907 "ncoGrammer.g"
				
				
				
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
#line 911 "ncoGrammer.g"
				
				
				const char *dmn_nm;
				long sz;
				
				dmn_nm=def->getFirstChild()->getText().c_str();
				
				var=out(def->getFirstChild()->getNextSibling());    
				var=nco_var_cnf_typ(NC_INT,var);
				
				(void)cast_void_nctype((nc_type)NC_INT,&var->val);
				sz=*var->val.lp;
				var=(var_sct*)nco_var_free(var);
				(void)ncap_def_dim(dmn_nm,sz,prs_arg);
				
#line 185 "ncoTree.cpp"
			}
			break;
		}
		case NULL_NODE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp99_AST_in = _t;
			match(_t,NULL_NODE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 928 "ncoGrammer.g"
				
				
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

var_sct * ncoTree::assign_ntl(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
	bool bram
) {
#line 933 "ncoGrammer.g"
	var_sct *var;
#line 225 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_ntl_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 933 "ncoGrammer.g"
	
	const std::string fnc_nm("assign_ntl"); 
	var=NULL_CEWI;
	
#line 240 "ncoTree.cpp"
	
	try {      // for error handling
		bool synPredMatched219 = false;
		if (((_t->getType() == VAR_ID))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t219 = _t;
			synPredMatched219 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t218 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp100_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp101_AST_in = _t;
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
			vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,VAR_ID);
			_t = _t->getFirstChild();
			lmt = _t;
			match(_t,LMT_LIST);
			_t = _t->getNextSibling();
			_t = __t220;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 938 "ncoGrammer.g"
				
				
				
				if(dbg_lvl_get() > 0)
				dbg_prn(fnc_nm,vid->getText()+"(limits)");
				
				
				const char *var_nm; 
				var_sct *var_lhs;
				NcapVar *Nvar;              
				
				var_nm=vid->getText().c_str();
				
				var_lhs=ncap_var_init(vid->getText(),prs_arg,false);
				if(var_lhs){
				var=nco_var_dpl(var_lhs);
				(void)ncap_var_write(var_lhs,bram,prs_arg);
				} else {
				
				// set var to udf
				var_lhs=ncap_var_udf(var_nm);
				var=nco_var_dpl(var_lhs);
				
				Nvar=new NcapVar(var_lhs);
				(void)prs_arg->ptr_int_vtr->push_ow(Nvar);
				}
				
#line 306 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched223 = false;
			if (((_t->getType() == VAR_ID))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t223 = _t;
				synPredMatched223 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t222 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp102_AST_in = _t;
					match(_t,VAR_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp103_AST_in = _t;
					match(_t,DMN_LIST);
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
				vid1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				dmn = _t;
				match(_t,DMN_LIST);
				_t = _t->getNextSibling();
				_t = __t224;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 966 "ncoGrammer.g"
					
					
					int idx;
					const char *var_nm;
					var_sct *var1;
					std::vector<std::string> str_vtr;
					RefAST  aRef;
					NcapVar *Nvar;
					int str_vtr_sz;
					
					
					if(dbg_lvl_get() > 0)
					dbg_prn(fnc_nm,vid1->getText()+"[dims]");
					
					var_nm=vid1->getText().c_str(); 
					
					// set class wide variables
					bcst=true;  
					var_cst=NULL_CEWI;
					
					aRef=dmn->getFirstChild();
					
					// pPut dimension names in vector       
					while(aRef) {
					str_vtr.push_back(aRef->getText());
					aRef=aRef->getNextSibling();      
					}
					
					//Check that all dims exist
					str_vtr_sz=str_vtr.size();
					for(idx=0 ; idx < str_vtr_sz ; idx++)
					if(   prs_arg->ptr_dmn_in_vtr->findi(str_vtr[idx]) ==-1             
					&& prs_arg->ptr_dmn_out_vtr->findi(str_vtr[idx]) ==-1)      
					break;
					
					// return undef if dim missing 
					if( idx <str_vtr_sz){
					var=NULL_CEWI;
					
					} else {
					
					// Cast is applied in VAR_ID action in function out()
					var_cst=ncap_cst_mk(str_vtr,prs_arg);
					
					var1=out(vid1->getNextSibling());
					if(var1->undefined) {
					var=NULL_CEWI;
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
					ncap_var_write(var1,bram,prs_arg);
					}
					
					if(var_cst)
					var_cst=nco_var_free(var_cst);
					
					bcst=false;   
					
					
#line 421 "ncoTree.cpp"
				}
			}
			else if ((_t->getType() == VAR_ID)) {
				vid2 = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1042 "ncoGrammer.g"
					
					
					
					if(dbg_lvl_get() > 0)
					dbg_prn(fnc_nm,vid2->getText());
					
					
					var_sct *var1;
					
					// Set class wide variables           
					bcst=false;
					var_cst=NULL_CEWI; 
					
					
					// get shape from RHS
					var1=out(vid2->getNextSibling());
					(void)nco_free(var1->nm);                
					var1->nm =strdup(vid2->getText().c_str());
					
					//Copy return variable
					var=nco_var_dpl(var1);
					
					// Write var to int_vtr
					// if var already in int_vtr or var_vtr then write call does nothing
					(void)ncap_var_write(var1,bram,prs_arg);
					
					
					
#line 458 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched227 = false;
				if (((_t->getType() == ATT_ID))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t227 = _t;
					synPredMatched227 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t226 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp104_AST_in = _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp105_AST_in = _t;
						match(_t,LMT_LIST);
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
					att = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
					match(_t,ATT_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp106_AST_in = _t;
					match(_t,LMT_LIST);
					_t = _t->getNextSibling();
					_t = __t228;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1071 "ncoGrammer.g"
						
						;
						
#line 501 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched231 = false;
					if (((_t->getType() == ATT_ID))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t231 = _t;
						synPredMatched231 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t230 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp107_AST_in = _t;
							match(_t,ATT_ID);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp108_AST_in = _t;
							match(_t,LMT_DMN);
							_t = _t->getNextSibling();
							_t = __t230;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched231 = false;
						}
						_t = __t231;
						inputState->guessing--;
					}
					if ( synPredMatched231 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t232 = _t;
						att1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp109_AST_in = _t;
						match(_t,DMN_LIST);
						_t = _t->getNextSibling();
						_t = __t232;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1074 "ncoGrammer.g"
							
							;
							
#line 544 "ncoTree.cpp"
						}
					}
					else if ((_t->getType() == ATT_ID)) {
						att2 = _t;
						match(_t,ATT_ID);
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1077 "ncoGrammer.g"
							
							
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
							
							
#line 572 "ncoTree.cpp"
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

var_sct * ncoTree::assign(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
	bool bram
) {
#line 1100 "ncoGrammer.g"
	var_sct *var;
#line 598 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 1100 "ncoGrammer.g"
	
	const std::string fnc_nm("assign"); 
	var=NULL_CEWI;
	
#line 613 "ncoTree.cpp"
	
	try {      // for error handling
		bool synPredMatched236 = false;
		if (((_t->getType() == VAR_ID))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t236 = _t;
			synPredMatched236 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t235 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp110_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp111_AST_in = _t;
				match(_t,LMT_LIST);
				_t = _t->getNextSibling();
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
			vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,VAR_ID);
			_t = _t->getFirstChild();
			lmt = _t;
			match(_t,LMT_LIST);
			_t = _t->getNextSibling();
			_t = __t237;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1106 "ncoGrammer.g"
				
				
				int idx;
				int nbr_dmn;
				int var_id; 
				const char *var_nm;
				
				if(dbg_lvl_get() > 0)
				dbg_prn(fnc_nm,vid->getText()+"(limits)");
				
				
				RefAST lmt_Ref;
				var_sct *var_lhs;
				var_sct *var_rhs;
				NcapVector<lmt_sct*> lmt_vtr;          
				
				lmt_Ref=vid->getFirstChild();
				
				bcst=false;
				var_cst=NULL_CEWI;
				var=NULL_CEWI;
				NcapVar *Nvar; 
				
				
				
				
				var_nm=vid->getText().c_str();
				
				lmt_Ref=lmt;               
				
				Nvar=prs_arg->ptr_var_vtr->find(var_nm);
				//set ram flag to flag in var_vtr 
				if(Nvar) bram=Nvar->flg_mem;
				
				
				// if var undefined in O or defined but not populated
				if(Nvar && !bram && Nvar->flg_stt==1){              
				// if var isn't in ouptut then copy it there
				var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
				
				// copy atts to output
				(void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
				(void)ncap_var_write(var_lhs,false,prs_arg);
				}
				
				if(Nvar==NULL && !bram){ 
				var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
				// copy atts to output
				(void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
				(void)ncap_var_write(var_lhs,false,prs_arg);
				}
				
				if(Nvar==NULL && bram){
				std::cout<<"Nvar==NULL && bram)\n";
				var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
				(void)ncap_var_write(var_lhs,true,prs_arg);
				}
				
				
				// Get "new" var_id   
				if(!bram){
				(void)nco_inq_varid(prs_arg->out_id,var_nm,&var_id);
				var_lhs=ncap_var_init(vid->getText(),prs_arg,false);
				}
				
				if(bram)
				var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
				
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
				var_rhs=out(vid->getNextSibling());         
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
				
				
				// Put RAM var
				if(bram){
				var_sct *var_tst;
				var_tst=ncap_var_init(vid->getText(),prs_arg,true);
				(void)nco_put_var_mem(var_tst,var_rhs,lmt_vtr);
				(void)ncap_var_write(var_tst,true,prs_arg); 
				}
				
				
				// Regular var
				if(!bram){ 
				long mult_srd=1L;
				std::vector<long> dmn_srt_vtr;
				std::vector<long> dmn_cnt_vtr;
				std::vector<long> dmn_srd_vtr;
				
				for(idx=0;idx<nbr_dmn;idx++){
				dmn_srt_vtr.push_back(lmt_vtr[idx]->srt);
				dmn_cnt_vtr.push_back(lmt_vtr[idx]->cnt);
				dmn_srd_vtr.push_back(lmt_vtr[idx]->srd);  
				mult_srd*=lmt_vtr[idx]->srd;
				} /* end loop over idx */
				
				/* Check for stride */
				if(mult_srd == 1L)
					            (void)nco_put_vara(prs_arg->out_id,var_id,&dmn_srt_vtr[0],&dmn_cnt_vtr[0],var_rhs->val.vp,var_rhs->type);
				else
					            (void)nco_put_vars(prs_arg->out_id,var_id,&dmn_srt_vtr[0],&dmn_cnt_vtr[0],&dmn_srd_vtr[0],var_rhs->val.vp,var_rhs->type);
				
				} // end put block !!
				
				
				
				var_lhs=nco_var_free(var_lhs);
				var_rhs=nco_var_free(var_rhs);
				
				//get variable again from disk!! for return value
				var=ncap_var_init(vid->getText(),prs_arg,true);
				
				
				// Empty and free vector 
				for(idx=0 ; idx < nbr_dmn ; idx++)
				(void)nco_lmt_free(lmt_vtr[idx]);
				
				
#line 809 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched240 = false;
			if (((_t->getType() == VAR_ID))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t240 = _t;
				synPredMatched240 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t239 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp112_AST_in = _t;
					match(_t,VAR_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp113_AST_in = _t;
					match(_t,DMN_LIST);
					_t = _t->getNextSibling();
					_t = __t239;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched240 = false;
				}
				_t = __t240;
				inputState->guessing--;
			}
			if ( synPredMatched240 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t241 = _t;
				vid1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				dmn = _t;
				match(_t,DMN_LIST);
				_t = _t->getNextSibling();
				_t = __t241;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1265 "ncoGrammer.g"
					
					
					var_sct *var1;
					std::vector<std::string> str_vtr;
					RefAST  aRef;
					
					if(dbg_lvl_get() > 0)
					dbg_prn(fnc_nm,vid1->getText()+"[dims]");
					
					
					// set class wide variables
					bcst=true;  
					var_cst=NULL_CEWI;
					
					//aRef=vid->getFirstChild()->getFirstChild();
					aRef=dmn->getFirstChild();
					
					// pPut dimension names in vector       
					while(aRef) {
					str_vtr.push_back(aRef->getText());
					aRef=aRef->getNextSibling();      
					}
					// Cast is applied in VAR_ID action in function out()
					var_cst=ncap_cst_mk(str_vtr,prs_arg);     
					var1=out(vid1->getNextSibling());
					
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
					
					//Copy return variable<
					var=nco_var_dpl(var1);
					
					//call to nco_var_get() in ncap_var_init() uses this property
					var1->typ_dsk=var1->type;
					(void)ncap_var_write(var1,bram,prs_arg);
					
					bcst=false;
					var_cst=nco_var_free(var_cst); 
					
					
#line 915 "ncoTree.cpp"
				}
			}
			else if ((_t->getType() == VAR_ID)) {
				vid2 = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1332 "ncoGrammer.g"
					
					// Set class wide variables
					
					var_sct *var1;
					NcapVar *Nvar;
					
					if(dbg_lvl_get() > 0)
					dbg_prn(fnc_nm,vid2->getText());
					
					
					
					bcst=false;
					var_cst=NULL_CEWI; 
					
					
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
					(void)ncap_var_write(var1,bram,prs_arg);
					
					
#line 963 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched244 = false;
				if (((_t->getType() == ATT_ID))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t244 = _t;
					synPredMatched244 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t243 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp114_AST_in = _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp115_AST_in = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						_t = __t243;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched244 = false;
					}
					_t = __t244;
					inputState->guessing--;
				}
				if ( synPredMatched244 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t245 = _t;
					att = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
					match(_t,ATT_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp116_AST_in = _t;
					match(_t,LMT_LIST);
					_t = _t->getNextSibling();
					_t = __t245;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1372 "ncoGrammer.g"
						
						;
						
#line 1006 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched248 = false;
					if (((_t->getType() == ATT_ID))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t248 = _t;
						synPredMatched248 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t247 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp117_AST_in = _t;
							match(_t,ATT_ID);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp118_AST_in = _t;
							match(_t,LMT_DMN);
							_t = _t->getNextSibling();
							_t = __t247;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched248 = false;
						}
						_t = __t248;
						inputState->guessing--;
					}
					if ( synPredMatched248 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t249 = _t;
						att1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp119_AST_in = _t;
						match(_t,DMN_LIST);
						_t = _t->getNextSibling();
						_t = __t249;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1375 "ncoGrammer.g"
							
							;
							
#line 1049 "ncoTree.cpp"
						}
					}
					else if ((_t->getType() == ATT_ID)) {
						att2 = _t;
						match(_t,ATT_ID);
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1378 "ncoGrammer.g"
							
							
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
							
							
#line 1078 "ncoTree.cpp"
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

var_sct * ncoTree::out(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 1401 "ncoGrammer.g"
	var_sct *var;
#line 1102 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST qus = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST m = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST pls_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST min_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST tim_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST div_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST prp = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST mtd = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST args = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_float = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_double = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_int = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_short = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_byte = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_ubyte = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_ushort = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_uint = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_int64 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST val_uint64 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST str = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dval = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST v = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vlst = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 1401 "ncoGrammer.g"
	
	const std::string fnc_nm("out"); 
		var_sct *var1;
	var_sct *var2;
	var=NULL_CEWI;
	
#line 1138 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case DIVIDE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t272 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp120_AST_in = _t;
			match(_t,DIVIDE);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t272;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1423 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, DIVIDE );
#line 1159 "ncoTree.cpp"
			}
			break;
		}
		case MOD:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t273 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp121_AST_in = _t;
			match(_t,MOD);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t273;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1425 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, MOD);
#line 1178 "ncoTree.cpp"
			}
			break;
		}
		case CARET:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t274 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp122_AST_in = _t;
			match(_t,CARET);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t274;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1427 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, CARET);
#line 1197 "ncoTree.cpp"
			}
			break;
		}
		case LNOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t275 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp123_AST_in = _t;
			match(_t,LNOT);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t275;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1431 "ncoGrammer.g"
				var=ncap_var_var_op(var1,NULL_CEWI, LNOT );
#line 1214 "ncoTree.cpp"
			}
			break;
		}
		case INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t278 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp124_AST_in = _t;
			match(_t,INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t278;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1437 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,NULL_CEWI,INC,false,prs_arg);
#line 1231 "ncoTree.cpp"
			}
			break;
		}
		case DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t279 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp125_AST_in = _t;
			match(_t,DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t279;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1439 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,NULL_CEWI, DEC,false,prs_arg );
#line 1248 "ncoTree.cpp"
			}
			break;
		}
		case POST_INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t280 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp126_AST_in = _t;
			match(_t,POST_INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t280;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1441 "ncoGrammer.g"
				
				var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,false,prs_arg);
				
#line 1267 "ncoTree.cpp"
			}
			break;
		}
		case POST_DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t281 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp127_AST_in = _t;
			match(_t,POST_DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t281;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1444 "ncoGrammer.g"
				
				var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,false,prs_arg);
				
#line 1286 "ncoTree.cpp"
			}
			break;
		}
		case QUESTION:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t282 = _t;
			qus = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,QUESTION);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t282;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1449 "ncoGrammer.g"
				
				bool br;
				
				// if initial scan 
				if(prs_arg->ntl_scn){
				var1=nco_var_free(var1);
				var=ncap_var_udf("~question"); 
				return var;
				}
				
				br=ncap_var_lgcl(var1);
				var1=nco_var_free(var1);
				
				if(br) 
				var=out(qus->getFirstChild()->getNextSibling());
				else
				var=out(qus->getFirstChild()->getNextSibling()->getNextSibling());      
				
#line 1320 "ncoTree.cpp"
			}
			break;
		}
		case FUNC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t283 = _t;
			m = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,FUNC);
			_t = _t->getFirstChild();
			ANTLR_USE_NAMESPACE(antlr)RefAST __t284 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp128_AST_in = _t;
			match(_t,FUNC_ARG);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t284;
			_t = _t->getNextSibling();
			_t = __t283;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1471 "ncoGrammer.g"
				
				sym_sct * sym_ptr;
				
				sym_ptr= prs_arg->ptr_sym_vtr->find(m->getText());
				if(sym_ptr ==NULL) { 
				cout << "Function  " << m->getText() << " not found" << endl;
				exit(1);
				} 
				// Catch delete function
				if(m->getText() == "delete"){
				;
				
				} else     
				var=ncap_var_fnc(var1,sym_ptr);
				
				
#line 1358 "ncoTree.cpp"
			}
			break;
		}
		case LAND:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t285 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp129_AST_in = _t;
			match(_t,LAND);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t285;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1491 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LAND );
#line 1377 "ncoTree.cpp"
			}
			break;
		}
		case LOR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t286 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp130_AST_in = _t;
			match(_t,LOR);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t286;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1493 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LOR );
#line 1396 "ncoTree.cpp"
			}
			break;
		}
		case LTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t287 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp131_AST_in = _t;
			match(_t,LTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t287;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1496 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LTHAN );
#line 1415 "ncoTree.cpp"
			}
			break;
		}
		case GTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t288 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp132_AST_in = _t;
			match(_t,GTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t288;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1498 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GTHAN );
#line 1434 "ncoTree.cpp"
			}
			break;
		}
		case GEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t289 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp133_AST_in = _t;
			match(_t,GEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t289;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1500 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GEQ );
#line 1453 "ncoTree.cpp"
			}
			break;
		}
		case LEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t290 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp134_AST_in = _t;
			match(_t,LEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t290;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1502 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LEQ );
#line 1472 "ncoTree.cpp"
			}
			break;
		}
		case EQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t291 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp135_AST_in = _t;
			match(_t,EQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t291;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1504 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, EQ );
#line 1491 "ncoTree.cpp"
			}
			break;
		}
		case NEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t292 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp136_AST_in = _t;
			match(_t,NEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t292;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1506 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, NEQ );
#line 1510 "ncoTree.cpp"
			}
			break;
		}
		case PLUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t293 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp137_AST_in = _t;
			match(_t,PLUS_ASSIGN);
			_t = _t->getFirstChild();
			pls_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t293;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1509 "ncoGrammer.g"
				
				var1=out_asn(pls_asn);
				var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN ,(pls_asn->getType()==TIMES), prs_arg);
				
#line 1533 "ncoTree.cpp"
			}
			break;
		}
		case MINUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t294 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp138_AST_in = _t;
			match(_t,MINUS_ASSIGN);
			_t = _t->getFirstChild();
			min_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t294;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1513 "ncoGrammer.g"
				
				var1=out_asn(min_asn);
				var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN ,(min_asn->getType()==TIMES), prs_arg);
				
#line 1556 "ncoTree.cpp"
			}
			break;
		}
		case TIMES_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t295 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp139_AST_in = _t;
			match(_t,TIMES_ASSIGN);
			_t = _t->getFirstChild();
			tim_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t295;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1517 "ncoGrammer.g"
				
				var1=out_asn(tim_asn);
				var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN ,(tim_asn->getType()==TIMES), prs_arg);
				
#line 1579 "ncoTree.cpp"
			}
			break;
		}
		case DIVIDE_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t296 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp140_AST_in = _t;
			match(_t,DIVIDE_ASSIGN);
			_t = _t->getFirstChild();
			div_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t296;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1521 "ncoGrammer.g"
					
				var1=out_asn(div_asn);
				var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN ,(div_asn->getType()==TIMES), prs_arg);
				
#line 1602 "ncoTree.cpp"
			}
			break;
		}
		case ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t297 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp141_AST_in = _t;
			match(_t,ASSIGN);
			_t = _t->getFirstChild();
			asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t297;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1527 "ncoGrammer.g"
				
				// Check for RAM variable - if present 
				// change tree - for example from:
				//     ( EXPR ( = ( * n1 ) ( + four four ) ) )
				// to  ( EXPR ( = n1 ( + four four ) ) )
				RefAST tr;
				bool bram;
				if(asn->getType()==TIMES){
				tr=asn->getFirstChild();
				tr->setNextSibling(asn->getNextSibling());
				bram=true;
				} else { 
				tr=asn; 
				bram=false;
				}
				
				if(prs_arg->ntl_scn)
				var=assign_ntl(tr,bram); 
				else
				var=assign(tr,bram);
				
				
#line 1641 "ncoTree.cpp"
			}
			break;
		}
		case PROP:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t298 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp142_AST_in = _t;
			match(_t,PROP);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			prp = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t298;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1551 "ncoGrammer.g"
				
				
				if(prs_arg->ntl_scn){
				var1=nco_var_free(var1);
				var=ncap_sclr_var_mk(static_cast<std::string>("~property"),(nc_type)NC_INT,false);        
				} else { 
				
				switch(prp->getType()){ 
				case PSIZE:
				var=ncap_sclr_var_mk(static_cast<std::string>("~property"),(nco_int)var1->sz);
				break;
				case PTYPE:
				var=ncap_sclr_var_mk(static_cast<std::string>("~property"),(nco_int)var1->type);
				break;
				case PNDIMS:
				var=ncap_sclr_var_mk(static_cast<std::string>("~property"),(nco_int)var1->nbr_dim);            
				
				} // end switch
				var1=nco_var_free(var1); 
				}
				
#line 1681 "ncoTree.cpp"
			}
			break;
		}
		case DOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t299 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp143_AST_in = _t;
			match(_t,DOT);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			mtd = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			args = _t;
			match(_t,FUNC_ARG);
			_t = _t->getNextSibling();
			_t = __t299;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1574 "ncoGrammer.g"
				
				
				int nbr_arg;
				int idx;
				int nbr_dim;
				dmn_sct **dim;
				
				RefAST aRef;
				std::vector<std::string> str_vtr;
				NcapVector<dmn_sct*> dmn_vtr;
				// de-reference 
				ddra_info_sct ddra_info;        
				
				// blow out if unrecognized method
				if(mtd->getType()==VAR_ID){
				std::string serr;
				std::string sva(ncap_var_is_att(var1)?"Attribute" :"Variable");
				serr= sva+" " + std::string(var1->nm)+ " has unrecognized method "+ "\""+mtd->getText()+"\"";
				err_prn(fnc_nm,serr );
				}
				
				// Process method arguments if any exist !! 
				if(args && (nbr_arg=args->getNumberOfChildren()) >0){  
				aRef=args->getFirstChild();
				while(aRef){
				
				switch(aRef->getType()){
				case DIM_ID: 
				case DIM_MTD_ID:{  
				str_vtr.push_back(aRef->getText());
				break;    
				}
				
				// This is garanteed to contain at least one DIM_ID or DIM_MTD  
				// and NOTHING else --no need to type check!!
				case DMN_ARG_LIST: 
				{ RefAST bRef=aRef->getFirstChild();
				while(bRef){
				str_vtr.push_back(bRef->getText());
				bRef=bRef->getNextSibling();
				}  
				break;
				} 
				
				// ignore expr type argument
				default:
				std::string serr;
				serr="Argument \""+aRef->getText()+"\" to method "+mtd->getText()+" is not a dimension";      
				wrn_prn(fnc_nm,serr);
				break;
				
				} // end switch
				aRef=aRef->getNextSibling();
				} // end while
				
				dmn_vtr=ncap_dmn_mtd(var1, str_vtr);
				}           
				
				// Initial scan 
				if(prs_arg->ntl_scn){
				nbr_dim=var1->nbr_dim;
				
				if(var1->undefined)
				var=ncap_var_udf("~dot_methods");  
				// deal with average over all dims or scalar var
				else if( nbr_dim==0 || dmn_vtr.size()== 0 || dmn_vtr.size()==nbr_dim)  
				var=ncap_sclr_var_mk(static_cast<std::string>("~dot_methods"),var1->type,false);    
				else {
				// cast a variable with the correct dims in the correct order
				dim=var1->dim;
				std::vector<std::string> cst_vtr;              
				
				for(idx=0 ; idx < nbr_dim ; idx++){
				std::string sdm(dim[idx]->nm);    
				if( dmn_vtr.findi(sdm) == -1)
				cst_vtr.push_back(sdm);       
				}                
				
				var=ncap_cst_mk(cst_vtr,prs_arg);
				var=nco_var_cnf_typ(var1->type,var);
				}
				
				var1=nco_var_free(var1);
				// NB: cannot use return -- as this results with
				// problems with automagically generated code 
				goto end_dot;
				}
				
				if(dmn_vtr.size() >0){
				dim=&dmn_vtr[0];
				nbr_dim=dmn_vtr.size();                           
				} else {
				dim=var1->dim;
				nbr_dim=var1->nbr_dim; 
				}    
				
				// Final scan
				if(!prs_arg->ntl_scn){
				
				switch(mtd->getType()){
				
				case PAVG:
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_avg,False,&ddra_info);
				// Use tally to normalize
				(void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
				break;
				
				case PAVGSQR:
				var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_avgsqr,False,&ddra_info);
				// Normalize
				(void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
				break;
				
				case PMAX:
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_max,False,&ddra_info);
				break;
				
				case PMIN:
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_min,False,&ddra_info);
				break; 
				
				case PRMS:
				var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_rms,False,&ddra_info);
				// Normalize
				(void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
				// Take root
				(void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
				break;
				
				case PRMSSDN:
				var1=ncap_var_var_op(var1, NULL_CEWI,SQR2);
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_rmssdn,False,&ddra_info);
				// Normalize
				(void)nco_var_nrm_sdn(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
				// Take root
				(void)nco_var_sqrt(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val,var->val);  
				break;
				
				case PSQRAVG:
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_sqravg,False,&ddra_info);
				// Normalize 
				(void)nco_var_nrm(var->type,var->sz,var->has_mss_val,var->mss_val,var->tally,var->val);
				// Square mean
				(void)nco_var_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,var->val);
				break;
				
				case PTTL:
				var=nco_var_avg(var1,dim,nbr_dim,nco_op_ttl,False,&ddra_info);
				break;
				} 
				// var1 is freed in nco_var_avg()
				}
				
				end_dot: ;
				
				
#line 1861 "ncoTree.cpp"
			}
			break;
		}
		case FLOAT:
		{
			val_float = _t;
			match(_t,FLOAT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1735 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~float"),(nc_type)NC_FLOAT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~float"),static_cast<float>(std::strtod(val_float->getText().c_str(),(char **)NULL)));
#line 1873 "ncoTree.cpp"
			}
			break;
		}
		case DOUBLE:
		{
			val_double = _t;
			match(_t,DOUBLE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1737 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~double"),(nc_type)NC_DOUBLE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~double"),strtod(val_double->getText().c_str(),(char **)NULL));
#line 1885 "ncoTree.cpp"
			}
			break;
		}
		case INT:
		{
			val_int = _t;
			match(_t,INT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1739 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int"),(nc_type)NC_INT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int"),static_cast<nco_int>(std::strtol(val_int->getText().c_str(),(char **)NULL,10)));
#line 1897 "ncoTree.cpp"
			}
			break;
		}
		case SHORT:
		{
			val_short = _t;
			match(_t,SHORT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1741 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~short"),(nc_type)NC_SHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~short"),static_cast<nco_short>(std::strtol(val_short->getText().c_str(),(char **)NULL,10)));
#line 1909 "ncoTree.cpp"
			}
			break;
		}
		case BYTE:
		{
			val_byte = _t;
			match(_t,BYTE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1743 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),(nc_type)NC_BYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),static_cast<nco_byte>(std::strtol(val_byte->getText().c_str(),(char **)NULL,10)));
#line 1921 "ncoTree.cpp"
			}
			break;
		}
		case UBYTE:
		{
			val_ubyte = _t;
			match(_t,UBYTE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1748 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),(nc_type)NC_UBYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),static_cast<nco_ubyte>(std::strtoul(val_ubyte->getText().c_str(),(char **)NULL,10)));
#line 1933 "ncoTree.cpp"
			}
			break;
		}
		case USHORT:
		{
			val_ushort = _t;
			match(_t,USHORT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1752 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),(nc_type)NC_USHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),static_cast<nco_ushort>(std::strtoul(val_ushort->getText().c_str(),(char **)NULL,10)));
#line 1945 "ncoTree.cpp"
			}
			break;
		}
		case UINT:
		{
			val_uint = _t;
			match(_t,UINT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1754 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),(nc_type)NC_UINT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),static_cast<nco_uint>(std::strtoul(val_uint->getText().c_str(),(char **)NULL,10)));
#line 1957 "ncoTree.cpp"
			}
			break;
		}
		case INT64:
		{
			val_int64 = _t;
			match(_t,INT64);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1756 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),(nc_type)NC_INT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),sng2nbr(val_int64->getText(),nco_int64_CEWI));
#line 1969 "ncoTree.cpp"
			}
			break;
		}
		case UINT64:
		{
			val_uint64 = _t;
			match(_t,UINT64);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1760 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),(nc_type)NC_UINT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),sng2nbr(val_uint64->getText(),nco_uint64_CEWI));
#line 1981 "ncoTree.cpp"
			}
			break;
		}
		case NSTRING:
		{
			str = _t;
			match(_t,NSTRING);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1765 "ncoGrammer.g"
				
				char *tsng;
				
				tsng=strdup(str->getText().c_str());
				(void)sng_ascii_trn(tsng);            
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("~zz@string");
				var->nbr_dim=0;
				var->sz=strlen(tsng);
				var->type=NC_CHAR;
				if(!prs_arg->ntl_scn){
				var->val.vp=(void*)nco_malloc(var->sz*nco_typ_lng(NC_CHAR));
				(void)cast_void_nctype((nc_type)NC_CHAR,&var->val);
				strncpy(var->val.cp,tsng,(size_t)var->sz);  
				(void)cast_nctype_void((nc_type)NC_CHAR,&var->val);
				}
				tsng=(char*)nco_free(tsng);      
				
#line 2013 "ncoTree.cpp"
			}
			break;
		}
		case DIM_ID_SIZE:
		{
			dval = _t;
			match(_t,DIM_ID_SIZE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1787 "ncoGrammer.g"
				
				string sDim=dval->getText();
				dmn_sct *dmn_fd;
				
				// Check output
				if(prs_arg->ntl_scn){
				var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),(nc_type)NC_INT,false);
				}else{ 
				// Check output 
				dmn_fd=prs_arg->ptr_dmn_out_vtr->find(sDim);
				// Check input
				if(dmn_fd==NULL_CEWI)
				dmn_fd=prs_arg->ptr_dmn_in_vtr->find(sDim);
				
				if( dmn_fd==NULL_CEWI ){
				err_prn(fnc_nm,"Unable to locate dimension " +dval->getText()+ " in input or output files ");
				}
				var=ncap_sclr_var_mk(static_cast<std::string>("~dmn"),dmn_fd->sz);
				} // end else 
				
#line 2044 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1965 "ncoGrammer.g"
				
				
				NcapVar *Nvar=NULL;
				
				if(prs_arg->ntl_scn)
				Nvar=prs_arg->ptr_int_vtr->find(att->getText());
				
				if(Nvar==NULL) 
				Nvar=prs_arg->ptr_var_vtr->find(att->getText());
				
				var=NULL_CEWI;    
				if(Nvar !=NULL)
				var=nco_var_dpl(Nvar->var);
				else    
				// Check input file for attribute
				var=ncap_att_init(att->getText(),prs_arg);
				
				if(!prs_arg->ntl_scn && var==NULL_CEWI ){
				err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files.");
				}
				
				// if att not found return undefined
				if(prs_arg->ntl_scn && var==NULL_CEWI )
				var=ncap_var_udf(att->getText().c_str());
				
				
				if(prs_arg->ntl_scn && var->val.vp !=NULL)
				var->val.vp=(void*)nco_free(var->val.vp);
				
				
#line 2085 "ncoTree.cpp"
			}
			break;
		}
		case VALUE_LIST:
		{
			vlst = _t;
			match(_t,VALUE_LIST);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1997 "ncoGrammer.g"
				
				
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
				//rRef=vlst->getNextSibling();
				
				/*
				if(prs_arg->ntl_scn){
				var=ncap_var_udf("~zz@value_list");  
				return var;
				}   
				*/
				
				while(rRef){
				exp_vtr.push_back(out(rRef));   
				rRef=rRef->getNextSibling();
				}       
				nbr_lst=exp_vtr.size();
				
				// find highest type
				for(idx=0;idx <nbr_lst ;idx++)
				if(exp_vtr[idx]->type > type) type=exp_vtr[idx]->type;     
				
				
				// Inital Scan
				if(prs_arg->ntl_scn){
				
				//skip loop if highest type is double 
				if(type==NC_DOUBLE) idx=nbr_lst;
				
				for(idx=0 ; idx <nbr_lst ; idx++) 
				if(exp_vtr[idx]->undefined) break;
				
				// Exit if an element in the list is "undefined" 
				if(idx < nbr_lst){ 
				var_ret=ncap_var_udf("~zz@value_list");  
				goto end_val;  
				}
				
				var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var_ret); 
				
				/* Overwrite with attribute expression information */
				var_ret->nm=strdup("~zz@value_list");
				var_ret->nbr_dim=0;
				var_ret->sz=nbr_lst;
				var_ret->type=type;
				
				goto end_val;          
				
				} // end initial scan
				
				// convert every element in vector to highest type
				for(idx=0;idx <nbr_lst ;idx++)
				exp_vtr[idx]=nco_var_cnf_typ(type,exp_vtr[idx]);  
				
				var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var_ret); 
				
				/* Overwrite with attribute expression information */
				var_ret->nm=strdup("~zz@value_list");
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
				end_val: for(idx=0 ; idx < nbr_lst ; idx++)
				(void)nco_var_free(exp_vtr[idx]);    
				
				var=var_ret;
				
				
#line 2189 "ncoTree.cpp"
			}
			break;
		}
		default:
			bool synPredMatched253 = false;
			if (((_t->getType() == PLUS))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t253 = _t;
				synPredMatched253 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t252 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp144_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					out(_t);
					_t = _retTree;
					out(_t);
					_t = _retTree;
					_t = __t252;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched253 = false;
				}
				_t = __t253;
				inputState->guessing--;
			}
			if ( synPredMatched253 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t254 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp145_AST_in = _t;
				match(_t,PLUS);
				_t = _t->getFirstChild();
				var1=out(_t);
				_t = _retTree;
				var2=out(_t);
				_t = _retTree;
				_t = __t254;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1411 "ncoGrammer.g"
					var=ncap_var_var_op(var1,var2, PLUS );
#line 2233 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched257 = false;
				if (((_t->getType() == MINUS))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t257 = _t;
					synPredMatched257 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t256 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp146_AST_in = _t;
						match(_t,MINUS);
						_t = _t->getFirstChild();
						out(_t);
						_t = _retTree;
						out(_t);
						_t = _retTree;
						_t = __t256;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched257 = false;
					}
					_t = __t257;
					inputState->guessing--;
				}
				if ( synPredMatched257 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t258 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp147_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					var2=out(_t);
					_t = _retTree;
					_t = __t258;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1413 "ncoGrammer.g"
						var=ncap_var_var_op(var1,var2, MINUS );
#line 2276 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched262 = false;
					if (((_t->getType() == TIMES))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t262 = _t;
						synPredMatched262 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t260 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp148_AST_in = _t;
							match(_t,TIMES);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST __t261 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp149_AST_in = _t;
							match(_t,POST_INC);
							_t = _t->getFirstChild();
							out(_t);
							_t = _retTree;
							_t = __t261;
							_t = _t->getNextSibling();
							_t = __t260;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched262 = false;
						}
						_t = __t262;
						inputState->guessing--;
					}
					if ( synPredMatched262 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t263 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp150_AST_in = _t;
						match(_t,TIMES);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST __t264 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp151_AST_in = _t;
						match(_t,POST_INC);
						_t = _t->getFirstChild();
						var1=out_asn(_t);
						_t = _retTree;
						_t = __t264;
						_t = _t->getNextSibling();
						_t = __t263;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1414 "ncoGrammer.g"
							
							var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,true,prs_arg);      
							
#line 2329 "ncoTree.cpp"
						}
					}
					else {
						bool synPredMatched268 = false;
						if (((_t->getType() == TIMES))) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t268 = _t;
							synPredMatched268 = true;
							inputState->guessing++;
							try {
								{
								ANTLR_USE_NAMESPACE(antlr)RefAST __t266 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp152_AST_in = _t;
								match(_t,TIMES);
								_t = _t->getFirstChild();
								ANTLR_USE_NAMESPACE(antlr)RefAST __t267 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp153_AST_in = _t;
								match(_t,POST_DEC);
								_t = _t->getFirstChild();
								out(_t);
								_t = _retTree;
								_t = __t267;
								_t = _t->getNextSibling();
								_t = __t266;
								_t = _t->getNextSibling();
								}
							}
							catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
								synPredMatched268 = false;
							}
							_t = __t268;
							inputState->guessing--;
						}
						if ( synPredMatched268 ) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t269 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp154_AST_in = _t;
							match(_t,TIMES);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST __t270 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp155_AST_in = _t;
							match(_t,POST_DEC);
							_t = _t->getFirstChild();
							var1=out_asn(_t);
							_t = _retTree;
							_t = __t270;
							_t = _t->getNextSibling();
							_t = __t269;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 1417 "ncoGrammer.g"
								
								var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,true,prs_arg);      
								
#line 2382 "ncoTree.cpp"
							}
						}
						else if ((_t->getType() == TIMES)) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t271 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp156_AST_in = _t;
							match(_t,TIMES);
							_t = _t->getFirstChild();
							var1=out(_t);
							_t = _retTree;
							var2=out(_t);
							_t = _retTree;
							_t = __t271;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 1421 "ncoGrammer.g"
								var=ncap_var_var_op(var1,var2, TIMES );
#line 2399 "ncoTree.cpp"
							}
						}
						else if ((_t->getType() == MINUS)) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t276 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp157_AST_in = _t;
							match(_t,MINUS);
							_t = _t->getFirstChild();
							var1=out(_t);
							_t = _retTree;
							_t = __t276;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 1433 "ncoGrammer.g"
								var=ncap_var_var_op(var1,NULL_CEWI, MINUS );
#line 2414 "ncoTree.cpp"
							}
						}
						else if ((_t->getType() == PLUS)) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t277 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp158_AST_in = _t;
							match(_t,PLUS);
							_t = _t->getFirstChild();
							var1=out(_t);
							_t = _retTree;
							_t = __t277;
							_t = _t->getNextSibling();
						}
						else {
							bool synPredMatched302 = false;
							if (((_t->getType() == VAR_ID))) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t302 = _t;
								synPredMatched302 = true;
								inputState->guessing++;
								try {
									{
									ANTLR_USE_NAMESPACE(antlr)RefAST __t301 = _t;
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp159_AST_in = _t;
									match(_t,VAR_ID);
									_t = _t->getFirstChild();
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp160_AST_in = _t;
									match(_t,LMT_LIST);
									_t = _t->getNextSibling();
									_t = __t301;
									_t = _t->getNextSibling();
									}
								}
								catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
									synPredMatched302 = false;
								}
								_t = __t302;
								inputState->guessing--;
							}
							if ( synPredMatched302 ) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t303 = _t;
								vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
								match(_t,VAR_ID);
								_t = _t->getFirstChild();
								lmt = _t;
								match(_t,LMT_LIST);
								_t = _t->getNextSibling();
								_t = __t303;
								_t = _t->getNextSibling();
								if ( inputState->guessing==0 ) {
#line 1809 "ncoGrammer.g"
									
									bool bram;   // Check for a RAM variable
									int idx;
									int nbr_dmn;
									const char *var_nm;
									var_sct *var_rhs;
									var_sct *var_nw;
									dmn_sct *dmn_nw;
									
									NcapVar *Nvar;
									RefAST lRef;           
									
									NcapVector<lmt_sct*> lmt_vtr;
									NcapVector<dmn_sct*> dmn_vtr;
									
									
									//if initial scan return undef
									if(prs_arg->ntl_scn){  
									//var=ncap_var_udf(vid->getText().c_str());       
									var=ncap_var_udf("~rhs_undefined");       
									goto end;  // cannot use return var!!
									
									}
									
									
									
									var_nm=vid->getText().c_str(); 
									var_rhs=ncap_var_init(vid->getText(),prs_arg,false);            
									nbr_dmn=var_rhs->nbr_dim;          
									lRef=lmt;
									
									// Check for RAM variable
									Nvar=prs_arg->ptr_var_vtr->find(var_nm);
									if(Nvar && Nvar->flg_mem){ 
									bram=true;
									var_rhs=nco_var_free(var_rhs);
									
									var_rhs=Nvar->cpyVar();
									var_rhs->nc_id=prs_arg->out_id;
									}else{
									bram=false;
									}
									
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
									dmn_vtr.push_back(dmn_nw);
									}  
									
									
									if(!bram){
									// Fudge -- fill out var again -but using dims defined in dmn_vtr
									// We need data in var so that LHS logic in assign can access var shape 
									var_nw=nco_var_fll(var_rhs->nc_id,var_rhs->id,var_nm, &dmn_vtr[0],dmn_vtr.size());
									
									// Now get data from disk - use nco_var_get() 
									(void)nco_var_get(var_nw->nc_id,var_nw); 
									}
									
									// Ram variable -do an in memory get  
									if(bram){
									
									//Do an in memory get 
									var_nw=nco_get_var_mem(var_rhs,dmn_vtr);
									//var_nw=nco_var_dpl(var_rhs);
									}
									
									
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
									end: ;
									
									
#line 2600 "ncoTree.cpp"
								}
							}
							else if ((_t->getType() == VAR_ID)) {
								v = _t;
								match(_t,VAR_ID);
								_t = _t->getNextSibling();
								if ( inputState->guessing==0 ) {
#line 1947 "ncoGrammer.g"
									
									
									var=ncap_var_init(v->getText(), prs_arg,true);
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
									
									
									
#line 2626 "ncoTree.cpp"
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

var_sct * ncoTree::out_asn(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 2096 "ncoGrammer.g"
	var_sct *var;
#line 2650 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_asn_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 2096 "ncoGrammer.g"
	
	const std::string fnc_nm("assign_asn");
	var=NULL_CEWI; 
	
#line 2660 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case TIMES:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t305 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp161_AST_in = _t;
			match(_t,TIMES);
			_t = _t->getFirstChild();
			vid1 = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			_t = __t305;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2103 "ncoGrammer.g"
				
				if(vid1->getFirstChild())
				err_prn(fnc_nm,"Invalid Lvalue " +vid1->getText() );
				
				var=ncap_var_init(vid1->getText(),prs_arg,true);
				if(var== NULL_CEWI){
				nco_exit(EXIT_FAILURE);
				}
				
				
#line 2689 "ncoTree.cpp"
			}
			break;
		}
		case VAR_ID:
		{
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2114 "ncoGrammer.g"
				
				if(vid->getFirstChild())
				err_prn(fnc_nm,"Invalid Lvalue " +vid->getText() );
				
				var=ncap_var_init(vid->getText(),prs_arg,true);
				if(var== NULL_CEWI){
				nco_exit(EXIT_FAILURE);
				}
				
				
				
				
#line 2712 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2127 "ncoGrammer.g"
				
				// check "output"
				NcapVar *Nvar=NULL;
				
				if(att->getFirstChild())
				err_prn(fnc_nm,"Invalid Lvalue " +att->getText() );
				
				
				if(prs_arg->ntl_scn)
				Nvar=prs_arg->ptr_int_vtr->find(att->getText());
				
				if(Nvar==NULL) 
				Nvar=prs_arg->ptr_var_vtr->find(att->getText());
				
				var=NULL_CEWI;    
				if(Nvar !=NULL)
				var=nco_var_dpl(Nvar->var);
				else    
				var=ncap_att_init(att->getText(),prs_arg);
				
				
				if(!prs_arg->ntl_scn && var==NULL_CEWI ){
				err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files.");
				}
				
				// if att not found return undefined
				if(prs_arg->ntl_scn && var==NULL_CEWI )
				var=ncap_var_udf(att->getText().c_str());
				
				
				if(prs_arg->ntl_scn && var->val.vp !=NULL)
				var->val.vp=(void*)nco_free(var->val.vp);
				
				
				
#line 2758 "ncoTree.cpp"
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
	";",
	"{",
	"}",
	"\"if\"",
	"(",
	")",
	"\"else\"",
	"VAR_ID",
	"ATT_ID",
	"\"defdim\"",
	"a string",
	",",
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



