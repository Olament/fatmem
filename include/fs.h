#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// data block definition (in bytes)
#define BLOCK_INDEX_SIZE 4
#define BLOCK_DATA_SIZE 124
#define BLOCK_SIZE (BLOCK_INDEX_SIZE + BLOCK_DATA_SIZE)
#define MAX_BLOCK_SIZE 1024
#define BLOCK_INDEX_END UINT32_MAX

// file entry
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 256

typedef struct file_entry {
    char name[MAX_FILENAME];
    uint32_t start_index;
    size_t file_size;
} file_entry_t;

void fs_init();

int fs_open(const char* file_name);

bool fs_write(int fd, uint8_t* buf, size_t size);

bool fs_write_at(int fd, size_t index, uint8_t* buff, size_t size);

bool fs_append(int fd, uint8_t* buf, size_t size);

void fs_read(int fd);

void fs_delete(int fd);

bool fs_rename(const char* oldpath, const char* newpath);
