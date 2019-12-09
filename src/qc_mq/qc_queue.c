#include "qc_prelude.h"
#include "qc_queue.h"
#include "qc_log.h"
#include "qc_list.h"
#include "qc_error.h"
#include "qc_getters.h"
#include "qc_putters.h"
#include "qc_msgchain.h"
#include "qc_message.h"



struct __QcQueue{
	QcMutex *quelock;

	unsigned int limit;
	QcMsgChain *mesgesChain;

	QcGettersList *gettersList;
	QcPuttersChain *puttersChain;
};



QcQueue* qc_queue_create(unsigned int limit, unsigned int priority_max, QcErr *err){
    QcQueue *queue = NULL;

	if(limit <= 0){
		qc_seterr(err, QC_ERR_BADPARAM, "parameter of limit must >0, current value=%d.", limit);
		return NULL;
	}

    qc_malloc(queue, sizeof(QcQueue));
	if(NULL == queue){
		qc_seterr(err, QC_ERR_RUNTIME, "malloc instance of queue failed.");
		return NULL;
	}

	memset(queue, 0, sizeof(QcQueue));
	queue->limit = limit;

	queue->quelock = qc_thread_mutex_create();
	if(NULL == queue->quelock){
		qc_seterr(err, QC_ERR_RUNTIME, "create quelock for queue failed.");
		goto failed;
	}

	queue->mesgesChain = qc_msgchain_create(limit, priority_max);
	if(NULL == queue->mesgesChain){
		qc_seterr(err, QC_ERR_RUNTIME, "create mesgesChain for queue failed.");
		goto failed;
	}

	queue->gettersList = qc_getterslist_create();
	if(NULL == queue->gettersList){
		qc_seterr(err, QC_ERR_RUNTIME, "create gettersList for queue failed.");
		goto failed;
	}

	queue->puttersChain = qc_putterschain_create(priority_max);
	if(NULL == queue->puttersChain){
		qc_seterr(err, QC_ERR_RUNTIME, "create putterList for queue failed.");
		goto failed;
	}

	return queue;

failed:
	qc_queue_destroy(queue);
	return NULL;
}


int qc_queue_destroy(QcQueue *queue){

	if(queue->quelock) qc_thread_mutex_destroy(queue->quelock);
	if(queue->mesgesChain) qc_msgchain_destroy(queue->mesgesChain);
	if(queue->gettersList)  qc_getterslist_destroy(queue->gettersList);
	if(queue->puttersChain)  qc_putterschain_destroy(queue->puttersChain);

	qc_free(queue);
	return 0;
}


unsigned int qc_queue_msgcount(QcQueue *queue){
	if(NULL == queue){
		return -1;
	}
	return qc_msgchain_msgcount(queue->mesgesChain);
}


int qc_queue_msgput(QcQueue *queue, QcMessage *message, int msec, QcErr *err){

	if(NULL == queue){
		qc_seterr(err, QC_ERR_BADPARAM, "invalid input parameter, QcQueue* is NULL.");
		return -1;
	}
	if(NULL == message){
		qc_seterr(err, QC_ERR_BADPARAM, "invalid input params, QcMessage* is NULL.");
		return -1;
	}

put_loop:

	qc_thread_mutex_lock(queue->quelock);

	QcGetter *getter = qc_getterslist_pop(queue->gettersList);

	//if there are getters waiting
	if(getter){
		qc_thread_condlock_lock(getter->condlock);
		qc_thread_mutex_unlock(queue->quelock);

		//maybe this getter has justly wait-timeout
		if(getter->is_timedout){
			qc_thread_condlock_unlock(getter->condlock);
			qc_getter_destroy(getter); //destroy popped timeouted getter
			goto put_loop;
		}

		//getter->message = message;
		qc_msgchain_puttemp(queue->mesgesChain, message);

		qc_thread_cond_signal(getter->cond);
		qc_thread_condlock_unlock(getter->condlock);

		return 0;
	}
	else{
		if(qc_msgchain_msgcount(queue->mesgesChain) == queue->limit){  //overloaded
			QcPutter *putter = qc_putter_create();
			if(NULL == putter){
				qc_thread_mutex_unlock(queue->quelock);
				qc_seterr(err, QC_ERR_RUNTIME, "create putter failed.");
				return -1;
			}

			qc_thread_condlock_lock(putter->condlock);

			putter->message = message;
			putter->priority = qc_message_priority(message);
			qc_putterschain_push(queue->puttersChain, putter);

			qc_thread_mutex_unlock(queue->quelock);
			int ret = qc_thread_cond_timedwait(putter->cond, putter->condlock, msec);
			
			if(QC_TIMEOUT == ret){
				putter->is_timedout = 1;
			}

			qc_thread_condlock_unlock(putter->condlock);

			if(putter->is_timedout){
				int r = qc_putterschain_remove(queue->puttersChain, putter);
				if(r == 0) qc_putter_destroy(putter);  //distroy un-popped timeouted putter

				qc_seterr(err, QC_TIMEOUT, "time out.");
				return -1;
			}

			qc_putter_destroy(putter);
			return 0;
		}
		else{		
			qc_msgchain_pushmsg(queue->mesgesChain, message);
			qc_thread_mutex_unlock(queue->quelock);

			return 0;
		}
	}

	return -1;
}


QcMessage* qc_queue_msgget(QcQueue *queue, int msec, QcErr *err){

	if(NULL == queue){
		qc_seterr(err, QC_ERR_BADPARAM, "invalid input params, QcQueue* is NULL.");
		return NULL;
	}

	qc_thread_mutex_lock(queue->quelock);

	if(0 == qc_msgchain_msgcount(queue->mesgesChain)){	//no message in list
		QcGetter *getter = qc_getter_create();
		if(NULL == getter){
			qc_thread_mutex_unlock(queue->quelock);
			qc_seterr(err, QC_ERR_RUNTIME, "create getter failed.");
			return NULL;
		}

		qc_thread_condlock_lock(getter->condlock);

		qc_getterslist_push(queue->gettersList, getter);

		qc_thread_mutex_unlock(queue->quelock);
		
		QcMessage *message = NULL;

		int ret = qc_thread_cond_timedwait(getter->cond, getter->condlock, msec);
		if(QC_TIMEOUT == ret){
			getter->is_timedout = 1;
		}
		else {
			//QcMessage *message = getter->message;
			message = qc_msgchain_gettemp(queue->mesgesChain);
			qc_assert(message);
		}

		qc_thread_condlock_unlock(getter->condlock);

		if(getter->is_timedout){
			int r = qc_getterslist_remove(queue->gettersList, getter);
			if(r == 0) qc_getter_destroy(getter);  //distroy un-popped timeouted getter

			qc_seterr(err, QC_TIMEOUT, "time out.");
			return NULL;
		}

		qc_getter_destroy(getter);
		return message;
	}
	else{
		QcMessage *message = qc_msgchain_popmsg(queue->mesgesChain);
        QcPutter *putter;  //must before loop??

pop_loop:

		putter = qc_putterschain_pop(queue->puttersChain);
		if(putter){
			qc_thread_condlock_lock(putter->condlock);			
			if(putter->is_timedout){
				qc_thread_condlock_unlock(putter->condlock);
				qc_putter_destroy(putter); //destroy popped timeouted putter			
				goto pop_loop;
			}
			else{
				QcMessage *putmsg = putter->message;
				qc_msgchain_pushmsg(queue->mesgesChain, putmsg);
				qc_thread_cond_signal(putter->cond);
			}

			qc_thread_condlock_unlock(putter->condlock);
		}

		qc_thread_mutex_unlock(queue->quelock);
		return message;
	}

	return NULL;
}


int qc_queue_forceput(QcQueue *queue, QcMessage *message, QcMessage **msg_popped, QcErr *err)
{
	int ret;
	if(NULL == queue){
		qc_seterr(err, QC_ERR_BADPARAM, "invalid input params, QcQueue* is NULL.");
		return -1;
	}

	qc_thread_mutex_lock(queue->quelock);
	ret = qc_msgchain_forcepush(queue->mesgesChain, message, msg_popped);
	qc_thread_mutex_unlock(queue->quelock);

	if(ret < 0){
		qc_seterr(err, QC_ERR_INTERNAL, "internal error");
		return -1;
	}

	return ret;
}
