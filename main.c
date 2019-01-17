#include <stdio.h>

#include "simplefs.h"

int main() {
    int fd = simplefs_open("/test.txt", O_RDWR, O_CREAT);
    simplefs_close(fd);
    return 0;
}