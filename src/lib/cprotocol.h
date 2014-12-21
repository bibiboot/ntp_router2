#pragma once

#include "globals.h"

#define ROUTER_MAC 0
#define NODE1_MAC 1
#define NODE2_MAC 2
#define NODE3_MAC 3

#define NODE1_IP 1
#define NODE2_IP 2
#define NODE3_IP 3

#define CNTP_PORT 3

#define C_HLEN 68

/*
 * Custom protocol
 */
struct custom_packet_header{
    uint16_t identifier;
    uint16_t next_hop_addr;
    uint16_t dst_addr;
    uint16_t src_addr;
    uint16_t dst_port;
    char padding[58];
};
