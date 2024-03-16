#pragma once

#include "IPanel.h"

class Entity;
class SceneHierarchyPanel : public IPanel
{
public:
    SceneHierarchyPanel();
    virtual ~SceneHierarchyPanel();

    virtual void Draw(Scene* scene) override;
private:
    void DrawNodeRecursive(Entity node, bool open = false);
};
