
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_sixplz.h"

#include "libsixplz/6pack.h"
#include "libsixplz/6unpack.h"

#define SIXPACK_COMPRESS pack_compressed
#define SIXPACK_UNCOMPRESS unpack

/* If you declare any globals in php_sixplz.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(sixplz)
*/

/* True global resources - no need for thread safety here */
static int le_sixplz;

/* {{{ sixplz_functions[]
 *
 * Every user visible function must have an entry in sixplz_functions[].
 */
const zend_function_entry sixplz_functions[] = {
	PHP_FE(sixplz_unpack,	NULL)
	PHP_FE(sixplz_pack,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in sixplz_functions[] */
};
/* }}} */

/* {{{ sixplz_module_entry
 */
zend_module_entry sixplz_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"sixplz",
	sixplz_functions,
	PHP_MINIT(sixplz),
	PHP_MSHUTDOWN(sixplz),
	PHP_RINIT(sixplz),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(sixplz),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(sixplz),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SIXPLZ
ZEND_GET_MODULE(sixplz)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("sixplz.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_sixplz_globals, sixplz_globals)
    STD_PHP_INI_ENTRY("sixplz.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_sixplz_globals, sixplz_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_sixplz_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_sixplz_init_globals(zend_sixplz_globals *sixplz_globals)
{
	sixplz_globals->global_value = 0;
	sixplz_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sixplz)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(sixplz)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(sixplz)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(sixplz)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sixplz)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "sixplz support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ proto int ()
   6plz_unpack(void *input, size_t input_size, FILE* output); */
PHP_FUNCTION(sixplz_unpack)
{
	char *value;
	int value_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
		return;
    }

    unsigned char* decompressed = emalloc(0);
    size_t decompressed_size;

    decompressed_size = SIXPACK_UNCOMPRESS((void *) value, value_len, (void *) &decompressed);

    RETURN_STRINGL(decompressed, decompressed_size, 0);
}
/* }}} */

/* {{{ proto long ()
   6plz_pack(void* input, size_t input_size, int level, FILE* f); */
PHP_FUNCTION(sixplz_pack)
{
	char *value;
	int value_len;
	long compression_level;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &value, &value_len, &compression_level) == FAILURE) {
		return;
    }


    unsigned char* compressed = emalloc(0);
    size_t compressed_size;

    compressed_size = SIXPACK_COMPRESS((void *) value, (size_t) value_len, 1, (void *) &compressed);

    RETURN_STRINGL(compressed, compressed_size, 0);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
