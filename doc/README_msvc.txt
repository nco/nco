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

LIB_NETCDF defines the absolute name (eith path) of the netCDF library (called netcdf.lib), located on your local hard drive.
Note that here, the file name is included 
	
Example
	
LIB_NETCDF  
J:\netcdf-c-4.3.1.1\build\liblib\Debug\netcdf.lib	
