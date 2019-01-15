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
    int fd = simplefs_open("/file01", 0);
    assert(fd >= 0);

    // TODO simplefs_close(fd);

    assert(simplefs_read(10, buf, 1) == ERR_INVALID_FD);
};

void Read_WriteOnlyFd_ErrorCodeReturned() {
    char buf[1];
    int fd = simplefs_open("/foo_dir/bar_file", 0);

    // todo error code
    assert(simplefs_read(fd, buf, 1) < 0);
};

void Read_NegativeFd_ErrorCodeReturned() {
    char buf[1];
    assert(simplefs_read(-1, buf, 1) == ERR_INVALID_FD);
};

void Read_NegativeLen_ErrorCodeReturned() {
    char buf[1];
    int fd = simplefs_open("/foo_dir/bar_file", 0);

    // todo error code
    assert(simplefs_read(fd, buf, -1) < 0);
};

void Read_ReadOnlyFile_DataRead() {
    char buf[4];
    int fd = simplefs_open("/foo_dir/file03", 0);

    assert(simplefs_read(fd, buf, 4) == 4);
    // todo verify contents
};

void Read_ReadFile_DataRead() {
    char buf[4];
    int fd = simplefs_open("/foo_dir/file02", 0);

    assert(simplefs_read(fd, buf, 4) == 4);
    // todo verify contents
};


#endif //SIMPLEFS_SIMPLEFS_READ_H
