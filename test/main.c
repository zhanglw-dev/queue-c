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

#include "qc_prelude.h"
#include "test_mq.h"
#include "test_psist.h"
#include "test_qsys.h"
#include "test_sock.h"
#include "test_sem.h"
#include "test_shm.h"
#include "test_log.h"


#define ENABLE_LOG_TEST  1



int main(int argc, char **argv)
{
	int ret;

	ret = test_sem();
	if (0 != ret) {
		printf("sem test failed.");
		exit(-1);
	}
	
	ret = test_shm();
	if (0 != ret) {
		printf("shm test failed.");
		exit(-1);
	}

	ret = mq_test_all();
	if (0 != ret) {
		printf("mq test failed.");
		exit(-1);
	}


	ret = test_qsys();
	if (0 != ret) {
		printf("qsys test failed.");
		exit(-1);
	}


	ret = test_psist_file();
	if (0 != ret) {
		printf("psist test failed.");
		exit(-1);
	}


	ret = test_net();
	if (0 != ret){
		printf("net test failed.");
		exit(-1);
	}


	if(ENABLE_LOG_TEST)
	{
		ret = test_log();
		if (0 != ret){
			printf("log test failed.");
			exit(-1);
		}
	}

	printf("all test succeed!\n");
	exit(0);
}
