#include "globals.h"
#include <linux/if_ether.h>

int create_socket(char *,struct sockaddr_ll *);
void set_socket_inf(int,char *,struct sockaddr_ll *);
int inf_to_index(char *);
