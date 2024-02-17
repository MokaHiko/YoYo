#include "Layer.h"
#include "Log.h"

namespace yoyo
{
    LayerStack::LayerStack()
        : m_insert_index(0) {}

    LayerStack::~LayerStack()
    {
        for (Layer *layer : m_layers)
        {
            delete layer;
        }
    }

    void LayerStack::PushLayer(Layer *layer)
    {
        m_layers.emplace(m_layers.begin() + m_insert_index, layer);
        m_insert_index++;
    }

    void LayerStack::PopLayer(Layer *layer)
    {
        auto it = std::find(m_layers.begin(), m_layers.end(), layer);

        if (it != m_layers.end())
        {
            m_layers.erase(it);
            m_insert_index--;
        }
    }

    void LayerStack::EnableLayer(const std::string &layer_name)
    {
        for (Layer *layer : m_layers)
        {
            if (layer->Name() == layer_name)
            {
                layer->Enable();
                return;
            }
        }

        YERROR("No layer with such name");
    }

    void LayerStack::DisableLayer(const std::string &layer_name)
    {
        for (Layer *layer : m_layers)
        {
            if (layer->Name() == layer_name)
            {
                layer->Disable();
                return;
            }
        }

        YERROR("No layer with such name");
    }

    Layer::Layer(const std::string &name)
    {
        m_enabled = false;
    }

    Layer::Layer()
    {
        m_enabled = false;
    }

    void Layer::Enable()
    {
        m_enabled = true;
        OnEnable();
    }

    void Layer::Disable()
    {
        m_enabled = false;
        OnDisable();
    }
}