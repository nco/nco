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
#define	STRING	257
#define	ATTRIBUTE	258
#define	VAR	259
#define	OUT_VAR	260
#define	OUT_ATT	261
#define	FUNCTION	262
#define	LHS_SBS	263
#define	POWER	264
#define	ABS	265
#define	ATOSTR	266
#define	IGNORE	267
#define	EPROVOKE	268
#define	UMINUS	269

