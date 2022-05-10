#include "fd.h"

#include <stdlib.h>

#include "fs.h"

typedef struct fd_node {
    int fd;
    file_entry_t* file_entry;
    struct fd_note* next;
} fd_node_t;

fd_node_t* fd_list = NULL;

// next fd number to assign
// TODO: allow re-use fd number
static int next_fd = 0;

int create_fd(file_entry_t* file_entry) {
    int fd = next_fd;
    next_fd += 1;

    fd_node_t* new_node = malloc(sizeof(fd_node_t));
    new_node->fd = fd;
    new_node->file_entry = file_entry;
    new_node->next = fd_list;

    fd_list = new_node;

    return fd;
}

file_entry_t* find_fd(int fd) {
    fd_node_t* p = fd_list;
    while (p != NULL) {
        if (p->fd == fd) {
            return p->file_entry;
        }
        p = p->next;
    }

    return NULL;
}

void remove_fd(int fd) {
    fd_node_t* prev = NULL;
    fd_node_t* curr = fd_list;

    while (curr != NULL) {
        if (curr->fd == fd) {
            if (prev == NULL) {  // node to delete is the first node
                fd_list = fd_list->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return;
        }

        prev = curr;
        curr = curr->next;
    }
}
