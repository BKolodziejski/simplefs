//
// Created by kolo on 12.01.19.
//

#ifndef SIMPLEFS_FILESYSTEM_H
#define SIMPLEFS_FILESYSTEM_H

#include <stdint.h>
#include "consts.h"

#define SIMPLEFS_FILETYPE_DIR 0
#define SIMPLEFS_FILETYPE_FILE 1

typedef struct block_ {
    uint8_t data[SIMPLEFS_BLOCK_SIZE];
    uint32_t nextBlockIndex;
} Block;

typedef struct inode_ {
    uint8_t accesses;
    uint8_t fileType;
    uint8_t isUsed;
    uint32_t ownerId;
    uint32_t firstBlockIndex;
    uint64_t fileSize;
} Inode;

typedef struct directory_record_ {
    char filename[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
    uint32_t inodeIndex;
} DirectoryRecord;

typedef struct directory_ {
    DirectoryRecord files[SIMPLEFS_MAX_FILES_IN_DIR];
} Directory;

void createDefaultSimplefs();

void simplefsInit();


#endif //SIMPLEFS_FILESYSTEM_H
