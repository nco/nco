/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.c,v 1.1 2011-07-23 18:19:10 zender Exp $ */

/* Purpose: Group utilities */

/* Copyright (C) 2011--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_grp_utl.h" /* Group utilities */

int /* [rcd] Return code */
nco_grp_dfn /* [fnc] Define groups in output file */
(const int in_id, /* I [enm] netCDF input-file ID */
 const int out_id, /* I [enm] netCDF output-file ID */
 nm_id_sct *grp_xtr_lst, /* [grp] Number of groups to be defined */
 const int grp_nbr) /* I [nbr] Number of groups to be defined */
{
  /* Purpose: Define groups in output file */
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* For each (possibly user-specified) top-level group ... */
  for(idx=0;idx<grp_nbr;idx++){
    int grp_out_id;
    /* Define group and all subgroups */
    rcd+=nco_def_grp_rcr(in_id,out_id,prn_id)
  } /* end loop over top-level groups */

  return rcd;
} /* end nco_grp_dfn() */

int /* [rcd] Return code */
nco_def_grp_rcr
(const int in_id, /* I [enm] netCDF input-file ID */
 const int out_id, /* I [enm] netCDF output-file ID */
 const int prn_id, /* [enm] Parent group ID */
 int * const grp_nbr) /* I [nbr] Number of sub-groups that were defined */
{
  /* Purpose: Recursively define parent group and all subgroups in output file */
  int idx;
  int grp_nbr; /* I [nbr] Number of sub-groups */
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* How many sub-groups are in this group? */
  rcd+=nco_inq_grps(prn_id,grp_nm,&grp_out_id);

  for(idx=0;idx<grp_nbr;idx++){
    int grp_out_id;
    /* Define group and all subgroups */
    rcd+=nco_def_grp_rcr(grp_xtr_lst[idx],&grp_out_id);
  } /* end loop over top-level groups */
  /* Define group and all subgroups */
  rcd+=nco_def_grp(prn_id,grp_nm,&grp_out_id);

  return rcd;
} /* end nco_grp_dfn() */

