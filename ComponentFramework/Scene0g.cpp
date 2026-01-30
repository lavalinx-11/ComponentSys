#include <glew.h>
#include <iostream>
#include <SDL3/SDL.h>
#include "Scene0g.h"
#include <MMath.h>
#include <QMath.h>
#include "Debug.h"
#include "GuiWindow.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "TransformComponent.h"
///ImGui includes
#include "UIManager.h"


Scene0g::Scene0g() :
drawInWireMode{false},
window{ nullptr }, 
context{ nullptr }
{
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0g::~Scene0g() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool Scene0g::OnCreate() {

	window = new Window();
	camera = new CameraActor(nullptr, 45.0f, 16.0f / 9.0f, 0.1f, 100.0f, window->getWindow());
	camera->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, -15.0f), Quaternion());
	camera->OnCreate();
	camera->setCamMovement(true);

	actor = new Actor(nullptr);
	actor->AddComponent<MaterialComponent>(nullptr, "textures/mario_main.png");
	actor->AddComponent<MeshComponent>(nullptr, "meshes/Mario.obj");
	actor->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	actor->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.0f), Quaternion(0.0f,(Vec3(0.0f, 1.0f, 0.0f))));
	actor->OnCreate(); 
	actor->ListComponents();

	/*actor = new Actor(nullptr);
	actor->AddComponent<MaterialComponent>(nullptr, "textures/mario_main.jpg");
	actor->AddComponent<MeshComponent>(nullptr, "meshes/Mario.obj");
	actor->AddComponent<ShaderComponent>(nullptr, "shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	actor->AddComponent<TransformComponent>(nullptr, Vec3(0.0f, -1.0f, -5.0f), QMath::angleAxisRotation(15.0f, Vec3(1.0f, 0.0f,0.0f)),Vec3(0.0f, 1.0f, 0.0f));
	actor->OnCreate();*/
	return true;
}

void Scene0g::OnDestroy() {
	actor->OnDestroy();
	camera->OnDestroy();
	window->OnDestroy();
	delete window;
}

void Scene0g::HandleEvents(const SDL_Event &sdlEvent) {
	camera->HandleEvents(sdlEvent);
	switch( sdlEvent.type ) {
    case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
			
		}
		break;

	case SDL_EVENT_MOUSE_MOTION:
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		break; 

	case SDL_EVENT_MOUSE_BUTTON_UP:
	break;

	default:
		break;
    }
}

void Scene0g::RenderGUI()
{
	ImVec4 r = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 g = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 b = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);

	UIManager::StartInvisibleWindow("GunSelector", ImVec2(0, 50));
	UIManager::PushButtonStyle(b, g, r, 5.0f);

	if (ImGui::Button("Test")) {
		r = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
	}

	if (ImGui::Button("Test Two")) {
		r = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	UIManager::PopButtonStyle();
	UIManager::EndWindow();
}

void Scene0g::Update(const float deltaTime) {
	std::cout << deltaTime << std::endl;
	camera->Update(deltaTime);
}

void Scene0g::Render() const {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	ShaderComponent* shader = actor->GetComponent<ShaderComponent>();

	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera->GetViewMatrix());
	glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1, GL_FALSE,actor->GetComponent<TransformComponent>()->GetTransformMatrix());
	glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
	actor->GetComponent<MeshComponent>()->Render();
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	
}



	
