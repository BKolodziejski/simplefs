#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../simplefs.h"
#include "simplefs_write.h"
#include "simplefs_open.h"
#include "simplefs_chmode.h"
#include "simplefs_unlink.h"
#include "simplefs_read.h"


int main() {
    // create sample files

    simplefs_mkdir("/foo_dir");
    simplefs_open("/foo_dir/bar_file", O_RDWR, O_CREAT); // create file

    //write data to files
    writeFileToSimpleFs("../tests/test-files/file02", "/foo_dir/file02");
    writeFileToSimpleFs("../tests/test-files/file03", "/foo_dir/file03");
    //simplefs_chmode("/foo_dir/file03", READ only)

    // simplefs_write
    Write_NegativeFd_ErrorCodeReturned();
    Write_UnopenedFd_ErrorCodeReturned();
    Write_ClosedFd_ErrorCodeReturned();
    Write_ReadOnlyFd_ErrorCodeReturned();
    Write_DirectoryFd_ErrorCodeReturned();
    Write_NewFile_DataWritten();
    Write_NewFileDataLargerThanBlockSize_DataWritten();
    Write_ExistingFileWithData_DataReplaced();
    Write_NewFileWriteOnlyMode_DataWritten();
    Write_TwoFiles_EachFilesDataWritten();
    // run separately
    //Write_ReachBlockCountLimit_DataWritten();

    // simplefs_open
    Open_FileDoesNotExist_ErrorCodeReturned();
    Open_DirectoryDoesNotExist_ErrorCodeReturned();
    Open_FileExists_FileOpened();
    Open_FileExistsParentInPath_FileOpened();
    Open_FilenameUsedAsDirInPath_ErrorCodeReturned();
    Open_TooLongFilename_ErrorCodeReturned();
    Open_TooLongPath_ErrorCodeReturned();
    Open_ReachFilesInDirLimit_ErrorCodeReturned();

    // run separately, SIMPLEFS_BLOCK_COUNT >= 10000 required
    //Open_ReachTooManyFilesOpenLimit_ErrorCodeReturned();
    //Open_ReachInodeCountLimit_ErrorCodeReturned();


    // simplefs_read
    Read_UnopenedFd_ErrorCodeReturned();
    Read_ClosedFd_ErrorCodeReturned();
    Read_WriteOnlyFd_ErrorCodeReturned();
    Read_NegativeFd_ErrorCodeReturned();
    Read_NegativeLen_ErrorCodeReturned();
    Read_File_DataRead();
    Read_ReadOnlyFile_DataRead();


    // simplefs_mkdir
    Mkdir_NonExistingPath_ErrorCodeReturned();
    //Mkdir_FilenameAsDirPath_ErrorCodeReturned(); TODO fix
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
    // TODO disallow unlink when files are opened
    //Unlink_FileOpened_ErrorCodeReturned();
    //Unlink_DirectoryHasOpenedFiles_ErrorCodeReturned();

    // TODO chmode
/*
    // simplefs_chmode
    Chmode_NonExistingDirectory_ErrorCodeReturned();
    Chmode_NonExistingFile_ErrorCodeReturned();
    Chmode_FileOpened_ErrorCodeReturned();
    Chmode_SetFileReadOnly_ModeChanged();
    Chmode_SetFileReadWrite_ModeChanged();
    Chmode_SetDirectorReadOnly_ModeChanged();
    Chmode_SetDirectorReadWrite_ModeChanged();
*/

    system("rm ../tests/test-output/file02");
    system("rm ../tests/test-output/file03");

    readFileFromSimpleFs("/foo_dir/file02", "../tests/test-output/file02", getFilesize("../tests/test-files/file02"));
    readFileFromSimpleFs("/foo_dir/file03", "../tests/test-output/file03", getFilesize("../tests/test-files/file03"));

    system("diff ../tests/test-files/file02 ../tests/test-output/file02 > ../tests/test-output/diff_file02");
    system("diff ../tests/test-files/file03 ../tests/test-output/file03 > ../tests/test-output/diff_file03");

    return 0;
}