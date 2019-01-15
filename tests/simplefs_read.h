#ifndef SIMPLEFS_SIMPLEFS_READ_H
#define SIMPLEFS_SIMPLEFS_READ_H

#include <assert.h>
#include "simplefs_open.h"

void Read_UnopenedFd_ErrorCodeReturned() {
    char buf[1];
    assert(simplefs_read(10, buf, 1) == ERR_INVALID_FD);
};

void Read_ClosedFd_ErrorCodeReturned() {
    char buf[1];
    int fd = simplefs_open("/file01", O_RDWR, O_CREAT);
    assert(fd >= 0);

    simplefs_close(fd);

    assert(simplefs_read(fd, buf, 1) == ERR_INVALID_FD);
};

void Read_WriteOnlyFd_ErrorCodeReturned() {
    char buf[1];
    int fd = simplefs_open("/foo_dir/bar_file", O_WRONLY, 0);

    assert(simplefs_read(fd, buf, 1) == ERR_INVALID_FD_MODE);

    simplefs_close(fd);
};

void Read_NegativeFd_ErrorCodeReturned() {
    char buf[1];
    assert(simplefs_read(-1, buf, 1) == ERR_INVALID_FD);
};

void Read_NegativeLen_ErrorCodeReturned() {
    char buf[1];
    int fd = simplefs_open("/foo_dir/file02", O_RDONLY, 0);

    assert(simplefs_read(fd, buf, -1) == ERR_INVALID_LEN);

    simplefs_close(fd);
};

void Read_File_DataRead() {
    char buf[4];
    int fd = simplefs_open("/foo_dir/file02", 0, 0);

    assert(simplefs_read(fd, buf, 4) == 4);
    assert(buf[0] == 'L');
    assert(buf[1] == 'o');
    assert(buf[2] == 'r');
    assert(buf[3] == 'e');
};

void Read_ReadOnlyFile_DataRead() {
    char buf[4];
    int fd = simplefs_open("/foo_dir/file03", 0, 0);

    assert(simplefs_read(fd, buf, 4) == 4);
    assert(buf[0] == 'a');
    assert(buf[1] == 'b');
    assert(buf[2] == 'c');
    assert(buf[3] == 'd');
};


#endif //SIMPLEFS_SIMPLEFS_READ_H
