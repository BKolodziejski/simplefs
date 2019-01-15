#ifndef SIMPLEFS_SIMPLEFS_WRITE_H
#define SIMPLEFS_SIMPLEFS_WRITE_H

#include "../simplefs.h"
#include "utils.h"
#include <assert.h>

void Write_UnopenedFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    assert(simplefs_write(10, buf, 1) == ERR_INVALID_FD);
};

void Write_ClosedFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    int fd = simplefs_open("/file01", 0);
    assert(fd >= 0);

    // TODO simplefs_close(fd);

    // TODO assert(simplefs_write(10, buf, 1) == ERR_INVALID_FD);
};

void Write_NegativeFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    assert(simplefs_write(-1, buf, 1) == ERR_INVALID_FD);
};

void Write_ReadOnlyFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    // TODO int fd = simplefs_open("/file01", READ);

    // TODO assert(simplefs_write(-1, buf, 1) == ERR_INVALID_FD);
};

void Write_NewFile_DataWritten() {
    int fd = simplefs_open("/file40", 0);
    char buf[] = {'X', 'Y', 'Z'};
    int bufLen = 3;

    assert(simplefs_write(fd, buf, bufLen) == bufLen);

    char readBuf[bufLen];

    assert(simplefs_read(fd, readBuf, bufLen) == bufLen);
    for (int i = 0; i < bufLen; i++) {
        assert(buf[i] == readBuf[i]);
    }
}

void Write_NewFileDataLargerThanBlockSize_DataWritten() {
    char *srcFilename = "../tests/test-files/file01";

    FILE *file = fopen(srcFilename, "rb");
    assert(file != NULL);

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    __uint8_t buf[fileSize];
    int bytesRead = fread(buf, 1, fileSize, file);
    fclose(file);
    assert(bytesRead == fileSize);

    int fd = simplefs_open("/file41", 0);
    // TODO simplefs_close(fd);

    int bytesWritten = simplefs_write(fd, buf, fileSize);
    assert(bytesWritten == fileSize);

    __uint8_t readBuf[fileSize];
    fd = simplefs_open("/file41", 0);
    assert(simplefs_read(fd, readBuf, fileSize) == fileSize);


    for (int i = 0; i < fileSize; i++) {
        assert(buf[i] == readBuf[i]);
    }

    // TODO simplefs_close(fd);
}

void Write_ExistingFileWithData_DataReplaced() {
    int fd = simplefs_open("/file42", 0);
    char buf[] = {'U', 'W', 'X', 'Y', 'Z'};
    int bufLen = 5;
    simplefs_write(fd, buf, bufLen);

    // TODO simplefs_close(fd);

    fd = simplefs_open("/file42", 0);
    char buf2[] = {'A', 'B', 'C'};
    int buf2Len = 3;
    simplefs_write(fd, buf2, buf2Len);

    char readBuf[bufLen];

    assert(simplefs_read(fd, readBuf, bufLen) == (bufLen));
    for (int i = 0; i < buf2Len; i++) {
        assert(readBuf[i] == buf2[i]);
    }

    for (int i = buf2Len; i < bufLen; i++) {
        assert(readBuf[i] == buf[i]);
    }

    // TODO simplefs_close(fd);
}

void Write_TwoFiles_EachFilesDataWritten() {
    int fd1 = simplefs_open("/file46", 0);
    int fd2 = simplefs_open("/file47", 0);
    char buf1[] = {'X', 'Y', 'Z'};
    char buf2[] = {'D', 'E', 'F', 'G'};
    int buf1Len = 3;
    int buf2Len = 4;
    char readBuf1[buf1Len];
    char readBuf2[buf2Len];


    assert(simplefs_write(fd1, buf1, buf1Len) == buf1Len);
    assert(simplefs_write(fd1, buf2, buf2Len) == buf2Len);

    assert(simplefs_read(fd1, readBuf1, buf1Len) == buf1Len);
    for (int i = 0; i < buf1Len; i++) {
        assert(buf1[i] == readBuf1[i]);
    }

    assert(simplefs_read(fd2, readBuf2, buf2Len) == buf2Len);
    for (int i = 0; i < buf2Len; i++) {
        assert(buf2[i] == readBuf2[i]);
    }
}


#endif //SIMPLEFS_SIMPLEFS_WRITE_H
