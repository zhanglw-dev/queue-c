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

#include "test_log.h"
#include "qc_log.h"


int test_log()
{
    char cwd[128];
    char logfile[256];

    memset(cwd, 0, sizeof(cwd));
    memset(logfile, 0, sizeof(logfile));

    getcwd(cwd, sizeof(cwd));

    sprintf(logfile, "%s/%s", cwd, "test.log");

    qc_log_init(logfile, 8, 10, 5);

    qc_info("log test start...");

    for(int i=0; i<30000; i++)
    {
        qc_pdebug(1, "test debug(level%d) out =========================================== %d", 1, i);
        qc_pdebug(2, "test debug(level%d) out =========================================== %d", 2, i);
        qc_pdebug(3, "test debug(level%d) out =========================================== %d", 3, i);
        qc_pdebug(4, "test debug(level%d) out =========================================== %d", 4, i);
        qc_pdebug(5, "test debug(level%d) out =========================================== %d", 5, i);
        qc_pdebug(6, "test debug(level%d) out =========================================== %d", 6, i);
        qc_pdebug(7, "test debug(level%d) out =========================================== %d", 7, i);
        qc_pdebug(8, "test debug(level%d) out =========================================== %d", 8, i);
        qc_pdebug(9, "test debug(level%d) out =========================================== %d", 9, i);
        qc_pdebug(10, "test debug(level%d) out =========================================== %d", 10, i);
    }

    qc_info("log test stoped!");

    return 0;
}
