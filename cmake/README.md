# nco_bld
build NCO and its dependencies on Windows

http://nco.sourceforge.net/

Do

```
clone.bat
bld.bat
```

this clones and builds

```
zlib
hdf5
curl
netcdf
nco
```

# Changes needed

## libcurl

edit libcurl.vcxproj to include ws2_32.lib as library dependenccy

```
<Lib>
<AdditionalOptions>%(AdditionalOptions) /machine:X86 /machine:X86</AdditionalOptions>
<AdditionalDependencies>ws2_32.lib</AdditionalDependencies>
</Lib>
<ProjectReference>
<LinkLibraryDependencies>true</LinkLibraryDependencies>
</ProjectReference>
```


## hdf5

edit hdf5-static.vcxproj and add full path of zlib library as dependency

```
<Lib>
<AdditionalOptions>%(AdditionalOptions) /machine:X86</AdditionalOptions>
<AdditionalDependencies>I:\nco\cmake\zlib\build\Debug\zlibstaticd.lib;%(AdditionalDependencies)</AdditionalDependencies>
</Lib>
```

## netcdf

edit netcdf.vcxproj and add full path of HDF5 and curl libraries as dependencies

```
<Lib>
<AdditionalOptions>%(AdditionalOptions) /machine:X86</AdditionalOptions>
<AdditionalDependencies>I:\nco\cmake\hdf5\build\bin\Debug\libhdf5_hl_D.lib;I:\nco\cmake\hdf5\build\bin\Debug\libhdf5_D.lib;I:\nco\cmake\curl\build\lib\Debug\libcurl-d.lib;%(AdditionalDependencies)</AdditionalDependencies>
</Lib>
```

edit CMakeLists.txt and add an option to detect the HDF5 HL library

```
INCLUDE_DIRECTORIES(${HDF5_HL_INCLUDE_DIR})
```

# NCO CMake build

NCO should detect the ZLIB dependency in HDF5 as

```
-- Looking for H5Z_DEFLATE in I:/nco/cmake/hdf5/build/bin/Debug/libhdf5_D.lib
-- Looking for H5Z_DEFLATE in I:/nco/cmake/hdf5/build/bin/Debug/libhdf5_D.lib - found
-- ZLIB library is needed...
```

the netcdf function detection should be

```
-- Found netcdf library at: I:/nco/cmake/netcdf-c/build/liblib/Debug/netcdf.lib
-- Looking for nc_inq_path in I:/nco/cmake/netcdf-c/build/liblib/Debug/netcdf.lib
-- Looking for nc_inq_path in I:/nco/cmake/netcdf-c/build/liblib/Debug/netcdf.lib - found
```