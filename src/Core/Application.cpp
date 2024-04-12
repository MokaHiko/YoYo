#include "Application.h"

#include "Platform/Platform.h"

#include "Core/Memory.h"
#include "Core/Log.h"
#include "Core/Time.h"

#include "Input/Input.h"

#include "Resource/ResourceManager.h"
#include "Renderer/RendererLayer.h"

#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

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

        // TODO: Init memory system

        // Subscribe to application events
        EventManager::Instance().Subscribe(ApplicationCloseEvent::s_event_type, [&](Ref<Event> event) {return OnClose();});

        // Debug Layers
        PushLayer(YNEW ImGuiLayer(this));

        // Default layers
        PushLayer(YNEW RendererLayer(this));
        PushLayer(YNEW RuntimeResourceLayer());
        PushLayer(YNEW InputLayer());

        m_running = success;
    }

    void Application::Shutdown()
    {
        for (Layer* layer : m_layers)
        {
            layer->OnDisable();
        }

        for (Layer* layer : m_layers)
        {
            layer->OnDetatch();
        }

        Platform::Shutdown();
    }

    void Application::Run()
    {
        // Enable layers
        for (auto rit = m_layers.rbegin(); rit != m_layers.rend(); rit++)
        {
            (*rit)->OnEnable();
        }

        while (m_running)
        {
            ScopedTimer profiler_time([&](const ScopedTimer& timer) {
                Time::SetDeltaTime(timer.delta);
            });

            Platform::PumpMessages();

            const float dt = Time::DeltaTime();
            for (auto rit = m_layers.rbegin(); rit != m_layers.rend(); rit++)
            {
                (*rit)->OnUpdate(dt);
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
