#ifndef H_QC_ERROR
#define H_QC_ERROR

#include "qc_prelude.h"


#define QC_SUCCEED    0
#define QC_ERR_BADPARAM  -10
#define QC_ERR_BADLENGTH -11
#define QC_ERR_RUNTIME   -20
#define QC_ERR_SOCKET    -30
#define QC_ERR_QUEUE_NOTEXIST -40
#define QC_ERR_INTERNAL  -50

#define QC_MAXSIZE_ERRDESC 255


typedef struct __QcErr{
    int  code;
    char desc[QC_MAXSIZE_ERRDESC+1];
}QcErr;



#ifdef __cplusplus
extern "C" {
#endif


int qc_seterr(QcErr *err, int errcode, const char *fmt, ...);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_ERROR*/

