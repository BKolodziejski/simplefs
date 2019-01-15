//
// Created by michal on 13.01.19.
//

#ifndef SIMPLEFS_SEM_H
#define SIMPLEFS_SEM_H

#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include "consts.h"
#include "filesystem.h"

// Semaphore names must be no longer than _POSIX_NAME_MAX (14)
#define SIMPLEFS_SEM_BLOCK_BITMAP_NAME "/sfs_bitmap"
#define SIMPLEFS_SEM_INODE_TABLE_NAME "/sfs_inode_tab"
#define SIMPLEFS_SEM_COUNTING_NAME "/sfs_counting"
#define SIMPLEFS_SEM_INODE_PREFIX "/sfs_"

#define SIMPLEFS_SEM_LOCK_FAILED -1
#define SIMPLEFS_SEM_ALL_BUSY -2

void getInodeSemaphoreName(char* buf, SimplefsIndex inodeIndex);

/**
 * @param inodeIndex
 * @return 0 - OK;
 *         SIMPLEFS_SEM_ALL_BUSY - all inode semaphores are in use;
 *         SIMPLEFS_SEM_LOCK_FAILED - inode semaphore of inodeIndex is locked;
 */
int lockInode(SimplefsIndex inodeIndex);

/**
 * @return 0 - OK; SIMPLEFS_SEM_LOCK_FAILED - block bitmap semaphore is locked;
 */
int lockBlockBitmap();

/**
 * @return 0 - OK; SIMPLEFS_SEM_LOCK_FAILED - inode table semaphore is locked;
 */
int lockInodeTable();

void unlockInode(SimplefsIndex inodeIndex);
void unlockBlockBitmap();
void unlockInodeTable();

#endif //SIMPLEFS_SEM_H
