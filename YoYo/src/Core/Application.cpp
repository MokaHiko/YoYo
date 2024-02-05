#include "Application.h"

#include "Platform/Platform.h"

#include "Core/Memory.h"
#include "Core/Log.h"
#include "Core/Time.h"

#include "Input/Input.h"

#include "Resource/ResourceManager.h"
#include "Renderer/RendererLayer.h"

#include "Events/ApplicationEvent.h"

namespace yoyo
{
    Application::Application(const ApplicationSettings& settings)
    {
        m_settings = settings;
    }

    Application::~Application() {}

    void Application::Init()
    {
        bool success = Platform::Init(m_settings.x, m_settings.y, m_settings.width, m_settings.height, m_settings.app_name);

        // TODO: Memory System Init

        EventManager::Instance()->Subscribe(ApplicationCloseEvent::s_event_type, [&](Ref<Event> event) {return OnClose();});

        // Default Layers
        PushLayer(Y_NEW InputLayer());
        PushLayer(Y_NEW RuntimeResourceLayer());
        PushLayer(Y_NEW RendererLayer());

        m_running = success;
    }

    void Application::Shutdown()
    {
        Platform::Shutdown();
    }

    void Application::Run()
    {
        // Enable layers
        for (Layer* layer : m_layers)
        {
            layer->OnEnable();
        }

        while (m_running)
        {
            Timer profiler_time([&](const Timer& timer) {
                Time::SetDeltaTime(timer.delta);
            });

            Platform::PumpMessages();

            const float dt = Time::DeltaTime();
            for (Layer* layer : m_layers)
            {
                layer->OnUpdate(dt);
            }
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_layers.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer* layer)
    {
        m_layers.PopLayer(layer);
        layer->OnDetatch();
    }

    ApplicationSettings& Application::Settings()
    {
        return m_settings;
    }

    bool Application::OnWindowResize(float x, float y, float width, float height)
    {
        YINFO("RESIZE!");
        return false;
    }

    bool Application::OnClose()
    {
        m_running = false;
        return false;
    }
}
