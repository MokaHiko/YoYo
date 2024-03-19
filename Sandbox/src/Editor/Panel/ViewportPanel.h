#pragma once

#include "IPanel.h"
#include <Renderer/Texture.h>
#include <Renderer/Renderer.h>

#include "ECS/Entity.h"

class ViewportPanel : public IPanel
{
public:
    ViewportPanel(Ref<yoyo::Renderer>);
    ~ViewportPanel();

    virtual void Draw(Scene* scene) override;
private:
    Ref<yoyo::Renderer> m_renderer;
    Entity m_focused_entity;
};
