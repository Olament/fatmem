#include <stdio.h>

#include "fs.h"

int main() {
    fs_init();

    bool rtr = write("example.txt", "testcontent\n", 12);
    read("example.txt");
}