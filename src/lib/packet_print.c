#include "cprotocol.h"
#include "packet_print.h"

void reverse_kernel_timestamp(unsigned char *payload, int offset)
{
    unsigned char *timestamp_payload = payload + offset;

    int i;
    for(i=0; i<4; i++) {
        char temp = timestamp_payload[i];
        timestamp_payload[i] = timestamp_payload[7-i];
        timestamp_payload[7-i] = temp;
    }
}

void print_timestamp_hex(unsigned char *timestamp_payload)
{
    printf("Timestamp payload : ");
    int i;
    for(i=0; i<8; i++) {
       printf(" %.2X", timestamp_payload[i]);
    }
    printf("\n");
}

unsigned long get_drtt(unsigned char *payload, int offset)
{
    struct timestamp *drtt_st = (struct timestamp *)(payload + offset);
    return (unsigned long)drtt_st->fsec;
}

unsigned long get_kernel_timestamp(unsigned char *packet, int offset)
{
    unsigned char *timestamp_payload = packet + offset;
    unsigned char kernel_timestamp[8];
    memcpy(kernel_timestamp, timestamp_payload, TIMESTAMP_LEN);

    //print_timestamp_hex(kernel_timestamp);
    int i;
    for(i=0; i<4; i++) {
        char temp = kernel_timestamp[i];
        kernel_timestamp[i] = kernel_timestamp[7-i];
        kernel_timestamp[7-i] = temp;
    }
    //print_timestamp_hex(kernel_timestamp);
    return (unsigned long)EXTRACT_64BITS(kernel_timestamp);
}

unsigned long get_so_timestamp(unsigned char *packet, int offset)
{
    struct timestamp *remote_rx = (struct timestamp *)(packet + offset);
    return (unsigned long)remote_rx->sec * SECONDS + remote_rx->fsec * NANOSECONDS;
}

void print_header(unsigned char *packet, int packet_len)
{
    printf(KMAG "Total [%d]: Header size [%d]: Payload [%d]: \n" RESET,
           packet_len, C_HLEN, packet_len - C_HLEN);

    struct custom_packet_header *hdr = (struct custom_packet_header *)packet;
    printf("++++++++++++++++++++++++++++++++++++++\n");
    printf("| NEXT_HOP | SRC_IP | DEST_IP | PORT |\n");
    printf("++++++++++++++++++++++++++++++++++++++\n");
    printf("| %8d | %6d | %7d | %4d |\n",ntohs(hdr->next_hop_addr),
                                         ntohs(hdr->src_addr),
                                         ntohs(hdr->dst_addr),
                                         ntohs(hdr->dst_port));
    printf("++++++++++++++++++++++++++++++++++++++\n");
}

/*
 * Print al the DRTT and Xmit timestamp couples
 * Single node   | C_HLEN | DRTT-1 | Xmit-1 |
 * Single router | C_HLEN | DRTT-1 | Xmit-1 | Recv-2 | DRTT-2 | Xmit-2 |
 * Two router    | C_HLEN | DRTT-1 | Xmit-1 | Recv-2 | DRTT-2 | Xmit-2 | Recv-3 | DRTT-3 | Xmit-3 |
 */
void recv_packet_print(unsigned char *packet, int packet_len)
{
    print_header(packet, packet_len);

    int payload_len = packet_len - C_HLEN;
    unsigned char *payload = packet + C_HLEN;

    /*
     * Printing starts from payload
     * Recv timestamp is only printed if the length
     * check matches
     */
    while(payload_len > 0) {

        unsigned long drtt           = get_drtt(payload, 0);
        unsigned long xmit_timestamp = get_kernel_timestamp(payload, TIMESTAMP_LEN);
        unsigned long recv_timestamp = get_kernel_timestamp(payload, 2*TIMESTAMP_LEN);

        printf("Drtt                       : %lu\n", drtt);
        printf("Local transmit timestamp   : %lu\n", xmit_timestamp);
        printf("Remote receieve timestamp  : %lu\n", recv_timestamp);

        payload += 3*TIMESTAMP_LEN;
        payload_len -= 3*TIMESTAMP_LEN;
    }
}
