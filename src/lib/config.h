#pragma once

#define CONFIG_INIT { \
    .packet_len = 65535, \
    .ipc_sleep_time = 30*1, \
    .payload_len = 1024, \
    .rtt = 2, }

struct config {
    uint16_t rtt;
    uint16_t packet_len;
    uint16_t payload_len;
    uint16_t ipc_sleep_time;
};
