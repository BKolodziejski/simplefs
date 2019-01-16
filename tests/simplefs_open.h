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

void Open_ReachFilesInDirLimit_ErrorCodeReturned() {
    char filename[] = "/max_files_dir/test_00000";
    simplefs_mkdir("/max_files_dir");
    for (int i = 0; i < 500; i++) {
        sprintf(filename, "/max_files_dir/test_%d", i);
        int fd = simplefs_open(filename, O_RDWR, O_CREAT);
        if (fd < 0 && fd != ERR_MAX_FILES_IN_DIR_REACHED) {
            assert(fd >= 0);
        } else if (fd == ERR_MAX_FILES_IN_DIR_REACHED) {
            printf("files in dir reached: %d\r\n", i);
            simplefs_close(fd);
            return;
        }
        simplefs_close(fd);
    }
    assert(0);
}

void Open_ReachTooManyFilesOpenLimit_ErrorCodeReturned() {
    // requires more blocks than in default config
    simplefs_mkdir("/max_open_proc0");
    simplefs_mkdir("/max_open_proc1");
    simplefs_mkdir("/max_open_proc2");
    simplefs_mkdir("/max_open_proc3");
    simplefs_mkdir("/max_open_proc4");
    simplefs_mkdir("/max_open_proc5");
    simplefs_mkdir("/max_open_proc6");
    char filename[] = "/max_open_proc01/test_00000";
    for (int i = 0; i < 1500; i++) {
        sprintf(filename, "/max_open_proc%d/test_%d", i/250, i);
        printf("i=%d\r\n",i);
        int fd = simplefs_open(filename, O_RDWR, O_CREAT);
        if (fd < 0 && fd != ERR_SIMPLEFS_TOO_MANY_FILES_OPEN) {
            assert(fd >= 0);
        } else if (fd == ERR_SIMPLEFS_TOO_MANY_FILES_OPEN) {
            printf("open files per process reached: %d\r\n", i);
            return;
        }
    }
    assert(0);
}

void Open_ReachInodeCountLimit_ErrorCodeReturned() {
    // requires more blocks than in default config
    simplefs_mkdir("/max_inode_0");
    simplefs_mkdir("/max_inode_1");
    simplefs_mkdir("/max_inode_2");
    simplefs_mkdir("/max_inode_3");
    simplefs_mkdir("/max_inode_4");
    simplefs_mkdir("/max_inode_5");
    simplefs_mkdir("/max_inode_6");
    simplefs_mkdir("/max_inode_7");
    simplefs_mkdir("/max_inode_8");
    simplefs_mkdir("/max_inode_9");
    simplefs_mkdir("/max_inode_10");
    simplefs_mkdir("/max_inode_11");
    simplefs_mkdir("/max_inode_12");
    simplefs_mkdir("/max_inode_13");
    simplefs_mkdir("/max_inode_14");
    simplefs_mkdir("/max_inode_15");
    simplefs_mkdir("/max_inode_16");
    simplefs_mkdir("/max_inode_17");
    char filename[] = "/max_inode_00/test_00000";
    for (int i = 0; i < 4300; i++) {
        sprintf(filename, "/max_inode_%d/test_%d", i/250, i);
        printf("i=%d\r\n",i);
        int fd = simplefs_open(filename, O_RDWR, O_CREAT);
        if (fd < 0 && fd != ERR_NOT_ENOUGH_SPACE) {
            assert(fd >= 0);
        } else if (fd == ERR_NOT_ENOUGH_SPACE) {
            printf("inode count limit reached for files: %d\r\n", i);
            simplefs_close(fd);
            return;
        }
        simplefs_close(fd);
    }
    assert(0);
}


#endif //SIMPLEFS_SIMPLEFS_OPEN_H
