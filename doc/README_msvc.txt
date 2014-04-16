Microsoft Windows Visual Studio 2010 (MSVC) build instructions:

Building the NCO source code from within Visual Studio is an easy process, consisting of 2 steps:

1) Define environment variables.
2) Build the solution.

1) Define environment variables

The Visual Studio NCO solution file (located at /qt/nco.sln) uses 2 types of environment variables: 

a)	C source file header paths.
b)	NCO dependency libraries.

The minimal requirement to build NCO is the netCDF library. These 2 symbols are defined in the MSVC solution
and must be defined as enviroment variables

HEADER_NETCDF
LIB_NETCDF  

HEADER_NETCDF defines the location (path) where the netCDF header file is located on your local hard drive. 
This file is called netcdf.h. Note that only the path (without the file name) is defined.

Example

HEADER_NETCDF 
J:\netcdf-c-4.3.1.1\include

LIB_NETCDF defines the absolute name (with path) of the netCDF library (the file is called netcdf.lib), located on your local hard drive.
Note that here, the file name is included 
	
Example
	
LIB_NETCDF  
J:\netcdf-c-4.3.1.1\build\liblib\Debug\netcdf.lib	

Note:

To define an enviroment variable in Microsoft Windows, do:

1) Go to menu “Start”
2) Go to menu “Computer”
3) Right click menu "Properties"
4) Choose option “Advanced System Settings”
5) Choose option “Environment Variables”
6) Choose "New" button on the section "System Variables"
7) Enter a pair name, value

From the previous example, a name would be 
HEADER_NETCDF
with value
J:\netcdf-c-4.3.1.1\include

Note: You’ll have to exit Visual Studio and open the project again so that these values are detected by MSVC.

Building ncap2

ncpa2 requires 2 additional libraries, Antlr, and gsl.

Define these environment variables (examples follow)

HEADER_ANTLR
J:\antlr-2.7.7\lib\cpp

LIB_ANTLR
J:\antlr-2.7.7\lib\cpp\debug\antlr.lib

HEADER_GSL
J:\gsl-1.8\src\gsl\1.8\gsl-1.8

LIB_GSL
J:\gsl-1.8\src\gsl\1.8\gsl-1.8\VC8\libgsl\Debug-StaticLib\libgsl_d.lib







