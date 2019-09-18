#ifndef H_QC_MESSAGE
#define H_QC_MESSAGE

#include "qc_prelude.h"


#pragma pack(push)
#pragma pack(1)

struct __QcMessage {
	int priority;
	int pool_flag;
	unsigned int pool_idx;
	int getter_holding;
	int bufflen;
	char *buff;
};

#pragma pack(pop)


typedef struct __QcMessage QcMessage;



#ifdef __cplusplus
extern "C" {
#endif


QcMessage* qc_message_create(const char *buff, int len, int flag_dupbuff);

void qc_message_release(QcMessage *message, int flag_freebuff);

const char* qc_message_buff(QcMessage *message);

int qc_message_bufflen(QcMessage *message);

void qc_message_setpriority(QcMessage *message, int priority);

int qc_message_priority(QcMessage *message);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_MESSAGE*/
