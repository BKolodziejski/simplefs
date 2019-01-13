//
// Created by kolo on 12.01.19.
//

#include <math.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "consts.h"
#include "filesystem.h"
#include "patheval.h"
#include "simplefs.h"

int writeFile(SimplefsIndex inodeIndex, void* buf, uint32_t startPos, uint32_t len);

static uint64_t blockIndexToBitmapPosition(SimplefsIndex blockIndex) ;

int reserveBlocks(int blocks, SimplefsIndex* destination);

void createDefaultSimplefs() {
    int fd = open(SIMPLEFS_PATH, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

    int fileSystemContainerSize = SIMPLEFS_INODE_COUNT * sizeof(Inode)
                                  + SIMPLEFS_BLOCK_COUNT * sizeof(uint8_t)
                                  + SIMPLEFS_BLOCK_COUNT * sizeof(Block);

    ftruncate(fd, fileSystemContainerSize);

    Inode root = {0, SIMPLEFS_FILETYPE_DIR, 1, 0, 0, 0};
    write(fd, &root, sizeof(Inode));
    // set first block to used by root
    lseek(fd, blockIndexToBitmapPosition(0), SEEK_SET);
    uint8_t blockIsTaken = 1;
    write(fd, &blockIsTaken, sizeof(uint8_t));
    Directory emptyDir;
    writeFile(ROOT_INODE_INDEX, &emptyDir, 0, sizeof(Directory));
    close(fd);
}

static uint64_t inodeIndexToPosition(SimplefsIndex inodeIndex) {
    return inodeIndex * sizeof(Inode);
}

static uint64_t blockIndexToPosition(SimplefsIndex blockIndex) {
    return SIMPLEFS_INODE_COUNT * sizeof(Inode) + SIMPLEFS_BLOCK_COUNT * sizeof(uint8_t) + blockIndex * sizeof(Block);
}

static uint64_t blockIndexToBitmapPosition(SimplefsIndex blockIndex) {
    return SIMPLEFS_INODE_COUNT * sizeof(Inode) + blockIndex * sizeof(uint8_t);
}

static Inode getInode(int fd, SimplefsIndex index) {
    uint64_t inodePosition = inodeIndexToPosition(index);
    Inode inode;
    lseek(fd, inodePosition, SEEK_SET);
    read(fd, &inode, sizeof(Inode));
    return inode;
}

SimplefsIndex reserveNextFreeInode(uint8_t type) {
    Inode inode;
    int fd = open(SIMPLEFS_PATH, O_RDWR);
    for (SimplefsIndex i = 0; i < SIMPLEFS_INODE_COUNT; ++i) {
        read(fd, &inode, sizeof(Inode));
        if (!inode.isUsed) {
            lseek(fd, -sizeof(Inode), SEEK_CUR);
            inode.isUsed = 1;
            inode.fileType = type;
            if (reserveBlocks(1, &inode.firstBlockIndex) == ERR_NOT_ENOUGH_SPACE) {
                return SIMPLEFS_INODE_COUNT;
            }
            write(fd, &inode, sizeof(Inode));
            close(fd);
            return i;
        }
    }
    close(fd);
    return SIMPLEFS_INODE_COUNT;
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
    uint64_t remainingSizeInLastBlock = SIMPLEFS_BLOCK_SIZE - (currentSize % SIMPLEFS_BLOCK_SIZE);
    return (int) ceil((double)(newSize - currentSize - remainingSizeInLastBlock) / SIMPLEFS_BLOCK_SIZE);
}

int writeFile(SimplefsIndex inodeIndex, void* buf, uint32_t startPos, uint32_t len) {
    int fd = open(SIMPLEFS_PATH, O_RDWR);
    Inode inode = getInode(fd, inodeIndex);

    int requiredNewBlocks = calculateRequiredNumberOfBlocks(inode.fileSize, len + startPos);
    SimplefsIndex reservedBlocks[requiredNewBlocks];


    if (reserveBlocks(requiredNewBlocks, reservedBlocks) == ERR_NOT_ENOUGH_SPACE) {
        return ERR_NOT_ENOUGH_SPACE;
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
}

int reserveBlocks(int howMany, SimplefsIndex* reserved) {
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
        return ERR_NOT_ENOUGH_SPACE;
    }

    blockIsTaken = 1;
    for (int i = 0; i < howManyFound; ++i) {
        lseek(fd, blockIndexToBitmapPosition(reserved[i]), SEEK_SET);
        write(fd, &blockIsTaken, sizeof(uint8_t));
    }

    close(fd);

    return howManyFound;
}

int makeDir(SimplefsIndex parentDirInodeIndex, char* name) {
    int fsDescriptor = open(SIMPLEFS_PATH, O_RDWR);
    Directory dir;
    readFile(parentDirInodeIndex, &dir, 0, sizeof(Directory));
    for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
        if (!dir.files[i].isUsed) {
            strcpy(dir.files[i].filename, name);
            dir.files[i].inodeIndex = reserveNextFreeInode(SIMPLEFS_FILETYPE_DIR);
            if (dir.files[i].inodeIndex == SIMPLEFS_INODE_COUNT) {
                return ERR_NOT_ENOUGH_SPACE;
            }
            dir.files[i].isUsed = 1;
            writeFile(parentDirInodeIndex, &dir, 0, sizeof(Directory));
            Directory emptyDir;
            DirectoryRecord parent = {"..", parentDirInodeIndex, 1};
            DirectoryRecord thisDir = {".", dir.files[i].inodeIndex, 1};
            emptyDir.files[0] = parent;
            emptyDir.files[1] = thisDir;
            writeFile(dir.files[i].inodeIndex, &emptyDir, 0, sizeof(Directory));
            close(fsDescriptor);
            return 0;
        }
    }
}

int createFile(SimplefsIndex parentDirInodeIndex, char* name) {
    int fsDescriptor = open(SIMPLEFS_PATH, O_RDWR);
    Directory dir;
    readFile(parentDirInodeIndex, &dir, 0, sizeof(Directory));
    for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
        if (!dir.files[i].isUsed) {
            strcpy(dir.files[i].filename, name);
            dir.files[i].inodeIndex = reserveNextFreeInode(SIMPLEFS_FILETYPE_FILE);

            if (dir.files[i].inodeIndex == SIMPLEFS_INODE_COUNT) {
                close(fsDescriptor);
                return ERR_NOT_ENOUGH_SPACE;
            }

            dir.files[i].isUsed = 1;
            writeFile(parentDirInodeIndex, &dir, 0, sizeof(Directory));
            close(fsDescriptor);
            return 0;
        }
    }
}

int unlinkFile(SimplefsIndex parentDirInodeIndex, char* fileName) {

    Directory parentDir;
    readFile(parentDirInodeIndex, &parentDir, 0, sizeof(Directory));

    SimplefsIndex inodeIndex = SIMPLEFS_INODE_COUNT;

    uint8_t isDirectory = 0;

    for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
        if (parentDir.files[i].isUsed && strcmp(parentDir.files[i].filename, fileName) == 0) {
            parentDir.files[i].isUsed = 0;
            inodeIndex = parentDir.files[i].inodeIndex;
            writeFile(parentDirInodeIndex, &parentDir, 0, sizeof(Directory));
            break;
        }
    }

    if (inodeIndex == SIMPLEFS_INODE_COUNT) {
        return ERR_FILENAME_NOT_FOUND;
    }

    int fsDescriptor = open(SIMPLEFS_PATH, O_RDWR);
    Inode inode;
    lseek(fsDescriptor, inodeIndexToPosition(inodeIndex), SEEK_SET);
    read(fsDescriptor, &inode, sizeof(Inode));

    if (inode.fileType == SIMPLEFS_FILETYPE_DIR) {
        Directory dir;
        readFile(inodeIndex, &dir, 0, sizeof(Directory));
        for (int i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
            if (dir.files[i].isUsed && strcmp(dir.files[i].filename, "..") != 0 && strcmp(dir.files[i].filename, ".") != 0) {
                unlinkFile(inodeIndex, dir.files[i].filename);
            }
        }
    }

    lseek(fsDescriptor, inodeIndexToPosition(inodeIndex), SEEK_SET);
    inode.isUsed = 0;
    write(fsDescriptor, &inode, sizeof(Inode));
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
    return 0;
}

void simplefsInit() {
    int fsDescriptor = open(SIMPLEFS_PATH, O_RDONLY);
    if (fsDescriptor == UNIX_OPEN_FAILED_CODE) {
        createDefaultSimplefs();
    }
    // debug stuff
    makeDir(0, "child");
    unlinkFile(0, "child");
    makeDir(0, "child2");
    makeDir(0, "child3");
    makeDir(0, "child4");
    makeDir(0, "child5");
    makeDir(0, "child6");
    makeDir(0, "child7");
    makeDir(1, "child8");
    makeDir(2, "child9");

    Directory root;
    readFile(0, &root, 0, sizeof(Directory));
    SimplefsIndex index = evaluatePath("/child2/child8");
    char fname[65];
    Directory root1;
    readFile(1, &root1, 0, sizeof(Directory));
    Directory root2;
    readFile(2, &root2, 0, sizeof(Directory));
    SimplefsIndex x = evaluatePathForParent("/child2/../child3/child8", fname);
    unlinkFile(0, "child2");
    SimplefsIndex index2 = evaluatePath("/child2/child8");
    close(fsDescriptor);
}

