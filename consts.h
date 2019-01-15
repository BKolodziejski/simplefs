//
// Created by kolo on 12.01.19.
//

#ifndef SIMPLEFS_CONSTS_H
#define SIMPLEFS_CONSTS_H

#define SIMPLEFS_PATH "/home/michal/uxp1a/data/container"
#define SIMPLEFS_MAX_FILENAME_LENGTH 64
#define SIMPLEFS_MAX_PATH_LENGTH 256
#define SIMPLEFS_BLOCK_SIZE 4096
#define SIMPLEFS_BLOCK_COUNT 1000
#define SIMPLEFS_INODE_COUNT 4096
#define SIMPLEFS_MAX_FILES_IN_DIR 255
#define SIMPLEFS_MAX_OPEN_FILES_PER_PROCESS 1000

#define SIMPLEFS_MAX_INODE_SEM_COUNT 20
#define SIMPLEFS_SEM_WAIT_TIME 1000 // in milliseconds


#define UNIX_OPEN_FAILED_CODE -1

#endif //SIMPLEFS_CONSTS_H
