//
// Created by kolo on 12.01.19.
//

#ifndef SIMPLEFS_FILESYSTEM_H
#define SIMPLEFS_FILESYSTEM_H

#include <stdint.h>
#include "consts.h"

#define SIMPLEFS_FILETYPE_DIR 0
#define SIMPLEFS_FILETYPE_FILE 1

#define ROOT_INODE_INDEX 0

typedef struct block_ {
    uint8_t data[SIMPLEFS_BLOCK_SIZE];
    uint32_t nextBlockIndex;
} Block;

typedef uint32_t SimplefsIndex;

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
    uint8_t isUsed;
} DirectoryRecord;

typedef struct directory_ {
    DirectoryRecord files[SIMPLEFS_MAX_FILES_IN_DIR];
} Directory;

void createDefaultSimplefs();

void simplefsInit();

uint64_t readFile(SimplefsIndex inodeIndex, void* whereTo, uint32_t startPos, uint32_t len);

int unlinkFile(SimplefsIndex parentDirInodeIndex, char* fileName);

int makeDir(SimplefsIndex parentDirInodeIndex, char* name);

int createFile(SimplefsIndex parentDirInodeIndex, char* name, SimplefsIndex* createdFileInodeIndex);

int writeFile(SimplefsIndex inodeIndex, void* buf, uint32_t startPos, uint32_t len, uint8_t apiCall);


#endif //SIMPLEFS_FILESYSTEM_H
