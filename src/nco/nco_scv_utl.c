scv_sct 
ptr_unn_2_scv(nc_type type,ptr_unn val)
{
  /* Purpose: Convert a ptr_unn to an attribute scv_sct
     Assumes that val is initially cast to void
     Note does not convert cp (strings) as these are not handled by scv_sct
     Note: netCDF attributes may contain multiple values
     Only FIRST value in memory block is converted */
  
  scv_sct scv;
  (void)cast_void_nctype(type,&val);
  switch(type){
  case NC_FLOAT: scv.val.f=*val.fp; break;
  case NC_DOUBLE: scv.val.d =*val.dp; break;
  case NC_INT: scv.val.l =*val.lp; break;
  case NC_SHORT: scv.val.s=*val.sp; break;
  case NC_BYTE: scv.val.b =*val.bp; break;
  case NC_CHAR: break; /* Do nothing */
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  scv.type=type;
  /* Do not uncast pointer as we are working with a copy */
  return scv;
} /* end ptr_unn_2_scv */

