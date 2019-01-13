//
// Created by michal on 13.01.19.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <limits.h>

//number of semaphores a process can have in use at one time
//#define _POSIX_SEM_NSEMS_MAX	256

//number of bytes in a filename, not including the terminating null
//#define	_POSIX_NAME_MAX		14

//sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);

//  0 - locked
// >0 - unlocked


int main() {
    printf("proc1: Start\n");
    // Forward slash at the beginning of the name follows POSIX convention
    // If semaphore already exists, value argument has no effect
    sem_t* sem = sem_open("/simplefs_test", O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 2);

    printf("proc1: Sem wait\n");
    sem_wait(sem);
    printf("proc1: Sem locked\n");
    for (int i = 0; i < 10; ++i) {
        sleep(1);
        printf("proc1: sleep... %d\n", i + 1);
    }
    sem_post(sem);
    printf("proc1: Sem unlocked\n");

    if (sem_close(sem) == -1) {
        printf("proc1: sem_close failed");
    } else {
        printf("proc1: Sem closed\n");
    }

    return 0;
}