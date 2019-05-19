#include "qc_error.h"



int qc_seterr(QcErr *err, int errcode, const char *fmt, ...){
    if(NULL == err)
        return -1;
    
    memset(err, 0, sizeof(QcErr));
    err->code = errcode;

    char vs[QC_MAXSIZE_ERRDESC];
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    strncpy(err->desc, vs, sizeof(vs));
    va_end(vl);

    return 0;
}
