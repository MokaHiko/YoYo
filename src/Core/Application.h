#pragma once

#include "Defines.h"

#include "Events/Event.h"
#include "Layer.h"
#include "Log.h"

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
        Application(const ApplicationSettings& settings);
        virtual ~Application();

#ifdef Y_DEBUG
        std::unordered_map<std::string, float> d_layer_profiles;
#endif
        void Init();
        void Run();
        void Shutdown();

        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        LayerStack& Layers() { return m_layers; } // Returns the layers of the application

        ApplicationSettings& Settings();

        bool OnWindowResize(float x, float y, float width, float height);
        bool OnClose();

        template<typename T>
        T* FindLayer()
        {
            for (Layer* layer : m_layers)
            {
                if (layer->Name() == T::s_name)
                {
                    return (T*)(layer);
                }
            }

            YERROR("Application has no such layer!");
            return nullptr;
        }

    private:
        ApplicationSettings m_settings;
        LayerStack m_layers;
        bool m_running;
    };
}

extern yoyo::Application* CreateApplication();