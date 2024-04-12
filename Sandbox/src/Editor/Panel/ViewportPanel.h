#pragma once

#include <Core/Memory.h>

#include "IPanel.h"
#include "ECS/Entity.h"

namespace yoyo
{
    class Renderer;
}

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
