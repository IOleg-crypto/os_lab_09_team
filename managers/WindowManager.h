#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

// ============================================================================
// THIRD-PARTY LIBRARY HEADERS
// ============================================================================
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// ============================================================================

#include <GL/gl.h>
#include <dwmapi.h>
// ============================================================================
// STANDARD C++ LIBRARY HEADERS
// ============================================================================
#include <iostream>
#include <string>

class WindowManager
{
public:
    WindowManager();
    ~WindowManager();

    bool Initialize(int width, int height, const char *title);
    void Shutdown();

public:
    GLFWwindow *GetWindow() const;
    bool ShouldClose() const;
    void SwapBuffers();
    void PollEvents();

private:
    void SetupPlatformSpecificWindow();

    GLFWwindow *m_window;
    bool m_initialized;
};

#endif // WINDOW_MANAGER_H
