#ifndef SIMPLEFS_SIMPLEFS_MKDIR_H
#define SIMPLEFS_SIMPLEFS_MKDIR_H

#include <assert.h>
#include "../simplefs.h"

void Mkdir_NonExistingPath_ErrorCodeReturned() {
    assert(simplefs_mkdir("/non_exist_dir/new_dir") == ERR_FILENAME_NOT_FOUND);
}

void Mkdir_FilenameAsDirPath_ErrorCodeReturned() {
    // TODO
    int i = simplefs_mkdir("/foo_dir/bar_file/new_dir");
    assert(i < 0);
}

void Mkdir_DirectoryExists_ErrorCodeReturned() {
    assert(simplefs_mkdir("/foo_dir") == ERR_FILE_ALREADY_EXISTS);
}

void Mkdir_FileWithSameNameExists_ErrorCodeReturned() {
    assert(simplefs_mkdir("/foo_dir/bar_file") == ERR_FILE_ALREADY_EXISTS);
}

void Mkdir_ValidPath_DirectoryCreated() {
    assert(simplefs_mkdir("/foo_dir/new_dir") == 0);
}

void Mkdir_ValidPathWithParent_DirectoryCreated() {
    assert(simplefs_mkdir("/foo_dir/../foo_dir/new_dir_2") == 0);
}


#endif //SIMPLEFS_SIMPLEFS_MKDIR_H
