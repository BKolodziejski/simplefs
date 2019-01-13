//
// Created by kolo on 13.01.19.
//

#ifndef SIMPLEFS_PATHEVAL_H
#define SIMPLEFS_PATHEVAL_H

#include "filesystem.h"

SimplefsIndex evaluatePath(char* path);
SimplefsIndex evaluatePathForParent(char* path, char* childFileNameOutput);

#endif //SIMPLEFS_PATHEVAL_H
