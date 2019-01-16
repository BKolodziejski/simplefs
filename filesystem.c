//
// Created by kolo on 12.01.19.
//

#include <math.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "consts.h"
#include "filesystem.h"
#include "patheval.h"
#include "simplefs.h"
#include "sem.h"

int writeFile(SimplefsIndex inodeIndex, void* buf, uint32_t startPos, uint32_t len, uint8_t apiCall);

static uint64_t blockIndexToBitmapPosition(SimplefsIndex blockIndex) ;

int reserveBlocks(int blocks, SimplefsIndex* destination);

static uint64_t blockIndexToPosition(SimplefsIndex blockIndex) {
    return SIMPLEFS_INODE_COUNT * sizeof(Inode) + SIMPLEFS_BLOCK_COUNT * sizeof(uint8_t) + blockIndex * sizeof(Block);
}

void debugPrint() {
    printf("------------------------------");
    int fd = open(SIMPLEFS_PATH, O_RDONLY);
    for (int i = 0; i < SIMPLEFS_INODE_COUNT; ++i) {
        Inode inode;
        read(fd, &inode, sizeof(Inode));
        if (inode.isUsed) {
            printf("Inode %d\n", i);
            printf("Size %ld\n", inode.fileSize);
            int64_t size = inode.fileSize;
            SimplefsIndex blockIndex = inode.firstBlockIndex;
            int fdPerInode = open(SIMPLEFS_PATH, O_RDONLY);
            char buf[SIMPLEFS_BLOCK_SIZE];
            while (size > 0) {
                printf("Block Index: %d\n", blockIndex);
                printf("Block position: %ld\n", blockIndexToPosition(blockIndex));
                lseek(fdPerInode, blockIndexToPosition(blockIndex), SEEK_SET);
                read(fdPerInode, buf, SIMPLEFS_BLOCK_SIZE);
                read(fdPerInode, &blockIndex, sizeof(SimplefsIndex));
                size -= SIMPLEFS_BLOCK_SIZE;
            }
            close(fdPerInode);
        }
    }
}

void createDefaultSimplefs() {
    int fd = open(SIMPLEFS_PATH, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

    if(fd == UNIX_OPEN_FAILED_CODE) {
        exit(ERR_CANNOT_OPEN_CONTAINER);
    }

    int fileSystemContainerSize = SIMPLEFS_INODE_COUNT * sizeof(Inode)
                                  + SIMPLEFS_BLOCK_COUNT * sizeof(uint8_t)
                                  + SIMPLEFS_BLOCK_COUNT * sizeof(Block);

    ftruncate(fd, fileSystemContainerSize);

    Inode root = {0, SIMPLEFS_FILETYPE_DIR, 1, 0, ROOT_INODE_INDEX, 0};
    write(fd, &root, sizeof(Inode));
    // set first block to used by root
    lseek(fd, blockIndexToBitmapPosition(0), SEEK_SET);
    uint8_t blockIsTaken = 1;
    write(fd, &blockIsTaken, sizeof(uint8_t));
    Directory emptyDir = {};
    writeFile(ROOT_INODE_INDEX, &emptyDir, 0, sizeof(Directory), 0);
    close(fd);
}

static uint64_t inodeIndexToPosition(SimplefsIndex inodeIndex) {
    return inodeIndex * sizeof(Inode);
}

static uint64_t blockIndexToBitmapPosition(SimplefsIndex blockIndex) {
    return SIMPLEFS_INODE_COUNT * sizeof(Inode) + blockIndex * sizeof(uint8_t);
}

static Inode getInode(int fd, SimplefsIndex index) {
    uint64_t inodePosition = inodeIndexToPosition(index);
    Inode inode = {};
    lseek(fd, inodePosition, SEEK_SET);
    read(fd, &inode, sizeof(Inode));
    return inode;
}

/**
 * @param type file type
 * @param freeInode new inode index
 * @return 0 - OK; ERR_NOT_ENOUGH_SPACE - no free inode or block; ERR_RESOURCE_BUSY - inode table or block bitmap is locked
 */
int reserveNextFreeInode(uint8_t type, SimplefsIndex* freeInode) {
    if (lockInodeTable()) {
        return ERR_RESOURCE_BUSY;
    }
    Inode inode = {};
    int fd = open(SIMPLEFS_PATH, O_RDWR);
    for (SimplefsIndex i = 0; i < SIMPLEFS_INODE_COUNT; ++i) {
        read(fd, &inode, sizeof(Inode));
        if (!inode.isUsed) {
            lseek(fd, -sizeof(Inode), SEEK_CUR);
            inode.isUsed = 1;
            inode.fileType = type;
            inode.fileSize = 0;
            int status;
            if ((status = reserveBlocks(1, &inode.firstBlockIndex))) {
                close(fd);
                unlockInodeTable();
                return status;
            }
            write(fd, &inode, sizeof(Inode));
            *freeInode = i;
            close(fd);
            unlockInodeTable();
            return 0;
        }
    }
    close(fd);
    unlockInodeTable();
    return ERR_NOT_ENOUGH_SPACE;
}

uint64_t readFile(SimplefsIndex inodeIndex, void* whereTo, uint32_t startPos, uint32_t len) {
    uint32_t currentBufferOffset = 0;
    int fd = open(SIMPLEFS_PATH, O_RDONLY);
    Inode inode = getInode(fd, inodeIndex);
    uint64_t remainingSizeToRead = len < inode.fileSize ? len : inode.fileSize;
    uint64_t readDataSize = remainingSizeToRead;
    uint64_t currentFileSystemPos = blockIndexToPosition(inode.firstBlockIndex);
    uint32_t currentFilePos = 0;
    SimplefsIndex currentBlockIndex;

    uint64_t remainingSizeInCurrentBlock = SIMPLEFS_BLOCK_SIZE;

    while (currentFilePos != startPos) {
        remainingSizeInCurrentBlock = SIMPLEFS_BLOCK_SIZE;
        lseek(fd, currentFileSystemPos, SEEK_SET);
        uint32_t positionDelta = startPos - currentFilePos;
        uint32_t dataToSkipSize = positionDelta > SIMPLEFS_BLOCK_SIZE ? SIMPLEFS_BLOCK_SIZE : positionDelta;
        lseek(fd, dataToSkipSize, SEEK_CUR);
        remainingSizeInCurrentBlock -= dataToSkipSize;
        currentFilePos += dataToSkipSize;
        if (remainingSizeInCurrentBlock == 0) {
            read(fd, &currentBlockIndex, sizeof(SimplefsIndex));
            currentFileSystemPos = blockIndexToPosition(currentBlockIndex);
            remainingSizeInCurrentBlock = SIMPLEFS_BLOCK_SIZE;
        } else {
            currentFileSystemPos += dataToSkipSize;
        }
    }

    while(remainingSizeToRead != 0) {
        lseek(fd, currentFileSystemPos, SEEK_SET);
        uint64_t dataToReadSize = remainingSizeToRead > remainingSizeInCurrentBlock ? remainingSizeInCurrentBlock : remainingSizeToRead;

        currentBufferOffset += read(fd, (whereTo + currentBufferOffset), dataToReadSize);
        remainingSizeToRead -= dataToReadSize;

        if (remainingSizeToRead != 0) {
            read(fd, &currentBlockIndex, sizeof(SimplefsIndex));
            currentFileSystemPos = blockIndexToPosition(currentBlockIndex);
        }
        remainingSizeInCurrentBlock = SIMPLEFS_BLOCK_SIZE;
    }

    close(fd);

    return readDataSize;
}

int calculateRequiredNumberOfBlocks(uint64_t currentSize, uint64_t newSize) {
    if (newSize <= currentSize) {
        return 0;
    }
    uint64_t remainingSizeInLastBlock = currentSize == 0 ? SIMPLEFS_BLOCK_SIZE : SIMPLEFS_BLOCK_SIZE - (currentSize % SIMPLEFS_BLOCK_SIZE);
    if((newSize - currentSize) <= remainingSizeInLastBlock) {
        return 0;
    }
    return (int) ceil((double)(newSize - currentSize - remainingSizeInLastBlock) / SIMPLEFS_BLOCK_SIZE);
}

// TODO: handle writeFile() return values in all usages
int writeFile(SimplefsIndex inodeIndex, void* buf, uint32_t startPos, uint32_t len, u_int8_t apiCall) {
    // TODO handle fd = -1 !
    int fd = open(SIMPLEFS_PATH, O_RDWR);
    Inode inode = getInode(fd, inodeIndex);

    if(apiCall && inode.fileType == SIMPLEFS_FILETYPE_DIR) {
        return ERR_WRITE_WITH_DIR_FD_DISALLOWED;
    }

    int requiredNewBlocks = calculateRequiredNumberOfBlocks(inode.fileSize, len + startPos);
    SimplefsIndex reservedBlocks[requiredNewBlocks];

    int status;
    if ((status = reserveBlocks(requiredNewBlocks, reservedBlocks))) {
        close(fd);
        return status;
    }
    int currentReservedBlockIndex = 0;
    uint32_t currentBufferOffset = 0;
    uint64_t currentFileSystemPos = blockIndexToPosition(inode.firstBlockIndex);
    uint32_t currentFilePos = 0;
    uint64_t remainingSizeInCurrentBlock = SIMPLEFS_BLOCK_SIZE;
    int64_t remainingSizeToEOF = inode.fileSize;
    uint64_t remainingSizeToWrite = len;
    SimplefsIndex currentBlockIndex;

    // go to starting position
    while (currentFilePos != startPos) {
        remainingSizeInCurrentBlock = SIMPLEFS_BLOCK_SIZE;
        lseek(fd, currentFileSystemPos, SEEK_SET);
        uint32_t positionDelta = startPos - currentFilePos;
        uint32_t dataToSkipSize = positionDelta > SIMPLEFS_BLOCK_SIZE ? SIMPLEFS_BLOCK_SIZE : positionDelta;
        lseek(fd, dataToSkipSize, SEEK_CUR);
        remainingSizeInCurrentBlock -= dataToSkipSize;
        currentFilePos += dataToSkipSize;
        remainingSizeToEOF -= dataToSkipSize;

        if (currentFilePos != startPos) {
            read(fd, &currentBlockIndex, sizeof(SimplefsIndex));
            currentFileSystemPos = blockIndexToPosition(currentBlockIndex);
        } else {
            currentFileSystemPos += dataToSkipSize;
        }
    }

    // write data
    while(remainingSizeToWrite != 0) {
        if (remainingSizeToEOF <= 0 && currentBufferOffset != 0) {
            SimplefsIndex newBlockIndex = reservedBlocks[currentReservedBlockIndex++];
            write(fd, &newBlockIndex, sizeof(SimplefsIndex));
            currentFileSystemPos = blockIndexToPosition(newBlockIndex);
        }
        lseek(fd, currentFileSystemPos, SEEK_SET);
        uint64_t dataToWriteSize = remainingSizeToWrite > remainingSizeInCurrentBlock ? remainingSizeInCurrentBlock : remainingSizeToWrite;

        int64_t writtenSize = write(fd, (buf + currentBufferOffset), dataToWriteSize);
        currentBufferOffset += writtenSize;
        remainingSizeToWrite -= writtenSize;
        remainingSizeToEOF -= writtenSize;

        if (remainingSizeToWrite != 0 && remainingSizeToEOF > 0) {
            read(fd, &currentBlockIndex, sizeof(SimplefsIndex));
            currentFileSystemPos = blockIndexToPosition(currentBlockIndex);
        }

        remainingSizeInCurrentBlock = SIMPLEFS_BLOCK_SIZE;
    }
    // update size
    if (startPos + len - inode.fileSize > 0) {
        inode.fileSize = startPos + len - inode.fileSize;
        lseek(fd, inodeIndexToPosition(inodeIndex), SEEK_SET);
        write(fd, &inode, sizeof(Inode));
    }
    close(fd);
    return currentBufferOffset;
}

/**
 * @param howMany number of blocks to reserve
 * @param reserved table of reserved blocks' indices
 * @return 0 - OK; ERR_NOT_ENOUGH_SPACE - not enough blocks free; ERR_RESOURCE_BUSY - block bitmap semaphore is locked
 */
int reserveBlocks(int howMany, SimplefsIndex* reserved) {
    if(lockBlockBitmap()) {
        return ERR_RESOURCE_BUSY;
    }
    uint8_t blockIsTaken;
    int howManyFound = 0;

    int fd = open(SIMPLEFS_PATH, O_RDWR);
    int blockBitmapOffset = SIMPLEFS_INODE_COUNT * sizeof(Inode);
    lseek(fd, blockBitmapOffset, SEEK_SET);

    for (SimplefsIndex i = 0; i < SIMPLEFS_BLOCK_COUNT && howMany != howManyFound; ++i) {
        read(fd, &blockIsTaken, sizeof(uint8_t));
        if (!blockIsTaken) {
            reserved[howManyFound] = i;
            ++howManyFound;
        }
    }

    if (howManyFound != howMany) {
        unlockBlockBitmap();
        return ERR_NOT_ENOUGH_SPACE;
    }

    blockIsTaken = 1;
    for (int i = 0; i < howManyFound; ++i) {
        lseek(fd, blockIndexToBitmapPosition(reserved[i]), SEEK_SET);
        write(fd, &blockIsTaken, sizeof(uint8_t));
    }

    close(fd);

    unlockBlockBitmap();
    return 0;
}

/**
 * @param parentDirInodeIndex
 * @param name name of the new directory
 * @return 0 - OK; ERR_NOT_ENOUGH_SPACE, ERR_RESOURCE_BUSY, ERR_MAX_FILES_IN_DIR_REACHED - errors;
 */
int makeDir(SimplefsIndex parentDirInodeIndex, char* name) {
    if (lockInode(parentDirInodeIndex)) {
        return ERR_RESOURCE_BUSY;
    }
    int fsDescriptor = open(SIMPLEFS_PATH, O_RDWR);
    Directory dir = {};
    readFile(parentDirInodeIndex, &dir, 0, sizeof(Directory));
    for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
        if (!dir.files[i].isUsed) {
            strcpy(dir.files[i].filename, name);
            SimplefsIndex newDirInode = 0;
            int status;
            if ((status = reserveNextFreeInode(SIMPLEFS_FILETYPE_DIR, &newDirInode))) {
                close(fsDescriptor);
                unlockInode(parentDirInodeIndex);
                return status;
            }
            dir.files[i].inodeIndex = newDirInode;
            dir.files[i].isUsed = 1;
            Directory emptyDir = {};
            DirectoryRecord parent = {"..", parentDirInodeIndex, 1};
            DirectoryRecord thisDir = {".", dir.files[i].inodeIndex, 1};
            emptyDir.files[0] = parent;
            emptyDir.files[1] = thisDir;
            writeFile(dir.files[i].inodeIndex, &emptyDir, 0, sizeof(Directory), 0);
            writeFile(parentDirInodeIndex, &dir, 0, sizeof(Directory), 0);
            close(fsDescriptor);
            unlockInode(parentDirInodeIndex);
            return 0;
        }
    }

    close(fsDescriptor);
    unlockInode(parentDirInodeIndex);
    return ERR_MAX_FILES_IN_DIR_REACHED;
}

int createFile(SimplefsIndex parentDirInodeIndex, char* name, SimplefsIndex* createdFileInodeIndex) {
    if (lockInode(parentDirInodeIndex)) {
        return ERR_RESOURCE_BUSY;
    }
    int fsDescriptor = open(SIMPLEFS_PATH, O_RDWR);
    Directory dir = {};
    readFile(parentDirInodeIndex, &dir, 0, sizeof(Directory));
    for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
        if (!dir.files[i].isUsed) {
            strcpy(dir.files[i].filename, name);
            SimplefsIndex newFileInode;
            int status;
            if ((status = reserveNextFreeInode(SIMPLEFS_FILETYPE_FILE, &newFileInode))) {
                close(fsDescriptor);
                unlockInode(parentDirInodeIndex);
                return status;
            }

            int retVal = 0;
            if ((lockInode(newFileInode))) {
                retVal = ERR_FILE_CREATED_RESOURCE_BUSY;
            }
            *createdFileInodeIndex = newFileInode;
            dir.files[i].inodeIndex = newFileInode;
            dir.files[i].isUsed = 1;
            writeFile(parentDirInodeIndex, &dir, 0, sizeof(Directory), 0);
            close(fsDescriptor);
            unlockInode(parentDirInodeIndex);
            return retVal;
        }
    }
    unlockInode(parentDirInodeIndex);
    return ERR_MAX_FILES_IN_DIR_REACHED;
}

/**
 * @param parentDirInodeIndex
 * @param fileName name of the file or directory to unlink
 * @return 0 - OK; ERR_FILENAME_NOT_FOUND - filename doesn't exist in directory;
 *         ERR_RESOURCE_BUSY - parent directory inode or file inode locked;
 */
int unlinkFile(SimplefsIndex parentDirInodeIndex, char* fileName) {
    if (lockInode(parentDirInodeIndex)) {
        return ERR_RESOURCE_BUSY;
    }
    // entered parent dir inode critical section

    Directory parentDir = {};
    readFile(parentDirInodeIndex, &parentDir, 0, sizeof(Directory));

    int fileIndex = -1;

    // find file inode in directory by filename
    for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
        if (parentDir.files[i].isUsed && strcmp(parentDir.files[i].filename, fileName) == 0) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1) {
        unlockInode(parentDirInodeIndex);
        return ERR_FILENAME_NOT_FOUND;
    }

    SimplefsIndex inodeIndex = parentDir.files[fileIndex].inodeIndex; // inode of file to unlink // TODO fix SEGFAULT
    if (lockInode(inodeIndex)) {
        unlockInode(parentDirInodeIndex);
        return ERR_RESOURCE_BUSY;
    }
    // entered file inode critical section

    if (lockInodeTable()) {
        unlockInode(inodeIndex);
        unlockInode(parentDirInodeIndex);
        return ERR_RESOURCE_BUSY;
    }
    // entered inode table critical section

    if (lockBlockBitmap()) {
        unlockInodeTable();
        unlockInode(inodeIndex);
        unlockInode(parentDirInodeIndex);
        return ERR_RESOURCE_BUSY;
    }
    // entered block bitmap critical section

    // Mark file as unused in parent dir
    parentDir.files[fileIndex].isUsed = 0;
    writeFile(parentDirInodeIndex, &parentDir, 0, sizeof(Directory), 0);

    int fsDescriptor = open(SIMPLEFS_PATH, O_RDWR);

    Inode inode = {}; // inode of file to unlink
    lseek(fsDescriptor, inodeIndexToPosition(inodeIndex), SEEK_SET);
    read(fsDescriptor, &inode, sizeof(Inode));

    if (inode.fileType == SIMPLEFS_FILETYPE_DIR) {
        Directory dir = {}; // directory to unlink
        readFile(inodeIndex, &dir, 0, sizeof(Directory));
        for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
            if (dir.files[i].isUsed && strcmp(dir.files[i].filename, "..") != 0 && strcmp(dir.files[i].filename, ".") != 0) {
                unlinkFile(inodeIndex, dir.files[i].filename);
            }
        }
    }

    // Mark file inode as unused in inode table
    lseek(fsDescriptor, inodeIndexToPosition(inodeIndex), SEEK_SET);
    inode.isUsed = 0;
    write(fsDescriptor, &inode, sizeof(Inode));

    // Mark file's blocks as unused in block bitmap
    int64_t remainingFileSize = inode.fileSize;
    SimplefsIndex currentBlockIndex = inode.firstBlockIndex;
    SimplefsIndex nextBlockIndex;
    uint8_t isNotUsed = 0;
    do {
        lseek(fsDescriptor, blockIndexToPosition(currentBlockIndex) + SIMPLEFS_BLOCK_SIZE, SEEK_SET);
        read(fsDescriptor, &nextBlockIndex, sizeof(SimplefsIndex));
        lseek(fsDescriptor, blockIndexToBitmapPosition(currentBlockIndex), SEEK_SET);
        write(fsDescriptor, &isNotUsed, sizeof(uint8_t));
        currentBlockIndex = nextBlockIndex;
        remainingFileSize -= SIMPLEFS_BLOCK_SIZE;
    } while (remainingFileSize > 0);
    close(fsDescriptor);

    unlockBlockBitmap();
    unlockInodeTable();
    unlockInode(inodeIndex);
    unlockInode(parentDirInodeIndex);

    return 0;
}

void simplefsInit() {
    remove(SIMPLEFS_PATH); // start with clean file system

    int fsDescriptor = open(SIMPLEFS_PATH, O_RDONLY);
    if (fsDescriptor == UNIX_OPEN_FAILED_CODE) {
        createDefaultSimplefs();
    }

    // debug stuff
    makeDir(0, "child");
    Directory c1 = {};
    readFile(1, &c1, 0, sizeof(Directory));

    unlinkFile(0, "child");
    Directory c2 = {};
    readFile(1, &c2, 0, sizeof(Directory));

    makeDir(0, "child2"); // 1 (inode number)
    makeDir(0, "child3"); // 2
    makeDir(0, "child4"); // 3
    makeDir(0, "child5"); // 4
    makeDir(0, "child6"); // 5
    makeDir(0, "child7"); // 6

    makeDir(1, "child8"); // 7 => /child2/child8
    Directory c9 = {};
    readFile(1, &c9, 0, sizeof(Directory));
//    debugPrint();

    makeDir(2, "child9"); // 8 => /child3/child9
    Directory c10 = {};
    readFile(1, &c10, 0, sizeof(Directory));
//    debugPrint();

    SimplefsIndex ch2Idx = evaluatePath("/child2/../child2");
    Directory child2 = {};
    readFile(ch2Idx, &child2, 0, sizeof(Directory));

    SimplefsIndex ch8Idx = evaluatePath("/child2/../child2/child8");
    Directory child8 = {};
    readFile(ch8Idx, &child8, 0, sizeof(Directory));

    Directory root = {};
    readFile(0, &root, 0, sizeof(Directory));
    char fname[65];
    Directory root1 = {};
    readFile(1, &root1, 0, sizeof(Directory));
    Directory root2 = {};
    readFile(2, &root2, 0, sizeof(Directory));
    int lastNotedSlashOffset = getFilename("/child2/../child3/child8", fname);
    SimplefsIndex x = evaluatePathForParent("/child2/../child3/child8", lastNotedSlashOffset);

    debugPrint();
    unlinkFile(0, "child2");
    debugPrint();

    SimplefsIndex index2 = evaluatePath("/child2/child8");

    close(fsDescriptor);
}

