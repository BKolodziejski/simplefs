cmake_minimum_required(VERSION 3.13)
project(simplefs)

set(CMAKE_C_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(simplefs main.c simplefs.h consts.h simplefs.c filesystem.c filesystem.h patheval.c patheval.h sem.h sem.c)
add_executable(simplefs_unlink unlink_sem.c consts.h sem.h sem.c)
add_executable(proc1 sem/proc1.c)
add_executable(proc2 sem/proc2.c)
add_executable(proc3 sem/proc3.c)
target_link_libraries(simplefs Threads::Threads m)
target_link_libraries(simplefs_unlink Threads::Threads)
target_link_libraries(proc1 Threads::Threads)
target_link_libraries(proc2 Threads::Threads)
target_link_libraries(proc3 Threads::Threads)


add_executable(simplefs-tests tests/main.c simplefs.h consts.h simplefs.c filesystem.c filesystem.h patheval.c patheval.h tests/simplefs_write.h tests/utils.h tests/simplefs_open.h tests/simplefs_mkdir.h tests/simplefs_chmode.h tests/simplefs_read.h)

target_link_libraries(simplefs-tests m)
