#ifndef APPLICATION_H
#define APPLICATION_H

#include "../managers/GuiRenderer.h"
#include "../managers/WindowManager.h"

class Application
{
public:
    Application();
    ~Application();

    void Init();
    void Run();

private:
    WindowManager m_windowManager;
    GuiRenderer m_guiRenderer;
};

#endif