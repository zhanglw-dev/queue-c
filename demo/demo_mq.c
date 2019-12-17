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

#include "qc_queue.h"
#include "qc_message.h"


int demo_mq()
{
    QcErr err;

    QcQueue* queue = qc_queue_create(1000, 3, &err);
    if(NULL == queue){
        printf("create queue failed: %s", err.desc);
        exit(-1);
    }

    int len = 256;
    for(int i=0; i<10; i++){
        char *buff = (char*)malloc(len);
        memset(buff, 0, len);
        sprintf(buff, "hello queue-c [%d]", i);

        QcMessage *message = qc_message_create(buff, len, 0);

        if(0 != qc_queue_msgput(queue, message, -1, &err)){
            printf("put msg failed: %s\n", err.desc);
            exit(-1);
        }
    }

    for(int i=0; i<10; i++){
        QcMessage *message = qc_queue_msgget(queue, -1, &err);
        if(NULL == message){
            printf("get msg failed: %s\n", err.desc);
            exit(-1);
        }

        printf("got msg: %s [length:%d]\n", qc_message_buff(message), qc_message_bufflen(message));
        qc_message_release(message, 1);
    }

    qc_queue_destroy(queue);

    printf("qc mq is ok!\n");
    return 0;
}
