//
// Created by kolo on 12.01.19.
//
#include "simplefs.h"
#include "consts.h"
#include "filesystem.h"
#include "patheval.h"
#include "sem.h"

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

static void initializeIfNeeded() {
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

int simplefs_open(char* path, int mode) {
    initializeIfNeeded();
    // TODO: semaphores
    int fd = getNextFreeFd();
    if (fd == SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS) {
        return ERR_SIMPLEFS_TOO_MANY_FILES_OPEN;
    }
    fdToData[fd].inodeNumber = evaluatePath(path);
    if (fdToData[fd].inodeNumber == ERR_FILENAME_NOT_FOUND) {
        return ERR_FILENAME_NOT_FOUND;
    }
    fdToData[fd].isOpen = 1;
    return fd;
}

int simplefs_unlink(char* path) {
    initializeIfNeeded();
    // TODO: semaphores
    char filename[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
    SimplefsIndex index = evaluatePathForParent(path, filename);
    if (index == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    }
    return unlinkFile(index, filename);
}

int simplefs_mkdir(char* path) {
    initializeIfNeeded();
    // TODO: semaphores
    char filename[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
    SimplefsIndex index = evaluatePathForParent(path, filename);
    if (index == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    }

    return makeDir(index, filename);
}

int simplefs_read(int fd, char* buf, int len) {
    initializeIfNeeded();
    // TODO: semaphores
    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }

    return readFile(fdToData[fd].inodeNumber, buf, fdToData[fd].filePosition, len);
}

int simplefs_write(int fd, char* buf, int len) {
    initializeIfNeeded();
    // TODO: semaphores
    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }

    return writeFile(fdToData[fd].inodeNumber, buf, fdToData[fd].filePosition, len);
}


