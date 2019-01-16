#ifndef SIMPLEFS_SIMPLEFS_WRITE_H
#define SIMPLEFS_SIMPLEFS_WRITE_H

#include "../simplefs.h"
#include "utils.h"
#include <assert.h>
#include <fcntl.h>

void Write_UnopenedFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    assert(simplefs_write(100, buf, 1) == ERR_INVALID_FD);
};

void Write_ClosedFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    int fd = simplefs_open("/file01", O_RDWR, O_CREAT);
    assert(fd >= 0);

    simplefs_close(fd);

    assert(simplefs_write(fd, buf, 1) == ERR_INVALID_FD);
};

void Write_NegativeFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    assert(simplefs_write(-1, buf, 1) == ERR_INVALID_FD);
};

void Write_ReadOnlyFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    int fd = simplefs_open("/file01", O_RDWR, O_CREAT);
    simplefs_close(fd);

    fd = simplefs_open("/file01", O_RDONLY, 0);

    assert(simplefs_write(fd, buf, 1) == ERR_INVALID_FD_MODE);
};

void Write_DirectoryFd_ErrorCodeReturned() {
    char buf[] = {'X'};
    int fd = simplefs_open("/foo_dir", O_RDWR, O_CREAT);
    assert(fd >= 0);

    assert(simplefs_write(fd, buf, 1) == ERR_WRITE_WITH_DIR_FD_DISALLOWED);

    simplefs_close(fd);
};

void Write_ReadOnlyFile_ErrorCodeReturned() {
    int fd = simplefs_open("/foo_dir/file03", O_RDWR, O_CREAT);
    assert(fd >= 0);
    char buf[] = {'X', 'Y', 'Z'};
    int bufLen = 3;

    assert(simplefs_write(fd, buf, bufLen) == ERR_ACCESS_DENIED);
}

void Write_NewFile_DataWritten() {
    int fd = simplefs_open("/file40", O_RDWR, O_CREAT);
    assert(fd >= 0);
    char buf[] = {'X', 'Y', 'Z'};
    int bufLen = 3;

    assert(simplefs_write(fd, buf, bufLen) == bufLen);

    char readBuf[bufLen];

    assert(simplefs_read(fd, readBuf, bufLen) == bufLen);
    for (int i = 0; i < bufLen; i++) {
        assert(buf[i] == readBuf[i]);
    }
}

void Write_NewFileWriteOnlyMode_DataWritten() {
    int fd = simplefs_open("/file40", O_WRONLY, O_CREAT);
    assert(fd >= 0);
    char buf[] = {'X', 'Y', 'Z'};
    int bufLen = 3;

    assert(simplefs_write(fd, buf, bufLen) == bufLen);
    simplefs_close(fd);

    fd = simplefs_open("/file40", O_RDONLY, 0);
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

    int fd = simplefs_open("/file41", O_WRONLY, O_CREAT);

    int bytesWritten = simplefs_write(fd, buf, fileSize);
    assert(bytesWritten == fileSize);
    simplefs_close(fd);

    __uint8_t readBuf[fileSize];
    fd = simplefs_open("/file41", O_RDONLY, 0);
    assert(simplefs_read(fd, readBuf, fileSize) == fileSize);


    for (int i = 0; i < fileSize; i++) {
        assert(buf[i] == readBuf[i]);
    }

    simplefs_close(fd);
}

void Write_ExistingFileWithData_DataReplaced() {
    int fd = simplefs_open("/file42", O_RDWR, O_CREAT);
    char buf[] = {'U', 'W', 'X', 'Y', 'Z'};
    int bufLen = 5;
    simplefs_write(fd, buf, bufLen);

    simplefs_close(fd);

    fd = simplefs_open("/file42", O_RDWR, 0);
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

    simplefs_close(fd);
}

void Write_TwoFiles_EachFilesDataWritten() {
    int fd1 = simplefs_open("/file46", O_RDWR, O_CREAT);
    int fd2 = simplefs_open("/file47", O_RDWR, O_CREAT);
    char buf1[] = {'X', 'Y', 'Z'};
    char buf2[] = {'D', 'E', 'F', 'G'};
    int buf1Len = 3;
    int buf2Len = 4;
    char readBuf1[buf1Len];
    char readBuf2[buf2Len];


    assert(simplefs_write(fd1, buf1, buf1Len) == buf1Len);
    assert(simplefs_write(fd2, buf2, buf2Len) == buf2Len);

    assert(simplefs_read(fd1, readBuf1, buf1Len) == buf1Len);
    for (int i = 0; i < buf1Len; i++) {
        assert(buf1[i] == readBuf1[i]);
    }

    assert(simplefs_read(fd2, readBuf2, buf2Len) == buf2Len);
    for (int i = 0; i < buf2Len; i++) {
        assert(buf2[i] == readBuf2[i]);
    }
}

void Write_ReachBlockCountLimit_DataWritten() {
    int fd = simplefs_open("/file66", O_RDWR, O_CREAT);
    assert(fd >= 0);
    char buf[1000*4096];
    int bufLen = 1000*4096;

    assert(simplefs_write(fd, buf, bufLen) == ERR_NOT_ENOUGH_SPACE);
}


#endif //SIMPLEFS_SIMPLEFS_WRITE_H
