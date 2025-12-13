#include "Application.h"
#include <iostream>

// Force use of dedicated GPU
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // For NVIDIA
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // For AMD
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::Init()
{
    // Initialize Window
    if (!m_windowManager.Initialize(800, 600, "ImGui Window"))
    {
        std::cerr << "Failed to initialize Window Manager\n";
        exit(EXIT_FAILURE);
    }

    // Initialize GUI
    if (!m_guiRenderer.Initialize(m_windowManager.GetWindow()))
    {
        std::cerr << "Failed to initialize GUI Renderer\n";
        exit(EXIT_FAILURE);
    }
}

void Application::Run()
{
    while (!m_windowManager.ShouldClose())
    {
        m_windowManager.PollEvents();

        m_guiRenderer.NewFrame();

        // Draw Main GUI
        m_guiRenderer.DrawMainGUI();

        int display_w, display_h;
        glfwGetFramebufferSize(m_windowManager.GetWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Clear background (transparent)
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_guiRenderer.Render();

        m_windowManager.SwapBuffers();
    }
}
