#include "my_rawsocket_ts.h"
	
int
create_recv_rawsocket_ts(char* inf)                                                       
{                                                                               
    int s;                 
    int so_timestamping_flags = 0; 
    int so_timestamp = 0;                                                       
    int so_timestampns = 0;
    int enabled = 1;
    int val;                                                                    
    socklen_t len;

    struct ifreq hwtstamp;                                                      
    struct hwtstamp_config hwconfig, hwconfig_requested;                        
    struct sockaddr_in addr; 
    struct sockaddr_ll socket_address;    
     
    //This should go in config
    so_timestamping_flags |= SOF_TIMESTAMPING_RX_SOFTWARE;                      
    so_timestamping_flags |= SOF_TIMESTAMPING_SOFTWARE;                                                   
  
    //s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);   
    s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
    if (s == -1)                                                            
    {                                                                       
        printf("ERROR: socket(). Exiting...\n");                        
        perror("SOCKET:");                                              
        exit(1);                                                        
    }   
    //set_socket_inf(s, inf, socket_address);
    setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, inf, strlen(inf));  
    
    socket_address.sll_family   = AF_PACKET;
    socket_address.sll_protocol = htons(ETH_P_ALL);
    socket_address.sll_ifindex = inf_to_index_raw(inf);
    if (bind(s,
            (struct sockaddr *)&socket_address,
            sizeof(struct sockaddr_ll)) < 0){
        printf("bind: %s\n", strerror(errno));
        exit(1);
    }


    
    memset(&hwtstamp, 0, sizeof(hwtstamp));                                     
    strncpy(hwtstamp.ifr_name, inf, sizeof(hwtstamp.ifr_name));           
    hwtstamp.ifr_data = (void *)&hwconfig;                                      
    memset(&hwconfig, 0, sizeof(hwconfig));                                     
    hwconfig.tx_type =                                                          
        (so_timestamping_flags & SOF_TIMESTAMPING_TX_HARDWARE) ?                
        HWTSTAMP_TX_ON : HWTSTAMP_TX_OFF;                                       
    hwconfig.rx_filter =                                                        
        (so_timestamping_flags & SOF_TIMESTAMPING_RX_HARDWARE) ?                
        HWTSTAMP_FILTER_PTP_V1_L4_SYNC : HWTSTAMP_FILTER_NONE;                  
    hwconfig_requested = hwconfig;                                              
    if (ioctl(s, SIOCSHWTSTAMP, &hwtstamp) < 0) {                            
        if ((errno == EINVAL || errno == ENOTSUP) &&                            
            hwconfig_requested.tx_type == HWTSTAMP_TX_OFF &&                    
            hwconfig_requested.rx_filter == HWTSTAMP_FILTER_NONE)               
            printf("SIOCSHWTSTAMP: disabling hardware time stamping not possible\n");
        else{                                                                               
            printf("SIOCSHWTSTAMP: %s\n", strerror(errno));                                 
            exit(1);                                                                    
        }                                                                     
    }                                                                           
    printf("SIOCSHWTSTAMP: tx_type %d requested, got %d; rx_filter %d requested, got %d\n",
           hwconfig_requested.tx_type, hwconfig.tx_type,                        
           hwconfig_requested.rx_filter, hwconfig.rx_filter);    
    
   


    if (so_timestamp &&                                                         
        setsockopt(s, SOL_SOCKET, SO_TIMESTAMP,                              
               &enabled, sizeof(enabled)) < 0){
        printf("setsockopt SO_TIMESTAMP: %s\n", strerror(errno));
        exit(1);
    }                                  
                                                                                
    if (so_timestampns &&                                                       
        setsockopt(s, SOL_SOCKET, SO_TIMESTAMPNS,                            
               &enabled, sizeof(enabled)) < 0){
        printf("setsockopt SO_TIMESTAMPNS: %s\n", strerror(errno));
        exit(1);
    }                                  
                                                                                
    if (so_timestamping_flags &&                                                
        setsockopt(s, SOL_SOCKET, SO_TIMESTAMPING,                           
               &so_timestamping_flags,                                          
               sizeof(so_timestamping_flags)) < 0){
        printf("setsockopt SO_TIMESTAMPING: %s\n", strerror(errno));
        exit(1);
    }                              
                                                                                
    /* request IP_PKTINFO for debugging purposes */                             
    if (setsockopt(s, SOL_IP, IP_PKTINFO,                                    
               &enabled, sizeof(enabled)) < 0)                                  
        printf("%s: %s\n", "setsockopt IP_PKTINFO", strerror(errno));                                                          


    /* verify socket options */                                                 
    len = sizeof(val);                                                          
    if (getsockopt(s, SOL_SOCKET, SO_TIMESTAMP, &val, &len) < 0)             
        printf("%s: %s\n", "getsockopt SO_TIMESTAMP", strerror(errno));         
    else                                                                        
        printf("SO_TIMESTAMP %d\n", val);                                       
                                                                                
    if (getsockopt(s, SOL_SOCKET, SO_TIMESTAMPNS, &val, &len) < 0)           
        printf("%s: %s\n", "getsockopt SO_TIMESTAMPNS",                         
               strerror(errno));                                                
    else                                                                        
        printf("SO_TIMESTAMPNS %d\n", val);                                     
                                                                                
    if (getsockopt(s, SOL_SOCKET, SO_TIMESTAMPING, &val, &len) < 0) {        
        printf("%s: %s\n", "getsockopt SO_TIMESTAMPING",                        
               strerror(errno));                                                
    } else {                                                                    
        printf("SO_TIMESTAMPING %d\n", val);                                    
        if (val != so_timestamping_flags)                                       
            printf("   not the expected value %d\n",                            
                   so_timestamping_flags);                                      
    }                                          

  
    return s;                                                               
} 
void
setup_raw_msghdr(struct msghdr* msg, struct iovec* entry, struct control* control, 
            char *payload, int payload_len, struct sockaddr_in* from_addr)
{
    memset(msg, 0, sizeof(struct msghdr));
    msg->msg_iov = entry;                                                       
    msg->msg_iovlen = 1;                                                         
    entry->iov_base = payload;                                                      
    entry->iov_len = payload_len;                                               
    msg->msg_name = (caddr_t)from_addr;                                         
    msg->msg_namelen = sizeof(struct sockaddr_in);                                        
    msg->msg_control = control;                                                 
    msg->msg_controllen = sizeof(struct control);    
}

int 
recv_rawpacket_ts(int s, struct msghdr* msg, int recvmsg_flags, 
                  int* err_packet, struct timestamp* recv_kern_ts)
{
    int i;
    int res;
    res = recvmsg(s, msg, 0);
    //printf("In recvpacket after recvmsg\n");
    if (res < 0){
        printf("%s %s: %s\n",
               "recvmsg",
               (recvmsg_flags & MSG_ERRQUEUE) ? "error" : "regular",
               strerror(errno));
    } else {
        //printpacket(&msg, res, payload,
        //        s, recvmsg_flags);
        if ((recvmsg_flags & MSG_ERRQUEUE) == 0){
            *err_packet = 0;
            get_recv_kern_ts(msg, recv_kern_ts, res); 
            /*printf("after:regular:data:\n");
            for (i = 0; i < 256; i++)
            {
                printf("0x%x\t",(uint8_t)*(data+i));
            }
            printf("\n");*/
        }
        else{
            *err_packet = -1;
            res = -1; 
            printf("Received error packet\n");
        }
    }
    return res;
}

void 
print_rawpacket(struct msghdr *msg, int res,
            char *data,	int sock, int recvmsg_flags)
{
	struct sockaddr_in *from_addr = (struct sockaddr_in *)msg->msg_name;
	struct cmsghdr *cmsg;
	struct timeval tv;
	struct timespec ts;
	struct timeval now;

	//gettimeofday(&now, 0);

	/*printf("%ld.%06ld: received %s data, %d bytes from %s, %zu bytes control messages\n",
	       (long)now.tv_sec, (long)now.tv_usec,
	       (recvmsg_flags & MSG_ERRQUEUE) ? "error" : "regular",
	       res,
	       inet_ntoa(from_addr->sin_addr),
	       msg->msg_controllen);
    */
    printf("received %s data, %d bytes\n",
           (recvmsg_flags & MSG_ERRQUEUE) ? "error" : "regular", res);
	if ((recvmsg_flags & MSG_ERRQUEUE) == 0){
		printf("Data Data Data\n");
		printf("Data:%u\n",*(uint8_t*)data);
	}
	for (cmsg = CMSG_FIRSTHDR(msg);
	     cmsg;
	     cmsg = CMSG_NXTHDR(msg, cmsg)) {
		printf("   cmsg len %zu: ", cmsg->cmsg_len);
		switch (cmsg->cmsg_level) {
		case SOL_SOCKET:
			printf("SOL_SOCKET ");
			switch (cmsg->cmsg_type) {
			case SO_TIMESTAMP: {
				struct timeval *stamp =
					(struct timeval *)CMSG_DATA(cmsg);
				printf("SO_TIMESTAMP %ld.%06ld",
				       (long)stamp->tv_sec,
				       (long)stamp->tv_usec);
				break;
			}
			case SO_TIMESTAMPNS: {
				struct timespec *stamp =
					(struct timespec *)CMSG_DATA(cmsg);
				printf("SO_TIMESTAMPNS %ld.%09ld",
				       (long)stamp->tv_sec,
				       (long)stamp->tv_nsec);
				break;
			}
			case SO_TIMESTAMPING: {
				struct timespec *stamp =
					(struct timespec *)CMSG_DATA(cmsg);
				printf("SO_TIMESTAMPING ");
				printf("SW %ld.%09ld ",
				       (long)stamp->tv_sec,
				       (long)stamp->tv_nsec);
				stamp++;
				/* skip deprecated HW transformed */
				stamp++;
				printf("HW raw %ld.%09ld",
				       (long)stamp->tv_sec,
				       (long)stamp->tv_nsec);
				break;
			}
			default:
				printf("type %d", cmsg->cmsg_type);
				break;
			}
			break;
		case IPPROTO_IP:
			printf("IPPROTO_IP ");
			switch (cmsg->cmsg_type) {
			case IP_RECVERR: {
				struct sock_extended_err *err =
					(struct sock_extended_err *)CMSG_DATA(cmsg);
				printf("IP_RECVERR ee_errno '%s' ee_origin %d => %s",
					strerror(err->ee_errno),
					err->ee_origin,
#ifdef SO_EE_ORIGIN_TIMESTAMPING
					err->ee_origin == SO_EE_ORIGIN_TIMESTAMPING ?
					"bounced packet" : "unexpected origin"
#else
					"probably SO_EE_ORIGIN_TIMESTAMPING"
#endif
					);
				if (res < sizeof(sync))
					printf(" => truncated data?!");
				else if (!memcmp(sync, data + res - sizeof(sync),
							sizeof(sync)))
					printf(" => GOT OUR DATA BACK (HURRAY!)");
				break;
			}
			case IP_PKTINFO: {
				struct in_pktinfo *pktinfo =
					(struct in_pktinfo *)CMSG_DATA(cmsg);
				printf("IP_PKTINFO interface index %u",
					pktinfo->ipi_ifindex);
				break;
			}
			default:
				printf("##############type %d", cmsg->cmsg_type);
				break;
			}
			break;
		default:
			printf("level %d type %d",
				cmsg->cmsg_level,
				cmsg->cmsg_type);
			break;
		}
		printf("\n");
	}
}
void
get_recv_kern_ts(struct msghdr *msg, struct timestamp* recv_kern_ts, int res)
{
    //printf("in recv_kern_ts()\n");
	struct cmsghdr *cmsg;
    for (cmsg = CMSG_FIRSTHDR(msg);
	     cmsg;
	     cmsg = CMSG_NXTHDR(msg, cmsg)) {
	    //printf("   cmsg len %zu: ", cmsg->cmsg_len);
	    switch (cmsg->cmsg_level) {
	    case SOL_SOCKET:
	    	//printf("SOL_SOCKET ");
	    	switch (cmsg->cmsg_type) {
	    	case SO_TIMESTAMP: {
	    		struct timeval *stamp =
	    			(struct timeval *)CMSG_DATA(cmsg);
                recv_kern_ts->sec = (long)stamp->tv_sec;
                recv_kern_ts->fsec = (long)stamp->tv_usec;
	    		/*printf("SO_TIMESTAMP %ld.%06ld",
		    	       (long)stamp->tv_sec,
		     	       (long)stamp->tv_usec);*/
		    	break;
		    }
    		case SO_TIMESTAMPNS: {
	    		struct timespec *stamp =
		    		(struct timespec *)CMSG_DATA(cmsg);
                recv_kern_ts->sec = (long)stamp->tv_sec;
                recv_kern_ts->fsec = (long)stamp->tv_nsec;
		    	/*printf("SO_TIMESTAMPNS %ld.%09ld",
			           (long)stamp->tv_sec,
			           (long)stamp->tv_nsec);
                */
			    break;
    		}
    		case SO_TIMESTAMPING: {
	    		struct timespec *stamp =
    			(struct timespec *)CMSG_DATA(cmsg);
     			/*printf("SO_TIMESTAMPING ");
		    	printf("SW %ld.%09ld ",
			           (long)stamp->tv_sec,
			           (long)stamp->tv_nsec);*/
                recv_kern_ts->sec = (long)stamp->tv_sec;
                recv_kern_ts->fsec = (long)stamp->tv_nsec;
			    stamp++;
			    /* skip deprecated HW transformed */
			    stamp++;
			    /*printf("HW raw %ld.%09ld\n",
			       (long)stamp->tv_sec,
			       (long)stamp->tv_nsec);
                */
			    break;
		    }
		default:
		    printf("type %d", cmsg->cmsg_type);
            printf("We never anticipate this data\n");
            exit(1);
			break;
		}
		break;
	}
    }
    printf("\n");
}


int
inf_to_index_raw(char* inf)
{
	int inf_indx;
	if ((inf_indx = if_nametoindex (inf)) == 0){                   
		printf("Failed to get interface index of \'%s\'\n",inf);                                                
		perror ("if_nametoindex():");      
		exit (EXIT_FAILURE);                                                
	}
    return inf_indx;
}



int set_promisc(char *interface, int sock ) {
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface,strlen(interface)+1);
    if((ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)) {
        /*Could not retrieve flags for the
        * interface*/
        perror("Could not retrive flags for the interface");
        exit(0);
    }
    //printf("DEBUG: The interface is ::: %s\n", interface);
    //perror("DEBUG: Retrieved flags from interface successfully");

    /*now that the flags have been
    * retrieved*/
    /* set the flags to PROMISC */
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl (sock, SIOCSIFFLAGS, &ifr) == -1 ) {
        perror("Could not set the PROMISC flag:");
        exit(0);
    }
    //printf("DEBUG: Setting interface ::: %s ::: to promisc\n", interface);
    return(0);
}
