#ifndef COMMON_H
#define COMMON_H

#define BUF_SIZE 1024
#define NAME_SIZE 256

typedef struct
{
    char filename[NAME_SIZE];
    int filesize;
} Header;

#endif