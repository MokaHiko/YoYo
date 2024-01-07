#pragma once

#include "Application.h"

extern yoyo::Application* CreateApplication();

int main()
{
    yoyo::Application* app = CreateApplication();

    app->Init();
    app->Run();
    app->Shutdown();

    delete app;

    return 0;
}