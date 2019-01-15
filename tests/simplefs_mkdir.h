#ifndef SIMPLEFS_SIMPLEFS_MKDIR_H
#define SIMPLEFS_SIMPLEFS_MKDIR_H

#include <assert.h>
#include "../simplefs.h"

void Mkdir_NonExistingDirectory_ErrorCodeReturned() {
    // TODO error code
    assert(simplefs_mkdir("/non_exist_dir/new_dir") < 0);
}

void Mkdir_FilenameAsDirPath_ErrorCodeReturned() {
    // TODO error code
    assert(simplefs_mkdir("/foo_dir/bar_file/new_dir") < 0);
}

void Mkdir_DirectoryExists_ErrorCodeReturned() {
    // TODO error code
    assert(simplefs_mkdir("/foo_dir") < 0);
}

void Mkdir_FileWithSameNameExists_ErrorCodeReturned() {
    // TODO error code
    assert(simplefs_mkdir("/foo_dir/bar_file") < 0);
}

void Mkdir_ValidPath_DirectoryCreated() {
    assert(simplefs_mkdir("/foo_dir/new_dir") == 0);
}

void Mkdir_ValidPathWithParent_DirectoryCreated() {
    assert(simplefs_mkdir("/foo_dir/../foo_dir/new_dir") == 0);
}


#endif //SIMPLEFS_SIMPLEFS_MKDIR_H
