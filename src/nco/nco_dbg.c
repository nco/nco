void
err_prn(char *err_msg)
/* 
   char *err_msg: I the formatted error message to print
 */
{
  /* Routine to print an error message (currently only to stdout) approximately in GNU style,
     i.e., "program_name: ERROR error message....\n". This routine is intended to make error
     messages convenient to use in routines without the routine itself needing access to a copy
     of program. Therefore a copy of this routine should be placed in each main.c file, just
     like the program's usg_prn() routine. */

  /* This routine is more difficult to implement practically than it first seemed, because 
     it assumes all the calling routines have their error messages in a single string, when
     in reality, they often utilize the var_args capabilities of fprintf() themselves and
     have multiple formatted inputs. Better hold off for now. */

  (void)fprintf(stdout,"%s: %s",prg_nm_get(),err_msg);

} /* end err_prn() */
