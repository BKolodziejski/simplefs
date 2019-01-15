#ifndef SIMPLEFS_SIMPLEFS_OPEN_H
#define SIMPLEFS_SIMPLEFS_OPEN_H

#include <assert.h>
#include "../simplefs.h"

void Open_FileDoesNotExist_ErrorCodeReturned() {
    assert(simplefs_open("/non_existing", O_RDONLY, 0) == ERR_FILENAME_NOT_FOUND);
}

void Open_DirectoryDoesNotExist_ErrorCodeReturned() {
    // trying to create a new file in non existing dir
    assert(simplefs_open("/non_exist_dir/file", O_RDWR, O_CREAT) == ERR_DIRECTORY_NOT_FOUND);
}

void Open_Directory_ErrorCodeReturned() {
    // TODO
//    int i = simplefs_open("/foo_dir", O_RDWR, 0);
    //  int k = 0;

}

void Open_FileExists_FileOpened() {
    int fd = simplefs_open("/foo_dir/bar_file", O_RDONLY, 0);
    assert(fd >= 0);

    simplefs_close(fd);
}

void Open_FileExistsParentInPath_FileOpened() {
    int fd = simplefs_open("/foo_dir/../foo_dir/bar_file", O_RDONLY, 0);
    assert(fd >= 0);

    simplefs_close(fd);
}

void Open_FilenameUsedAsDirInPath_ErrorCodeReturned() {
    assert(simplefs_open("/foo_dir/bar_file/xxx", 0, 0) == ERR_FILENAME_NOT_FOUND);
}

void Open_TooLongFilename_ErrorCodeReturned() {
    assert(
            simplefs_open(
                    "/long_file_name_long_file_name_long_file_name_long_file_name_long_file_name",
                    O_RDWR,
                    O_CREAT
            ) == ERR_FILENAME_TOO_LONG
    );


}

void Open_TooLongPath_ErrorCodeReturned() {
    assert(
            simplefs_open(
                    "/long_file_name_longe/file_name_long_file_name_long_file_name_long_file/long_file_name_longe/file_name_long_file_name_long_file_name_long_file"
                    "/long_file_name_longe/file_name_long_file_name_long_file_name_long_file/long_file_name_longe/file_name_long_file_name_long_file_name_long_file"
                    "/long_file_name_longe/file_name_long_file_name_long_file_name_long_file/long_file_name_longe/file_name_long_file_name_long_file_name_long_file",
                    O_RDWR,
                    O_CREAT
            ) == ERR_PATH_TOO_LONG);

}

#endif //SIMPLEFS_SIMPLEFS_OPEN_H
