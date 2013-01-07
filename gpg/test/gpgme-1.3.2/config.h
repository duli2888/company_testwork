/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Whether Assuan support is enabled */
#define ENABLE_ASSUAN 1

/* Whether G13 support is enabled */
/* #undef ENABLE_G13 */

/* Whether GPGCONF support is enabled */
/* #undef ENABLE_GPGCONF */

/* Whether GPGSM support is enabled */
/* #undef ENABLE_GPGSM */

/* Defined if we are building with uiserver support. */
/* #undef ENABLE_UISERVER */

/* Path to the G13 binary. */
/* #undef G13_PATH */

/* Path to the GPGCONF binary. */
/* #undef GPGCONF_PATH */

/* version of the libassuan library */
#define GPGME_LIBASSUAN_VERSION "2.0.3"

/* Path to the GPGSM binary. */
/* #undef GPGSM_PATH */

/* The default error source for GPGME. */
#define GPG_ERR_SOURCE_DEFAULT GPG_ERR_SOURCE_GPGME

/* Path to the GnuPG binary. */
#define GPG_PATH "/usr/bin/gpg"

/* Define to 1 if you have the <argp.h> header file. */
#define HAVE_ARGP_H 1

/* Define if ttyname_r is does not work with small buffers */
/* #undef HAVE_BROKEN_TTYNAME_R */

/* Define to 1 if you have the declaration of `ttyname_r', and to 0 if you
   don't. */
#define HAVE_DECL_TTYNAME_R 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Defined if we run on some of the PCDOS like systems (DOS, Windoze. OS/2)
   with special properties like no file modes */
/* #undef HAVE_DOSISH_SYSTEM */

/* Define to 1 if the system has the type `error_t'. */
#define HAVE_ERROR_T 1

/* Define to 1 if you have the `fopencookie' function. */
#define HAVE_FOPENCOOKIE 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the `funopen' function. */
/* #undef HAVE_FUNOPEN */

/* Define to 1 if you have the `getegid' function. */
#define HAVE_GETEGID 1

/* Define to 1 if you have the `getenv_r' function. */
/* #undef HAVE_GETENV_R */

/* Define to 1 if you have the `getgid' function. */
#define HAVE_GETGID 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if the ttyname_r function has a POSIX compliant declaration. */
#define HAVE_POSIXDECL_TTYNAME_R 1

/* Define if we have pthread. */
#define HAVE_PTHREAD /**/

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/uio.h> header file. */
#define HAVE_SYS_UIO_H 1

/* Define if getenv() is thread-safe */
#define HAVE_THREAD_SAFE_GETENV 1

/* Define to 1 if you have the `timegm' function. */
#define HAVE_TIMEGM 1

/* Define if __thread is supported */
#define HAVE_TLS 1

/* Define to 1 if you have the `ttyname_r' function. */
#define HAVE_TTYNAME_R 1

/* Define to 1 if the system has the type `uintptr_t'. */
#define HAVE_UINTPTR_T 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Defined if we run on a W32 CE API based system */
/* #undef HAVE_W32CE_SYSTEM */

/* Defined if we run on any kind of W32 API based system */
/* #undef HAVE_W32_SYSTEM */

/* Defined if we run on a 64 bit W32 API based system */
/* #undef HAVE_W64_SYSTEM */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* used to implement the va_copy macro */
/* #undef MUST_COPY_VA_BYVAL */

/* Min. needed G13 version. */
#define NEED_G13_VERSION "2.1.0"

/* Min. needed GPGCONF version. */
#define NEED_GPGCONF_VERSION "2.0.4"

/* Min. needed GPGSM version. */
#define NEED_GPGSM_VERSION "1.9.6"

/* Min. needed GnuPG version. */
#define NEED_GPG_VERSION "1.4.0"

/* Name of this package */
#define PACKAGE "gpgme"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugs.gnupg.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "gpgme"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "gpgme 1.3.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "gpgme"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.3.2"

/* Define to 1 if ttyname_r is a replacement function. */
/* #undef REPLACE_TTYNAME_R */


/* Separators as used in $PATH.  */
#ifdef HAVE_DOSISH_SYSTEM
#define PATHSEP_C ';'
#else
#define PATHSEP_C ':'
#endif


/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Defined if descriptor passing is enabled and supported */
/* #undef USE_DESCRIPTOR_PASSING */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version of this package */
#define VERSION "1.3.2"

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* To allow the use of GPGME in multithreaded programs we have to use
  special features from the library.
  IMPORTANT: gpgme is not yet fully reentrant and you should use it
  only from one thread.  */
#ifndef _REENTRANT
# define _REENTRANT 1
#endif

/* Activate POSIX interface on MacOS X */
/* #undef _XOPEN_SOURCE */

/* Define to a type to use for `error_t' if it is not otherwise available. */
/* #undef error_t */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to the type of an unsigned integer type wide enough to hold a
   pointer, if such a type exists, and if the system does not define it. */
/* #undef uintptr_t */


/* Definition of GCC specific attributes.  */
#if __GNUC__ > 2
# define GPGME_GCC_A_PURE  __attribute__ ((__pure__))
#else
# define GPGME_GCC_A_PURE
#endif

/* Under WindowsCE we need gpg-error's strerror macro.  */
#define GPG_ERR_ENABLE_ERRNO_MACROS 1

