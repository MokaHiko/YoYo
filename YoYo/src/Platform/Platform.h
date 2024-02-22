#pragma once

#include "Defines.h"

namespace yoyo {
    namespace Platform
    {
        // Platform memory allocation call
        void* Allocate(size_t size);

        // Platform specific initialization (windowing, surface, etc...)
        bool Init(float x, float y, float width, float height, const std::string& app_name = "");

        // Request the platforms current applicatiion window
        void* NativeAppWindow();

        void ConsoleWrite(const char* message, uint8_t color);

        void PumpMessages();

        void CreateSurface(void* context, void* surface);

        void Shutdown();

        bool FileRead(const char *path, char **buffer, size_t* size);

        YAPI uint64_t GenerateUUIDV4();

        // Checks if system has enough hard disk space
        static bool CheckStorage(size_t required_size);

        // Checks if system has enough physical and virtual memory
        static bool CheckMemory(size_t physical_size, size_t virtual_size);
    };
}