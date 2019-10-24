#include "qc_putters.h"
#include "qc_log.h"


QcPutter* qc_putter_create() {
	QcPutter *putter;

	qc_malloc(putter, sizeof(QcPutter));
	if (NULL == putter) {
		qc_error("malloc QcPutter faild.");
		return NULL;
	}

	putter->cond = qc_thread_cond_create();
	if (NULL == putter->cond) {
		qc_error("create putter->cond failed.");
		qc_free(putter);
		return NULL;
	}

	putter->condlock = qc_thread_condlock_create();
	if (NULL == putter->condlock) {
		qc_error("create putter->condlock failed.");
		qc_free(putter);
		return NULL;
	}

	putter->message = NULL;
	putter->is_timedout = 0;
	putter->priority = 0;

	return putter;
}


int qc_putter_destroy(QcPutter *putter) {
	if (NULL == putter) {
		qc_error("invalid params.");
		return -1;
	}

	qc_thread_cond_destroy(putter->cond);
	qc_thread_condlock_destroy(putter->condlock);

	qc_free(putter);

	return 0;
}


//--------------------------------------------------------------------------------------------------------

QcPutBucket* qc_putbucket_create() {

	QcPutBucket *putBucket;
	qc_malloc(putBucket, sizeof(QcPutBucket));
	if (NULL == putBucket) {
		return NULL;
	}
	QcList *puttersChain = qc_list_create(0);
	if (NULL == puttersChain) {
		qc_free(putBucket);
		return NULL;
	}

	putBucket->puttersChain = puttersChain;
	return putBucket;
}


int qc_putbucket_destroy(QcPutBucket *putBucket) {
	if (NULL == putBucket) {
		return -1;
	}

	if (putBucket->puttersChain) qc_list_destroy(putBucket->puttersChain);
	qc_free(putBucket);
	return 0;
}

//--------------------------------------------------------------------------------

QcPuttersChain* qc_putterschain_create(int bucket_count) {
	QcPuttersChain *putterChain;
	qc_malloc(putterChain, sizeof(QcPuttersChain));
	if (NULL == putterChain) {
		return NULL;
	}

	putterChain->bucket_count = bucket_count;
	putterChain->cursor_bucketsn = 0;
	putterChain->putter_count = 0;

	qc_malloc(putterChain->putBuckets, sizeof(QcPutBucket*)*putterChain->bucket_count);
	if (NULL == putterChain->putBuckets) {
		return NULL;
	}
	memset(putterChain->putBuckets, 0, sizeof(QcPuttersChain*)*putterChain->bucket_count);

	for (int i = 0; i < putterChain->bucket_count; i++) {
		QcPutBucket *putBucket = qc_putbucket_create();
		if (NULL == putBucket) {
			goto failed;
		}
		putterChain->putBuckets[i] = putBucket;
	}

	return putterChain;

failed:
	qc_putterschain_destroy(putterChain);
	return NULL;
}


int qc_putterschain_destroy(QcPuttersChain *putterChain) {
	if (NULL == putterChain) {
		return -1;
	}

	if (putterChain->putBuckets) {
		for (int i = 0; i < putterChain->bucket_count; i++) {
			if (putterChain->putBuckets[i])
				qc_free(putterChain->putBuckets[i]);
		}
		qc_free(putterChain->putBuckets);
	}

	qc_free(putterChain);

	return 0;
}


int qc_putterschain_push(QcPuttersChain *putterChain, QcPutter *putter) {

	qc_assert(NULL != putterChain && NULL != putter);
	qc_assert(putter->message);

	QcListEntry *listEntry;

	int bucket_sn = putter->priority - 1;
	int ret = qc_list_inserttail2(putterChain->putBuckets[bucket_sn]->puttersChain, putter, &listEntry);
	qc_assert(ret == 0);

	putter->_entry = listEntry;

	putterChain->putter_count++;

	if (bucket_sn > putterChain->cursor_bucketsn)
		putterChain->cursor_bucketsn = bucket_sn;

	return 0;
}


QcPutter* qc_putterschain_pop(QcPuttersChain *putterChain) {

	if (putterChain->putter_count == 0)
		return NULL;

	int bucket_sn = putterChain->cursor_bucketsn;

	QcPutBucket *bucket;  //must before loop??

	//locate the cursor_bucketsn
	while (1) {
		bucket = putterChain->putBuckets[bucket_sn];
		if (qc_list_count(bucket->puttersChain) > 0) {
			putterChain->cursor_bucketsn = bucket_sn;
			break;
		}
		bucket_sn--;
		qc_assert(bucket_sn >= 0);
	}

	int cycle = 0;
try_loop:

	bucket = putterChain->putBuckets[bucket_sn];
	int counter_max = bucket_sn + 1;

	QcPutter *putter;

	if (bucket->pop_counter < counter_max) {
		if (qc_list_count(bucket->puttersChain) > 0) {
			putter = qc_list_pophead(putterChain->putBuckets[bucket_sn]->puttersChain);
			bucket->pop_counter++;
			putterChain->putter_count--;
			return putter;
		}
		else {
			bucket_sn--;
			qc_assert(bucket_sn >= 0);
			goto try_loop;
		}
	}
	else {
		//no message in other bucket...
		if (qc_list_count(putterChain->putBuckets[bucket_sn]->puttersChain) == putterChain->putter_count) {
			putter = qc_list_pophead(bucket->puttersChain);
			putterChain->putter_count--;
			return putter;
		}

		bucket_sn--;
		if (bucket_sn < 0) {
			bucket_sn = putterChain->bucket_count - 1;
			qc_assert(cycle == 0);  //be sure just loop one cycle
			cycle++;
		}
		bucket->pop_counter = 0;
		goto try_loop;
	}

	return NULL;
}


int qc_putterschain_remove(QcPuttersChain *putterChain, QcPutter *putter) {
	qc_assert(NULL != putterChain && NULL != putter);

	QcListEntry *listEntry = putter->_entry;
	int bucket_sn = putter->priority - 1;
	int ret = qc_list_removeentry(putterChain->putBuckets[bucket_sn]->puttersChain, listEntry);
	qc_assert(ret == 0);

	putterChain->putter_count--;
	return 0;
}

