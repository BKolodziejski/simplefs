//
// Created by kolo on 12.01.19.
//

#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "consts.h"
#include "filesystem.h"

#define ROOT_INODE_INDEX 0

void createDefaultSimplefs() {
    int fd = open(SIMPLEFS_PATH, O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);

    int fileSystemContainerSize = SIMPLEFS_INODE_COUNT * sizeof(Inode)
                                  + SIMPLEFS_BLOCK_COUNT * sizeof(uint8_t)
                                  + SIMPLEFS_BLOCK_COUNT * sizeof(Block);

    ftruncate(fd, fileSystemContainerSize);

    Inode root = {0, SIMPLEFS_FILETYPE_DIR, 1, 0, 0, sizeof(Directory)};
    write(fd, &root, sizeof(Inode));

//    Inode defaultInode = {0, 0, 0, 0, 0};
//    for (int i = 0; i < SIMPLEFS_INODE_COUNT - 1; ++i) {
//        write(fd, &defaultInode, sizeof(Inode));
//    }
//
//    uint8_t blockIsUsed = 0;
//    for (int i = 0; i < SIMPLEFS_BLOCK_COUNT; ++i) {
//        write(fd, &blockIsUsed, sizeof(uint8_t));
//    }

    close(fd);
}

void simplefsInit() {
    int fsDescriptor = open(SIMPLEFS_PATH, O_RDONLY);
    if (fsDescriptor == UNIX_OPEN_FAILED_CODE) {
        createDefaultSimplefs();
    }
    close(fsDescriptor);
}

void readInode(uint32_t inodeIndex, uint8_t* whereTo, uint32_t startPos, uint32_t endPos) {

}
