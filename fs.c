#include "fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// data block definition (in bytes)
#define BLOCK_INDEX_SIZE 4
#define BLOCK_DATA_SIZE 124
#define BLOCK_SIZE (BLOCK_INDEX_SIZE + BLOCK_DATA_SIZE)
#define MAX_BLOCK_SIZE 1024

// file entry
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 256

typedef struct __attribute__((__packed__)) data_block {
    uint32_t index;
    uint8_t data[BLOCK_DATA_SIZE];
} data_block_t;

typedef struct file_entry {
    char name[MAX_FILENAME];
    uint32_t start_index;
    size_t file_size;
} file_entry_t;

typedef struct node {
    struct node* next;
} node_t;

node_t* free_list = NULL;
data_block_t* blocks = NULL;

file_entry_t entries[MAX_FILE_SIZE];
size_t next_entry = 0;

void fs_init() {
    // initialize data blocks
    blocks = malloc(sizeof(data_block_t) * MAX_BLOCK_SIZE);

    // adding data blocks to the free list
    for (size_t i = 0; i < MAX_BLOCK_SIZE; i++) {
        node_t* curr_node = (node_t*)(&blocks[i]);
        curr_node->next = free_list;
        free_list = curr_node;
    }
}

size_t min(size_t num1, size_t num2) { return num1 > num2 ? num2 : num1; }

bool write(const char* file_name, uint8_t* buf, size_t size) {
    // adding it to the file entry
    if (next_entry >= MAX_FILE_SIZE) {
        return false;
    }
    strcpy(entries[next_entry].name, file_name);
    entries[next_entry].file_size = size;
    // index calculated by its relative distance to first block
    entries[next_entry].start_index =
        ((uintptr_t)free_list - (uintptr_t)blocks) / BLOCK_SIZE;

    // copy data from buf into data block
    size_t total_blocks = (size + BLOCK_DATA_SIZE -1) / BLOCK_DATA_SIZE;
    for (size_t i = 0; i < total_blocks; i++) {
        // obtain a new data block
        if (free_list == NULL) {
            return false;
        }
        data_block_t* new_block = (data_block_t*)free_list;
        free_list = free_list->next;

        // copy data over
        size_t size_to_copy = min(size, BLOCK_DATA_SIZE);
        memcpy(new_block->data, buf, size_to_copy);
        buf += size_to_copy;
        size -= size_to_copy;

        // set block index
        new_block->index =
            (i < (total_blocks - 1))
                ? ((uintptr_t)free_list - (uintptr_t)blocks) / BLOCK_SIZE
                : -1;
    }
    next_entry++;

    return true;
}

bool append(const char *file_name, uint8_t *buf, size_t size)
{
    for (size_t i = 0; i < next_entry; i++)
    {
        if (strcmp(file_name, entries[i].name) == 0)
        {

            size_t fsize = entries[i].file_size;
 
            // loop through each block to get to last
            data_block_t *curr_block = &blocks[entries[i].start_index];
            while (curr_block->index != -1)
            { // EOF
                curr_block = &blocks[curr_block->index];
            }
            
            size_t start = fsize % BLOCK_DATA_SIZE; // write index of last block
            size_t leftover = BLOCK_DATA_SIZE - start; // leftover space of last block

            
            // If the block has enough space to fit the buf, write and return.
            // If start is 0, there is no space left--the last block was completely filled
            if(leftover >= size && start != 0){
                memcpy((curr_block->data)+start, buf, size);
                entries[i].file_size += size;
                printf("Final file size: %zu\n", entries[i].file_size);
                return true;
            }
            else if(leftover < size && start != 0){
                //  Otherwise write until the very end of the block.
                memcpy((curr_block->data)+start, buf, leftover);
                buf += leftover;
                entries[i].file_size += leftover;
                size -= leftover;
            }
            
            // set block index
            curr_block->index = ((uintptr_t)free_list - (uintptr_t)blocks) / BLOCK_SIZE;

            // Determine how many MORE blocks we need to add to the file to copy the data over, and add them.
            size_t total_blocks = (size + BLOCK_DATA_SIZE -1) / BLOCK_DATA_SIZE;
            for (size_t j = 0; j < total_blocks; j++)
            {
                // obtain a new data block
                if (free_list == NULL)
                {
                    return false;
                }
                data_block_t *new_block = (data_block_t *)free_list;
                free_list = free_list->next;

                // copy data over
                size_t size_to_copy = min(size, BLOCK_DATA_SIZE);
                memcpy(new_block->data, buf, size_to_copy);
                buf += size_to_copy;
                size -= size_to_copy;
                entries[i].file_size += size_to_copy;
              

                // set block index
                new_block->index =
                    (j < (total_blocks - 1))
                        ? ((uintptr_t)free_list - (uintptr_t)blocks) / BLOCK_SIZE
                        : -1;
            }
            printf("Final file size: %zu\n", entries[i].file_size);
        }
    }
    
    return true;
}

void read(const char* file_name) {
    for (size_t i = 0; i < next_entry; i++) {
        if (strcmp(file_name, entries[i].name) == 0) {
            size_t total_size = entries[i].file_size;
            data_block_t* curr_block = &blocks[entries[i].start_index];

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
    }
}

void delete(const char* file_name) {
    for(size_t i = 0; i < next_entry; i++) {
        if (strcmp(file_name, entries[i].name) == 0) {
            // Put data block on freelist for future use
            blocks[entries[i].start_index].index = 0;
            node_t* curr_node = (node_t*)(&blocks[entries[i].start_index]); 
            curr_node->next = free_list;
            free_list = curr_node;

            // Remove entry
            strcpy(entries[i].name, "");
            memset(&entries[i].start_index, 0, sizeof(uint32_t));
            memset(&entries[i].file_size, 0, sizeof(size_t));
        }
    }
}