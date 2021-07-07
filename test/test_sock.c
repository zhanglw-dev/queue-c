/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, zhanglw (zhanglw366@163.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "test_sock.h"
#include "qc_server.h"
#include "qc_client.h"
#include "qc_message.h"
#include "qc_queue.h"
#include "qc_qsystem.h"
#include "qc_thread.h"
#include "qc_log.h"


#define PRODUCER_NUM  10
#define CONSUMER_NUM  10

const char *ip = "127.0.0.1";
const int port = 5555;
const char *qname = "queue01";
const char *msgstr = "123456789";
const int msgcount = 10000;
const int msec = -1;
const int quelimit = 1000;
const int maxlevel = 3;


void* producer_routine(void* arg) {
	//printf("producer start.\n");
	int ret;
	QcErr err;

	QcClient* client = qc_client_connect(ip, port, &err);
	if (!client) {
		printf("client connect failed.\n");
		return NULL;
	}

	//printf("producer connected.\n");

	for (int i = 0; i < msgcount; i++) {
		QcMessage* message_put = qc_message_create(msgstr, (int)strlen(msgstr), 0);
		ret = qc_client_msgput(client, "queue01", message_put, msec, &err);
		if (0 != ret) {
			//printf("msgput fail: %d %s\n", err.code, err.desc);
			if(err.code == QC_ERR_TIMEOUT){
				i--;
				qc_message_release(message_put, 0);
				continue;
			}

			printf("client msgput failed.\n");
			qc_client_disconnect(client);
			return NULL;
		}
        //printf("message put ok.\n");
		qc_message_release(message_put, 0);
	}

	qc_client_disconnect(client);
	return NULL;
}


void* consumer_routine(void* arg) {
	//printf("consumer start.\n");
	QcErr err;

	QcClient* client = qc_client_connect(ip, port, &err);
	if (!client) {
		printf("client connect failed.\n");
		return NULL;
	}

	//printf("consumer connected.\n");

	for (int i = 0; i < msgcount; i++) {
		QcMessage* message_get = qc_client_msgget(client, "queue01", msec, &err);
		if (!message_get) {
			//printf("msgget fail: %d %s\n", err.code, err.desc);
			if(err.code == QC_ERR_TIMEOUT){
				i--;
				continue;
			}

			printf("client msgget failed.\n");
			qc_client_disconnect(client);
			return NULL;
		}

		//printf("message get ok.\n");
		char* buff = qc_message_buff(message_get);
		if (0 != strcmp(buff, msgstr)) {
			printf("message get error.\n");
			qc_client_disconnect(client);
			return NULL;
		}

		qc_message_release(message_get, 1);
	}

	qc_client_disconnect(client);
	return NULL;
}


int test_net()
{
    int ret;
    QcErr err;

    QcQueue *queue = qc_queue_create(quelimit, maxlevel, &err);
    if(!queue){
        printf("create queue failed.\n");
        return -1;
    }

    QcQSystem *qSystem = qc_qsys_create();
    ret = qc_qsys_addqueue(qSystem, qname, queue, &err);
    if(0 != ret){
        printf("add queue to qsys failed.\n");
        return -1;
    }

    QcQueueSrv *queueSrv = qc_queuesrv_create(ip, port, qSystem, &err);
    if(!queueSrv){
        printf("create queuesvc failed.\n");
        return -1;
    }

    ret = qc_queuesrv_start(queueSrv, 1, &err);
    if(0 != ret){
        printf("start queuesvc failed.\n");
        return -1;
    }

	qc_info("test concurrent producer/consumer start.\n");
	time_t rawtime;
	time(&rawtime);
	qc_pinfo("%s", asctime(localtime(&rawtime)));

	QcThread* producer[PRODUCER_NUM];

	for (int i = 0; i < PRODUCER_NUM; i++) {
		QcThread* putthread = qc_thread_create(producer_routine, NULL);
		producer[i] = putthread;
	}

	QcThread* consumer[CONSUMER_NUM];

	for (int i = 0; i < CONSUMER_NUM; i++) {
		QcThread* getthread = qc_thread_create(consumer_routine, NULL);
		consumer[i] = getthread;
	}

	for (int i = 0; i < PRODUCER_NUM; i++) {
		int exitcode;
		qc_thread_join(producer[i], &exitcode);
	}

	for (int i = 0; i < CONSUMER_NUM; i++) {
		int exitcode;
		qc_thread_join(consumer[i], &exitcode);
	}

	time(&rawtime);
	qc_pinfo("%s", asctime(localtime(&rawtime)));

	int ct = qc_queue_msgcount(queue);
	if(ct != 0){
		printf("sock test failed: last count != 0 (%d)\n", ct);
	}
	else{
		qc_info("test concurrent producer/consumer succeed.\n");
	}

    qc_queuesrv_stop(queueSrv);
    qc_queuesrv_destory(queueSrv);

    return 0;
}
