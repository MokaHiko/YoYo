#pragma once

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

    class Application
    {
    public:
        Application(const ApplicationSettings &settings);
        virtual ~Application();

        void Init();
        void Run();
        void Shutdown();

        void PushLayer(Layer* layer);
        void PopLayer(uint32_t id);

        ApplicationSettings& Settings();
    private:
        ApplicationSettings m_settings;

        LayerStack m_layers;
        bool m_running;
    };
}

extern yoyo::Application *CreateApplication();