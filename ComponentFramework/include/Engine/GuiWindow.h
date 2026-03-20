#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <glew.h>




class GuiWindow {
public:
    GuiWindow();
    ~GuiWindow();

    bool OnCreate(std::string name, int width, int height);
    void OnDestroy();
    void BeginFrame();
    void EndFrame();

    SDL_Window* GetSDLWindow() const { return window; }

private:
    SDL_Window* window;
    SDL_GLContext context;
    int width;
    int height;

    void GetInstalledOpenGlInfo(int* major, int* minor);
    void SetAttributes(int major, int minor);
};

