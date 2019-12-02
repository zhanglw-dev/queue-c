#include "qc_prelude.h"
#include "test_mq.h"
#include "test_psist.h"
#include "test_qsys.h"
#include "test_net.h"



int main(int argc, char **argv)
{
	int ret;

	/*
	ret = test_qsys();
	if (0 != ret) {
		printf("qsys test failed.");
		exit(-1);
	}
	*/

	/*
	ret = test_qtbl_file();
	if (0 != ret) {
		printf("qdb test failed.");
		exit(-1);
	}
	*/

	/*
	ret = mq_test_all();
	if (0 != ret) {
		printf("mq test failed.");
		exit(-1);
	}
	*/
	
	ret = test_net();
	if (0 != ret){
		printf("net test failed.");
		exit(-1);
	}
	
	exit(0);
}
