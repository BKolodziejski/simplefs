//
// Created by kolo on 12.01.19.
//
#include "simplefs.h"
#include "consts.h"
#include "filesystem.h"

typedef struct FdData {
    int isOpen;
    int inodeNumber;
    int filePosition;
} DescriptorData;

static DescriptorData fdToData[SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS];

static int fdArrayInitialized = 0;

static int getNextFreeFd() {
    for (int i = 0; i < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS; ++i) {
        if (!fdToData[i].isOpen) {
            return i;
        }
    }

    return SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS;
}

void initializeIfNeeded() {
    if (fdArrayInitialized) {
        return;
    }

    for (int i = 0; i < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS; ++i) {
        fdToData[i].isOpen = 0;
        fdToData[i].inodeNumber = 0;
        fdToData[i].filePosition = 0;
    }

    simplefsInit();

    fdArrayInitialized = 1;
}

int simplefs_open(char* name, int mode) {
    initializeIfNeeded();
    // TODO: semaphores
    int fd = getNextFreeFd();
    if (fd == SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS) {
        return ERR_SIMPLEFS_TOO_MANY_FILES_OPEN;
    }
    // TODO: set inode number
    fdToData[fd].isOpen = 1;
    return fd;
}

int simplefs_unlink(char* name) {
    initializeIfNeeded();
    // TODO: semaphores
}

int simplefs_mkdir(char* name) {
    initializeIfNeeded();
    // TODO: semaphores
}

int simplefs_read(int fd, char* buf, int len) {
    initializeIfNeeded();
    // TODO: semaphores
    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }
}

int simplefs_write(int fd, char* buf, int len) {
    initializeIfNeeded();
    // TODO: semaphores
    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }
}


