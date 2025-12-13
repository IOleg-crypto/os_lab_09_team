// ============================================================================
// PROJECT HEADERS
// ============================================================================
#include "WindowManager.h"

// ============================================================================
// STANDARD C++ LIBRARY HEADERS
// ============================================================================
#include <cstdlib>
#include <cstring>

WindowManager::WindowManager() : m_window(nullptr), m_initialized(false)
{
}

WindowManager::~WindowManager()
{
    Shutdown();
}

bool WindowManager::Initialize(int width, int height, const char *title)
{
    if (m_initialized)
        return true;

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    m_window = glfwCreateWindow(screenWidth, screenHeight, title, nullptr, nullptr);

    if (!m_window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    SetupPlatformSpecificWindow();

    m_initialized = true;
    return true;
}

void WindowManager::SetupPlatformSpecificWindow()
{
    HWND hwnd = glfwGetWin32Window(m_window);
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLong(hwnd, GWL_EXSTYLE, style);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
}

void WindowManager::Shutdown()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    if (m_initialized)
    {
        glfwTerminate();
        m_initialized = false;
    }
}

bool WindowManager::ShouldClose() const
{
    if (!m_window)
        return true;

    return glfwWindowShouldClose(m_window);
}

void WindowManager::SwapBuffers()
{
    if (m_window)
        glfwSwapBuffers(m_window);
}

void WindowManager::PollEvents()
{
    glfwPollEvents();
}
GLFWwindow *WindowManager::GetWindow() const
{
    return m_window;
}
