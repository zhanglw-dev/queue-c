#ifndef QC_CLIENT_H
#define QC_CLIENT_H

#include "qc_message.h"
#include "qc_error.h"


typedef struct __QcClient QcClient;


QcClient* qc_client_connect(const char *ip, int port, const char *qname, QcErr *err);

void qc_client_disconnect(QcClient *client);

int qc_client_msgput(QcClient *client, const char *qname, QcMessage *message, int msec, QcErr *err);

QcMessage* qc_consumer_msgget(QcClient *client, const char *qname, int msec, QcErr *err);


#endif /*QC_CLIENT_H*/
