#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include <pcap.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>

#include "config.h"
#include "color.h"
#include "cprotocol.h"
#include "uthash.h"

#define SNAP_LEN 1518

// Interface Information
#define NUM_OF_INF 3
#define TIMESTAMP_LEN 8

#define INF1 "eth1"
#define INF2 "eth1"

#define INFADDR1 2
#define DNODEIP1 1

#define INFADDR2 3
#define DNODEIP2 4

#define SECONDS      1000000000
#define MILLISECONDS 1000000
#define MICROSECONDS 1000
#define NANOSECONDS  1

#define EXTRACT_64BITS(p) \
                ((u_int64_t)((u_int64_t)*((const u_int8_t *)(p) + 0) << 56 | \
                                            (u_int64_t)*((const u_int8_t *)(p) + 1) << 48 | \
                                            (u_int64_t)*((const u_int8_t *)(p) + 2) << 40 | \
                                            (u_int64_t)*((const u_int8_t *)(p) + 3) << 32 | \
                                            (u_int64_t)*((const u_int8_t *)(p) + 4) << 24 | \
                                            (u_int64_t)*((const u_int8_t *)(p) + 5) << 16 | \
                                            (u_int64_t)*((const u_int8_t *)(p) + 6) << 8 | \
                                            (u_int64_t)*((const u_int8_t *)(p) + 7)))

/*
 * Not used
 */
typedef struct hashl {
    int key;
    char value[100];
    UT_hash_handle hh;
} dict_node;

struct interface {
    char inf_name[64];
    uint16_t addr;
    uint16_t dnode_ip;  // Used for filtering
    int sock;
    int inf_index;
    struct sockaddr_ll sk;
    pthread_t thrd;
};

struct timestamp{
    uint32_t sec;
    uint32_t fsec;
};

/*
 * Routing information
 */
struct fwd_info {
    uint16_t next_hop;
    char fwding_inf_name[64];
};

struct globals {
    struct config config;              /* config st    */
    struct interface inf[NUM_OF_INF];
    pthread_t ipc_th;                  /* ipc thread   */
    pthread_t recv_th;                 /* sniff thread */
    char *drtt;                        /* drtt         */
    int src_node;                      /* src node     */
    dict_node *hashl;                  /* Not used     */
};

char errbuf[PCAP_ERRBUF_SIZE];
extern struct globals globals;
