#include "sock_details.h"

int create_socket(char *inf_name,struct sockaddr_ll *sk)
{
    int s;
    s = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if (-1 == s)
    {
        printf("ERROR: socket(). Exiting...\n");
        perror("SOCKET:");
        exit(1);
    }

    set_socket_inf(s,inf_name,sk);

    return s;
}

void set_socket_inf(int s, char *inf_name, struct sockaddr_ll *sk)
{
    sk->sll_family   = PF_PACKET;
    sk->sll_protocol = htons(ETH_P_IP);
    sk->sll_ifindex  = inf_to_index(inf_name);
}

int inf_to_index(char *inf_name)
{
    int inf_indx;

    if ((inf_indx = if_nametoindex (inf_name)) == 0)
    {
        printf("Failed to get interface index of \'%s\'\n",inf_name);
        perror ("if_nametoindex():");
        exit (EXIT_FAILURE);
    }

    return inf_indx;
}
