#include <stdio.h>
#include <assert.h>

#include "../simplefs.h"
#include "simplefs_write.h"
#include "simplefs_open.h"


int main() {
    simplefs_mkdir("/foo_dir");
    simplefs_open("/foo_dir/bar_file", 0); // create file


    Open_FileDoesNotExist_ErrorCodeReturned();

    Write_NegativeFd_ErrorCodeReturned();
    Write_UnopenedFd_ErrorCodeReturned();
    Write_ClosedFd_ErrorCodeReturned();
    Write_ReadOnlyFd_ErrorCodeReturned();

    Write_NewFile_DataWritten();
    Write_NewFileDataLargerThanBlockSize_DataWritten();
    Write_ExistingFileWithData_DataReplaced();
    Write_TwoFiles_EachFilesDataWritten();

    return 0;
}