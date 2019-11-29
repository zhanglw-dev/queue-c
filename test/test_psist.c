#include "test_psist.h"
#include "qc_psist.h"
#include "qc_file.h"

#define MSG_BUFF_SIZE 1024
#define MSG_COUNT_LIMIT 10000

static const char* filename = "mq_test01.tbl";
static const char *buff = "hello table file!";


int test_psist_file()
{
	QcErr err;
	int ret;

	char buf[256];
	memset(buf, 0, sizeof(buf));

	char* cwd = qc_dir_getcwd(buf, sizeof(buf));

	char filepath[512];
	memset(filepath, 0, sizeof(filepath));
	sprintf(filepath, "%s/../db/%s", cwd, filename);


	QcPsist *qPsist = qc_psist_open(MSG_BUFF_SIZE, MSG_COUNT_LIMIT, filepath, &err);
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

		printf("msg:%s\n", qc_message_buff(message1));
		qc_message_release(message1, 0);
	}

	qc_psist_close(qPsist);

	return 0;
}
