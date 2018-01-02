#ifndef __SDB_CONFIG_H__
#define __SDB_CONFIG_H__

/** Global switch */
#define SDB_ENABLE

/**
 * \block:      Modules
 * @{ */
/* #define SDB_MDL_GET */
/* #define SDB_MDL_DUMP */
/* #define SDB_MDL_MENU */
/* #define SDB_MDL_SELFTEST */

/* #define SDB_STACK_WATCH */
/** @} */

/**
 * \block:      Platform environment
 * @{ */
/**
 * Display text with color.
 * Need ANSI sequences support.
 */
#define SDB_ENV_ANSI_SEQUENCES

/**
 * Assign standard functions in base I/O as default.
 * Require <stdio.h>: printf(), getchar()
 */
#define SDB_ENV_STDIO

/**
 * Display standard error messages.
 * Require <errno.h>: stderror(), errno
 */
#define SDB_ENV_STDERR

/** size_t */
#define SDB_ENV_SIZE_T

/** long */
#define SDB_ENV_LONG

/** long long */
#define SDB_ENV_LONG_LONG
/** @} */

/**
 * \block:      Configurations
 * @{ */
/** Size of internal buffer for put a line of message. */
#define SDB_CONF_BUFFER_SIZE    288

/** Size of internal buffer for get a string of number. */
#define SDB_CONF_BUFFER_SIZE_GETNUM 32

/** Color definition */
#define SDB_COLOR_RES           "\33[0m"    /* Resume */
#define SDB_COLOR_WARN          "\33[1;36m" /* Warning messages */
#define SDB_COLOR_ERR           "\33[1;31m" /* Error messages */
#define SDB_COLOR_INPUT         "\33[1;32m" /* Input request */
#define SDB_COLOR_HL            "\33[1m"    /* High light, bold */
#define SDB_COLOR_TITLE         "\33[7m"    /* Title, inverse */
#define SDB_COLOR_LABEL         "\33[1;30m" /* Label, dark, bold */

/* Mark definition */
#define SDB_MARK_NONE           "    "      /* None */
#define SDB_MARK_INFO           " .  "      /* Prompt */
#define SDB_MARK_WARN           " !  "      /* Warning */
#define SDB_MARK_ERR            " x  "      /* Error */
#define SDB_MARK_GETNUM         "I.n "      /* Input, get number */
#define SDB_MARK_GETSTR         "I.s "      /* Input, get string */
#define SDB_MARK_ECHO           "I.e "      /* Echo of input */
#define SDB_MARK_DUMP           "  | "      /* Dump */
/** @} */

/**
 * Version format: 0x00AAIISS.
 *      A:Major
 *      I:Minor
 *      S:Status, 0 .. 0xFE:Beta, 0xFF:Release
 */
#define SDB_VERSION_NUMBER      0x00020001

#include "libsdb.h"
#include "sdb_internal.h"

#endif /* __SDB_CONFIG_H__ */

