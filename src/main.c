#include <stdio.h>

#include "fs.h"

int main() {
    printf("nihao\n");
    fs_init();
    int fd = fs_open("new_file.txt");
    if (!fs_write(fd, "nihao", 5)) {
        printf("write failed!\n");
    }
}