#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void fs_init();

bool write(const char* file_name, uint8_t* buf, size_t size);

bool append(const char* file_name, uint8_t* buf, size_t size);

void read(const char* file_name);

void delete(const char* file_name);