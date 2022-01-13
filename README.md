# queue-c
Queue-C is a light-weight, cross platform, message queue libaray, C-based development.

Features:
* Clearly API for message queue, support time wait, message priority
* Supoort message buffer pointer directly store and duplicate store
* Layered interface for queue operate: single queue API, queue system API, socket based IPC/Network API.
* Light-weight struct design, server and embeded system integrated. 
* Pure C source code, cross platform complie, like Windows, Linux, MacOS and other Unix like OS.


Queue-C是基于C语言开发的轻量级的、高效、跨平台消息队列开发库。

特性：
* 支持阻塞定时等待,消息优先级,进程内/进程间/主机间交换的消息队列接口
* 内存指针和内存拷贝双存储模式
* 分层接口API: 单队列API，队列系统API，基于socket的进程间/主机间消息传输API
* 轻量级结构设计，适合主机与嵌入式环境
* 跨平台编译: 可运行于Windows、Linux、MacOS 及其他类Unix系统
  
  
  
//Sample code on single mode:  

QcErr err;  
char data = "hello queue-c";  

QcQueue* queue = qc_queue_create(1000, 3, &err);  
QcMessage *message_put = qc_message_create(data, strlen(data)+1, BUFFFLAG_NO_FREE);  

if(0 != qc_queue_msgput(queue, message_put, -1, &err)){  
  printf("put msg failed: %s\n", err.desc);  
  exit(-1);  
}  

QcMessage *message_got = qc_queue_msgget(queue, -1, &err);  

if(NULL == message){  
  printf("get msg failed: %s\n", err.desc);  
  exit(-1);  
}  

printf("got msg: %s [length:%d]\n", qc_message_buff(message), qc_message_bufflen(message));  

qc_message_release(message_got, 1);  
qc_queue_destroy(queue);  


