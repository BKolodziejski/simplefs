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
    printf("proc2: Start\n");
    sem_t* sem = sem_open("/simplefs_test", 0);

    printf("proc2: Sem wait\n");
    sem_wait(sem);
    printf("proc2: Sem locked\n");
    for (int i = 0; i < 10; ++i) {
        sleep(1);
        printf("proc2: sleep... %d\n", i);
    }
    sem_post(sem);
    printf("proc2: Sem freed\n");

    if (sem_close(sem) == -1) {
        printf("proc2: sem_close failed");
    } else {
        printf("proc2: Sem closed\n");
    }

    return 0;
}