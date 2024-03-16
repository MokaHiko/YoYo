#pragma once

#include "IPanel.h"

#include "ECS/Entity.h"
#include "ECS/EntityImpl.h"

using DrawComponentHandler = std::function<void(Entity e)>;

template<typename T>
void DrawComponentUI(const std::string& name, Entity entity, std::function<void(T&)> ui_function, bool open = false)
{
    if (!entity.HasComponent<T>())
    {
        return;
    }

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
    ImGuiTreeNodeFlags flags = base_flags; 
    if(open)
    {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    if (ImGui::CollapsingHeader(name.c_str(), flags))
    {
        if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1])
        {
            ImGui::OpenPopup("Component Options");
        }

        if (ImGui::BeginPopup("Component Options"))
        {
            bool removed = false;
            if (ImGui::Button("Remove Component"))
            {
                removed = entity.RemoveComponent<T>();
            }
            ImGui::EndPopup();

            if (removed)
            {
                return;
            }
        }

        ui_function(entity.GetComponent<T>());
    }
}

class InspectorPanel : public IPanel
{
public:
    InspectorPanel();
    ~InspectorPanel();

    virtual void Draw(Scene* scene) override;
private:
    void DrawComponents();
    Entity m_focused_entity;
};
