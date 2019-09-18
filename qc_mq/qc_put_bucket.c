#include "qc_put_bucket.h"


QcPutBucket* qc_putbucket_create() {

	QcPutBucket *putBucket;
	qc_malloc(putBucket, sizeof(QcPutBucket));
	if (NULL == putBucket) {
		return NULL;
	}
	QcList *putterList = qc_list_create(0);
	if (NULL == putterList) {
		qc_free(putBucket);
		return NULL;
	}

	putBucket->putterList = putterList;
	return putBucket;
}


int qc_putbucket_destroy(QcPutBucket *putBucket) {
	if (NULL == putBucket) {
		return -1;
	}

	if (putBucket->putterList) qc_list_destroy(putBucket->putterList);
	qc_free(putBucket);
	return 0;
}

