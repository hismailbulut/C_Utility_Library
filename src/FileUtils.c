#include "FileUtils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "Debug.h"
#include "StringUtils.h"

static size_t _GetFileSize(const char* path) {
    struct stat info;
    if (stat(path, &info) == 0) {
        return info.st_size;
    }
    DEBUG_LOG_ERROR("File size couldn't be calculated.");
    return 0;
}

static inline void _CheckError(FILE* file) {
    if (ferror(file) != 0) {
        perror("[FILE UTILS ERROR]");
    }
}

bool FileUtilsReadString(const char* path, String* outString) {
    size_t fileSize = _GetFileSize(path);
    if (fileSize <= 0) {
        return false;
    }
    bool success = false;
    FILE* file;
    char* buffer = malloc(fileSize + 1);
    if (fopen_s(&file, path, "r") == 0) {
        if (fread(buffer, 1, fileSize, file) > 0) {
            *outString = StringCreateCStr(buffer);
            success = true;
        }
        fclose(file);
    }
    free(buffer);
    if (!success) {
        outString->c_str = NULL;
        outString->length = 0;
        _CheckError(file);
    }
    return success;
}

bool FileUtilsWriteString(const char* path, String string) {
    bool success = false;
    FILE* file;
    if (fopen_s(&file, path, "w") == 0) {
        if (fwrite(string.c_str, 1, strlen(string.c_str), file) > 0) {
            success = true;
        }
        fclose(file);
    }
    if (!success) {
        _CheckError(file);
    }
    return success;
}

bool FileUtilsAppendString(const char* path, String string) {
    bool success = false;
    FILE* file;
    if (fopen_s(&file, path, "a") == 0) {
        if (fwrite(string.c_str, 1, strlen(string.c_str), file) > 0) {
            success = true;
        }
        fclose(file);
    }
    if (!success) {
        _CheckError(file);
    }
    return success;
}

bool FileUtilsReadBinary(const char* path, void** outBuffer,
                         size_t* outBufferSize) {
    size_t fileSize = _GetFileSize(path);
    if (fileSize <= 0) {
        return false;
    }
    bool success = false;
    FILE* file;
    if (fopen_s(&file, path, "rb") == 0) {
        *outBuffer = malloc(fileSize);
        if (fread(*outBuffer, 1, fileSize, file) > 0) {
            *outBufferSize = fileSize;
            success = true;
        } else {
            free(*outBuffer);
        }
        fclose(file);
    }
    if (!success) {
        *outBufferSize = 0;
        _CheckError(file);
    }
    return success;
}

bool FileUtilsWriteBinary(const char* path, void* buffer, size_t bufferSize) {
    bool success = false;
    FILE* file;
    if (fopen_s(&file, path, "wb") == 0) {
        if (fwrite(buffer, 1, bufferSize, file) > 0) {
            success = true;
        }
        fclose(file);
    }
    if (!success) {
        _CheckError(file);
    }
    return success;
}