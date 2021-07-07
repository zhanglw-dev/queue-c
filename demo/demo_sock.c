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

#include "demo_sock.h"
#include "qc_qsystem.h"
#include "qc_message.h"
#include "qc_server.h"
#include "qc_client.h"


int demo_sock()
{
    QcErr err;

    QcQueue *queue = qc_queue_create(1000, 3, &err);
    if (!queue) {
        printf("create queue failed: %s\n", err.desc);
        exit(-1);
    }

	QcQSystem *qSys = qc_qsys_create();
	if (!qSys) {
		printf("create qsystem failed.\n");
		exit(-1);
	}

	char *qname = "queue01";
    if (0 != qc_qsys_addqueue(qSys, qname, queue, &err)) {
        printf("add queue to qsys failed: %s\n", err.desc);
        exit(-1);
    }

    QcQueueSrv *queueSrv = qc_queuesrv_create("127.0.0.1", 5555, qSys, &err);
    if(!queueSrv){
        printf("create queuesrv failed.\n");
        exit(-1);
    }

    if(0 != qc_queuesrv_start(queueSrv, 1, &err)){
        printf("start queuesrv failed.\n");
        exit(-1);
    }

    QcClient *client = qc_client_connect("127.0.0.1", 5555, &err);
    if(!client){
        printf("client connect failed.\n");
        exit(-1);
    }

    char *buff = "hello qc_sock!";
    QcMessage *message_put = qc_message_create(buff, (int)strlen(buff)+1, 0);
    if(0 != qc_client_msgput(client, "queue01", message_put, 3, &err)){
        printf("client msgput failed.\n");
        exit(-1);
    }

    QcMessage *message_get = qc_client_msgget(client, "queue01", 3, &err);
    if(!message_get){
        printf("client msgget failed.\n");
        exit(-1);
    }

    char *buff_got = qc_message_buff(message_get);
    if(0 != strcmp(buff_got, buff)){
        printf("message get error.\n");
        exit(-1);
    }

    qc_client_disconnect(client);
    qc_queuesrv_stop(queueSrv);
    qc_queuesrv_destory(queueSrv);

    printf("qc sock is ok!\n");

    return 0;
}
