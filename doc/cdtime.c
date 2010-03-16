/* Purpose: Illustrate usage of CDAT calendar functions in CF library */

/* Usage:
   gcc -I/usr/local/include -L/usr/local/lib -lcf cdtime.c -o cdtime */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libcf.h" 

#define True 1
#define False 0

int main(int argc, char **argv){

  char *s_in;  // disk units e.g "days since 170:01:01"
  char *s_out;  //string of value (from hyperslab limit) 

  cdCompTime comptime;  

  s_in=strdup(argv[1]);  // string with number units e.g 10 mm
  s_out=strdup(argv[2]); // output units             e.g feet,inches, "days since 2001:10:12 00:00:01"
  //s_out=strdup("feet");
  printf("system check argv[1]=%s argv[2]=%s\n", s_in,s_out); 

    
  (void)cdChar2Comp(cdStandard,s_in, &comptime);     

  // print out comptime
  printf("input =%s \n", s_in);
  printf("year=%li month=%d day=%d hour=%f \n",comptime.year,comptime.month,comptime.day,comptime.hour);

  exit(0);

}

