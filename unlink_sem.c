//
// Created by michal on 13.01.19.
//

#include <semaphore.h>
#include <stdio.h>
#include "consts.h"
#include "sem.h"

void unlinkAndPrint(const char* name) {
    if (sem_unlink(name) == -1) {
        printf("failed to unlink %s\n", name);
    } else {
        printf("unlinked %s\n", name);
    }
}

// Remove all named semaphores used by simplefs from operating system
int main() {
    unlinkAndPrint(SIMPLEFS_SEM_BLOCK_BITMAP_NAME);
    unlinkAndPrint(SIMPLEFS_SEM_INODE_TABLE_NAME);
    unlinkAndPrint(SIMPLEFS_SEM_COUNTING_NAME);

    for (SimplefsIndex i = 0; i < SIMPLEFS_INODE_COUNT; ++i) {
        char semName[15];
        getInodeSemaphoreName(semName, i);
        unlinkAndPrint(semName);
    }
    return 0;
}
