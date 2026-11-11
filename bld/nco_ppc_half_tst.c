/* Exercise half-precision quantization without requiring a NEP installation.
   From the configured repository root:
   ./libtool --mode=link --tag=CC gcc -DHAVE_CONFIG_H -I. -Isrc/nco \
     $(nc-config --cflags) bld/nco_ppc_half_tst.c src/nco/libnco.la \
     $(nc-config --libs) -lm -o bld/nco_ppc_half_tst
   bld/nco_ppc_half_tst
   With NEP, --write/--verify also exercise the ncks raw I/O path:
   bld/nco_ppc_half_tst --write input.nc
   src/nco/ncks --nextcdf4 -O -L 0 --qnt_alg=btg --qnt default=1 input.nc output.nc
   bld/nco_ppc_half_tst --verify output.nc btg */
#define MAIN_PROGRAM_FILE
#include "libnco.h"

#define CHECK(call) do { \
  const int rcd=(call); \
  if(rcd != NC_NOERR){ \
    fprintf(stderr,"%s:%d: %s: %s\n",__FILE__,__LINE__,#call,nc_strerror(rcd)); \
    exit(EXIT_FAILURE); \
  } \
} while(0)

static void
check_values(const nc_type type,const int alg,const int lvl)
{
  uint16_t * const bits=(uint16_t *)nco_malloc(65536UL*sizeof(uint16_t));
  const uint16_t missing=type == NC_FLOAT16 ? 0x3555U : 0x3eaaU;
  const unsigned int exponent_mask=type == NC_FLOAT16 ? 0x7c00U : 0x7f80U;
  const int fraction_bits=type == NC_FLOAT16 ? 10 : 7;
  int nc_id,var_id;
  ptr_unn val,mss;
  CHECK(nc_create("nco_ppc_half_tst.nc",NC_NETCDF4|NC_CLOBBER|NC_DISKLESS,&nc_id));
  CHECK(nc_def_var(nc_id,"data",NC_FLOAT,0,NULL,&var_id));
  for(unsigned int raw=0;raw<65536U;raw++) bits[raw]=(uint16_t)raw;
  val.vp=bits;
  mss.vp=(void *)&missing;
  nco_baa_cnv=(unsigned short)alg;
  nco_ppc_bitmask(nc_id,var_id,lvl,type,65536L,True,mss,val);
  unsigned int changed=0U;
  for(unsigned int raw=0;raw<65536U;raw++){
    const unsigned int mag=raw & 0x7fffU;
    const double original=nco_f16_to_dbl((uint16_t)raw,type);
    if((raw & exponent_mask) == exponent_mask || mag == 0U || raw == missing){
      assert(bits[raw] == raw);
      continue;
    }
    assert(nco_dbl_to_f16(original,type) == raw);
    const double rounded=nco_f16_to_dbl(bits[raw],type);
    assert(isfinite(rounded));
    assert(signbit(original) == signbit(rounded));
    if(bits[raw] != raw) changed++;
    if(alg == nco_baa_btr){
      assert(fabs(rounded-original) <= fabs(original)*ldexp(0.5,-lvl));
      if(lvl >= fraction_bits) assert(bits[raw] == raw);
      int exponent;
      (void)frexp(fabs(original),&exponent);
      const double step=ldexp(1.0,exponent-1-lvl);
      const double expected=round(fabs(original)/step)*step;
      const uint16_t encoded=nco_dbl_to_f16(copysign(expected,original),type);
      if((encoded & exponent_mask) != exponent_mask) assert(bits[raw] == encoded);
    }else{
      const int digits=(int)floor(log10(fabs(original)))+1;
      const double allowed=0.5*pow(10.0,digits-lvl);
      assert(fabs(rounded-original) <= allowed);
      if(alg == nco_baa_btg){
	assert((bits[raw] & exponent_mask) == (raw & exponent_mask));
	assert(raw % 2U ? fabs(rounded) >= fabs(original) : fabs(rounded) <= fabs(original));
      }
    }
  }
  if((alg == nco_baa_btr && lvl < fraction_bits) || (alg != nco_baa_btr && lvl == 1)) assert(changed > 0U);
  CHECK(nc_close(nc_id));
  nco_free(bits);
}

#ifdef NC_NEXTCDF4
static const double samples[]={1.234375,-2.71875,0.0,-0.0,INFINITY,-INFINITY,NAN,0.000123,123.5,31.75,0.75,0.125};

static void
write_fixture(const char * const path)
{
  int nc_id,dim_id,var_id;
  CHECK(nc_create(path,NC_NEXTCDF4|NC_CLOBBER,&nc_id));
  CHECK(nc_def_dim(nc_id,"sample",sizeof(samples)/sizeof(samples[0]),&dim_id));
  for(int idx=0;idx<2;idx++){
    const nc_type type=idx ? NC_BFLOAT16 : NC_FLOAT16;
    const char * const name=idx ? "bf16" : "f16";
    CHECK(nc_def_var(nc_id,name,type,1,&dim_id,&var_id));
    CHECK(nc_def_var_fill(nc_id,var_id,NC_NOFILL,NULL));
    CHECK(nc_def_var(nc_id,idx ? "bf16_scalar" : "f16_scalar",type,0,NULL,&var_id));
    CHECK(nc_def_var_fill(nc_id,var_id,NC_NOFILL,NULL));
  }
  CHECK(nc_enddef(nc_id));
  for(int idx=0;idx<4;idx++){
    const nc_type type=idx % 2 ? NC_BFLOAT16 : NC_FLOAT16;
    const char * const names[]={"f16","bf16","f16_scalar","bf16_scalar"};
    uint16_t bits[sizeof(samples)/sizeof(samples[0])];
    for(size_t jdx=0;jdx<sizeof(samples)/sizeof(samples[0]);jdx++) bits[jdx]=nco_dbl_to_f16(samples[jdx],type);
    CHECK(nc_inq_varid(nc_id,names[idx],&var_id));
    CHECK(nc_put_var(nc_id,var_id,bits));
  }
  CHECK(nc_close(nc_id));
}

static void
verify_fixture(const char * const path,const char * const alg)
{
  int nc_id,var_id,metadata_id,level;
  CHECK(nc_open(path,NC_NEXTCDF4|NC_NOWRITE,&nc_id));
  nco_baa_cnv=!strcmp(alg,"btg") ? nco_baa_btg : !strcmp(alg,"gbr") ? nco_baa_gbr : nco_baa_btr;
  const char * const algorithm=nco_baa_cnv == nco_baa_btg ? "bitgroom" : nco_baa_cnv == nco_baa_gbr ? "granular_bitround" : "bitround";
  CHECK(nc_inq_varid(nc_id,"quantization_info",&metadata_id));
  char text[64]={0};
  CHECK(nc_get_att_text(nc_id,metadata_id,"algorithm",text));
  assert(!strcmp(text,algorithm));
  for(int idx=0;idx<4;idx++){
    const nc_type type=idx % 2 ? NC_BFLOAT16 : NC_FLOAT16;
    const char * const names[]={"f16","bf16","f16_scalar","bf16_scalar"};
    uint16_t actual[sizeof(samples)/sizeof(samples[0])];
    uint16_t expected[sizeof(samples)/sizeof(samples[0])];
    nc_type actual_type;
    CHECK(nc_inq_varid(nc_id,names[idx],&var_id));
    CHECK(nc_inq_vartype(nc_id,var_id,&actual_type));
    assert(actual_type == type);
    const size_t count=idx < 2 ? sizeof(samples)/sizeof(samples[0]) : 1;
    for(size_t jdx=0;jdx<count;jdx++) expected[jdx]=nco_dbl_to_f16(samples[jdx],type);
    ptr_unn val,mss;
    val.vp=expected;
    mss.vp=NULL;
    /* A separate in-memory file permits quantization metadata writes. */
    int tmp_id,tmp_var;
    CHECK(nc_create("nco_ppc_half_tst.nc",NC_NETCDF4|NC_CLOBBER|NC_DISKLESS,&tmp_id));
    CHECK(nc_def_var(tmp_id,"data",NC_FLOAT,0,NULL,&tmp_var));
    nco_ppc_bitmask(tmp_id,tmp_var,1,type,(long)count,False,mss,val);
    CHECK(nc_close(tmp_id));
    CHECK(nc_get_var(nc_id,var_id,actual));
    assert(!memcmp(actual,expected,count*sizeof(uint16_t)));
    CHECK(nc_get_att_int(nc_id,var_id,nco_baa_cnv == nco_baa_btr ? "quantization_nsb" : "quantization_nsd",&level));
    assert(level == 1);
    if(nco_baa_cnv == nco_baa_btr){
      nc_type att_type;
      float mre;
      CHECK(nc_inq_atttype(nc_id,var_id,"quantization_maximum_relative_error",&att_type));
      CHECK(nc_get_att_float(nc_id,var_id,"quantization_maximum_relative_error",&mre));
      assert(att_type == NC_FLOAT && mre == 0.25f);
    }
  }
  CHECK(nc_close(nc_id));
}
#endif

int
main(int argc,char **argv)
{
  nco_prg_nm=(char *)"nco_ppc_half_tst";
  if(argc > 1){
#ifdef NC_NEXTCDF4
    if(argc == 3 && !strcmp(argv[1],"--write")) write_fixture(argv[2]);
    else if(argc == 4 && !strcmp(argv[1],"--verify")) verify_fixture(argv[2],argv[3]);
    else return EXIT_FAILURE;
    return EXIT_SUCCESS;
#else
    return 77;
#endif
  }
  const int algorithms[]={nco_baa_btg,nco_baa_gbr,nco_baa_btr};
  for(int idx=0;idx<2;idx++)
    for(int alg=0;alg<3;alg++){
      const int max_level=alg == 2 ? (idx ? 8 : 11) : (idx ? 2 : 3);
      for(int lvl=1;lvl<=max_level;lvl++) check_values(idx ? NC_BFLOAT16 : NC_FLOAT16,algorithms[alg],lvl);
    }
  return EXIT_SUCCESS;
}
