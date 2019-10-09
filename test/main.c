#include "qc_prelude.h"
#include "test_mq.h"



int main(int argc, char **argv)
{
	int ret;

	ret = mq_test_all();
	if (0 != ret) {
		printf("mq test failed.");
		exit(-1);
	}

	exit(0);
}
