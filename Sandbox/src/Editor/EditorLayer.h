#pragma once

#include <Core/Layer.h>
#include "Panel/IPanel.h"

#include <Core/Application.h>

#include "ECS/Entity.h"

namespace yoyo
{
    class Application;
}

class EditorLayer: public yoyo::Layer
{
public:
    LayerType(EditorLayer)

    EditorLayer(yoyo::Application* app);
    virtual ~EditorLayer();

    virtual void OnImGuiRender();

    virtual void OnAttach() override;
    virtual void OnDetatch() override;

    virtual void OnEnable() override;
    virtual void OnDisable() override;
private:
    std::vector<Ref<IPanel>> m_panels;
    yoyo::Application* m_app;

    // If true, the editor layer will not be rendered
    bool m_hide;

    Entity m_focused_entity;
    Scene* m_scene;
};
