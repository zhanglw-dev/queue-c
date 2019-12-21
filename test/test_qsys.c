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

#include "test_qsys.h"
#include "qc_qsystem.h"
#include "qc_queue.h"



int test_qsys()
{
	QcErr err;
	int ret;

	QcQSystem *qSys = qc_qsys_create();
	if (!qSys) {
		printf("create qsystem failed.\n");
		return -1;
	}

	char qname[20];

	for (int i = 0; i < 100; i++)
	{
		QcQueue *queue = qc_queue_create(1000, 10, &err);
		if (!queue) {
			printf("create queue failed.\n");
			return -1;
		}

		memset(qname, 0, sizeof(qname));
		sprintf(qname, "queue-%d", i);

		ret = qc_qsys_addqueue(qSys, qname, queue, &err);
		if (0 != ret) {
			printf("add queue to qsys failed.\n");
			return -1;
		}
	}

	for (int i = 0; i < 100; i++)
	{
		memset(qname, 0, sizeof(qname));
		sprintf(qname, "queue-%d", i);
		/*
		QcQueue *queue = qc_qsys_queue_get(qSys, qname, &err);
		if (!queue) {
			printf("add queue to qsys failed.\n");
			return -1;
		}
		*/
	}

	printf("qsys test ok!\n");
	qc_qsys_destory(qSys);
	return 0;
}
