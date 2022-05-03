#include <stdio.h>

#include "fs.h"

int main() {
    fs_init();
    bool rtr = write("example.txt", "ThisIsATestokay\n", 16);
    read("example.txt");
    delete("example.txt");
    append("example.txt", "helloHI\n", 8);
    read("example.txt");
    write("example.txt", "a\n\n", 3);
    append("example.txt", "Whatsup?\n", 9);
    read("example.txt");

}