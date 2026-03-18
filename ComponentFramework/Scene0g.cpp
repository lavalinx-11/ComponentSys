#include <glew.h>
#include <iostream>
#include <cstdlib>
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

PieceType StringToPieceType(const std::string& typeStr) {
	if (typeStr == "Pawn")   return PieceType::PAWN;
	if (typeStr == "Rook")   return PieceType::ROOK;
	if (typeStr == "Knight") return PieceType::KNIGHT;
	if (typeStr == "Bishop") return PieceType::BISHOP;
	if (typeStr == "Queen")  return PieceType::QUEEN;
	if (typeStr == "King")   return PieceType::KING;
	return PieceType::PAWN;
}

bool Scene0g::OnCreate() {
	shader = std::make_shared<ShaderComponent>(std::weak_ptr<Component>(),"shaders/texturePhongVert.glsl","shaders/texturePhongFrag.glsl");
	shader->OnCreate();
	window = new Window();
	camera = std::make_shared<CameraActor>(std::weak_ptr<Actor>(), 45.0f, 16.0f / 9.0f, 0.1f, 100.0f, window->getWindow());
	camera->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 0.0f, -15.0f), Quaternion());
	camera->OnCreate();
	camera->setCamMovement(true);
	
	board = std::make_shared<Actor>(std::weak_ptr<Component>());
	board->AddComponent<MaterialComponent>(std::weak_ptr<Component>(), "textures/8x8_board_red.png");
	board->AddComponent<MeshComponent>(std::weak_ptr<Component>(), "meshes/Plane.obj");
	board->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 0.0f, 0.0f), QMath::angleAxisRotation(-90.0f, Vec3(1.0f, 0.0f, 0.0f)));
	board->GetComponent<TransformComponent>()->SetScale(Vec3(5.0f, 5.0f, 5.0f));
	board->OnCreate();

	Ref<MeshComponent> pawnMesh = std::make_shared<MeshComponent>(std::weak_ptr<Component>(),"meshes/Pawn.obj");
	pawnMesh->OnCreate();
	
	Ref<MeshComponent> queenMesh = std::make_shared<MeshComponent>(std::weak_ptr<Component>(),"meshes/Queen.obj");
	queenMesh->OnCreate();
	
	Ref<MeshComponent> kingMesh = std::make_shared<MeshComponent>(std::weak_ptr<Component>(),"meshes/King.obj");
	kingMesh->OnCreate();
	
	Ref<MeshComponent> bishopMesh = std::make_shared<MeshComponent>(std::weak_ptr<Component>(),"meshes/Bishop.obj");
	bishopMesh->OnCreate();
	
	Ref<MeshComponent> knightMesh = std::make_shared<MeshComponent>(std::weak_ptr<Component>(),"meshes/Knight.obj");
	knightMesh->OnCreate();
	
	Ref<MeshComponent> rookMesh = std::make_shared<MeshComponent>(std::weak_ptr<Component>(),"meshes/Rook.obj");
	rookMesh->OnCreate();
	
	Ref<MaterialComponent>blackChessPieces = std::make_shared<MaterialComponent>(std::weak_ptr<Component>(), "textures/BlackChessPiece.png");
	blackChessPieces->OnCreate();
	
	Ref<MaterialComponent>whiteChessPieces = std::make_shared<MaterialComponent>(std::weak_ptr<Component>(), "textures/WhiteChessPiece.png");
	whiteChessPieces->OnCreate();

	std::string pieceTypes[] = {
	"Rook", "Knight", "Bishop", "Queen", "King", "Bishop", "Knight", "Rook"
	};

	PieceType aType;
	for (int i = 0; i < 32; i++) {
		int pieceIndex = i % 16; 
		std::unique_ptr<Actor>actorNew = std::make_unique<Actor>(board); 
		std::string type = (pieceIndex < 8) ? pieceTypes[pieceIndex] : "Pawn";
		aType = StringToPieceType(type);
		actorNew->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(-4.4f, -4.4f, 0.0f), Quaternion(), Vec3(0.125f, 0.125f, 0.125f));
		Quaternion rot = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
		actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= rot);
		
		if (i < 16) {
			actorColour = "BlackActor";
		}
		else {
			actorColour = "WhiteActor";
		}
		
		switch (aType)
		{
		case(PAWN):
			actorNew->AddComponent<MeshComponent>(pawnMesh);
			break;
			
		case(KING):
			actorNew->AddComponent<MeshComponent>(kingMesh);

			break;
			
		case(QUEEN):
			actorNew->AddComponent<MeshComponent>(queenMesh);

			break;
			
		case(BISHOP):
			actorNew->AddComponent<MeshComponent>(bishopMesh);

			break;
			
		case(ROOK):
			actorNew->AddComponent<MeshComponent>(rookMesh);

			break;
			
		case(KNIGHT):
			actorNew->AddComponent<MeshComponent>(knightMesh);
			break;
		}
		
		if (actorColour == "BlackActor")
		{
			actorNew->AddComponent<MaterialComponent>(blackChessPieces);
		}
		
		if (actorColour == "WhiteActor")
		{
			actorNew->AddComponent<MaterialComponent>(whiteChessPieces);
		}
		
		actorNew->OnCreate();
		if (type == "Knight" && actorColour == "WhiteActor") {
			Quaternion extraRot = QMath::angleAxisRotation(180, Vec3(0.0f, 0.0f, 1.0f));
			actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= extraRot);
		}
		
		actorName = actorColour + std::to_string(i);
		actors.emplace(actorName, ActorData{std::move(actorNew), aType, actorColour});

	}


	// Places Pieces in the correct places
	for (int i = 0; i < actors.size(); i++) {
		Vec3 startingPosBlack = Vec3(-4.4f, 4.4f, 0.1f); 
		Vec3 startingPosRed = Vec3(-4.4f, -6.85f, 0.1f);
		int row = i / 8; 
		int col = i % 8; 
		float actorOffset = 1.25f;
		Vec3 actorPosBlack;
		actorPosBlack.x = startingPosBlack.x + (actorOffset * col); 
		actorPosBlack.y = startingPosBlack.y - (actorOffset * row);

		Vec3 actorPosRed;
		actorPosRed.x = startingPosRed.x + (actorOffset * col);
		actorPosRed.y = startingPosRed.y + (actorOffset * row); 
		if (i < 16) {
			actors.at("BlackActor" + std::to_string(i)).actor->GetComponent<TransformComponent>()->setPosition(actorPosBlack);
		}
		else   {
			actors.at("WhiteActor" + std::to_string(i)).actor->GetComponent<TransformComponent>()->setPosition(actorPosRed);
		}
	}

	return true;
}

void Scene0g::OnDestroy() {
	actors.clear();

	shader->OnDestroy();
	board->OnDestroy();
	camera->OnDestroy();
	window->OnDestroy();
	delete window;
}

void Scene0g::HandleEvents(const SDL_Event &sdlEvent) {
	camera->HandleEvents(sdlEvent);
	switch( sdlEvent.type ) {
    case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent.key.scancode) {
		case SDL_SCANCODE_J:
		{
			Vec3 speed = Vec3(0.0f, 0.0f, 1.0f);
			board->GetComponent<TransformComponent>()->setPosition(board->GetComponent<TransformComponent>()->GetPosition() + speed); 

			break;
		}
		case SDL_SCANCODE_K:
		{
			Vec3 speed = Vec3(0.0f, 0.0f, -1.0f);
			board->GetComponent<TransformComponent>()->setPosition(board->GetComponent<TransformComponent>()->GetPosition() + speed);

			break;
		}

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
	camera->Update(deltaTime);


	//Board Rotation
	float rotationSpeed = 10.0f;
	float frameRotation = rotationSpeed * deltaTime;
	Quaternion rota = QMath::angleAxisRotation(frameRotation, Vec3(0.0f, 1.0f, 0.0f));

	Ref<TransformComponent> boardTransform = board->GetComponent<TransformComponent>();
	if (boardTransform) {
		boardTransform->SetOrientation(boardTransform->GetOrientation() *= rota);
	}

}

void Scene0g::Render() const {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);



	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE, camera->GetProjectionMatrix());
	glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera->GetViewMatrix());

	
	glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1, GL_FALSE, board->GetComponent<TransformComponent>()->GetTransformMatrix());
	glBindTexture(GL_TEXTURE_2D, board->GetComponent<MaterialComponent>()->getTextureID());
	board->GetComponent<MeshComponent>()->Render();

	 
	for (const auto& pair : actors) {
		PieceType actorType = pair.second.actorType;
		Actor* piece = pair.second.actor.get();
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, piece->GetModelMatrix());
		glBindTexture(GL_TEXTURE_2D, piece->GetComponent<MaterialComponent>()->getTextureID());
		piece->GetComponent<MeshComponent>()->Render();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	
}

