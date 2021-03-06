#ifndef SIMPLEFS_UTILS_H
#define SIMPLEFS_UTILS_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../simplefs.h"

#define ERROR_CANNOT_OPEN_SRC_FILE -100
#define ERROR_SRC_FILE_READ_FAILED -110
#define ERROR_WRITING_TEST_DATA_SIMPLEFS_OPEN_FAILED -120
#define ERROR_WRITING_TEST_DATA_SIMPLEFS_WRITE_FAILED -130

#define SIMPLEFS_FILE_OPEN_FAILED -140
#define SIMPLEFS_FILE_READ_FAILED -150
#define ERROR_CANNOT_OPEN_DST_FILE -160
#define ERROR_DST_FILE_WRITE_FAILED -170

int writeFileToSimpleFs(const char *srcFilename, char *dstFilename) {
    FILE *file = fopen(srcFilename, "rb");
    if (file == NULL) return ERROR_CANNOT_OPEN_SRC_FILE;

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    __uint8_t buf[fileSize];
    int bytesRead = fread(buf, 1, fileSize, file);
    fclose(file);
    if (bytesRead != fileSize) return ERROR_SRC_FILE_READ_FAILED;

    int fd = simplefs_open(dstFilename, O_RDWR, O_CREAT);
    if (fd < 0) return ERROR_WRITING_TEST_DATA_SIMPLEFS_OPEN_FAILED;

    int bytesWritten = simplefs_write(fd, buf, fileSize);
    if (bytesWritten != fileSize) return ERROR_WRITING_TEST_DATA_SIMPLEFS_WRITE_FAILED;

    simplefs_close(fd);

    return 0;
}

int readFileFromSimpleFs(char *srcFilename, const char *dstFilename, int fileSize) {
    int fd = simplefs_open(srcFilename, O_RDONLY, 0);
    if (fd < 0) return SIMPLEFS_FILE_OPEN_FAILED;

    __uint8_t buf[fileSize];
    int bytesRead = simplefs_read(fd, buf, fileSize);
    if (bytesRead != fileSize) return SIMPLEFS_FILE_READ_FAILED;
    simplefs_close(fd);


    FILE *file = fopen(dstFilename, "wb");
    if (file == NULL) return ERROR_CANNOT_OPEN_DST_FILE;

    int bytesWritten = fwrite(buf, 1, fileSize, file);
    fclose(file);
    if (bytesWritten != fileSize) return ERROR_DST_FILE_WRITE_FAILED;

    return 0;
}

int getFilesize(char *filename) {
    struct stat result;
    if (stat(filename, &result) != 0)
        return -1;
    return result.st_size;
}


int writeFileToSimpleFsDebug(const char *srcFilename, char *dstFilename, int executorId) {
    FILE *file = fopen(srcFilename, "rb");
    if (file == NULL) return ERROR_CANNOT_OPEN_SRC_FILE;

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    __uint8_t buf[fileSize];
    int bytesRead = fread(buf, 1, fileSize, file);
    fclose(file);
    if (bytesRead != fileSize) return ERROR_SRC_FILE_READ_FAILED;

    int fd = simplefs_open(dstFilename, O_RDWR, O_CREAT);
    if (fd < 0) return ERROR_WRITING_TEST_DATA_SIMPLEFS_OPEN_FAILED;

    printf("PID=%d starting write\n", executorId);
    int bytesWritten = simplefs_write(fd, buf, fileSize);
    sleep(1); // TODO
    printf("PID=%d write finished\n", executorId);
    if (bytesWritten != fileSize) return ERROR_WRITING_TEST_DATA_SIMPLEFS_WRITE_FAILED;

    simplefs_close(fd);

    return 0;
}

int readFileFromSimpleFsDebug(char *srcFilename, const char *dstFilename, int fileSize, int executorId) {
    int fd = simplefs_open(srcFilename, O_RDONLY, 0);
    if (fd < 0) return SIMPLEFS_FILE_OPEN_FAILED;

    __uint8_t buf[fileSize];
    printf("PID=%d starting read\n", executorId);
    int bytesRead = simplefs_read(fd, buf, fileSize);
    sleep(2); // TODO
    printf("PID=%d read finished\n", executorId);
    if (bytesRead != fileSize) return SIMPLEFS_FILE_READ_FAILED;
    simplefs_close(fd);


    FILE *file = fopen(dstFilename, "wb");
    if (file == NULL) return ERROR_CANNOT_OPEN_DST_FILE;

    int bytesWritten = fwrite(buf, 1, fileSize, file);
    fclose(file);
    if (bytesWritten != fileSize) return ERROR_DST_FILE_WRITE_FAILED;

    return 0;
}

#endif //SIMPLEFS_UTILS_H
