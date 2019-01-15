#ifndef SIMPLEFS_SIMPLEFS_OPEN_H
#define SIMPLEFS_SIMPLEFS_OPEN_H

#include <assert.h>
#include "../simplefs.h"

void Open_FileDoesNotExist_ErrorCodeReturned() {
    assert(simplefs_open("/non_existing", 0) == ERR_FILENAME_NOT_FOUND);
}

void Open_DirectoryDoesNotExist_ErrorCodeReturned() {
    // trying to create a new file in non existing dir
    // TODO
    // assert(simplefs_open("/non_exist_dir/file", 0) ==  ERR_FILENAME_NOT_FOUND);
}

void Open_Directory_ErrorCodeReturned() {
    // TODO assert(simplefs_open("/foo_dir", 0) ==  TODO);
}

void Open_FileExists_FileOpened() {
    int fd = simplefs_open("/foo_dir/bar_file", 0);
    assert(fd >= 0);

    // TODO simplefs_close(fd)
}

void Open_FileExistsParentInPath_FileOpened() {
    int fd = simplefs_open("/foo_dir/../foo_dir/bar_file", 0);
    assert(fd >= 0);

    // TODO simplefs_close(fd)
}

void Open_FileameUsedAsDirInPath_ErrorCodeReturned() {
    // TODO
    assert(simplefs_open("/foo_dir/bar_file/xxx", 0) < 0);

    // TODO simplefs_close(fd)
}

void Open_TooLongFilename_ErrorCodeReturned() {
    // TODO
    assert(
            simplefs_open(
                    "/long_file_name_long_file_name_long_file_name_long_file_name_long_file_name",
                    0
            ) < 0

    );

    // TODO simplefs_close(fd)
}

// TODO modes
// TODO create ?

#endif //SIMPLEFS_SIMPLEFS_OPEN_H
