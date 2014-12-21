#include "globals.h"
#include "ipc_client.h"
#include "packet_sniff.h"
#include "sock_details.h"

void init()
{
    // interfac1
    strcpy(globals.inf[0].inf_name,INF1);
    globals.inf[0].addr = INFADDR1;
    globals.inf[0].dnode_ip = DNODEIP1;
    globals.inf[0].sock = create_socket(globals.inf[0].inf_name,&(globals.inf[0].sk));

    // interfac2
    strcpy(globals.inf[1].inf_name,INF2);
    globals.inf[1].addr = INFADDR2;
    globals.inf[1].dnode_ip = DNODEIP2;
    globals.inf[1].sock = create_socket(globals.inf[1].inf_name,&(globals.inf[1].sk));
}

void start()
{
    pthread_create(&globals.recv_th, 0, &packet_capture,(void*)(&globals.inf[0]));
    pthread_create(&globals.ipc_th, 0, start_ipc_client, NULL);

    pthread_join(globals.recv_th, NULL);
    pthread_join(globals.ipc_th, NULL);
}

int main(int argc, char *argv[])
{
    globals.src_node = atoi(argv[1]);
    //globals.drtt = "177800";

    init();

    start();

    return 0;
}
