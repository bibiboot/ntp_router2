#include "globals.h"

/*
 * Initializing the global structure with default values
 */
struct globals globals = {
    .config = CONFIG_INIT,
    .hashl = NULL           /*This is null as per documentation of uthash*/
};
