#ifdef _DEBUG
#include <stdio.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "ggarena.h"

#ifdef _WIN32
Arena* Arena_CreateArena(size_t sizeInBytes)
{
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    arena->size = sizeInBytes;
    arena->currentLocation = 0;
    arena->currentLocPtr = (char*)malloc(sizeInBytes);
    return arena;
}

void* Arena_Alloc(Arena* arena, size_t sizeInBytes)
{
    if ((arena->currentLocation + sizeInBytes) > arena->size)
    {
        __debugbreak();
        errno = ENOMEM;
        return NULL;
    }

    void* ptr = (void*)(arena->currentLocPtr + arena->currentLocation);
    arena->currentLocation = arena->currentLocation + sizeInBytes;

    // Set memory to zero.
    memset(ptr, 0, sizeInBytes);

    return ptr;
}

void* Arena_Realloc(Arena* arena, void* oldPtr, size_t oldSizeInBytes, size_t sizeInBytes)
{
    // return a new pointer and memset the old data.
    if ((arena->currentLocation + sizeInBytes) > arena->size)
    {
        __debugbreak();
        errno = ENOMEM;
        return NULL;
    }

    void* ptr = (void*)(arena->currentLocPtr + arena->currentLocation);
    arena->currentLocation = arena->currentLocation + sizeInBytes;

    // Set old memory to whatever already existed.
    //memcpy(ptr, oldPtr, oldSizeInBytes);

    // Set new memory to zero
    //memset((char *)ptr + oldSizeInBytes, 0, sizeInBytes - oldSizeInBytes);


    return ptr;
}

void Arena_FreeAll(Arena* arena)
{
    arena->currentLocation = 0;
}

#elif PLATFORM_WEB
Arena* Arena_CreateArena(size_t sizeInBytes)
{
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    arena->size = sizeInBytes;
    arena->currentLocation = 0;
    arena->currentLocPtr = (char*)malloc(sizeInBytes);
    return arena;
}

void* Arena_Alloc(Arena* arena, size_t sizeInBytes)
{
    if ((arena->currentLocation + sizeInBytes) > arena->size)
    {
        //__debugbreak();
        errno = ENOMEM;
        return NULL;
    }

    void* ptr = (void*)(arena->currentLocPtr + arena->currentLocation);
    arena->currentLocation = arena->currentLocation + sizeInBytes;

    // Set memory to zero.
    memset(ptr, 0, sizeInBytes);

    return ptr;
}

void* Arena_Realloc(Arena* arena, void* oldPtr, size_t oldSizeInBytes, size_t sizeInBytes)
{
    // return a new pointer and memset the old data.
    if ((arena->currentLocation + sizeInBytes) > arena->size)
    {
        //__debugbreak();
        errno = ENOMEM;
        return NULL;
    }

    void* ptr = (void*)(arena->currentLocPtr + arena->currentLocation);
    arena->currentLocation = arena->currentLocation + sizeInBytes;

    // Set old memory to whatever already existed.
    //memcpy(ptr, oldPtr, oldSizeInBytes);

    // Set new memory to zero
    //memset((char *)ptr + oldSizeInBytes, 0, sizeInBytes - oldSizeInBytes);


    return ptr;
}

void Arena_FreeAll(Arena* arena)
{
    arena->currentLocation = 0;
}


#elif __linux__
// Linux-specific code
Arena* CreateArena(size_t sizeInBytes)
{
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    arena->size = sizeInBytes;
    arena->currentLocation = 0;
    arena->currentLocPtr = (char*)malloc(sizeInBytes);
    return arena;
}

void* Arena_Alloc(Arena* arena, size_t sizeInBytes)
{
    if ((arena->currentLocation + sizeInBytes) > arena->size)
    {
        __debugbreak();
        errno = ENOMEM;
        return NULL;
    }
    void* ptr = (void*)(arena->currentLocPtr + arena->currentLocation);
    arena->currentLocation = arena->currentLocation + sizeInBytes;

    return ptr;
}
#elif __APPLE__
// macOS-specific code
Arena* CreateArena(size_t sizeInBytes)
{
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    arena->size = sizeInBytes;
    arena->currentLocation = 0;
    arena->currentLocPtr = (char*)malloc(sizeInBytes);
    return arena;
}

void* Arena_Alloc(Arena* arena, size_t sizeInBytes)
{
    if ((arena->currentLocation + sizeInBytes) > arena->size)
    {
        __debugbreak();
        errno = ENOMEM;
        return NULL;
    }
    void* ptr = (void*)(arena->currentLocPtr + arena->currentLocation);
    arena->currentLocation = arena->currentLocation + sizeInBytes;

    return ptr;
}
#endif
