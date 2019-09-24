
#include "qc_mqueue.h"
#include "qc_message.h"
#include "qc_log.h"
#include "qc_error.h"
#include "qc_thread.h"


const int queue_limit = 1000;
const int message_num = 100000;
const char *buff = "hello queue!";

#define THREAD_NUM 10



void* putroutine(void *arg){
    QcQueue *queue = arg;
    int ret;

    for(int i=0; i<message_num; i++){
        QcMessage *message = qc_message_create(buff, (int)strlen(buff), 0);
		qc_message_setpriority(message, 10);

        ret = qc_queue_msgput(queue, message, -1, NULL);
        if(ret != 0){
            qc_perror("qc_queue_msgput failed, ret=%d", ret);
            return NULL;
        }

		////qc_message_release(message, 0);
    }

    return 0;
}


void* getroutine(void *arg){
    QcQueue *queue = arg;

    for(int i=0; i<message_num; i++){
        QcMessage *message = qc_queue_msgget(queue, -1, NULL);
        if(NULL == message){
            qc_error("qc_queue_msgget failed.");
            return NULL;
        }

        if(strcmp(qc_message_buff(message), buff) != 0)
        {
            qc_error("message data verify failed.");
            return NULL;
        }

        qc_message_release(message, 0);
    }

    return 0;
}


int main(int argc, char **argv)
{
    int ret;

    printf("test queue start...\n");

    QcQueue* queue = qc_queue_create(queue_limit, 10, NULL);


    //---------------------------------------------------------------------
    qc_info("test put start.");
    
    for(int i=0; i<queue_limit; i++){
        QcMessage *message = qc_message_create(buff, (int)strlen(buff), 0);

        ret = qc_queue_msgput(queue, message, -1, NULL);
        if(ret != 0){
            qc_perror("qc_queue_msgput failed, ret=%d", ret);
            return -1;
        }
    }
    qc_info("test put succeed.");
    //---------------------------------------------------------------------
    


    //---------------------------------------------------------------------    
    qc_info("test block put start.");

    QcErr puterr;
    ret = qc_queue_msgput(queue, qc_message_create(NULL, 0, 0), 1, &puterr);
    if(0 !=ret ){
        if(QC_TIMEOUT != puterr.code){
            qc_perror("qc_queue_msgput failed, errcode=%d", puterr.code);
            return -1;
        }
    }

    unsigned int count = qc_queue_msgcount(queue);
    qc_assert(count==queue_limit);
    
    qc_info("test block put succeed.");
    //---------------------------------------------------------------------



    //---------------------------------------------------------------------    
    qc_info("test get start.");
    
    for(int i=0; i<queue_limit; i++){
        QcMessage *message = qc_queue_msgget(queue, -1, NULL);
        if(NULL == message){
            qc_error("qc_queue_msgget failed.");
            return -1;
        }
        qc_message_release(message, 0);
    }

    count = qc_queue_msgcount(queue);
    qc_assert(count==0);
    
    qc_info("test get succeed.");
    //---------------------------------------------------------------------



    //---------------------------------------------------------------------    
    qc_info("test block get start.");
    QcErr geterr;
    QcMessage *message = qc_queue_msgget(queue, 1, &geterr);
    if(NULL == message){
        if(QC_TIMEOUT != geterr.code){
            qc_perror("qc_queue_msgget failed, errcode=%d", geterr.code);
            return -1;
        }
    }

    count = qc_queue_msgcount(queue);
    qc_assert(count==0);

    qc_info("test block get succeed.");
    //---------------------------------------------------------------------



    //---------------------------------------------------------------------
    qc_info("test concurrent put/get start");    

    time_t rawtime; 
    time(&rawtime); 
    qc_pinfo("%s", asctime(localtime(&rawtime)));

    QcThread* put[THREAD_NUM];

    for(int i=0; i< THREAD_NUM; i++){
        QcThread* putthread = qc_thread_create(putroutine, queue);
        put[i] = putthread;
    }


    QcThread* get[THREAD_NUM];

    for(int i=0; i< THREAD_NUM; i++){
        QcThread* getthread = qc_thread_create(getroutine, queue);
        get[i] = getthread;
    }


    for(int i=0; i< THREAD_NUM; i++){
        int exitcode;
        qc_thread_join(put[i], &exitcode);
        qc_thread_join(get[i], &exitcode);
    }

    time(&rawtime); 
    qc_pinfo("%s", asctime(localtime(&rawtime)));

    qc_info("test concurrent put/get succeed.");        
    //---------------------------------------------------------------------


    count = qc_queue_msgcount(queue);
    qc_assert(count==0);

    ret = qc_queue_destroy(queue, NULL);
    qc_assert(ret == 0);


    printf("test queue finished.\n");
    return 0;
}

