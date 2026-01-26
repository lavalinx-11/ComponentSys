#include "GuiWindow.h"
#include "Debug.h"

///ImGui includes
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

GuiWindow::GuiWindow() : window{ nullptr }, context{ nullptr }, width{ 0 }, height{ 0 } {}
GuiWindow::~GuiWindow() { OnDestroy(); }

bool GuiWindow::OnCreate(std::string name, int width_, int height_) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Debug::FatalError("Failed to initialize SDL", __FILE__, __LINE__);
        return false;
    }

    this->width = width_;
    this->height = height_;
    window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_OPENGL);
    if (!window) {
        Debug::FatalError("Failed to create a window", __FILE__, __LINE__);
        return false;
    }

    context = SDL_GL_CreateContext(window);

    int major, minor;
    GetInstalledOpenGlInfo(&major, &minor);
    SetAttributes(major, minor);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        Debug::FatalError("Glew initialization failed", __FILE__, __LINE__);
        return false;
    }

    glViewport(0, 0, width, height);

    // --- ImGui setup ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

void GuiWindow::OnDestroy() {
    // --- ImGui cleanup ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (context) {
        SDL_GL_DestroyContext(context);
        context = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

void GuiWindow::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void GuiWindow::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void GuiWindow::GetInstalledOpenGlInfo(int* major, int* minor) {
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    glGetIntegerv(GL_MAJOR_VERSION, major);
    glGetIntegerv(GL_MINOR_VERSION, minor);

    Debug::Info("OpenGL version: " + std::string((char*)version), __FILE__, __LINE__);
    Debug::Info("Graphics card vendor: " + std::string((char*)vendor), __FILE__, __LINE__);
    Debug::Info("Graphics card name: " + std::string((char*)renderer), __FILE__, __LINE__);
    Debug::Info("GLSL Version: " + std::string((char*)glslVersion), __FILE__, __LINE__);
}

void GuiWindow::SetAttributes(int major, int minor) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    SDL_GL_SetSwapInterval(1);
    glewExperimental = GL_TRUE;
}
