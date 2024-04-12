#pragma once

#include "Defines.h"
#include "Application.h"

extern yoyo::Application* CreateApplication();

YAPI int main()
{
    yoyo::Application* app = CreateApplication();

    app->Init();
    app->Run();
    app->Shutdown();

    delete app;

    return 0;
}