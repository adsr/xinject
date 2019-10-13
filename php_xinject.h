#ifndef PHP_XINJECT_H
#define PHP_XINJECT_H

#define XINJECT_NAME            "xinject"
#define XINJECT_VERSION         "0.1.0-dev"
#define XINJECT_AUTHOR          "Adam Saponara"
#define XINJECT_COPYRIGHT_SHORT "Copyright (c) 2019"
#define XINJECT_URL             "https://github.com/adsr/xinject"
#define XINJECT_URL_FAQ         "https://github.com/adsr/xinject"

#include "php.h"
#include "TSRM.h"
#include "main/SAPI.h"
#include "ext/standard/info.h"
#include "zend_extensions.h"

extern zend_module_entry xinject_module_entry;

PHP_MINIT_FUNCTION(xinject);
PHP_MSHUTDOWN_FUNCTION(xinject);
PHP_RINIT_FUNCTION(xinject);
PHP_RSHUTDOWN_FUNCTION(xinject);
PHP_MINFO_FUNCTION(xinject);

PHP_FUNCTION(xinject_run);
PHP_FUNCTION(xinject_dump);

ZEND_BEGIN_MODULE_GLOBALS(xinject)
    char *registry_path;
    char *callback_fn;
    zend_bool invalidate_opcache;
    zend_long registry_refresh_interval_s;
ZEND_END_MODULE_GLOBALS(xinject)

#endif
