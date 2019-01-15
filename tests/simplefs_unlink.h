#ifndef SIMPLEFS_SIMPLEFS_UNLINK_H
#define SIMPLEFS_SIMPLEFS_UNLINK_H


#include <assert.h>
#include "../simplefs.h"

void Unlink_NonExistingDirectory_ErrorCodeReturned() {
    assert(simplefs_unlink("/non_exist_dir") == ERR_FILENAME_NOT_FOUND);
}

void Unlink_NonExistingFile_ErrorCodeReturned() {
    assert(simplefs_unlink("/non_exist_file") == ERR_FILENAME_NOT_FOUND);
}

void Unlink_NonExistingFileInDirectory_ErrorCodeReturned() {
    assert(simplefs_unlink("/non_exist_dir/file") == ERR_FILENAME_NOT_FOUND);
}

void Unlink_Directory_DirectoryRemoved() {
    simplefs_mkdir("/foo_dir/rm_dir");

    assert(simplefs_unlink("/foo_dir/rm_dir") == 0);

    assert(simplefs_open("/foo_dir/rm_dir", 0, 0) == ERR_FILENAME_NOT_FOUND);
}

void Unlink_File_FileRemoved() {
    int fd = simplefs_open("/foo_dir/rm_file", O_RDWR, O_CREAT);
    assert(fd >= 0);
    simplefs_close(fd);

    assert(simplefs_unlink("/foo_dir/rm_file") == 0);

    assert(simplefs_open("/foo_dir/rm_file", 0, 0) == ERR_FILENAME_NOT_FOUND);
}

void Unlink_FileOpened_ErrorCodeReturned() {
    int fd = simplefs_open("/foo_dir/temp_file", O_RDWR, O_CREAT);

    assert(simplefs_unlink("/foo_dir/temp_file") < 0);

    simplefs_close(fd);
}

void Unlink_DirectoryHasOpenedFiles_ErrorCodeReturned() {
    simplefs_mkdir("/foo_dir/tmp_dir");
    int fd = simplefs_open("/foo_dir/tmp_dir/temp_file", 0, O_CREAT);

    assert(simplefs_unlink("/foo_dir/tmp_dir") < 0);

    simplefs_close(fd);
}


#endif //SIMPLEFS_SIMPLEFS_UNLINK_H
