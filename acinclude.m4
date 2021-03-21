AC_DEFUN([AC_CXX_HAVE_VALARRAY],
[AC_CACHE_CHECK(whether C++ compiler has working valarray<T>,
ac_cv_cxx_have_valarray,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG([C++])
 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <valarray>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif]], [[valarray<float> x(100); return 0;]])],[ac_cv_cxx_have_valarray=yes],[ac_cv_cxx_have_valarray=no])
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
 AC_LANG([C++])
 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[namespace Outer { namespace Inner { int i = 0; }}]], [[using namespace Outer::Inner; return i;]])],[ac_cv_cxx_namespaces=yes],[ac_cv_cxx_namespaces=no])
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[Define if C++ compiler implements namespaces])
fi
])

dnl NCO_CHECK_FUNCS provided by Eric Blake of RedHat on autoconf USENET group 20100805
AC_DEFUN([NCO_CHECK_FUNCS],
[m4_foreach_w([NCO_Func],[$1],[AC_CHECK_FUNC(NCO_Func,
 [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_]NCO_Func), [1],
     [Define to 1 if compiler finds external `]NCO_Func[' function])],
 [AC_DEFINE_UNQUOTED(AS_TR_CPP([NEED_]NCO_Func), [1],
     [Define to 1 if compiler needs external `]NCO_Func[' function])]dnl
)])])
