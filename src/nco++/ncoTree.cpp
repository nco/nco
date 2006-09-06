/* $ANTLR 2.7.6 (20060511): "ncoGrammer.g" -> "ncoTree.cpp"$ */
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
#line 629 "ncoGrammer.g"
	int nbr_dmn=0;
#line 19 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_peek_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		lmt = _t;
		match(_t,LMT_LIST);
		_t = _t->getNextSibling();
		if ( inputState->guessing==0 ) {
#line 631 "ncoGrammer.g"
			
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
#line 644 "ncoGrammer.g"
	
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
#line 649 "ncoGrammer.g"
				
				run(blk->getFirstChild());
				
				
#line 82 "ncoTree.cpp"
			}
			break;
		}
		case EXPR:
		{
			exp = _t;
			match(_t,EXPR);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 654 "ncoGrammer.g"
				
				
				if(exp->getFirstChild()->getType() == ASSIGN){
				if(dbg_lvl_get() > 0){
				dbg_prn("Type ASSIGN "+exp->getFirstChild()->getFirstChild()->getText());
				}
				}
				var=out(exp->getFirstChild());
				var=nco_var_free(var);
				
#line 103 "ncoTree.cpp"
			}
			break;
		}
		case IF:
		{
			iff = _t;
			match(_t,IF);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 666 "ncoGrammer.g"
				
				bool br;
				var_sct *var1;
				RefAST ex;      
					  //Calculate logical expression
					  var1= out( iff->getFirstChild());
					  br=ncap_var_lgcl(var1);
					  var1=nco_var_free(var1);
				
				if(br) { 
				run(iff->getFirstChild()->getNextSibling() );    
					     }else{ 
				// See if else exists 
				ex=iff->getFirstChild()->getNextSibling()->getNextSibling(); 
				if(ex && ex->getType()==ELSE ) run(ex->getFirstChild());
				}
				
				var=(var_sct*)NULL;
				
				
#line 134 "ncoTree.cpp"
			}
			break;
		}
		case ELSE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp70_AST_in = _t;
			match(_t,ELSE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 688 "ncoGrammer.g"
				
				
				
#line 148 "ncoTree.cpp"
			}
			break;
		}
		case DEFDIM:
		{
			def = _t;
			match(_t,DEFDIM);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 692 "ncoGrammer.g"
				
				
				const char *dmn_nm;
				long sz;
				
				dmn_nm=def->getFirstChild()->getText().c_str();
				
				var=out(def->getFirstChild()->getNextSibling());    
				var=nco_var_cnf_typ(NC_INT,var);
				
				(void)cast_void_nctype(NC_INT,&var->val);
				sz=*var->val.lp;
				var=(var_sct*)nco_var_free(var);
				(void)ncap_def_dim(dmn_nm,sz,prs_arg);
				
#line 174 "ncoTree.cpp"
			}
			break;
		}
		case NULL_NODE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp71_AST_in = _t;
			match(_t,NULL_NODE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 709 "ncoGrammer.g"
				
				
#line 187 "ncoTree.cpp"
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

var_sct * ncoTree::assign(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 714 "ncoGrammer.g"
	var_sct *var;
#line 212 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		bool synPredMatched175 = false;
		if (((_t->getType() == ASSIGN))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t175 = _t;
			synPredMatched175 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t174 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp72_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp73_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				_t = __t174;
				_t = _t->getNextSibling();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched175 = false;
			}
			_t = __t175;
			inputState->guessing--;
		}
		if ( synPredMatched175 ) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t176 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp74_AST_in = _t;
			match(_t,ASSIGN);
			_t = _t->getFirstChild();
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			_t = __t176;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 716 "ncoGrammer.g"
				
				
				const std::string fnc_nm("assign_var"); 
				
				if(vid->getFirstChild()) {
				
				switch( vid->getFirstChild()->getType()){
				
				
				// Deal with LHS hyperslab
				case LMT_LIST:{
				int idx;
				int jdx;
				int rcd;
				int nbr_dmn;
				int var_id; 
				char *var_nm;
				
				RefAST lmt_Ref;
				lmt_sct *lmt_ptr;
				var_sct *var_lhs;
				var_sct *var_rhs;
				NcapVector<lmt_sct*> lmt_vtr;          
				
				lmt_Ref=vid->getFirstChild();
				
				bcst=false;
				var_cst=(var_sct*)NULL;
				var=(var_sct*)NULL;
				
				
				var_nm=strdup(vid->getText().c_str());
				
				
				
				// if var isn't in ouptut then copy it there
				rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
				if(rcd != NC_NOERR) {
				var_lhs=ncap_var_init(var_nm,prs_arg,true);
				// copy atts to output
				(void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
				(void)ncap_var_write(var_lhs,prs_arg);
				// Get "new" var_id 
				(void)nco_inq_varid(prs_arg->out_id,var_nm,&var_id);
				} 
				
				
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
				
				} break;
				
				// Deal with LHS casting 
				case DMN_LIST:{
				
				var_sct *var1;
				NcapVector<std::string> str_vtr;
				RefAST  aRef;
				
				
				if(dbg_lvl_get() > 0)
				dbg_prn(fnc_nm,"In ASSIGN/DMN");
				
				
				// set class wide variables
				bcst=true;  
				var_cst=(var_sct*)NULL;
				
				aRef=vid->getFirstChild()->getFirstChild();
				
				// pPut dimension names in vector       
				while(aRef) {
				str_vtr.push(aRef->getText());
				aRef=aRef->getNextSibling();      
				}
				// Cast is applied in VAR_ID action in function out()
				var_cst=ncap_cst_mk(str_vtr,prs_arg);     
				var1=out(vid->getNextSibling());
				
				// Cast isn't applied to naked numbers,
				// or variables of size 1, or attributes
				// so apply it here
				if(var1->sz ==1 )
				var1=ncap_cst_do(var1,var_cst,prs_arg->ntl_scn);
				
				var1->nm=(char*)nco_free(var1->nm);
				
				var1->nm =strdup(vid->getText().c_str());
				
				//Copy return variable
				var=nco_var_dpl(var1);
				
				(void)ncap_var_write(var1,prs_arg);
				
				bcst=false;
				var_cst=nco_var_free(var_cst); 
				
				} break;
				
				}
				
				
				} else {
				// Set class wide variables
				int var_id;
				int rcd;
				var_sct *var1;
				
				bcst=false;
				var_cst=(var_sct*)NULL; 
				
				var1=out(vid->getNextSibling());
				// Save name 
				std::string s_var_rhs(var1->nm);
				(void)nco_free(var1->nm);                
				var1->nm =strdup(vid->getText().c_str());
				
				// Do attribute propagation only if
				// var doesn't already exist
				rcd=nco_inq_varid_flg(prs_arg->out_id,var1->nm ,&var_id);
				
				if(rcd !=NC_NOERR)
				(void)ncap_att_cpy(vid->getText(),s_var_rhs,prs_arg);
				
				//Copy return variable
				var=nco_var_dpl(var1);
				
				// Write var to disk
				(void)ncap_var_write(var1,prs_arg);
				
				} // end else 
				
				
#line 469 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched179 = false;
			if (((_t->getType() == ASSIGN))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t179 = _t;
				synPredMatched179 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t178 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp75_AST_in = _t;
					match(_t,ASSIGN);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp76_AST_in = _t;
					match(_t,ATT_ID);
					_t = _t->getNextSibling();
					_t = __t178;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched179 = false;
				}
				_t = __t179;
				inputState->guessing--;
			}
			if ( synPredMatched179 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t180 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp77_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				att = _t;
				match(_t,ATT_ID);
				_t = _t->getNextSibling();
				_t = __t180;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 932 "ncoGrammer.g"
					
					
					const std::string fnc_nm("assign_att");
					
					switch( att->getNextSibling()->getType()){
					
					case LMT_LIST:{
					;
					} break;
					
					case DMN_LIST:{
					;
					}break;
					
					default: {
					var_sct *var1;
					string sa=att->getText();
					
					if(dbg_lvl_get() > 0)
					dbg_prn(fnc_nm,"Saving attribute " +sa);
					
					var1=out(att->getNextSibling());
					(void)nco_free(var1->nm);
					var1->nm=strdup(sa.c_str());
					
					//var_nw=nco_var_dpl(var);
					NcapVar *Nvar=new NcapVar(sa,var1);
					prs_arg->ptr_var_vtr->push_ow(Nvar);       
					
					// Copy return variable
					var=nco_var_dpl(var1);    
					
					} break; 
					} // end switch 
					
					
#line 545 "ncoTree.cpp"
				}
			}
		else {
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

var_sct * ncoTree::out(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 971 "ncoGrammer.g"
	var_sct *var;
#line 569 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST qus = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST m = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST pls_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST min_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST tim_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST div_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST c = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST s = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST i = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST f = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST d = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST str = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dval = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST aval = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST v = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 971 "ncoGrammer.g"
	
	const std::string fnc_nm("out"); 
		var_sct *var1;
	var_sct *var2;
	
#line 596 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case TIMES:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t190 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp78_AST_in = _t;
			match(_t,TIMES);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t190;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 984 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, TIMES );
#line 617 "ncoTree.cpp"
			}
			break;
		}
		case DIVIDE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t191 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp79_AST_in = _t;
			match(_t,DIVIDE);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t191;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 986 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, DIVIDE );
#line 636 "ncoTree.cpp"
			}
			break;
		}
		case MOD:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t192 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp80_AST_in = _t;
			match(_t,MOD);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t192;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 988 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, MOD);
#line 655 "ncoTree.cpp"
			}
			break;
		}
		case CARET:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t193 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp81_AST_in = _t;
			match(_t,CARET);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t193;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 990 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, CARET);
#line 674 "ncoTree.cpp"
			}
			break;
		}
		case LNOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t194 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp82_AST_in = _t;
			match(_t,LNOT);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t194;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 994 "ncoGrammer.g"
				var=ncap_var_var_op(var1,(var_sct*)NULL, LNOT );
#line 691 "ncoTree.cpp"
			}
			break;
		}
		case INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t197 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp83_AST_in = _t;
			match(_t,INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t197;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1000 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, INC,prs_arg);
#line 708 "ncoTree.cpp"
			}
			break;
		}
		case DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t198 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp84_AST_in = _t;
			match(_t,DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t198;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1002 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, DEC,prs_arg );
#line 725 "ncoTree.cpp"
			}
			break;
		}
		case POST_INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t199 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp85_AST_in = _t;
			match(_t,POST_INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t199;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1005 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, POST_INC,prs_arg);
#line 742 "ncoTree.cpp"
			}
			break;
		}
		case POST_DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t200 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp86_AST_in = _t;
			match(_t,POST_DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t200;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1007 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,(var_sct*)NULL, POST_DEC,prs_arg );
#line 759 "ncoTree.cpp"
			}
			break;
		}
		case QUESTION:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t201 = _t;
			qus = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,QUESTION);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t201;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1010 "ncoGrammer.g"
				
				bool br;
				br=ncap_var_lgcl(var1);
				var1=nco_var_free(var1);
				if(br) 
				var=out(qus->getFirstChild()->getNextSibling());
				else
				var=out(qus->getFirstChild()->getNextSibling()->getNextSibling());      
				
#line 784 "ncoTree.cpp"
			}
			break;
		}
		case FUNC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t202 = _t;
			m = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,FUNC);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t202;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1023 "ncoGrammer.g"
				
				sym_sct * sym_ptr;
				
				sym_ptr= prs_arg->ptr_sym_vtr->find(m->getText());
				if(sym_ptr !=NULL)   var=ncap_var_fnc(var1,sym_ptr); 
				else{
				cout << "Function  " << m->getText() << " not found" << endl;
				exit(1);
				} 
				
#line 810 "ncoTree.cpp"
			}
			break;
		}
		case LAND:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t203 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp87_AST_in = _t;
			match(_t,LAND);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t203;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1037 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LAND );
#line 829 "ncoTree.cpp"
			}
			break;
		}
		case LOR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t204 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp88_AST_in = _t;
			match(_t,LOR);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t204;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1039 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LOR );
#line 848 "ncoTree.cpp"
			}
			break;
		}
		case LTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t205 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp89_AST_in = _t;
			match(_t,LTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t205;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1042 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LTHAN );
#line 867 "ncoTree.cpp"
			}
			break;
		}
		case GTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t206 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp90_AST_in = _t;
			match(_t,GTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t206;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1044 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GTHAN );
#line 886 "ncoTree.cpp"
			}
			break;
		}
		case GEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t207 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp91_AST_in = _t;
			match(_t,GEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t207;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1046 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GEQ );
#line 905 "ncoTree.cpp"
			}
			break;
		}
		case LEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t208 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp92_AST_in = _t;
			match(_t,LEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t208;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1048 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LEQ );
#line 924 "ncoTree.cpp"
			}
			break;
		}
		case EQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t209 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp93_AST_in = _t;
			match(_t,EQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t209;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1050 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, EQ );
#line 943 "ncoTree.cpp"
			}
			break;
		}
		case NEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t210 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp94_AST_in = _t;
			match(_t,NEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t210;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1052 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, NEQ );
#line 962 "ncoTree.cpp"
			}
			break;
		}
		case PLUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t211 = _t;
			pls_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,PLUS_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t211;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1056 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN , prs_arg);
#line 981 "ncoTree.cpp"
			}
			break;
		}
		case MINUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t212 = _t;
			min_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,MINUS_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t212;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1058 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN, prs_arg );
#line 1000 "ncoTree.cpp"
			}
			break;
		}
		case TIMES_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t213 = _t;
			tim_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,TIMES_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t213;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1060 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN, prs_arg );
#line 1019 "ncoTree.cpp"
			}
			break;
		}
		case DIVIDE_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t214 = _t;
			div_asn = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,DIVIDE_ASSIGN);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t214;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1062 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN,prs_arg );
#line 1038 "ncoTree.cpp"
			}
			break;
		}
		case ASSIGN:
		{
			asn = _t;
			match(_t,ASSIGN);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1064 "ncoGrammer.g"
				var=assign(asn);
#line 1050 "ncoTree.cpp"
			}
			break;
		}
		case BYTE:
		{
			c = _t;
			match(_t,BYTE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1069 "ncoGrammer.g"
				
				int ival;
				nc_type type=NC_BYTE;
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_byte");
				var->nbr_dim=0;
				var->sz=1;
				// Get nco type
				ival=atoi(c->getText().c_str());
				var->type=type;
				
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.bp = (signed char)ival;
				(void)cast_nctype_void(type,&var->val);
				
#line 1080 "ncoTree.cpp"
			}
			break;
		}
		case SHORT:
		{
			s = _t;
			match(_t,SHORT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1090 "ncoGrammer.g"
				
				int ival;
				nc_type type=NC_SHORT;
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_short");
				var->nbr_dim=0;
				var->sz=1;
				var->type=type;
				
				ival=atoi(s->getText().c_str());
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.sp = ival;
				(void)cast_nctype_void(type,&var->val);
				
				
#line 1110 "ncoTree.cpp"
			}
			break;
		}
		case INT:
		{
			i = _t;
			match(_t,INT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1110 "ncoGrammer.g"
				
				int ival;
				nc_type type=NC_INT;
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_int");
				var->nbr_dim=0;
				var->sz=1;
				// Get nco type
				ival=atoi(i->getText().c_str());
				var->type=type;
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.lp = ival;
				(void)cast_nctype_void(type,&var->val);
				
#line 1139 "ncoTree.cpp"
			}
			break;
		}
		case FLOAT:
		{
			f = _t;
			match(_t,FLOAT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1130 "ncoGrammer.g"
				
				float  fval;
				nc_type type=NC_FLOAT;
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_float");
				var->nbr_dim=0;
				var->sz=1;
				// Get nco type
				fval=atof(f->getText().c_str());
				var->type=type;
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.fp = fval;
				(void)cast_nctype_void(type,&var->val);
				
#line 1168 "ncoTree.cpp"
			}
			break;
		}
		case DOUBLE:
		{
			d = _t;
			match(_t,DOUBLE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1150 "ncoGrammer.g"
				
				double r;
				nc_type type=NC_DOUBLE;
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_double");
				var->nbr_dim=0;
				var->sz=1;
				// Get nco type
				r=strtod(d->getText().c_str(),(char**)NULL);
				var->type=type;
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.dp = r;
				(void)cast_nctype_void(type,&var->val);
				
#line 1197 "ncoTree.cpp"
			}
			break;
		}
		case NSTRING:
		{
			str = _t;
			match(_t,NSTRING);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1170 "ncoGrammer.g"
				
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
				var->val.vp=(void*)nco_malloc(var->sz*nco_typ_lng(NC_CHAR));
				(void)cast_void_nctype(NC_CHAR,&var->val);
				strncpy(var->val.cp,tsng,(size_t)var->sz);  
				(void)cast_nctype_void(NC_CHAR,&var->val);
				
				tsng=(char*)nco_free(tsng);      
				
				
#line 1229 "ncoTree.cpp"
			}
			break;
		}
		case DIM_ID_SIZE:
		{
			dval = _t;
			match(_t,DIM_ID_SIZE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1193 "ncoGrammer.g"
				
				string sDim=dval->getText();
				dmn_sct *dmn_fd;
				nc_type type=NC_INT;
				
				// check output
				dmn_fd=prs_arg->ptr_dmn_out_vtr->find(sDim);
				
				// Check input
				if(dmn_fd==NULL)
				dmn_fd=prs_arg->ptr_dmn_in_vtr->find(sDim);
				
				
				if( dmn_fd==NULL ){
				err_prn(fnc_nm,"Unable to locate dimension " +dval->getText()+ " in input or output files ");
				}
				
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_dmn");
				var->nbr_dim=0;
				var->sz=1;
				// Get nco type
				
				var->type=type;
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.lp = dmn_fd->sz;
				(void)cast_nctype_void(type,&var->val);
				
				
				
#line 1274 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID_SIZE:
		{
			aval = _t;
			match(_t,ATT_ID_SIZE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1229 "ncoGrammer.g"
				
				long att_sz;
				nc_type type=NC_INT;
				NcapVar *Nvar;
				
				Nvar=prs_arg->ptr_var_vtr->find(aval->getText());
				if(Nvar !=NULL){
				att_sz=Nvar->var->sz;
				}else{
				var_sct *var_tmp;    
				var_tmp=ncap_att_init(aval->getText(),prs_arg);
				if(var_tmp== (var_sct*)NULL)
				err_prn(fnc_nm,"Unable to locate attribute " +aval->getText()+ " in input or output files.");
				att_sz=var_tmp->sz;
				var_tmp=nco_var_free(var_tmp);
				}
				
				var=(var_sct *)nco_malloc(sizeof(var_sct));
				/* Set defaults */
				(void)var_dfl_set(var); 
				/* Overwrite with attribute expression information */
				var->nm=strdup("_att");
				var->nbr_dim=0;
				var->sz=1;
				// Get nco type
				
				var->type=type;
				var->val.vp=(void*)nco_malloc(nco_typ_lng(type));
				(void)cast_void_nctype(type,&var->val);
				*var->val.lp = att_sz;
				(void)cast_nctype_void(type,&var->val);
				
#line 1317 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1381 "ncoGrammer.g"
				
				// check "output"
				NcapVar *Nvar;
				Nvar=prs_arg->ptr_var_vtr->find(att->getText());
				var=(var_sct*)NULL;    
				if(Nvar !=NULL)
				var=nco_var_dpl(Nvar->var);
				else    
				// Check input file for attribute
				var=ncap_att_init(att->getText(),prs_arg);
				
				if(var== (var_sct*)NULL){
				err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files.");
				}
				
				
#line 1344 "ncoTree.cpp"
			}
			break;
		}
		default:
			bool synPredMatched184 = false;
			if (((_t->getType() == PLUS))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t184 = _t;
				synPredMatched184 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t183 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp95_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					out(_t);
					_t = _retTree;
					out(_t);
					_t = _retTree;
					_t = __t183;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched184 = false;
				}
				_t = __t184;
				inputState->guessing--;
			}
			if ( synPredMatched184 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t185 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp96_AST_in = _t;
				match(_t,PLUS);
				_t = _t->getFirstChild();
				var1=out(_t);
				_t = _retTree;
				var2=out(_t);
				_t = _retTree;
				_t = __t185;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 980 "ncoGrammer.g"
					var=ncap_var_var_op(var1,var2, PLUS );
#line 1388 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched188 = false;
				if (((_t->getType() == MINUS))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t188 = _t;
					synPredMatched188 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t187 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp97_AST_in = _t;
						match(_t,MINUS);
						_t = _t->getFirstChild();
						out(_t);
						_t = _retTree;
						out(_t);
						_t = _retTree;
						_t = __t187;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched188 = false;
					}
					_t = __t188;
					inputState->guessing--;
				}
				if ( synPredMatched188 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t189 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp98_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					var2=out(_t);
					_t = _retTree;
					_t = __t189;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 982 "ncoGrammer.g"
						var=ncap_var_var_op(var1,var2, MINUS );
#line 1431 "ncoTree.cpp"
					}
				}
				else if ((_t->getType() == MINUS)) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t195 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp99_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					_t = __t195;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 996 "ncoGrammer.g"
						var=ncap_var_var_op(var1,(var_sct*)NULL, MINUS );
#line 1446 "ncoTree.cpp"
					}
				}
				else if ((_t->getType() == PLUS)) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t196 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp100_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					_t = __t196;
					_t = _t->getNextSibling();
				}
				else {
					bool synPredMatched217 = false;
					if (((_t->getType() == VAR_ID))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t217 = _t;
						synPredMatched217 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t216 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp101_AST_in = _t;
							match(_t,VAR_ID);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp102_AST_in = _t;
							match(_t,LMT_LIST);
							_t = _t->getNextSibling();
							_t = __t216;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched217 = false;
						}
						_t = __t217;
						inputState->guessing--;
					}
					if ( synPredMatched217 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t218 = _t;
						vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
						match(_t,VAR_ID);
						_t = _t->getFirstChild();
						lmt = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						_t = __t218;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1263 "ncoGrammer.g"
							
							
							int idx;
							int nbr_dmn;
							char *var_nm;
							var_sct *var_rhs;
							var_sct *var_nw;
							var_sct *var1;
							dmn_sct *dmn_nw;
							
							NcapVector<lmt_sct*> lmt_vtr;
							NcapVector<dmn_sct*> dmn_vtr;
							
							var_nm=strdup(vid->getText().c_str()); 
							var_rhs=ncap_var_init(var_nm,prs_arg,false);            
							nbr_dmn=var_rhs->nbr_dim;          
							
							// Now populate lmt_vtr                  
							if( lmt_mk(lmt,lmt_vtr) == 0){
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
							
							
							if(bcst && var->sz >1)
							var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);
							
							var_rhs=nco_var_free(var_rhs);
							
							//free vectors
							for(idx=0 ; idx < nbr_dmn ; idx++){
							(void)nco_lmt_free(lmt_vtr[idx]);
							(void)nco_dmn_free(dmn_vtr[idx]);
							}    
							var_nm=(char*)nco_free(var_nm);
							
							
#line 1593 "ncoTree.cpp"
						}
					}
					else if ((_t->getType() == VAR_ID)) {
						v = _t;
						match(_t,VAR_ID);
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1362 "ncoGrammer.g"
							
							
							char *nm;
							nm =strdup(v->getText().c_str());
							var=ncap_var_init(nm, prs_arg,true);
							if(var== (var_sct*)NULL){
							nco_exit(EXIT_FAILURE);
							}
							
							var->undefined=False;
							// apply cast only if sz >1 
							if(bcst && var->sz >1)
							var=ncap_cst_do(var,var_cst,prs_arg->ntl_scn);
							
							// free nm (It is copied in nco_var_fll())
							nm=(char*)nco_free(nm);
							
							
#line 1620 "ncoTree.cpp"
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
#line 1403 "ncoGrammer.g"
	var_sct *var;
#line 1644 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_asn_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 1403 "ncoGrammer.g"
	
	const std::string fnc_nm("assign_asn"); 
	
#line 1652 "ncoTree.cpp"
	
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
#line 1409 "ncoGrammer.g"
				
				var=ncap_var_init(vid->getText().c_str(),prs_arg,true);
				if(var== (var_sct*)NULL){
				nco_exit(EXIT_FAILURE);
				}
				var->undefined=False;
				
				if(vid->getFirstChild() !=NULL) {
				err_prn(fnc_nm,"Invalid Lvalue defintion of variable " +vid->getText() );
				}
				
#line 1676 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1421 "ncoGrammer.g"
				
				// check "output"
				NcapVar *Nvar;
				Nvar=prs_arg->ptr_var_vtr->find(att->getText());
				var=(var_sct*)NULL;    
				
				var = (Nvar !=NULL) ?
				nco_var_dpl(Nvar->var):
				ncap_att_init(att->getText(),prs_arg);
				
				if(var== (var_sct*)NULL)
				err_prn(fnc_nm,"Unable to locate attribute " +att->getText()+ " in input or output files");
				
				if(att->getFirstChild() !=NULL) 
				err_prn(fnc_nm,"Invalid Lvalue defintion of attribute " +att->getText() );
				
				
#line 1704 "ncoTree.cpp"
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
	"NUMBER",
	"NUMBER_DOT",
	"VAR_ATT",
	"DIM_VAL",
	0
};



