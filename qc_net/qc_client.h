#ifndef QC_CLIENT_H
#define QC_CLIENT_H

#include "qc_socket.h"
#include "qc_message.h"
#include "qc_error.h"



struct __QcClient {
	QcSocket *socket;
};


typedef struct __QcClient QcClient;



QcClient* qc_producer_connect(const char *ip, int port, const char *qname, QcErr *err);

void qc_producer_disconnect(QcClient *client);

int qc_producer_msgput(QcClient *client, QcMessage *message, int msec, QcErr *err);


QcClient* qc_consumer_connect(const char *ip, int port, const char *qname, QcErr *err);

void qc_consumer_disconnect(QcClient *client);

QcMessage* qc_consumer_msgget(QcClient *client, int msec, QcErr *err);


#endif /*QC_CLIENT_H*/
