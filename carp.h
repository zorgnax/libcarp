#ifndef __CARP_H__
#define __CARP_H__

#include <stdio.h>
#include <errno.h>

#define warn(...)       warn_at_loc      (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define ewarn(...)      warn_at_loc      (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define die(...)        die_at_loc       (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define edie(...)       die_at_loc       (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define carp(...)       carp_at_loc      (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define ecarp(...)      carp_at_loc      (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define croak(...)      croak_at_loc     (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define ecroak(...)     croak_at_loc     (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define cluck(...)      cluck_at_loc     (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define ecluck(...)     cluck_at_loc     (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define confess(...)    confess_at_loc   (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define econfess(...)   confess_at_loc   (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define swarn(...)      swarn_at_loc     (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define eswarn(...)     swarn_at_loc     (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define sdie(...)       sdie_at_loc      (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define esdie(...)      sdie_at_loc      (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define scarp(...)      scarp_at_loc     (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define escarp(...)     scarp_at_loc     (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define scroak(...)     scroak_at_loc    (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define escroak(...)    scroak_at_loc    (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define scluck(...)     scluck_at_loc    (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define escluck(...)    scluck_at_loc    (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)
#define sconfess(...)   sconfess_at_loc  (__FILE__, __FUNCTION__, __LINE__, 0,     ## __VA_ARGS__, NULL)
#define esconfess(...)  sconfess_at_loc  (__FILE__, __FUNCTION__, __LINE__, errno, ## __VA_ARGS__, NULL)

#ifdef _WIN32
#   define CARP_EXP __declspec(dllexport)
#else
#   define CARP_EXP
#endif

CARP_EXP void  warn_at_loc      (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP void  die_at_loc       (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP void  carp_at_loc      (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP void  croak_at_loc     (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP void  cluck_at_loc     (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP void  confess_at_loc   (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP char *swarn_at_loc     (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP char *sdie_at_loc      (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP char *scarp_at_loc     (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP char *scroak_at_loc    (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP char *scluck_at_loc    (const char *file, const char *func, int line, int errnum, const char *fmt, ...);
CARP_EXP char *sconfess_at_loc  (const char *file, const char *func, int line, int errnum, const char *fmt, ...);

typedef void (*CarpOutputFunc) (const char *mesg);
CARP_EXP void carp_set (const char *key, ...);

#endif

