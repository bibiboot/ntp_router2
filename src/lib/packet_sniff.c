#include "cntp_handler.h"
#include "filter.h"
#include "my_rawsocket_ts.h"
#include "packet_sniff.h"
#include "packet_print.h"

void *packet_capture(void *args)
{
    struct msghdr msg; //add this in globals.h
    struct iovec entry;// add this in globals.h
    struct control control;//add this in globals.h
    char *payload;
    struct sockaddr_in from_addr;
    int num_bytes_read;
    int read_socket_fd;
    int err_packet;
    int packet_size;
    struct timeval recv_usr;

    struct custom_header *hdr;//similar to AB's custom_packet_header
    struct timestamp recv_kern;
    struct timestamp time_diff;
    struct timestamp *from_packet_kern;
    struct interface *arg;//similar to AB's struct receiver_arg

    arg = (struct interface *)args;

    //arg->inf_index = create_socket(arg->inf_name,&(arg->sk));
    //already created in main.c

    payload = malloc(globals.config.payload_len);
    memset(payload, 0, globals.config.payload_len);

    printf(KMAG "Sniffing started\n" RESET);

    read_socket_fd = create_recv_rawsocket_ts(arg->inf_name);

    set_promisc(arg->inf_name, read_socket_fd);

    setup_raw_msghdr(&msg,
                 &entry,
                 &control,
                 payload,
                 globals.config.payload_len,
                 &from_addr);

    while(1)
    {
        num_bytes_read = recv_rawpacket_ts(read_socket_fd, &msg, 0, &err_packet, &recv_kern);

        if (num_bytes_read < 0){
            printf("Error receiving\n");
            exit(1);
        } else if (err_packet) {
            printf("Error packet. Exiting\n");
            continue;
        }

        gettimeofday(&recv_usr, 0);

        hdr = (struct custom_header*)payload;

        if (is_cntp(payload)) {

            printf(KMAG "CNTP PACKET RECEIVED \n" RESET);

	    //packet_print(payload, num_bytes_read);
            unsigned long kernel_recv_time = (unsigned long)recv_kern.sec * SECONDS + recv_kern.fsec * NANOSECONDS;
            unsigned long user_recv_time = (unsigned long)recv_usr.tv_sec * SECONDS + recv_usr.tv_usec * MICROSECONDS;

            printf("[Kernel][ %lu ] : Recieved DRTT request\n", kernel_recv_time);
            printf("[User  ][ %lu ] : Sending DRTT response\n", user_recv_time);
            printf("[Len   ][ %d  ] : Packet length receieved\n", num_bytes_read);

            cntp_handler(&recv_kern,payload,args, num_bytes_read);

        } else {
            /*
            printf("packet received: user space ts:%ld.%06ld:received %d bytes\n",
                        (long)recv_usr.tv_sec, (long)recv_usr.tv_usec, ret);
	    printf(KMAG "Ignoring Packet Not CNTP\n" RESET);
            */
	}
    }
}
