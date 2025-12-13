#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

// ============================================================================
// THIRD-PARTY LIBRARY HEADERS
// ============================================================================
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// ============================================================================
// STANDARD C++ LIBRARY HEADERS
// ============================================================================
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <windows.h>

struct RenderContext
{
    bool exit = false;
};

class GuiRenderer
{
public:
    GuiRenderer();
    ~GuiRenderer();

    bool Initialize(GLFWwindow *window);
    void Shutdown();
    void NewFrame();
    void Render();

    // Main drawing interface
    void DrawMainGUI(RenderContext &ctx);

private:
    void InitImGui(GLFWwindow *window);
    static void SetDarkStyle();

private:
    GLFWwindow *m_window;
    bool m_initialized;
};

#endif // GUI_RENDERER_H
