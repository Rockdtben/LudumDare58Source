#pragma once
#include "stdlib.h"

typedef struct Arena_s {
    size_t size;
    size_t currentLocation;
    char* arenaName;
    char* currentLocPtr;
    
} Arena;

Arena* Arena_CreateArena(size_t sizeInBytes);
void* Arena_Alloc(Arena* arena, size_t sizeInBytes);
void* Arena_Realloc(Arena* arena, void* oldPtr, size_t oldSizeInBytes, size_t sizeInBytes);
void Arena_FreeAll(Arena* arena);
