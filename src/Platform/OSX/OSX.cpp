#include "OSX.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <cassert>

#include <fstream>

#include "Core/Log.h"
#include "Core/Memory.h"
#include "Core/Assert.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace yoyo
{
    static SDL_Window *window = nullptr;

    void *Platform::Allocate(size_t size)
    {
        return malloc(size);
    }

    bool Platform::Init(float x, float y, float width, float height, const std::string &app_name)
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            printf("error initializing SDL: %s\n", SDL_GetError());
        }
        // be sure to initialize your SDL window with the vulkan flag
        window = SDL_CreateWindow(app_name.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width,
                                  height,
                                  SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
        if (!window)
        {
            printf("%s\n", SDL_GetError());
            return false;
        }

        return true;
    }

	void Platform::ConsoleWrite(const char *message, uint8_t color)
    {
        printf("message");
    }

    void* Platform::NativeAppWindow()
    {
        YASSERT(window, "Platform Init Not called before requesting native window instance");
        return window;
    }

    void Platform::PumpMessages()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                Ref<Event> app_close_event = CreateRef<ApplicationCloseEvent>();
                EventManager::Instance().Dispatch(app_close_event);
            }
        }
    }

    void Platform::CreateSurface(void *context, void *surface)
    {
        YASSERT(window != nullptr, "Cannot Create Surface before SDL initialization");
        if (SDL_Vulkan_CreateSurface(window, *((VkInstance *)context), (VkSurfaceKHR *)surface) != SDL_TRUE)
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

    bool Platform::FileRead(const char *path, char **buffer, size_t* size)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (file.is_open())
        {
            *size = file.tellg();
            file.seekg(0);

            *buffer = (char *)YAllocate(*size, MemoryTag::String);
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

    uint64_t Platform::GenerateUUIDV4()
    {
        // TODO: Properly generate UUIDs
        return rand();
    }
}