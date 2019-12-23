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
#include "qc_service.h"
#include "qc_client.h"
#include "qc_message.h"
#include "qc_queue.h"
#include "qc_qsystem.h"
#include "qc_thread.h"
#include "qc_log.h"


#define PRODUCER_NUM  5
#define CONSUMER_NUM  5

const char *ip = "127.0.0.1";
const int port = 5555;
const char *qname = "queue01";
const char *msgstr = "123456789";
const int msgcount = 100000;


void* producer_routine(void* arg) {
	//printf("producer start.\n");
	int ret;
	QcErr err;

	QcClient* client = qc_client_connect(ip, port, &err);
	if (!client) {
		printf("client connect failed.");
		return NULL;
	}

	for (int i = 0; i < msgcount; i++) {
		QcMessage* message_put = qc_message_create(msgstr, (int)strlen(msgstr), 0);
		ret = qc_client_msgput(client, "queue01", message_put, 3, &err);
		if (0 != ret) {
			printf("client msgput failed.");
			return NULL;
		}
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
		printf("client connect failed.");
		return NULL;
	}

	for (int i = 0; i < msgcount; i++) {
		QcMessage* message_get = qc_client_msgget(client, "queue01", 3, &err);
		if (!message_get) {
			printf("client msgget failed");
			return NULL;
		}
		char* buff = qc_message_buff(message_get);
		if (0 != strcmp(buff, msgstr)) {
			printf("message get error");
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

    QcQueue *queue = qc_queue_create(1000, 10, &err);
    if(!queue){
        printf("create queue failed.");
        return -1;
    }

    QcQSystem *qSystem = qc_qsys_create();
    ret = qc_qsys_addqueue(qSystem, qname, queue, &err);
    if(0 != ret){
        printf("add queue to qsys failed.");
        return -1;
    }

    QcQueueSvc *queueSvc = qc_queuesvc_create(ip, port, qSystem, &err);
    if(!queueSvc){
        printf("create queuesvc failed.");
        return -1;
    }

    ret = qc_queuesvc_start(queueSvc, 1, &err);
    if(0 != ret){
        printf("start queuesvc failed");
        return -1;
    }

	qc_info("test concurrent producer/consumer start");
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

	qc_info("test concurrent producer/consumer succeed.");

    qc_queuesvc_stop(queueSvc);
    qc_queuesvc_destory(queueSvc);

    return 0;
}
