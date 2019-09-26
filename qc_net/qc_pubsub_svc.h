#ifndef QC_PUBSUB_SVC_H
#define QC_PUBSUB_SVC_H

#include "qc_list.h"
#include "qc_thread.h"
#include "qc_error.h"
#include "qc_pubsub_hdl.h"
#include "qc_socket.h"



struct __QcTopicSock {
	QcSocket *socket;
};

typedef struct __QcTopicSock QcTopicSock;


struct __QcPubSubSvc {
	char svc_ip[256];
	int svc_port;

	QcThread *poll_thread;
	QcList *topicProcList;

	QcHashTbl *topicSockTable;

	QcPublishHdl *publishHdl;
	QcSubscribHdl *subscribHdl;
};

typedef struct __QcPubSubSvc QcPubSubSvc;



QcPubSubSvc* qc_pubsubsvc_create(const char* ip, int port, QcQSystem *qSystem, QcErr *err);

void qc_pubsubsvc_destory(QcPubSubSvc *pubsubSvc);

int qc_pubsubsvc_addtopic(QcPubSubSvc *pubsubSvc, const char *qname);

int qc_pubsubsvc_start(QcPubSubSvc *pubsubSvc, QcErr *err);

void qc_pubsubsvc_stop(QcPubSubSvc *pubsubSvc);



#endif /*QC_PUBSUB_SVC_H*/
