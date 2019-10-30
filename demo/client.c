#include "qc_qsystem.h"
#include "qc_queue.h"
#include "qc_service.h"
#include "qc_client.h"



void producer_routine(void *param)
{
	QcErr err;
	printf("start producer...\n");

	QcClient *client = qc_producer_connect("127.0.0.1", 5555, "queue1", &err);
	if (NULL == client) {
		printf("producer connect failed.\n");
		return;
	}

	qc_producer_disconnect(client);

	qc_thread_exit(0);
}


void consumer_routine(void *param)
{
	QcErr err;
	printf("start consumer...\n");

	QcClient *client = qc_consumer_connect("127.0.0.1", 5555, "queue1", &err);
	if (NULL == client) {
		printf("consumer connect failed.\n");
		return;
	}

	qc_consumer_disconnect(client);

	qc_thread_exit(0);
}


int main(int argc, char **argv)
{
	int excode;

	QcThread *threadProducer;
	QcThread *threadConsumer;

	//threadProducer = qc_thread_create(producer_routine, NULL);
	threadConsumer = qc_thread_create(consumer_routine, NULL);

	//qc_thread_join(threadProducer, &excode);
	qc_thread_join(threadConsumer, &excode);

	exit(0);
}
