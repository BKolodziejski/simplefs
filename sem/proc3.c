//
// Created by michal on 13.01.19.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <limits.h>

int main() {
    printf("proc3: Start\n");
    
    // The sem_unlink function removes the name of the semaphore.
    // If there are no open references to the semaphore, then it is destroyed.
    // Otherwise, destruction is deferred until the last open reference is closed.
    if (sem_unlink("/simplefs_test") == -1) {
        printf("proc3: sem_unlink failed");
    } else {
        printf("proc3: Sem unlinked\n");
    }

    return 0;
}