#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// data block definition (in bytes)
#define BLOCK_INDEX_SIZE 4
#define BLOCK_DATA_SIZE 124
#define BLOCK_SIZE (BLOCK_INDEX_SIZE + BLOCK_DATA_SIZE)
#define MAX_BLOCK_SIZE 1024

// file entry
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 256

typedef struct file_entry {
    char name[MAX_FILENAME];
    uint32_t start_index;
    size_t file_size;
} file_entry_t;

void fs_init();

bool write(const char* file_name, uint8_t* buf, size_t size);

bool append(const char* file_name, uint8_t* buf, size_t size);

void read(const char* file_name);

void delete (const char* file_name);

int rename(const char *oldpath, const char *newpath);
