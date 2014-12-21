#include "packet_routing.h"
#include "sock_details.h"

/*
 * Hard code everything -
 * This is basically routing/forwarding function of router
 **/
void get_fwding_info(unsigned char *packet,
                     struct fwd_info *args,
                     void *arg)
{
    struct interface *inf = (struct interface *)arg;
    struct custom_packet_header *hdr = (struct custom_packet_header *)packet;

    if (ntohs(hdr->dst_addr) == 2) {
            args->next_hop = 2;
    	    inf->sk.sll_ifindex = inf_to_index("eth0");
    } else if (ntohs(hdr->dst_addr) == 1) {
            args->next_hop = 1;
    	    inf->sk.sll_ifindex = inf_to_index("eth1");
    } else {
        printf(KRED "Route not found\n" RESET);
        //TODO
    }
}
