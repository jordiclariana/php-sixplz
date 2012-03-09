
#ifndef PHP_SIXPLZ_H
#define PHP_SIXPLZ_H

extern zend_module_entry sixplz_module_entry;
#define phpext_sixplz_ptr &sixplz_module_entry

#ifdef PHP_WIN32
#	define PHP_SIXPLZ_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SIXPLZ_API __attribute__ ((visibility("default")))
#else
#	define PHP_SIXPLZ_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(sixplz);
PHP_MSHUTDOWN_FUNCTION(sixplz);
PHP_RINIT_FUNCTION(sixplz);
PHP_RSHUTDOWN_FUNCTION(sixplz);
PHP_MINFO_FUNCTION(sixplz);

PHP_FUNCTION(sixplz_unpack);
PHP_FUNCTION(sixplz_pack);

/*
ZEND_BEGIN_MODULE_GLOBALS(sixplz)
	long  compression_level;
ZEND_END_MODULE_GLOBALS(sixplz)

ZEND_EXTERN_MODULE_GLOBALS(sixplz)
*/

/* In every utility function you add that needs to use variables 
   in php_sixplz_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as SIXPLZ_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define SIXPLZ_G(v) TSRMG(sixplz_globals_id, zend_sixplz_globals *, v)
#else
#define SIXPLZ_G(v) (sixplz_globals.v)
#endif

#endif	/* PHP_SIXPLZ_H */

