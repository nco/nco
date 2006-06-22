/* $ANTLR 2.7.6 (20060530): "ncoGrammer.g" -> "ncoTree.cpp"$ */
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
#line 536 "ncoGrammer.g"
	int nbr_dmn=0;
#line 19 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_peek_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		lmt = _t;
		match(_t,LMT_LIST);
		_t = _t->getNextSibling();
		if ( inputState->guessing==0 ) {
#line 538 "ncoGrammer.g"
			
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
	ANTLR_USE_NAMESPACE(antlr)RefAST ass = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST iff = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
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
#line 553 "ncoGrammer.g"
				
				run(blk->getFirstChild());
				
				
#line 75 "ncoTree.cpp"
			}
			break;
		}
		case ASSIGN:
		{
			ass = _t;
			match(_t,ASSIGN);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 557 "ncoGrammer.g"
				
					  cout << "Type ASSIGN " <<  ass->getFirstChild()->getText() <<endl;
				assign(ass);
				
				
#line 91 "ncoTree.cpp"
			}
			break;
		}
		case IF:
		{
			iff = _t;
			match(_t,IF);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 563 "ncoGrammer.g"
				
				bool br;
				var_sct *var;
				RefAST ex;      
					  //Calculate logical expression
					  var= out( iff->getFirstChild());
					  br=ncap_var_lgcl(var);
					  var=nco_var_free(var);
				
				if(br) { 
				run(iff->getFirstChild()->getNextSibling() );    
					     }else{ 
				// See if else exists 
				ex=iff->getFirstChild()->getNextSibling()->getNextSibling(); 
				if(ex && ex->getType()==ELSE ) run(ex->getFirstChild());
				}
				
#line 119 "ncoTree.cpp"
			}
			break;
		}
		case ELSE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp56_AST_in = _t;
			match(_t,ELSE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 580 "ncoGrammer.g"
				
				
				
#line 133 "ncoTree.cpp"
			}
			break;
		}
		case NULL_NODE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp57_AST_in = _t;
			match(_t,NULL_NODE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 583 "ncoGrammer.g"
				
				
#line 146 "ncoTree.cpp"
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

void ncoTree::assign(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 588 "ncoGrammer.g"
	
	var_sct *var;
	
#line 176 "ncoTree.cpp"
	
	try {      // for error handling
		bool synPredMatched153 = false;
		if (((_t->getType() == ASSIGN))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t153 = _t;
			synPredMatched153 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t152 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp58_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp59_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				_t = __t152;
				_t = _t->getNextSibling();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched153 = false;
			}
			_t = __t153;
			inputState->guessing--;
		}
		if ( synPredMatched153 ) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t154 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp60_AST_in = _t;
			match(_t,ASSIGN);
			_t = _t->getFirstChild();
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			_t = __t154;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 592 "ncoGrammer.g"
				
				
				
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
				printf("zero return for lmt_vtr\n");
				nco_exit(EXIT_FAILURE);
				}
				
				if( lmt_vtr.size() != nbr_dmn){
				(void)fprintf(stderr,"Error: Number of limits doesn't  match number of dimensions\n" );                                      
				nco_exit(EXIT_FAILURE);     
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
				(void)fprintf(stderr,"Error: Mismatch - number of elements on LHS(%li) doesn't equal number of elements on RHS(%li)\n",var_lhs->sz,var_rhs->sz );                                      
				nco_exit(EXIT_FAILURE); 
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
				
				var_nm=(char*)nco_free(var_nm);
				
				// Empty and free vector 
				for(idx=0 ; idx < nbr_dmn ; idx++)
				(void)nco_lmt_free(lmt_vtr[idx]);
				
				} break;
				
				case DMN_LIST:{
				// Deal with LHS casting 
				int dmn_nbr=0;
				char** sbs_lst;
				RefAST aRef;
				
				cout<< "In ASSIGN/DMN\n";
				
				// set class wide variables
				bcst=true;  
				var_cst=(var_sct*)NULL;
				
				sbs_lst=(char**)nco_calloc(NC_MAX_DIMS, sizeof(char*));
				aRef=vid->getFirstChild()->getFirstChild();
				
				// Get dimension names in list       
				while(aRef) {
				sbs_lst[dmn_nbr++]=strdup(aRef->getText().c_str());
				aRef=aRef->getNextSibling();      
				}
				// Cast is applied in VAR_ID action in function out()
				var_cst=ncap_mk_cst(sbs_lst,dmn_nbr,prs_arg);     
				var=out(vid->getNextSibling());
				
				// Cast isn't applied to naked numbers,
				// or variables of size 1, or attributes
				// so apply it here
				if(var->sz ==1 )
				var=ncap_do_cst(var,var_cst,prs_arg->ntl_scn);
				
				var->nm=(char*)nco_free(var->nm);
				
				var->nm =strdup(vid->getText().c_str());
				(void)ncap_var_write(var,prs_arg);
				
				bcst=false;
				var_cst=nco_var_free(var_cst); 
				(void)nco_sng_lst_free(sbs_lst,NC_MAX_DIMS);    
				
				} break;
				
				}
				
				
				} else {
				// Set class wide variables
				bcst=false;
				var_cst=(var_sct*)NULL; 
				
				var=out(vid->getNextSibling());
				// Save name 
				std::string s_var_rhs(var->nm);
				(void)nco_free(var->nm);                
				var->nm =strdup(vid->getText().c_str());
				
				//Do attribute propagation
				(void)ncap_att_cpy(vid->getText(),s_var_rhs,prs_arg);
				
				(void)ncap_var_write(var,prs_arg);
				
				
				
				} // end else 
				
				
#line 412 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched157 = false;
			if (((_t->getType() == ASSIGN))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t157 = _t;
				synPredMatched157 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t156 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp61_AST_in = _t;
					match(_t,ASSIGN);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp62_AST_in = _t;
					match(_t,ATT_ID);
					_t = _t->getNextSibling();
					_t = __t156;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched157 = false;
				}
				_t = __t157;
				inputState->guessing--;
			}
			if ( synPredMatched157 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t158 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp63_AST_in = _t;
				match(_t,ASSIGN);
				_t = _t->getFirstChild();
				att = _t;
				match(_t,ATT_ID);
				_t = _t->getNextSibling();
				_t = __t158;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 790 "ncoGrammer.g"
					
					
					switch( att->getNextSibling()->getType()){
					
					case LMT_LIST:{
					;
					} break;
					
					case DMN_LIST:{
					;
					}break;
					
					default: {
					var_sct *var_nw;
					string sa=att->getText();
					
					cout <<"Saving attribute " << sa <<endl;
					
					var=out(att->getNextSibling());
					//var_nw=nco_var_dpl(var);
					NcapVar *Nvar=new NcapVar(sa,var);
					prs_arg->ptr_var_vtr->push_ow(Nvar);       
					//(void)nco_var_free(var);
					} break; 
					} // end switch 
					
					
#line 479 "ncoTree.cpp"
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
}

var_sct * ncoTree::out(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 820 "ncoGrammer.g"
	var_sct *var;
#line 502 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST m = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST c = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST s = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST i = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST f = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST d = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST str = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST v = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 820 "ncoGrammer.g"
	
		var_sct *var1;
	var_sct *var2;
	
#line 520 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case TIMES:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t168 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp64_AST_in = _t;
			match(_t,TIMES);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t168;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 832 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, TIMES );
#line 541 "ncoTree.cpp"
			}
			break;
		}
		case DIVIDE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t169 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp65_AST_in = _t;
			match(_t,DIVIDE);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t169;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 834 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, DIVIDE );
#line 560 "ncoTree.cpp"
			}
			break;
		}
		case MOD:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t170 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp66_AST_in = _t;
			match(_t,MOD);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t170;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 836 "ncoGrammer.g"
				var=ncap_var_var_mod(var1,var2);
#line 579 "ncoTree.cpp"
			}
			break;
		}
		case CARET:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t171 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp67_AST_in = _t;
			match(_t,CARET);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t171;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 838 "ncoGrammer.g"
				var=ncap_var_var_pwr(var1,var2);
#line 598 "ncoTree.cpp"
			}
			break;
		}
		case LNOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t172 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp68_AST_in = _t;
			match(_t,LNOT);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t172;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 842 "ncoGrammer.g"
				var=ncap_var_var_op(var1,(var_sct*)NULL, LNOT );
#line 615 "ncoTree.cpp"
			}
			break;
		}
		case FUNC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t175 = _t;
			m = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,FUNC);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t175;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 850 "ncoGrammer.g"
				
				sym_sct * sym_ptr;
				
				sym_ptr= prs_arg->ptr_sym_vtr->find(m->getText());
				if(sym_ptr !=NULL)   var=ncap_var_fnc(var1,sym_ptr); 
				else{
				cout << "Function  " << m->getText() << " not found" << endl;
				exit(1);
				} 
				
#line 641 "ncoTree.cpp"
			}
			break;
		}
		case LAND:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t176 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp69_AST_in = _t;
			match(_t,LAND);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t176;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 864 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LAND );
#line 660 "ncoTree.cpp"
			}
			break;
		}
		case LOR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t177 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp70_AST_in = _t;
			match(_t,LOR);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t177;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 866 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LOR );
#line 679 "ncoTree.cpp"
			}
			break;
		}
		case LTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t178 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp71_AST_in = _t;
			match(_t,LTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t178;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 869 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LTHAN );
#line 698 "ncoTree.cpp"
			}
			break;
		}
		case GTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t179 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp72_AST_in = _t;
			match(_t,GTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t179;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 871 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GTHAN );
#line 717 "ncoTree.cpp"
			}
			break;
		}
		case GEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t180 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp73_AST_in = _t;
			match(_t,GEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t180;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 873 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GEQ );
#line 736 "ncoTree.cpp"
			}
			break;
		}
		case LEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t181 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp74_AST_in = _t;
			match(_t,LEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t181;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 875 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LEQ );
#line 755 "ncoTree.cpp"
			}
			break;
		}
		case EQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t182 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp75_AST_in = _t;
			match(_t,EQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t182;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 877 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, EQ );
#line 774 "ncoTree.cpp"
			}
			break;
		}
		case NEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t183 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp76_AST_in = _t;
			match(_t,NEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t183;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 879 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, NEQ );
#line 793 "ncoTree.cpp"
			}
			break;
		}
		case BYTE:
		{
			c = _t;
			match(_t,BYTE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 885 "ncoGrammer.g"
				
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
				
#line 823 "ncoTree.cpp"
			}
			break;
		}
		case SHORT:
		{
			s = _t;
			match(_t,SHORT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 906 "ncoGrammer.g"
				
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
				
				
#line 853 "ncoTree.cpp"
			}
			break;
		}
		case INT:
		{
			i = _t;
			match(_t,INT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 926 "ncoGrammer.g"
				
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
				
#line 882 "ncoTree.cpp"
			}
			break;
		}
		case FLOAT:
		{
			f = _t;
			match(_t,FLOAT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 946 "ncoGrammer.g"
				
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
				
#line 911 "ncoTree.cpp"
			}
			break;
		}
		case DOUBLE:
		{
			d = _t;
			match(_t,DOUBLE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 966 "ncoGrammer.g"
				
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
				
#line 940 "ncoTree.cpp"
			}
			break;
		}
		case NSTRING:
		{
			str = _t;
			match(_t,NSTRING);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 986 "ncoGrammer.g"
				
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
				
#line 971 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1120 "ncoGrammer.g"
				
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
				fprintf(stderr,"unable to locate attribute %s in input or output files\n",att->getText().c_str());
				nco_exit(EXIT_FAILURE);
				}
				
				
#line 999 "ncoTree.cpp"
			}
			break;
		}
		default:
			bool synPredMatched162 = false;
			if (((_t->getType() == PLUS))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t162 = _t;
				synPredMatched162 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t161 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp77_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					out(_t);
					_t = _retTree;
					out(_t);
					_t = _retTree;
					_t = __t161;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched162 = false;
				}
				_t = __t162;
				inputState->guessing--;
			}
			if ( synPredMatched162 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t163 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp78_AST_in = _t;
				match(_t,PLUS);
				_t = _t->getFirstChild();
				var1=out(_t);
				_t = _retTree;
				var2=out(_t);
				_t = _retTree;
				_t = __t163;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 828 "ncoGrammer.g"
					var=ncap_var_var_op(var1,var2, PLUS );
#line 1043 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched166 = false;
				if (((_t->getType() == MINUS))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t166 = _t;
					synPredMatched166 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t165 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp79_AST_in = _t;
						match(_t,MINUS);
						_t = _t->getFirstChild();
						out(_t);
						_t = _retTree;
						out(_t);
						_t = _retTree;
						_t = __t165;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched166 = false;
					}
					_t = __t166;
					inputState->guessing--;
				}
				if ( synPredMatched166 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t167 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp80_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					var2=out(_t);
					_t = _retTree;
					_t = __t167;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 830 "ncoGrammer.g"
						var=ncap_var_var_op(var1,var2, MINUS );
#line 1086 "ncoTree.cpp"
					}
				}
				else if ((_t->getType() == MINUS)) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t173 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp81_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					_t = __t173;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 844 "ncoGrammer.g"
						var=ncap_var_var_op(var1,(var_sct*)NULL, MINUS );
#line 1101 "ncoTree.cpp"
					}
				}
				else if ((_t->getType() == PLUS)) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t174 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp82_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					_t = __t174;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 846 "ncoGrammer.g"
						var= var1;
#line 1116 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched186 = false;
					if (((_t->getType() == VAR_ID))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t186 = _t;
						synPredMatched186 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t185 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp83_AST_in = _t;
							match(_t,VAR_ID);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp84_AST_in = _t;
							match(_t,LMT_LIST);
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
						vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
						match(_t,VAR_ID);
						_t = _t->getFirstChild();
						lmt = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						_t = __t187;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1007 "ncoGrammer.g"
							
							
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
							(void)fprintf(stderr,"Error: Number of limits doesn't match number of dimensions\n" );                                      
							nco_exit(EXIT_FAILURE);     
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
							dmn_nw->id=lmt_vtr[idx]->id;
							dmn_nw->cnt=lmt_vtr[idx]->cnt;  
							dmn_nw->srt=lmt_vtr[idx]->srt;  
							dmn_nw->end=lmt_vtr[idx]->end;  
							dmn_nw->srd=lmt_vtr[idx]->srd;  
							dmn_vtr.push(dmn_nw);
							}  
							
							// Fudge -- fill out var again -but using dims defined in dmn_vtr
							// We need data in var so that LHS logic in assign can access var shape 
							var_nw=nco_var_fll(var_rhs->nc_id,var_rhs->id,var_nm, dmn_vtr.ptr(0),dmn_vtr.size());
							
							// Now get data from disk -can't use nco_var_get() -as it lacks stride
							var_nw->val.vp=(void*)nco_malloc(var_nw->sz*nco_typ_lng(var_nw->typ_dsk));
							if(var_nw->sz >1)  
							(void)nco_get_varm(var_nw->nc_id,var_nw->id,var_nw->srt,var_nw->cnt,var_nw->srd,(long*)NULL,var_nw->val.vp,var_nw->typ_dsk);
							else
							(void)nco_get_var1(var_nw->nc_id,var_nw->id,var_nw->srt,var_nw->val.vp,var_nw->typ_dsk);
							
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
							var=ncap_do_cst(var,var_cst,prs_arg->ntl_scn);
							
							var_rhs=nco_var_free(var_rhs);
							
							//free vectors
							for(idx=0 ; idx < nbr_dmn ; idx++){
							(void)nco_lmt_free(lmt_vtr[idx]);
							(void)nco_dmn_free(dmn_vtr[idx]);
							}    
							var_nm=(char*)nco_free(var_nm);
							
							
#line 1247 "ncoTree.cpp"
						}
					}
					else if ((_t->getType() == VAR_ID)) {
						v = _t;
						match(_t,VAR_ID);
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1100 "ncoGrammer.g"
							
							
							char *nm;
							nm =strdup(v->getText().c_str());
							var=ncap_var_init(nm, prs_arg,true);
							if(var== (var_sct*)NULL){
							nco_exit(EXIT_FAILURE);
							}
							
							var->undefined=False;
							// apply cast only if sz >1 
							if(bcst && var->sz >1)
							var=ncap_do_cst(var,var_cst,prs_arg->ntl_scn);
							
							// free nm (It is copied in nco_var_fll())
							nm=(char*)nco_free(nm);
							
							
#line 1274 "ncoTree.cpp"
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



