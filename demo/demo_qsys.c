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

#include "demo_qsys.h"
#include "qc_qsystem.h"
#include "qc_message.h"


int demo_qsys()
{
    QcErr err;

    QcQueue *queue = qc_queue_create(1000, 10, &err);
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

    char *buff = "hello qsys!";
    QcMessage *message_put = qc_message_create(buff, (int)strlen(buff)+1, BUFFFLAG_NO_FREE);

    if( 0!= qc_qsys_putmsg(qSys, qname, message_put, 0, &err)){
        printf("qsys put msg failed: %s\n", err.desc);
        exit(-1);
    }

    QcMessage *message_got = qc_qsys_getmsg(qSys, qname, 0, &err);
    if(NULL == message_got){
        printf("qsys get msg failed: %s\n", err.desc);
        exit(-1);
    }

    printf("got message: buff=%s\n", qc_message_buff(message_got));

    qc_message_release(message_got);

	qc_qsys_destory(qSys);

    printf("qc qsys is ok!\n");
	return 0;
}
