#include "stdio.h"
#include "demo_mq.h"
#include "demo_qsys.h"
#include "demo_sock.h"


int main()
{
    demo_mq();
    demo_qsys();
    demo_sock();

    printf("all demo completed!\n");
}
