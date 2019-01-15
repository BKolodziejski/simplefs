#ifndef SIMPLEFS_SIMPLEFS_CHMODE_H
#define SIMPLEFS_SIMPLEFS_CHMODE_H

#include "simplefs_mkdir.h"

void Chmode_NonExistingDirectory_ErrorCodeReturned() {
    // assert(simplefs_chmode("/non_exist_dir/new_dir") < 0);

}

void Chmode_NonExistingFile_ErrorCodeReturned() {

}

void Chmode_FileOpened_ErrorCodeReturned() {

}

void Chmode_SetFileReadOnly_ModeChanged() {
    // try writing to file

}

void Chmode_SetFileReadWrite_ModeChanged() {
    // try writing to file

}

void Chmode_SetDirectorReadOnly_ModeChanged() {
    // try creating a file

}

void Chmode_SetDirectorReadWrite_ModeChanged() {
    // creating and remove file

}

#endif //SIMPLEFS_SIMPLEFS_CHMODE_H
