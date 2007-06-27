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
#line 878 "ncoGrammer.g"
	int nbr_dmn=0;
#line 19 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt_peek_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		lmt = _t;
		match(_t,LMT_LIST);
		_t = _t->getNextSibling();
		if ( inputState->guessing==0 ) {
#line 880 "ncoGrammer.g"
			
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

int  ncoTree::statements(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 893 "ncoGrammer.g"
	int iret=0;
#line 57 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST statements_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST blk = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST exp = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST ass = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST stmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lgcl = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST stmt1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST e1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST e2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST e3 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST stmt2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST def = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST del = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST mss = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST ch_mss = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 893 "ncoGrammer.g"
	
	var_sct *var;
	const std::string fnc_nm("statements");
	// list of while/for loops entered n.b depth is lpp_vtr.size()
	// Temporary fix so call run_exe only does a single parse in the
	// nested block
	static std::vector<std::string> lpp_vtr;
	
#line 83 "ncoTree.cpp"
	
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
#line 902 "ncoGrammer.g"
				
				//cout <<"Num of Children in block="<<blk->getNumberOfChildren()<<endl;
				iret=run_exe(blk->getFirstChild(),lpp_vtr.size() );
				
				
#line 101 "ncoTree.cpp"
			}
			break;
		}
		case EXPR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t223 = _t;
			exp = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,EXPR);
			_t = _t->getFirstChild();
			ass = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t223;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 908 "ncoGrammer.g"
				
				RefAST tr;
				RefAST ntr;  
				
				if(ass->getType()==ASSIGN && !prs_arg->ntl_scn ){
				ntr=ass->getFirstChild();
				if(ntr->getType()==TIMES) 
				ntr=ntr->getFirstChild();
				
				if(ntr->getType() == VAR_ID || ntr->getType() ==ATT_ID){
				ntr->addChild( astFactory->create(NORET,"no_ret") );
				// std::cout << "Modified assign "<<exp->toStringTree()<<std::endl;      
				}
				} 
				
				
				var=out(exp->getFirstChild());
				if(var != (var_sct*)NULL)
				var=nco_var_free(var);
				iret=EXPR;
				
#line 139 "ncoTree.cpp"
			}
			break;
		}
		case IF:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t224 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp133_AST_in = _t;
			match(_t,IF);
			_t = _t->getFirstChild();
			var=out(_t);
			_t = _retTree;
			stmt = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t224;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 931 "ncoGrammer.g"
				
				//if can have only 3 or 4 parts  , 1 node and 2 or 3 siblings
				// IF LOGICAL_EXP STATEMENT1 STATEMENT2
				bool br;
					  //Calculate logical expression
					  br=ncap_var_lgcl(var);
					  var=nco_var_free(var);
				
				if(br){ 
				// Execute 2nd sibling  
				if(stmt->getType()==BLOCK)
				iret=run_exe(stmt,lpp_vtr.size());
				else
				iret=statements(stmt);     
				}
				
				// See if else stmt exists (3rd sibling)       
					  if(!br && (stmt=stmt->getNextSibling()) ){
				if(stmt->getType()==BLOCK)
				iret=run_exe(stmt,lpp_vtr.size());
				else
				iret=statements(stmt);     
				
				}
				
				var=NULL_CEWI;
				
				
#line 186 "ncoTree.cpp"
			}
			break;
		}
		case WHILE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t225 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp134_AST_in = _t;
			match(_t,WHILE);
			_t = _t->getFirstChild();
			lgcl = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			stmt1 = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t225;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 960 "ncoGrammer.g"
				
				
				bool br;
				var_sct *var_tf;  
				
				var_tf=out(lgcl);
				br=ncap_var_lgcl(var_tf);
				var_tf=nco_var_free(var_tf);
				
				lpp_vtr.push_back("while");
				
				while(br){ 
				if(stmt1->getType()==BLOCK)
				iret=run_exe(stmt1,lpp_vtr.size());
				else
				iret=statements(stmt1);     
				
				if(iret==BREAK) break;
				var_tf=out(lgcl);
				br=ncap_var_lgcl(var_tf); 
				var_tf=nco_var_free(var_tf);       
				
				}
				lpp_vtr.pop_back(); 
				iret=WHILE;
				var=NULL_CEWI; 
				
				
#line 234 "ncoTree.cpp"
			}
			break;
		}
		case FOR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t226 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp135_AST_in = _t;
			match(_t,FOR);
			_t = _t->getFirstChild();
			e1 = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			e2 = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			e3 = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			stmt2 = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t226;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 989 "ncoGrammer.g"
				
				bool b1,b2,b3,br;
				int iret;
				var_sct *var_f1;
				var_sct *var_f2;
				var_sct *var_f3;
				
				b1=(e1->getType()!=NULL_NODE ? true:false);
				b2=(e2->getType()!=NULL_NODE ? true:false);
				b3=(e3->getType()!=NULL_NODE ? true:false);
				
				lpp_vtr.push_back("for");
				
				if(b1){
				var_f1=out(e1);
				var_f1=nco_var_free(var_f1);
				}          
				
				if(b2){
				var_f2=out(e2);
				br=ncap_var_lgcl(var_f2);
				var_f2=nco_var_free(var_f2);
				} else br=true;
				
				while(br){
				
				if(stmt2->getType()==BLOCK)
				iret=run_exe(stmt2,lpp_vtr.size());
				else
				iret=statements(stmt2);     
				
				if(iret==BREAK) break;
				
				
				if(b3){
				var_f3=out(e3);
				var_f3=nco_var_free(var_f3);
				}
				
				
				if(b2){
				var_f2=out(e2);
				br=ncap_var_lgcl(var_f2);
				var_f2=nco_var_free(var_f2);
				} 
				
				} // end while
				
				lpp_vtr.pop_back();
				iret=FOR;
				var=NULL_CEWI;                
				
				
#line 313 "ncoTree.cpp"
			}
			break;
		}
		case ELSE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp136_AST_in = _t;
			match(_t,ELSE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1044 "ncoGrammer.g"
				iret=ELSE;
#line 325 "ncoTree.cpp"
			}
			break;
		}
		case BREAK:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp137_AST_in = _t;
			match(_t,BREAK);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1045 "ncoGrammer.g"
				iret=BREAK;
#line 337 "ncoTree.cpp"
			}
			break;
		}
		case CONTINUE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp138_AST_in = _t;
			match(_t,CONTINUE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1046 "ncoGrammer.g"
				iret=CONTINUE;
#line 349 "ncoTree.cpp"
			}
			break;
		}
		case NULL_NODE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp139_AST_in = _t;
			match(_t,NULL_NODE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1047 "ncoGrammer.g"
				iret=NULL_NODE;
#line 361 "ncoTree.cpp"
			}
			break;
		}
		case DEFDIM:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t227 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp140_AST_in = _t;
			match(_t,DEFDIM);
			_t = _t->getFirstChild();
			def = _t;
			match(_t,NSTRING);
			_t = _t->getNextSibling();
			var=out(_t);
			_t = _retTree;
			_t = __t227;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1052 "ncoGrammer.g"
				
				
				const char *dmn_nm;
				long sz;
				
				dmn_nm=def->getText().c_str();
				
				var=nco_var_cnf_typ(NC_INT,var);
				iret=DEFDIM;
				
				(void)cast_void_nctype((nc_type)NC_INT,&var->val);
				sz=*var->val.lp;
				var=(var_sct*)nco_var_free(var);
				(void)ncap_def_dim(dmn_nm,sz,prs_arg);
				
#line 395 "ncoTree.cpp"
			}
			break;
		}
		case RAM_WRITE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t228 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp141_AST_in = _t;
			match(_t,RAM_WRITE);
			_t = _t->getFirstChild();
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			_t = __t228;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1069 "ncoGrammer.g"
				
				
				std::string va_nm;
				NcapVar *Nvar;
				
				va_nm=vid->getText();
				
				if(prs_arg->ntl_scn) goto ed0;
				
				Nvar=prs_arg->ptr_var_vtr->find(va_nm);
				
				if(Nvar) {
				if(Nvar->flg_mem==false)
				wrn_prn(fnc_nm,"RAM write function called with:"+va_nm+ " .This variable is already on disk");
				else{
				var_sct *var_nw;
				var_nw=nco_var_dpl(Nvar->var);          
				prs_arg->ptr_var_vtr->erase(va_nm); 
				ncap_var_write(var_nw,false,prs_arg);
				}
				
				}
				
				if(!Nvar)
				wrn_prn(fnc_nm,"RAM write function unable to find variable: "+va_nm); 
				
				ed0: ;
				
				
#line 441 "ncoTree.cpp"
			}
			break;
		}
		case RAM_DELETE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t229 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp142_AST_in = _t;
			match(_t,RAM_DELETE);
			_t = _t->getFirstChild();
			del = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t229;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1099 "ncoGrammer.g"
				
				std::string va_nm;
				NcapVar *Nvar;
				
				va_nm=del->getText();
				
				if(prs_arg->ntl_scn) goto ed1;
				
				Nvar=prs_arg->ptr_var_vtr->find(va_nm);
				
				if(Nvar) {
				// deal with var
				if(del->getType()==VAR_ID){
				if(Nvar->flg_mem==false)
				wrn_prn(fnc_nm,"Delete function cannot remove disk variable:\""+va_nm+ "\". Delete can only remove RAM variables.");
				else
				prs_arg->ptr_var_vtr->erase(va_nm); 
				}
				
				if(del->getType()==ATT_ID) 
				prs_arg->ptr_var_vtr->erase(va_nm); 
				}
				
				if(!Nvar)
				wrn_prn(fnc_nm,"Delete function unable to find "+va_nm); 
				
				ed1: ;
				
				
#line 487 "ncoTree.cpp"
			}
			break;
		}
		case SET_MISS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t230 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp143_AST_in = _t;
			match(_t,SET_MISS);
			_t = _t->getFirstChild();
			mss = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			var=out(_t);
			_t = _retTree;
			_t = __t230;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1129 "ncoGrammer.g"
				
				var_sct *var_in;
				std::string va_nm;
				NcapVar *Nvar;
				
				va_nm=mss->getText();
				
				if(prs_arg->ntl_scn) goto end;
				
				Nvar=prs_arg->ptr_var_vtr->find(va_nm);
				if(!Nvar){
				wrn_prn(fnc_nm,"Set missing value function unable to find :"+va_nm); 
				goto end; 
				}
				
				// De-reference
				var_in=Nvar->var;
				
				var=nco_var_cnf_typ(var_in->type,var);
				
				var->has_mss_val=True;
				var->mss_val=nco_mss_val_mk(var->type);
				
				(void)memcpy(var->mss_val.vp, var->val.vp,nco_typ_lng(var->type));
				
				nco_mss_val_cp(var,var_in);
				
				end: nco_var_free(var); 
				
				
				
#line 537 "ncoTree.cpp"
			}
			break;
		}
		case CH_MISS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t231 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp144_AST_in = _t;
			match(_t,CH_MISS);
			_t = _t->getFirstChild();
			ch_mss = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			var=out(_t);
			_t = _retTree;
			_t = __t231;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1161 "ncoGrammer.g"
				
				
				char *cp_out;
				long slb_sz;
				var_sct *var_in;
				std::string va_nm;
				NcapVar *Nvar;
				
				if(prs_arg->ntl_scn) goto end1;
				
				va_nm=ch_mss->getText();
				
				Nvar=prs_arg->ptr_var_vtr->find(va_nm);
				if(!Nvar){
				wrn_prn(fnc_nm,"Change missing value function unable to find :"+va_nm); 
				goto end1; 
				}
				
				// De-reference
				var_in=Nvar->var;
				
				var=nco_var_cnf_typ(var_in->type,var);
				
				var->has_mss_val=True;
				var->mss_val=nco_mss_val_mk(var->type);
				
				(void)memcpy(var->mss_val.vp, var->val.vp,nco_typ_lng(var->type));
				
				// if no missing add one then exit
				if(!var_in->has_mss_val){
				nco_mss_val_cp(var,var_in);
				
				}else{ 
				
				var_in=ncap_var_init(va_nm,prs_arg,true);
				
				cp_out=(char*)var_in->val.vp;
				slb_sz=nco_typ_lng(var_in->type);
				
				for(long idx=0 ;idx<var_in->sz;idx++){
				if( !memcmp(cp_out,var_in->mss_val.vp,slb_sz))
				(void)memcpy(cp_out,var->mss_val.vp,slb_sz);
				cp_out+=(ptrdiff_t)slb_sz;
				}   
				// Copy new missing value 
				nco_mss_val_cp(var,var_in);
				(void)ncap_var_write(var_in,false,prs_arg);
				}   
				
				end1: nco_var_free(var);         
				
				
#line 608 "ncoTree.cpp"
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
	return iret;
}

var_sct * ncoTree::out(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 1763 "ncoGrammer.g"
	var_sct *var;
#line 634 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST pls_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST min_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST tim_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST div_asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST asn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST qus = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST m = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dval = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST v = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vlst = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST str = ANTLR_USE_NAMESPACE(antlr)nullAST;
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
#line 1763 "ncoGrammer.g"
	
	const std::string fnc_nm("out"); 
		var_sct *var1;
	var_sct *var2;
	var=NULL_CEWI;
	
#line 666 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case DIVIDE:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t288 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp145_AST_in = _t;
			match(_t,DIVIDE);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t288;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1785 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, DIVIDE );
#line 687 "ncoTree.cpp"
			}
			break;
		}
		case MOD:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t289 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp146_AST_in = _t;
			match(_t,MOD);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t289;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1787 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, MOD);
#line 706 "ncoTree.cpp"
			}
			break;
		}
		case CARET:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t290 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp147_AST_in = _t;
			match(_t,CARET);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t290;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1789 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, CARET);
#line 725 "ncoTree.cpp"
			}
			break;
		}
		case LNOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t291 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp148_AST_in = _t;
			match(_t,LNOT);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t291;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1793 "ncoGrammer.g"
				var=ncap_var_var_op(var1,NULL_CEWI, LNOT );
#line 742 "ncoTree.cpp"
			}
			break;
		}
		case INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t294 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp149_AST_in = _t;
			match(_t,INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t294;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1799 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,NULL_CEWI,INC,false,prs_arg);
#line 759 "ncoTree.cpp"
			}
			break;
		}
		case DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t295 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp150_AST_in = _t;
			match(_t,DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t295;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1801 "ncoGrammer.g"
				var=ncap_var_var_inc(var1,NULL_CEWI, DEC,false,prs_arg );
#line 776 "ncoTree.cpp"
			}
			break;
		}
		case POST_INC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t296 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp151_AST_in = _t;
			match(_t,POST_INC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t296;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1803 "ncoGrammer.g"
				
				var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,false,prs_arg);
				
#line 795 "ncoTree.cpp"
			}
			break;
		}
		case POST_DEC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t297 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp152_AST_in = _t;
			match(_t,POST_DEC);
			_t = _t->getFirstChild();
			var1=out_asn(_t);
			_t = _retTree;
			_t = __t297;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1806 "ncoGrammer.g"
				
				var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,false,prs_arg);
				
#line 814 "ncoTree.cpp"
			}
			break;
		}
		case LAND:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t298 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp153_AST_in = _t;
			match(_t,LAND);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t298;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1812 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LAND );
#line 833 "ncoTree.cpp"
			}
			break;
		}
		case LOR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t299 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp154_AST_in = _t;
			match(_t,LOR);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t299;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1814 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LOR );
#line 852 "ncoTree.cpp"
			}
			break;
		}
		case LTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t300 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp155_AST_in = _t;
			match(_t,LTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t300;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1817 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LTHAN );
#line 871 "ncoTree.cpp"
			}
			break;
		}
		case GTHAN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t301 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp156_AST_in = _t;
			match(_t,GTHAN);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t301;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1819 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GTHAN );
#line 890 "ncoTree.cpp"
			}
			break;
		}
		case GEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t302 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp157_AST_in = _t;
			match(_t,GEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t302;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1821 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, GEQ );
#line 909 "ncoTree.cpp"
			}
			break;
		}
		case LEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t303 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp158_AST_in = _t;
			match(_t,LEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t303;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1823 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, LEQ );
#line 928 "ncoTree.cpp"
			}
			break;
		}
		case EQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t304 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp159_AST_in = _t;
			match(_t,EQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t304;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1825 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, EQ );
#line 947 "ncoTree.cpp"
			}
			break;
		}
		case NEQ:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t305 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp160_AST_in = _t;
			match(_t,NEQ);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			var2=out(_t);
			_t = _retTree;
			_t = __t305;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1827 "ncoGrammer.g"
				var=ncap_var_var_op(var1,var2, NEQ );
#line 966 "ncoTree.cpp"
			}
			break;
		}
		case PLUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t306 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp161_AST_in = _t;
			match(_t,PLUS_ASSIGN);
			_t = _t->getFirstChild();
			pls_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t306;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1830 "ncoGrammer.g"
				
				var1=out_asn(pls_asn);
				var=ncap_var_var_inc(var1,var2, PLUS_ASSIGN ,(pls_asn->getType()==TIMES), prs_arg);
				
#line 989 "ncoTree.cpp"
			}
			break;
		}
		case MINUS_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t307 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp162_AST_in = _t;
			match(_t,MINUS_ASSIGN);
			_t = _t->getFirstChild();
			min_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t307;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1834 "ncoGrammer.g"
				
				var1=out_asn(min_asn);
				var=ncap_var_var_inc(var1,var2, MINUS_ASSIGN ,(min_asn->getType()==TIMES), prs_arg);
				
#line 1012 "ncoTree.cpp"
			}
			break;
		}
		case TIMES_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t308 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp163_AST_in = _t;
			match(_t,TIMES_ASSIGN);
			_t = _t->getFirstChild();
			tim_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t308;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1838 "ncoGrammer.g"
				
				var1=out_asn(tim_asn);
				var=ncap_var_var_inc(var1,var2, TIMES_ASSIGN ,(tim_asn->getType()==TIMES), prs_arg);
				
#line 1035 "ncoTree.cpp"
			}
			break;
		}
		case DIVIDE_ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t309 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp164_AST_in = _t;
			match(_t,DIVIDE_ASSIGN);
			_t = _t->getFirstChild();
			div_asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			var2=out(_t);
			_t = _retTree;
			_t = __t309;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1842 "ncoGrammer.g"
					
				var1=out_asn(div_asn);
				var=ncap_var_var_inc(var1,var2, DIVIDE_ASSIGN ,(div_asn->getType()==TIMES), prs_arg);
				
#line 1058 "ncoTree.cpp"
			}
			break;
		}
		case ASSIGN:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t310 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp165_AST_in = _t;
			match(_t,ASSIGN);
			_t = _t->getFirstChild();
			asn = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t310;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1848 "ncoGrammer.g"
				
				// Check for RAM variable - if present 
				// change tree - for example from:
				//     ( EXPR ( = ( * n1 ) ( + four four ) ) )
				// to  ( EXPR ( = n1 ( + four four ) ) )
				RefAST tr;
				bool bram;
				NcapVar *Nvar;
				
				if(asn->getType()==TIMES){
				tr=asn->getFirstChild();
				tr->setNextSibling(asn->getNextSibling());
				bram=true;
				} else { 
				tr=asn; 
				bram=false;
				}
				
				// Die if attempting to create a RAM var 
				// from an existing disk var   
				Nvar= prs_arg->ptr_var_vtr->find(tr->getText());
				
				if(bram && tr->getType()==VAR_ID && Nvar && Nvar->flg_mem==false){
				std::string serr;
				serr= "It is impossible to recast disk variable: \"" + tr->getText() +"\" as a RAM variable.";
				err_prn(fnc_nm,serr );       
				}                
				
				
				
				if(prs_arg->ntl_scn)
				var=assign_ntl(tr,bram); 
				else
				var=assign(tr,bram);
				
				
#line 1111 "ncoTree.cpp"
			}
			break;
		}
		case QUESTION:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t311 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp166_AST_in = _t;
			match(_t,QUESTION);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			qus = _t;
			if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
			_t = _t->getNextSibling();
			_t = __t311;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1886 "ncoGrammer.g"
				
				bool br;
				
				// if initial scan 
				if(prs_arg->ntl_scn){
				var=ncap_var_udf("~question"); 
				} else {
				
				br=ncap_var_lgcl(var1);
				if(br) 
				var=out(qus);
				else
				var=out(qus->getNextSibling());      
				}   
				var1=nco_var_free(var1);
				
#line 1146 "ncoTree.cpp"
			}
			break;
		}
		case FUNC:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t312 = _t;
			m = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,FUNC);
			_t = _t->getFirstChild();
			ANTLR_USE_NAMESPACE(antlr)RefAST __t313 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp167_AST_in = _t;
			match(_t,FUNC_ARG);
			_t = _t->getFirstChild();
			var1=out(_t);
			_t = _retTree;
			_t = __t313;
			_t = _t->getNextSibling();
			_t = __t312;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1906 "ncoGrammer.g"
				
				sym_sct * sym_ptr;
				
				sym_ptr= prs_arg->ptr_sym_vtr->find(m->getText());
				if(sym_ptr ==NULL) { 
				cout << "Function  " << m->getText() << " not found" << endl;
				exit(1);
				} 
				var=ncap_var_fnc(var1,sym_ptr);
				
				
#line 1179 "ncoTree.cpp"
			}
			break;
		}
		case DOT:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t314 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp168_AST_in = _t;
			match(_t,DOT);
			_t = _t->getFirstChild();
			var=methods(_t);
			_t = _retTree;
			_t = __t314;
			_t = _t->getNextSibling();
			break;
		}
		case PROP:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t315 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp169_AST_in = _t;
			match(_t,PROP);
			_t = _t->getFirstChild();
			var=property(_t);
			_t = _retTree;
			_t = __t315;
			_t = _t->getNextSibling();
			break;
		}
		case DIM_ID_SIZE:
		{
			dval = _t;
			match(_t,DIM_ID_SIZE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1925 "ncoGrammer.g"
				
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
				
#line 1234 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1973 "ncoGrammer.g"
				
				
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
				
				
#line 1275 "ncoTree.cpp"
			}
			break;
		}
		case VALUE_LIST:
		{
			vlst = _t;
			match(_t,VALUE_LIST);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2005 "ncoGrammer.g"
				
				var=value_list(vlst);
				
#line 1289 "ncoTree.cpp"
			}
			break;
		}
		case NSTRING:
		{
			str = _t;
			match(_t,NSTRING);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2010 "ncoGrammer.g"
				
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
				
#line 1321 "ncoTree.cpp"
			}
			break;
		}
		case FLOAT:
		{
			val_float = _t;
			match(_t,FLOAT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2034 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~float"),(nc_type)NC_FLOAT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~float"),static_cast<float>(std::strtod(val_float->getText().c_str(),(char **)NULL)));
#line 1333 "ncoTree.cpp"
			}
			break;
		}
		case DOUBLE:
		{
			val_double = _t;
			match(_t,DOUBLE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2036 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~double"),(nc_type)NC_DOUBLE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~double"),strtod(val_double->getText().c_str(),(char **)NULL));
#line 1345 "ncoTree.cpp"
			}
			break;
		}
		case INT:
		{
			val_int = _t;
			match(_t,INT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2038 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int"),(nc_type)NC_INT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int"),static_cast<nco_int>(std::strtol(val_int->getText().c_str(),(char **)NULL,10)));
#line 1357 "ncoTree.cpp"
			}
			break;
		}
		case SHORT:
		{
			val_short = _t;
			match(_t,SHORT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2040 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~short"),(nc_type)NC_SHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~short"),static_cast<nco_short>(std::strtol(val_short->getText().c_str(),(char **)NULL,10)));
#line 1369 "ncoTree.cpp"
			}
			break;
		}
		case BYTE:
		{
			val_byte = _t;
			match(_t,BYTE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2042 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),(nc_type)NC_BYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~byte"),static_cast<nco_byte>(std::strtol(val_byte->getText().c_str(),(char **)NULL,10)));
#line 1381 "ncoTree.cpp"
			}
			break;
		}
		case UBYTE:
		{
			val_ubyte = _t;
			match(_t,UBYTE);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2047 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),(nc_type)NC_UBYTE,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ubyte"),static_cast<nco_ubyte>(std::strtoul(val_ubyte->getText().c_str(),(char **)NULL,10)));
#line 1393 "ncoTree.cpp"
			}
			break;
		}
		case USHORT:
		{
			val_ushort = _t;
			match(_t,USHORT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2051 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),(nc_type)NC_USHORT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~ushort"),static_cast<nco_ushort>(std::strtoul(val_ushort->getText().c_str(),(char **)NULL,10)));
#line 1405 "ncoTree.cpp"
			}
			break;
		}
		case UINT:
		{
			val_uint = _t;
			match(_t,UINT);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2053 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),(nc_type)NC_UINT,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint"),static_cast<nco_uint>(std::strtoul(val_uint->getText().c_str(),(char **)NULL,10)));
#line 1417 "ncoTree.cpp"
			}
			break;
		}
		case INT64:
		{
			val_int64 = _t;
			match(_t,INT64);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2055 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),(nc_type)NC_INT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~int64"),sng2nbr(val_int64->getText(),nco_int64_CEWI));
#line 1429 "ncoTree.cpp"
			}
			break;
		}
		case UINT64:
		{
			val_uint64 = _t;
			match(_t,UINT64);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2059 "ncoGrammer.g"
				if(prs_arg->ntl_scn) var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),(nc_type)NC_UINT64,false); else var=ncap_sclr_var_mk(static_cast<std::string>("~uint64"),sng2nbr(val_uint64->getText(),nco_uint64_CEWI));
#line 1441 "ncoTree.cpp"
			}
			break;
		}
		default:
			bool synPredMatched269 = false;
			if (((_t->getType() == PLUS))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t269 = _t;
				synPredMatched269 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t268 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp170_AST_in = _t;
					match(_t,PLUS);
					_t = _t->getFirstChild();
					out(_t);
					_t = _retTree;
					out(_t);
					_t = _retTree;
					_t = __t268;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched269 = false;
				}
				_t = __t269;
				inputState->guessing--;
			}
			if ( synPredMatched269 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t270 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp171_AST_in = _t;
				match(_t,PLUS);
				_t = _t->getFirstChild();
				var1=out(_t);
				_t = _retTree;
				var2=out(_t);
				_t = _retTree;
				_t = __t270;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1773 "ncoGrammer.g"
					var=ncap_var_var_op(var1,var2, PLUS );
#line 1485 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched273 = false;
				if (((_t->getType() == MINUS))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t273 = _t;
					synPredMatched273 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t272 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp172_AST_in = _t;
						match(_t,MINUS);
						_t = _t->getFirstChild();
						out(_t);
						_t = _retTree;
						out(_t);
						_t = _retTree;
						_t = __t272;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched273 = false;
					}
					_t = __t273;
					inputState->guessing--;
				}
				if ( synPredMatched273 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t274 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp173_AST_in = _t;
					match(_t,MINUS);
					_t = _t->getFirstChild();
					var1=out(_t);
					_t = _retTree;
					var2=out(_t);
					_t = _retTree;
					_t = __t274;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1775 "ncoGrammer.g"
						var=ncap_var_var_op(var1,var2, MINUS );
#line 1528 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched278 = false;
					if (((_t->getType() == TIMES))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t278 = _t;
						synPredMatched278 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t276 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp174_AST_in = _t;
							match(_t,TIMES);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST __t277 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp175_AST_in = _t;
							match(_t,POST_INC);
							_t = _t->getFirstChild();
							out(_t);
							_t = _retTree;
							_t = __t277;
							_t = _t->getNextSibling();
							_t = __t276;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched278 = false;
						}
						_t = __t278;
						inputState->guessing--;
					}
					if ( synPredMatched278 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t279 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp176_AST_in = _t;
						match(_t,TIMES);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST __t280 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp177_AST_in = _t;
						match(_t,POST_INC);
						_t = _t->getFirstChild();
						var1=out_asn(_t);
						_t = _retTree;
						_t = __t280;
						_t = _t->getNextSibling();
						_t = __t279;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1776 "ncoGrammer.g"
							
							var=ncap_var_var_inc(var1,NULL_CEWI,POST_INC,true,prs_arg);      
							
#line 1581 "ncoTree.cpp"
						}
					}
					else {
						bool synPredMatched284 = false;
						if (((_t->getType() == TIMES))) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t284 = _t;
							synPredMatched284 = true;
							inputState->guessing++;
							try {
								{
								ANTLR_USE_NAMESPACE(antlr)RefAST __t282 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp178_AST_in = _t;
								match(_t,TIMES);
								_t = _t->getFirstChild();
								ANTLR_USE_NAMESPACE(antlr)RefAST __t283 = _t;
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp179_AST_in = _t;
								match(_t,POST_DEC);
								_t = _t->getFirstChild();
								out(_t);
								_t = _retTree;
								_t = __t283;
								_t = _t->getNextSibling();
								_t = __t282;
								_t = _t->getNextSibling();
								}
							}
							catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
								synPredMatched284 = false;
							}
							_t = __t284;
							inputState->guessing--;
						}
						if ( synPredMatched284 ) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t285 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp180_AST_in = _t;
							match(_t,TIMES);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST __t286 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp181_AST_in = _t;
							match(_t,POST_DEC);
							_t = _t->getFirstChild();
							var1=out_asn(_t);
							_t = _retTree;
							_t = __t286;
							_t = _t->getNextSibling();
							_t = __t285;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 1779 "ncoGrammer.g"
								
								var=ncap_var_var_inc(var1,NULL_CEWI,POST_DEC,true,prs_arg);      
								
#line 1634 "ncoTree.cpp"
							}
						}
						else if ((_t->getType() == TIMES)) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t287 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp182_AST_in = _t;
							match(_t,TIMES);
							_t = _t->getFirstChild();
							var1=out(_t);
							_t = _retTree;
							var2=out(_t);
							_t = _retTree;
							_t = __t287;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 1783 "ncoGrammer.g"
								var=ncap_var_var_op(var1,var2, TIMES );
#line 1651 "ncoTree.cpp"
							}
						}
						else if ((_t->getType() == MINUS)) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t292 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp183_AST_in = _t;
							match(_t,MINUS);
							_t = _t->getFirstChild();
							var1=out(_t);
							_t = _retTree;
							_t = __t292;
							_t = _t->getNextSibling();
							if ( inputState->guessing==0 ) {
#line 1795 "ncoGrammer.g"
								var=ncap_var_var_op(var1,NULL_CEWI, MINUS );
#line 1666 "ncoTree.cpp"
							}
						}
						else if ((_t->getType() == PLUS)) {
							ANTLR_USE_NAMESPACE(antlr)RefAST __t293 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp184_AST_in = _t;
							match(_t,PLUS);
							_t = _t->getFirstChild();
							var1=out(_t);
							_t = _retTree;
							_t = __t293;
							_t = _t->getNextSibling();
						}
						else {
							bool synPredMatched318 = false;
							if (((_t->getType() == VAR_ID))) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t318 = _t;
								synPredMatched318 = true;
								inputState->guessing++;
								try {
									{
									ANTLR_USE_NAMESPACE(antlr)RefAST __t317 = _t;
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp185_AST_in = _t;
									match(_t,VAR_ID);
									_t = _t->getFirstChild();
									ANTLR_USE_NAMESPACE(antlr)RefAST tmp186_AST_in = _t;
									match(_t,LMT_LIST);
									_t = _t->getNextSibling();
									_t = __t317;
									_t = _t->getNextSibling();
									}
								}
								catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
									synPredMatched318 = false;
								}
								_t = __t318;
								inputState->guessing--;
							}
							if ( synPredMatched318 ) {
								ANTLR_USE_NAMESPACE(antlr)RefAST __t319 = _t;
								vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
								match(_t,VAR_ID);
								_t = _t->getFirstChild();
								ANTLR_USE_NAMESPACE(antlr)RefAST tmp187_AST_in = _t;
								match(_t,LMT_LIST);
								_t = _t->getNextSibling();
								_t = __t319;
								_t = _t->getNextSibling();
								if ( inputState->guessing==0 ) {
#line 1947 "ncoGrammer.g"
									
									var=var_lmt(vid);
									
									
#line 1720 "ncoTree.cpp"
								}
							}
							else if ((_t->getType() == VAR_ID)) {
								v = _t;
								match(_t,VAR_ID);
								_t = _t->getNextSibling();
								if ( inputState->guessing==0 ) {
#line 1954 "ncoGrammer.g"
									
									
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
									
									
									
#line 1746 "ncoTree.cpp"
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

var_sct * ncoTree::assign_ntl(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
	bool bram
) {
#line 1216 "ncoGrammer.g"
	var_sct *var;
#line 1772 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_ntl_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 1216 "ncoGrammer.g"
	
	const std::string fnc_nm("assign_ntl"); 
	var=NULL_CEWI;
	
#line 1787 "ncoTree.cpp"
	
	try {      // for error handling
		bool synPredMatched235 = false;
		if (((_t->getType() == VAR_ID))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t235 = _t;
			synPredMatched235 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t234 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp188_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp189_AST_in = _t;
				match(_t,LMT_LIST);
				_t = _t->getNextSibling();
				_t = __t234;
				_t = _t->getNextSibling();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched235 = false;
			}
			_t = __t235;
			inputState->guessing--;
		}
		if ( synPredMatched235 ) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t236 = _t;
			vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,VAR_ID);
			_t = _t->getFirstChild();
			lmt = _t;
			match(_t,LMT_LIST);
			_t = _t->getNextSibling();
			_t = __t236;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1221 "ncoGrammer.g"
				
				
				
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
				
#line 1853 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched239 = false;
			if (((_t->getType() == VAR_ID))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t239 = _t;
				synPredMatched239 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t238 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp190_AST_in = _t;
					match(_t,VAR_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp191_AST_in = _t;
					match(_t,DMN_LIST);
					_t = _t->getNextSibling();
					_t = __t238;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched239 = false;
				}
				_t = __t239;
				inputState->guessing--;
			}
			if ( synPredMatched239 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t240 = _t;
				vid1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				dmn = _t;
				match(_t,DMN_LIST);
				_t = _t->getNextSibling();
				_t = __t240;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1249 "ncoGrammer.g"
					
					
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
					
					
#line 1968 "ncoTree.cpp"
				}
			}
			else if ((_t->getType() == VAR_ID)) {
				vid2 = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1325 "ncoGrammer.g"
					
					
					
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
					
					
					
#line 2005 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched243 = false;
				if (((_t->getType() == ATT_ID))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t243 = _t;
					synPredMatched243 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t242 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp192_AST_in = _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp193_AST_in = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						_t = __t242;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched243 = false;
					}
					_t = __t243;
					inputState->guessing--;
				}
				if ( synPredMatched243 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t244 = _t;
					att = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
					match(_t,ATT_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp194_AST_in = _t;
					match(_t,LMT_LIST);
					_t = _t->getNextSibling();
					_t = __t244;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1354 "ncoGrammer.g"
						
						;
						
#line 2048 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched247 = false;
					if (((_t->getType() == ATT_ID))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t247 = _t;
						synPredMatched247 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t246 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp195_AST_in = _t;
							match(_t,ATT_ID);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp196_AST_in = _t;
							match(_t,LMT_DMN);
							_t = _t->getNextSibling();
							_t = __t246;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched247 = false;
						}
						_t = __t247;
						inputState->guessing--;
					}
					if ( synPredMatched247 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t248 = _t;
						att1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp197_AST_in = _t;
						match(_t,DMN_LIST);
						_t = _t->getNextSibling();
						_t = __t248;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1357 "ncoGrammer.g"
							
							;
							
#line 2091 "ncoTree.cpp"
						}
					}
					else if ((_t->getType() == ATT_ID)) {
						att2 = _t;
						match(_t,ATT_ID);
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1360 "ncoGrammer.g"
							
							
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
							
							
#line 2119 "ncoTree.cpp"
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
#line 1383 "ncoGrammer.g"
	var_sct *var;
#line 2145 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST assign_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST dmn = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att2 = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 1383 "ncoGrammer.g"
	
	const std::string fnc_nm("assign"); 
	var=NULL_CEWI;
	
#line 2160 "ncoTree.cpp"
	
	try {      // for error handling
		bool synPredMatched252 = false;
		if (((_t->getType() == VAR_ID))) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t252 = _t;
			synPredMatched252 = true;
			inputState->guessing++;
			try {
				{
				ANTLR_USE_NAMESPACE(antlr)RefAST __t251 = _t;
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp198_AST_in = _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp199_AST_in = _t;
				match(_t,LMT_LIST);
				_t = _t->getNextSibling();
				_t = __t251;
				_t = _t->getNextSibling();
				}
			}
			catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
				synPredMatched252 = false;
			}
			_t = __t252;
			inputState->guessing--;
		}
		if ( synPredMatched252 ) {
			ANTLR_USE_NAMESPACE(antlr)RefAST __t253 = _t;
			vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
			match(_t,VAR_ID);
			_t = _t->getFirstChild();
			lmt = _t;
			match(_t,LMT_LIST);
			_t = _t->getNextSibling();
			_t = __t253;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 1389 "ncoGrammer.g"
				
				
				int idx;
				int nbr_dmn;
				int var_id; 
				int slb_sz;
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
				
				// Deal with RAM variables
				if( Nvar==NULL && bram || Nvar && Nvar->flg_mem) {
				
				
				if(Nvar && Nvar->flg_mem && Nvar->flg_stt==1){
				var_sct *var_ini;
				var_ini=ncap_var_init(vid->getText(),prs_arg,true);       
				Nvar->var->val.vp=var_ini->val.vp;
				var_ini->val.vp=(void*)NULL;
				var_ini=nco_var_free(var_ini);
				Nvar->flg_stt=2; 
				}
				
				if(Nvar && Nvar->flg_mem && Nvar->flg_stt==2){ 
				var_lhs=Nvar->var;    
				}   
				
				if(Nvar==NULL && bram){
				var_lhs=ncap_var_init(vid->getText(),prs_arg,true);       
				}
				
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
				
				
				slb_sz=1;        
				// fill out limit structure
				for(idx=0 ; idx < nbr_dmn ;idx++){
				(void)nco_lmt_evl(prs_arg->out_id,lmt_vtr[idx],0L,prs_arg->FORTRAN_IDX_CNV);
				// Calculate size
				slb_sz *= lmt_vtr[idx]->cnt;
				}
				// Calculate RHS variable                  
				var_rhs=out(vid->getNextSibling());         
				// Convert to LHS type
				var_rhs=nco_var_cnf_typ(var_lhs->type,var_rhs);             
				
				// deal with scalar on RHS first         
				if(var_rhs->sz == 1)
				(void)ncap_att_stretch(var_rhs,slb_sz);
				
				
				// make sure var_lhs and var_rhs are the same size
				// and that they are the same shape (ie they conform!!)          
				if(var_rhs->sz != slb_sz){
				err_prn(fnc_nm, "Hyperslab for "+vid->getText()+" - number of elements on LHS(" +nbr2sng(slb_sz) +  ") doesn't equal number of elements on RHS(" +nbr2sng(var_rhs->sz) +  ")");                                       
				}
				
				(void)nco_put_var_mem(var_rhs,var_lhs,lmt_vtr);
				if(Nvar==NULL)
				(void)ncap_var_write(var_lhs,true,prs_arg); 
				
				
				
				} else {                 
				// deal with Regular Vars
				
				// if var undefined in O or defined but not populated
				if(Nvar==NULL || ( Nvar && Nvar->flg_stt==1)){              
				// if var isn't in ouptut then copy it there
				//rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
				var_lhs=ncap_var_init(vid->getText(),prs_arg,true);
				
				// copy atts to output
				(void)ncap_att_cpy(vid->getText(),vid->getText(),prs_arg);
				(void)ncap_var_write(var_lhs,false,prs_arg);
				}
				
				// Get "new" var_id   
				(void)nco_inq_varid(prs_arg->out_id,var_nm,&var_id);
				
				var_lhs=ncap_var_init(vid->getText(),prs_arg,false);
				
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
				
				//put block              
				{ 
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
				
				} // end else
				
				
				var_rhs=nco_var_free(var_rhs);
				
				// See If we have to return something
				if(lmt->getNextSibling() && lmt->getNextSibling()->getType()==NORET)
				var=NULL_CEWI;
				else 
				var=ncap_var_init(vid->getText(),prs_arg,true);
				
				
				// Empty and free vector 
				for(idx=0 ; idx < nbr_dmn ; idx++)
				(void)nco_lmt_free(lmt_vtr[idx]);
				
				
#line 2399 "ncoTree.cpp"
			}
		}
		else {
			bool synPredMatched256 = false;
			if (((_t->getType() == VAR_ID))) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t256 = _t;
				synPredMatched256 = true;
				inputState->guessing++;
				try {
					{
					ANTLR_USE_NAMESPACE(antlr)RefAST __t255 = _t;
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp200_AST_in = _t;
					match(_t,VAR_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp201_AST_in = _t;
					match(_t,DMN_LIST);
					_t = _t->getNextSibling();
					_t = __t255;
					_t = _t->getNextSibling();
					}
				}
				catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
					synPredMatched256 = false;
				}
				_t = __t256;
				inputState->guessing--;
			}
			if ( synPredMatched256 ) {
				ANTLR_USE_NAMESPACE(antlr)RefAST __t257 = _t;
				vid1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
				match(_t,VAR_ID);
				_t = _t->getFirstChild();
				dmn = _t;
				match(_t,DMN_LIST);
				_t = _t->getNextSibling();
				_t = __t257;
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1592 "ncoGrammer.g"
					
					
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
					
					(void)nco_free(var_cst->val.vp);
					var_cst->val.vp=(void*)NULL;
					
					if(var_cst->sz >1 && var1->sz==1){
					var_sct *var_nw;
					void *vp_swp;
					
					var_nw=nco_var_dpl(var_cst);
					var_nw=nco_var_cnf_typ(var1->type,var_nw);
					(void)ncap_att_stretch(var1,var_nw->sz);
					
					vp_swp=var_nw->val.vp;
					var_nw->val.vp=var1->val.vp;
					var1->val.vp=vp_swp;
					
					var1=nco_var_free(var1);
					var1=var_nw;
					
					
					}else if(var_cst->sz==var1->sz &&  ( ncap_var_is_att(var1) ||var1->has_dpl_dmn==-1 )){
					var_sct *var_nw;
					void *vp_swp;
					
					var_nw=nco_var_dpl(var_cst);
					var_nw=nco_var_cnf_typ(var1->type,var_nw);
					
					vp_swp=var_nw->val.vp;
					var_nw->val.vp=var1->val.vp;
					var1->val.vp=vp_swp;
					
					var1=nco_var_free(var1);
					var1=var_nw;
					
					}
					
					//blow out if vars not the same size      
					if(var1->sz != var_cst->sz) 
					err_prn(fnc_nm, "LHS cast for "+vid1->getText()+" - cannot make RHS "+ std::string(var1->nm) + " conform.");               
					
					var1->nm=(char*)nco_free(var1->nm);
					
					var1->nm =strdup(vid1->getText().c_str());
					
					
					
					// See If we have to return something
					if(dmn->getNextSibling() && dmn->getNextSibling()->getType()==NORET)
					var=NULL_CEWI;
					else 
					var=nco_var_dpl(var1);               ;
					
					
					
					//call to nco_var_get() in ncap_var_init() uses this property
					var1->typ_dsk=var1->type;
					(void)ncap_var_write(var1,bram,prs_arg);
					
					bcst=false;
					var_cst=nco_var_free(var_cst); 
					
					
					
#line 2530 "ncoTree.cpp"
				}
			}
			else if ((_t->getType() == VAR_ID)) {
				vid2 = _t;
				match(_t,VAR_ID);
				_t = _t->getNextSibling();
				if ( inputState->guessing==0 ) {
#line 1684 "ncoGrammer.g"
					
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
					
					
					// See If we have to return something
					if(vid2->getFirstChild() && vid2->getFirstChild()->getType()==NORET)
					var=NULL_CEWI;
					else 
					var=nco_var_dpl(var1);               ;
					
					
					// Write var to disk
					(void)ncap_var_write(var1,bram,prs_arg);
					
					
					
#line 2584 "ncoTree.cpp"
				}
			}
			else {
				bool synPredMatched260 = false;
				if (((_t->getType() == ATT_ID))) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t260 = _t;
					synPredMatched260 = true;
					inputState->guessing++;
					try {
						{
						ANTLR_USE_NAMESPACE(antlr)RefAST __t259 = _t;
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp202_AST_in = _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp203_AST_in = _t;
						match(_t,LMT_LIST);
						_t = _t->getNextSibling();
						_t = __t259;
						_t = _t->getNextSibling();
						}
					}
					catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
						synPredMatched260 = false;
					}
					_t = __t260;
					inputState->guessing--;
				}
				if ( synPredMatched260 ) {
					ANTLR_USE_NAMESPACE(antlr)RefAST __t261 = _t;
					att = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
					match(_t,ATT_ID);
					_t = _t->getFirstChild();
					ANTLR_USE_NAMESPACE(antlr)RefAST tmp204_AST_in = _t;
					match(_t,LMT_LIST);
					_t = _t->getNextSibling();
					_t = __t261;
					_t = _t->getNextSibling();
					if ( inputState->guessing==0 ) {
#line 1730 "ncoGrammer.g"
						
						;
						
#line 2627 "ncoTree.cpp"
					}
				}
				else {
					bool synPredMatched264 = false;
					if (((_t->getType() == ATT_ID))) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t264 = _t;
						synPredMatched264 = true;
						inputState->guessing++;
						try {
							{
							ANTLR_USE_NAMESPACE(antlr)RefAST __t263 = _t;
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp205_AST_in = _t;
							match(_t,ATT_ID);
							_t = _t->getFirstChild();
							ANTLR_USE_NAMESPACE(antlr)RefAST tmp206_AST_in = _t;
							match(_t,LMT_DMN);
							_t = _t->getNextSibling();
							_t = __t263;
							_t = _t->getNextSibling();
							}
						}
						catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& pe) {
							synPredMatched264 = false;
						}
						_t = __t264;
						inputState->guessing--;
					}
					if ( synPredMatched264 ) {
						ANTLR_USE_NAMESPACE(antlr)RefAST __t265 = _t;
						att1 = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
						match(_t,ATT_ID);
						_t = _t->getFirstChild();
						ANTLR_USE_NAMESPACE(antlr)RefAST tmp207_AST_in = _t;
						match(_t,DMN_LIST);
						_t = _t->getNextSibling();
						_t = __t265;
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1733 "ncoGrammer.g"
							
							;
							
#line 2670 "ncoTree.cpp"
						}
					}
					else if ((_t->getType() == ATT_ID)) {
						att2 = _t;
						match(_t,ATT_ID);
						_t = _t->getNextSibling();
						if ( inputState->guessing==0 ) {
#line 1736 "ncoGrammer.g"
							
							
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
							
							
							// See If we have to return something
							if(att2->getFirstChild() && att2->getFirstChild()->getType()==NORET)
							var=NULL_CEWI;
							else 
							var=nco_var_dpl(var1);               ;
							
							
#line 2703 "ncoTree.cpp"
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
#line 2070 "ncoGrammer.g"
	var_sct *var;
#line 2727 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST out_asn_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid1 = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST att = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 2070 "ncoGrammer.g"
	
	const std::string fnc_nm("assign_asn");
	var=NULL_CEWI; 
	
#line 2737 "ncoTree.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case TIMES:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t321 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp208_AST_in = _t;
			match(_t,TIMES);
			_t = _t->getFirstChild();
			vid1 = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			_t = __t321;
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2077 "ncoGrammer.g"
				
				if(vid1->getFirstChild())
				err_prn(fnc_nm,"Invalid Lvalue " +vid1->getText() );
				
				var=ncap_var_init(vid1->getText(),prs_arg,true);
				if(var== NULL_CEWI){
				nco_exit(EXIT_FAILURE);
				}
				
				
#line 2766 "ncoTree.cpp"
			}
			break;
		}
		case VAR_ID:
		{
			vid = _t;
			match(_t,VAR_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2088 "ncoGrammer.g"
				
				if(vid->getFirstChild())
				err_prn(fnc_nm,"Invalid Lvalue " +vid->getText() );
				
				var=ncap_var_init(vid->getText(),prs_arg,true);
				if(var== NULL_CEWI){
				nco_exit(EXIT_FAILURE);
				}
				
				
				
				
#line 2789 "ncoTree.cpp"
			}
			break;
		}
		case ATT_ID:
		{
			att = _t;
			match(_t,ATT_ID);
			_t = _t->getNextSibling();
			if ( inputState->guessing==0 ) {
#line 2101 "ncoGrammer.g"
				
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
				
				
				
#line 2835 "ncoTree.cpp"
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

var_sct * ncoTree::methods(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 2420 "ncoGrammer.g"
	var_sct *var;
#line 2861 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST methods_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST mtd = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST args = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 2420 "ncoGrammer.g"
	
	const std::string fnc_nm("methods");
	var=NULL_CEWI; 
	var_sct *var1;
	
#line 2871 "ncoTree.cpp"
	
	try {      // for error handling
		{
		var1=out(_t);
		_t = _retTree;
		mtd = _t;
		if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
		_t = _t->getNextSibling();
		args = _t;
		match(_t,FUNC_ARG);
		_t = _t->getNextSibling();
		}
		if ( inputState->guessing==0 ) {
#line 2428 "ncoGrammer.g"
			
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
			
			
#line 3043 "ncoTree.cpp"
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

var_sct * ncoTree::property(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 2387 "ncoGrammer.g"
	var_sct *var;
#line 3062 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST property_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST prp = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 2387 "ncoGrammer.g"
	
	const std::string fnc_nm("property");
	var=NULL_CEWI; 
	var_sct *var1;
	
#line 3071 "ncoTree.cpp"
	
	try {      // for error handling
		{
		var1=out(_t);
		_t = _retTree;
		prp = _t;
		if ( _t == ANTLR_USE_NAMESPACE(antlr)nullAST ) throw ANTLR_USE_NAMESPACE(antlr)MismatchedTokenException();
		_t = _t->getNextSibling();
		}
		if ( inputState->guessing==0 ) {
#line 2395 "ncoGrammer.g"
			
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
			
#line 3103 "ncoTree.cpp"
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

var_sct * ncoTree::value_list(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 2139 "ncoGrammer.g"
	var_sct *var;
#line 3122 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST value_list_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vlst = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 2139 "ncoGrammer.g"
	
	const std::string fnc_nm("var_lmt");
	var=NULL_CEWI; 
	
#line 3130 "ncoTree.cpp"
	
	try {      // for error handling
		{
		vlst = _t;
		match(_t,VALUE_LIST);
		_t = _t->getNextSibling();
		}
		if ( inputState->guessing==0 ) {
#line 2144 "ncoGrammer.g"
			
			
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
			
			
#line 3233 "ncoTree.cpp"
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

var_sct * ncoTree::var_lmt(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 2242 "ncoGrammer.g"
	var_sct *var;
#line 3252 "ncoTree.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST var_lmt_AST_in = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST vid = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST lmt = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 2242 "ncoGrammer.g"
	
	const std::string fnc_nm("var_lmt");
	var=NULL_CEWI; 
	
#line 3261 "ncoTree.cpp"
	
	try {      // for error handling
		ANTLR_USE_NAMESPACE(antlr)RefAST __t325 = _t;
		vid = (_t == ANTLR_USE_NAMESPACE(antlr)RefAST(ASTNULL)) ? ANTLR_USE_NAMESPACE(antlr)nullAST : _t;
		match(_t,VAR_ID);
		_t = _t->getFirstChild();
		lmt = _t;
		match(_t,LMT_LIST);
		_t = _t->getNextSibling();
		_t = __t325;
		_t = _t->getNextSibling();
		if ( inputState->guessing==0 ) {
#line 2247 "ncoGrammer.g"
			
			bool bram;   // Check for a RAM variable
			int idx;
			int nbr_dmn;
			const char *var_nm;
			var_sct *var_rhs;
			var_sct *var_nw;
			var_sct *var1;
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
			(void)nco_get_var_mem(var_rhs,dmn_vtr);
			var_nw=nco_var_dpl(var_rhs);
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
			
			
#line 3413 "ncoTree.cpp"
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



