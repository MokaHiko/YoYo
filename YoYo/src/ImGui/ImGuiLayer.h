#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include "Defines.h"

#include "Core/Memory.h"
#include "Core/Layer.h"

#include "Renderer/Texture.h"

// TODO: Change to PlatformAppHandle
struct SDL_Window;

namespace yoyo
{
    class Event;
    class Application;

    class YAPI ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer(Application* app);
        virtual ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetatch() override;

        virtual void OnEnable() override;
        virtual void OnDisable() override;

        static ImGuiContext* GetContext();

        bool OnEvent(Ref<Event> event);

        LayerType(InputLayer)
    public:
        void OnMainPassBegin(void* render_context);
        void OnMainPassEnd(void* render_context);
    private:
        void SetupImGuiStyle(bool bStyleDark_, float alpha_);
    private:
        Application* m_app;
        SDL_Window* m_window;
    };
}

namespace ImGui
{
	void YAPI Image(Ref<yoyo::Texture> texture, const ImVec2& image_size);
}