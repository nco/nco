AC_DEFUN([AC_CXX_HAVE_VALARRAY],
[AC_CACHE_CHECK(whether C++ compiler has working valarray<T>,
ac_cv_cxx_have_valarray,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <valarray>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[valarray<float> x(100); return 0;],
 ac_cv_cxx_have_valarray=yes, ac_cv_cxx_have_valarray=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_valarray" = yes; then
  AC_DEFINE(HAVE_VALARRAY,,[Define if C++ compiler has working valarray<T>])
fi
])

AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether C++ compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[Define if C++ compiler implements namespaces])
fi
])

AC_DEFUN([NCO_CHECK_FUNCS],
[AC_FOREACH([AC_Func],[$1],
  [AH_TEMPLATE(AS_TR_CPP(HAVE_[]AC_Func),dnl
		[Define to 1 if compiler finds external `]AC_Func[' function])
	AH_TEMPLATE(AS_TR_CPP(NEED_[]AC_Func),dnl
	[Define to 1 if compiler needs external `]AC_Func[' function])	       
	       ])dnl
for ac_func in $1
do
AC_CHECK_FUNC($ac_func,
              [AC_DEFINE_UNQUOTED([AS_TR_CPP([HAVE_$ac_func])])],
              [AC_DEFINE_UNQUOTED([AS_TR_CPP([NEED_$ac_func])])])dnl
done
])
