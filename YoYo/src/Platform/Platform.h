#pragma once

namespace yoyo {
    namespace Platform
    {
        // Platform memory allocation call
        void* Allocate(size_t size);

        // Platform specific initialization (windowing, surface, etc...)
        bool Init(float x, float y, float width, float height, const std::string& app_name = "");

        void PumpMessages();

        void CreateSurface(void* context, void* surface);

        void Shutdown();

        void Assert(bool value, const char* msg);

        bool FileRead(const char *path, char **buffer);
    };
}