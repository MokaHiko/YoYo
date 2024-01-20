#pragma once

#include "Defines.h"

#include "Events/Event.h"
#include "Layer.h"

namespace yoyo
{
    struct ApplicationSettings
    {
        std::string app_name;

        float x;
        float y;

        float width;
        float height;
    };

    class YAPI Application
    {
    public:
        Application(const ApplicationSettings &settings);
        virtual ~Application();

        void Init();
        void Run();
        void Shutdown();

        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        ApplicationSettings& Settings();

        bool OnWindowResize(float x, float y, float width, float height);
        bool OnClose();
    private:
        ApplicationSettings m_settings;
        EventManager m_event_manager;

        LayerStack m_layers;
        bool m_running;
    };
}

extern yoyo::Application *CreateApplication();