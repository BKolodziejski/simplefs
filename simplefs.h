//
// Created by kolo on 12.01.19.
//

#ifndef SIMPLEFS_SIMPLEFS_H
#define SIMPLEFS_SIMPLEFS_H

int simplefs_open(char* name, int mode, int flags);
int simplefs_unlink(char* name);
int simplefs_mkdir(char* name);
int simplefs_read(int fd, char* buf, int len);
int simplefs_write(int fd, char* buf, int len);

#define ERR_SIMPLEFS_TOO_MANY_FILES_OPEN -1
#define ERR_INVALID_FD -2
#define ERR_FILENAME_NOT_FOUND -3
#define ERR_NOT_ENOUGH_SPACE -4
#define ERR_MAX_FILES_IN_DIR_REACHED -5
#define ERR_RESOURCE_BUSY -6

#endif //SIMPLEFS_SIMPLEFS_H
