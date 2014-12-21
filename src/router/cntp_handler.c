#include "cntp_handler.h"
#include "packet_routing.h"

void cntp_handler(struct timestamp* recv_kern,
                  unsigned char *packet,
                  void* args, int packet_len)
{
    unsigned char *newpacket = malloc(packet_len + TIMESTAMP_LEN);

    if (newpacket == NULL) {
        printf(KMAG "Error:malloc()\n" RESET);
        exit(1);
    }

    recv_packet_print(packet, packet_len);

    struct fwd_info fwd_info;
    memset(&fwd_info, 0, sizeof(struct fwd_info));

    get_fwding_info(packet, &fwd_info,args);

    memcpy(newpacket, packet, packet_len);

    printf("Update\n");
    packet_update(newpacket, fwd_info.next_hop, packet_len);

    printf("Forward\n");
    packet_forward(newpacket, packet_len + TIMESTAMP_LEN, args);
}
