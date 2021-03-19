/* $Header$ */

/* Purpose: NCO wrappers for netCDF C library */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Utility routines not defined by netCDF library, but useful in working with it */
char * /* O [sng] netCDF-compatible name */
nm2sng_nc /* [fnc] Turn group/variable/dimension/attribute name into legal netCDF */
(const char * const nm_sng) /* I [sng] Name to netCDF-ize */
{
  /* Purpose: Turn variable/dimension/attribute name into legal netCDF
     Currently this means replacing forward slash by underscore and parentheses by underscore
     NB: Calling function must free() memory containing netCDF-ized string
     Weird file menagerie shows that:
     NASA HDF4 CERES files (CER*) have forward slashes and spaces in attribute, dimension, and variable names: "Clear/layer/overlap percent coverages"
     NASA HDF4 CERES files (CER*) have dimension names that begin with special characters: "(FOV) Footprints" */

  char chr_sf='_'; /* [chr] Safe character */
  char chr_1st; /* [chr] First character of name */
  char *nm_nc; /* [sng] netCDF-compatible name */
  char *nm_cpy; /* [sng] Moving pointer to netCDF-compatible name */

  if(nm_sng == NULL) return NULL;

  nm_cpy=nm_nc=(char *)strdup(nm_sng);

  /* netCDF disallows forward slashes in element name. Replace slash by underscore. */
  while(*nm_cpy){
    if(*nm_cpy == '/') *nm_cpy=chr_sf;
    nm_cpy++;
  } /* end while */

  /* netCDF requires first character of element name to be alphanumeric
     Change first character that is special by underscore */
  nm_cpy=nm_nc;
  chr_1st=nm_cpy[0];
  if(!isalnum(chr_1st)) nm_cpy[0]=chr_sf;
  if(chr_1st == '('){
    /* Change all parentheses to underscores */
    while(*nm_cpy){
      if(*nm_cpy == '(') *nm_cpy=chr_sf;
      if(*nm_cpy == ')') *nm_cpy=chr_sf;
      nm_cpy++;
    } /* end while */
  } /* endif parentheses*/

  return nm_nc;
} /* end nm2sng_nc() */

void
nco_err_exit /* [fnc] Print netCDF error message, routine name, then exit */
(const int rcd, /* I [enm] netCDF error code */
 const char * const msg) /* I [sng] Supplemental error message */
{
  /* Purpose: Print netCDF error message, routine name, then exit(EXIT_FAILURE) or abort()
     Routine is called by all wrappers when fatal error is encountered
     Goal is to have all NCO error exits to go through nco_err_exit()
     This provides uniform handling of exit() status and abort() option
     Successful NCO exits should call nco_exit() not nco_err_exit()

     msg variable allows wrapper to pass more descriptive information than
     is contained in the netCDF-defined error message.
     Use msg to print, e.g., name of variable or routine which caused error */

  const char fnc_nm[]="nco_err_exit()";
#ifdef NCO_ABORT_ON_ERROR
  const char exit_fnc_nm[]="abort()";
#else /* !NCO_ABORT_ON_ERROR */
  const char exit_fnc_nm[]="exit(EXIT_FAILURE)";
#endif /* !NCO_ABORT_ON_ERROR */

  switch(rcd){ /* Alphabetical by error code. Codes that depend on HDF5 are #ifdef'd unless their stubs are defined in nco_netcdf.h, which defines stubs for newer (post netCDF 3.6.1) error codes only. */
  case NC_EACCESS: /* (-77) netcdf.h added NC_EACCESS for DAP support in ~2012 */
    (void)fprintf(stdout,"ERROR NC_EACCESS Access failure\nHINT: NC_EACCESS errors signify a problem receiving data from a DAP server. This can occur in multiple ways, including 1) When NCO requests (with nco_var_get()) more data than the server is configured to dispense at one time. A workaround for this might be to request data in smaller amounts at one time. This can be accomplished by accessing hyperslabs or multi-slabs of data as described at http://nco.sf.net/nco.html#mlt. 2) When NCO requests data from a buggy DAP library. The DAP software built into netCDF versions 4.6.2 and 4.6.3 is known to be problematic. The only solution for this would be to use an NCO built with a newer version (at least 4.7.3) of netCDF.\n"); break;
  case NC_EBADTYPE: /* (-45) */
    (void)fprintf(stdout,"ERROR NC_BADTYPE Not a netCDF data type\nHINT: NC_EBADTYPE errors can occur when NCO tries to write netCDF4 features to a netCDF3 file, and, separately, when a variable's type conflicts with its _FillValue type. 1. netCDF4 features that cannot be defined in a netCDF3 file, and that thus will trigger this error, include groups and netCDF4 atomic types (e.g., NC_STRING, NC_UBYTE, NC_INT64). To avoid this, remove all netCDF4 features before attempting to convert them to netCDF3 format, or write a netCDF4 output file instead of a netCDF3 output file. 2. The netCDF best practice is for _FillValue attributes to be the same numeric type as their host variables.\n"); break;
  case NC_ECANTWRITE: /* (-103) */
    (void)fprintf(stdout,"ERROR NC_ECANTWRITE Can't write file\nHINT: NC_ECANTWRITE errors can occur when NCO tries to write to an HDF5 file that is not netCDF4-compliant. One workaround is translate the file to a netCDF4-compliant file first, e.g., with \'ncks in.h5 out.nc\'.\n"); break;
  case NC_EDAP: /* (-66) */
  case NC_EDAPSVC: /* (-70) */
  case NC_EDDS: /* (-72), also NC_EDMR */
  case NC_EDATADDS: /* (-73), also NC_EDATADAP */
  case NC_EDAPURL: /* (-74), also NC_EURL */
  case NC_EDAPCONSTRAINT: /* (-75), also NC_CONSTRAINT */
    (void)fprintf(stdout,"ERROR NC_EDAP Generic DAP error\nHINT: NC_EDAP errors can occur when NCO is invoked with a request that a remote DAP server cannot meet, for example, due to the size of the required data transfer. One workaround might be to re-formulate the NCO command into more commands that each request smaller subsets/hyperslabs from the remote DAP server. However, there are a number of DAP-related errors, only one of which can be reported at a time. The others include: NC_EDAPSVC (-70, DAP server error), NC_EDDS and NC_EDMR (-72, Malformed or inaccessible DDS and Dap4 alias, respectively), NC_EDATADDS and NC_EDATADAP (-73, Malformed or inaccessible DATADDS and Dap4 alias, respectively), NC_EDAPURL and NC_EURL (-74, Malformed DAP URL and Malformed URL, respectively), NC_EDAPCONSTRAINT and NC_ECONSTRAINT (-75, Malformed DAP Constraint and Malformed Constraint, respectively). Few, if any, of these errors are expected to be caused by NCO, since NCO just relays user commands to the DAP server. It is more likely that the DAP syntax employed is incorrect, or there is an error with the DAP server, or the request exceeds the server constraints.\n"); break;
  case NC_EINVAL: /* (-36) */
    (void)fprintf(stdout,"ERROR NC_EINVAL Invalid argument\nHINT: NC_EINVAL errors can occur for many reasons. Three common ones are described here. 1. When invalid chunking directives are given, e.g., to unchunk a record variable or to chunk a variable with an invalid size (such as zero or larger than a fixed dimension). This is the most frequent/likely cause in our experience. 2. When HDF4-enabled NCO attempts to directly write to an HDF4 (not HDF5) file. Not all HDF5 calls are available with HDF4, and the operators ncrename and ncatted are liklier to trigger this problem. We are working to solve this (fxm TODO nco1104). Please let us know if it affects you. For now the workaround is to convert the HDF4 file to netCDF4 first (e.g., with ncks -4 in.hdf out.nc), then use ncrename or ncatted. 3. When NCO operators attempt to open netCDF4 files using the diskless option, usually invoked with --diskless_all, --ram_all, or --open_ram. Is your input file netCDF4 format?  (http://nco.sf.net/nco.html#fmt_inq shows how to tell.) If so then omitting the diskless option may solve this problem.\n"); break; 
  case NC_EIO: /* (-68) netcdf.h added NC_EIO in ~2012 */
    (void)fprintf(stdout,"ERROR NC_EIO Generic IO error\nHINT: NC_EIO errors can occur when NCO tries to read a file through a non-existant DAP location. Then NCO automatically tries to retrieve the file through another method, e.g., searching for a file of the same name in the current directory. This can trigger a subsequent error. When debugging the problem, first address the originating error (from NC_EIO).\n"); break;
#ifdef ENABLE_NETCDF4 
  case NC_EHDFERR: /* (-101) */
    (void)fprintf(stdout,"ERROR NC_EHDFERR Error at HDF5 layer\nHINT: NC_EHDFERR errors indicate that the HDF5-backend to netCDF is unable to perform the requested task. NCO can receive this devilishly inscrutable error for a variety of possible reasons including: 1) The run-time dynamic linker attempts to resolve calls from the netCDF library to the HDF library with an HDF5 libhdf5.a that is incompatible with the version used to build NCO and netCDF. 2) The file system does not allow the HDF5 flock() function, as of HDF5 1.10.x, to enable multiple processes to open the same file for reading, a feature known as SWMR (Single Write Multiple Read). The fix is to disable the HDF5 flock() by setting an environment variable thusly: \"export HDF5_USE_FILE_LOCKING=FALSE\". 3) An incorrect netCDF4 library implementation of a procedure (e.g., nc_rename_var()) in terms of HDF function calls (e.g., HDF5Lmove()) manifests an error or inconsistent state within the HDF5 layer. This often occurs during renaming operations (https://github.com/Unidata/netcdf-c/issues/597). 4) Attempting to compress or decompress a netCDF4 dataset with a non-standard (i.e., non-DEFLATE) filter when the requisite shared library to encode/decode that compression filter is not present in either the default location (/usr/local/hdf5/lib/plugin) or in the user-configurable location referred to by the HDF5_PLUGIN_PATH environment variable. One can determine if missing plugin libraries are the culprit by dumping the hidden attributes of the dataset with, e.g., ncks --hdn -m in.nc or ncdump -s -h in.nc. Any variables with (hidden) \"_Filter\" attributes require the corresponding shared libraries to be located in HDF5_PLUGIN_PATH. Some HDF5 implementations (at least MacOSX with MacPorts as of 20200907) may also require explicitly setting the plugin path in the environment, even for the default location! To test this, re-try your NCO command after doing this: \"export HDF5_PLUGIN_PATH=/usr/local/hdf5/lib/plugin\". 5) Bad vibes.\n"); break;
#endif /* !ENABLE_NETCDF4 */
  case NC_ELATEFILL: /* (-122) netcdf.h replaced NC_EFILLVALUE by NC_ELATEFILL after about netCDF ~4.2.1 */ 
    (void)fprintf(stdout,"ERROR NC_ELATEFILL (formerly NC_EFILLVALUE) Attempt to define fill value when data already exists\nHINT: NC_ELATEFILL errors can occur when NCO attempts to create, modify, or overwrite a _FillValue attribute for an existing variable in a netCDF4 file. The netCDF4 format (unlike netCDF3) does not permit this. Does your output file need to be netCDF4 or netCDF4_classic format? One workaround is to change the output format to netCDF3 (e.g., ncks -3 in.nc out.nc), edit _FillValue attributes to your heart's content, and then convert back to netCDF4 (e.g., ncks -4 in.nc out.nc). Unfortunately, the netCDF library behavior in this regard changed (near version 4.4.0 with patch NCF-187) and it has proven difficult to workaround the netCDF4 limitation in all cases with all netCDF4 library versions.\n"); break; 
#ifdef ENABLE_NETCDF4 
  case NC_ENOTBUILT: /* (-128) */
    (void)fprintf(stdout,"ERROR NC_ENOTBUILT Attempt to use feature that was not turned on when netCDF was built\nHINT: NC_ENOTBUILT errors usually occur when attempting to access HDF4 or CDF5 files: 1. It is only possible to access HDF4 (including HDF-EOS2) files if NCO is first re-linked to a version of netCDF configured with the --enable-hdf4 option, which itself must be linked to a version of HDF4 configured with the --disable-netcdf option. These are non-standard build options! Full instructions are here: http://www.unidata.ucar.edu/software/netcdf/docs/build_hdf4.html\nFollow those instructions to rebuild HDF4 and netCDF, then rebuild NCO on top of that netCDF, and then your NCO command will likely work. 2. It is only possible to access CDF5 (including PnetCDF) files if NCO is first re-linked to netCDF version 4.4.0+. Moreover, netCDF has changed default CDF5 support through time. netCDF versions 4.4.0-4.4.1 automatically support CDF5. netCDF 4.5.x-4.6.1 must be configured with the --enable-cdf5 option. netCDF 4.6.2+ is expected to enable CDF5 by default once again.\n"); break;
#endif /* !ENABLE_NETCDF4 */
  case NC_ENOTNC: /* (-51) */
    (void)fprintf(stdout,"ERROR NC_ENOTNC Not a netCDF file\nHINT: NC_ENOTNC errors can occur for many reasons. If your use-case matches one of the four listed below, take the corrective action indicated:\n1. An NCO operator linked only to the classic netCDF3 library attempts to read netCDF4 (or HDF5) files. "); 
#ifdef ENABLE_NETCDF4 
     (void)fprintf(stdout,"However, this executable seems to have been built with the capability to manipulate netCDF4 files, so it is unlikely that this command failed only because the input datasets are netCDF4 format. Something else is going wrong. \n"); 
#else /* !ENABLE_NETCDF4 */ 
     (void)fprintf(stdout,"Are your input files netCDF4 format?  (http://nco.sf.net/nco.html#fmt_inq shows how to tell.) If so then installing or re-building a netCDF4-compatible version of NCO should solve this problem. First upgrade netCDF to version 4.x, then install NCO using those netCDF 4.x libraries.\n2. NC_ENOTNC can occur when users attempt to utilize diskless (i.e., RAM) files.  In this case remove the diskless switches (e.g., --ram or --diskless) and then re-issue the command. \n"); 
#endif /* !ENABLE_NETCDF4 */ 
     (void)fprintf(stdout,"2. NCO attempts to read other filetypes (HDF4, HDF-EOS2, PnetCDF/CDF5) for which support must be (but was not) enabled at netCDF build-time. NCO can access HDF4 files if NCO is first re-linked to a version of netCDF configured with the --enable-hdf4 option. This is a non-standard netCDF build option described here: http://www.unidata.ucar.edu/software/netcdf/docs/build_hdf4.html. NCO can access PnetCDF/CDF5 files if NCO is first re-linked to netCDF version 4.4.0 or later.\n3. NCO attempts to utilize diskless (i.e., RAM) files.  In this case remove the diskless switches (e.g., --ram or --diskless) and then re-issue the command.\n4. Access to a DAP URL fails, and the backup method of downloading the URL using wget obtains a data aggregation file (e.g., a .ncml file) instead of an actual netCDF file. In this case the problem is with the DAP server or URL.\n"); break; 
  case NC_ERANGE: /* (-60) */
    (void)fprintf(stdout,"ERROR NC_ERANGE Result not representable in output file\nHINT: NC_ERANGE errors typically occur after an arithmetic operation results in a value not representible by the output variable type when NCO attempts to write those values to an output file.  Possible workaround for cases when the correct values fall outside the range of the storage type: Promote the variable to higher precision before attempting arithmetic.  For example,\nncap2 -O -s \'foo=double(foo);\' in.nc in.nc\nFor more details, see http://nco.sf.net/nco.html#typ_cnv\n"); break; 
  case NC_EUNLIMIT: /* (-54) */
    (void)fprintf(stdout,"ERROR NC_UNLIMIT NC_UNLIMITED size already in use\nHINT: NC_EUNLIMIT errors can occur when attempting to convert netCDF4 classic files that contain multiple record dimensions into a netCDF3 file that allows only one record dimension. In this case, try first fixing the excess record dimension(s) (with, e.g., ncks --fix_rec_dmn) and then convert to netCDF3. For more details, see http://nco.sf.net/nco.html#fix_rec_dmn\n"); break;
  case NC_EVARSIZE: /* (-62) */
    (void)fprintf(stdout,"ERROR NC_EVARSIZE One or more variable sizes violate format constraints\nHINT: NC_EVARSIZE errors occur when attempting to copy or aggregate input files together into an output file that exceeds the per-file capacity of the output file format, and when trying to copy, aggregate, or define individual variables that exceed the per-variable constraints of the output file format. The per-file limit of all netCDF formats is not less than 8 EiB on modern computers, so any NC_EVARSIZE error is almost certainly due to violating a per-variable limit. Relevant limits: netCDF3 NETCDF_CLASSIC format limits fixed variables to sizes smaller than 2^31 B = 2 GiB ~ 2.1 GB, and record variables to that size per record. A single variable may exceed this limit if and only if it is the last defined variable. netCDF3 NETCDF_64BIT_OFFSET format limits fixed variables to sizes smaller than 2^32 B = 4 GiB ~ 4.2 GB, and record variables to that size per record. Any number of variables may reach, though not exceed, this size for fixed variables, or this size per record for record variables. The netCDF3 NETCDF_64BIT_DATA and netCDF4 NETCDF4 formats have no variable size limitations of real-world import. If any variable in your dataset exceeds these limits, alter the output file to a format capacious enough, either netCDF3 classic with 64-bit offsets (with -6 or --64), to PnetCDF/CDF5 with 64-bit data (with -5), or to netCDF4 (with -4 or -7). For more details, see http://nco.sf.net/nco.html#fl_fmt\n"); break;
  } /* end switch */

  /* Print NCO-generated error message, if any */
  if(msg) (void)fprintf(stderr,"%s: ERROR Short NCO-generated message (usually name of function that triggered error): %s\n",fnc_nm,msg);

  /* On occasion, routine is called with non-netCDF errors, e.g., by nco_dfl_case_nc_type_err()
     non-netCDF errors call nco_err_exit() with rcd == 0
     Only attempt to print netCDF error messages when rcd != 0 */
  (void)fprintf(stderr,"%s: ERROR Error code is %d. ",fnc_nm,rcd);
  if(rcd == NC_NOERR){
    (void)fprintf(stderr,"This indicates an error occurred in NCO code or in a system call, not in the netCDF layer.\n");
  }else{
    (void)fprintf(stderr,"Translation into English with nc_strerror(%d) is \"%s\"\n",rcd,nc_strerror(rcd));
  } /* !NC_NOERR */

  (void)fprintf(stdout,"%s: ERROR NCO will now exit with system call %s\n",fnc_nm,exit_fnc_nm);

#ifdef NCO_ABORT_ON_ERROR
  /* abort() produces core dump and traceback information
     Most debuggers (e.g., gdb) use this information to print the calling tree that produced the abort()
     This makes debugging much easier.
     Hence we recommend developers compile NCO with -DNCO_ABORT_ON_ERROR */
  abort();
#else /* !NCO_ABORT_ON_ERROR */
  /* exit() produces no core dump or useful debugger information
     It is slightly more friendly to the end-user since it does not leave core files laying around
     Hence we recommend installing NCO without -DNCO_ABORT_ON_ERROR for end users */
  exit(EXIT_FAILURE);
#endif /* !NCO_ABORT_ON_ERROR */
} /* !nco_err_exit() */

size_t /* O [B] Native type size */
nco_typ_lng /* [fnc] Convert netCDF type enum to native type size */
(const nc_type nco_typ) /* I [enm] netCDF type */
{
  /* Purpose: Return native size of specified netCDF type
     Routine is used to determine memory required to store variables in RAM
     This is only routine in nco_netcdf which needs NCO opaque type definitions
     nco_byte, nco_char, and nco_int defined in nco_typ.h */

  switch(nco_typ){
  case NC_FLOAT:
    return sizeof(float);
  case NC_DOUBLE:
    return sizeof(double);
  case NC_INT:
    return sizeof(nco_int);
  case NC_SHORT:
    return sizeof(short int);
  case NC_CHAR:
    return sizeof(nco_char);
  case NC_BYTE:
    return sizeof(nco_byte);
  case NC_UBYTE:
    return sizeof(nco_ubyte);
  case NC_USHORT:
    return sizeof(nco_ushort);
  case NC_UINT:
    return sizeof(nco_uint);
  case NC_INT64:
    return sizeof(nco_int64);
  case NC_UINT64:
    return sizeof(nco_uint64);
  case NC_STRING:
    return sizeof(nco_string);
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return 0;
} /* !nco_typ_lng() */

size_t /* O [B] Native type size */
nco_typ_lng_udt /* [fnc] Convert netCDF type enum to native type size */
(const int nc_id, /* I [ID] File ID */
 const nc_type nco_typ) /* I [enm] netCDF type */
{
  /* Purpose: Return native size of specified netCDF type, including non-atomic types
     Routine is used to determine memory required to store variables in RAM
     This is only routine in nco_netcdf which needs NCO opaque type definitions
     nco_byte, nco_char, and nco_int defined in nco_typ.h */

  if(nco_typ <= NC_MAX_ATOMIC_TYPE){
    switch(nco_typ){
    case NC_FLOAT:
      return sizeof(float);
    case NC_DOUBLE:
      return sizeof(double);
    case NC_INT:
      return sizeof(nco_int);
    case NC_SHORT:
      return sizeof(short int);
    case NC_CHAR:
      return sizeof(nco_char);
    case NC_BYTE:
      return sizeof(nco_byte);
    case NC_UBYTE:
      return sizeof(nco_ubyte);
    case NC_USHORT:
      return sizeof(nco_ushort);
    case NC_UINT:
      return sizeof(nco_uint);
    case NC_INT64:
      return sizeof(nco_int64);
    case NC_UINT64:
      return sizeof(nco_uint64);
    case NC_STRING:
      return sizeof(nco_string);
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    size_t typ_sz;
    (void)nco_inq_user_type(nc_id,nco_typ,NULL,&typ_sz,NULL,NULL,NULL);
    return typ_sz;
  } /* !udt */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return 0;
} /* !nco_typ_lng_udt() */

const char * /* O [sng] String describing type */
nco_typ_sng /* [fnc] Convert netCDF type enum to string */
(const nc_type type) /* I [enm] netCDF type */
{
  if(type >= NC_FIRSTUSERTYPEID) return "User-defined";

  switch(type){
  case NC_FLOAT:
    return "NC_FLOAT";
  case NC_DOUBLE:
    return "NC_DOUBLE";
  case NC_INT:
    return "NC_INT";
  case NC_SHORT:
    return "NC_SHORT";
  case NC_CHAR:
    return "NC_CHAR";
  case NC_BYTE:
    return "NC_BYTE";
  case NC_UBYTE:
    return "NC_UBYTE";
  case NC_USHORT:
    return "NC_USHORT";
  case NC_UINT:
    return "NC_UINT";
  case NC_INT64:
    return "NC_INT64";
  case NC_UINT64:
    return "NC_UINT64";
  case NC_STRING:
    return "NC_STRING";
  case NC_VLEN:
    return "NC_VLEN";
  case NC_OPAQUE:
    return "NC_OPAQUE";
  case NC_ENUM:
    return "NC_ENUM";
  case NC_COMPOUND:
    return "NC_COMPOUND";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_typ_sng() */

const char * /* O [sng] Native CDL type */
cdl_typ_nm /* [fnc] Return string describing native CDL type */
(const nc_type type) /* I [enm] netCDF type */
{
  /* Purpose: Divine CDL type string from netCDF external type enum */
  switch(type){
  case NC_FLOAT:
    return "float";
  case NC_DOUBLE:
    return "double";
  case NC_INT:
    return "int";
  case NC_SHORT:
    return "short";
  case NC_CHAR:
    return "char";
  case NC_BYTE:
    return "byte";
  case NC_UBYTE:
    return "ubyte";
  case NC_USHORT:
    return "ushort";
  case NC_UINT:
    return "uint";
  case NC_INT64:
    return "int64";
  case NC_UINT64:
    return "uint64";
  case NC_STRING:
    return "string";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end cdl_typ_nm() */

char * /* O [sng] Native CDL type */
cdl_typ_nm_udt /* [fnc] Return string describing native CDL type */
(const int nc_id, /* I [ID] File ID */
 const nc_type type) /* I [enm] netCDF type */
{
  /* Purpose: Divine CDL type string from netCDF external type enum, including non-atomic types */
  /* 20180507: fxm Routine causes bevy of C++ warnings like thise:
     nco_netcdf.c:314:14: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
     return "float"; */
  char typ_nm[NC_MAX_NAME+1L];
  if(type <= NC_MAX_ATOMIC_TYPE){
    switch(type){
    case NC_FLOAT:
      return "float";
    case NC_DOUBLE:
      return "double";
    case NC_INT:
      return "int";
    case NC_SHORT:
      return "short";
    case NC_CHAR:
      return "char";
    case NC_BYTE:
      return "byte";
    case NC_UBYTE:
      return "ubyte";
    case NC_USHORT:
      return "ushort";
    case NC_UINT:
      return "uint";
    case NC_INT64:
      return "int64";
    case NC_UINT64:
      return "uint64";
    case NC_STRING:
      return "string";
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    (void)nco_inq_user_type(nc_id,type,typ_nm,NULL,NULL,NULL,NULL);
    //(void)fprintf(stdout,"DEBUG cdl_typ_nm_udt() reports non-atomic type name is %s\n",typ_nm);
    /* strdup(typ_nm) because typ_nm is local storage, eliminated after return()
       NB: strdup() leads to memory leak unless it is free()'d by calling routine */
    return strdup(typ_nm);
  } /* !udt */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end cdl_typ_nm_udt() */

const char * /* O [sng] Native XML type */
xml_typ_nm /* [fnc] Return string describing native XML type */
(const nc_type type) /* I [enm] netCDF type */
{
  /* Purpose: Divine XML type string from netCDF external type enum */
  switch(type){
  case NC_FLOAT:
    return "float";
  case NC_DOUBLE:
    return "double";
  case NC_INT:
    return "int";
  case NC_SHORT:
    return "short";
  case NC_CHAR:
    return "char";
  case NC_BYTE:
    return "byte";
  case NC_UBYTE:
    return "byte";
  case NC_USHORT:
    return "short";
  case NC_UINT:
    return "int";
  case NC_INT64:
    return "long";
  case NC_UINT64:
    return "long";
  case NC_STRING:
    return "String";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end xml_typ_nm() */

const char * /* O [sng] Native JSON type */
jsn_typ_nm /* [fnc] Return string describing native JSON type */
(const nc_type type) /* I [enm] netCDF type */
{
  /* Purpose: Divine JSON type string from netCDF external type enum */
  switch(type){
  case NC_FLOAT:
    return "float";
  case NC_DOUBLE:
    return "double";
  case NC_INT:
    return "int";
  case NC_SHORT:
    return "short";
  case NC_CHAR:
    return "char";
  case NC_BYTE:
    return "byte";
  case NC_UBYTE:
    return "ubyte";
  case NC_USHORT:
    return "ushort";
  case NC_UINT:
    return "uint";
  case NC_INT64:
    return "int64";
  case NC_UINT64:
    return "uint64";
  case NC_STRING:
    return "string";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end xml_typ_nm() */

const char * /* O [sng] Native CDL type suffix */
cdl_typ_sfx /* [fnc] Return suffix string for CDL type */
(const nc_type type) /* I [enm] netCDF type */
{
  /* Purpose: Return suffix string for CDL */
  switch(type){
  case NC_FLOAT:
    return ".f";
  case NC_DOUBLE:
    return ".";
  case NC_INT:
    return "";
  case NC_SHORT:
    return "s";
  case NC_CHAR:
    return "";
  case NC_BYTE:
    return "b";
  case NC_UBYTE:
    return "ub";
  case NC_USHORT:
    return "us";
  case NC_UINT:
    return "u";
  case NC_INT64:
    return "ll";
  case NC_UINT64:
    return "ull";
  case NC_STRING:
    return "";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end cdl_typ_sfx() */

const char * /* O [sng] Native C type */
c_typ_nm /* [fnc] Return string describing native C type */
(const nc_type type) /* O [enm] netCDF type */
{
  /* Purpose: Divine internal (native) C type string from netCDF external type enum
     fxm: This breaks on Crays where both NC_FLOAT and NC_DOUBLE are native type float
     fxm: Modify this to handle different opaque types correctly
     This may mean defining tokens containing opaque type names in nco_typ.h */
  switch(type){
  case NC_FLOAT:
    return "float";
  case NC_DOUBLE:
    return "double";
  case NC_INT:
    return NCO_INT_SNG;
  case NC_SHORT:
    return "signed short int";
  case NC_CHAR:
    return NCO_CHAR_SNG;
  case NC_BYTE:
    return NCO_BYTE_SNG;
  case NC_UBYTE:
    return NCO_UBYTE_SNG;
  case NC_USHORT:
    return NCO_USHORT_SNG;
  case NC_UINT:
    return NCO_UINT_SNG;
  case NC_INT64:
    return NCO_INT64_SNG;
  case NC_UINT64:
    return NCO_UINT64_SNG;
  case NC_STRING:
    return NCO_STRING_SNG;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end c_typ_nm() */

const char * /* O [sng] Native Fortran77 type */
f77_typ_nm /* [fnc] Return string describing native Fortran77 type */
(const nc_type type) /* O [enm] netCDF type */
{
  /* Purpose: Divine internal (native) Fortran type string from netCDF external type enum */
  switch(type){
  case NC_FLOAT:
    return "real*4";
  case NC_DOUBLE:
    return "real*8";
  case NC_INT:
    return "integer*4";
  case NC_SHORT:
    return "integer*2";
  case NC_CHAR:
    return "character";
  case NC_BYTE:
    return "character";
  case NC_UBYTE:
    return "character";
  case NC_USHORT:
    return "integer*2";
  case NC_UINT:
    return "integer*4";
  case NC_INT64:
    return "integer*8";
  case NC_UINT64:
    return "integer*8";
  case NC_STRING:
    return "character fxm";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end f77_typ_nm() */

const char * /* O [sng] Native Fortran90 type */
f90_typ_nm /* [fnc] Return string describing native Fortran90 type */
(const nc_type type) /* O [enm] netCDF type */
{
  /* Purpose: Divine internal (native) Fortran type string from netCDF external type enum */
  switch(type){
  case NC_FLOAT:
    return "real(selected_real_kind(p=6))";
  case NC_DOUBLE:
    return "real(selected_real_kind(p=12))";
  case NC_INT:
    return "integer(selected_int_kind(6))";
  case NC_SHORT:
    return "integer(selected_int_kind(2))";
  case NC_CHAR:
    return "character(1)";
  case NC_BYTE:
    return "character(1)";
  case NC_UBYTE:
    return "character(1)";
    /* NB: Not sure about generality of integer kinds */
  case NC_USHORT:
    return "integer(selected_int_kind(2))";
  case NC_UINT:
    return "integer(selected_int_kind(6))";
  case NC_INT64:
    return "integer(selected_int_kind(8))";
  case NC_UINT64:
    return "integer(selected_int_kind(8))";
  case NC_STRING:
    return "character(1) fxm";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end f90_typ_nm() */

const char * /* O [sng] String describing file format */
nco_fmt_sng /* [fnc] Convert netCDF file format enum to string */
(const int fl_fmt) /* I [enm] netCDF file format */
{
  /* Purpose: Convert netCDF file format enum to string */
  switch(fl_fmt){
  case NC_FORMAT_CLASSIC:
    return "NC_FORMAT_CLASSIC";
  case NC_FORMAT_64BIT_OFFSET:
    return "NC_FORMAT_64BIT_OFFSET";
  case NC_FORMAT_NETCDF4:
    return "NC_FORMAT_NETCDF4";
  case NC_FORMAT_NETCDF4_CLASSIC:
    return "NC_FORMAT_NETCDF4_CLASSIC";
  case NC_FORMAT_CDF5:
    return "NC_FORMAT_CDF5";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_fmt_sng() */

const char * /* O [sng] String describing extended file format */
nco_fmt_xtn_sng /* [fnc] Convert netCDF extended file format enum to string */
(const int fl_fmt_xtn) /* I [enm] netCDF extended file format */
{
  /* Purpose: Convert netCDF extended file format enum to string */
  /* NB: nc_inq_format_extended() introduced in netCDF 4.3.1, but NC_LIB_VERSION does not work until netCDF 4.4.0 */
#if NC_LIB_VERSION < 440
    switch(fl_fmt_xtn){
      /* NB: netCDF < 4.3.1 unaware of NC_FORMATX_* tokens */
    case NC_FORMAT_NC3:
      return "NC_FORMATX_NC3";
    case NC_FORMAT_NC_HDF5:
      return "NC_FORMATX_NC_HDF5";
    case NC_FORMAT_NC_HDF4:
      return "NC_FORMATX_NC_HDF4";
    case NC_FORMAT_PNETCDF:
      return "NC_FORMATX_PNETCDF";
    case NC_FORMAT_DAP2:
      return "NC_FORMATX_DAP2";
    case NC_FORMAT_DAP4:
      return "NC_FORMATX_DAP4";
    case NC_FORMAT_UNDEFINED:
      return "NC_FORMATX_UNDEFINED";
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
#else /* !NC_LIB_VERSION */
    switch(fl_fmt_xtn){
    case NC_FORMATX_NC3:
      return "NC_FORMATX_NC3"; /* NB: CDF5 self-report NC_FORMATX_NC3 when files opened through netCDF serial API */
    case NC_FORMATX_NC_HDF5:
      return "NC_FORMATX_NC_HDF5";
    case NC_FORMATX_NC_HDF4:
      return "NC_FORMATX_NC_HDF4";
    case NC_FORMATX_PNETCDF:
      return "NC_FORMATX_PNETCDF"; /* CDF5 files report NC_FORMATX_PNETCDF when opened through PnetCDF parallel API */
    case NC_FORMATX_DAP2:
      return "NC_FORMATX_DAP2";
    case NC_FORMATX_DAP4:
      return "NC_FORMATX_DAP4";
    case NC_FORMATX_UNDEFINED:
      return "NC_FORMATX_UNDEFINED";
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
#endif /* !NC_LIB_VERSION */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_fmt_xtn_sng() */

const char * /* O [sng] String describing file format for hidden attributes */
nco_fmt_hdn_sng /* [fnc] Convert netCDF file format enum to string for hidden attributes */
(const int fl_fmt) /* I [enm] netCDF file format */
{
  /* Purpose: Convert netCDF file format enum to string for hidden attributes
     20131229: String values obtained from ncgen man page */
  switch(fl_fmt){
  case NC_FORMAT_CLASSIC:
    return "classic";
  case NC_FORMAT_64BIT_OFFSET:
    return "\"64-bit offset\"";
  case NC_FORMAT_NETCDF4:
    return "netCDF-4";
  case NC_FORMAT_NETCDF4_CLASSIC:
    return "\"netCDF-4 classic model\"";
  case NC_FORMAT_CDF5:
    return "\"64-bit data\"";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_fmt_hdn_sng() */

const char * /* O [sng] String describing endianness for hidden attributes */
nco_ndn_sng /* [fnc] Convert netCDF endianness enum to string for hidden attributes */
(const int flg_ndn) /* I [enm] netCDF endianness */
{
  /* Purpose: Convert netCDF endianness enum to string for hidden attributes */
  switch(flg_ndn){
  case NC_ENDIAN_NATIVE:
    return "classic";
  case NC_ENDIAN_LITTLE:
    return "little";
  case NC_ENDIAN_BIG:
    return "big";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_ndn_sng() */

void
nco_dfl_case_flt_err(void) /* [fnc] Print error and exit for illegal switch(nco_flt_enm) case */
{
  /* Purpose: Print error and exit when switch statement reaches illegal default case
     Routine reduces bloat because many switch() statements invoke this functionality */
  const char fnc_nm[]="nco_dfl_case_flt_err()";
  (void)fprintf(stdout,"%s: ERROR switch statement fell through to default case, which is unsafe. This specific error handler ensures all switch(nco_flt_enm) statements are fully enumerated. This error may be triggered by invoking a filter (i.e., a compression filter like BitGroom or Zstandard) that is unknown to and/or not supported by the CCR library that NCO depends on for these filters.\nHINT: Choose a different filter or Re-Configure/build NCO with --enable-ccr after installing the latest CCR. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* !nco_dfl_case_flt_err() */

void
nco_dfl_case_generic_err(void) /* [fnc] Print error and exit for illegal switch case */
{
  /* Purpose: Print error and exit when switch statement reaches illegal default case
     Routine reduces bloat because many switch() statements invoke this functionality */
  const char fnc_nm[]="nco_dfl_case_generic_err()";
  (void)fprintf(stdout,"%s: ERROR switch statement fell through to default case, which is unsafe. This generic error handler ensures all switch statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* !nco_dfl_case_generic_err() */

void
nco_dfl_case_nc_type_err(void) /* [fnc] Print error and exit for illegal switch(nc_type) case */
{
  /* Purpose: Print error and exit when switch statement reaches illegal default case
     Routine reduces bloat because many switch() statements invoke this functionality */

  /* Purpose: Convenience routine for printing error and exiting when
     switch(nctype) statement receives an illegal default case
     NCO emits warnings when compiled by GCC with -DNETCDF2_ONLY since,
     apparently, there are a whole bunch of things besides numeric
     types in the old nctype enum and gcc warns about enums that are
     not exhaustively considered in switch() statements.
     All these default statements can be removed with netCDF3 interface
     so perhaps these should be surrounded with #ifdef NETCDF2_ONLY
     constructs, but they actually do make sense for netCDF3 as well
     so I have implemented a uniform error function, nco_dfl_case_nc_type_err(),
     to be called by all routines which emit errors only when compiled with
     NETCDF2_ONLY.
     This makes the behavior easy to modify or remove in the future. */
  const char fnc_nm[]="nco_dfl_case_nc_type_err()";
  (void)fprintf(stdout,"%s: ERROR switch(nctype) statement fell through to default case, which is illegal, because each type should have a well-defined action. This error may be triggered by using an NCO built with only netCDF3 functionality to examine a netCDF4 dataset that contains a new atomic type (e.g., NC_INT64).\nHINT: Configure/build NCO with --enable-netCDF4. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* !nco_dfl_case_nc_type_err() */

void
nco_dfl_case_prg_id_err(void) /* [fnc] Print error and exit for illegal switch(nco_prg_id) case */
{
  /* Purpose: Print error and exit when switch statement reaches illegal default case
     Routine reduces bloat because many switch() statements invoke this functionality */
  const char fnc_nm[]="nco_dfl_case_prg_id_err()";
  (void)fprintf(stdout,"%s: ERROR switch(nco_prg_id) statement fell through to default case, which is unsafe. This specific error handler ensures all switch(nco_prg_id) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* !nco_dfl_case_prg_id_err() */

void
nco_sng_cnv_err /* [fnc] Print error and exit for failed strtol()-type calls */
(const char *cnv_sng, /* I [sng] String to convert */
 const char *sng_cnv_fnc, /* I [sng] Name of function used to convert string */
 const char *err_ptr) /* I [chr] First illegal character in string */
{
  /* Purpose: Convenience routine for printing error and exiting when
     strtol()/strtoul()/strtoll()/strtoull() receives illegal characters.
     Placing this in its own routine saves many lines of error handling. */
  const char fnc_nm[]="nco_sng_cnv_err()";
  if(!strcmp(sng_cnv_fnc,"strtod")){
    /* Handle conversion errors for strtod()... */
    (void)fprintf(stdout,"%s: ERROR an NCO function or main program attempted to convert the (probably user-defined) string \"%s\" to a floating point type using the standard C-library function \"%s()\". This function stopped converting the input string when it encountered the illegal (i.e., non-numeric) character \'%c\'. This probably indicates a syntax error by the user. Please check the argument syntax and re-try the command. ",fnc_nm,cnv_sng,sng_cnv_fnc,err_ptr[0]);
  }else{ /* !strtod() */
    /* Handle conversion errors for strtol(), strtoul()... */
    (void)fprintf(stdout,"%s: ERROR an NCO function or main program attempted to convert the user-defined string \"%s\" to an integer-type using the standard C-library function \"%s()\". This function stopped converting the input string when it encountered the illegal (i.e., non-numeric or non-integer) character \'%c\'. This probably indicates a syntax error by the user. Please check the argument syntax and re-try the command. ",fnc_nm,cnv_sng,sng_cnv_fnc,err_ptr[0]);
  } /* !strtod() */
  if(err_ptr[0] == ',') (void)fprintf(stdout,"HINT: Conversion functions like \"%s()\" accept only one number at a time, so comma-separated lists of numbers are invalid. ",sng_cnv_fnc);
  (void)fprintf(stdout,"Exiting...\n");
  nco_err_exit(0,fnc_nm);
} /* !nco_sng_cnv_err() */

/* Begin file-level routines */
int
nco_create(const char * const fl_nm,const int cmode,int * const nc_id)
{
  /* Purpose: Wrapper for nc_create() */
  const char fnc_nm[]="nco_create()";
  int rcd;
  rcd=nc_create(fl_nm,cmode,nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to create file \"%s\"\n",fnc_nm,fl_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_create */

int
nco__create(const char * const fl_nm,const int cmode,const size_t sz_ntl,size_t * const bfr_sz_hnt,int * const nc_id)
{
  /* Purpose: Wrapper for nc__create() */
  const char fnc_nm[]="nco__create()";
  int rcd;
  rcd=nc__create(fl_nm,cmode,sz_ntl,bfr_sz_hnt,nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to create file \"%s\"\n",fnc_nm,fl_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco__create */

int
nco_open(const char * const fl_nm,const int mode,int * const nc_id)
{
  /* Purpose: Wrapper for nc_open() */
  const char fnc_nm[]="nco_open()";
  int rcd;
  rcd=nc_open(fl_nm,mode,nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to open file \"%s\"\n",fnc_nm,fl_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_open */

int
nco__open(const char * const fl_nm,const int mode,size_t * const bfr_sz_hnt,int * const nc_id)
{
  /* Purpose: Wrapper for nc__open() */
  const char fnc_nm[]="nco__open()";
  int rcd;
  rcd=nc__open(fl_nm,mode,bfr_sz_hnt,nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to open file \"%s\"\n",fnc_nm,fl_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco__open */

int
nco_open_flg(const char * const fl_nm,const int mode,int * const nc_id)
{
  /* Purpose: Error-tolerant wrapper for nc_open(). Tolerates all errors.
     Currently used only by nco_fl_mk_lcl() to test file accessibility via DAP */
  int rcd;
  rcd=nc_open(fl_nm,mode,nc_id);
  return rcd;
} /* !nco_open */

/* #if NC_LIB_VERSION < 440 */
#ifndef HAVE_NETCDF_MEM_H
int
nc_open_mem(const char * const fl_nm,const int mode,const size_t sz,void * const void_ptr,int * const nc_id)
{
  /* Purpose: Pseudo-library stub function to open a netCDF file stored in RAM
     Only called by NCO built against a netCDF library that pre-dates nc_open_mem() */
  int rcd;
  const char fnc_nm[]="nc_open_mem()";
  rcd=strlen(fl_nm)+mode+sz;
  (void)fprintf(stdout,"ERROR: %s reports attempt to open file memory was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.4.0-rc1 (released ~20150610) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  *nc_id=*((int *)void_ptr);
  return rcd;
} /* end nc_open_mem() */
#endif /* !HAVE_NETCDF_MEM_H */

#if NC_LIB_VERSION < 462 
int
nc_create_mem(const char * const fl_nm,const int mode,const size_t sz_ntl,int * const nc_id)
{
  /* Purpose: Pseudo-library stub function to create a netCDF file stored in RAM
     Only called by NCO built against a netCDF library that pre-dates nc_create_mem() */
  const char fnc_nm[]="nc_create_mem()";
  int rcd;
  rcd=strlen(fl_nm)+mode+sz_ntl;
  (void)fprintf(stdout,"ERROR: %s reports attempt to create file in memory was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.6.2 (released ~201810) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  *nc_id+=mode*0;
  return rcd;
} /* !nc_create_mem() */

int
nc_open_memio(const char * const fl_nm,const int mode,NC_memio * const info,int * const nc_id)
{
  /* Purpose: Pseudo-library stub function to open a netCDF file stored in RAM
     Only called by NCO built against a netCDF library that pre-dates nc_open_memio() */
  int rcd;
  const char fnc_nm[]="nc_open_memio()";
  rcd=strlen(fl_nm)+mode;
  (void)fprintf(stdout,"ERROR: %s reports attempt to open file memory was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.6.2 (released ~201810) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  *nc_id+=mode*0;
  return rcd;
} /* end nc_open_memio() */

int
nc_close_memio(const int nc_id,NC_memio * const info)
{
  /* Purpose: Pseudo-library stub function to close a netCDF file stored in RAM
     Only called by NCO built against a netCDF library that pre-dates nc_close_memio() */
  int rcd=NC_NOERR; /* CEWI */
  const char fnc_nm[]="nc_close_memio()";
  (void)fprintf(stdout,"ERROR: %s reports attempt to close file memory was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.6.2 (released ~201810) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  info->size+=0L;
  return rcd;
} /* end nc_close_memio() */
#endif /* 4.6.2 */

int
nco_open_mem(const char * const fl_nm,const int mode,const size_t sz,void * const void_ptr,int * const nc_id)
{
  /* Purpose: Wrapper for nc_open_mem() */
  const char fnc_nm[]="nco_open_mem()";
  int rcd;
  rcd=nc_open_mem(fl_nm,mode,sz,void_ptr,nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to open_mem() file \"%s\"\n",fnc_nm,fl_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_open_mem() */

int
nco_create_mem(const char * const fl_nm,const int mode,const size_t sz_ntl,int * const nc_id)
{
  /* Purpose: Wrapper for nc_create_mem() */
  const char fnc_nm[]="nco_create_mem()";
  int rcd;
  rcd=nc_create_mem(fl_nm,mode,sz_ntl,nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to create_mem() file \"%s\"\n",fnc_nm,fl_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_create_mem() */

int
nco_open_memio(const char * const fl_nm,const int mode,NC_memio * const info,int * const nc_id)
{
  /* Purpose: Wrapper for nc_open_memio() */
  const char fnc_nm[]="nco_open_memio()";
  int rcd;
  rcd=nc_open_memio(fl_nm,mode,info,nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to open_memio() file \"%s\"\n",fnc_nm,fl_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_open_memio() */

int
nco_close_memio(const int nc_id,NC_memio * const info)
{
  /* Purpose: Wrapper for nc_close_memio() */
  const char fnc_nm[]="nco_close_memio()";
  int rcd;
  rcd=nc_close_memio(nc_id,info);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s unable to close_memio() with nc_id=%d\n",fnc_nm,nc_id);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_close_memio() */

#if NC_LIB_VERSION < 460
int nc_def_var_filter(const int nc_id,const int var_id,const unsigned int flt_id,const size_t prm_nbr,const unsigned int * const prm_lst)
{
  /* Purpose: Pseudo-library stub function to create a filter for a variable
     This particular stub routine is only called by netCDF4-enabled code
     when built against a netCDF library that it too old to have the nc_def_var_filter() function. */
  int rcd;
  const char fnc_nm[]="nc_def_var_filter()";
  rcd=NC_NOERR+0*(nc_id+var_id+flt_id+prm_nbr+*prm_lst); /* CEWI */
  (void)fprintf(stdout,"ERROR: %s reports define variable filter was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.6.0 (released ~20180125) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nc_def_var_filter() */

int nc_inq_var_filter(const int nc_id,const int var_id,unsigned int * const flt_id,size_t * const prm_nbr,unsigned int * const prm_lst)
{
  /* Purpose: Pseudo-library stub function to inquire a filter for a variable
     This particular stub routine is only called by netCDF4-enabled code
     when built against a netCDF library too old to have the nc_inq_var_filter() function. */
  int rcd;
  const char fnc_nm[]="nc_inq_var_filter()";
  rcd=NC_NOERR+0*(nc_id+var_id);
  *flt_id=*prm_nbr=*prm_lst=rcd; /* CEWI */
  (void)fprintf(stdout,"ERROR: %s reports inquire variable filter was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.6.0 (released ~20180125) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* end nc_inq_var_filter() */
#endif /* !4.6.0 */

#if NC_LIB_VERSION < 474
int nc_inq_var_filter_ids(const int nc_id,const int var_id,size_t * const flt_nbr,unsigned int * const flt_lst)
{
  /* Purpose: Pseudo-library stub function to inquire all filter IDs for a variable
     This particular stub routine is only called by netCDF4-enabled code
     when built against a netCDF library too old to have the nc_inq_var_filter_ids() function. */
  int rcd;
  const char fnc_nm[]="nc_inq_var_filter_ids()";
  rcd=NC_NOERR+0*(nc_id+var_id);
  *flt_nbr=*flt_lst=rcd; /* CEWI */
  (void)fprintf(stdout,"ERROR: %s reports inquire variable filter was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.7.4 (released ~20200327) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* end nc_inq_var_filter_ids() */

int nc_inq_var_filter_info(const int nc_id,const int var_id,const unsigned int flt_id,size_t * const prm_nbr,unsigned int * const prm_lst)
{
  /* Purpose: Pseudo-library stub function to inquire specific filter settings for a variable
     This particular stub routine is only called by netCDF4-enabled code
     when built against a netCDF library too old to have the nc_inq_var_filter_info() function. */
  int rcd;
  const char fnc_nm[]="nc_inq_var_filter_info()";
  rcd=NC_NOERR+0*(nc_id+var_id);
  *prm_nbr=*prm_lst=rcd; /* CEWI */
  (void)fprintf(stdout,"ERROR: %s reports inquire variable filter information was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.7.4 (released ~20200327) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* end nc_inq_var_filter_info() */
#endif /* !4.7.4 */

int nc_def_var_filter_ccr(const int nc_id,const int var_id,const char * const flt_nm,const size_t prm_nbr,const char ** const prm_lst)
{
  /* Purpose: Placeholder stub function to assign a CCR filter to a variable
     This is experimental
     There is not, and may never be, a netCDF library function nc_def_var_filter_ccr(). */
  int rcd;
  const char fnc_nm[]="nc_def_var_filter_ccr()";
  rcd=NC_NOERR+0*(nc_id+var_id+*flt_nm+prm_nbr+(int)**prm_lst); /* CEWI */
  (void)fprintf(stdout,"ERROR: %s reports define variable filter was foiled because libnetcdf.a does not contain %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.8.0 (released ~20201001) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nc_def_var_filter_ccr() */

#ifdef ENABLE_MPI
# ifdef HAVE_NETCDF4_H
/* netCDF4 routines defined by Unidata netCDF4 Library libnetcdf.a 
   20051129: nc_*_par() routines require that netCDF4 be configured for parallel filesystems */
int 
nco_create_par(const char * const fl_nm,const int cmode,MPI_Comm mpi_cmm,MPI_Info mpi_nfo,int * const nc_id)
{
  /* Purpose: Wrapper for nc_create_par() */
  const char fnc_nm[]="nco_create_par()";
  int rcd;
  rcd=nc_create_par(fl_nm,cmode,mpi_cmm,mpi_nfo,nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_create_par */

int 
nco_open_par(const char * const fl_nm,const int mode,MPI_Comm mpi_cmm,MPI_Info mpi_nfo,int * const nc_id)
{
  /* Purpose: Wrapper for nc_open_par() */
  const char fnc_nm[]="nco_open_par()";
  int rcd;
  rcd=nc_open_par(fl_nm,mode,mpi_cmm,mpi_nfo,nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_open_par */

int 
nco_var_par_access(const int nc_id,const int var_id,const int par_access)
{
  /* Purpose: Wrapper for nc_var_par_access()
     NB: HDF5 allows per-variable setting of access type
     PnetCDF sets all variables to same access type */
  const char fnc_nm[]="nco_var_par_access()";
  int rcd;
  rcd=nc_var_par_access(nc_id,var_id,par_access);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_var_par_access */
# endif /* !HAVE_NETCDF4_H */

# ifdef PNETCDF_EXPOSED_API
/* PnetCDF routines defined by ANL Parallel netCDF Library libpnetcdf.a
   NB: netCDF4 versions 4.3.x automagically call PnetCDF internally, yet do not expose its API
   These sample prototypes will never be used until/unless someone exposes the PnetCDF API to NCO (unlikely)
   Until then, the token ENABLE_PNETCDF means that netCDF4 was built with --enable-pnetcdf */
int
ncompi_create(MPI_Comm mpi_cmm,const char * const fl_nm,const int cmode,MPI_Info mpi_nfo,int * const nc_id)
{
  /* Purpose: Wrapper for ncmpi_create() */
  const char fnc_nm[]="ncompi_create()";
  int rcd;
  rcd=ncmpi_create(mpi_cmm,fl_nm,cmode,mpi_nfo,nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !ncompi_create */
int
ncompi_open(MPI_Comm mpi_cmm,const char * const fl_nm,const int omode,MPI_Info mpi_nfo,int * const nc_id)
{
  /* Purpose: Wrapper for ncmpi_open() */
  const char fnc_nm[]="ncompi_open()";
  int rcd;
  rcd=ncmpi_open(mpi_cmm,fl_nm,omode,mpi_nfo,nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !ncompi_open */
# endif /* !PNETCDF_EXPOSED_API */
#endif /* !ENABLE_MPI */

int
nco_redef(const int nc_id)
{
  /* Purpose: Wrapper for nc_redef() */
  int rcd;
  rcd=nc_redef(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_redef()");
  return rcd;
} /* !nco_redef */

int
nco_set_fill(const int nc_id,const int fill_mode,int * const old_mode)
{
  /* Purpose: Wrapper for nc_set_fill() */
  int rcd;
  rcd=nc_set_fill(nc_id,fill_mode,old_mode);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_set_fill()");
  return rcd;
} /* !nco_set_fill */

int
nco_enddef(const int nc_id)
{
  /* Purpose: Wrapper for nc_enddef() */
  int rcd;
  rcd=nc_enddef(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_enddef()");
  return rcd;
} /* !nco_enddef */

int /* O [enm] netCDF error code */
nco__enddef /* [fnc] Wrapper for nc__enddef */
(const int nc_id, /* [ID] netCDF ID */
 const size_t hdr_pad) /* [B] Pad at end of header section */
{
  /* Purpose: Wrapper for nc__enddef() */
  int rcd;
  /* hdr_pad is netCDF library h_minfree variable */
  const size_t v_align=4UL; /* [B] Alignment of beginning of data section for fixed variables */
  const size_t v_minfree=0UL; /* [B] Pad at end of data section for fixed size variables */
  const size_t r_align=4UL; /* [B] Alignment of beginning of data section for record variables */
  /* nc_enddef(ncid) is equivalent to nc__enddef(ncid,0,4,0,4) */
  rcd=nc__enddef(nc_id,hdr_pad,v_align,v_minfree,r_align);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco__enddef()");
  return rcd;
} /* !nco__enddef */

int
nco_sync(const int nc_id)
{
  /* Purpose: Wrapper for nc_sync() */
  int rcd;
  rcd=nc_sync(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_sync()");
  return rcd;
} /* !nco_sync */

int
nco_abort(const int nc_id)
{
  /* Purpose: Wrapper for nc_abort() */
  int rcd;
  rcd=nc_abort(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_abort()");
  return rcd;
} /* !nco_abort */

int
nco_close(const int nc_id)
{
  /* Purpose: Wrapper for nc_close() */
  const char fnc_nm[]="nco_close()";
  int rcd=NC_NOERR;
  /* 20171108: Activate this check by building NCO with, e.g., CPPFLAGS='-DNCO_CDF5_BUG_CHK' ./configure ... */
#ifdef NCO_CDF5_BUG_CHK
# if NC_LIB_VERSION >= 440
  /* 20170912 Diagnose whether file may be (for input files) or is (for output files) infected by CDF5 bug */
  int fl_fmt; /* [enm] File format */
  int fl_fmt_xtn; /* [enm] Extended file format */
  int mode; /* [enm] Mode */
  rcd=nc_inq_format(nc_id,&fl_fmt);
  rcd=nc_inq_format_extended(nc_id,&fl_fmt_xtn,&mode);
  if(fl_fmt == NC_FORMAT_CDF5){
    char *path=NULL;
    char var_nm[NC_MAX_NAME+1L];
    int dmn_id[NC_MAX_VAR_DIMS];
    int var_id[NC_MAX_VARS];
    int bug_idx=-1;
    int bug_nbr=0;
    int dmn_idx;
    int dmn_nbr;
    int FIRST_WARNING=1;
    int var_idx;
    int var_nbr;
    nc_type var_typ;
    size_t dmn_sz[NC_MAX_VAR_DIMS];
    size_t pathlen;
    size_t var_sz;
    rcd=nc_inq_path(nc_id,&pathlen,NULL);
    path=(char *)malloc(pathlen*sizeof(char));
    rcd=nc_inq_path(nc_id,NULL,path);
    //(void)fprintf(stdout,"DEBUG: %s reports NC_LIB_VERSION = %d.\n",fnc_nm,NC_LIB_VERSION);
    //(void)fprintf(stdout,"DEBUG: %s reports file format and extended format are %d = %s and %d = %s, respectively\n",fnc_nm,fl_fmt,nco_fmt_sng(fl_fmt),fl_fmt_xtn,nco_fmt_xtn_sng(fl_fmt_xtn));
    //(void)fprintf(stdout,"DEBUG: %s reports file mode is %o (octal) = %d (decimal) = %04x (hex)\n",fnc_nm,mode,(unsigned)mode,(unsigned)mode);
    rcd=nc_inq_varids(nc_id,&var_nbr,var_id);
    for(var_idx=0;var_idx<var_nbr;var_idx++){
      var_sz=1L;
      rcd=nc_inq_varndims(nc_id,var_id[var_idx],&dmn_nbr);
      rcd=nc_inq_vardimid(nc_id,var_id[var_idx],dmn_id);
      for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
	rcd=nc_inq_dimlen(nc_id,dmn_id[dmn_idx],dmn_sz+dmn_idx);
	var_sz*=dmn_sz[dmn_idx];
      } /* !dmn_idx */
      rcd=nc_inq_vartype(nc_id,var_id[var_idx],&var_typ);
      var_sz*=nco_typ_lng(var_typ);
      if(var_sz > 4ULL*1073741824ULL){ /* 4 GiB */
	if(FIRST_WARNING){
	  (void)fprintf(stdout,"INFO: %s currently closing and sniffing-around for corruption in CDF5 file %s\n",fnc_nm,path);
	  FIRST_WARNING=0;
	} /* !FIRST_WARNING */
	rcd=nc_inq_varname(nc_id,var_id[var_idx],var_nm);
	(void)fprintf(stdout,"WARNING: %s reports variable %s is \"large\" (%lu B =~ %lu GiB > 4294967296 B = 4 GiB)\n",fnc_nm,var_nm,(unsigned long)var_sz,(unsigned long)(1.0*var_sz/1073741824UL));
	bug_idx=var_idx;
	bug_nbr++;
      } /* !var_sz */
    } /* !var_idx */
    if(path) free((void *)path);
    if(bug_nbr > 0){
      (void)fprintf(stdout,"WARNING: %s reports total number of \"large\" (> 4 GiB) variables in this CDF5 file is %d\n",fnc_nm,bug_nbr);
      if(bug_nbr > 1 || bug_idx != var_nbr-1){
	(void)fprintf(stdout,"WARNING: %s reports at least one \"large\" (> 4 GiB) variable in this CDF5 file is not the last variable defined. Writing CDF5 files with large variables is buggy in netCDF library versions 4.4.0-4.6.0 unless there is only one such \"large\" variable and it is the last to be defined. Input datasets (that NCO reads) that were originally written by PnetCDF are likely fine (because PnetCDF writes CDF5 through a different mechanism than serial programs like NCO's writer). And CDF5 datasets originally written by any netCDF version 4.6.1 (the first public release that fixes the CDF5 bug that we identified on 20170906) or greater may be fine. However, CDF5 input files written by any serial netCDF writer (like NCO) employing netCDF library 4.4.0-4.6.0 are likely corrupt and variables were silently truncated when writing it. An output file (that this NCO _writes_) will definitely be corrupt, as this NCO employs (i.e., is linked to) netCDF library version %d which is buggy (so please upgrade to netCDF 4.6.1+ ASAP).\nHINT: There are two potential solutions for data affected by this bug: 1. Re-write (using any netCDF version) original input files in netCDF4 format instead of CDF5, then process these as normal and write netCDF4 output (instead of CDF5); 2. Re-compile NCO with netCDF library 4.6.1 or later and use it to convert non-corrupt datasets to netCDF4 format, then process the data. This message should only appear if there is a possibility that you are reading or writing a corrupt dataset. Sorry to scare you if this is a false positive. For more information on this nasty bug, see https://github.com/Unidata/netcdf-c/issues/463\n",fnc_nm,NC_LIB_VERSION);
      }else{
	(void)fprintf(stdout,"WARNING: Congratulations! %s reports that the only \"large\" (> 4 GiB) variable in this CDF5 file appears to be the last variable defined. Writing CDF5 files with large variables is buggy in netCDF library versions 4.4.0-4.6.0 (this NCO is linked to netCDF library version %d) unless there is only one such \"large\" variable and it is the last to be defined. Therefore this file may be fine, i.e., not corrupted by this nasty netCDF CDF5 bug: https://github.com/Unidata/netcdf-c/issues/463. Sorry not to scare you if this is a false negative.\n",fnc_nm,NC_LIB_VERSION);
      } /* !bug_idx */
    } /* !bug_nbr */
  } /* !CDF5 */
# endif /* !NC_LIB_VERSION */
#endif /* !NCO_CDF5_BUG_CHK */
  rcd=nc_close(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_close */

int
nco_inq(const int nc_id,int * const dmn_nbr_fl,int * const var_nbr_fl,int * const att_glb_nbr,int * const rec_dmn_id)
{
  /* Purpose: Wrapper for nc_inq() */
  int rcd;
  rcd=nc_inq(nc_id,dmn_nbr_fl,var_nbr_fl,att_glb_nbr,rec_dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq()");
  return rcd;
} /* !nco_inq() */

/* NB: nc_inq_path() introduced in netCDF 4.3.2, but NC_LIB_VERSION does not work until netCDF 4.4.0 */
#ifndef HAVE_NC_INQ_PATH
int nc_inq_path(const int nc_id,size_t * const pathlen,char * const path)
{
  /* Purpose: 20170913: Stub for nc_inq_path(), introduced in netCDF 4.3.2
     Forward compatibility prototype required for systems with netCDF < 4.3.2 */
  *pathlen=0L;
  *path=NULL;
  return NC_NOERR+0*nc_id; /* CEWI */
} /* !nc_inq_path() */
#endif /* !HAVE_NC_INQ_PATH */
int nco_inq_path(const int nc_id,size_t * const pathlen,char * const path)
{
  /* Purpose: Wrapper for nc_inq_path() */
  int rcd;
  rcd=nc_inq_path(nc_id,pathlen,path);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_format()");
  return rcd;
} /* !nco_inq_path() */

  /* NB: nc_inq_format() introduced in netCDF 3.6.1, but NC_LIB_VERSION does not work until netCDF 4.4.0 */
#ifndef HAVE_NC_INQ_FORMAT
int nc_inq_format(int nc_id,int * const fl_fmt)
{
  /* Purpose: Stub for nc_inq_format(), introduced in netCDF 3.6.1 or 3.6.2
     20070901 Current OPeNDAP does not have nc_inq_format() and thus requires this stub */
  *fl_fmt=NC_FORMAT_CLASSIC; /* [enm] Output file format */
  return NC_NOERR+0*nc_id; /* CEWI */
} /* end nc_inq_format() */
#endif /* !HAVE_NC_INQ_FORMAT */
int
nco_inq_format(const int nc_id,int * const fl_fmt)
{
  /* Purpose: Wrapper for nc_inq_format() */
  int rcd;
  /* NB: Function nc_inq_format(int ncid, int *formatp) appeared in netCDF 3.6.1
     Forward compatibility prototype required for systems with netCDF < 3.6.1 */
  rcd=nc_inq_format(nc_id,fl_fmt);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_format()");
  return rcd;
} /* !nco_inq_format() */

  /* NB: nc_inq_format_extended() introduced in netCDF 4.3.1, but NC_LIB_VERSION does not work until netCDF 4.4.0 */
#ifndef NC_HAVE_INQ_FORMAT_EXTENDED
int nc_inq_format_extended(const int nc_id,int * const fl_fmt,int * const mode)
{
  /* Purpose: 20131222: Stub for nc_inq_format_extended(), which appeared in netCDF 4.3.1-rc7
     Forward compatibility prototype required for systems with netCDF < 4.3.1 */
  int rcd=NC_NOERR;
  if(fl_fmt) *fl_fmt=NC_FORMAT_UNDEFINED; /* [enm] Output file format */
  if(mode) *mode=0; /* [enm] Output file format */
  return NC_NOERR+0*nc_id+0*rcd; /* CEWI */
} /* end nc_inq_format_extended() */
#endif /* !NC_HAVE_INQ_FORMAT_EXTENDED */
int
nco_inq_format_extended(const int nc_id,int * const fl_fmt,int * const mode)
{
  /* Purpose: Wrapper for nc_inq_format_extended() */
  int rcd=NC_NOERR;
  /* NB: 20131222: Function nc_inq_format_extended(int ncid,int *formatp,int *mode) appeared in netCDF 4.3.1-rc7
     Forward compatibility prototype required for systems with netCDF < 4.3.1 */
  /* NUG: "netCDF API presents file as if it had the format specified by nc_inq_format(). The true file format, however, may not even be a netCDF file; it might be DAP, HDF4, or PNETCDF, for example. nc_inq_format_extended() returns that true file type. It also returns the effective mode for the file. */
  rcd=nc_inq_format_extended(nc_id,fl_fmt,mode);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_format_extended()");
  return rcd;
} /* !nco_inq_format_extended() */

/* 20210311: nc_set_log_level() prototype always defined in netcdf.h since netCDF a.b.c, though definition could be disabled with --disable-logging (or must be enabled with --enable-logging? 
   Libraries libnetcdf.a <= 4.6.1 predate this PR https://github.com/Unidata/netcdf-c/pull/969/files and have strange implementation of nc_set_log_level() that requires additional ifdefs */
#ifndef HAVE_NC_SET_LOG_LEVEL
# ifndef ENABLE_SET_LOG_LEVEL
#  define nc_set_log_level(e) nc_set_log_level(e)
# endif /* !ENABLE_SET_LOG_LEVEL */
int nc_set_log_level(const int log_lvl)
{
  /* Purpose: Stub for nc_set_log_level()
     Required for netCDF libraries compiled without --enable-logging */
  return NC_NOERR+0*log_lvl; /* CEWI */
} /* !nc_set_log_level() */
#endif /* !HAVE_NC_SET_LOG_LEVEL */

#ifdef HAVE_NETCDF4_H
int
nco_inq_ncid(const int nc_id,const char * const grp_nm,int * const grp_id)
{
  /* Purpose: Wrapper for nc_inq_ncid() */
  int rcd;
  rcd=nc_inq_ncid(nc_id,grp_nm,grp_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_ncid()");
  return rcd;
} /* !nco_inq_ncid() */

int
nco_inq_ncid_flg(const int nc_id,const char * const grp_nm,int * const grp_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_ncid(). Tolerates NC_ENOGRP. */
  int rcd;
  rcd=nc_inq_ncid(nc_id,grp_nm,grp_id);
  if(rcd == NC_ENOGRP) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_ncid()");
  return rcd;
} /* !nco_inq_ncid_flg() */
#endif /* !HAVE_NETCDF4_H */

int
nco_inq_ndims(const int nc_id,int * const dmn_nbr_fl)
{
  /* Purpose: Wrapper for nc_inq_ndims() */
  int rcd;
  rcd=nc_inq_ndims(nc_id,dmn_nbr_fl);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_ndims()");
  return rcd;
} /* !nco_inq_ndims() */

int
nco_inq_nvars(const int nc_id,int * const var_nbr_fl)
{
  /* Purpose: Wrapper for nc_inq_nvars() */
  int rcd;
  rcd=nc_inq_nvars(nc_id,var_nbr_fl);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_nvars()");
  return rcd;
} /* !nco_inq_nvars() */

int
nco_inq_natts(const int nc_id,int * const att_glb_nbr)
{
  /* Purpose: Wrapper for nc_inq_natts() */
  int rcd;
  rcd=nc_inq_natts(nc_id,att_glb_nbr);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_natts()");
  return rcd;
} /* !nco_inq_natts() */

int
nco_inq_unlimdim(const int nc_id,int * const rec_dmn_id)
{
  /* Purpose: Wrapper for nc_inq_unlimdim() */
  int rcd;
  rcd=nc_inq_unlimdim(nc_id,rec_dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_unlimdim()");
  return rcd;
} /* !nco_inq_unlimdim() */

int
nco_inq_unlimdim_flg(const int nc_id,int * const rec_dmn_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_unlimdim(). Tolerates NC_EBADDIM. */
  int rcd;
  rcd=nc_inq_unlimdim(nc_id,rec_dmn_id);
  if(rcd == NC_EBADDIM) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_unlimdim_flg()");
  return rcd;
} /* !nco_inq_unlimdim_flg() */

int nco_get_chunk_cache
(size_t * const sz_byt, /* [B] Raw data chunk cache size */
 size_t * const cnk_nbr_hsh, /* [nbr] Chunk slots in raw data chunk cache hash table */
 float * const pmp_fvr_frc) /* [frc] Preemption favor fraction */
{
  /* Purpose: Wrapper for nc_get_chunk_cache() */
  int rcd;
  rcd=nc_get_chunk_cache(sz_byt,cnk_nbr_hsh,pmp_fvr_frc);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_chunk_cache()");
  return rcd;
} /* !nco_get_chunk_cache() */

int nco_set_chunk_cache
(const size_t sz_byt, /* [B] Raw data chunk cache size */
 const size_t cnk_nbr_hsh, /* [nbr] Chunk slots in raw data chunk cache hash table */
 const float pmp_fvr_frc) /* [frc] Preemption favor fraction */
{
  /* Purpose: Wrapper for nc_set_chunk_cache() */
  int rcd;
  rcd=nc_set_chunk_cache(sz_byt,cnk_nbr_hsh,pmp_fvr_frc);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_set_chunk_cache()");
  return rcd;
} /* !nco_set_chunk_cache() */

/* End File routines */

/* Begin Group routines (_grp) */
#ifdef HAVE_NETCDF4_H
int nco_def_grp(const int nc_id,const char * const grp_nm,int * const grp_id)
{
  /* Purpose: Wrapper for nc_def_grp() */
  int rcd;
  rcd=nc_def_grp(nc_id,grp_nm,grp_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_grp()");
  return rcd;
} /* !nco_def_grp() */

int nco_def_grp_flg(const int nc_id,const char * const grp_nm,int * const grp_id)
{
  /* Purpose: Error-tolerant wrapper for nc_def_grp(). Tolerates NC_ENAMEINUSE (-42) "String match to name in use". */
  int rcd;
  rcd=nc_def_grp(nc_id,grp_nm,grp_id);
  if(rcd == NC_ENAMEINUSE) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_grp_flg()");
  return rcd;
} /* !nco_def_grp_flg() */

#ifndef HAVE_NC_RENAME_GRP
int
nc_rename_grp(int grp_id,const char * const grp_nm)
{
  /* Purpose: Pseudo-library stub function to rename groups.
     This particular stub routine is only called by netCDF4-enabled code
     when built against a netCDF library that it too old to have the nc_rename_grp() function.
     Test by running something like this:
     ncrename -O -g g1,obama ~/nco/data/in_grp.nc ~/foo.nc */
  const char fnc_nm[]="nc_rename_grp()";
  char grp_nm_old[NC_MAX_NAME+1L];
  int rcd;
  rcd=NC_NOERR;
  rcd+=nco_inq_grpname(grp_id,grp_nm_old);
  (void)fprintf(stdout,"INFO: %s reports attempt to rename group \"%s\" to \"%s\" was foiled because libnetcdf.a does not contain nc_rename_grp(). To obtain this functionality, please rebuild NCO against netCDF library version 4.3.1-pre1 (released ~201309) or later.\nContinuing as though nothing untoward happened...\n",fnc_nm,grp_nm_old,grp_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* end nc_rename_grp() */
#endif /* HAVE_NC_RENAME_GRP */

int
nco_rename_grp(int grp_id,const char * const grp_nm)
{
  /* Purpose: Wrapper for nc_rename_grp() */
  const char fnc_nm[]="nco_rename_grp()";
  int rcd;
  rcd=nc_rename_grp(grp_id,grp_nm);
  if(rcd == NC_ENAMEINUSE){
    (void)fprintf(stdout,"ERROR: %s cannot define group name \"%s\" which is already in use\n",fnc_nm,grp_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_rename_grp()");
  return rcd;
} /* !nco_rename_grp() */

int nco_inq_grpname(const int nc_id,char * const grp_nm)
{
  /* Purpose: Wrapper for nc_inq_grpname() */
  int rcd;
  rcd=nc_inq_grpname(nc_id,grp_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grpname()");
  return rcd;
} /* !nco_inq_grpname() */

int nco_inq_grpname_full(const int nc_id,size_t * grp_nm_lng,char * const grp_nm_full)
{
  /* Purpose: Wrapper for nc_inq_grpname_full() */
  int rcd;
  rcd=nc_inq_grpname_full(nc_id,grp_nm_lng,grp_nm_full);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grpname_full()");
  return rcd;
} /* !nco_inq_grpname_full() */

int nco_inq_grpname_len(const int nc_id,size_t * const grp_nm_lng)
{
  /* Purpose: Wrapper for nc_inq_grpname_len() */
  int rcd;
  rcd=nc_inq_grpname_len(nc_id,grp_nm_lng);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grpname_len()");
  return rcd;
} /* !nco_inq_grpname_len() */

int nco_inq_grps(const int nc_id,int * const grp_nbr,int * const grp_ids)
{
  /* Purpose: Wrapper for nc_inq_grps() */
  int rcd;
  rcd=nc_inq_grps(nc_id,grp_nbr,grp_ids);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grps()");
  return rcd;
} /* !nco_inq_grps() */

int nco_inq_grp_ncid(const int nc_id,const char * const grp_nm,int * const grp_id)
{
  /* Purpose: Wrapper for nc_inq_grp_ncid() */
  int rcd;
  rcd=nc_inq_grp_ncid(nc_id,grp_nm,grp_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grp_ncid()");
  return rcd;
} /* !nco_inq_grp_ncid() */

int nco_inq_grp_ncid_flg(const int nc_id,const char * const grp_nm,int * const grp_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_grp_ncid(). Tolerates NC_ENOGRP. */
  int rcd;
  rcd=nc_inq_grp_ncid(nc_id,grp_nm,grp_id);
  if(rcd == NC_ENOGRP) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grp_ncid()");
  return rcd;
} /* !nco_inq_grp_ncid() */

int nco_inq_grp_full_ncid(const int nc_id,const char * const grp_nm_fll,int * const grp_id)
{
  /* Purpose: Wrapper for nc_inq_grp_full_ncid() */
  int fl_fmt;
  int rcd;
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_inq_grp_full_ncid(nc_id,grp_nm_fll,grp_id);
  }else{ /* netCDF3 */
    *grp_id=nc_id;
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grp_full_ncid()");
  return rcd;
} /* !nco_inq_grp_full_ncid() */

int nco_inq_grp_full_ncid_flg(const int nc_id,const char * const grp_nm_fll,int * const grp_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_grp_full_ncid(). Tolerates NC_ENOGRP. */
  int fl_fmt;
  int rcd;
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_inq_grp_full_ncid(nc_id,grp_nm_fll,grp_id);
  }else{ /* netCDF3 */
    *grp_id=nc_id;
  } /* endif */
  if(rcd == NC_ENOGRP) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grp_full_ncid()");
  return rcd;
} /* !nco_inq_grp_full_ncid_flg() */

int nco_inq_grp_parent(const int nc_id,int * const prn_id)
{
  /* Purpose: Wrapper for nc_inq_grp_parent() */
  int rcd;
  rcd=nc_inq_grp_parent(nc_id,prn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grp_parent()");
  return rcd;
} /* !nco_inq_grp_parent() */

int nco_inq_grp_parent_flg(const int nc_id,int * const prn_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_grp_parent_flg(). Tolerates NC_ENOGRP. */
  int rcd;
  rcd=nc_inq_grp_parent(nc_id,prn_id);
  if(rcd == NC_ENOGRP) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_grp_parent_flg()");
  return rcd;
} /* !nco_inq_grp_parent_flg() */
#endif /* !HAVE_NETCDF4_H */
/* End Group routines */

/* Begin Dimension routines (_dim) */
int
nco_def_dim(const int nc_id,const char * const dmn_nm,const long dmn_sz,int * const dmn_id)
{
  /* Purpose: Wrapper for nc_def_dim() */
  const char fnc_nm[]="nco_def_dim()";
  int rcd;
  rcd=nc_def_dim(nc_id,dmn_nm,(size_t)dmn_sz,dmn_id);
  if(rcd == NC_ENAMEINUSE){
    (void)fprintf(stdout,"ERROR: %s cannot define dimension \"%s\" because that name is already in use\n",fnc_nm,dmn_nm);
  } /* !rcd */
  if(rcd == NC_EDIMSIZE){
    (void)fprintf(stdout,"ERROR: %s cannot define dimension \"%s\" with illegal size = %ldL\n",fnc_nm,dmn_nm,dmn_sz);
  } /* !rcd */
  if(rcd == NC_EBADNAME){
    char *nm_nc=NULL; /* [sng] netCDF-compatible name */
    (void)fprintf(stdout,"INFO: %s reports input file dimension name \"%s\" contains illegal characters. ",fnc_nm,dmn_nm);
    nm_nc=nm2sng_nc(dmn_nm);
    rcd=nc_def_dim(nc_id,nm_nc,(size_t)dmn_sz,dmn_id);
    /* Did sanitized name pass syntax checker? */
    if(rcd == NC_NOERR){
      (void)fprintf(stdout,"Defined dimension in output file with netCDF-safe name \"%s\" instead.\n",nm_nc);
    }else if(rcd == NC_EBADNAME){
      (void)fprintf(stdout,"Presumptively netCDF-safe name (created by nm2sng_nc()) \"%s\" also contains illegal characters. Exiting.",nm_nc);
      nco_err_exit(rcd,fnc_nm);
    }else if(rcd == NC_ENAMEINUSE){
      rcd=nc_inq_dimid(nc_id,nm_nc,dmn_id);
      (void)fprintf(stdout," Will return dimension ID = %d of existing netCDF-safe dimension name \"%s\".\n",*dmn_id,nm_nc);
    } /* endif */
    if(nm_nc) free(nm_nc);
    assert(rcd == NC_NOERR || rcd == NC_EBADNAME || rcd == NC_ENAMEINUSE);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_def_dim */

int
nco_inq_dimid(const int nc_id,const char * const dmn_nm,int * const dmn_id)
{
  /* Purpose: Wrapper for nc_inq_dimid() */
  const char fnc_nm[]="nco_inq_dimid()";
  int rcd;
  rcd=nc_inq_dimid(nc_id,dmn_nm,dmn_id);
  if(rcd == NC_EBADDIM){
    (void)fprintf(stdout,"ERROR: %s reports requested dimension \"%s\" is not in input file\n",fnc_nm,dmn_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_dimid */

int
nco_inq_dimid_flg(const int nc_id,const char * const dmn_nm,int * const dmn_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_dimid(). Tolerates NC_EBADDIM. */
  int rcd;
  rcd=nc_inq_dimid(nc_id,dmn_nm,dmn_id);
  if(rcd == NC_EBADDIM) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimid_flg()");
  return rcd;
} /* !nco_inq_dimid */

int
nco_inq_dim(const int nc_id,const int dmn_id,char *dmn_nm,long *dmn_sz)
{
  /* Purpose: Wrapper for nc_inq_dim() */
  const char fnc_nm[]="nco_inq_dim()";
  int rcd;
  size_t dmn_sz_t; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  if(dmn_sz) dmn_sz_t=*dmn_sz;
  rcd=nc_inq_dim(nc_id,dmn_id,dmn_nm,&dmn_sz_t);
  if(dmn_sz) *dmn_sz=(long)dmn_sz_t;
  if(rcd == NC_EBADDIM){
    (void)fprintf(stdout,"ERROR: %s reports requested dimension \"%s\" is not in input file\n",fnc_nm,dmn_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dim()");
  return rcd;
}/* !nco_inq_dim */

int
nco_inq_dim_flg(const int nc_id,const int dmn_id,char *dmn_nm,long *dmn_sz)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_dim_flg(). Tolerates NC_EBADDIM. */
  int rcd;
  size_t dmn_sz_t; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  if(dmn_sz) dmn_sz_t=*dmn_sz;
  rcd=nc_inq_dim(nc_id,dmn_id,dmn_nm,&dmn_sz_t);
  if(dmn_sz) *dmn_sz=(long)dmn_sz_t;
  if(rcd == NC_EBADDIM) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dim_flg()");
  return rcd;
}/* !nco_inq_dim */

int
nco_inq_dimname(const int nc_id,const int dmn_id,char *dmn_nm)
{
  /* Purpose: Wrapper for nc_inq_dimname() */
  int rcd;
  rcd=nc_inq_dimname(nc_id,dmn_id,dmn_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimname()");
  return rcd;
} /* !nco_inq_dimname */

int
nco_inq_dimlen(const int nc_id,const int dmn_id,long *dmn_sz)
{
  /* Purpose: Wrapper for nc_inq_dimlen() */
  int rcd;
  size_t dmn_sz_t; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  if(dmn_sz) dmn_sz_t=*dmn_sz;
  rcd=nc_inq_dimlen(nc_id,dmn_id,&dmn_sz_t);
  if(dmn_sz) *dmn_sz=(long)dmn_sz_t;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimlen()");
  return rcd;
} /* !nco_inq_dimlen */

int
nco_rename_dim(const int nc_id,const int dmn_id,const char * const dmn_nm)
{
  /* Purpose: Wrapper for nc_rename_dim() */
  const char fnc_nm[]="nco_rename_dim()";
  int rcd;
  rcd=nc_rename_dim(nc_id,dmn_id,dmn_nm);
  if(rcd == NC_ENAMEINUSE){
    (void)fprintf(stdout,"ERROR: %s cannot define dimension name \"%s\" which is already in use\n",fnc_nm,dmn_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_rename_dim()");
  return rcd;
}  /* !nco_inq_rename_dim */

#ifdef HAVE_NETCDF4_H
int nco_inq_dimids(const int nc_id,int * const dmn_nbr,int * const dmn_ids,int flg_prn)
{
  /* Purpose: Wrapper for nc_inq_dimids() */
  int rcd;
  rcd=nc_inq_dimids(nc_id,dmn_nbr,dmn_ids,flg_prn);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimids()");
  return rcd;
} /* !nco_inq_dimids() */

int
nco_inq_unlimdims(const int nc_id,int *nbr_dmn_ult,int *dmn_ids_ult)
{
  /* Purpose: Wrapper for nc_inq_unlimdims() */
  int rcd;
  rcd=nc_inq_unlimdims(nc_id,nbr_dmn_ult,dmn_ids_ult);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nc_inq_unlimdims()");
  return rcd;
} /* !nco_inq_unlimdims() */
#endif /* !HAVE_NETCDF4_H */
/* End Dimension routines */

/* Begin Type routines */
#ifdef HAVE_NETCDF4_H
int
nco_inq_type(const int nc_id,const nc_type var_typ,char * const typ_nm,size_t * const typ_sz)
{
  /* Purpose: Wrapper for nc_inq_type() 
     NB: nc_inq_type() works on any type, including user-defined types */
  const char fnc_nm[]="nco_inq_type()";
  int rcd;
  rcd=nc_inq_type(nc_id,var_typ,typ_nm,typ_sz);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_inq_type() type %d\n",fnc_nm,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_type() */

int
nco_inq_typeids(const int nc_id,int * const typ_nbr,int * const typ_ids)
{
  /* Purpose: Wrapper for nc_inq_typeids() */
  const char fnc_nm[]="nco_inq_typeids()";
  int rcd;
  rcd=nc_inq_typeids(nc_id,typ_nbr,typ_ids);
  if(rcd != NC_NOERR){
    char grp_nm[NC_MAX_NAME+1L];
    (void)nco_inq_grpname(nc_id,grp_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_inq_typeids() in group %s\n",fnc_nm,grp_nm);
  } /* !rcd */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_typeids() */

int
nco_inq_user_type(const int nc_id,const nc_type var_typ,char * const typ_nm,size_t * const typ_sz,nc_type * const bs_typ,size_t * const fld_nbr,int * const cls_typ)
{
  /* Purpose: Wrapper for nc_inq_user_type()
     NB: nc_inq_user_type() returns NC_BADTYPE when var_typ < NC_MAX_ATOMIC_TYPE in 
     nc_inq_user_type(grp_id,var_typ,typ_nm,&typ_sz,&bs_typ,&fld_nbr,&cls_typ); */
  const char fnc_nm[]="nco_inq_user_type()";
  int rcd;
  rcd=nc_inq_user_type(nc_id,var_typ,typ_nm,typ_sz,bs_typ,fld_nbr,cls_typ);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_inq_user_type() type %d\n",fnc_nm,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_user_type() */
#endif /* !HAVE_NETCDF4_H */
/* End Type routines */

/* Begin user-defined data type routines (_enum, _vlen) */
/* Begin enum routines (_enum) */
#ifdef HAVE_NETCDF4_H
int
nco_def_enum(const int nc_id,const nc_type bs_typ,const char * const typ_nm,nc_type * const var_typ)
{
  /* Purpose: Wrapper for nc_def_enum() */
  const char fnc_nm[]="nco_def_enum()";
  int rcd;
  rcd=nc_def_enum(nc_id,bs_typ,typ_nm,var_typ);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_def_enum() type \"%s\"\n",fnc_nm,typ_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_def_enum() */

int
nco_insert_enum(const int nc_id,const nc_type var_typ,const char * const mbr_nm,const void * const vp)
{
  /* Purpose: Wrapper for nc_insert_enum() */
  const char fnc_nm[]="nco_insert_enum()";
  int rcd;
  rcd=nc_insert_enum(nc_id,var_typ,mbr_nm,vp);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_insert_enum() member \"%s\"\n",fnc_nm,mbr_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_insert_enum() */

int
nco_inq_enum(const int nc_id,const nc_type var_typ,char * const typ_nm,nc_type * const bs_typ,size_t * const typ_sz,size_t * const typ_nbr)
{
  /* Purpose: Wrapper for nc_inq_enum() */
  const char fnc_nm[]="nco_inq_enum()";
  int rcd;
  rcd=nc_inq_enum(nc_id,var_typ,typ_nm,bs_typ,typ_sz,typ_nbr);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_inq_enum() type %d\n",fnc_nm,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_enum() */

int
nco_inq_enum_member(const int nc_id,const nc_type var_typ,const int idx,char * const mbr_nm,void * const vp)
{
  /* Purpose: Wrapper for nc_inq_enum_member() */
  const char fnc_nm[]="nco_inq_enum_member()";
  int rcd;
  rcd=nc_inq_enum_member(nc_id,var_typ,idx,mbr_nm,vp);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_inq_enum_member() type %d\n",fnc_nm,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_enum_member() */

int
nco_inq_enum_ident(const int nc_id,const nc_type var_typ,const long long val,char * const mbr_nm)
{
  /* Purpose: Wrapper for nc_inq_enum_ident() */
  const char fnc_nm[]="nco_inq_enum_ident()";
  int rcd;
  rcd=nc_inq_enum_ident(nc_id,var_typ,val,mbr_nm);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_inq_enum_ident() type %d\n",fnc_nm,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_enum_ident() */

/* End enum routines */

/* Begin vlen routines (_vlen) */
int
nco_def_vlen(const int nc_id,const char * const typ_nm,const nc_type bs_typ,nc_type * const var_typ)
{
  /* Purpose: Wrapper for nc_def_vlen() */
  const char fnc_nm[]="nco_def_vlen()";
  int rcd;
  rcd=nc_def_vlen(nc_id,typ_nm,bs_typ,var_typ);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_def_vlen() type \"%s\"\n",fnc_nm,typ_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_def_vlen() */

int
nco_inq_vlen(const int nc_id,const nc_type var_typ,char * const typ_nm,size_t * const typ_sz,nc_type * const bs_typ)
{
  /* Purpose: Wrapper for nc_inq_vlen() */
  const char fnc_nm[]="nco_inq_vlen()";
  int rcd;
  rcd=nc_inq_vlen(nc_id,var_typ,typ_nm,typ_sz,bs_typ);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"ERROR: %s failed to nc_inq_vlen() type %d\n",fnc_nm,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_vlen() */

int
nco_free_vlen(nc_vlen_t * const vlenp)
{
  /* Purpose: Wrapper for nc_free_vlen() */
  const char fnc_nm[]="nco_free_vlen()";
  int rcd;
  rcd=nc_free_vlen(vlenp);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_free_vlen() */

int
nco_free_vlens(const size_t sz,nc_vlen_t * const vlenp)
{
  /* Purpose: Wrapper for nc_free_vlens() */
  const char fnc_nm[]="nco_free_vlens()";
  int rcd;
  rcd=nc_free_vlens(sz,vlenp);
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_free_vlens() */
/* End vlen routines */
#endif /* !HAVE_NETCDF4_H */
/* End user-defined data type routines (_enum, _vlen) */

/* Begin Variable routines (_var) */
int
nco_copy_var(const int nc_in_id,const int var_id,const int nc_out_id)
{
  /* Purpose: Wrapper for nc_copy_var() */
  int rcd;
  rcd=nc_copy_var(nc_in_id,var_id,nc_out_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_copy_var()");
  return rcd;
} /* !nco_copy_var */

int
nco_def_var(const int nc_id,const char * const var_nm,const nc_type var_typ,const int dmn_nbr,const int * const dmn_id,int * const var_id)
{
  /* Purpose: Wrapper for nc_def_var() */
  const char fnc_nm[]="nco_def_var()";
  int rcd;
  rcd=nc_def_var(nc_id,var_nm,var_typ,dmn_nbr,dmn_id,var_id);
  if(rcd == NC_EBADNAME){
    char att_nm[]="hdf_name"; /* [sng] Attribute to preserve original name */
    char *nm_nc=NULL; /* [sng] netCDF-compatible name */
    (void)fprintf(stdout,"INFO: %s reports input file variable name \"%s\" contains illegal characters. ",fnc_nm,var_nm);
    nm_nc=nm2sng_nc(var_nm);
    rcd=nc_def_var(nc_id,nm_nc,var_typ,dmn_nbr,dmn_id,var_id);
    /* Did sanitized name pass syntax checker? */
    if(rcd == NC_NOERR){
      (void)fprintf(stdout,"Defined variable in output file with netCDF-safe name \"%s\" instead. ",nm_nc);
    }else if(rcd == NC_EBADNAME){
      (void)fprintf(stdout,"Presumptively netCDF-safe name (created by nm2sng_nc()) \"%s\" also contains illegal characters. Exiting.",nm_nc);
      nco_err_exit(rcd,fnc_nm);
    } /* endif */
    assert(rcd == NC_NOERR || rcd == NC_EBADNAME);
    rcd=NCO_PUT_ATT_CHAR(nc_id,*var_id,att_nm,NC_CHAR,(size_t)strlen(var_nm),(const nco_char *)var_nm);
    if(nm_nc) free(nm_nc);
    if(rcd == NC_NOERR) (void)fprintf(stdout,"Original variable name is preserved in \"%s\" attribute.\n",att_nm);
  } /* endif */
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"ERROR: %s failed to nc_def_var() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */

  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_var()");
  return rcd;
} /* !nco_def_var() */

int nco_def_var_chunking
(const int nc_id, /* [ID] netCDF ID */
 const int var_id, /* [ID] Variable ID */
 const int srg_typ, /* [enm] Storage type */
 const size_t * const cnk_sz) /* [nbr] Chunk sizes */
{
  /* Purpose: Wrapper for nc_def_var_chunking() */
  int rcd;
  const char fnc_nm[]="nco_def_var_chunking()";

  /* NB: 20090713: netCDF4 API for nc_def_var_chunking() changed ~200906 
     Before that a weak netCDF4 prototype did not make cnk_sz const
     After I notified Unidata of this, they changed prototype to 
     const size_t * const cnk_sz
     This API change may cause confusion/differences when compiling
     NCO with netCDF4 versions pre- and post-200906, e.g., 
     netcdf-4.1-beta1-snapshot2009050200 has old behavior while
     netcdf-4.1-beta1-snapshot2009071200 has new behavior.
     Finding one-size-fits-all method is difficult! */
  /*  rcd=nc_def_var_chunking(nc_id,var_id,srg_typ,cnk_sz);*/

  rcd=nc_def_var_chunking(nc_id,var_id,srg_typ,(size_t *)cnk_sz);
  if(rcd == NC_EBADCHUNK){
    int idx;
    int dmn_nbr;
    nc_type var_typ;
    size_t sz;

    (void)nco_inq_varndims(nc_id,var_id,&dmn_nbr);
    (void)nco_inq_vartype(nc_id,var_id,&var_typ);

    sz=nco_typ_lng(var_typ);
    for(idx=0;idx<dmn_nbr;idx++){
      if(cnk_sz[idx] < 1L) (void)fprintf(stderr, "%s: ERROR Chunk sizes must exceed zero and requested chunk size cnk_sz[%d] = %ld.\n",fnc_nm,idx,cnk_sz[idx]);
      sz*=cnk_sz[idx];
    } /* !idx */

    if(sz > NCO_MAX_CHUNK_SIZE) (void)fprintf(stderr, "%s: ERROR Total requested chunk size = %lu exceeds netCDF maximium-supported chunk size = %u\n",fnc_nm,sz,NCO_MAX_CHUNK_SIZE);
  } /* !rcd */

  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);

  return rcd;
} /* !nco_def_var_chunking() */

int nco_def_var_deflate
(const int nc_id, /* [ID] netCDF ID */
 const int var_id, /* [ID] Variable ID */
 const int shuffle, /* [flg] Turn-on shuffle filter */
 const int deflate, /* [flg] Turn-on deflate filter */
 const int dfl_lvl) /* [enm] Deflate level [0..9] */
{
  /* Purpose: Wrapper for nc_def_var_deflate() */
  int rcd;
  int deflate_new;
  /* 20131204: Turn-off deflate filter when dfl_lvl == 0 */
  if(dfl_lvl == 0) deflate_new=0; else deflate_new=deflate;
  rcd=nc_def_var_deflate(nc_id,var_id,shuffle,deflate_new,dfl_lvl);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_var_deflate()");
  return rcd;
} /* !nco_def_var_deflate() */

int nco_def_var_filter(const int nc_id,const int var_id,const unsigned int flt_id,const size_t prm_nbr,const unsigned int * const prm_lst)
{
  /* Purpose: Wrapper for nc_def_var_filter() */
  int rcd;
  rcd=nc_def_var_filter(nc_id,var_id,flt_id,prm_nbr,prm_lst);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_var_filter()");
  return rcd;
} /* !nco_def_var_filter() */

int nco_def_var_filter_ccr(const int nc_id,const int var_id,const char * const flt_nm,const size_t prm_nbr,const char ** const prm_lst)
{
  /* Purpose: Wrapper for nc_def_var_filter_ccr() */
  int rcd;
  rcd=nc_def_var_filter_ccr(nc_id,var_id,flt_nm,prm_nbr,prm_lst);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_var_filter_ccr()");
  return rcd;
} /* !nco_def_var_filter_ccr() */

int
nco_inq_var(const int nc_id,const int var_id,char * const var_nm,nc_type *var_typ,int * const dmn_nbr,int * const dmn_id,int * const att_nbr)
{
  /* Purpose: Wrapper for nco_inq_var() */
  int rcd;
  rcd=nc_inq_var(nc_id,var_id,var_nm,var_typ,dmn_nbr,dmn_id,att_nbr);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var()");
  return rcd;
} /* !nco_inq_var */

int nco_inq_var_chunking
(const int nc_id, /* [ID] netCDF ID */
 const int var_id, /* [ID] Variable ID */
 int * const srg_typ, /* [enm] Storage type */
 size_t * const cnk_sz) /* [nbr] Chunk sizes */
{
  /* Purpose: Wrapper for nc_inq_var_chunking() */
  /* NB: netCDF chunking inquire function only works on netCDF4 files
     NCO wrapper works on netCDF3 and netCDF4 files */
  int rcd;
  int fl_fmt; /* [enm] Input file format */
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_inq_var_chunking(nc_id,var_id,srg_typ,cnk_sz);
  }else{ /* !netCDF4 */
    /* Defensive programming */
    *srg_typ=NC_CONTIGUOUS;
  } /* !netCDF4 */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_chunking()");
  return rcd;
} /* !nco_inq_var_chunking() */

int
nco_inq_var_deflate
(const int nc_id, /* I [ID] netCDF ID */
 const int var_id, /* I [ID] Variable ID */
 int * const shuffle, /* O [flg] Turn-on shuffle filter */
 int * const deflate, /* O [flg] Turn-on deflate filter */
 int * const dfl_lvl) /* O [enm] Deflate level [0..9] */
{
  /* Purpose: Wrapper for nc_inq_var_deflate() */
  /* NB: netCDF deflate inquire function only works on netCDF4 files
     NCO wrapper works on netCDF3 and netCDF4 files */
  int rcd;
  int fl_fmt; /* [enm] Input file format */
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_inq_var_deflate(nc_id,var_id,shuffle,deflate,dfl_lvl);
  }else{ /* !netCDF4 */
    if(shuffle) *shuffle=NC_NOSHUFFLE;
    if(deflate) *deflate=0; /* NB: netCDF uses naked integer rather than token, e.g., NC_NODEFLATE, for this */
    if(dfl_lvl) *dfl_lvl=0;
  } /* !netCDF4 */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_deflate()");
  return rcd;
} /* !nco_inq_var_deflate() */

int
nco_inq_var_endian
(const int nc_id, /* I [ID] netCDF ID */
 const int var_id, /* I [ID] Variable ID */
 int * const ndn_typ) /* O [enm] Endianness */
{
  /* Purpose: Wrapper for nc_inq_var_endian() */
  /* NB: netCDF endian inquire function only works on netCDF4 files
     NCO wrapper works on netCDF3 and netCDF4 files */
  int rcd;
  int fl_fmt; /* [enm] Input file format */
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_inq_var_endian(nc_id,var_id,ndn_typ);
  }else{ /* !netCDF4 */
    if(ndn_typ) *ndn_typ=NC_ENDIAN_NATIVE;
  } /* !netCDF4 */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_endian()");
  return rcd;
} /* !nco_inq_var_endian() */

int
nco_inq_var_fill
(const int nc_id, /* I [ID] netCDF ID */
 const int var_id, /* I [ID] Variable ID */
 int * const fll_nil, /* O [enm] NO_FILL */
 void * const fll_val) /* O [enm] Fill value */
{
  /* Purpose: Wrapper for nc_inq_var_fill() */
  /* NB: netCDF fill inquire function only works on netCDF4 files
     NCO wrapper works on netCDF3 and netCDF4 files */
  int rcd;
  int fl_fmt; /* [enm] Input file format */
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_inq_var_fill(nc_id,var_id,fll_nil,fll_val);
  }else{ /* !netCDF4 */
    if(fll_nil) *fll_nil=0;
    /* fxm: implement netCDF3-compatible function which returns real fill values based on variable type
       This could be based on nco_mss_val_get() */
    if(fll_val) assert(0);
    rcd=NC_NOERR;
  } /* !netCDF4 */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_fill()");
  return rcd;
} /* !nco_inq_var_fill() */

int nco_inq_var_filter(const int nc_id,const int var_id,unsigned int * const flt_id,size_t * const prm_nbr,unsigned int * const prm_lst)
{
  /* Purpose: Wrapper for nc_inq_var_filter() */
  const char fnc_nm[]="nco_inq_var_filter()";
  int rcd;
  rcd=nc_inq_var_filter(nc_id,var_id,flt_id,prm_nbr,prm_lst);
  if(rcd == NC_ENOFILTER){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s reports no filter defined for variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_filter()");
  return rcd;
} /* !nco_inq_var_filter() */

int nco_inq_var_filter_flg(const int nc_id,const int var_id,unsigned int * const flt_id,size_t * const prm_nbr,unsigned int * const prm_lst)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_var_filter(). Tolerates NC_ENOFILTER. */
  int rcd;
  rcd=nc_inq_var_filter(nc_id,var_id,flt_id,prm_nbr,prm_lst);
  if(rcd == NC_ENOFILTER) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_filter_flg()");
  return rcd;
} /* !nco_inq_var_filter_flg() */

int nco_inq_var_filter_ids(const int nc_id,const int var_id,size_t * const flt_nbr,unsigned int * const flt_lst)
{
  /* Purpose: Wrapper for nc_inq_var_filter_ids() */
  int rcd;
  rcd=nc_inq_var_filter_ids(nc_id,var_id,flt_nbr,flt_lst);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_filter_ids()");
  return rcd;
} /* !nco_inq_var_filter_ids() */

int nco_inq_var_filter_info(const int nc_id,const int var_id,const unsigned int flt_id,size_t * const prm_nbr,unsigned int * const prm_lst)
{
  /* Purpose: Wrapper for nc_inq_var_filter_info() */
  int rcd;
  rcd=nc_inq_var_filter_info(nc_id,var_id,flt_id,prm_nbr,prm_lst);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_filter_info()");
  return rcd;
} /* !nco_inq_var_filter_info() */

int
nco_def_var_fletcher32
(const int nc_id, /* I [ID] netCDF ID */
 const int var_id, /* I [ID] Variable ID */
 const int chk_typ) /* I [enm] Checksum type */
{
  /* Purpose: Wrapper for nc_def_var_fletcher32() */
  /* NB: netCDF fletcher32 set function only works on netCDF4 files
     NCO wrapper works on netCDF3 and netCDF4 files */
  int rcd=NC_NOERR;
  int fl_fmt; /* [enm] Input file format */
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_def_var_fletcher32(nc_id,var_id,chk_typ);
  } /* !netCDF4 */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_var_fletcher32()");
  return rcd;
} /* !nco_def_var_fletcher32() */

int
nco_inq_var_fletcher32
(const int nc_id, /* I [ID] netCDF ID */
 const int var_id, /* I [ID] Variable ID */
 int * const chk_typ) /* O [enm] Checksum type */
{
  /* Purpose: Wrapper for nc_inq_var_fletcher32() */
  /* NB: netCDF fletcher32 inquire function only works on netCDF4 files
     NCO wrapper works on netCDF3 and netCDF4 files */
  int rcd=NC_NOERR;
  int fl_fmt; /* [enm] Input file format */
  rcd=nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    rcd=nc_inq_var_fletcher32(nc_id,var_id,chk_typ);
  }else{ /* !netCDF4 */
    if(chk_typ) *chk_typ=NC_NOCHECKSUM;
  } /* !netCDF4 */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var_fletcher32()");
  return rcd;
} /* !nco_inq_var_fletcher32() */

int /* O [flg] Return success code */
nco_inq_var_packing /* [fnc] Check whether variable is packed on disk */
(const int nc_id, /* I [ID] netCDF ID */
 const int var_id, /* I [ID] Variable ID */
 int * const packing) /* O [flg] Variable is packed on disk */
{
  /* Purpose: Check whether variable is packed on disk
     Designed to behave like (non-existent) netCDF-library call for packing
     Based on nco_pck_dsk_inq()
     Difference is that:
     nco_pck_dsk_inq() fills in members of variable structure
     nco_inq_var_packing() does not need a variable structure, or anything from nco.h
     Hence, nco_inq_var_packing is suitable for light-overhead calls, such as by ncks for printing */ 
  
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  
  int rcd; /* [rcd] Return success code */
  int has_scl_fct; /* [flg] Valid scale_factor attribute exists */
  int has_add_fst; /* [flg] Valid add_offset attribute exists */
  
  long add_fst_lng; /* [idx] Number of elements in add_offset attribute */
  long scl_fct_lng; /* [idx] Number of elements in scale_factor attribute */

  nc_type add_fst_typ; /* [idx] Type of add_offset attribute */
  nc_type scl_fct_typ; /* [idx] Type of scale_factor attribute */

  /* Initialize and default to most-likely outcome, that variable is not packed */
  has_scl_fct=0; /* [flg] Valid scale_factor attribute exists */
  has_add_fst=0; /* [flg] Valid add_offset attribute exists */
  *packing=0; /* [flg] Variable is packed on disk */

  /* Vet scale_factor */
  rcd=nco_inq_att_flg(nc_id,var_id,scl_fct_sng,&scl_fct_typ,&scl_fct_lng);
  if(rcd != NC_ENOTATT){
    if(scl_fct_typ == NC_BYTE || scl_fct_typ == NC_CHAR) return NC_NOERR;
    if(scl_fct_lng != 1) return NC_NOERR;
    has_scl_fct=1; /* [flg] Valid scale_factor attribute exists */
  } /* endif */

  /* Vet add_offset */
  rcd=nco_inq_att_flg(nc_id,var_id,add_fst_sng,&add_fst_typ,&add_fst_lng);
  if(rcd != NC_ENOTATT){
    if(add_fst_typ == NC_BYTE || add_fst_typ == NC_CHAR) return NC_NOERR;
    if(add_fst_lng != 1) return NC_NOERR;
    has_add_fst=1; /* [flg] Valid add_offset attribute exists */
  } /* endif */

  if(has_scl_fct && has_add_fst)
    if(scl_fct_typ != add_fst_typ) 
      return NC_NOERR;

  /* Variable is considered packed iff either or both valid scale_factor or add_offset exist */
  if(has_scl_fct || has_add_fst) *packing=1; /* [flg] Variable is packed on disk */

  return NC_NOERR;
} /* !nco_inq_var_packing() */

int
nco_inq_varid(const int nc_id,const char * const var_nm,int * const var_id)
{
  /* Purpose: Wrapper for nc_inq_varid() */
  const char fnc_nm[]="nco_inq_varid()";
  int rcd;
  rcd=nc_inq_varid(nc_id,var_nm,var_id);
  if(rcd == NC_ENOTVAR){
    /* Variable of input name is not in file
       Check if variable with netCDF-sanitized version of input name exists */
    char *nm_nc=NULL; /* [sng] netCDF-compatible name */
    nm_nc=nm2sng_nc(var_nm);
    rcd=nc_inq_varid(nc_id,nm_nc,var_id);
    if(rcd == NC_NOERR) (void)fprintf(stdout,"INFO: %s reports requested variable \"%s\" is not defined in file, though variable with netCDF-safe name \"%s\" is. Returning safe-named variable ID = %d.\n",fnc_nm,var_nm,nm_nc,*var_id); else (void)fprintf(stdout,"ERROR: %s reports requested variable \"%s\" is not defined in file\n",fnc_nm,var_nm);
    if(nm_nc) free(nm_nc);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_varid */

int
nco_inq_varid_flg(const int nc_id,const char * const var_nm,int * const var_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_varid(). Tolerates NC_ENOTVAR. */
  int rcd;
  rcd=nc_inq_varid(nc_id,var_nm,var_id);
  if(rcd == NC_ENOTVAR) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varid_flg()");
  return rcd;
} /* !nco_inq_varid */

int
nco_inq_varname(const int nc_id,const int var_id,char * const var_nm)
{
  /* Purpose: Wrapper for nc_inq_varname() */
  const char fnc_nm[]="nco_inq_varname()";
  int rcd;
  rcd=nc_inq_varname(nc_id,var_id,var_nm);
  if(rcd == NC_ENOTVAR){
    (void)fprintf(stdout,"ERROR: %s reports specified dataset %d has no variable ID %d\n",fnc_nm,nc_id,var_id);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varname()");
  return rcd;
} /* !nco_inq_varname */

int
nco_inq_vartype(const int nc_id,const int var_id,nc_type * const var_typ)
{
  /* Purpose: Wrapper for nc_inq_vartype() */
  int rcd;
  rcd=nc_inq_vartype(nc_id,var_id,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_vartype()");
  return rcd;
} /* !nco_inq_vartype */

int
nco_inq_varndims(const int nc_id,const int var_id,int * const dmn_nbr)
{
  /* Purpose: Wrapper for nc_inq_varndims() */
  int rcd;
  rcd=nc_inq_varndims(nc_id,var_id,dmn_nbr);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varndims()");
  return rcd;
} /* !nco_inq_varndims */

int
nco_inq_vardimid(const int nc_id,const int var_id,int * const dmn_id)
{
  /* Purpose: Wrapper for nc_inq_vardimid() */
  int rcd;
  rcd=nc_inq_vardimid(nc_id,var_id,dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_vardimid()");
  return rcd;
} /* !nco_inq_vardimid */

int
nco_inq_varnatts(const int nc_id,const int var_id,int * const att_nbr)
{
  /* Purpose: Wrapper for nc_inq_varnatts() */
  const char fnc_nm[]="nco_inq_varnatts()";
  int rcd;
  rcd=nc_inq_varnatts(nc_id,var_id,att_nbr);
  if(rcd == NC_ENOTVAR){
    (void)fprintf(stdout,"ERROR: %s reports specified dataset %d has no variable ID %d\n",fnc_nm,nc_id,var_id);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varnatts()");
  return rcd;
} /* !nco_inq_varnatts */

int
nco_rename_var(const int nc_id,const int var_id,const char * const var_nm)
{
  /* Purpose: Wrapper for nc_rename_var() */
  const char fnc_nm[]="nco_rename_var()";
  int rcd;
  rcd=nc_rename_var(nc_id,var_id,var_nm);
  if(rcd == NC_ENAMEINUSE){
    (void)fprintf(stdout,"ERROR: %s cannot define variable name \"%s\" which is already in use\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_rename_var()");
  return rcd;
} /* !nco_rename_var */

#ifdef HAVE_NETCDF4_H
int nco_inq_varids(const int nc_id,int * const var_nbr,int * const var_ids)
{
  /* Purpose: Wrapper for nc_inq_varids() */
  int rcd;
  rcd=nc_inq_varids(nc_id,var_nbr,var_ids);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varids()");
  return rcd;
} /* !nco_inq_varids() */
#endif /* !HAVE_NETCDF4_H */
/* End _var */

/* Start _get _put _var */
int
nco_get_var(const int nc_id,const int var_id,void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_get_var_*() */
  const char fnc_nm[]="nco_get_var()";
  int rcd=NC_NOERR;
  if(var_typ <= NC_MAX_ATOMIC_TYPE){
    switch(var_typ){
    case NC_FLOAT: rcd=nc_get_var_float(nc_id,var_id,(float *)vp); break;
    case NC_DOUBLE: rcd=nc_get_var_double(nc_id,var_id,(double *)vp); break;
    case NC_INT: rcd=NCO_GET_VAR_INT(nc_id,var_id,(nco_int *)vp); break;
    case NC_SHORT: rcd=nc_get_var_short(nc_id,var_id,(nco_short *)vp); break;
    case NC_CHAR: rcd=NCO_GET_VAR_CHAR(nc_id,var_id,(nco_char *)vp); break;
    case NC_BYTE: rcd=NCO_GET_VAR_BYTE(nc_id,var_id,(nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
    case NC_UBYTE: rcd=NCO_GET_VAR_UBYTE(nc_id,var_id,(nco_ubyte *)vp); break;
    case NC_USHORT: rcd=NCO_GET_VAR_USHORT(nc_id,var_id,(nco_ushort *)vp); break;
    case NC_UINT: rcd=NCO_GET_VAR_UINT(nc_id,var_id,(nco_uint *)vp); break;
    case NC_INT64: rcd=NCO_GET_VAR_INT64(nc_id,var_id,(nco_int64 *)vp); break;
    case NC_UINT64: rcd=NCO_GET_VAR_UINT64(nc_id,var_id,(nco_uint64 *)vp); break;
    case NC_STRING: rcd=NCO_GET_VAR_STRING(nc_id,var_id,(nco_string *)vp); break;
      /* Allow calling routine to pass var_typ as either variable type or class-type */
    case NC_VLEN:
    case NC_OPAQUE:
    case NC_ENUM:
    case NC_COMPOUND:
      rcd=nc_get_var(nc_id,var_id,(void *)vp); break;
#endif /* !ENABLE_NETCDF4 */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    rcd=nc_get_var(nc_id,var_id,(void *)vp);
  } /* !udt */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_get_var() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_get_var */

int
nco_put_var(const int nc_id,const int var_id,const void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_put_var_*()
     20160228: nc_put_var() family does type conversion, nc_put_vara() family does not */
  const char fnc_nm[]="nco_put_var()";
  int rcd=NC_NOERR;
  switch(var_typ){
  case NC_FLOAT: rcd=nc_put_var_float(nc_id,var_id,(const float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_var_double(nc_id,var_id,(const double *)vp); break;
  case NC_INT: rcd=NCO_PUT_VAR_INT(nc_id,var_id,(const nco_int *)vp); break;
  case NC_SHORT: rcd=nc_put_var_short(nc_id,var_id,(const short *)vp); break;
  case NC_CHAR: rcd=NCO_PUT_VAR_CHAR(nc_id,var_id,(const nco_char *)vp); break;
  case NC_BYTE: rcd=NCO_PUT_VAR_BYTE(nc_id,var_id,(const nco_byte *)vp); break;
# ifdef ENABLE_NETCDF4
  case NC_UBYTE: rcd=NCO_PUT_VAR_UBYTE(nc_id,var_id,(const nco_ubyte *)vp); break;
  case NC_USHORT: rcd=NCO_PUT_VAR_USHORT(nc_id,var_id,(const nco_ushort *)vp); break;
  case NC_UINT: rcd=NCO_PUT_VAR_UINT(nc_id,var_id,(const nco_uint *)vp); break;
  case NC_INT64: rcd=NCO_PUT_VAR_INT64(nc_id,var_id,(const nco_int64 *)vp); break;
  case NC_UINT64: rcd=NCO_PUT_VAR_UINT64(nc_id,var_id,(const nco_uint64 *)vp); break;
    /* NC_STRING prototype next causes same compiler warnings described in nco_put_var1() above */
  case NC_STRING: rcd=NCO_PUT_VAR_STRING(nc_id,var_id,(const char **)vp); break;
# endif /* !ENABLE_NETCDF4 */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_put_var() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_var()");
  return rcd;
} /* !nco_put_var */

int
nco_get_var1(const int nc_id,const int var_id,const long * const srt,void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_get_var1_*() */
  const char fnc_nm[]="nco_get_var1()";
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  if(srt){
    for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) srt_sz_t[dmn_idx]=srt[dmn_idx];
  }else{
    for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) srt_sz_t[dmn_idx]=0L;
  } /* !srt */
  if(var_typ <= NC_MAX_ATOMIC_TYPE){
    switch(var_typ){
    case NC_FLOAT: rcd=nc_get_var1_float(nc_id,var_id,srt_sz_t,(float *)vp); break;
    case NC_DOUBLE: rcd=nc_get_var1_double(nc_id,var_id,srt_sz_t,(double *)vp); break;
    case NC_INT: rcd=NCO_GET_VAR1_INT(nc_id,var_id,srt_sz_t,(nco_int *)vp); break;
    case NC_SHORT: rcd=nc_get_var1_short(nc_id,var_id,srt_sz_t,(nco_short *)vp); break;
    case NC_CHAR: rcd=NCO_GET_VAR1_CHAR(nc_id,var_id,srt_sz_t,(nco_char *)vp); break;
    case NC_BYTE: rcd=NCO_GET_VAR1_BYTE(nc_id,var_id,srt_sz_t,(nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
    case NC_UBYTE: rcd=NCO_GET_VAR1_UBYTE(nc_id,var_id,srt_sz_t,(nco_ubyte *)vp); break;
    case NC_USHORT: rcd=NCO_GET_VAR1_USHORT(nc_id,var_id,srt_sz_t,(nco_ushort *)vp); break;
    case NC_UINT: rcd=NCO_GET_VAR1_UINT(nc_id,var_id,srt_sz_t,(nco_uint *)vp); break;
    case NC_INT64: rcd=NCO_GET_VAR1_INT64(nc_id,var_id,srt_sz_t,(nco_int64 *)vp); break;
    case NC_UINT64: rcd=NCO_GET_VAR1_UINT64(nc_id,var_id,srt_sz_t,(nco_uint64 *)vp); break;
    case NC_STRING: rcd=NCO_GET_VAR1_STRING(nc_id,var_id,srt_sz_t,(nco_string *)vp); break;
      /* Allow calling routine to pass var_typ as either variable type or class-type */
    case NC_VLEN:
    case NC_OPAQUE:
    case NC_ENUM:
    case NC_COMPOUND:
      rcd=nc_get_var1(nc_id,var_id,srt_sz_t,(void *)vp); break;
#endif /* !ENABLE_NETCDF4 */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    rcd=nc_get_var1(nc_id,var_id,srt_sz_t,(void *)vp);
  } /* !udt */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_get_var1() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_get_var1 */

int
nco_put_var1(const int nc_id,const int var_id,const long * const srt,const void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_put_var1_*() */
  const char fnc_nm[]="nco_put_var1()";
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  if(srt){
    for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) srt_sz_t[dmn_idx]=srt[dmn_idx];
  }else{
    for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) srt_sz_t[dmn_idx]=0L;
  } /* !srt */
  switch(var_typ){
  case NC_FLOAT: rcd=nc_put_var1_float(nc_id,var_id,srt_sz_t,(const float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_var1_double(nc_id,var_id,srt_sz_t,(const double *)vp); break;
  case NC_INT: rcd=NCO_PUT_VAR1_INT(nc_id,var_id,srt_sz_t,(const nco_int *)vp); break;
  case NC_SHORT: rcd=nc_put_var1_short(nc_id,var_id,srt_sz_t,(const short *)vp); break;
  case NC_CHAR: rcd=NCO_PUT_VAR1_CHAR(nc_id,var_id,srt_sz_t,(const nco_char *)vp); break;
  case NC_BYTE: rcd=NCO_PUT_VAR1_BYTE(nc_id,var_id,srt_sz_t,(const nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
  case NC_UBYTE: rcd=NCO_PUT_VAR1_UBYTE(nc_id,var_id,srt_sz_t,(const nco_ubyte *)vp); break;
  case NC_USHORT: rcd=NCO_PUT_VAR1_USHORT(nc_id,var_id,srt_sz_t,(const nco_ushort *)vp); break;
  case NC_UINT: rcd=NCO_PUT_VAR1_UINT(nc_id,var_id,srt_sz_t,(const nco_uint *)vp); break;
  case NC_INT64: rcd=NCO_PUT_VAR1_INT64(nc_id,var_id,srt_sz_t,(const nco_int64 *)vp); break;
  case NC_UINT64: rcd=NCO_PUT_VAR1_UINT64(nc_id,var_id,srt_sz_t,(const nco_uint64 *)vp); break;
    /* Next line produces GCC warning:
       attention : passing argument 4 of ‘nc_put_var1_string’ from incompatible pointer type 
       I think this warning occurs because this routine receives input vp 
       as "const void * const vp", a singly indirect pointer, and then
       casts vp to a doubly indirect pointer on output, i.e., to something like
       "(const char **)vp" or "(const nco_string * const)vp".
       Is casting a singly-indirect pointer to a doubly-indirect ever allowed? */
    /* Of the following three options we must pick one
       None is completely satisfactory with gcc
       Choose solution that allows trouble-free g++ compilation */
    /* Next line produces gcc and g++ warning:
       attention : le transtypage annule des qualificateurs du type pointeur ciblé */
  case NC_STRING: rcd=NCO_PUT_VAR1_STRING(nc_id,var_id,srt_sz_t,(const char **)vp); break;
      /* Next line produces g++ warning:
       erreur: invalid conversion from ‘char* const* const’ to ‘const char** */
    /* case NC_STRING: rcd=NCO_PUT_VAR1_STRING(nc_id,var_id,srt_sz_t,(const nco_string * const)vp); break;*/
    /* Next line produces g++ warning:
        erreur: invalid conversion from ‘char* const*’ to ‘const char** */
    /*  case NC_STRING: rcd=NCO_PUT_VAR1_STRING(nc_id,var_id,srt_sz_t,(const nco_string *)vp); break;*/
#endif /* !ENABLE_NETCDF4 */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_put_var1() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_put_var1 */

int
nco_get_vara(const int nc_id,const int var_id,const long * const srt,const long * const cnt,void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_get_vara_*() */
  const char fnc_nm[]="nco_get_vara()";
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  size_t cnt_sz_t[NC_MAX_VAR_DIMS];
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    cnt_sz_t[dmn_idx]=cnt[dmn_idx];
    srt_sz_t[dmn_idx]=srt[dmn_idx];
  } /* !dmn_idx */
  if(var_typ <= NC_MAX_ATOMIC_TYPE){
    switch(var_typ){
    case NC_FLOAT: rcd=nc_get_vara_float(nc_id,var_id,srt_sz_t,cnt_sz_t,(float *)vp); break;
    case NC_DOUBLE: rcd=nc_get_vara_double(nc_id,var_id,srt_sz_t,cnt_sz_t,(double *)vp); break;
    case NC_INT: rcd=NCO_GET_VARA_INT(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_int *)vp); break;
    case NC_SHORT: rcd=nc_get_vara_short(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_short *)vp); break;
    case NC_CHAR: rcd=NCO_GET_VARA_CHAR(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_char *)vp); break;
    case NC_BYTE: rcd=NCO_GET_VARA_BYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
    case NC_UBYTE: rcd=NCO_GET_VARA_UBYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_ubyte *)vp); break;
    case NC_USHORT: rcd=NCO_GET_VARA_USHORT(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_ushort *)vp); break;
    case NC_UINT: rcd=NCO_GET_VARA_UINT(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_uint *)vp); break;
    case NC_INT64: rcd=NCO_GET_VARA_INT64(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_int64 *)vp); break;
    case NC_UINT64: rcd=NCO_GET_VARA_UINT64(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_uint64 *)vp); break;
    case NC_STRING: rcd=NCO_GET_VARA_STRING(nc_id,var_id,srt_sz_t,cnt_sz_t,(nco_string *)vp); break;
      /* Allow calling routine to pass var_typ as either variable type or class-type */
    case NC_VLEN:
    case NC_OPAQUE:
    case NC_ENUM:
    case NC_COMPOUND:
      rcd=nc_get_vara(nc_id,var_id,srt_sz_t,cnt_sz_t,(void *)vp); break;
#endif /* !ENABLE_NETCDF4 */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    rcd=nc_get_vara(nc_id,var_id,srt_sz_t,cnt_sz_t,(void *)vp);
  } /* !udt */
#if 0
  if(var_typ <= NC_MAX_ATOMIC_TYPE){
    switch(var_typ){
    case NC_FLOAT: rcd=nc_get_vara_float(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(float *)vp); break;
    case NC_DOUBLE: rcd=nc_get_vara_double(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(double *)vp); break;
    case NC_INT: rcd=NCO_GET_VARA_INT(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_int *)vp); break;
    case NC_SHORT: rcd=nc_get_vara_short(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_short *)vp); break;
    case NC_CHAR: rcd=NCO_GET_VARA_CHAR(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_char *)vp); break;
    case NC_BYTE: rcd=NCO_GET_VARA_BYTE(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
    case NC_UBYTE: rcd=NCO_GET_VARA_UBYTE(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_ubyte *)vp); break;
    case NC_USHORT: rcd=NCO_GET_VARA_USHORT(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_ushort *)vp); break;
    case NC_UINT: rcd=NCO_GET_VARA_UINT(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_uint *)vp); break;
    case NC_INT64: rcd=NCO_GET_VARA_INT64(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_int64 *)vp); break;
    case NC_UINT64: rcd=NCO_GET_VARA_UINT64(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_uint64 *)vp); break;
    case NC_STRING: rcd=NCO_GET_VARA_STRING(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(nco_string *)vp); break;
      /* Allow calling routine to pass var_typ as either variable type or class-type */
    case NC_VLEN:
    case NC_OPAQUE:
    case NC_ENUM:
    case NC_COMPOUND:
      rcd=nc_get_vara(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(void *)vp); break;
#endif /* !ENABLE_NETCDF4 */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    rcd=nc_get_vara(nc_id,var_id,(const size_t *)srt,(const size_t *)cnt,(void *)vp);
  } /* !udt */
#endif /* !0 */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_get_vara() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_get_vara */

int
nco_put_vara(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_put_vara_*() */
  const char fnc_nm[]="nco_put_vara()";
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  size_t cnt_sz_t[NC_MAX_VAR_DIMS];
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    cnt_sz_t[dmn_idx]=cnt[dmn_idx];
    srt_sz_t[dmn_idx]=srt[dmn_idx];
  } /* !dmn_idx */
  switch(var_typ){
  case NC_FLOAT: rcd=nc_put_vara_float(nc_id,var_id,srt_sz_t,cnt_sz_t,(const float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_vara_double(nc_id,var_id,srt_sz_t,cnt_sz_t,(const double *)vp); break;
  case NC_INT: rcd=NCO_PUT_VARA_INT(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_int *)vp); break;
  case NC_SHORT: rcd=nc_put_vara_short(nc_id,var_id,srt_sz_t,cnt_sz_t,(const short *)vp); break;
  case NC_CHAR: rcd=NCO_PUT_VARA_CHAR(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_char *)vp); break;
  case NC_BYTE: rcd=NCO_PUT_VARA_BYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
  case NC_UBYTE: rcd=NCO_PUT_VARA_UBYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_ubyte *)vp); break;
  case NC_USHORT: rcd=NCO_PUT_VARA_USHORT(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_ushort *)vp); break;
  case NC_UINT: rcd=NCO_PUT_VARA_UINT(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_uint *)vp); break;
  case NC_INT64: rcd=NCO_PUT_VARA_INT64(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_int64 *)vp); break;
  case NC_UINT64: rcd=NCO_PUT_VARA_UINT64(nc_id,var_id,srt_sz_t,cnt_sz_t,(const nco_uint64 *)vp); break;
    /* NC_STRING prototype next causes same compiler warnings described in nco_put_var1() above */
  case NC_STRING: rcd=NCO_PUT_VARA_STRING(nc_id,var_id,srt_sz_t,cnt_sz_t,(const char **)vp); break;
#endif /* !ENABLE_NETCDF4 */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_put_vara() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_vara()");
  return rcd;
} /* !nco_put_vara */

int
nco_get_vars(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_get_vars_*() */
  const char fnc_nm[]="nco_get_vars()";
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  ptrdiff_t srd_pd_t[NC_MAX_VAR_DIMS];
  size_t cnt_sz_t[NC_MAX_VAR_DIMS];
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    cnt_sz_t[dmn_idx]=cnt[dmn_idx];
    srd_pd_t[dmn_idx]=srd[dmn_idx];
    srt_sz_t[dmn_idx]=srt[dmn_idx];
  } /* !dmn_idx */
  if(var_typ <= NC_MAX_ATOMIC_TYPE){
    switch(var_typ){
    case NC_FLOAT: rcd=nc_get_vars_float(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(float *)vp); break;
    case NC_DOUBLE: rcd=nc_get_vars_double(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(double *)vp); break;
    case NC_INT: rcd=NCO_GET_VARS_INT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_int *)vp); break;
    case NC_SHORT: rcd=nc_get_vars_short(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_short *)vp); break;
    case NC_CHAR: rcd=NCO_GET_VARS_CHAR(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_char *)vp); break;
    case NC_BYTE: rcd=NCO_GET_VARS_BYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
    case NC_UBYTE: rcd=NCO_GET_VARS_UBYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_ubyte *)vp); break;
    case NC_USHORT: rcd=NCO_GET_VARS_USHORT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_ushort *)vp); break;
    case NC_UINT: rcd=NCO_GET_VARS_UINT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_uint *)vp); break;
    case NC_INT64: rcd=NCO_GET_VARS_INT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_int64 *)vp); break;
    case NC_UINT64: rcd=NCO_GET_VARS_UINT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_uint64 *)vp); break;
    case NC_STRING: rcd=NCO_GET_VARS_STRING(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(nco_string *)vp); break;
      /* Allow calling routine to pass var_typ as either variable type or class-type */
    case NC_VLEN:
    case NC_OPAQUE:
    case NC_ENUM:
    case NC_COMPOUND:
      rcd=nc_get_vars(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(void *)vp); break;
#endif /* !ENABLE_NETCDF4 */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    rcd=nc_get_vars(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(void *)vp);
  } /* !udt */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_get_vars() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_get_vars */

int
nco_put_vars(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,const void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_put_vars_*() */
  const char fnc_nm[]="nco_put_vars()";
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  ptrdiff_t srd_pd_t[NC_MAX_VAR_DIMS];
  size_t cnt_sz_t[NC_MAX_VAR_DIMS];
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    cnt_sz_t[dmn_idx]=cnt[dmn_idx];
    srd_pd_t[dmn_idx]=srd[dmn_idx];
    srt_sz_t[dmn_idx]=srt[dmn_idx];
  } /* !dmn_idx */
  switch(var_typ){
  case NC_FLOAT: rcd=nc_put_vars_float(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t, (const float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_vars_double(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(const double *)vp); break;
  case NC_INT: rcd=NCO_PUT_VARS_INT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(const nco_int *)vp); break;
  case NC_SHORT: rcd=nc_put_vars_short(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t, (const short *)vp); break;
  case NC_CHAR: rcd=NCO_PUT_VARS_CHAR(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(const nco_char *)vp); break;
  case NC_BYTE: rcd=NCO_PUT_VARS_BYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t, (const nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
  case NC_UBYTE: rcd=NCO_PUT_VARS_UBYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(const nco_ubyte *)vp); break;
  case NC_USHORT: rcd=NCO_PUT_VARS_USHORT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t, (const nco_ushort *)vp); break;
  case NC_UINT: rcd=NCO_PUT_VARS_UINT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t, (const nco_uint *)vp); break;
  case NC_INT64: rcd=NCO_PUT_VARS_INT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t, (const nco_int64 *)vp); break;
  case NC_UINT64: rcd=NCO_PUT_VARS_UINT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(const nco_uint64 *)vp); break;
    /* NC_STRING prototype next causes same compiler warnings described in nco_put_var1() above */
  case NC_STRING: rcd=NCO_PUT_VARS_STRING(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,(const char **)vp); break;
#endif /* !ENABLE_NETCDF4 */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stdout,"ERROR: %s failed to nc_put_vars() variable \"%s\"\n",fnc_nm,var_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_put_vars */

int
nco_get_varm(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,const long * const map,void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_get_varm_*() */
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  ptrdiff_t map_pd_t[NC_MAX_VAR_DIMS];
  ptrdiff_t srd_pd_t[NC_MAX_VAR_DIMS];
  size_t cnt_sz_t[NC_MAX_VAR_DIMS];
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    cnt_sz_t[dmn_idx]=cnt[dmn_idx];
    map_pd_t[dmn_idx]=map[dmn_idx];
    srd_pd_t[dmn_idx]=srd[dmn_idx];
    srt_sz_t[dmn_idx]=srt[dmn_idx];
  } /* !dmn_idx */
  if(var_typ <= NC_MAX_ATOMIC_TYPE){
    switch(var_typ){
    case NC_FLOAT: rcd=nc_get_varm_float(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(float *)vp); break;
    case NC_DOUBLE: rcd=nc_get_varm_double(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(double *)vp); break;
    case NC_INT: rcd=NCO_GET_VARM_INT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_int *)vp); break;
    case NC_SHORT: rcd=nc_get_varm_short(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_short *)vp); break;
    case NC_CHAR: rcd=NCO_GET_VARM_CHAR(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_char *)vp); break;
    case NC_BYTE: rcd=NCO_GET_VARM_BYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
    case NC_UBYTE: rcd=NCO_GET_VARM_UBYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_ubyte *)vp); break;
    case NC_USHORT: rcd=NCO_GET_VARM_USHORT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_ushort *)vp); break;
    case NC_UINT: rcd=NCO_GET_VARM_UINT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_uint *)vp); break;
    case NC_INT64: rcd=NCO_GET_VARM_INT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_int64 *)vp); break;
    case NC_UINT64: rcd=NCO_GET_VARM_UINT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_uint64 *)vp); break;
    case NC_STRING: rcd=NCO_GET_VARM_STRING(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(nco_string *)vp); break;
      /* Allow calling routine to pass var_typ as either variable type or class-type */
    case NC_VLEN:
    case NC_OPAQUE:
    case NC_ENUM:
    case NC_COMPOUND:
      rcd=nc_get_varm(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(void *)vp); break;
#endif /* !ENABLE_NETCDF4 */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    rcd=nc_get_varm(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(void *)vp);
  } /* !udt */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_varm()");
  return rcd;
} /* !nco_get_varm */

int
nco_put_varm(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,const long * const map,const void * const vp,const nc_type var_typ)
{
  /* Purpose: Wrapper for nc_put_varm_*() */
  int rcd=NC_NOERR;
  int dmn_nbr; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  ptrdiff_t map_pd_t[NC_MAX_VAR_DIMS];
  ptrdiff_t srd_pd_t[NC_MAX_VAR_DIMS];
  size_t cnt_sz_t[NC_MAX_VAR_DIMS];
  size_t srt_sz_t[NC_MAX_VAR_DIMS];
  rcd=nc_inq_varndims(nc_id,var_id,&dmn_nbr);
  for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    cnt_sz_t[dmn_idx]=cnt[dmn_idx];
    map_pd_t[dmn_idx]=map[dmn_idx];
    srd_pd_t[dmn_idx]=srd[dmn_idx];
    srt_sz_t[dmn_idx]=srt[dmn_idx];
  } /* !dmn_idx */
  switch(var_typ){
  case NC_FLOAT: rcd=nc_put_varm_float(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_varm_double(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const double *)vp); break;
  case NC_INT: rcd=NCO_PUT_VARM_INT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_int *)vp); break;
  case NC_SHORT: rcd=nc_put_varm_short(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const short *)vp); break;
  case NC_CHAR: rcd=NCO_PUT_VARM_CHAR(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_char *)vp); break;
  case NC_BYTE: rcd=NCO_PUT_VARM_BYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
  case NC_UBYTE: rcd=NCO_PUT_VARM_UBYTE(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_ubyte *)vp); break;
  case NC_USHORT: rcd=NCO_PUT_VARM_USHORT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_ushort *)vp); break;
  case NC_UINT: rcd=NCO_PUT_VARM_UINT(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_uint *)vp); break;
  case NC_INT64: rcd=NCO_PUT_VARM_INT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_int64 *)vp); break;
  case NC_UINT64: rcd=NCO_PUT_VARM_UINT64(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const nco_uint64 *)vp); break;
    /* NC_STRING prototype next causes same compiler warnings described in nco_put_var1() above */
  case NC_STRING: rcd=NCO_PUT_VARM_STRING(nc_id,var_id,srt_sz_t,cnt_sz_t,srd_pd_t,map_pd_t,(const char **)vp); break;
#endif /* !ENABLE_NETCDF4 */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_varm()");
  return rcd;
} /* !nco_put_varm */
/* End Variable routines */

/* Begin Attribute routines (_att) */
int
nco_inq_att(const int nc_id,const int var_id,const char * const att_nm,nc_type * const att_typ,long * const att_sz)
{
  /* Purpose: Wrapper for nc_inq_att() */
  const char fnc_nm[]="nco_inq_att()";
  int rcd;
  size_t att_sz_t; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  if(att_sz) att_sz_t=*att_sz;
  rcd=nc_inq_att(nc_id,var_id,att_nm,att_typ,&att_sz_t);
  if(att_sz) *att_sz=(long)att_sz_t;
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"ERROR: %s unable to inquire attribute var_id: %d, att_nm: %s\n",fnc_nm,var_id,att_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_inq_att */

int
nco_inq_att_flg(const int nc_id,const int var_id,const char * const att_nm,nc_type * const att_typ,long * const att_sz)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_att(). Tolerates ENOTATT. */
  const char fnc_nm[]="nco_inq_att_flg()";
  int rcd;
  size_t att_sz_t; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  if(att_sz) att_sz_t=*att_sz;
  rcd=nc_inq_att(nc_id,var_id,att_nm,att_typ,&att_sz_t);
  if(att_sz) *att_sz=(long)att_sz_t;
  if(rcd == NC_ENOTATT) return rcd;
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"ERROR: %s unable to inquire attribute var_id: %d, att_nm: %s\n",fnc_nm,var_id,att_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_inq_att_flg */

int
nco_inq_attid(const int nc_id,const int var_id,const char * const att_nm,int * const att_id)
{
  /* Purpose: Wrapper for nc_inq_attid() */
  int rcd;
  rcd=nc_inq_attid(nc_id,var_id,att_nm,att_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_attid()");
  return rcd;
} /* !nco_inq_attid */

int
nco_inq_attid_flg(const int nc_id,const int var_id,const char * const att_nm,int * const att_id)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_attid(): Tolerates NC_ENOTATT. */
  const char fnc_nm[]="nco_inq_attid_flg()";
  int rcd;
  rcd=nc_inq_attid(nc_id,var_id,att_nm,att_id);
  if(rcd == NC_ENOTATT) return rcd;
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"ERROR: %s unable to inquire attribute var_id: %d, att_nm: %s\n",fnc_nm,var_id,att_nm);
    nco_err_exit(rcd,fnc_nm);
  } /* endif */
  return rcd;
} /* !nco_inq_attid_flg */

int
nco_inq_atttype(const int nc_id,const int var_id,const char * const att_nm,nc_type * const att_typ)
{
  /* Purpose: Wrapper for nc_inq_atttype() */
  int rcd;
  rcd=nc_inq_atttype(nc_id,var_id,att_nm,att_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_atttype()");
  return rcd;
} /* !nco_inq_atttype */

int
nco_inq_attlen(const int nc_id,const int var_id,const char * const att_nm,long * const att_sz)
{
  /* Purpose: Wrapper for nc_inq_attlen() */
  int rcd;
  size_t att_sz_t; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  if(att_sz) att_sz_t=*att_sz;
  rcd=nc_inq_attlen(nc_id,var_id,att_nm,&att_sz_t);
  if(att_sz) *att_sz=(long)att_sz_t;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_attlen()");
  return rcd;
} /* !nco_inq_attlen */

int
nco_inq_attlen_flg(const int nc_id,const int var_id,const char * const att_nm,long * const att_sz)
{
  /* Purpose: Error-tolerant wrapper for nc_inq_attlen(). Tolerates NC_ENOTATT. */
  const char fnc_nm[]="nco_inq_attlen_flg()";
  int rcd;
  size_t att_sz_t; /* 20171115: WIN64 workaround: sizeof(long) = 4 != 8 = sizeof(size_t) */
  if(att_sz) att_sz_t=*att_sz;
  rcd=nc_inq_attlen(nc_id,var_id,att_nm,&att_sz_t);
  if(att_sz) *att_sz=(long)att_sz_t;
  if(rcd == NC_ENOTATT) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nco_inq_attlen */

int
nco_inq_attname(const int nc_id,const int var_id,const int att_id,char * const att_nm)
{
  /* Purpose: Wrapper for nc_inq_attname() */
  int rcd;
  rcd=nc_inq_attname(nc_id,var_id,att_id,att_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_attname()");
  return rcd;
} /* !nco_inq_attname */

int
nco_copy_att(const int nc_id_in,const int var_id_in,const char * const att_nm,const int nc_id_out,const int var_id_out)
{
  /* Purpose: Wrapper for nc_copy_att() */
  const char fnc_nm[]="nco_copy_att()";
  int rcd;
  /* 20191212 Prevent NCO from attempting to copy internal properties 
     Related discussion in 
     https://sourceforge.net/p/nco/bugs/117
     https://github.com/Unidata/netcdf-c/issues/803 */
  if(!strcmp(att_nm,"_NCProperties")){
    (void)fprintf(stdout,"INFO: %s requested to copy attribute \"%s\". As of version 4.9.1 (December, 2019), NCO refuses to copy this attribute as its value is exclusively reserved for the netCDF library to manipulate internally. This attribute was likely exposed to NCO via an upstream violation of the netCDF API. The netCDF library should though does not enforce this proscription itself, at least through netCDF verstion 4.7.3. To reduce headaches, NCO will proceed as though this operation were never requested. This message is informational only and may be safely ignored.\n",fnc_nm,att_nm);
    rcd=NC_NOERR;
  }else{
    rcd=nc_copy_att(nc_id_in,var_id_in,att_nm,nc_id_out,var_id_out);
    if(rcd == NC_ENAMEINUSE){
      if(var_id_out > NC_GLOBAL){
	char var_nm_in[NC_MAX_NAME+1L];
	char var_nm_out[NC_MAX_NAME+1L];
	(void)nco_inq_varname(nc_id_in,var_id_in,var_nm_in);
	(void)nco_inq_varname(nc_id_out,var_id_out,var_nm_out);
	(void)fprintf(stdout,"ERROR: %s unable to copy attribute \"%s\" from \"%s\" to \"%s\" because the destination variable already has an attribute of that name\n",fnc_nm,att_nm,var_nm_in,var_nm_out);
      }else{
	char grp_nm_in[NC_MAX_NAME+1L];
	char grp_nm_out[NC_MAX_NAME+1L];
	(void)nco_inq_grpname(nc_id_in,grp_nm_in);
	(void)nco_inq_grpname(nc_id_out,grp_nm_out);
	(void)fprintf(stdout,"ERROR: %s unable to copy attribute \"%s\" from \"%s\" to \"%s\" because the destination group already has an attribute of that name\n",fnc_nm,att_nm,grp_nm_in,grp_nm_out);
      } /* endif */
    } /* !NC_ENAMEINUSE */
  } /* !_NCProperties */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_copy_att()");
  return rcd;
}  /* !nco_copy_att */

int
nco_rename_att(const int nc_id,const int var_id,const char * const att_nm,const char * const att_new_nm)
{
  /* Purpose: Wrapper for nc_rename_att() */
  const char fnc_nm[]="nco_rename_att()";
  int rcd;
  rcd=nc_rename_att(nc_id,var_id,att_nm,att_new_nm);
  if(rcd == NC_ENAMEINUSE){
    if(var_id > NC_GLOBAL){
      char var_nm[NC_MAX_NAME+1L];
      (void)nco_inq_varname(nc_id,var_id,var_nm);
      (void)fprintf(stdout,"ERROR: %s unable to rename variable \"%s\" attribute \"%s\" to \"%s\" because the variable already has an attribute of that name\n",fnc_nm,var_nm,att_nm,att_new_nm);
    }else{
      char grp_nm[NC_MAX_NAME+1L];
      (void)nco_inq_grpname(nc_id,grp_nm);
      (void)fprintf(stdout,"ERROR: %s unable to rename group \"%s\" attribute \"%s\" to \"%s\" because the group already has an attribute of that name\n",fnc_nm,grp_nm,att_nm,att_new_nm);
    } /* endif */
  } /* !NC_ENAMEINUSE */
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
}  /* !nco_rename_att */

int
nco_del_att(const int nc_id,const int var_id,const char * const att_nm)
{
  /* Purpose: Wrapper for nc_del_att() */
  int rcd;
  rcd=nc_del_att(nc_id,var_id,att_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_del_att()");
  return rcd;
} /* !nco_del_att */

int
nco_put_att(const int nc_id,const int var_id,const char * const att_nm,const nc_type att_typ,const long att_len,const void * const vp)
{
  /* Purpose: Wrapper for nc_put_att_*() */
  const char fnc_nm[]="nco_put_att()";
  int rcd=NC_NOERR;
  switch(att_typ){
  case NC_FLOAT: rcd=nc_put_att_float(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_att_double(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const double *)vp); break;
  case NC_INT: rcd=NCO_PUT_ATT_INT(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_int *)vp); break;
  case NC_SHORT: rcd=nc_put_att_short(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const short *)vp); break;
  case NC_CHAR: rcd=NCO_PUT_ATT_CHAR(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_char *)vp); break;
  case NC_BYTE: rcd=NCO_PUT_ATT_BYTE(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
    /* 20051119: netCDF4 library did not support these until alpha10, still does not support nco_put/get_att_ubyte() */
  case NC_UBYTE: rcd=NCO_PUT_ATT_UBYTE(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_ubyte *)vp); break;
  case NC_USHORT: rcd=NCO_PUT_ATT_USHORT(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_ushort *)vp); break;
  case NC_UINT: rcd=NCO_PUT_ATT_UINT(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_uint *)vp); break;
  case NC_INT64: rcd=NCO_PUT_ATT_INT64(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_int64 *)vp); break;
  case NC_UINT64: rcd=NCO_PUT_ATT_UINT64(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const nco_uint64 *)vp); break;
    /* NC_STRING prototype next causes same compiler warnings described in nco_put_var1() above */
  case NC_STRING: rcd=NCO_PUT_ATT_STRING(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(const char **)vp); break;
#endif /* !ENABLE_NETCDF4 */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  /* 20170811: netCDF 4.5.0-development enforced rule attempting to add _FillValue to root/group (NC_GLOBAL) returns NC_EGLOBAL */
  if(rcd == NC_EGLOBAL && !strcmp(att_nm,"_FillValue")){
    char grp_nm[NC_MAX_NAME+1L];
    (void)nco_inq_grpname(nc_id,grp_nm);
    (void)fprintf(stdout,"WARNING: %s received NC_EGLOBAL error writing attribute \"%s\" to metadata for group \"%s\". netCDF 4.5.0-development forbids writing the _FillValue attribute to global or group metadata, though earlier versions allow it. Proceeding normally without writing %s attribute...\n",fnc_nm,att_nm,grp_nm,att_nm);
    rcd=NC_NOERR;
  } /* !rcd */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_att()");
  return rcd;
} /* !nco_put_att */

int
nco_get_att(const int nc_id,const int var_id,const char * const att_nm,void * const vp,const nc_type att_typ)
{
  /* Purpose: Wrapper for nc_get_att_*() */
  const char fnc_nm[]="nco_get_att()";
  int rcd=NC_NOERR;
  if(att_typ <= NC_MAX_ATOMIC_TYPE){
    switch(att_typ){
    case NC_FLOAT: rcd=nc_get_att_float(nc_id,var_id,att_nm,(float *)vp); break;
    case NC_DOUBLE: rcd=nc_get_att_double(nc_id,var_id,att_nm,(double *)vp); break;
    case NC_INT: rcd=NCO_GET_ATT_INT(nc_id,var_id,att_nm,(nco_int *)vp); break;
    case NC_SHORT: rcd=nc_get_att_short(nc_id,var_id,att_nm,(nco_short *)vp); break;
    case NC_CHAR: rcd=NCO_GET_ATT_CHAR(nc_id,var_id,att_nm,(nco_char *)vp); break;
    case NC_BYTE: rcd=NCO_GET_ATT_BYTE(nc_id,var_id,att_nm,(nco_byte *)vp); break;
#ifdef ENABLE_NETCDF4
      /* 20051119: netCDF4 library did not support these until alpha10, still does not support nco_put/get_att_ubyte() */
    case NC_UBYTE: rcd=NCO_GET_ATT_UBYTE(nc_id,var_id,att_nm,(nco_ubyte *)vp); break;
    case NC_USHORT: rcd=NCO_GET_ATT_USHORT(nc_id,var_id,att_nm,(nco_ushort *)vp); break;
    case NC_UINT: rcd=NCO_GET_ATT_UINT(nc_id,var_id,att_nm,(nco_uint *)vp); break;
    case NC_INT64: rcd=NCO_GET_ATT_INT64(nc_id,var_id,att_nm,(nco_int64 *)vp); break;
    case NC_UINT64: rcd=NCO_GET_ATT_UINT64(nc_id,var_id,att_nm,(nco_uint64 *)vp); break;
    case NC_STRING: rcd=NCO_GET_ATT_STRING(nc_id,var_id,att_nm,(nco_string *)vp); break;
      /* Allow calling routine to pass att_typ as either attribute type or class-type */
    case NC_VLEN:
    case NC_OPAQUE:
    case NC_ENUM:
    case NC_COMPOUND:
      rcd=nc_get_att(nc_id,var_id,att_nm,(void *)vp); break;
#endif /* !ENABLE_NETCDF4 */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  }else{
    /* https://www.unidata.ucar.edu/software/netcdf/docs/group__attributes.html
       "nc_get_att() works for any type of attribute, and must be used to get attributes of user-defined type.
       We recommend that the type safe versions of this function be used for atomic data types." */
    rcd=nc_get_att(nc_id,var_id,att_nm,(void *)vp);
    if(0){
      char typ_nm[NC_MAX_NAME+1L]; /* [sng] Type name used in CDL "types" declaration (e.g., "vlen_t") */
      int cls_typ; /* [enm] netCDF class type, same as var_typ except contiguous from 0..16 */
      nc_type bs_typ; /* [enm] netCDF atomic type underlying vlen and enum types */
      size_t fld_nbr; /* [nbr] Number of fields in enum and compound types */
      size_t typ_sz; /* [B] Size of user-defined type */
      rcd=nc_inq_user_type(nc_id,att_typ,typ_nm,&typ_sz,&bs_typ,&fld_nbr,&cls_typ);
      (void)fprintf(stdout,"%s: DEBUG %s reports attribute %s type %d = %s, typ_nm = %s, typ_sz = %lu, bs_typ = %d = %s, fld_nbr = %lu, cls_typ = %d = %s\n","nco_get_att",fnc_nm,att_nm,att_typ,nco_typ_sng(att_typ),typ_nm,(unsigned long)typ_sz,bs_typ,nco_typ_sng(bs_typ),(unsigned long)fld_nbr,cls_typ,nco_typ_sng(cls_typ));
    } /* !0 */
  } /* !udt */
  if(rcd == NC_ENOTATT){
    char var_nm[NC_MAX_NAME+1L];
    (void)nco_inq_varname(nc_id,var_id,var_nm);
    (void)fprintf(stderr,"ERROR: %s unable to get attribute var_id: %d, var_nm: %s, att_nm: %s\n",fnc_nm,var_id,var_nm,att_nm);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_att()");
  return rcd;
} /* !nco_get_att */
/* End Attribute routines */

/* Begin netCDF4 stubs */
#ifndef HAVE_NETCDF4_H
/* NB: netCDF chunking/deflate define/inquire functions work only on netCDF4 files
   NCO stubs perform no-ops on netCDF3 files */
# ifdef NC_HAVE_NEW_CHUNKING_API
  /* Newer, post-200906 netCDF4 API has chk_sz as const
     netcdf.h signals this API with NC_HAVE_NEW_CHUNKING_API as of ~200911 */
int nc_def_var_chunking(const int nc_id,const int var_id,const int srg_typ,const size_t * const cnk_sz){return 1;}
# else /* !NC_HAVE_NEW_CHUNKING_API */
  /* Older, pre-200906 netCDF4 API has chk_sz as non-const */
int nc_def_var_chunking(const int nc_id,const int var_id,const int srg_typ,size_t * const cnk_sz){return 1;}
# endif /* !NC_HAVE_NEW_CHUNKING_API */
int nc_inq_var_chunking(const int nc_id,const int var_id,int * const srg_typ,size_t *const cnk_sz){*srg_typ=(size_t)NC_CONTIGUOUS;*cnk_sz=(size_t)NULL;return 1;}
int nc_def_var_deflate(const int nc_id,const int var_id,const int shuffle,const int deflate,const int dfl_lvl){return 1;}
int nc_inq_var_deflate(const int nc_id,const int var_id,int * const shuffle, int * const deflate,int * const dfl_lvl){if(shuffle) *shuffle=NC_NOSHUFFLE;if(deflate) *deflate=0;if(dfl_lvl) *dfl_lvl=0;return 1;}
int nc_inq_var_endian(const int nc_id,const int var_id,int * const ndn_typ){if(ndn_typ) *ndn_typ=0;return 1;}
int nc_def_var_fletcher32(const int nc_id,const int var_id,const int chk_typ){return 1;}
int nc_inq_var_fletcher32(const int nc_id,const int var_id,int * const chk_typ){if(chk_typ) *chk_typ=NC_NOCHECKSUM;return 1;}
int nc_inq_var_fill(const int nc_id,const int var_id,int * const fll_nil,void * const fll_val){if(fll_nil) *fll_nil=0;if(fll_val) assert(0);return 1;}
int nc_get_chunk_cache(size_t * const sz_byt,size_t * const cnk_nbr_hsh,float * const pmp_fvr_frc){if(sz_byt) *sz_byt=0L;if(cnk_nbr_hsh) *cnk_nbr_hsh=0L;if(pmp_fvr_frc) *pmp_fvr_frc=0.0f;return 1;}
int nc_set_chunk_cache(const size_t sz_byt,const size_t cnk_nbr_hsh,const float pmp_fvr_frc){return 1;}
#endif /* HAVE_NETCDF4_H */
#if !defined(HAVE_NETCDF4_H) 
/* Stubs thus present a fake library for manipulating netCDF3 files with the netCDF4 API
   These are only called when netCDF4 library is unavailable, thus I/O assumed to be netCDF3 */
int nco_def_enum(const int nc_id,const nc_type bs_typ,const char * const typ_nm,nc_type * const var_typ){assert(0);return NC_NOERR;}
int nco_insert_enum(const int nc_id,const nc_type var_typ,const char * const mbr_nm,const void * const vp){assert(0);return NC_NOERR;}
int nco_inq_enum(const int nc_id,const nc_type var_typ,char * const typ_nm,nc_type * const bs_typ,size_t * const typ_sz,size_t * const typ_nbr){assert(0);return NC_NOERR;}
int nco_inq_enum_member(const int nc_id,const nc_type var_typ,const int idx,char * const mbr_nm,void * const vp){assert(0);return NC_NOERR;}
int nco_inq_enum_ident(const int nc_id,const nc_type var_typ,const long long val,char * const mbr_nm){assert(0);return NC_NOERR;}
int nco_def_vlen(const int nc_id,const char * const typ_nm,const nc_type bs_typ,nc_type * const var_typ){assert(0);return NC_NOERR;}
int nco_inq_vlen(const int nc_id,const nc_type var_typ,char * const typ_nm,size_t * const typ_sz,nc_type * const bs_typ){assert(0);return NC_NOERR;}
int nco_free_vlen(nc_vlen_t * const vlenp){assert(0);return NC_NOERR;}
int nco_free_vlens(const size_t sz,nc_vlen_t * const vlenp){assert(0);return NC_NOERR;}
int nco_def_grp(const int nc_id,const char * const grp_nm,int * const grp_id){assert(0);return NC_NOERR;}
int nco_inq_grpname_full(const int nc_id,size_t * grp_nm_lng,char * const grp_nm_fll){assert(0);return NC_NOERR;}
int nco_inq_grpname_len(const int nc_id,size_t * const grp_nm_lng){assert(0);return NC_NOERR;}
int nco_inq_grps(const int nc_id,int * const grp_nbr,int * const grp_ids){if(grp_nbr) *grp_nbr=0;return NC_NOERR;}
int nco_inq_grp_full_ncid(const int nc_id,const char * const grp_nm_fll,int * const grp_id){*grp_id=nc_id;return NC_NOERR;}
int nco_inq_grp_ncid(const int nc_id,const char * const grp_nm,int * const grp_id){assert(0);return NC_NOERR;}
int nco_inq_grp_parent(const int nc_id,int * const prn_id){assert(0);return NC_ENOGRP;}
int nco_inq_grp_parent_flg(const int nc_id,int * const prn_id){return NC_ENOGRP;}
int nco_inq_ncid_flg(const int nc_id,const char * const grp_nm,int * const grp_id){assert(0);return NC_NOERR;}
int nco_inq_grp_ncid_flg(const int nc_id,const char * const grp_nm,int * const grp_id){*grp_id=nc_id;return NC_NOERR;}
int nco_inq_grpname(const int nc_id,char * const grp_nm){if(grp_nm) strcpy(grp_nm,"/");return NC_NOERR;}
int nco_inq_grp_full_ncid_flg(const int nc_id,const char * const grp_nm_fll,int * const grp_id){*grp_id=nc_id;return NC_NOERR;}
int nco_rename_grp(int grp_id,const char * const grp_nm){assert(0);return NC_NOERR;}

int nco_inq_dimids(const int nc_id,int * const dmn_nbr,int * const dmn_ids,int flg_prn){
  int dmn_idx;
  int rcd;
  rcd=nc_inq(nc_id,dmn_nbr,(int *)NULL,(int *)NULL,(int *)NULL);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimids()");
  /* netCDF3 file dimension IDs range 0..N-1 */
  if(dmn_ids)
    for(dmn_idx=0;dmn_idx<*dmn_nbr;dmn_idx++) dmn_ids[dmn_idx]=dmn_idx;
  return NC_NOERR;
} /* !nco_inq_dimids() */
int nco_inq_unlimdims(const int nc_id,int *nbr_dmn_ult,int *dmn_ids_ult){
  int rcd;
  int unlimid;
  rcd=nc_inq_unlimdim(nc_id,&unlimid);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_unlimdims()");
  if(unlimid != -1){
    *nbr_dmn_ult=1;
    if(dmn_ids_ult) dmn_ids_ult[0]=unlimid;
  }else{
    *nbr_dmn_ult=0;
  } /* unlimid */
  return NC_NOERR;
} /* !nco_inq_unlimdims() */

int nco_inq_type(const int nc_id,const nc_type var_typ,char * const typ_nm,size_t * const typ_sz){assert(0);return NC_NOERR;}
int nco_inq_user_type(const int nc_id,const nc_type var_typ,char * const typ_nm,size_t * const typ_sz,nc_type * const bs_typ,size_t * const fld_nbr,int * const cls_typ){assert(0);return NC_NOERR;}
int nco_inq_typeids(const int nc_id,int * const typ_nbr,int * const typ_ids){if(typ_nbr) *typ_nbr=0;return NC_NOERR;}

int nco_inq_varids(const int nc_id,int * const var_nbr,int * const var_ids){
  int rcd;
  /* netCDF3 files have only the root group, with variable IDs 0..N-1 */
  rcd=nc_inq(nc_id,(int *)NULL,var_nbr,(int *)NULL,(int *)NULL);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varids()");
  if(var_ids)
    for(int var_idx=0;var_idx<*var_nbr;var_idx++) var_ids[var_idx]=var_idx;
  return NC_NOERR;
} /* !nco_inq_varids() */
#endif /* HAVE_NETCDF4_H */
#ifndef _MSC_VER
#ifndef ENABLE_NETCDF4
int NCO_GET_VAR1_UBYTE(const int nc_id,const int var_id,const size_t *srt,nco_ubyte *ubp){return 1;}
int NCO_GET_VAR1_USHORT(const int nc_id,const int var_id,const size_t *srt,nco_ushort *usp){return 1;}
int NCO_GET_VAR1_UINT(const int nc_id,const int var_id,const size_t *srt,nco_uint *uip){return 1;}
int NCO_GET_VAR1_INT64(const int nc_id,const int var_id,const size_t *srt,nco_int64 *i64p){return 1;}
int NCO_GET_VAR1_UINT64(const int nc_id,const int var_id,const size_t *srt,nco_uint64 *ui64p){return 1;}
int NCO_GET_VAR1_STRING(const int nc_id,const int var_id,const size_t *srt,nco_string *sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
int NCO_PUT_VAR1_UBYTE(const int nc_id,const int var_id,const size_t *srt,const nco_ubyte *ubp){return 1;}
int NCO_PUT_VAR1_USHORT(const int nc_id,const int var_id,const size_t *srt,const nco_ushort *usp){return 1;}
int NCO_PUT_VAR1_UINT(const int nc_id,const int var_id,const size_t *srt,const nco_uint *uip){return 1;}
int NCO_PUT_VAR1_INT64(const int nc_id,const int var_id,const size_t *srt,const nco_int64 *i64p){return 1;}
int NCO_PUT_VAR1_UINT64(const int nc_id,const int var_id,const size_t *srt,const nco_uint64 *ui64p){return 1;}
int NCO_PUT_VAR1_STRING(const int nc_id,const int var_id,const size_t *srt,const char **sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
int NCO_GET_VARA_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_ubyte *ubp){return 1;}
int NCO_GET_VARA_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_ushort *usp){return 1;}
int NCO_GET_VARA_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_uint *uip){return 1;}
int NCO_GET_VARA_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_int64 *i64p){return 1;}
int NCO_GET_VARA_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_uint64 *ui64p){return 1;}
int NCO_GET_VARA_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_string *sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
int NCO_PUT_VARA_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_ubyte *ubp){return 1;}
int NCO_PUT_VARA_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_ushort *usp){return 1;}
int NCO_PUT_VARA_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_uint *uip){return 1;}
int NCO_PUT_VARA_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_int64 *i64p){return 1;}
int NCO_PUT_VARA_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_uint64 *ui64p){return 1;}
int NCO_PUT_VARA_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const char **sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
int NCO_GET_VARS_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_ubyte *ubp){return 1;}
int NCO_GET_VARS_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_ushort *usp){return 1;}
int NCO_GET_VARS_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_uint *uip){return 1;}
int NCO_GET_VARS_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_int64 *i64p){return 1;}
int NCO_GET_VARS_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_uint64 *ui64p){return 1;}
int NCO_GET_VARS_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_string *sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
int NCO_PUT_VARS_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const nco_ubyte *ubp){return 1;}
int NCO_PUT_VARS_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd, const nco_ushort *usp){return 1;}
int NCO_PUT_VARS_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd, const nco_uint *uip){return 1;}
int NCO_PUT_VARS_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd, const nco_int64 *i64p){return 1;}
int NCO_PUT_VARS_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const nco_uint64 *ui64p){return 1;}
int NCO_PUT_VARS_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const char **sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
int NCO_GET_VARM_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_ubyte *ubp){return 1;}
int NCO_GET_VARM_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_ushort *usp){return 1;}
int NCO_GET_VARM_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_uint *uip){return 1;}
int NCO_GET_VARM_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_int64 *i64p){return 1;}
int NCO_GET_VARM_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_uint64 *ui64p){return 1;}
int NCO_GET_VARM_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_string *sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
int NCO_PUT_VARM_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_ubyte *ubp){return 1;}
int NCO_PUT_VARM_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_ushort *usp){return 1;}
int NCO_PUT_VARM_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_uint *uip){return 1;}
int NCO_PUT_VARM_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_int64 *i64p){return 1;}
int NCO_PUT_VARM_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_uint64 *ui64p){return 1;}
int NCO_PUT_VARM_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const char **sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
/* 20051119: netcdf4 library did not support these until alpha10, still does not support nco_put/get_att_ubyte() */
int NCO_PUT_ATT_UBYTE(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_ubyte *ubp){return 1;}
int NCO_PUT_ATT_USHORT(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_ushort *usp){return 1;}
int NCO_PUT_ATT_UINT(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_uint *uip){return 1;}
int NCO_PUT_ATT_INT64(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_int64 *i64p){return 1;}
int NCO_PUT_ATT_UINT64(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_uint64 *ui64p){return 1;}
int NCO_PUT_ATT_STRING(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const char **sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#ifndef ENABLE_NETCDF4
/* 20051119: netCDF4 library did not support these until alpha10, still does not support nco_put/get_att_ubyte() */
int NCO_GET_ATT_UBYTE(const int nc_id,const int var_id,const char *att_nm,nco_ubyte *ubp){return 1;}
int NCO_GET_ATT_USHORT(const int nc_id,const int var_id,const char *att_nm,nco_ushort *usp){return 1;}
int NCO_GET_ATT_UINT(const int nc_id,const int var_id,const char *att_nm,nco_uint *uip){return 1;}
int NCO_GET_ATT_INT64(const int nc_id,const int var_id,const char *att_nm,nco_int64 *i64p){return 1;}
int NCO_GET_ATT_UINT64(const int nc_id,const int var_id,const char *att_nm,nco_uint64 *ui64p){return 1;}
int NCO_GET_ATT_STRING(const int nc_id,const int var_id,const char *att_nm,nco_string *sngp){return 1;}
#endif /* ENABLE_NETCDF4 */
#endif /* _MSC_VER */
/* end netCDF4 stubs */
