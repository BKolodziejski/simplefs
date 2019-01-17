#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include "utils.h"

__pid_t newWriterChild();

__pid_t newReaderChild();

void readers() {
    newReaderChild();
    newReaderChild();
    newReaderChild();

    while (waitpid(-1, NULL, 0) > 0);
}

void writers() {
    newWriterChild();
    newWriterChild();
    newWriterChild();
    newWriterChild();

    while (waitpid(-1, NULL, 0) > 0);
}

int main() {
    system("rm ../tests/test-output/file02_*");
    int status = writeFileToSimpleFsDebug("../tests/test-files/file02", "/file02", 0);
    printf("MAIN: write status: %d\n", status);

    printf("MAIN: starting processes\n");


    //readers();
    writers();




    return 0;
}

__pid_t newWriterChild() {
    __pid_t pid = fork();
    if (pid == 0) {
        __pid_t myPid = getpid();
        printf("PID=%d started\n", myPid);

        int writeStatus = writeFileToSimpleFsDebug("../tests/test-files/file02", "/file02", myPid);

        printf("PID=%d write status: %d\n", myPid, writeStatus);
        printf("PID=%d finished\n", myPid);
        exit(0);
    } else {
        return pid;
    }
}


__pid_t newReaderChild() {
    __pid_t pid = fork();
    if (pid == 0) {
        __pid_t myPid = getpid();
        printf("PID=%d started\n", myPid);
        char dstFilename[] = "../tests/test-output/file02_00000000";
        sprintf(dstFilename, "../tests/test-output/file02_%d", myPid);
        printf("%s\n", dstFilename);

        int readStatus = readFileFromSimpleFsDebug("/file02", dstFilename, getFilesize("../tests/test-files/file02"), myPid);

        printf("PID=%d read status: %d\n", myPid, readStatus);
        printf("PID=%d finished\n", myPid);
        exit(0);
    } else {
        return pid;
    }
}