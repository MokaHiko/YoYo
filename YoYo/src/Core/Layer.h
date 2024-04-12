#pragma once

#include "Defines.h"
#include "Platform/Platform.h"

namespace yoyo
{
    using LayerID = uint64_t;

    // Application layer base class. Layers are disabled by default
    class YAPI Layer
    {
    public:
        Layer();
        virtual ~Layer() {};

        virtual const LayerID ID() const = 0;
        virtual const std::string& Name() const = 0;

        // Called the moment the layer is pushed to the layer stack
        virtual void OnAttach() {};

        virtual void OnDetatch() {};

        // Enable is called after all other layers have been attached
        virtual void OnEnable(){};

        virtual void OnDisable(){};

        virtual void OnUpdate(float dt) {};

        virtual void OnImGuiRender() {};

        void Enable();
        void Disable();

        inline const bool IsEnabled() const {return m_enabled;}
    protected:
        friend class LayerStack;
        bool m_enabled;
    };

#define LayerType(type) const yoyo::LayerID ID() const override {return s_layer_id;}\
                        inline static const yoyo::LayerID s_layer_id = yoyo::Platform::GenerateUUIDV4();\
                        inline const std::string& Name() const override {return s_name;}\
                        inline static const std::string s_name = #type;\

    // Manages layers and their lifetimes
    class LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);
        void PopLayer(Layer* layer);

        void EnableLayer(const std::string& layer_name);
        void DisableLayer(const std::string& layer_name);

        std::vector<Layer*>::iterator begin() { return m_layers.begin(); } 
        std::vector<Layer*>::iterator end() { return m_layers.end(); }

        std::vector<Layer*>::reverse_iterator rbegin() { return m_layers.rbegin(); } 
        std::vector<Layer*>::reverse_iterator rend() { return m_layers.rend(); }
    private:
        std::vector<Layer*> m_layers;
        uint32_t m_insert_index = 0;
    };
}