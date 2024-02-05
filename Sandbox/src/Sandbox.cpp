#include <Yoyo.h>

#include <Core/Log.h>
#include <Math/Math.h>

#include <ECS/Scene.h>
#include <Renderer/Meshes/MeshComponents.h>

#include <Input/Input.h>
#include <ECS/Components/Components.h>

#include <Renderer/Meshes/MeshComponents.h>

class GameLayer : public yoyo::Layer
{
public:
    virtual void OnAttach() override
    {
    };

    virtual void OnDetatch() override 
    {
    };

    virtual void OnEnable() override
    {
        m_scene = Y_NEW yoyo::Scene;
        m_scene->Instantiate("root");

        auto e = m_scene->Instantiate("cube");
        e.AddComponent<yoyo::MeshRenderer>();
    };

    virtual void OnDisable() override
    {
    };

    virtual void OnUpdate(float dt) override 
    {
        // Input
        if(yoyo::Input::GetKeyDown(yoyo::KeyCode::Key_a)){YINFO("GetKeyDown() => True!");}
        if(yoyo::Input::GetKey(yoyo::KeyCode::Key_d)){YINFO("GetKey() => True!");}

        // Rendering system (send render packets)

        // Scripting system (update scripts)
    };
private:
    yoyo::Scene* m_scene;
};

class Sandbox : public yoyo::Application
{
public:
    Sandbox()
        : yoyo::Application({"Sandbox", 0, 0, 720, 480})
    {
        PushLayer(Y_NEW GameLayer);
    }
};

yoyo::Application *CreateApplication()
{
    return new Sandbox();
};