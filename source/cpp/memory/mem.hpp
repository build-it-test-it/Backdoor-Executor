#pragma once

// Only include JNI for Android builds, not iOS
#if !defined(IOS_TARGET) && !defined(__APPLE__)
#include <jni.h>
#endif

#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>

// Utils.h from LGL Mod Menu

typedef unsigned long DWORD;
static uintptr_t libBase;

static DWORD getLibBase(const char *library) {
    char filename[0xFF] = {0},
            buffer[1024] = {0};
    FILE *fp = NULL;
    DWORD address = 0;

    sprintf(filename, "/proc/self/maps");

    fp = fopen(filename,"rt");
    if (fp == NULL) {
        perror("fopen");
        goto done;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, library)) {
            address = (DWORD) strtoul(buffer, NULL, 16);
            goto done;
        }
    }

    done:

    if (fp) {
        fclose(fp);
    }

    return address;
}

static DWORD getAddress(DWORD relativeAddr) {
    libBase = getLibBase(("libroblox.so"));
    if (libBase == 0)
        return 0;
    return (reinterpret_cast<DWORD>(libBase + relativeAddr));
}

static DWORD getLibAddress(const char* lib, DWORD relativeAddr)
{
    libBase = getLibBase(lib);
    if (libBase == 0)
        return 0;
    return (reinterpret_cast<DWORD>(libBase + relativeAddr));
}

static bool isLibraryLoaded(const char *libraryName) {
    char line[512] = {0};
    FILE *fp = fopen(("/proc/self/maps"), ("rt"));
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            std::string a = line;
            if (strstr(line, libraryName)) {
                return true;
            }
        }
        fclose(fp);
    }
    return false;
}


// Conditionally compile JNI-dependent functions
#if !defined(IOS_TARGET) && !defined(__APPLE__)
static jboolean isGameLibLoaded(JNIEnv *env, jobject thiz) {
    return isLibraryLoaded(("libroblox.so"));
}
#else
// iOS version doesn't need JNI
static bool isGameLibLoaded() {
    return isLibraryLoaded(("libroblox.dylib"));
}
#endif
