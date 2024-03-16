#pragma once

#include "ECS/Components/Components.h"
#include "ECS/System.h"

// Updates transforms and relative transform components of the scene hierarchy
class SceneGraph : public System<TransformComponent>
{
public:
    SceneGraph(Scene* scene)
        :System<TransformComponent>(scene)
    {
    }

    virtual ~SceneGraph() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update(float dt) override;

    virtual void OnComponentCreated(Entity e, TransformComponent& transform) override;
    virtual void OnComponentDestroyed(Entity e, TransformComponent& transform) override;
private:
    void RecursiveUpdate(TransformComponent& node);
};