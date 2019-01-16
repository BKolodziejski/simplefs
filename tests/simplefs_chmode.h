#ifndef SIMPLEFS_SIMPLEFS_CHMODE_H
#define SIMPLEFS_SIMPLEFS_CHMODE_H

#include "simplefs_mkdir.h"

void Chmode_NonExistingFile_ErrorCodeReturned() {
    assert(simplefs_chmode("/non_exist_file/new_dir", SFS_READ_WRITE) == ERR_FILENAME_NOT_FOUND);
}

void Chmode_SetFileReadOnly_ModeChanged() {
    assert(simplefs_chmode("/foo_dir/chmode_test_file", SFS_READ_WRITE) == 0);
}

void Chmode_SetFileWriteOnly_ModeChanged() {
    assert(simplefs_chmode("/foo_dir/chmode_test_file", SFS_WRITE) == 0);
}

void Chmode_SetFileReadWrite_ModeChanged() {
    assert(simplefs_chmode("/foo_dir/chmode_test_file", SFS_READ_WRITE) == 0);
}

#endif //SIMPLEFS_SIMPLEFS_CHMODE_H
