// ============================================================================
// PROJECT HEADERS
// ============================================================================
#include "GuiRenderer.h"
#include "../core/Application.h" // For context if needed, though ctx is passed

// ============================================================================
// STANDARD C++ LIBRARY HEADERS
// ============================================================================
#include <iostream>

GuiRenderer::GuiRenderer() : m_window(nullptr), m_initialized(false)
{
}

GuiRenderer::~GuiRenderer()
{
    Shutdown();
}

bool GuiRenderer::Initialize(GLFWwindow *window)
{
    if (m_initialized)
        return true;

    m_window = window;
    InitImGui(window);
    m_initialized = true;
    return true;
}

void GuiRenderer::Shutdown()
{
    if (m_initialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        m_initialized = false;
    }
}

void GuiRenderer::NewFrame()
{
    if (!m_initialized)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiRenderer::Render()
{
    if (!m_initialized)
        return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiRenderer::InitImGui(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImFontConfig config;
    config.SizePixels = 18.0f;

    ImFont *font =
        io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", config.SizePixels, &config);
    if (!font)
    {
        font = io.Fonts->AddFontDefault(&config);
    }
    const char *glsl_version = "#version 330";

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    SetDarkStyle();
}

void GuiRenderer::SetDarkStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();

    // style.WindowRounding = 0.0f;
    // style.FrameRounding = 3.0f;
    // style.ScrollbarRounding = 3.0f;
    // style.GrabRounding = 3.0f;
    // style.WindowBorderSize = 1.0f;
    // style.FrameBorderSize = 1.0f;
    // style.TabRounding = 0.0f;
    // style.ChildRounding = 3.0f;

    // ImVec4 *colors = style.Colors;

    // colors[ImGuiCol_WindowBg] = ImVec4(0.051f, 0.051f, 0.051f, 1.0f);
    // colors[ImGuiCol_ChildBg] = ImVec4(0.051f, 0.051f, 0.051f, 1.0f);
    // colors[ImGuiCol_PopupBg] = ImVec4(0.051f, 0.051f, 0.051f, 1.0f);

    // colors[ImGuiCol_Border] = ImVec4(0.278f, 0.278f, 0.278f, 1.0f);
    // colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // colors[ImGuiCol_FrameBg] = ImVec4(0.078f, 0.078f, 0.078f, 1.0f);
    // colors[ImGuiCol_FrameBgHovered] = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    // colors[ImGuiCol_FrameBgActive] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);

    // colors[ImGuiCol_TitleBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.0f);
    // colors[ImGuiCol_TitleBgActive] = ImVec4(0.039f, 0.039f, 0.039f, 1.0f);
    // colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.02f, 0.02f, 0.02f, 1.0f);

    // colors[ImGuiCol_MenuBarBg] = ImVec4(0.039f, 0.039f, 0.039f, 1.0f);

    // colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    // colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    // colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 1.0f);

    // colors[ImGuiCol_Button] = ImVec4(0.078f, 0.078f, 0.078f, 1.0f);
    // colors[ImGuiCol_ButtonHovered] = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    // colors[ImGuiCol_ButtonActive] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);

    // colors[ImGuiCol_Header] = ImVec4(0.078f, 0.078f, 0.078f, 1.0f);
    // colors[ImGuiCol_HeaderHovered] = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    // colors[ImGuiCol_HeaderActive] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);

    // colors[ImGuiCol_Separator] = ImVec4(0.278f, 0.278f, 0.278f, 1.0f);
    // colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    // colors[ImGuiCol_SeparatorActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
}

void GuiRenderer::DrawMainGUI(RenderContext &ctx)
{
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

    // Simple window
    if (ImGui::Begin("Simple ImGui Window", &ctx.exit))
    {
        ImGui::Text("Hello from Cross-Platform ImGui!");

        static int counter = 0;
        if (ImGui::Button("Click Me (Counter)"))
        {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("Clicks: %d", counter);

        static bool show_demo = false;
        ImGui::Checkbox("Toggle State", &show_demo);
        if (show_demo)
        {
            ImGui::Text("The toggle is ON!");
        }

        ImGui::Separator();

        if (ImGui::Button("Close Application"))
        {
            ctx.exit = true;
        }
    }
    ImGui::End();

    if (ctx.exit)
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}
