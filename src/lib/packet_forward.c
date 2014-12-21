#include "packet_forward.h"

void packet_forward(void *packet,int packet_size,void *args)
{
    struct interface *inf = (struct interface *)args;
    int sockfd = inf->sock;
    struct sockaddr_ll *sk = &(inf->sk);

    int send_result = sendto(sockfd,(unsigned char*)packet,packet_size,0,(struct sockaddr *)sk,sizeof(struct sockaddr_ll));
    if (-1 == send_result)
    {
        printf("Erro:Sendto()\n");
        exit(1);
    }
}
