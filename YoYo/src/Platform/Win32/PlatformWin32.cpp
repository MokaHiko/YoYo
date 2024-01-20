#include "PlatformWin32.h"

#include <direct.h>

#include <windows.h>
#include <direct.h>
#include <cassert>
#include <fstream>

#include <SDL.h>
#include <SDL_vulkan.h>

#include "Core/Log.h"
#include "Core/Memory.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#pragma comment(lib, "rpcrt4.lib") 

namespace yoyo
{
    static SDL_Window* window = nullptr;

    void* Platform::Allocate(size_t size)
    {
        return malloc(size);
    }

    bool Platform::Init(float x, float y, float width, float height, const std::string& app_name)
    {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0)
        {
            printf("error initializing SDL: %s\n", SDL_GetError());
        }

        // be sure to initialize your SDL window with the vulkan flag
        window = SDL_CreateWindow(app_name.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            720,
            480,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
        if (!window)
        {
            printf("%s\n", SDL_GetError());
            return false;
        }

        return true;
    }

    void Platform::PumpMessages(EventManager* event_manager)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                Ref<Event> app_close_event = CreateRef<ApplicationCloseEvent>();
                event_manager->Dispatch(app_close_event);
            }
        }
    }

    void Platform::Assert(bool value, const char* msg)
    {
        assert(value && msg);
    }

	void Platform::ConsoleWrite(const char *message, uint8_t color)
	{
		// Set color
		HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		static uint8_t levels[6] = { 64, 7, 1, 8, 5, 6}; // FATAL, INFO, DEBUG, TRACE, ERROR, WARN
		SetConsoleTextAttribute(console_handle, levels[color]);

		// Output to debug console
		OutputDebugStringA(message);

		// Output to application console
		uint64_t length = strlen(message);
		LPDWORD number_written = 0;
		WriteConsoleA(console_handle, message, (DWORD)length, number_written, 0);
	}

    void Platform::CreateSurface(void* context, void* surface)
    {
        YASSERT(window != nullptr, "Cannot Create Surface before SDL initialization");
        if (SDL_Vulkan_CreateSurface(window, *((VkInstance*)context), (VkSurfaceKHR*)surface) != SDL_TRUE)
        {
            YERROR("Failed to create vulkan surface: Error: %s", SDL_GetError());
        }
    }

    void Platform::Shutdown()
    {
        SDL_DestroyWindow(window);
        window = nullptr;

        SDL_Quit();
    }

    bool Platform::FileRead(const char* path, char** buffer, size_t* size)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (file.is_open())
        {
            *size = file.tellg();
            file.seekg(0);

            *buffer = (char*)YAllocate(*size, MemoryTag::String);
            file.read(*buffer, *size);

            file.close();
            return true;
        }
        else
        {
            YERROR("Failed to load file at path %s", path);
            return false;
        }
    }

    bool Platform::CheckStorage(size_t required_size)
    {
        int const drive = _getdrive();
        struct _diskfree_t disk_free = {};

        _getdiskfree(drive, &disk_free);
        unsigned __int64 const needed_clusters = required_size / (disk_free.sectors_per_cluster * disk_free.bytes_per_sector);

        if (disk_free.avail_clusters < needed_clusters)
        {
            YERROR("Check Storage Failure: Not enough physical storage.");
            return false;
        }

        return true;
    }

    bool Platform::CheckMemory(size_t physical_size, size_t virtual_size)
    {
        MEMORYSTATUSEX status;
        GlobalMemoryStatusEx(&status);
        if (status.ullTotalPhys < physical_size)
        {
            // you don’t have enough physical memory. Tell the player to go get a
            // real computer and give this one to his mother.
            YERROR("CheckMemory Failure : Not enough physical memory.");
            return false;
        }
        // Check for enough free memory.
        if (status.ullAvailVirtual < virtual_size)
        {
            // you don’t have enough virtual memory available.
            // Tell the player to shut down the copy of Visual Studio running in the
            // background, or whatever seems to be sucking the memory dry.
            YERROR("CheckMemory Failure : Not enough virtual memory.");
            return false;
        }

        char* buff = Y_NEW char[virtual_size];
        if (buff)
        {
            delete[] buff;
        }
        else
        {
            // even though there is enough memory, it isn’t available in one
            // block, which can be critical for games that manage their own memory
            YERROR("CheckMemory Failure : Not enough contiguos memory.");
            return false;
        }

        return true;
    }

    uint64_t Platform::GenerateUUIDV4()
    {
        UUID uuid;
        RPC_CSTR  uuid_str;
        std::string uuid_out;

        if (UuidCreate(&uuid) != RPC_S_OK)
        {
            YERROR("couldn't create uuid\nError code %d", GetLastError());
        }

        return uuid.Data1;
    }
}