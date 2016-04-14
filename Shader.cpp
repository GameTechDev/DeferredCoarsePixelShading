#include "Shader.h"

namespace ShaderFactoryUtil
{

    // Reads file from disk. Doesn't handle memory de-allocation.
    DWORD ReadFileFromDisk(LPCTSTR pFile,
        void** buffer)
    {
        // Open compiled shader file
        HANDLE fileHandle = CreateFile(pFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        assert(fileHandle != INVALID_HANDLE_VALUE);

        DWORD fileSize = GetFileSize(fileHandle, NULL);
        assert(fileSize != INVALID_FILE_SIZE);

        // We need enough mem to load the file from disk
        *buffer = malloc(fileSize);
        assert(*buffer != NULL);

        // Read file from disk
        BOOL bhr;
        DWORD wmWritten;
        bhr = ReadFile(fileHandle, (LPVOID)(*buffer), fileSize, &wmWritten, NULL);
        assert(bhr != 0);

        // Close file
        CloseHandle(fileHandle);

        return fileSize;
    }
}