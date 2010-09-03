# Fedora RPMs are up-to-date!
# http://cvs.fedoraproject.org/viewvc/devel/nco/nco.spec?view=co

Name:           nco
Version:        4.0.3
Release:        1%{?dist}
Summary:        Programs that manipulate netCDF files
Group:          Applications/Engineering
License:        GPL3
URL:            http://nco.sourceforge.net/

# Obtain NCO version 4.0.3-1 tar.gz from Sourceforge using CVS:
# cvs -d:pserver:anonymous@nco.cvs.sf.net:/cvsroot/nco login
# cvs -z3 -d:pserver:anonymous@nco.cvs.sf.net:/cvsroot/nco co -r nco-4_0_3-1 -d nco-%{version} nco
# tar czf nco-%{version}.tar.gz --exclude='nco-4.0.3/debian*' --exclude='.cvsignore' --exclude=ncap_lex.c --exclude='ncap_yacc.[ch]' ./nco-%{version}
Source0:        nco-%{version}.tar.gz
#Patch0:		nco_install_C_headers.patch
#Patch1:         nco_find_udunits-dat.patch
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  automake, autoconf, libtool
BuildRequires:  netcdf, netcdf-devel
BuildRequires:  udunits, udunits-devel
# Required for ncap:
BuildRequires:  bison, flex
# Required for ncap2:
BuildRequires:  gsl, gsl-devel
#BuildRequires:  antlr antlr-c++-devel
# Following libraries required to DAP-enable NCO:
BuildRequires:  curl, curl-devel 
BuildRequires:  libxml2, libxml2-devel
BuildRequires:  libdap, libdap-devel, libnc-dap, libnc-dap-devel

%package devel
Summary:        Development files for NCO
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description
NCO is a suite of programs known as operators. The operators are stand-alone, 
command-line programs executable in a POSIX shell. Operators take one or more 
netCDF files as input, perform operations (e.g., averaging, hyperslabbing), 
and produce a netCDF output file. NCO was originally designed to manipulate and 
analyze climate data, though it works on any netCDF format datasets.

%description devel
This package contains NCO header files and static libraries.

%prep
%setup -q
#%patch0 -p1
#%patch1 -p1

%build
aclocal
autoheader
automake --foreign
autoconf
# Explicitly set system netCDF directories to override development netCDF
# installations in, e.g., /usr/local
export CPPFLAGS=-I%{_includedir}/netcdf-3
# Always put netcdf-3 on path and, for x86_64, add -L/usr/lib64, when present, to correctly resolve 32/64-bit libraries
export LDFLAGS="-L%{_libdir}/netcdf-3 %( uname -m | egrep -q '_64$' && [ -d /usr/lib64 ] && echo '-L/usr/lib64' )"
export CFLAGS="$RPM_OPT_FLAGS -fPIC"
export CXXFLAGS="$RPM_OPT_FLAGS -fpermissive -fPIC"
%configure --includedir=%{_includedir}/nco
make %{?_smp_mflags}
unset CPPFLAGS LDFLAGS CFLAGS CXXFLAGS

%install
rm -rf ${RPM_BUILD_ROOT}
mkdir ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}%{_includedir}/nco
make install DESTDIR=${RPM_BUILD_ROOT}
rm -f ${RPM_BUILD_ROOT}%{_libdir}/*.la
rm -f ${RPM_BUILD_ROOT}%{_infodir}/dir
rm -f ${RPM_BUILD_ROOT}%{_bindir}/mpnc*

%clean
rm -rf ${RPM_BUILD_ROOT}

%post
/sbin/ldconfig
/sbin/install-info %{_infodir}/nco.info.gz \
    %{_infodir}/dir 2>/dev/null || :

%postun
/sbin/ldconfig
if [ "$1" = 0 ]; then
  /sbin/install-info --delete %{_infodir}/nco.info.gz \
    %{_infodir}/dir 2>/dev/null || :
fi

%files
%defattr(-,root,root,-)
%doc doc/README doc/LICENSE doc/rtfm.txt
%{_bindir}/*
%{_mandir}/*/*
%{_infodir}/*
%{_libdir}/libnco*[0-9]*.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/nco
%{_libdir}/libnco*.a
%{_libdir}/libnco.so
%{_libdir}/libnco_c++.so
# %{_libdir}/libnco++.so

%changelog
* Thu Sep 02 2010 Charlie Zender <zender@uci.edu> - 4.0.3-1
- new upstream 4.0.3

* Sun Jun 27 2010 Charlie Zender <zender@uci.edu> - 4.0.2-1
- new upstream 4.0.2

* Mon Apr 05 2010 Charlie Zender <zender@uci.edu> - 4.0.1-1
- new upstream 4.0.1

* Tue Jan 05 2010 Charlie Zender <zender@uci.edu> - 4.0.0-1
- new upstream 4.0.0

* Tue Jul 14 2009 Charlie Zender <zender@uci.edu> - 3.9.9-1
- new upstream 3.9.9

* Mon Mar 23 2009 Charlie Zender <zender@uci.edu> - 3.9.8-1
- new upstream 3.9.8

* Wed Mar 18 2009 Charlie Zender <zender@uci.edu> - 3.9.7-1
- new upstream 3.9.7

* Thu Jan 22 2009 Charlie Zender <zender@uci.edu> - 3.9.6-1
- new upstream 3.9.6

* Thu Oct 30 2008 Charlie Zender <zender@uci.edu> - 3.9.5-2
- buildrequire GSL

* Sun May 11 2008 Charlie Zender <zender@uci.edu> - 3.9.5-1
- new upstream 3.9.5

* Mon Mar 03 2008 Charlie Zender <zender@uci.edu> - 3.9.4-1
- new upstream 3.9.4

* Sat Dec 08 2007 Charlie Zender <zender@uci.edu> - 3.9.3-1
- new upstream 3.9.3

* Wed Aug 29 2007 Charlie Zender <zender@uci.edu> - 3.9.2-1
- new upstream 3.9.2

* Fri Jun 29 2007 Charlie Zender <zender@uci.edu> - 3.9.1-1
- new upstream 3.9.1

* Tue Jun 19 2007 Daniel L. Wang <wangd@uci.edu> - 3.9.0-1
- fix LDFLAGS to detect lib64 usage (workaround buggy libtool)

* Fri May 25 2007 Charlie Zender <zender@uci.edu> - 3.9.0-1
- new upstream 3.9.0

* Fri Apr 20 2007 Charlie Zender <zender@uci.edu> - 3.2.0-1
- new upstream 3.2.0 (includes rmssdn arithmetic bugfix)

* Sat Mar 10 2007 Charlie Zender <zender@uci.edu> - 3.1.9-1
- new 3.1.9 to synchronize upstream, debs, and RPMs

* Sat Mar 10 2007 Charlie Zender <zender@uci.edu> - 3.1.8-1
- clean up nco.spec
- verify ncap2 is built and distributed
- buildrequire libdap, libdap-devel, libnc-dap, libnc-dap-devel 
- new upstream 3.1.8

* Sat Nov 11 2006 Charlie Zender <zender@uci.edu> - 3.1.7-1
- Merge Fedora nco.spec UDUnits patch into upstream configure.in
- new upstream 3.1.7

* Sat Sep  2 2006 Ed Hill <ed@eh3.com> - 3.1.5-3
- br bison as well

* Sat Sep  2 2006 Ed Hill <ed@eh3.com> - 3.1.5-2
- buildrequire flex

* Sat Sep  2 2006 Ed Hill <ed@eh3.com> - 3.1.5-1
- new upstream 3.1.5

* Fri Apr 21 2006 Ed Hill <ed@eh3.com> - 3.1.2-1
- update to new upstream 3.1.2

* Thu Feb 16 2006 Ed Hill <ed@eh3.com> - 3.0.2-2
- rebuild for new gcc

* Mon Sep  5 2005 Ed Hill <ed@eh3.com> - 3.0.2-1
- update to new upstream 3.0.2

* Wed Aug  3 2005 Ed Hill <ed@eh3.com> - 3.0.1-4
- remove (hopefully only temporarily) opendap support

* Thu Jul 21 2005 Ed Hill <ed@eh3.com> - 3.0.1-3
- add LICENSE file

* Sat Jul  9 2005 Ed Hill <ed@eh3.com> - 3.0.1-2
- add BuildRequires: opendap-devel

* Sun Jun 19 2005 Ed Hill <ed@eh3.com> - 3.0.1-1
- update to upstream 3.0.1
- comment & fixes for BuildRequires

* Sat Apr 23 2005 Ed Hill <ed@eh3.com> - 3.0.0-2
- add BuildRequires and fix CXXFLAGS per Tom Callaway
- add udunits patch per Tom Callaway

* Sat Apr 16 2005 Ed Hill <ed@eh3.com> - 3.0.0-1
- update to ver 3.0.0
- devel package fixes per D.M. Kaplan and M. Schwendt
- fix info post/postun

* Sun Dec  5 2004 Ed Hill <eh3@mit.edu> - 0:2.9.9-0.fdr.4
- sync with netcdf-3.6.0beta6-0.fdr.0
- split into devel and non-devel

* Wed Dec  1 2004 Ed Hill <eh3@mit.edu> - 0:2.9.9-0.fdr.3
- sync with netcdf-0:3.5.1-0.fdr.11
- added '-fpermissive' for GCC 3.4.2 warnings
- added "Provides:nco-devel" for the headers and libs

* Mon Oct  4 2004 Ed Hill <eh3@mit.edu> - 0:2.9.9-0.fdr.2
- Add some of Michael Schwendt's suggested INC/LIB path fixes and 
  sync with the netcdf-3.5.1-0.fdr.10 dependency.

* Thu Sep 23 2004 Ed Hill <eh3@mit.edu> - 0:2.9.9-0.fdr.1
- add NETCDF_INC and NETCDF_LIB to work on systems where old
  versions of netcdf may exist in /usr/local

* Wed Sep  8 2004 Ed Hill <eh3@mit.edu> - 0:2.9.9-0.fdr.0
- updated to ver 2.9.9

* Sat Aug  7 2004 Ed Hill <eh3@mit.edu> - 0:2.9.8-0.fdr.0
- updated to ver 2.9.8

* Sat Jul 17 2004 Ed Hill <eh3@mit.edu> - 0:2.9.7-0.fdr.2
- removed unneeded %ifarch

* Sat Jul 17 2004 Ed Hill <eh3@mit.edu> - 0:2.9.7-0.fdr.1
- Add %post,%postun

* Sat Jul 17 2004 Ed Hill <eh3@mit.edu> - 0:2.9.7-0.fdr.0
- Initial working version

* Wed Mar  1 2000 Charlie Zender <zender@uci.edu> - 1.1.45-1
- Added original nco.spec to bld directory


