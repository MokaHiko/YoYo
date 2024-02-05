#pragma once

#include "Defines.h"

namespace yoyo
{
    // Application layer base class. Layers are disabled by default
    class YAPI Layer
    {
    public:
        Layer(const std::string& name);
        Layer();
        virtual ~Layer() {};

        virtual void OnAttach() {};
        virtual void OnDetatch() {};

        virtual void OnEnable(){};
        virtual void OnDisable(){};

        virtual void OnUpdate(float dt) {};

        void Enable();
        void Disable();

        inline const std::string& GetName() const {return m_name;}
        inline const bool IsEnabled() const {return m_enabled;}
    protected:
        friend class LayerStack;

        uint32_t m_id;
        std::string m_name;
        bool m_enabled;
    };

    // Manages layers and their lifetimes
    class LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        void EnableLayer(const std::string& layerm_name);
        void DisableLayer(const std::string& layerm_name);

        std::vector<Layer*>::iterator begin() { return m_layers.begin(); } 
        std::vector<Layer*>::iterator end() { return m_layers.end(); }
    private:
        std::vector<Layer*> m_layers;
        uint32_t m_insert_index = 0;
    };
}