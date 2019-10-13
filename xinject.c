#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_XINJECT

#include "php_xinject.h"

ZEND_BEGIN_ARG_INFO_EX(xinject_void_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

zend_function_entry xinject_functions[] = {
    PHP_FE(xinject_run,  xinject_void_args)
    PHP_FE(xinject_dump, xinject_void_args)
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
    NULL,
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

static void php_xinject_init_globals(zend_xinject_globals *xg) {
    memset(xg, 0, sizeof(zend_xinject_globals));
}

static void php_xinject_shutdown_globals(zend_xinject_globals *xg) {
    return;
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

ZEND_DLEXPORT int xinject_zend_startup(zend_extension *extension) {
    return zend_startup_module(&xinject_module_entry);
}

ZEND_DLEXPORT void xinject_zend_shutdown(zend_extension *extension) {
    return;
}

ZEND_DLEXPORT void xinject_op_array_handler(zend_op_array *op_array) {
    const char *fname;
    int lineno;
    zval zv;
    zval *zfunc;
    zend_function *func;
    zend_op *op;
    zend_string *func_name;

    /* TODO get from registry */
    fname = "/home/adam/xinject/test.php";
    lineno = 5;

    if (!op_array->function_name) {
        return;
    }

    if (op_array->filename && strcmp(fname, ZSTR_VAL(op_array->filename)) == 0) {
        if (lineno >= op_array->line_start && lineno <= op_array->line_end) {

            func_name = zend_string_init("xinject_run", strlen("xinject_run"), 0);
            zfunc = zend_hash_find_ex(EG(function_table), func_name, 0);
            zend_string_release(func_name);
            func = Z_PTR_P(zfunc);

            /* Make space for one more literal ("xinject_run") */
            op_array->last_literal += 1;
            op_array->literals = (zval *)erealloc(op_array->literals, sizeof(zval) * op_array->last_literal);
            // memset(&op_array->literals[op_array->last_literal - 1], 0, sizeof(zval));
            // ZVAL_STRING(&op_array->literals[op_array->last_literal - 1], "xinject_run");
            ZVAL_STR_COPY(&op_array->literals[op_array->last_literal - 1], func->common.function_name);
            // ZVAL_COPY_VALUE(&op_array->literals[op_array->last_literal - 1], &zv);

            /* Make space for two more opcodes (ZEND_INIT_FCALL, ZEND_DO_ICALL)  */
            op_array->last += 2;
            op_array->opcodes = (zend_op *)erealloc(op_array->opcodes, sizeof(zend_op) * op_array->last);

            /* TODO allow injecting in middle of op_array */
            memmove(op_array->opcodes + 2, op_array->opcodes, sizeof(zend_op) * (op_array->last - 2));

            op = &op_array->opcodes[0];
            memset(op, 0, sizeof(*op));
            op->opcode = ZEND_INIT_FCALL;
            op->op2.constant = op_array->last_literal - 1;
            op->op2_type = IS_CONST;
            op->op1_type = IS_UNUSED;
            op->result_type = IS_UNUSED;

            op = &op_array->opcodes[1];
            memset(op, 0, sizeof(*op));
            op->opcode = ZEND_DO_ICALL;
            op->op1_type = IS_UNUSED;
            op->op2_type = IS_UNUSED;
            op->result_type = IS_UNUSED;
        }
    }
}

/* {{{ proto void xinject_run(void)
   Injected in op_array of instrumented functions.
   Calls `xinject.callback_fn` in a try-catch block. */
PHP_FUNCTION(xinject_run) {
    zval retval;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;

    memset(&fci, 0, sizeof(fci));
    memset(&fcc, 0, sizeof(fcc));

    fci.size = sizeof(fci);
    ZVAL_STRING(&fci.function_name, "injectable"); /* TODO use callback_fn */
    fci.retval = &retval;
    fci.no_separation = 1;

    zend_try {
        if (zend_call_function(&fci, &fcc) != SUCCESS) {
            /* TODO call failed */
        }
    } zend_catch {
        /* TODO call threw exception */
    } zend_end_try();

    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&fci.function_name);

    RETURN_NULL();
}
/* }}} */

/* {{{ proto void xinject_dump(void)
   Default for `xinject.callback_fn`.
   Prints local symbol table */
PHP_FUNCTION(xinject_dump) {
    zend_array *sym_table;
    zval sym_table_z;

    sym_table = zend_rebuild_symbol_table();

    if (sym_table) {
        ZVAL_ARR(&sym_table_z, sym_table);
        zend_print_zval_r(&sym_table_z, 0);
    }

    RETURN_NULL();
}
/* }}} */

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
    xinject_zend_startup,       /* startup_func_t */
    xinject_zend_shutdown,      /* shutdown_func_t */
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
