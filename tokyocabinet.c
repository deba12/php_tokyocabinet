/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  |  Author: lijinxing(qiye)  MSN: lijinxing20@hotmail.com               |
  |  MAIL: lijinxing@gmail.com                                           |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"
#include "php_tokyocabinet.h"

#include <time.h>
#include <stdio.h>
#include <tchdb.h>
#include <tcbdb.h>
#include <tctdb.h>
#include <tcutil.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>


/* If you declare any globals in php_tokyocabinet.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(tokyocabinet)
*/


//tokyocabinet_hdb
PHP_METHOD(tokyocabinet_hdb, __construct);

PHP_METHOD(tokyocabinet_hdb, put);

PHP_METHOD(tokyocabinet_hdb, get);

PHP_METHOD(tokyocabinet_hdb, delete);

PHP_METHOD(tokyocabinet_hdb, sync);

PHP_METHOD(tokyocabinet_hdb, errmsg);

PHP_METHOD(tokyocabinet_hdb, iterinit);

PHP_METHOD(tokyocabinet_hdb, iternext);

PHP_METHOD(tokyocabinet_hdb, setcache);

PHP_METHOD(tokyocabinet_hdb, setxmsiz);

PHP_METHOD(tokyocabinet_hdb, tranbegin);

PHP_METHOD(tokyocabinet_hdb, trancommit);

PHP_METHOD(tokyocabinet_hdb, tranabort);

PHP_METHOD(tokyocabinet_hdb, fsize);

PHP_METHOD(tokyocabinet_hdb, rnum);


//tokyocabinet_bdb
PHP_METHOD(tokyocabinet_bdb, __construct);

PHP_METHOD(tokyocabinet_bdb, put);

PHP_METHOD(tokyocabinet_bdb, putdup);

PHP_METHOD(tokyocabinet_bdb, get);

PHP_METHOD(tokyocabinet_bdb, sync);

PHP_METHOD(tokyocabinet_bdb, delete);

PHP_METHOD(tokyocabinet_bdb, tranbegin);

PHP_METHOD(tokyocabinet_bdb, trancommit);

PHP_METHOD(tokyocabinet_bdb, tranabort);

PHP_METHOD(tokyocabinet_bdb, fsize);

PHP_METHOD(tokyocabinet_bdb, rnum);

PHP_METHOD(tokyocabinet_bdb, vnum);

PHP_METHOD(tokyocabinet_bdb, curfirst);

PHP_METHOD(tokyocabinet_bdb, curkey);

PHP_METHOD(tokyocabinet_bdb, curval);

PHP_METHOD(tokyocabinet_bdb, curnext);

PHP_METHOD(tokyocabinet_bdb, curdel);

PHP_METHOD(tokyocabinet_bdb, curprev);

PHP_METHOD(tokyocabinet_bdb, curlast);

PHP_METHOD(tokyocabinet_bdb, curout);

PHP_METHOD(tokyocabinet_bdb, setcache);

PHP_METHOD(tokyocabinet_bdb, setxmsiz);


//tokyocabinet Table DB
PHP_METHOD(tokyocabinet_tdb, __construct);

PHP_METHOD(tokyocabinet_tdb, sync);

PHP_METHOD(tokyocabinet_tdb, rnum);

PHP_METHOD(tokyocabinet_tdb, fsize);

PHP_METHOD(tokyocabinet_tdb, tranabort);

PHP_METHOD(tokyocabinet_tdb, trancommit);

PHP_METHOD(tokyocabinet_tdb, tranbegin);

PHP_METHOD(tokyocabinet_tdb, makeid);

PHP_METHOD(tokyocabinet_tdb, put);

PHP_METHOD(tokyocabinet_tdb, get);

PHP_METHOD(tokyocabinet_tdb, delete);

PHP_METHOD(tokyocabinet_tdb, setcache);

PHP_METHOD(tokyocabinet_tdb, setxmsiz);

PHP_METHOD(tokyocabinet_tdb, qrydel);

PHP_METHOD(tokyocabinet_tdb, qrynew);

PHP_METHOD(tokyocabinet_tdb, qrysearchout);

PHP_METHOD(tokyocabinet_tdb, qrysetlimit);

PHP_METHOD(tokyocabinet_tdb, qrysetorder);

PHP_METHOD(tokyocabinet_tdb, qrysearch);

PHP_METHOD(tokyocabinet_tdb, qrysearch_count);

PHP_METHOD(tokyocabinet_tdb, qryaddcond);

PHP_METHOD(tokyocabinet_tdb, setindex);


/* True global resources - no need for thread safety here */
static int tokyocabinet_hdb_le, tokyocabinet_bdb_le, tokyocabinet_tdb_le;

static zend_class_entry      *tokyocabinet_hdb_ce;
static zend_class_entry      *tokyocabinet_bdb_ce;
static zend_class_entry      *tokyocabinet_tdb_ce;


static zend_object_handlers  tokyocabinet_hdb_object_handlers;
static zend_object_handlers  tokyocabinet_bdb_object_handlers;
static zend_object_handlers  tokyocabinet_tdb_object_handlers;


/* {{{ tokyocabinet_functions[]
 *
 * Every user visible function must have an entry in tokyocabinet_functions[].
 */


typedef struct _tokyocabinet_hdb_object {
	zend_object     zo;
	int             rc;
	TCHDB           *db;
} tokyocabinet_hdb_object;


typedef struct _tokyocabinet_bdb_object {
	zend_object     zo;
	int             rc;
	TCBDB           *db;
	BDBCUR          *cur;
} tokyocabinet_bdb_object;


typedef struct _tokyocabinet_tdb_object {
	zend_object     zo;
	int             rc;
	TCTDB           *db;
	TCLIST          *res;
	TDBQRY          *qry;
} tokyocabinet_tdb_object;


//tokyocabinet Hash DB
ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_hdb_construct, 0, 0, 1)
  ZEND_ARG_INFO(0, path)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_hdb_put, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_hdb_delete, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_hdb_get, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_hdb_setcache, 0, 0, 1)
  ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_hdb_setxmsiz, 0, 0, 1)
  ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_sync, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_iterinit, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_iternext, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_errmsg, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_tranbegin, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_trancommit, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_tranabort, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_fsize, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_hdb_rnum, 0)
ZEND_END_ARG_INFO()


zend_function_entry tokyocabinet_hdb_functions[] = {
	PHP_ME(tokyocabinet_hdb, __construct, arginfo_tokyocabinet_hdb_construct, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, put, arginfo_tokyocabinet_hdb_put, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, get, arginfo_tokyocabinet_hdb_get, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, sync, arginfo_tokyocabinet_hdb_sync, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, delete, arginfo_tokyocabinet_hdb_delete, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, iterinit, arginfo_tokyocabinet_hdb_iterinit, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, iternext, arginfo_tokyocabinet_hdb_iternext, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, setcache, arginfo_tokyocabinet_hdb_setcache, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, setxmsiz, arginfo_tokyocabinet_hdb_setxmsiz, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, errmsg, arginfo_tokyocabinet_hdb_errmsg, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, tranbegin, arginfo_tokyocabinet_hdb_tranbegin, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, trancommit, arginfo_tokyocabinet_hdb_trancommit, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, tranabort, arginfo_tokyocabinet_hdb_tranabort, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, fsize, arginfo_tokyocabinet_hdb_fsize, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_hdb, rnum, arginfo_tokyocabinet_hdb_rnum, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};




//tokyocabinet B+ Tree DB

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_sync, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_construct, 0, 0, 1)
  ZEND_ARG_INFO(0, path)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_put, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_putdup, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_delete, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_get, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_vnum, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_tranbegin, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_trancommit, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_tranabort, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_fsize, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_rnum, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curfirst, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curkey, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curval, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curnext, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curprev, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curlast, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curdel, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_bdb_curout, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_setcache, 0, 0, 1)
  ZEND_ARG_INFO(0, lcnum)
  ZEND_ARG_INFO(0, ncnum)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_bdb_setxmsiz, 0, 0, 1)
  ZEND_ARG_INFO(0, xmsiz)
ZEND_END_ARG_INFO()


zend_function_entry tokyocabinet_bdb_functions[] = {
	PHP_ME(tokyocabinet_bdb, __construct, arginfo_tokyocabinet_bdb_construct, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, put, arginfo_tokyocabinet_bdb_put, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, putdup, arginfo_tokyocabinet_bdb_putdup, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, get, arginfo_tokyocabinet_bdb_get, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, sync, arginfo_tokyocabinet_bdb_sync, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, delete, arginfo_tokyocabinet_bdb_delete, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curfirst, arginfo_tokyocabinet_bdb_curfirst, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curlast, arginfo_tokyocabinet_bdb_curlast, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curdel, arginfo_tokyocabinet_bdb_curdel, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curout, arginfo_tokyocabinet_bdb_curout, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curprev, arginfo_tokyocabinet_bdb_curprev, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curnext, arginfo_tokyocabinet_bdb_curnext, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curkey, arginfo_tokyocabinet_bdb_curkey, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, curval, arginfo_tokyocabinet_bdb_curval, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, tranbegin, arginfo_tokyocabinet_bdb_tranbegin, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, trancommit, arginfo_tokyocabinet_bdb_trancommit, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, tranabort, arginfo_tokyocabinet_bdb_tranabort, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, fsize, arginfo_tokyocabinet_bdb_fsize, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, rnum, arginfo_tokyocabinet_bdb_rnum, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, vnum, arginfo_tokyocabinet_bdb_vnum, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, setcache, arginfo_tokyocabinet_bdb_setcache, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_bdb, setxmsiz, arginfo_tokyocabinet_bdb_setxmsiz, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};



//tokyocabinet Table DB

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_tdb_construct, 0, 0, 1)
  ZEND_ARG_INFO(0, path)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_put, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, arr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_get, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_delete, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_sync, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_tranbegin, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_trancommit, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_tranabort, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_fsize, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_rnum, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_makeid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_qrydel, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_qrynew, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_qrysearchout, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_qrysetorder, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tokyocabinet_tdb_qrysetlimit, 0)
	ZEND_ARG_INFO(0, max_t)
	ZEND_ARG_INFO(0, skip)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_tdb_setcache, 0, 0, 1)
  ZEND_ARG_INFO(0, rcnum)
  ZEND_ARG_INFO(0, lcnum)
  ZEND_ARG_INFO(0, ncnum)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_tdb_setxmsiz, 0, 0, 1)
  ZEND_ARG_INFO(0, xmsiz)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_tdb_qrysearch, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_tdb_qrysearch_count, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_tdb_qryaddcond, 0, 0, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, op)
  ZEND_ARG_INFO(0, expr)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_tokyocabinet_tdb_setindex, 0, 0, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

zend_function_entry tokyocabinet_tdb_functions[] = {
	PHP_ME(tokyocabinet_tdb, __construct, arginfo_tokyocabinet_tdb_construct, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, makeid, arginfo_tokyocabinet_tdb_makeid, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, put, arginfo_tokyocabinet_tdb_put, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, delete, arginfo_tokyocabinet_tdb_delete, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, get, arginfo_tokyocabinet_tdb_get, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, sync, arginfo_tokyocabinet_tdb_sync, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, rnum, arginfo_tokyocabinet_tdb_rnum, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qrydel, arginfo_tokyocabinet_tdb_qrydel, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qrynew, arginfo_tokyocabinet_tdb_qrynew, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, tranbegin, arginfo_tokyocabinet_tdb_tranbegin, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, trancommit, arginfo_tokyocabinet_tdb_trancommit, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, tranabort, arginfo_tokyocabinet_tdb_tranabort, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, fsize, arginfo_tokyocabinet_tdb_fsize, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, setcache, arginfo_tokyocabinet_tdb_setcache, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, setxmsiz, arginfo_tokyocabinet_tdb_setxmsiz, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qrysearchout, arginfo_tokyocabinet_tdb_qrysearchout, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qrysetlimit, arginfo_tokyocabinet_tdb_qrysetlimit, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qrysetorder, arginfo_tokyocabinet_tdb_qrysetorder, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qrysearch, arginfo_tokyocabinet_tdb_qrysearch, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qrysearch_count, arginfo_tokyocabinet_tdb_qrysearch_count, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, qryaddcond, arginfo_tokyocabinet_tdb_qryaddcond, ZEND_ACC_PUBLIC)
	PHP_ME(tokyocabinet_tdb, setindex, arginfo_tokyocabinet_tdb_setindex, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};


zend_module_entry tokyocabinet_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"tokyocabinet",
	tokyocabinet_hdb_functions,
	PHP_MINIT(tokyocabinet),
	PHP_MSHUTDOWN(tokyocabinet),
	PHP_RINIT(tokyocabinet),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(tokyocabinet),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(tokyocabinet),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TOKYOCABINET
ZEND_GET_MODULE(tokyocabinet)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("tokyocabinet.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_tokyocabinet_globals, tokyocabinet_globals)
    STD_PHP_INI_ENTRY("tokyocabinet.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_tokyocabinet_globals, tokyocabinet_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_tokyocabinet_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_tokyocabinet_init_globals(zend_tokyocabinet_globals *tokyocabinet_globals)
{
	tokyocabinet_globals->global_value = 0;
	tokyocabinet_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */


static void class_init_tokyocabinet_hdb(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, "tokyocabinet_hdb", tokyocabinet_hdb_functions);
	tokyocabinet_hdb_ce = zend_register_internal_class(&ce TSRMLS_CC);
	tokyocabinet_hdb_ce->create_object = tokyocabinet_hdb_object_handler_new;
	
	memcpy(&tokyocabinet_hdb_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}


static void class_init_tokyocabinet_bdb(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, "tokyocabinet_bdb", tokyocabinet_bdb_functions);
	tokyocabinet_bdb_ce = zend_register_internal_class(&ce TSRMLS_CC);
	tokyocabinet_bdb_ce->create_object = tokyocabinet_bdb_object_handler_new;
	
	memcpy(&tokyocabinet_bdb_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}


static void class_init_tokyocabinet_tdb(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, "tokyocabinet_tdb", tokyocabinet_tdb_functions);
	tokyocabinet_tdb_ce = zend_register_internal_class(&ce TSRMLS_CC);
	tokyocabinet_tdb_ce->create_object = tokyocabinet_tdb_object_handler_new;
	
	memcpy(&tokyocabinet_tdb_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}

PHP_MINIT_FUNCTION(tokyocabinet)
{
	
	class_init_tokyocabinet_hdb(TSRMLS_C);

	class_init_tokyocabinet_bdb(TSRMLS_C);

	class_init_tokyocabinet_tdb(TSRMLS_C);
	
	
	//Hash
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBOWRITER", HDBOWRITER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBOREADER", HDBOREADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBOCREAT", HDBOCREAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBOTRUNC", HDBOTRUNC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBOTSYNC", HDBOTSYNC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBONOLCK", HDBONOLCK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBOREADER", HDBOREADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_HDB_HDBOLCKNB", HDBOLCKNB, CONST_CS | CONST_PERSISTENT);
	
	//B+ tree
	REGISTER_LONG_CONSTANT("TOKYOCABINET_BDB_BDBOWRITER", BDBOWRITER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_BDB_BDBOREADER", BDBOREADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_BDB_BDBOCREAT", BDBOCREAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_BDB_BDBOREADER", BDBOREADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_BDB_BDBOLCKNB", BDBOLCKNB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_BDB_BDBOTSYNC", BDBOTSYNC, CONST_CS | CONST_PERSISTENT);

	//Table Database
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBOWRITER", TDBOWRITER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBOCREAT",  TDBOCREAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBOREADER", TDBOREADER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBOTSYNC",  TDBOTSYNC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBONOLCK",  TDBONOLCK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBOTRUNC",  TDBOTRUNC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBOLCKNB",  TDBOLCKNB, CONST_CS | CONST_PERSISTENT);
	
	//Table Database cond
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTREQ",   TDBQCSTREQ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTRINC",  TDBQCSTRINC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTRBW",   TDBQCSTRBW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTREW",   TDBQCSTREW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTRAND",  TDBQCSTRAND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTROR",   TDBQCSTROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTROREQ", TDBQCSTROREQ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCSTRRX",   TDBQCSTRRX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNUMEQ",   TDBQCNUMEQ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNUMGT",   TDBQCNUMGT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNUMGE",   TDBQCNUMGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNUMLT",   TDBQCNUMLT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNUMLE",   TDBQCNUMLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNUMBT",   TDBQCNUMBT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNUMOREQ", TDBQCNUMOREQ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCFTSPH",   TDBQCFTSPH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCFTSAND",   TDBQCFTSAND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCFTSOR",   TDBQCFTSOR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCFTSEX",   TDBQCFTSEX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNEGATE",   TDBQCNEGATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQCNOIDX",   TDBQCNOIDX, CONST_CS | CONST_PERSISTENT);
	
	//Table Database order
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQOSTRASC",   TDBQOSTRASC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQOSTRDESC",  TDBQOSTRDESC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQONUMASC",   TDBQONUMASC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBQONUMDESC",   TDBQONUMDESC, CONST_CS | CONST_PERSISTENT);

	//Table Database setindex
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBITLEXICAL",   TDBITLEXICAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBITDECIMAL",   TDBITDECIMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBITTOKEN",   TDBITTOKEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBITVOID",   TDBITVOID, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBITKEEP",   TDBITKEEP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBITQGRAM",   TDBITQGRAM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TOKYOCABINET_TDB_TDBITOPT",   TDBITOPT, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}


/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(tokyocabinet)
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
PHP_RINIT_FUNCTION(tokyocabinet)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(tokyocabinet)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(tokyocabinet)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "tokyocabinet support", "enabled");
	php_info_print_table_row(2, "author", "lijinxing(qiye)");
	php_info_print_table_row(2, "version", "RC 1.0");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


static void tokyocabinet_hdb_object_free_storage(void *object TSRMLS_DC) 
{
	tokyocabinet_hdb_object *intern = (tokyocabinet_hdb_object*)object;
	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	
	if(!tchdbclose(intern->db))
	{
		intern->rc = tchdbecode(intern->db);
		zend_error (E_WARNING, "close error: %s", tchdberrmsg(intern->rc));
	}
	tchdbdel(intern->db);
	efree(object);
}

static zend_object_value tokyocabinet_hdb_object_handler_new(zend_class_entry *ce TSRMLS_DC) 
{
	zend_object_value retval;
	tokyocabinet_hdb_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(tokyocabinet_hdb_object));
	memset(intern, 0, sizeof(tokyocabinet_hdb_object));
	
	zend_object_std_init(&intern->zo, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
	zend_hash_copy(intern->zo.properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref, (void*)&tmp, sizeof(zval*));
#else
	object_properties_init((zend_object*) &intern->zo.properties, ce);
#endif

	retval.handle   = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)tokyocabinet_hdb_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &tokyocabinet_hdb_object_handlers;
	return retval;
}


PHP_METHOD(tokyocabinet_hdb, __construct) 
{
	zval *object = getThis();
	
	long flags;
	int path_len     = 0;
	const char *path = NULL;
	tokyocabinet_hdb_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &path, &path_len, &flags) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);

	intern->db = tchdbnew();

	if(!tchdbopen(intern->db, path, flags))
	{
		intern->rc = tchdbecode(intern->db);
		zend_error (E_WARNING, "open error: %s", tchdberrmsg(intern->rc));
		RETURN_FALSE;
	}
}


PHP_METHOD(tokyocabinet_hdb, put) 
{
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	const char *value = NULL;
	
	int key_len   = 0;
	int value_len = 0;

	tokyocabinet_hdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len, &value, &value_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tchdbput(intern->db, key, key_len, value, value_len);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, get) 
{
	zend_bool ret;
	zval *object = getThis();
	
	char       *value  = NULL;
	char       *result = NULL;
	const char *key    = NULL;
	int key_len        = 0;

	tokyocabinet_hdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	value  = tchdbget2(intern->db, key);

	if(value == NULL)  RETURN_FALSE; 

	result = estrdup(value);
	free(value);

    RETURN_STRING(result, 0);
}



PHP_METHOD(tokyocabinet_hdb, iterinit) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tchdbiterinit(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, iternext) 
{
	char *key, *value;
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	key    = tchdbiternext2(intern->db);
	if(key == NULL) RETURN_FALSE; 

	value = estrdup(key);
	free(key);

    RETURN_STRING(value, 0);
}



PHP_METHOD(tokyocabinet_hdb, setcache)
{
	int32_t   num;
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	int key_len       = 0;
	tokyocabinet_hdb_object *intern;
	

	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	num    = (int32_t)strtoul(key, 0, 10);
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	ret    = tchdbsetcache(intern->db, num);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, setxmsiz)
{
	int64_t   num;
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	int key_len       = 0;
	tokyocabinet_hdb_object *intern;
	

	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	num    = (int64_t)strtoull(key, NULL, 10);
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	ret    = tchdbsetxmsiz(intern->db, num);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, sync) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tchdbsync(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, tranbegin) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tchdbtranbegin(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, trancommit) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tchdbtrancommit(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, tranabort) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;

	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tchdbtranabort(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_hdb, rnum) 
{
	uint64_t rnum;
	char ret[256];
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	rnum  = tchdbrnum(intern->db);
	
	memset(&ret, 0, sizeof(ret));

	snprintf(ret, sizeof(ret), "%llu", rnum);

	RETURN_STRING(ret, 1);
}

PHP_METHOD(tokyocabinet_hdb, fsize) 
{
	uint64_t fsize;
	char ret[256];
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	fsize  = tchdbfsiz(intern->db);
	
	memset(&ret, 0, sizeof(ret));

	snprintf(ret, sizeof(ret), "%llu", fsize);

	RETURN_STRING(ret, 1);
}

PHP_METHOD(tokyocabinet_hdb, errmsg) 
{
	zval *object = getThis();

	tokyocabinet_hdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	intern->rc = tchdbecode(intern->db);

	RETURN_STRING(tchdberrmsg(intern->rc), 1);
}


PHP_METHOD(tokyocabinet_hdb, delete) 
{
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	int key_len       = 0;

	tokyocabinet_hdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_hdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tchdbout(intern->db, key, key_len);

	RETURN_BOOL(ret);
}


//btree
static void tokyocabinet_bdb_object_free_storage(void *object TSRMLS_DC) 
{
	tokyocabinet_bdb_object *intern = (tokyocabinet_bdb_object*)object;
	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	
	if(!tcbdbecode(intern->db))
	{
		intern->rc = tcbdbecode(intern->db);
		zend_error (E_WARNING, "close error: %s", tcbdberrmsg(intern->rc));
	}
	tcbdbdel(intern->db);
	
	if(intern->cur)tcbdbcurdel(intern->cur);
	efree(object);
}

static zend_object_value tokyocabinet_bdb_object_handler_new(zend_class_entry *ce TSRMLS_DC) 
{
	zend_object_value retval;
	tokyocabinet_bdb_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(tokyocabinet_bdb_object));
	memset(intern, 0, sizeof(tokyocabinet_bdb_object));
	
	intern->cur = NULL;
	zend_object_std_init(&intern->zo, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
	zend_hash_copy(intern->zo.properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref, (void*)&tmp, sizeof(zval*));
#else
	object_properties_init((zend_object*) &intern->zo.properties, ce);
#endif
	retval.handle   = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)tokyocabinet_bdb_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &tokyocabinet_bdb_object_handlers;
	return retval;
}


PHP_METHOD(tokyocabinet_bdb, __construct) 
{
	zval *object = getThis();
	
	long flags;
	int path_len     = 0;
	const char *path = NULL;
	tokyocabinet_bdb_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &path, &path_len, &flags) == FAILURE)
	{
		return;
	}
	
	intern = (tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);

	intern->db = tcbdbnew();

	if(!tcbdbopen(intern->db, path, flags))
	{
		intern->rc = tcbdbecode(intern->db);
		zend_error (E_WARNING, "open error: %s", tcbdberrmsg(intern->rc));
		RETURN_FALSE;
	}
}


PHP_METHOD(tokyocabinet_bdb, put) 
{
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	const char *value = NULL;
	
	int key_len   = 0;
	int value_len = 0;

	tokyocabinet_bdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len, &value, &value_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tcbdbput(intern->db, key, key_len, value, value_len);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, putdup) 
{
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	const char *value = NULL;
	
	int key_len   = 0;
	int value_len = 0;

	tokyocabinet_bdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len, &value, &value_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tcbdbputdup(intern->db, key, key_len, value, value_len);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_bdb, get) 
{
	zend_bool ret;
	zval *object = getThis();
	
	char       *value  = NULL;
	char       *result = NULL;
	const char *key    = NULL;
	int key_len        = 0;

	tokyocabinet_bdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	value  = tcbdbget2(intern->db, key);

	if(value == NULL)  RETURN_FALSE; 

	result = estrdup(value);
	free(value);

    RETURN_STRING(result, 0);
}


PHP_METHOD(tokyocabinet_bdb, vnum) 
{
	zend_bool ret;
	zval *object = getThis();
	
	long       len;
	const char *key    = NULL;
	int key_len        = 0;

	tokyocabinet_bdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	len    = tcbdbvnum(intern->db, key, key_len);

    RETURN_LONG(len);
}


PHP_METHOD(tokyocabinet_bdb, delete) 
{
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	int key_len       = 0;

	tokyocabinet_bdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tcbdbout(intern->db, key, key_len);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_bdb, sync) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tcbdbsync(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, tranbegin) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tcbdbtranbegin(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, trancommit) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tcbdbtrancommit(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, tranabort) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;

	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tcbdbtranabort(intern->db);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, rnum) 
{
	uint64_t rnum;
	char ret[256];
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	rnum  = tcbdbrnum(intern->db);
	
	memset(&ret, 0, sizeof(ret));

	snprintf(ret, sizeof(ret), "%llu", rnum);

	RETURN_STRING(ret, 1);
}

PHP_METHOD(tokyocabinet_bdb, fsize) 
{
	uint64_t fsize;
	char ret[256];
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	fsize  = tcbdbfsiz(intern->db);
	
	memset(&ret, 0, sizeof(ret));

	snprintf(ret, sizeof(ret), "%llu", fsize);

	RETURN_STRING(ret, 1);
}


PHP_METHOD(tokyocabinet_bdb, curfirst) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	intern->cur = tcbdbcurnew(intern->db);
	ret         = tcbdbcurfirst(intern->cur);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, curkey) 
{
	zend_bool ret;
	char       *value  = NULL;
	char       *result = NULL;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	value  = tcbdbcurkey2(intern->cur);
	if(value == NULL)  RETURN_FALSE; 

	result = estrdup(value);
	free(value);

    RETURN_STRING(result, 0);
}


PHP_METHOD(tokyocabinet_bdb, curnext) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret = tcbdbcurnext(intern->cur);
	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_bdb, curprev) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret = tcbdbcurprev(intern->cur);
	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_bdb, curlast) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret = tcbdbcurlast(intern->cur);
	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_bdb, curval) 
{
	zend_bool  ret;
	char       *value  = NULL;
	char       *result = NULL;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	value  = tcbdbcurval2(intern->cur);
	if(value == NULL)  RETURN_FALSE; 

	result = estrdup(value);
	free(value);

    RETURN_STRING(result, 0);
}

PHP_METHOD(tokyocabinet_bdb, curout) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret = tcbdbcurout(intern->cur);
	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, curdel) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_bdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	tcbdbcurdel(intern->cur);
	intern->cur = NULL;
	RETURN_TRUE;
}

PHP_METHOD(tokyocabinet_bdb, setcache)
{
	zend_bool ret;
	int32_t   lcnum_t, ncnum_t;
	zval *object = getThis();
	
	const char *lcnum   = NULL;
	const char *ncnum   = NULL;
	int lcnum_len       = 0;
	int ncnum_len       = 0;
	tokyocabinet_bdb_object *intern;
	

	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &lcnum, &lcnum_len, &ncnum, &ncnum_len) == FAILURE)
	{
		return;
	}
	
	lcnum_t = (int32_t)strtol(lcnum, 0, 10);
	ncnum_t = (int32_t)strtol(ncnum, 0, 10);
	intern  = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	ret     = tcbdbsetcache(intern->db, lcnum_t, ncnum_t);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_bdb, setxmsiz)
{
	int64_t   num;
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	int key_len       = 0;
	tokyocabinet_bdb_object *intern;
	

	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	num    = (int64_t)strtoull(key, NULL, 10);
	intern = ( tokyocabinet_bdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	ret    = tcbdbsetxmsiz(intern->db, num);

	RETURN_BOOL(ret);
}


//tokyocabinet Table Database

static void tokyocabinet_tdb_object_free_storage(void *object TSRMLS_DC) 
{
	tokyocabinet_tdb_object *intern = (tokyocabinet_tdb_object*)object;
	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	
	if(!tctdbclose(intern->db))
	{
		intern->rc = tctdbecode(intern->db);
		zend_error (E_WARNING, "close error: %s", tctdberrmsg(intern->rc));
	}
	tctdbdel(intern->db);
	
	if(intern->qry)tctdbqrydel(intern->qry);

	efree(object);
}

static zend_object_value tokyocabinet_tdb_object_handler_new(zend_class_entry *ce TSRMLS_DC) 
{
	zend_object_value retval;
	tokyocabinet_tdb_object *intern;
	zval *tmp;

	intern = emalloc(sizeof(tokyocabinet_tdb_object));
	memset(intern, 0, sizeof(tokyocabinet_tdb_object));
	
	intern->qry  = NULL;

	zend_object_std_init(&intern->zo, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
	zend_hash_copy(intern->zo.properties, &ce->default_properties, (copy_ctor_func_t)zval_add_ref, (void*)&tmp, sizeof(zval*));
#else
	object_properties_init((zend_object*) &intern->zo.properties, ce);
#endif

	retval.handle   = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)tokyocabinet_tdb_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &tokyocabinet_tdb_object_handlers;
	
	return retval;
}


PHP_METHOD(tokyocabinet_tdb, __construct) 
{
	zval *object = getThis();
	
	long flags;
	int path_len     = 0;
	const char *path = NULL;
	tokyocabinet_tdb_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &path, &path_len, &flags) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);


	intern->db = tctdbnew();

	if(!tctdbopen(intern->db, path, flags))
	{
		intern->rc = tctdbecode(intern->db);
		zend_error (E_WARNING, "open error: %s", tctdberrmsg(intern->rc));
		RETURN_FALSE;
	}
}


PHP_METHOD(tokyocabinet_tdb, makeid)
{
	zval *object = getThis();
	
	char result[25];
	struct timespec resolution; 
	tokyocabinet_tdb_object *intern;

	intern = (tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	clock_gettime(CLOCK_REALTIME, &resolution);
		
	memset(result, 0, sizeof(result));
	snprintf(result, sizeof(result), "%ld%ld", resolution.tv_sec, resolution.tv_nsec);
	
	RETURN_STRING(result, 1);
}

PHP_METHOD(tokyocabinet_tdb, put) 
{
	zval *object = getThis();
	
	int total, i, idx;

	zend_bool  ret;
	TCMAP      *cols;
	zval       *array_t;
	zval       **item_t;
	HashTable  *myht;
	char       *value, *key_t;
	int        key_t_len;

	char          *key;
    ulong         index;
    uint          key_len;
	HashPosition  pointer;

	tokyocabinet_tdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &key_t, &key_t_len, &array_t) == FAILURE)
	{
		return;
	}

	myht   = Z_ARRVAL_P(array_t);
	total = zend_hash_num_elements(myht);

	if(total < 1)
	{
		zend_error (E_WARNING, "Is Empty Array");
		RETURN_FALSE; 
	}
	
	cols = tcmapnew2(total+1);
	zend_hash_internal_pointer_reset_ex(myht, &pointer);
	for (;; zend_hash_move_forward_ex(myht, &pointer))
	{
		i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pointer);
		if (i == HASH_KEY_NON_EXISTANT) break;

		if (zend_hash_get_current_data_ex(myht, (void **) &item_t, &pointer) == SUCCESS)
		{
			if (Z_TYPE_PP(item_t) == IS_STRING) 
			{
				tcmapput2(cols, key, Z_STRVAL_PP(item_t));
			}
			else
			{
				tcmapdel(cols);
				zend_error (E_WARNING, "this array not is string");
				RETURN_FALSE; 
			}
		}
	}
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret  = tctdbput(intern->db, key_t, key_t_len, cols);
	tcmapdel(cols);

	RETURN_BOOL(ret);
	
}


PHP_METHOD(tokyocabinet_tdb, delete) 
{
	zval *object = getThis();
	
	zend_bool  ret;
	char       *value  = NULL;
	const char *key    = NULL;
	int key_len        = 0;

	tokyocabinet_tdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret  = tctdbout(intern->db, key, key_len);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_tdb, get) 
{
	zend_bool  ret;
	TCMAP      *cols;
	char       *key;
	const char *keyword;
	int key_len;

	zval *object       = getThis();

	tokyocabinet_tdb_object *intern;
	
	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}

	intern = (tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	cols   = tctdbget(intern->db, key, key_len);
	if(cols == NULL)  RETURN_FALSE; 

	array_init(return_value);
	
	tcmapiterinit(cols);
	while((keyword = tcmapiternext2(cols)) != NULL)
	{
		add_assoc_string(return_value, keyword ,tcmapget2(cols, keyword), 1); 
    }
	tcmapdel(cols);
}


PHP_METHOD(tokyocabinet_tdb, sync) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tctdbsync(intern->db);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_tdb, tranbegin) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tctdbtranbegin(intern->db);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_tdb, trancommit) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
       
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tctdbtrancommit(intern->db);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_tdb, tranabort) 
{
	zend_bool ret;
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;

	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret    = tctdbtranabort(intern->db);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_tdb, rnum) 
{
	uint64_t rnum;
	char ret[256];
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	rnum  = tctdbrnum(intern->db);
	
	memset(&ret, 0, sizeof(ret));

	snprintf(ret, sizeof(ret), "%llu", rnum);

	RETURN_STRING(ret, 1);
}

PHP_METHOD(tokyocabinet_tdb, fsize) 
{
	uint64_t fsize;
	char ret[256];
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	fsize  = tctdbfsiz(intern->db);
	
	memset(&ret, 0, sizeof(ret));

	snprintf(ret, sizeof(ret), "%llu", fsize);

	RETURN_STRING(ret, 1);
}

PHP_METHOD(tokyocabinet_tdb, setcache)
{
	zend_bool ret;
	
	zval *object = getThis();
	
	int32_t rcnum, lcnum, ncnum;
	char *rcnum_t, *lcnum_t, *ncnum_t;
	int rcnum_len, lcnum_len, ncnum_len;
	tokyocabinet_tdb_object *intern;
	

	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &rcnum_t, &rcnum_len, &lcnum_t, &lcnum_len, &ncnum_t, &ncnum_len) == FAILURE)
	{
		return;
	}
	
	rcnum   = (int32_t)strtol(rcnum_t, 0, 10);
	lcnum   = (int32_t)strtol(lcnum_t, 0, 10);
	ncnum   = (int32_t)strtol(ncnum_t, 0, 10);
	intern  = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	ret     = tctdbsetcache(intern->db, rcnum, lcnum, ncnum);

	RETURN_BOOL(ret);
}


PHP_METHOD(tokyocabinet_tdb, setxmsiz)
{
	int64_t   num;
	zend_bool ret;
	zval *object = getThis();
	
	const char *key   = NULL;
	int key_len       = 0;
	tokyocabinet_tdb_object *intern;
	

	if(ZEND_NUM_ARGS() != 1) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE)
	{
		return;
	}
	
	num    = (int64_t)strtoull(key, NULL, 10);
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	ret    = tctdbsetxmsiz(intern->db, num);

	RETURN_BOOL(ret);
}

PHP_METHOD(tokyocabinet_tdb, qrynew) 
{
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	if(intern->qry)
	{
		tctdbqrydel(intern->qry);
		intern->qry = NULL;
	}
	
	intern->res = NULL;
	intern->qry = tctdbqrynew(intern->db); 
}

PHP_METHOD(tokyocabinet_tdb, qrydel) 
{
	zval *object = getThis();

	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	if(intern->qry)
	{
		tctdbqrydel(intern->qry);
		intern->qry = NULL;
	}
	if(intern->res)
	{
		tclistdel(intern->res);
		intern->res = NULL;
	}
}


PHP_METHOD(tokyocabinet_tdb, qrysetorder) 
{
	zval *object = getThis();

	long flags;
	int name_len     = 0;
	const char *name = NULL;
	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_len, &flags) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	tctdbqrysetorder(intern->qry, name, flags);
}


PHP_METHOD(tokyocabinet_tdb, qrysetlimit) 
{
	zval *object = getThis();

	long max_t, skip;
	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &max_t, &skip) == FAILURE)
	{
		return;
	}
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	tctdbqrysetlimit(intern->qry, max_t, skip);
}

PHP_METHOD(tokyocabinet_tdb, qrysearchout) 
{
	zval *object = getThis();
	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	tctdbqrysearchout(intern->qry);
}


PHP_METHOD(tokyocabinet_tdb, qryaddcond) 
{
	zval *object = getThis();
	const char *name, *expr;
	long name_len, expr_len, op;
	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 3) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sls", &name, &name_len, &op, &expr, &expr_len) == FAILURE)
	{
		return;
	}

	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	tctdbqryaddcond(intern->qry, name, op, expr);
}


PHP_METHOD(tokyocabinet_tdb, qrysearch_count) 
{
	TCMAP  *cols;
	zval   *line;
	int i, num, rsiz;
	const char *rbuf, *name;

	zval *object = getThis();
	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	if(intern->res)
	{
		num = tclistnum(intern->res);
		RETURN_LONG(num);
	}
	else
	{
		intern->res = tctdbqrysearch(intern->qry);
		num = tclistnum(intern->res);

		RETURN_LONG(num);
	}
}


PHP_METHOD(tokyocabinet_tdb, qrysearch) 
{
	TCMAP  *cols;
	zval   *line;
	int i, num, rsiz;
	const char *rbuf, *name;

	zval *object = getThis();
	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 0) WRONG_PARAM_COUNT;
	
	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	if(!intern->res)
	{
		intern->res = tctdbqrysearch(intern->qry);
	}
	
	num = tclistnum(intern->res);
	if(num < 1)
	{
		tclistdel(intern->res);
		tctdbqrydel(intern->qry);
		RETURN_FALSE; 
	}

	if (array_init(return_value) == FAILURE) 
	{
         RETURN_FALSE;
    }
	for(i = 0; i < num; i++)
	{
		rbuf = tclistval(intern->res, i, &rsiz);
		if(rbuf == NULL) continue;
		cols = tctdbget(intern->db, rbuf, rsiz);
		if(cols)
		{
			MAKE_STD_ZVAL(line);
			if (array_init(line) == FAILURE) 
			{
				RETURN_FALSE;
			}

			tcmapiterinit(cols);
			while((name = tcmapiternext2(cols)) != NULL)
			{
				add_assoc_string(line, name, tcmapget2(cols, name), 1); 
			}

			add_index_zval(return_value, i, line);
			tcmapdel(cols);
		}
	}
}


PHP_METHOD(tokyocabinet_tdb, setindex) 
{
	zval *object = getThis();
	
	zend_bool  ret;
	const char *name;
	long name_len, type;
	tokyocabinet_tdb_object *intern;
	
	if (ZEND_NUM_ARGS() != 2) WRONG_PARAM_COUNT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_len, &type) == FAILURE)
	{
		return;
	}

	intern = ( tokyocabinet_tdb_object *)zend_object_store_get_object(object TSRMLS_CC);
	
	ret = tctdbsetindex(intern->db, name, type);

	RETURN_BOOL(ret);
}

