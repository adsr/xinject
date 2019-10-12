#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_XINJECT

#include "php_xinject.h"

ZEND_BEGIN_ARG_INFO_EX(xinject_void_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

zend_function_entry xinject_functions[] = {
    PHP_FE(xinject_run, xinject_void_args)
    {NULL, NULL, 0, 0, 0}
};

zend_module_entry xinject_module_entry = {
    STANDARD_MODULE_HEADER,
    "xinject",
    xinject_functions,
    PHP_MINIT(xinject),
    PHP_MSHUTDOWN(xinject),
    PHP_RINIT(xinject),
    PHP_RSHUTDOWN(xinject),
    PHP_MINFO(xinject),
    XINJECT_VERSION,
    NO_MODULE_GLOBALS,
    ZEND_MODULE_POST_ZEND_DEACTIVATE_N(xinject),
    STANDARD_MODULE_PROPERTIES_EX
};

ZEND_DECLARE_MODULE_GLOBALS(xinject)

#if COMPILE_DL_XINJECT
ZEND_GET_MODULE(xinject)
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
#endif

PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("xinject.registry_path",               "/tmp/xinject.registry", PHP_INI_ALL, OnUpdateString, registry_path,               zend_xinject_globals, xinject_globals)
    STD_PHP_INI_ENTRY("xinject.registry_refresh_interval_s", "10",                    PHP_INI_ALL, OnUpdateLong,   registry_refresh_interval_s, zend_xinject_globals, xinject_globals)
    STD_PHP_INI_ENTRY("xinject.callback_fn",                 "xinject_dump",          PHP_INI_ALL, OnUpdateString, callback_fn,                 zend_xinject_globals, xinject_globals)
    STD_PHP_INI_BOOLEAN("xinject.invalidate_opcache",        "1",                     PHP_INI_ALL, OnUpdateBool,   invalidate_opcache,          zend_xinject_globals, xinject_globals)
PHP_INI_END()


static void php_xinject_init_globals(zend_xinject_globals *xg TSRMLS_DC) {
    memset(xg, 0, sizeof(zend_xinject_globals));
}

static void php_xinject_shutdown_globals(zend_xinject_globals *xg TSRMLS_DC) {
    (void);
}

PHP_MINIT_FUNCTION(xinject) {
    zend_extension dummy_ext;
    ZEND_INIT_MODULE_GLOBALS(xinject, php_xinject_init_globals, php_xinject_shutdown_globals);
    REGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(xinject) {
    return SUCCESS;
}

PHP_RINIT_FUNCTION(xinject) {
    int rv;

    #if defined(ZTS) && defined(COMPILE_DL_XINJECT)
        ZEND_TSRMLS_CACHE_UPDATE();
    #endif

    /* read registry */

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(xinject) {
    return SUCCESS;
}

PHP_MINFO_FUNCTION(xinject) {
    php_info_print_table_start();
    php_info_print_table_header(2, "xinject support", "enabled");
    php_info_print_table_row(2, "Version", XINJECT_VERSION);
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

/* {{{ proto void xinject_run(void)
   Injected in op_array of instrumented functions.
   Calls `xinject.callback_fn` in a try-catch block. */
PHP_FUNCTION(xinject_run) {
}
/* }}} */


/* {{{ proto void xinject_dump(void)
   Default for `xinject.callback_fn`.
   Prints local symbol table */
PHP_FUNCTION(xinject_dump) {
}
/* }}} */

ZEND_DLEXPORT void xinject_op_array_handler(zend_op_array *op_array) {
    TSRMLS_FETCH();
}

#ifndef ZEND_EXT_API
#define ZEND_EXT_API    ZEND_DLEXPORT
#endif

ZEND_EXT_API zend_extension_version_info extension_version_info = { ZEND_EXTENSION_API_NO, (char*) ZEND_EXTENSION_BUILD_ID };

ZEND_DLEXPORT zend_extension zend_extension_entry = {
    (char*) XINJECT_NAME,
    (char*) XINJECT_VERSION,
    (char*) XINJECT_AUTHOR,
    (char*) XINJECT_URL_FAQ,
    (char*) XINJECT_COPYRIGHT_SHORT,
    NULL,                       /* startup_func_t */
    NULL,                       /* shutdown_func_t */
    NULL,                       /* activate_func_t */
    NULL,                       /* deactivate_func_t */
    NULL,                       /* message_handler_func_t */
    xinject_op_array_handler,   /* op_array_handler_func_t */
    NULL,                       /* statement_handler_func_t */
    NULL,                       /* fcall_begin_handler_func_t */
    NULL,                       /* fcall_end_handler_func_t */
    NULL,                       /* op_array_ctor_func_t */
    NULL,                       /* op_array_dtor_func_t */
    STANDARD_ZEND_EXTENSION_PROPERTIES
};

#endif
