#ifndef SIMPLEFS_UTILS_H
#define SIMPLEFS_UTILS_H

#include <stdio.h>
#include "../simplefs.h"

#define ERROR_CANNOT_OPEN_SRC_FILE -100
#define ERROR_SRC_FILE_READ_FAILED -110
#define ERROR_WRITING_TEST_DATA_SIMPLEFS_OPEN_FAILED -120
#define ERROR_WRITING_TEST_DATA_SIMPLEFS_WRITE_FAILED -130

#define SIMPLEFS_FILE_OPEN_FAILED -140
#define SIMPLEFS_FILE_READ_FAILED -150
#define ERROR_CANNOT_OPEN_DST_FILE -160
#define ERROR_DST_FILE_WRITE_FAILED -170

int writeFileToSimpleFs(const char *srcFilename, const char *dstFilename) {
    FILE *file = fopen(srcFilename, "rb");
    if (file == NULL) return ERROR_CANNOT_OPEN_SRC_FILE;

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    __uint8_t buf[fileSize];
    int bytesRead = fread(buf, 1, fileSize, file);
    fclose(file);
    if (bytesRead != fileSize) return ERROR_SRC_FILE_READ_FAILED;

    int fd = simplefs_open(dstFilename, 0);
    if (fd < 0) return ERROR_WRITING_TEST_DATA_SIMPLEFS_OPEN_FAILED;

    int bytesWritten = simplefs_write(fd, buf, fileSize);
    if (bytesWritten != fileSize) return ERROR_WRITING_TEST_DATA_SIMPLEFS_WRITE_FAILED;

    // TODO simplefs_close(fd);

    return 0;
}

int readFileFromSimpleFs(const char *srcFilename, const char *dstFilename, int fileSize) {
    int fd = simplefs_open(srcFilename, 0);
    if (fd < 0) return SIMPLEFS_FILE_OPEN_FAILED;

    __uint8_t buf[fileSize];
    int bytesRead = simplefs_read(fd, buf, fileSize);
    if (bytesRead != fileSize) return SIMPLEFS_FILE_READ_FAILED;
    // TODO simplefs_close(fd);


    FILE *file = fopen(dstFilename, "rb");
    if (file == NULL) return ERROR_CANNOT_OPEN_DST_FILE;

    int bytesWritten = fwrite(buf, 1, fileSize, file);
    fclose(file);
    if (bytesWritten != fileSize) return ERROR_DST_FILE_WRITE_FAILED;

    return 0;
}


#endif //SIMPLEFS_UTILS_H
