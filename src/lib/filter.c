#include "filter.h"

/*
 * Packet must match next_hop ==
 *                   src_ip   == desired_dest_ip
 *                   port     == CNTP_PORT
 */
bool is_cntp(unsigned char *packet)
{
    struct custom_packet_header *hdr = (struct custom_packet_header *)packet;
    /*
    printf("Next hop : %d\n", ntohs(hdr->next_hop_addr));
    printf("Port     : %d\n", ntohs(hdr->dst_port));
    */

    if (ntohs(hdr->next_hop_addr) == globals.src_node &&
        ntohs(hdr->dst_port) == CNTP_PORT) {
        return true;
    }

    return false;
}
