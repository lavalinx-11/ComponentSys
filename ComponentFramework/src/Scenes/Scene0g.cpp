#include <glew.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <SDL3/SDL.h>
#include "Scenes/Scene0g.h"
#include <MMath.h>
#include <QMath.h>
#include "Engine/Debug.h"
#include "Engine/GuiWindow.h"
#include "Components/MaterialComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ShaderComponent.h"
#include "Components/TransformComponent.h"
#include <map>
///ImGui includes
#include "Engine/UIManager.h"


Scene0g::Scene0g() :
	currentState{MouseState::SELECTING_PIECE},drawInWireMode{false}, dynamicGridSize(0), gridOriginX(0), gridOriginY(0),
	window{nullptr},
	context{nullptr}
{
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0g::~Scene0g() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}


/*

				<-CAMERA MOVEMENT->
 Left Click + Mouse Movement = camera rotations 
 W = Move Forward
 A = Move Left
 S = Move Backward
 D = Move Right
 Q = Move Up
 E = Move Down
 
				<-BOARD MOVEMENT->
J = Move Board Forward
K = Move Board Backward
 
 I have 2 ImGui windows, one to manipulate my lights and another to manipulate the pieces. 
 There is also a toggle to rotate the board where the piece selector is.
 */



// Supporting function to convert strings to pieces to prevent an obnoxious render call in render
static PieceType StringToPieceType(const std::string& typeStr) {
	if (typeStr == "Pawn")   return PieceType::PAWN;
	if (typeStr == "Rook")   return PieceType::ROOK;
	if (typeStr == "Knight") return PieceType::KNIGHT;
	if (typeStr == "Bishop") return PieceType::BISHOP;
	if (typeStr == "Queen")  return PieceType::QUEEN;
	if (typeStr == "King")   return PieceType::KING;
	return PieceType::PAWN;
}
static inline float magSqr(const Vec3& v) {
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

bool Scene0g::OnCreate() {
	/*						<-ASSET MANAGER->										*/
	AssetManager::GetInstance().OnCreate("Include/Engine/Assets.xml");
	
	/*						<-SHADER AND CAMERA SETUP->										*/
	shader = AssetManager::GetInstance().GetComponent<ShaderComponent>("PhongShader");
	shader->OnCreate();
	
	window = new Window();
	camera = std::make_shared<CameraActor>(std::weak_ptr<Actor>(), 45.0f, 16.0f / 9.0f, 0.5f, 5000.0f, window->getWindow());
	camera->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 0.0f, -15.0f), Quaternion());
	camera->OnCreate();
	camera->SetCamMovement(true);
	camera->SkyboxSetup(
			"textures/spacePX.png",	
			"textures/spacePY.png",
			"textures/spacePZ.png",
			"textures/spaceNX.png",
			"textures/spaceNY.png",
			"textures/spaceNZ.png"
	);

	collisionSystem = std::make_unique<CollisionSystem>();
	
	/*							<-LIGHT INITIALIZATION->							*/
	lights[0] = std::make_unique<LightActor>(std::weak_ptr<Actor>(),
		Vec4(1.0f, 1.0f, 1.0f, 1.0f), // Specular
		Vec4(0.8f, 0.8f, 0.8f, 1.0f), // Diffuse
		Vec4(0.1f, 0.1f, 0.1f, 1.0f)  // Ambient
	);
	lights[0]->OnCreate();
	lights[0]->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 10.0f, 5.0f), Quaternion());
	for (int i = 1; i < 5; i++) {
		lights[i] = std::make_unique<LightActor>(std::weak_ptr<Actor>(),
			Vec4(0.5f, 0.5f, 0.5f, 1.0f), // Specular
			Vec4(0.4f, 0.4f, 0.4f, 1.0f), // Diffuse
			Vec4(0.05f, 0.05f, 0.05f, 1.0f) // Ambient
		);
		lights[i]->OnCreate();
		lights[i]->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(i * 2.0f, 5.0f, 0.0f), Quaternion());
	}
	SetupTheme("Midnight");
	
	/*						<-BOARD ACTOR SETUP->										*/
	board = std::make_shared<Actor>(std::weak_ptr<Component>());
	board->AddComponent<MaterialComponent>(AssetManager::GetInstance().GetComponent<MaterialComponent>("BoardMaterial"));
	board->AddComponent<MeshComponent>(AssetManager::GetInstance().GetComponent<MeshComponent>("PlaneMesh"));
	board->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 0.0f, 0.0f), QMath::angleAxisRotation(-90.0f, Vec3(1.0f, 0.0f, 0.0f)));
	board->GetComponent<TransformComponent>()->SetScale(Vec3(5.0f, 5.0f, 5.0f));
	board->AddComponent<CollisionComponent>(std::weak_ptr<Component>(), board->GetComponent<TransformComponent>(), AssetManager::GetInstance().GetComponent<MeshComponent>("PlaneMesh"));
	board->OnCreate();

	AABB boardBounds = board->GetComponent<CollisionComponent>()->GetAABB();
	Vec3 boardScale = board->GetComponent<TransformComponent>()->GetScale();
	
	float localHalfExtentX = boardBounds.halfExtents.x / boardScale.x;
	float localHalfExtentY = boardBounds.halfExtents.z / boardScale.z; 

	dynamicGridSize = (localHalfExtentX * 2.0f) / 8.0f; 

	gridOriginX = (-localHalfExtentX) + (dynamicGridSize / 2.0f); 
	gridOriginY = (localHalfExtentY)  - (dynamicGridSize / 2.0f); 

	
	/*						<-SETUP FOR COLLISION HITBOXES->					*/
	debugSphere = AssetManager::GetInstance().GetComponent<MeshComponent>("SphereMesh");
	debugSphere->OnCreate();

	debugCube = AssetManager::GetInstance().GetComponent<MeshComponent>("CubeMesh");
	debugCube->OnCreate();
	
	// This is a map used to make naming my pieces easier.
	std::map<std::string, int> pieceCounters;
	
	PieceType typeOfActor;
	auto& assetManager = AssetManager::GetInstance();
	for (int i = 0; i < 32; i++) {
		std::string pieceTypes[] = {
			"Rook", "Knight", "Bishop", "Queen", "King", "Bishop", "Knight", "Rook"
		};
		// Tools to index the Unordered Map to more easily index and find things.
		int pieceIndex = i % 16;
		//int row = i / 8; 
		int col = i % 8; 
		float actorOffset = 1.25f;
		
		// Used to determine what type of actor/chess piece this actor should be.
		std::string type = (pieceIndex < 8) ? pieceTypes[pieceIndex] : "Pawn";
		std::string color = (i < 16) ? "Black" : "White";
		pieceCounters[color + type]++;
		if (type == "King" || type == "Queen") {
			actorName = color + type;
		} else {
			actorName = color + type + std::to_string(pieceCounters[color + type]);
		}


		//							<-NEW ACTOR GENERATION->								//
		std::unique_ptr<Actor>actorNew = std::make_unique<Actor>(board);
		typeOfActor = StringToPieceType(type);
		
		
		switch (typeOfActor)
		{
		case(PAWN):
			actorNew->AddComponent<MeshComponent>(assetManager.GetComponent<MeshComponent>("PawnMesh"));
			break;
		case(KING):
			
			actorNew->AddComponent<MeshComponent>(assetManager.GetComponent<MeshComponent>("KingMesh"));
			break;
		case(QUEEN):
			actorNew->AddComponent<MeshComponent>(assetManager.GetComponent<MeshComponent>("QueenMesh"));
			break;
		case(BISHOP):
			actorNew->AddComponent<MeshComponent>(assetManager.GetComponent<MeshComponent>("BishopMesh"));
			break;
		case(ROOK):
			actorNew->AddComponent<MeshComponent>(assetManager.GetComponent<MeshComponent>("RookMesh"));
			break;
		case(KNIGHT):
			actorNew->AddComponent<MeshComponent>(assetManager.GetComponent<MeshComponent>("KnightMesh"));
			break;
		}
		Vec3 finalPos;

		// Based off of which type of piece it is give it the corresponding texture and update its position

		int boardRow;
		if (color == "Black")
		{
			actorNew->AddComponent<MaterialComponent>(assetManager.GetComponent<MaterialComponent>("BlackMaterial"));
			boardRow = (i < 8) ? 0 : 1;
		}
		
		if (color == "White")
		{
			actorNew->AddComponent<MaterialComponent>(assetManager.GetComponent<MaterialComponent>("WhiteMaterial"));
			boardRow = (i < 24) ? 7 : 6;

		}

		finalPos = Vec3(gridOriginX + (col * dynamicGridSize), gridOriginY - (boardRow * dynamicGridSize), 0.005f);


		
		actorNew->AddComponent<TransformComponent>(std::weak_ptr<Component>(), finalPos, Quaternion(), Vec3(0.125f, 0.125f, 0.125f));
		Quaternion rot = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
		actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= rot);
		//actorNew->AddComponent<CollisionComponent>(std::weak_ptr<Component>(),actorNew->GetComponent<TransformComponent>(), 0.34f); //Sphere Setup
		actorNew->AddComponent<CollisionComponent>(std::weak_ptr<Component>(),actorNew->GetComponent<TransformComponent>(),actorNew->GetComponent<MeshComponent>());
		actorNew->AddComponent<PhysicsComponent>(std::weak_ptr<Component>(), 1.0f);
		actorNew->GetComponent<PhysicsComponent>()->SetTransform(actorNew->GetComponent<TransformComponent>());
		
		
		// Rotate the white knights as they face the opposite direction by default
		actorNew->OnCreate();
		if (type == "Knight" && color == "White") {
			Quaternion extraRot = QMath::angleAxisRotation(180, Vec3(0.0f, 0.0f, 1.0f));
			actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= extraRot);
		}
		// Add the actors into the list assigning the name, move in the pointer, the type of actor it is and the colour of the actor.
		Vec3 currentPos = actorNew->GetComponent<TransformComponent>()->GetPosition();
		Vec3 currentVel = actorNew->GetComponent<PhysicsComponent>()->GetVelocity();
		int targetCol = std::round((currentPos.x - gridOriginX) / dynamicGridSize);
		int targetRow = std::round((gridOriginY - currentPos.y) / dynamicGridSize);
		actors.emplace(actorName, ActorData{std::move(actorNew), typeOfActor, color, targetCol, targetRow, targetCol, targetRow});
	}
	return true;
}

void Scene0g::OnDestroy() {
	actors.clear();
	shader->OnDestroy();
	board->OnDestroy();
	camera->OnDestroy();
	window->OnDestroy();
	collisionSystem.reset();
	debugCube.reset();
	debugSphere.reset();
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
			board->GetComponent<TransformComponent>()->SetPosition(board->GetComponent<TransformComponent>()->GetPosition() + speed); 

			break;
		}
		case SDL_SCANCODE_K:
		{
			Vec3 speed = Vec3(0.0f, 0.0f, -1.0f);
			board->GetComponent<TransformComponent>()->SetPosition(board->GetComponent<TransformComponent>()->GetPosition() + speed);

			break;
		}
		}
		break;

	case SDL_EVENT_MOUSE_MOTION:
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
if (sdlEvent.button.button == SDL_BUTTON_LEFT)
{
	int mouseX = sdlEvent.button.x;
	int mouseY = sdlEvent.button.y;
	
	HandleMouseClick(mouseX, mouseY);
}
		
		break; 

	case SDL_EVENT_MOUSE_BUTTON_UP:
	break;

	default:
		break;
    }
}

void Scene0g::RenderGUI()
{
	//Setup for lighting config window
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(260, 450), ImGuiCond_Always);
	ImGui::Begin("Lighting Inspector");
    
    if (ImGui::CollapsingHeader("Global Themes")) {
        if (ImGui::Button("Bright"))   SetupTheme("Bright");
        ImGui::SameLine();
        if (ImGui::Button("Midnight")) SetupTheme("Midnight");
        ImGui::SameLine();
        if (ImGui::Button("Sunset"))   SetupTheme("Sunset");
    }

	for (int i = 0; i < 5; i++) {
		std::string label = "Light " + std::to_string(i);

		ImGui::PushID(i); 

		if (ImGui::TreeNode(label.c_str())) {

			Vec3 pos = lights[i]->GetComponent<TransformComponent>()->GetPosition();
			if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
				lights[i]->GetComponent<TransformComponent>()->SetPosition(pos);
			}

			Vec4 diff = lights[i]->GetDiffuse();
			if (ImGui::ColorEdit3("Diffuse", &diff.x)) {
				lights[i]->SetDiffuse(diff);
			}

			float amb = lights[i]->GetAmbient().x;
			if (ImGui::SliderFloat("Ambient Intensity", &amb, 0.0f, 1.0f)) {
				lights[i]->SetAmbient(Vec4(amb, amb, amb, 1.0f));
			}

			ImGui::TreePop();
		}
		ImGui::PopID(); 
    }
    ImGui::End();

	// ImGui window initialization for the piece selector
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	float desiredWidth = 400.0f;  
	float desiredHeight = 500.0f; 
	float windowPosX = viewport->WorkPos.x + viewport->WorkSize.x - desiredWidth - 10.0f;
	float windowPosY = viewport->WorkPos.y + 50.0f; // Top padding

	ImGui::SetNextWindowPos(ImVec2(windowPosX, windowPosY), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(desiredWidth, desiredHeight), ImGuiCond_Always);
	ImGui::Begin("Piece Selector");

	// Highlight a specific piece based off of typing and order in list
	auto renderPieceList = [&](const std::string& colorFilter) {
		if (ImGui::TreeNode(colorFilter.c_str())) {
			// Define the specific order you want them displayed
			PieceType displayOrder[] = { 
				PieceType::KING, 
				PieceType::QUEEN, 
				PieceType::ROOK, 
				PieceType::BISHOP, 
				PieceType::KNIGHT, 
				PieceType::PAWN 
			};

			for (PieceType currentType : displayOrder) {
				std::vector<std::string> sortedNames;
				for (auto const& pair : actors) {
					if (pair.second.colour == colorFilter && pair.second.actorType == currentType) {
						sortedNames.push_back(pair.first);
					}
				}
				
				std::sort(sortedNames.begin(), sortedNames.end(), [](const std::string& a, const std::string& b) {
					if (a.length() != b.length()) return a.length() < b.length();
					return a < b;
				});
				for (const std::string& name : sortedNames) {
					bool isSelected = (selectedActorName == name);
					if (ImGui::Selectable(name.c_str(), isSelected)) {
						selectedActorName = name;
					}
				}
			}
			ImGui::TreePop();
		}
	};

	renderPieceList("Black");
	renderPieceList("White");


	// Clear the highlight selection
	if (ImGui::Button("Clear Selection")) {
		selectedActorName = "";
	}

	// Toggle for the board spinner
	if (ImGui::Button("Board Spinner"))
	{
		if (canBoardSpin)
		{
			canBoardSpin = false;
		}
		else
		{
			canBoardSpin = true;
		}
	}
	if (ImGui::Button("Show Hitboxes"))
	{
		if (showHitboxes)
		{
			showHitboxes = false;
		}
		else
		{
			showHitboxes = true;
		}
	}

	if (ImGui::Button("Return Home"))
	{
		for (auto &pair : actors)
		{
			pair.second.lastValidCol = pair.second.homeCol;
			pair.second.lastValidRow = pair.second.homeRow;

			pair.second.isReturning = true;
		}
	}
	float cameraSpeed = camera->GetCameraSpeed();
	float cameraSensitivity = camera->GetSensitivity();
	if (ImGui::SliderFloat("Cam Speed",&cameraSpeed, 0.1f, 50.0f)) {
		camera->SetCameraSpeed(cameraSpeed);
	}

	if (ImGui::SliderFloat("Cam Sensitivity",&cameraSensitivity, 0.1f, 1.0f)) {
		camera->SetCamSensitivity(cameraSensitivity);
	}

	
	ImGui::End();
}

//					<-PRESET LIGHTING THEMES->					//
void Scene0g::SetupTheme(const std::string& themeName)
{
	isTransitioning = true;
	transitionAlpha = 0.0f;

	for (int i = 0; i < 5; i++) {
		startDiffuse[i] = lights[i]->GetDiffuse();
		startSpecular[i] = lights[i]->GetSpecular();
		startAmbient[i] = lights[i]->GetAmbient();
		startPos[i] = lights[i]->GetComponent<TransformComponent>()->GetPosition();
	}
	
	if (themeName == "Midnight") {
		targetDiffuse[0] = Vec4(0.2f, 0.2f, 0.5f, 1.0f);
		targetSpecular[0] = Vec4(0.9f, 0.9f, 1.0f, 1.0f);
		targetAmbient[0] = Vec4(0.02f, 0.02f, 0.05f, 1.0f);
		targetPos[0] = Vec3(0.0f, 15.0f, -10.0f);
		for(int i=1; i<5; i++) targetDiffuse[i] = Vec4(0.01f, 0.01f, 0.03f, 1.0f);
	}
	else if (themeName == "Bright") {
		targetDiffuse[0] = Vec4(1.5f, 1.5f, 1.3f, 1.0f); 
		targetSpecular[0] = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		targetAmbient[0] = Vec4(0.4f, 0.4f, 0.4f, 1.0f);
		targetPos[0] = Vec3(0.0f, 20.0f, 0.0f); 
		targetDiffuse[1] = Vec4(0.5f, 0.5f, 0.5f, 1.0f); targetPos[1] = Vec3(10, 10, 10);
		targetDiffuse[2] = Vec4(0.5f, 0.5f, 0.5f, 1.0f); targetPos[2] = Vec3(-10, 10, 10);
	}
	else if (themeName == "Sunset") {
		targetDiffuse[0] = Vec4(1.0f, 0.5f, 0.2f, 1.0f); 
		targetSpecular[0] = Vec4(1.0f, 0.8f, 0.4f, 1.0f);
		targetAmbient[0] = Vec4(0.1f, 0.05f, 0.05f, 1.0f);
		targetPos[0] = Vec3(15.0f, 2.0f, 0.0f); 
		for(int i=1; i<5; i++) targetDiffuse[i] = Vec4(0.1f, 0.02f, 0.0f, 1.0f);
	}
}

void Scene0g::SphereCollisions()
{
	for (auto itA = actors.begin(); itA != actors.end(); ++itA) {
		Actor* actorA = itA->second.actor.get();
        
		auto collisionsA = actorA->GetComponent<CollisionComponent>();
		auto physA = actorA->GetComponent<PhysicsComponent>();
		auto transformA = actorA->GetComponent<TransformComponent>();
		if (!collisionsA || !physA || !transformA) continue;
		collisionsA->SetSphere(transformA->GetPosition(), collisionsA->GetSphere().r);
		
		for (auto itB = std::next(itA); itB != actors.end(); ++itB) {
			Actor* actorB = itB->second.actor.get();
			auto collisionsB = actorB->GetComponent<CollisionComponent>();
			auto physB = actorB->GetComponent<PhysicsComponent>();
			auto transformB = actorB->GetComponent<TransformComponent>();
			if (!collisionsB || !physB || !transformB) continue;
			
			collisionsB->SetSphere(transformB->GetPosition(), collisionsB->GetSphere().r);
			if (collisionSystem->CollisionDetection(collisionsA->GetSphere(), collisionsB->GetSphere())) {
				collisionSystem->SphereSphereCollisionResponse(
					collisionsA->GetSphere(), physA, 
					collisionsB->GetSphere(), physB
				);
			}
		}
	}
}

void Scene0g::AABBCollisions()
{
	for (auto itA = actors.begin(); itA != actors.end(); ++itA) {
		Actor* actorA = itA->second.actor.get();
		auto colA = actorA->GetComponent<CollisionComponent>();
		auto physA = actorA->GetComponent<PhysicsComponent>();

		if (!colA || !physA) continue;

		for (auto itB = std::next(itA); itB != actors.end(); ++itB) {
			if (itB->second.isReturning) continue;
			Actor* actorB = itB->second.actor.get();
			auto colB = actorB->GetComponent<CollisionComponent>();
			auto physB = actorB->GetComponent<PhysicsComponent>();

			if (!colB || !physB) continue;

			if (collisionSystem->CollisionDetection(colA->GetAABB(), colB->GetAABB())) {
				collisionSystem->AABBCollisionResponse(colA, physA, colB, physB);
			}
		}
	}
}

bool Scene0g::IsSquareOccupied(int targetCol, int targetRow, Actor* movingPiece) const
{
	for (const auto& pair : actors) {
		Actor* otherPiece = pair.second.actor.get();
		if (otherPiece == movingPiece) continue; 
		if (pair.second.lastValidCol == targetCol && pair.second.lastValidRow == targetRow) {
			return true; 
		}
	}
	return false;
}

void Scene0g::HandleMouseClick(int mouseX, int mouseY)
{	
	Vec3 boardHitPos = GetBoardIntersect(mouseX, mouseY);
	
	int clickedCol = static_cast<int>(std::round((boardHitPos.x - gridOriginX) / dynamicGridSize));
	int clickedRow = static_cast<int>(std::round((gridOriginY - boardHitPos.y) / dynamicGridSize));
	std::cout << "4. Grid Calc     : Col: " << clickedCol << " | Row: " << clickedRow << std::endl;

	if (clickedCol < 0 || clickedCol > 7 || clickedRow < 0 || clickedRow > 7) {
		std::cout << "-> OUT OF BOUNDS! Resetting selection." << std::endl;
		currentState = MouseState::SELECTING_PIECE;
		selectedActorName = ""; 
		return;
	}
	
	if (clickedCol < 0 || clickedCol > 7 || clickedRow < 0 || clickedRow > 7) {
		currentState = MouseState::SELECTING_PIECE;
		selectedActorName = ""; 
		return;
	}
	
	if (currentState == MouseState::SELECTING_PIECE) {
		std::cout << "5. State: SELECTING PIECE at [" << clickedCol << ", " << clickedRow << "]" << std::endl;
		for (const auto& pair : actors) {
			if (pair.second.lastValidCol == clickedCol && pair.second.lastValidRow == clickedRow) {
				std::cout << "-> SUCCESS: Selected " << pair.first << "!" << std::endl;
				selectedActorName = pair.first;
				currentState = MouseState::SELECTING_DESTINATION;
				return; 
			}
		}
		std::cout << "-> FAILED: No piece found at this square." << std::endl;
	}
	
	else if (currentState == MouseState::SELECTING_DESTINATION) {
		std::cout << "5. State: MOVING PIECE " << selectedActorName << " TO [" << clickedCol << ", " << clickedRow << "]" << std::endl;
		auto it = actors.find(selectedActorName);
		if (it != actors.end()) {
			it->second.lastValidCol = clickedCol;
			it->second.lastValidRow = clickedRow;
			it->second.isReturning = true; 
		}

		selectedActorName = "";
		currentState = MouseState::SELECTING_PIECE;
	}
    
	
}

Vec3 Scene0g::GetBoardIntersect(int mouseX, int mouseY)
{
	int screenWidth, screenHeight;
	SDL_GetWindowSize(window->getWindow(), &screenWidth, &screenHeight);

	// Using R(t) = O+Dt
	
	// Viewport Space to NDS
	float x = (2.0f * mouseX) / screenWidth - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / screenHeight;
	float z = 1.0f;
	Vec3 rayNDS = Vec3(x, y, z);

	// NDS to HCS
	Vec4 rayClip = Vec4(rayNDS.x, rayNDS.y, -1.0f, 1.0f);

	// HCS to Camera
	Matrix4 inverseProj = MMath::inverse(camera->GetProjectionMatrix());
	Vec4 rayCam = inverseProj * rayClip;
	rayCam = Vec4(rayCam.x, rayCam.y, -1.0f, 0.0f);

	// Camera to World Space
	Matrix4 inverseView = MMath::inverse(camera->GetViewMatrix());
	Vec4 rayWorld = inverseView * rayCam;

	// World Space Direction
	Vec3 rayDir = VMath::normalize(Vec3(rayWorld.x,rayWorld.y,rayWorld.z));
	Vec3 camPos = camera->GetComponent<TransformComponent>()->GetPosition();
    
	if (std::abs(rayDir.z) < 0.0001f) {
		return Vec3(0.0f, 0.0f, 0.0f); 
	}
	
	// Distance from origin
	float t = -camPos.z / rayDir.z;
	Vec3 intersectionPoint = camPos + (rayDir * t);
	std::cout << "--- RAYCAST DEBUG ---" << std::endl;
	std::cout << "1. Screen Pixels : X: " << mouseX << " | Y: " << mouseY << std::endl;
	std::cout << "2. Ray Direction : X: " << rayDir.x << " | Y: " << rayDir.y << " | Z: " << rayDir.z << std::endl;
	std::cout << "3. 3D Board Hit  : X: " << intersectionPoint.x << " | Y: " << intersectionPoint.y << " | Z: " << intersectionPoint.z << std::endl;
	return intersectionPoint;
}


void Scene0g::SnapToGrid() 
{
	const float velocityThreshold = 1.0f;
    const float snapRadius = 0.05f;
	const float pullRadius = 0.9f;
	const bool* keys = SDL_GetKeyboardState(nullptr);
	bool isPlayerPressingKeys =
		(keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT] || 
		keys[SDL_SCANCODE_UP]   || keys[SDL_SCANCODE_DOWN] ||
		keys[SDL_SCANCODE_SPACE]|| keys[SDL_SCANCODE_LCTRL]);

	
    for (auto& pair : actors)
    {
	    Actor* piece = pair.second.actor.get();
    	auto phys = piece->GetComponent<PhysicsComponent>();
    	auto transform = piece->GetComponent<TransformComponent>();
    	
    	if (pair.first == selectedActorName && isPlayerPressingKeys) {
    		pair.second.isReturning = false;
    		continue;
    	}
    		Vec3 currentPos = transform->GetPosition();
    		Vec3 currentVel = phys->GetVelocity();
    		float currentSpeedSqr = magSqr(currentVel); 
    		float thresholdSqr = velocityThreshold * velocityThreshold;

    		if (currentSpeedSqr > 0.0001f && currentSpeedSqr < thresholdSqr || pair.second.isReturning) {
    			float minX = gridOriginX - (dynamicGridSize / 2.0f);
    			float minY = gridOriginY - (7 * dynamicGridSize) - (dynamicGridSize / 2.0f);
    			float maxX = gridOriginX + (7 * dynamicGridSize) + (dynamicGridSize / 2.0f);
    			float maxY = gridOriginY + (dynamicGridSize / 2.0f); 

        
    			if (!pair.second.isReturning && (currentPos.x < minX || currentPos.x > maxX || 
					currentPos.y < minY || currentPos.y > maxY)) {
    				continue; 
					}

    			int targetCol = std::round((currentPos.x - gridOriginX) / dynamicGridSize);
    			int targetRow = std::round((gridOriginY - currentPos.y) / dynamicGridSize);
        	
    			targetCol = std::max(0, std::min(targetCol, 7));
    			targetRow = std::max(0, std::min(targetRow, 7));
    			if (pair.second.isReturning) {
    				targetCol = pair.second.lastValidCol;
    				targetRow = pair.second.lastValidRow;
    			}
    			else
    			{
    				if (targetCol < 0 || targetCol > 7 || targetRow < 0 || targetRow > 7) {
    					continue; 
    				}
        	
        	
    				if (IsSquareOccupied(targetCol,targetRow, piece))
    				{
    					targetCol = pair.second.lastValidCol;
    					targetRow = pair.second.lastValidRow;
    					pair.second.isReturning = true;
    				}
    			}
        	
    			float targetX = gridOriginX + (targetCol * dynamicGridSize);
    			float targetY = gridOriginY - (targetRow * dynamicGridSize);

    			float dist = VMath::distance(Vec3(currentPos.x, currentPos.y, 0.0f), Vec3(targetX, targetY, 0.0f));
        	
    			if (dist < pullRadius || pair.second.isReturning )
    			{
    				if (dist <= snapRadius) {
    					transform->SetPosition(Vec3(targetX, targetY, currentPos.z));
    					phys->SetVelocity(Vec3(0.0f, 0.0f, 0.0f));
    					phys->SetAcceleration(Vec3(0.0f, 0.0f, 0.0f));

    					pair.second.lastValidCol = targetCol;
    					pair.second.lastValidRow = targetRow;
    					pair.second.isReturning = false;
    				} 
    				else {
    					Vec3 pullVector = Vec3(targetX, targetY, 0.0f) - Vec3(currentPos.x, currentPos.y, 0.0f);
    					float pullStrength = pair.second.isReturning ? 1.0f : 1.1f; 
    					phys->SetVelocity(pullVector * pullStrength);
    				}
    			}
    		}
    	}
    }



void Scene0g::Update(const float deltaTime) {
	camera->Update(deltaTime);
	
	
	float time = SDL_GetTicks() / 1000.0f; // Seconds since start
	float radius = 10.0f;
	float x = cos(time * 0.5f) * radius;
	float z = sin(time * 0.5f) * radius;
	lights[0]->GetComponent<TransformComponent>()->SetPosition(Vec3(x, 10.0f, z));

	// Slow transition for the lights 
	if (isTransitioning) {
		transitionAlpha += deltaTime * transitionSpeed;
		transitionAlpha = std::min(transitionAlpha, 1.0f);

		for (int i = 0; i < 5; i++) {
			Vec4 nextDiff = startDiffuse[i] + (targetDiffuse[i] - startDiffuse[i]) * transitionAlpha;
			Vec4 nextSpec = startSpecular[i] + (targetSpecular[i] - startSpecular[i]) * transitionAlpha;
			Vec4 nextAmb  = startAmbient[i]  + (targetAmbient[i]  - startAmbient[i])  * transitionAlpha;
			Vec3 nextPos  = startPos[i]      + (targetPos[i]      - startPos[i])      * transitionAlpha;

			lights[i]->SetDiffuse(nextDiff);
			lights[i]->SetSpecular(nextSpec);
			lights[i]->SetAmbient(nextAmb);
			lights[i]->GetComponent<TransformComponent>()->SetPosition(nextPos);
		}
		if (transitionAlpha >= 1.0f) isTransitioning = false;
	}
	
	//Board Rotation
	if (canBoardSpin)
	{
		float rotationSpeed = 10.0f;
		float frameRotation = rotationSpeed * deltaTime;
		Quaternion rota = QMath::angleAxisRotation(frameRotation, Vec3(0.0f, 1.0f, 0.0f));

		Ref<TransformComponent> boardTransform = board->GetComponent<TransformComponent>();
		if (boardTransform) {
			boardTransform->SetOrientation(boardTransform->GetOrientation() *= rota);
		}
	}


	for (auto& pair : actors) {
		pair.second.actor->Update(deltaTime);
	}

	//Activate collision detection
	AABBCollisions();
	SnapToGrid();
	//SphereCollisions();
	if (!selectedActorName.empty()) {
		auto it = actors.find(selectedActorName);
		if (it != actors.end()) {
			auto phys = it->second.actor->GetComponent<PhysicsComponent>();
			const bool* keys = SDL_GetKeyboardState(nullptr);
			Vec3 forceDir(0.0f, 0.0f, 0.0f);


			if (keys[SDL_SCANCODE_LEFT])  forceDir.x -= 1.0f;
			if (keys[SDL_SCANCODE_RIGHT]) forceDir.x += 1.0f;
			if (keys[SDL_SCANCODE_UP])    forceDir.y += 1.0f;
			if (keys[SDL_SCANCODE_DOWN])  forceDir.y -= 1.0f;
			if (keys[SDL_SCANCODE_SPACE]) forceDir.z += 1.0f;
			if (keys[SDL_SCANCODE_LCTRL]) forceDir.z -= 1.0f;
			
			if (VMath::mag(forceDir) > 0.001f) {
				forceDir = VMath::normalize(forceDir);
				float strength = 40.0f; 
				if (phys) phys->ApplyForce(forceDir * strength);
			}
		}
	}
	
}

void Scene0g::Render() const {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


	// Turn on my skybox
	camera->RenderSkybox();
    glUseProgram(shader->GetProgram());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("projectionMatrix")), 1, GL_FALSE, camera->GetProjectionMatrix());
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("viewMatrix")), 1, GL_FALSE, camera->GetViewMatrix());



	/*							<-LIGHTS SETUP->								*/
	float allPos[15];     // 5 lights * 3 floats 
	float allDiff[20];    // 5 lights * 4 floats 
	float allSpec[20];    // 5 lights * 4 floats 
	float allAmb[20];     // 5 lights * 4 floats 

	for (int i = 0; i < 5; i++) {
		Vec3 worldPos = lights[i]->GetComponent<TransformComponent>()->GetPosition();
		Vec4 viewPos = camera->GetViewMatrix() * Vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

		allPos[i * 3 + 0] = viewPos.x;
		allPos[i * 3 + 1] = viewPos.y;
		allPos[i * 3 + 2] = viewPos.z;

		Vec4 diffuse = lights[i]->GetDiffuse();
		allDiff[i * 4 + 0] = diffuse.x; allDiff[i * 4 + 1] = diffuse.y; allDiff[i * 4 + 2] = diffuse.z; allDiff[i * 4 + 3] = diffuse.w;

		Vec4 specular = lights[i]->GetSpecular();
		allSpec[i * 4 + 0] = specular.x; allSpec[i * 4 + 1] = specular.y; allSpec[i * 4 + 2] = specular.z; allSpec[i * 4 + 3] = specular.w;

		Vec4 ambient = lights[i]->GetAmbient();
		allAmb[i * 4 + 0] = ambient.x; allAmb[i * 4 + 1] = ambient.y; allAmb[i * 4 + 2] = ambient.z; allAmb[i * 4 + 3] = ambient.w;
	}

	glUniform3fv(shader->GetUniformID("lightPos[0]"), 5, allPos);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"),  5, allDiff);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, allSpec);
	glUniform4fv(shader->GetUniformID("Ambient[0]"),  5, allAmb);

	
	/*								<-BOARD AND PIECE RENDERER->															*/
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1, GL_FALSE, board->GetComponent<TransformComponent>()->GetTransformMatrix());
    glBindTexture(GL_TEXTURE_2D, board->GetComponent<MaterialComponent>()->getTextureID());
    board->GetComponent<MeshComponent>()->Render();

    for (const auto& pair : actors) {
    	const std::string& name = pair.first;
    	Actor* piece = pair.second.actor.get();

    	float intensity = (name == selectedActorName) ? 4.5f : 1.0f;
    	glUniform1f(shader->GetUniformID("highlightIntensity"), intensity);
    	
       glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, piece->GetModelMatrix());
       glBindTexture(GL_TEXTURE_2D, piece->GetComponent<MaterialComponent>()->getTextureID());
       piece->GetComponent<MeshComponent>()->Render();
    }

	if (showHitboxes)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);               
		glBindTexture(GL_TEXTURE_2D, 0);        
		Matrix4 currentBoardMatrix = board->GetComponent<TransformComponent>()->GetTransformMatrix();

		for (const auto& pair : actors) {
			const std::string& name = pair.first;
			Actor* piece = pair.second.actor.get();
			auto col = piece->GetComponent<CollisionComponent>();
			if (!col) continue;

	
			if (name == selectedActorName) {
				glUniform4f(shader->GetUniformID("debugColor"), 1.0f, 0.41f, 0.70f, 1.0f); 
			} else {
				glUniform4f(shader->GetUniformID("debugColor"), 0.5f, 0.0f, 0.8f, 1.0f); 
			}
			
			if (col->GetColliderType() == ColliderType::SPHERE) {
				Sphere s = col->GetSphere();
				Matrix4 debugModel = currentBoardMatrix * MMath::translate(s.center) * MMath::scale(s.r, s.r, s.r);
				glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, debugModel);
				debugSphere->Render();
			}
			
			else if (col->GetColliderType() == ColliderType::AABB) {
				AABB box = col->GetAABB();
				Matrix4 debugModel = currentBoardMatrix * MMath::translate(box.center) * MMath::scale(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
				glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, debugModel);
				debugCube->Render();
			}
		}
			glUniform1f(shader->GetUniformID("highlightIntensity"), 1.0f);
		glUniform4f(shader->GetUniformID("debugColor"), 0.0f, 0.0f, 0.0f, 0.0f);		
	}
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
	glEnable(GL_CULL_FACE);
	
	glUniform1f(shader->GetUniformID("highlightIntensity"), 1.0f);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
