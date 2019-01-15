#include <stdio.h>
#include <assert.h>

#include "../simplefs.h"
#include "simplefs_write.h"
#include "simplefs_open.h"
#include "simplefs_chmode.h"
#include "simplefs_unlink.h"
#include "simplefs_read.h"


int main() {
    // create sample files
    simplefs_mkdir("/foo_dir");
    simplefs_open("/foo_dir/bar_file", 0); // create file

    // write data to files
    writeFileToSimpleFs("../tests/test-files/file01", "/foo_dir/file02");
    writeFileToSimpleFs("../tests/test-files/file01", "/foo_dir/file03");
    //simplefs_chmode("/foo_dir/file03", READ only)


    // simplefs_write
    Write_NegativeFd_ErrorCodeReturned();
    Write_UnopenedFd_ErrorCodeReturned();
    Write_ClosedFd_ErrorCodeReturned();
    Write_ReadOnlyFd_ErrorCodeReturned();
    Write_NewFile_DataWritten();
    Write_NewFileDataLargerThanBlockSize_DataWritten();
    Write_ExistingFileWithData_DataReplaced();
    Write_TwoFiles_EachFilesDataWritten();

    // simplefs_open
    Open_FileDoesNotExist_ErrorCodeReturned();
    Open_DirectoryDoesNotExist_ErrorCodeReturned();
    Open_Directory_ErrorCodeReturned();
    Open_FileExists_FileOpened();
    Open_FileExistsParentInPath_FileOpened();
    Open_FilenameUsedAsDirInPath_ErrorCodeReturned();
    Open_TooLongFilename_ErrorCodeReturned();

    // simplefs_chmode
    Chmode_NonExistingDirectory_ErrorCodeReturned();
    Chmode_NonExistingFile_ErrorCodeReturned();
    Chmode_FileOpened_ErrorCodeReturned();
    Chmode_SetFileReadOnly_ModeChanged();
    Chmode_SetFileReadWrite_ModeChanged();
    Chmode_SetDirectorReadOnly_ModeChanged();
    Chmode_SetDirectorReadWrite_ModeChanged();

    // simplefs_mkdir
    Mkdir_NonExistingDirectory_ErrorCodeReturned();
    Mkdir_FilenameAsDirPath_ErrorCodeReturned();
    Mkdir_DirectoryExists_ErrorCodeReturned();
    Mkdir_FileWithSameNameExists_ErrorCodeReturned();
    Mkdir_ValidPath_DirectoryCreated();
    Mkdir_ValidPathWithParent_DirectoryCreated();

    // simplefs_unlink
    Unlink_NonExistingDirectory_ErrorCodeReturned();
    Unlink_NonExistingFile_ErrorCodeReturned();
    Unlink_NonExistingFileInDirectory_ErrorCodeReturned();
    Unlink_Directory_DirectoryRemoved();
    Unlink_File_FileRemoved();
    Unlink_FileOpened_ErrorCodeReturned();
    Unlink_DirectoryHasOpenedFiles_ErrorCodeReturned();

    // simplefs_read
    Read_UnopenedFd_ErrorCodeReturned();
    Read_ClosedFd_ErrorCodeReturned();
    Read_WriteOnlyFd_ErrorCodeReturned();
    Read_NegativeFd_ErrorCodeReturned();
    Read_NegativeLen_ErrorCodeReturned();
    Read_ReadOnlyFile_DataRead();
    Read_ReadFile_DataRead();

    return 0;
}