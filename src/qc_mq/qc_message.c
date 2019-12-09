#include "qc_message.h"


#pragma pack(push)
#pragma pack(1)

//must as same as qc_persist_file.c::Qc_MsgEqual !!!
struct __QcMessage {
	int priority;
	int persist_id;
	int bufflen;
	char *buff;
};

#pragma pack(pop)



QcMessage* qc_message_create(const char *buff, int len, int flag_dupbuff){
    QcMessage *message;

    qc_malloc(message, sizeof(QcMessage));
    if(NULL == message)
        return NULL;
    
    if(NULL == (char *)buff) len = 0;

    if(flag_dupbuff){
        qc_malloc(message->buff, len);
        if(NULL == message->buff){
            qc_free(message);
            return NULL;
        }
        memcpy(message->buff, buff, len);
    }
    else{
        message->buff = (char *)buff;
    }

    message->bufflen = len;
    message->priority = 1;

    return message;
}


void qc_message_release(QcMessage *message, int flag_freebuff){
    if(NULL == message)
        return;

    if(flag_freebuff && message->buff) 
		qc_free(message->buff);

    qc_free(message);

    return;
}


char* qc_message_buff(QcMessage *message){
    qc_assert(message);
    return message->buff;
}


int qc_message_bufflen(QcMessage *message){
    qc_assert(message);    
    return message->bufflen;
}


void qc_message_setpriority(QcMessage *message, int priority){
    qc_assert(message);
    qc_assert(priority>=0)

    message->priority = priority;
}


int qc_message_priority(QcMessage *message){
    qc_assert(message);
    return message->priority;
}

