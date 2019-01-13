//
// Created by kolo on 13.01.19.
//

#include <string.h>
#include "filesystem.h"

static SimplefsIndex evaluatePathRecursive(SimplefsIndex currentInodeIndex, char* currentToken) {
    if (currentToken == NULL) {
        return currentInodeIndex;
    }
    Directory d;
    readFile(currentInodeIndex, &d, 0, sizeof(Directory));
    for (SimplefsIndex i = 0; i < SIMPLEFS_MAX_FILES_IN_DIR; ++i) {
        if (strcmp(currentToken, d.files[i].filename) == 0) {
            return evaluatePathRecursive(d.files[i].inodeIndex, strtok(NULL, "/"));
        }
    }

    return SIMPLEFS_INODE_COUNT; // if path invalid
}

SimplefsIndex evaluatePath(char* path) {
    char copied[SIMPLEFS_MAX_FILENAME_LENGTH + 1]; // strtok messes up string, so we copy to leave arg intact
    strcpy(copied, path);
    char* firstPathSection = strtok(copied, "/"); // will remove first "/" and later calls can be strtok(NULL, "/")
    return evaluatePathRecursive(ROOT_INODE_INDEX, firstPathSection);
}

void cutOffLastPathSection(char* path, char* lastPathSectionOutput) {
    char currentChar = 'x'; // any value that is not \0
    int lastNotedSlashOffset = 0;
    for (int pathOffset = 0; currentChar != '\0'; ++pathOffset) {
        currentChar = path[pathOffset];
        if (currentChar == '/') {
            lastNotedSlashOffset = pathOffset;
        }
    }
    strcpy(lastPathSectionOutput, path + lastNotedSlashOffset + 1);
    path[lastNotedSlashOffset] = '\0';
}

SimplefsIndex evaluatePathForParent(char* path, char* childFileNameOutput) {
    char copied[SIMPLEFS_MAX_FILENAME_LENGTH + 1];
    strcpy(copied, path);
    cutOffLastPathSection(copied, childFileNameOutput);
    return evaluatePath(copied);
}