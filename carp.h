#ifndef __CARP_H__
#define __CARP_H__

#include <errno.h>

#define warn(...)      warn_at_loc     (__FILE__, __FUNCTION__, __LINE__, 0,     __VA_ARGS__);
#define ewarn(...)     warn_at_loc     (__FILE__, __FUNCTION__, __LINE__, errno, __VA_ARGS__);
#define die(...)       die_at_loc      (__FILE__, __FUNCTION__, __LINE__, 0,     __VA_ARGS__);
#define edie(...)      die_at_loc      (__FILE__, __FUNCTION__, __LINE__, errno, __VA_ARGS__);
#define carp(...)      carp_at_loc     (__FILE__, __FUNCTION__, __LINE__, 0,     __VA_ARGS__);
#define ecarp(...)     carp_at_loc     (__FILE__, __FUNCTION__, __LINE__, errno, __VA_ARGS__);
#define croak(...)     croak_at_loc    (__FILE__, __FUNCTION__, __LINE__, 0,     __VA_ARGS__);
#define ecroak(...)    croak_at_loc    (__FILE__, __FUNCTION__, __LINE__, errno, __VA_ARGS__);
#define cluck(...)     cluck_at_loc    (__FILE__, __FUNCTION__, __LINE__, 0,     __VA_ARGS__);
#define ecluck(...)    cluck_at_loc    (__FILE__, __FUNCTION__, __LINE__, errno, __VA_ARGS__);
#define confess(...)   confess_at_loc  (__FILE__, __FUNCTION__, __LINE__, 0,     __VA_ARGS__);
#define econfess(...)  confess_at_loc  (__FILE__, __FUNCTION__, __LINE__, errno, __VA_ARGS__);

void warn_at_loc     (const char *file, const char *func, int line, int errnum, const char *mesg, ...);
void die_at_loc      (const char *file, const char *func, int line, int errnum, const char *mesg, ...);
void carp_at_loc     (const char *file, const char *func, int line, int errnum, const char *mesg, ...);
void croak_at_loc    (const char *file, const char *func, int line, int errnum, const char *mesg, ...);
void cluck_at_loc    (const char *file, const char *func, int line, int errnum, const char *mesg, ...);
void confess_at_loc  (const char *file, const char *func, int line, int errnum, const char *mesg, ...);

#endif

