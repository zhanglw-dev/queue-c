#ifndef QC_LOG_H
#define QC_LOG_H

#include "qc_prelude.h"


#define DEBUG_LEVEL1 1
#define DEBUG_LEVEL2 2
#define DEBUG_LEVEL3 3
#define DEBUG_LEVEL4 4
#define DEBUG_LEVEL5 5
#define DEBUG_LEVEL6 6


#define qc_error(str) \
        _qc_error(__FILE__, __LINE__, (const char *)str)

#define qc_perror(fmt, ...) \
        _qc_perror(__FILE__, __LINE__, (const char *)(fmt), __VA_ARGS__)

#define qc_debug(level, str) \
        _qc_debug(__FILE__, __LINE__, level, (const char *)str)

#define qc_pdebug(level, fmt, ...) \
        _qc_pdebug(__FILE__, __LINE__, level, (const char *)(fmt), __VA_ARGS__)

#define qc_warn(str) \
        _qc_warn((const char *)str)

#define qc_pwarn(fmt, ...) \
        _qc_pwarn((const char *)(fmt), __VA_ARGS__)

#define qc_info(str) \
        _qc_info((const char *)str)

#define qc_pinfo(fmt, ...) \
        _qc_pinfo((const char *)(fmt), __VA_ARGS__)


void _qc_error(char *srcfile, int srcline, const char *str);

void _qc_perror(char *srcfile, int srcline, const char *fmt, ...);
  
void _qc_debug(char *srcfile, int srcline, int level, const char *str);

void _qc_pdebug(char *srcfile, int srcline, int level, const char *fmt, ...);

void _qc_warn(const char *str);

void _qc_pwarn(const char *fmt, ...);

void _qc_info(const char *str);

void _qc_pinfo(const char *fmt, ...);


#endif  /*QC_LOG_H*/