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

#include "test_psist.h"
#include "qc_psist.h"
#include "qc_file.h"

#define MSG_BUFF_SIZE 1024
#define MSG_COUNT_LIMIT 10000

static const char* filename = "mq_test01.tbl";
static const char *buff = "hello psist file!";


int test_psist_file()
{
	QcErr err;
	int ret;

	char buf[256];
	memset(buf, 0, sizeof(buf));

	char* cwd = qc_dir_getcwd(buf, sizeof(buf));

	char descript[512];
	memset(descript, 0, sizeof(descript));
	sprintf(descript, "file://%s/%s", cwd, filename);


	QcPsist *qPsist = qc_psist_open(MSG_BUFF_SIZE, MSG_COUNT_LIMIT, descript, &err);
	if (!qPsist) {
		printf("open psist failed.\n");
		return -1;
	}

	for (int i = 0; i < 1000; i++) {
		QcMessage *message = qc_message_create(buff, (int)strlen(buff), 0);
		ret = qc_psist_append(qPsist, message, &err);
		if (0 != ret) {
			printf("psist append failed.\n");
			return -1;
		}
		qc_message_release(message, 0);
	}

	QcQueue* queue = qc_queue_create(MSG_COUNT_LIMIT, 10, NULL);
	ret = qc_psist_loadqueue(qPsist, queue, &err);
	if (0 != ret) {
		printf("load queue failed.\n");
		return -1;
	}

	for (int i = 0; i < 1000; i++) {
		QcMessage *message1 = qc_queue_msgget(queue, 1, NULL);
		if (NULL == message1) {
			printf("qc_queue_msgget failed.\n");
			return -1;
		}

		//printf("msg:%s\n", qc_message_buff(message1));
		qc_message_release(message1, 0);
	}

	qc_psist_close(qPsist);

	return 0;
}
