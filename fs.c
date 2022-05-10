#include "fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fd.h"

typedef struct __attribute__((__packed__)) data_block {
    uint32_t index;
    uint8_t data[BLOCK_DATA_SIZE];
} data_block_t;

typedef struct node {
    struct node *next;
} node_t;

node_t *free_list = NULL;
data_block_t *blocks = NULL;

file_entry_t entries[MAX_FILE_SIZE];
size_t next_entry = 0;

void fs_init() {
    // initialize data blocks
    blocks = malloc(sizeof(data_block_t) * MAX_BLOCK_SIZE);

    // adding data blocks to the free list
    for (size_t i = 0; i < MAX_BLOCK_SIZE; i++) {
        node_t *curr_node = (node_t *)(&blocks[i]);
        curr_node->next = free_list;
        free_list = curr_node;
    }
}

int open(const char *file_name) {
    if (next_entry >= MAX_FILE_SIZE) {
        return false;
    }

    // Find file and set fd
    for (size_t i = 0; i < next_entry; i++) {
        if (strcmp(file_name, entries[i].name) == 0) {
            int fd = create_fd(&entries[i]);
            return fd;
        }
    }

    // file not found, create a new file instead
    // sanity check
    if (next_entry >= MAX_FILE_SIZE) {
        return -1;
    }
    if (free_list != NULL) {
        return -1;
    }
    // setting up an empty file entry
    strcpy(entries[next_entry].name, file_name);
    entries[next_entry].file_size = 0;
    entries[next_entry].start_index = ((uintptr_t)free_list - (uintptr_t)blocks) / BLOCK_SIZE;
    data_block_t *new_block = free_list;
    new_block->index = -1;
    free_list = free_list->next;

    int fd = create_fd(&entries[next_entry]);
    next_entry += 1;

    return fd;
}

size_t min(size_t num1, size_t num2) { return num1 > num2 ? num2 : num1; }

bool write(int fd, uint8_t *buf, size_t size) { write_at(fd, 0, buf, size); }

void write_at(int fd, size_t index, uint8_t buff, size_t size) {
    file_entry_t *entry = find_fd(fd);
    size_t fsize = entry->file_size;

    if (index > fsize) return;

    int num_blocks = (fsize + BLOCK_DATA_SIZE - 1) / BLOCK_DATA_SIZE;
    int blocks_needed = (index + size + BLOCK_DATA_SIZE - 1) / BLOCK_DATA_SIZE;

    if (blocks_needed > num_blocks) {  // need allocate new block
        // go to the end of data block of file
        data_block_t *curr_block = &blocks[entry->start_index];
        while (curr_block->index != -1) {
            curr_block = &blocks[curr_block->index];
        }

        // allocate new block
        int num_new_block = blocks_needed - num_blocks;
        for (size_t i = 0; i < num_new_block; i++) {
            if (free_list == NULL) {
                // TODO: do something
                return;
            }

            data_block_t *new_block = free_list;
            free_list = free_list->next;

            curr_block->index = ((uintptr_t)new_block - (uintptr_t)blocks) / BLOCK_SIZE;
            curr_block = new_block;

            // set index to last block
            if (i == (num_new_block - 1)) {
                curr_block->index = -1;
            }
        }
    }

    // Skip number of blocks to get to index to start writing at.
    size_t start_block = index / BLOCK_DATA_SIZE;
    size_t write_point = index % BLOCK_DATA_SIZE;

    // Loop through each block to get to desired
    data_block_t *curr_block = &blocks[entry->start_index];
    for (int i = 0; i < start_block; i++) {  // EOF
        curr_block = &blocks[curr_block->index];
    }

    // Once we are at block, start writing byte by byte
    while (size > 0) {
        // change write point if we reach the end
        if (write_point == BLOCK_DATA_SIZE) {
            curr_block = &blocks[curr_block->index];
            write_point = 0;
        }

        memcpy((curr_block->data) + write_point++, buff++, 1);
        size--;
    }
}

bool append(int fd, uint8_t *buf, size_t size) {
    file_entry_t *entry = find_fd(fd);
    write_at(fd, entry->file_size, buf, size);
}

void read(int fd) {
    file_entry_t *entry = find_fd(fd);
    size_t total_size = entry->file_size;
    data_block_t *curr_block = &blocks[entry->start_index];

    for (;;) {
        for (size_t j = 0; j < min(BLOCK_DATA_SIZE, total_size); j++) {
            printf("%c", curr_block->data[j]);
        }
        total_size -= BLOCK_DATA_SIZE;

        // move to next block
        if (curr_block->index == -1) {  // EOF
            break;
        }
        curr_block = &blocks[curr_block->index];
    }
}

void delete (int fd) {
    file_entry_t *entry = find_fd(fd);

    // first block
    data_block_t *curr_block = &blocks[entry->start_index];
    node_t *curr_node;
    while (curr_block->index != -1) {  // EOF
        // cast to node
        node_t *curr_node = (node_t *)curr_block;
        curr_node->next = free_list;
        free_list = curr_node;
        curr_block = &blocks[curr_block->index];
    }
    // Remove entry
    strcpy(entry->name, "");
    memset(&entry->start_index, 0, sizeof(uint32_t));
    memset(&entry->file_size, 0, sizeof(size_t));
}

bool rename(const char *oldpath, const char *newpath) {
    for (int i = 0; i < next_entry; i++) {
        if (strcmp(oldpath, entries[i].name) == 0) {
            strcpy(entries[i].name, newpath);
            return true;
        }
    }

    return false;
}
