#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <glew.h>


///ImGui includes
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

class UIManager {

public:

    
    UIManager();
    ~UIManager();

   static void StartInvisibleWindow(const char* Label, ImVec2 pos);
   static void EndWindow();
   static void PushButtonStyle(ImVec4 normal, ImVec4 hovered, ImVec4 active, float rounding = 0.0f);
   static void PushSliderStyle(ImVec4 normal, ImVec4 hovered, ImVec4 active, float rounding = 0.0f);
   static void PushTextStyle(ImVec4 color, float size = 0.0f);

   static void PopButtonStyle();
   static void PopSliderStyle();
   static void PopTextStyle();

};

