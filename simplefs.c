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
    int mode;
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
        fdToData[i].mode = 0;
    }

    simplefsInit();

    fdArrayInitialized = 1;
}

int simplefs_open(char* path, int mode, int flags) {
    initializeIfNeeded();
    // TODO: semaphores

    if(flags & O_CREAT && evaluatePath(path) == SIMPLEFS_INODE_COUNT) {
        if(mode == O_RDONLY) {
            return ERR_INVALID_FD_MODE;
        }

        char filename[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
        SimplefsIndex index = evaluatePathForParent(path, filename);
        int status;
        if((status = createFile(index, filename))) {
            return status;
        };
    }

    int fd = getNextFreeFd();
    if (fd == SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS) {
        return ERR_SIMPLEFS_TOO_MANY_FILES_OPEN;
    }
    fdToData[fd].inodeNumber = evaluatePath(path);
    if (fdToData[fd].inodeNumber == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    }
    fdToData[fd].isOpen = 1;
    fdToData[fd].mode = mode;
    return fd;
}

int simplefs_close(int fd) {
    initializeIfNeeded();
    // TODO: semaphores ?

    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }

    fdToData[fd].isOpen = 0;
    fdToData[fd].inodeNumber = 0;
    fdToData[fd].filePosition = 0;
    fdToData[fd].mode = 0;
    return 0;
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
    if(fdToData[fd].mode != O_RDONLY && fdToData[fd].mode != O_RDWR) {
        return ERR_INVALID_FD_MODE;
    }

    return readFile(fdToData[fd].inodeNumber, buf, fdToData[fd].filePosition, len);
}

int simplefs_write(int fd, char* buf, int len) {
    initializeIfNeeded();
    // TODO: semaphores
    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }
    if(fdToData[fd].mode != O_WRONLY && fdToData[fd].mode != O_RDWR) {
        return ERR_INVALID_FD_MODE;
    }

    return writeFile(fdToData[fd].inodeNumber, buf, fdToData[fd].filePosition, len);
}


