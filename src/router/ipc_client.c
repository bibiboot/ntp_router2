#include "ipc_client.h"
#include "globals.h"

void *start_ipc_client()
{
    int shmid;
    key_t key;
    char *shm, *s;

    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = 5678;

    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    //if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    while(1) {
        if ((globals.drtt = shmat(shmid, NULL, 0)) == (char *) -1) {
            perror("shmat");
            exit(1);
        }
        //strcpy(globals.drtt, "1270");

        /*
        * Now read what the server put in the memory.
        */
        printf("Read from memory = %s\n", globals.drtt);
        //globals.drtt = '*';

        sleep(globals.config.ipc_sleep_time);
    }

    printf("Exiting IPC\n");
    exit(0);
}
