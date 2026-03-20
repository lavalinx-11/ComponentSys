#include <SDL.h>
#include "Scenes/SceneManager.h"
#include "Engine/Timer.h"
#include "Engine/Window.h"
#include "Scenes/Scene0g.h"
#include "Engine/GuiWindow.h"
#include "Engine/UIManager.h"
SceneManager::SceneManager(): 
	currentScene{nullptr}, window{nullptr}, timer{nullptr},
	fps(144), isRunning{ false }, fullScreen{ false }, imguiWin{ nullptr } {
	Debug::Info("Starting the SceneManager", __FILE__, __LINE__);
}

SceneManager::~SceneManager() {
	Debug::Info("Deleting the SceneManager", __FILE__, __LINE__);

	if (currentScene) {
		currentScene->OnDestroy();
		delete currentScene;
		currentScene = nullptr;
	}
	
	if (timer) {
		delete timer;
		timer = nullptr;
	}

	if (window) {
		delete window;
		window = nullptr;
	}

	if (imguiWin) {
		delete imguiWin;
		imguiWin = nullptr;
	}
}

bool SceneManager::Initialize(std::string name_, int width_, int height_) {

	bool useImGui = true; // flip this to false if you want to use Window instead

	if (useImGui)
	{
		imguiWin = new GuiWindow();
		if (!imguiWin->OnCreate(name_, width_, height_))
		{
			Debug::FatalError("Failed to initialize ImGuiWindow object", __FILE__, __LINE__);
			return false;
		}
	}
	else
	{
		window = new Window();
		if (!window->OnCreate(name_, width_, height_))
		{
			Debug::FatalError("Failed to initialize Window object", __FILE__, __LINE__);
			return false;
		}
	}
	timer = new Timer();
	if (timer == nullptr) {
		Debug::FatalError("Failed to initialize Timer object", __FILE__, __LINE__);
		return false;
	}

	/********************************   Default first scene   ***********************/
	BuildNewScene(SCENE_NUMBER::SCENE0g);
	/********************************************************************************/
	return true;
}

/// This is the whole game
void SceneManager::Run() {
	timer->Start();
	isRunning = true;
	while (isRunning)
	{
		HandleEvents();
		timer->UpdateFrameTicks();

		if (imguiWin)
		{
			imguiWin->BeginFrame();
		}

		// Scene logic
		currentScene->Update(timer->GetDeltaTime());
		currentScene->Render();

		// --- Scene-specific GUI ---
		if (imguiWin && currentScene)
		{
			currentScene->RenderGUI(); // <-- new per-scene ImGui
		}

		// --- Global debug GUI ---
		if (imguiWin)
		{
			ImVec4 r = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			ImVec4 g = ImVec4(0.0f, 1.0f, 0.0f, 0.7f);
			ImVec4 b = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
			ImVec4 text = ImVec4(0.95f, 0.95f, 1.0f, 1.0f); // very light blue-white

			UIManager::StartInvisibleWindow("Hidden WindowSceneManager", ImVec2(1845, 0)); // we start an invisible window here
			UIManager::PushButtonStyle(b, g, r, 90.0f); // pushing button style
			UIManager::PushTextStyle(text, 20.0f); // pushing text style
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("FPS: %1.f", io.Framerate);
			if (ImGui::Button("Refresh"))
			{
				BuildNewScene(SCENE_NUMBER::SCENE0g);
			}

			UIManager::PopButtonStyle(); // popping button style
			UIManager::PopTextStyle(); // popping text style
			UIManager::EndWindow(); // end the invisible window
		}

		if (imguiWin)
		{
			imguiWin->EndFrame();
		}
		else if (window)
		{
			SDL_GL_SwapWindow(window->getWindow());
		}

		SDL_Delay(timer->GetSleepTime(fps));
	}
}

void SceneManager::HandleEvents() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) { /// Loop over all events in the SDL queue
		ImGui_ImplSDL3_ProcessEvent(&sdlEvent);
		
		if (sdlEvent.type == SDL_EventType::SDL_EVENT_QUIT) {
			isRunning = false;
			return;
		}
		else if (sdlEvent.type == SDL_EVENT_KEY_DOWN) {
			switch (sdlEvent.key.scancode) {
			[[fallthrough]]; /// C17 Prevents switch/case fallthrough warnings
			case SDL_SCANCODE_ESCAPE:
			case SDL_SCANCODE_P:
				isRunning = false;
				return;
				

			case SDL_SCANCODE_F1:
			case SDL_SCANCODE_F2:
			case SDL_SCANCODE_F3:
			case SDL_SCANCODE_F4:
			case SDL_SCANCODE_F5:
		
				BuildNewScene(SCENE_NUMBER::SCENE0g);
				break;

			default:
				break;
			}
		}
		if (currentScene == nullptr) { /// Just to be careful
			Debug::FatalError("No currentScene", __FILE__, __LINE__);
			isRunning = false;
			return;
		}
		currentScene->HandleEvents(sdlEvent);
	}
}

bool SceneManager::BuildNewScene(SCENE_NUMBER scene) {
	bool status; 

	if (currentScene != nullptr) {
		currentScene->OnDestroy();
		delete currentScene;
		currentScene = nullptr;
	}

	switch (scene) {
	case SCENE_NUMBER::SCENE0g:
		currentScene = new Scene0g();
		status = currentScene->OnCreate();
		break;
	

	/*case SCENE_NUMBER::SCENE1g:
		currentScene = new Scene1g();
		status = currentScene->OnCreate();
		break;*/

	default:
		Debug::Error("Incorrect scene number assigned in the manager", __FILE__, __LINE__);
		currentScene = nullptr;
		return false;
	}
	return true;
}


