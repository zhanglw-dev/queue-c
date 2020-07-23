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

#ifndef QC_PRELUDE_H
#define QC_PRELUDE_H


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>


#if (IS_WINDOWS|WINDOWS|_WINDOWS|WIN32|_WIN32|WIN64|_WIN64)

    #include <WinSock2.h>
    #include <Ws2tcpip.h>
    #include <process.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <direct.h>
    #include <io.h>
    #include <signal.h>

    #define sleep(x) Sleep(1000*x)
    #define INFTIM -1
    #define getcwd _getcwd
    #define PTHREAD_CANCELED -1

    #ifndef _LIB
	    #ifdef _USRDLL
		    #define QCDLL __declspec (dllexport)
	    #else
		    #define QCDLL __declspec (dllimport)
	    #endif
    #else
	    #define QCDLL
    #endif

    #define QCAPI __cdecl

#else
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/socket.h>
    #include <poll.h>
    #include <sys/time.h>
    #include <sys/timeb.h>
    #include <sys/stat.h>
    #include <sys/ioctl.h>
    #include <sys/file.h>
    #include <sys/wait.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <pthread.h>
    #include <signal.h>
    #include <semaphore.h>
    #include <sys/mman.h>

    #include <string.h>       /*for stricmp? no use yet*/
    #include <netinet/tcp.h>  /*TCP_NODELAY, TCP_CORK on linux*/

    #ifndef INFTIM
        #define INFTIM (-1)
    #endif
    #ifndef INFINITE
        #define INFINITE (-1)
    #endif

    #define QCDLL
    #define QCAPI
    
#endif


//#define QC_INVALID_INT  -99
#define QC_TIMEOUT -33
#define QC_FULLED  -100


typedef size_t qc_size_t;
#define qc_off_t off_t


#define qc_stderr(fmt, ...) \
                    { \
                     fprintf(stderr, (const char *)(fmt), __VA_ARGS__); \
                     fprintf(stderr, " syserr=%d", errno); \
                     fprintf(stderr, " [%s  line:%d]\n", __FILE__, __LINE__); \
                    }


#define qc_malloc(ptr, size)  ptr=malloc(size); \
                              if(NULL == ptr) { \
                                  fprintf(stderr, "malloc failed. [%s  line:%d]\n", __FILE__, __LINE__); \
								  assert(ptr); \
                              } \
                              else \
                                 memset(ptr, 0, size);


#define qc_free(ptr)  free(ptr); ptr=NULL;


#define qc_assert(expr) if(!(expr)) \
                        { \
                            fprintf(stderr, "stderr: assert throw. [%s  line:%d]\n", __FILE__, __LINE__); \
                            assert(expr); \
                        }


#endif /*QC_PRELUDE_H*/
