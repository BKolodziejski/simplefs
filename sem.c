//
// Created by michal on 13.01.19.
//

#include <assert.h>
#include "sem.h"

// Convert milliseconds to timespec struct
static struct timespec* ms2ts(struct timespec* ts, unsigned long ms) {
    ts->tv_sec = ms / 1000;
    ts->tv_nsec = (ms % 1000) * 1000000;
    return ts;
}

static int semaphoresInitialised = 0;
static sem_t* countingSem;
static sem_t* blockBitmapSem;
static sem_t* inodeTableSem;

static void initializeIfNeeded() {
    if (semaphoresInitialised) {
        return;
    }

    countingSem = sem_open(SIMPLEFS_SEM_COUNTING_NAME, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO,
                           SIMPLEFS_MAX_INODE_SEM_COUNT);
    blockBitmapSem = sem_open(SIMPLEFS_SEM_BLOCK_BITMAP_NAME, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    inodeTableSem = sem_open(SIMPLEFS_SEM_INODE_TABLE_NAME, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);

    semaphoresInitialised = 1;
}

void getInodeSemaphoreName(char* buf, SimplefsIndex inodeIndex) {
    char temp[15];
    strcpy(temp, SIMPLEFS_SEM_INODE_PREFIX);
    strcat(temp, "%d");
    sprintf(buf, temp, inodeIndex);
}

static int lockSem(sem_t* sem) {
    struct timespec ts;
    if (sem_timedwait(sem, ms2ts(&ts, SIMPLEFS_SEM_WAIT_TIME)) == -1) {
        return SIMPLEFS_SEM_LOCK_FAILED;
    };
    return 0;
}

int lockInode(SimplefsIndex inodeIndex) {
    initializeIfNeeded();

    if (lockSem(countingSem) == SIMPLEFS_SEM_LOCK_FAILED) {
        return SIMPLEFS_SEM_ALL_BUSY;
    }

    // entered counting-semaphore critical section

    char semName[15];
    getInodeSemaphoreName(semName, inodeIndex);
    sem_t* inodeSem = sem_open(semName, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);

    if (lockSem(inodeSem) == SIMPLEFS_SEM_LOCK_FAILED) {
        sem_post(countingSem);
        // left counting-semaphore critical section
        return SIMPLEFS_SEM_LOCK_FAILED;
    }

    // entered inode critical section
    return 0;
}

int lockBlockBitmap() {
    initializeIfNeeded();
    return lockSem(blockBitmapSem);
}

int lockInodeTable() {
    initializeIfNeeded();
    return lockSem(inodeTableSem);
}

void unlockInode(SimplefsIndex inodeIndex) {
    assert(semaphoresInitialised);
    char semName[15];
    getInodeSemaphoreName(semName, inodeIndex);
    sem_t* inodeSem = sem_open(semName, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    sem_post(inodeSem);
    // left inode critical section

    sem_close(inodeSem);
    sem_unlink(semName);

    sem_post(countingSem);
    // left counting-semaphore critical section
}

void unlockBlockBitmap() {
    assert(semaphoresInitialised);
    sem_post(blockBitmapSem);
}
void unlockInodeTable() {
    assert(semaphoresInitialised);
    sem_post(inodeTableSem);
}
