#include "packet_update.h"

void packet_update(unsigned char  *newpacket,
                   uint16_t next_hop, int packet_len)
{
    struct custom_packet_header *hdr = (struct custom_packet_header *)newpacket;
    unsigned char *new_payload = newpacket + packet_len;

    hdr->next_hop_addr = htons(next_hop);

    struct timestamp *drtt_st = (struct timestamp *)(new_payload);
    drtt_st->sec = 0;
    drtt_st->fsec = atoi(globals.drtt);
    printf("Value = %lu\n", drtt_st->fsec);
}
