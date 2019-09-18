#include "qc_msg_bucket.h"


QcMsgBucket* qc_msgbucket_create() {

	QcMsgBucket *msgBucket;
	qc_malloc(msgBucket, sizeof(QcMsgBucket));
	if (NULL == msgBucket) {
		return NULL;
	}
	QcList *msgList = qc_list_create(0);
	if (NULL == msgList) {
		qc_free(msgBucket);
		return NULL;
	}

	msgBucket->msgList = msgList;
	return msgBucket;
}


int qc_msgbucket_destroy(QcMsgBucket *msgBucket) {
	if (NULL == msgBucket) {
		return -1;
	}

	if (msgBucket->msgList) qc_list_destroy(msgBucket->msgList);
	qc_free(msgBucket);
	return 0;
}
