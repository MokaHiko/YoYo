#include <Yoyo.h>

#include <Core/Log.h>
#include <Math/Math.h>

class Sandbox : public yoyo::Application
{
public:
    Sandbox()
        : yoyo::Application({"Sandbox", 0, 0, 720, 480})
    {
    }
};

yoyo::Application *CreateApplication()
{
    return new Sandbox();
};