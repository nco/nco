typedef union{
  char *str;
  char *output_var;
  char *vara;
  aed_sct att;
  sym_sct *sym;
  parse_sct attribute;
  var_sct *var;
  nm_lst_sct *sbs_lst;
} YYSTYPE;
#define	STRING	258
#define	ATTRIBUTE	259
#define	VAR	260
#define	OUT_VAR	261
#define	OUT_ATT	262
#define	FUNCTION	263
#define	LHS_SBS	264
#define	POWER	265
#define	ABS	266
#define	ATOSTR	267
#define	IGNORE	268
#define	EPROVOKE	269
#define	UMINUS	270

