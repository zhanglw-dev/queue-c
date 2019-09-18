#include "qc_log.h"
#include "qc_putter_chain.h"


//--------------------------------------------------------------------------------

QcPutterChain* qc_putterchain_create(int bucket_count){
	QcPutterChain *putterList;
	qc_malloc(putterList, sizeof(QcPutterChain));
    if(NULL == putterList){
        return NULL;
    }

	putterList->bucket_count = bucket_count;
	putterList->cursor_bucketsn = 0;
	putterList->putter_count = 0;

	qc_malloc(putterList->putBuckets, sizeof(QcPutBucket*)*putterList->bucket_count);
	if (NULL == putterList->putBuckets) {
		return NULL;
	}
	memset(putterList->putBuckets, 0, sizeof(QcPutterChain*)*putterList->bucket_count);

	for (int i = 0; i < putterList->bucket_count; i++) {
		QcPutBucket *putBucket = qc_putbucket_create();
		if (NULL == putBucket) {
			goto failed;
		}
		putterList->putBuckets[i] = putBucket;
	}

    return putterList;

failed:
	qc_putterchain_destroy(putterList);
	return NULL;
}


int qc_putterchain_destroy(QcPutterChain *putterList){
    if(NULL == putterList){
        return -1;
    }

	if (putterList->putBuckets) {
		for (int i = 0; i < putterList->bucket_count; i++) {
			if (putterList->putBuckets[i])
				qc_free(putterList->putBuckets[i]);
		}
		qc_free(putterList->putBuckets);
	}

    qc_free(putterList);

    return 0;
}


int qc_putterchain_push(QcPutterChain *putterList, QcPutter *putter){

    qc_assert(NULL!=putterList && NULL!=putter);
	qc_assert(putter->message);

    QcListEntry *listEntry;

	int bucket_sn = putter->priority - 1;
    int ret = qc_list_inserttail2(putterList->putBuckets[bucket_sn]->putterList, putter, &listEntry);
    qc_assert(ret == 0);

    putter->_entry = listEntry;

	putterList->putter_count++;

	if (bucket_sn > putterList->cursor_bucketsn)
		putterList->cursor_bucketsn = bucket_sn;

    return 0;
}


QcPutter* qc_putterchain_pop(QcPutterChain *putterList){

	if (putterList->putter_count == 0)
		return NULL;

	int bucket_sn = putterList->cursor_bucketsn;

	QcPutBucket *bucket;  //must before loop??

	//locate the cursor_bucketsn
	while (1) {
		bucket = putterList->putBuckets[bucket_sn];
		if (qc_list_count(bucket->putterList) > 0) {
			putterList->cursor_bucketsn = bucket_sn;
			break;
		}
		bucket_sn--;
		qc_assert(bucket_sn >= 0);
	}

	int cycle = 0;
try_loop:

	bucket = putterList->putBuckets[bucket_sn];
	int counter_max = bucket_sn + 1;

	QcPutter *putter;

	if (bucket->pop_counter < counter_max) {
		if (qc_list_count(bucket->putterList) > 0) {
			putter = qc_list_pophead(putterList->putBuckets[bucket_sn]->putterList);
			bucket->pop_counter++;
			putterList->putter_count--;
			return putter;
		}
		else {
			bucket_sn--;
			qc_assert(bucket_sn >= 0);
			goto try_loop;
		}
	}
	else {
		bucket_sn--;
		if (bucket_sn < 0) {
			bucket_sn = putterList->bucket_count - 1;
			qc_assert(cycle == 0);  //be sure just loop one cycle
			cycle++;
		}
		bucket->pop_counter = 0;
		goto try_loop;
	}

    return NULL;
}


int qc_putterchain_remove(QcPutterChain *putterList, QcPutter *putter){
    qc_assert(NULL!=putterList && NULL!=putter);

    QcListEntry *listEntry = putter->_entry;
	int bucket_sn = putter->priority - 1;
    int ret = qc_list_removeentry(putterList->putBuckets[bucket_sn]->putterList, listEntry);
    qc_assert(ret == 0);

	putterList->putter_count--;
    return 0;
}

