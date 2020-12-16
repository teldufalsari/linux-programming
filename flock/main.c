/* 
 * This program simply writes statistics on how many times it was executed.
 * It should work correctly with an empty, nonexisting file or when several processes
 * are running simultaneously.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <errno.h>

int main(void) {
    int count_fd = open("count.txt", O_CREAT | O_RDWR, 0600);
    if (lockf(count_fd, F_LOCK, 0)) {
        perror("lockf");
        close(count_fd);
        return 1;
    }
    struct stat statbuf;
    if (stat("count.txt", &statbuf)) {
        perror("stat(\"count.txt\")");
        close(count_fd);
        return 1;
    }
    long count;
    if (statbuf.st_size == 0)
        count = 1;
    else {
        char strbuf[16] = "";
        ssize_t num_read = read(count_fd, strbuf, statbuf.st_size);
        if (num_read != statbuf.st_size) {
            perror("read");
            close(count_fd);
            return 2;
        }
        errno = 0; //We need this to check conversion of string to number
        count = strtol(strbuf, NULL, 10);
        if (errno) {
            perror("strtol: conversion failed");
            close(count_fd);
            return 3;
        }
        count += 1;
    }
    FILE* fp_counter = fdopen(count_fd, "w+");
    rewind(fp_counter);
    fprintf(fp_counter, "%li", count);
    fflush(fp_counter);
    if (lockf(count_fd, F_ULOCK, 0)) {
        perror("flock");
        fclose(fp_counter);
        return 4;
    }
    if (fclose(fp_counter)) {
        perror("fclose:");
        close(count_fd);
        return 5;
    }
    return 0;
}