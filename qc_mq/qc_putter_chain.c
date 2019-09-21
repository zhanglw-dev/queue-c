#include "qc_log.h"
#include "qc_putter_chain.h"


//--------------------------------------------------------------------------------

QcPutterChain* qc_putterchain_create(int bucket_count){
	QcPutterChain *putterChain;
	qc_malloc(putterChain, sizeof(QcPutterChain));
    if(NULL == putterChain){
        return NULL;
    }

	putterChain->bucket_count = bucket_count;
	putterChain->cursor_bucketsn = 0;
	putterChain->putter_count = 0;

	qc_malloc(putterChain->putBuckets, sizeof(QcPutBucket*)*putterChain->bucket_count);
	if (NULL == putterChain->putBuckets) {
		return NULL;
	}
	memset(putterChain->putBuckets, 0, sizeof(QcPutterChain*)*putterChain->bucket_count);

	for (int i = 0; i < putterChain->bucket_count; i++) {
		QcPutBucket *putBucket = qc_putbucket_create();
		if (NULL == putBucket) {
			goto failed;
		}
		putterChain->putBuckets[i] = putBucket;
	}

    return putterChain;

failed:
	qc_putterchain_destroy(putterChain);
	return NULL;
}


int qc_putterchain_destroy(QcPutterChain *putterChain){
    if(NULL == putterChain){
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


int qc_putterchain_push(QcPutterChain *putterChain, QcPutter *putter){

    qc_assert(NULL!= putterChain && NULL!=putter);
	qc_assert(putter->message);

    QcListEntry *listEntry;

	int bucket_sn = putter->priority - 1;
    int ret = qc_list_inserttail2(putterChain->putBuckets[bucket_sn]->putterList, putter, &listEntry);
    qc_assert(ret == 0);

    putter->_entry = listEntry;

	putterChain->putter_count++;

	if (bucket_sn > putterChain->cursor_bucketsn)
		putterChain->cursor_bucketsn = bucket_sn;

    return 0;
}


QcPutter* qc_putterchain_pop(QcPutterChain *putterChain){

	if (putterChain->putter_count == 0)
		return NULL;

	int bucket_sn = putterChain->cursor_bucketsn;

	QcPutBucket *bucket;  //must before loop??

	//locate the cursor_bucketsn
	while (1) {
		bucket = putterChain->putBuckets[bucket_sn];
		if (qc_list_count(bucket->putterList) > 0) {
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
		if (qc_list_count(bucket->putterList) > 0) {
			putter = qc_list_pophead(putterChain->putBuckets[bucket_sn]->putterList);
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
		if (qc_list_count(putterChain->putBuckets[bucket_sn]->putterList) == putterChain->putter_count) {
			putter = qc_list_pophead(bucket->putterList);
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


int qc_putterchain_remove(QcPutterChain *putterChain, QcPutter *putter){
    qc_assert(NULL!= putterChain && NULL!=putter);

    QcListEntry *listEntry = putter->_entry;
	int bucket_sn = putter->priority - 1;
    int ret = qc_list_removeentry(putterChain->putBuckets[bucket_sn]->putterList, listEntry);
    qc_assert(ret == 0);

	putterChain->putter_count--;
    return 0;
}

