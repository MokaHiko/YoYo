#include "Application.h"

#include "Platform/Platform.h"

#include "Core/Memory.h"
#include "Core/Log.h"
#include "Core/Time.h"

#include "Renderer/RendererLayer.h"

namespace yoyo
{
    Application::Application(const ApplicationSettings &settings)
    {
        m_settings = settings;
    }

    Application::~Application() {}

    void Application::Init()
    {
        bool success = Platform::Init(m_settings.x, m_settings.y, m_settings.width, m_settings.height, m_settings.app_name);

        // Default Layers
        PushLayer(Y_NEW RendererLayer());

        m_running = success;
    }

    void Application::Run()
    {
        while(m_running)
        {
            Platform::PumpMessages();

            const float dt = Time::DeltaTime();
            for(Layer* layer : m_layers)
            {
                layer->OnUpdate(dt);
            }
        }
    }

    void Application::Shutdown()
    {
        Platform::Shutdown();
    }

    void Application::PushLayer(Layer *layer)
    {
        m_layers.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(uint32_t id)
    {
    }

    ApplicationSettings &Application::Settings()
    {
        return m_settings;
    }
}
