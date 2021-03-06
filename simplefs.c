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
    SimplefsIndex inodeNumber;
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

    if(strlen(path) > SIMPLEFS_MAX_PATH_LENGTH) {
        return ERR_PATH_TOO_LONG;
    }

    int fd = getNextFreeFd();
    if (fd == SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS) {
        return ERR_SIMPLEFS_TOO_MANY_FILES_OPEN;
    }

    SimplefsIndex fileInode = evaluatePath(path);
    if(flags & O_CREAT && fileInode == SIMPLEFS_INODE_COUNT) {
        if(mode == O_RDONLY) {
            return ERR_INVALID_FD_MODE;
        }

        char filename[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
        int lastNotedSlashOffset = getFilename(path, filename);
        if(lastNotedSlashOffset == ERR_FILENAME_TOO_LONG) {
            return ERR_FILENAME_TOO_LONG;
        }
        SimplefsIndex index = evaluatePathForParent(path, lastNotedSlashOffset);
        if(index == SIMPLEFS_INODE_COUNT) {
            return ERR_DIRECTORY_NOT_FOUND;
        }
        int status;
        if((status = createFile(index, filename, &fdToData[fd].inodeNumber))) {
            return status;
        };
        // entered created file inode critical section
    } else if (fileInode == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    } else {
        if (mode == O_WRONLY || mode == O_RDWR) {
            if ((lockInode(fileInode))) {
                return ERR_RESOURCE_BUSY;
            }
            // entered opened file inode critical section
        }
        fdToData[fd].inodeNumber = fileInode;
    }

    fdToData[fd].isOpen = 1;
    fdToData[fd].mode = mode;
    return fd;
}

int simplefs_close(int fd) {
    initializeIfNeeded();

    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }

    if (fdToData[fd].mode == O_WRONLY || fdToData[fd].mode == O_RDWR) {
        unlockInode(fdToData[fd].inodeNumber);
        //left file inode critical section
    }

    fdToData[fd].isOpen = 0;
    fdToData[fd].inodeNumber = 0;
    fdToData[fd].filePosition = 0;
    fdToData[fd].mode = 0;
    return 0;
}

int simplefs_unlink(char* path) {
    initializeIfNeeded();
    char filename[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
    int lastNotedSlashOffset = getFilename(path, filename);
    if(lastNotedSlashOffset == ERR_FILENAME_TOO_LONG) {
        return ERR_FILENAME_TOO_LONG;
    }
    SimplefsIndex parentDirInodeIndex = evaluatePathForParent(path, lastNotedSlashOffset);
    if (parentDirInodeIndex == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    }

    SimplefsIndex fileInode = evaluatePath(path);
    for (int i = 0; i < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS; ++i) {
        if (fdToData[i].inodeNumber == fileInode && fdToData[i].isOpen) {
            return ERR_RESOURCE_BUSY;
        }
    }

    return unlinkFile(parentDirInodeIndex, filename);
}

int simplefs_mkdir(char* path) {
    initializeIfNeeded();
    if(evaluatePath(path) != SIMPLEFS_INODE_COUNT) {
        return ERR_FILE_ALREADY_EXISTS;
    }
    char filename[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
    int lastNotedSlashOffset = getFilename(path, filename);
    if(lastNotedSlashOffset == ERR_FILENAME_TOO_LONG) {
        return ERR_FILENAME_TOO_LONG;
    }
    SimplefsIndex index = evaluatePathForParent(path, lastNotedSlashOffset);
    if (index == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    }

    return makeDir(index, filename);
}

int simplefs_read(int fd, char* buf, int len) {
    initializeIfNeeded();
    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }
    if(fdToData[fd].mode != O_RDONLY && fdToData[fd].mode != O_RDWR) {
        return ERR_INVALID_FD_MODE;
    }

    if(len < 0) {
        return ERR_INVALID_LEN;
    }

    uint64_t bytesRead = readFile(fdToData[fd].inodeNumber, buf, fdToData[fd].filePosition, len, 1);
    fdToData[fd].filePosition += bytesRead;
    return bytesRead;
}

int simplefs_write(int fd, char* buf, int len) {
    initializeIfNeeded();
    if (!(fd >= 0 && fd < SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS && fdToData[fd].isOpen)) {
        return ERR_INVALID_FD;
    }
    if(fdToData[fd].mode != O_WRONLY && fdToData[fd].mode != O_RDWR) {
        return ERR_INVALID_FD_MODE;
    }

    int bytesWritten = writeFile(fdToData[fd].inodeNumber, buf, fdToData[fd].filePosition, len, 1);
    fdToData[fd].filePosition += bytesWritten;
    return bytesWritten;
}

int simplefs_chmode(char *path, int mode) {
    initializeIfNeeded();

    if(mode != SFS_READ && mode != SFS_WRITE && mode != SFS_READ_WRITE) {
        return ERR_INVALID_ACCESS_MODE;
    }

    SimplefsIndex fileInode = evaluatePath(path);
    if (fileInode == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    }

    return changeMode(fileInode ,mode);
}


