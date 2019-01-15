//
// Created by kolo on 12.01.19.
//

#ifndef SIMPLEFS_SIMPLEFS_H
#define SIMPLEFS_SIMPLEFS_H

int simplefs_open(char* name, int mode, int flags);
int simplefs_close(int fd);
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
#define ERR_INVALID_FD_MODE -7
#define ERR_PATH_TOO_LONG -8
#define ERR_FILENAME_TOO_LONG -9
#define ERR_DIRECTORY_NOT_FOUND -10
#define ERR_INVALID_LEN -11
#define ERR_FILE_ALREADY_EXISTS -12
#define ERR_WRITE_WITH_DIR_FD_DISALLOWED -14

#endif //SIMPLEFS_SIMPLEFS_H
