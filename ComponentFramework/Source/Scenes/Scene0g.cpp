#include <glew.h>
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
drawInWireMode{false},
window{ nullptr }, 
context{ nullptr }
{
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0g::~Scene0g() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}


/*
 HELLO SCOTT, SORRY FOR THE LATE SUBMISSION I WAS GETTING MY SHIT TOGETHER
 As stated previously here is a input map so you don't get lost.
 
 
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

	/*						<-SHADER AND CAMERA SETUP->										*/
	shader = std::make_shared<ShaderComponent>(std::weak_ptr<Component>(),"shaders/texturePhongVert.glsl","shaders/texturePhongFrag.glsl");
	shader->OnCreate();
	window = new Window();
	camera = std::make_shared<CameraActor>(std::weak_ptr<Actor>(), 45.0f, 16.0f / 9.0f, 0.1f, 100.0f, window->getWindow());
	camera->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 0.0f, -15.0f), Quaternion());
	camera->OnCreate();
	camera->setCamMovement(true);
	camera->SkyboxSetup(
			"textures/spacePX.png",	
			"textures/spacePY.png",
			"textures/spacePZ.png",
			"textures/spaceNX.png",
			"textures/spaceNY.png",
			"textures/spaceNZ.png"
	);

	
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

	
	/*						<-BOARD ACTOR SETUP->										*/
	board = std::make_shared<Actor>(std::weak_ptr<Component>());
	board->AddComponent<MaterialComponent>(std::weak_ptr<Component>(), "textures/8x8_board_red.png");
	board->AddComponent<MeshComponent>(std::weak_ptr<Component>(), "meshes/Plane.obj");
	board->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 0.0f, 0.0f), QMath::angleAxisRotation(-90.0f, Vec3(1.0f, 0.0f, 0.0f)));
	board->GetComponent<TransformComponent>()->SetScale(Vec3(5.0f, 5.0f, 5.0f));
	board->OnCreate();

	/*						<-SETUP FOR SHARED MESHES->										*/
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

	
	/*						<-SETUP FOR SHARED TEXTURES AND PIECE TYPES->					*/
	Ref<MaterialComponent>blackChessPieces = std::make_shared<MaterialComponent>(std::weak_ptr<Component>(), "textures/BlackChessPiece.png");
	blackChessPieces->OnCreate();
	Ref<MaterialComponent>whiteChessPieces = std::make_shared<MaterialComponent>(std::weak_ptr<Component>(), "textures/WhiteChessPiece.png");
	whiteChessPieces->OnCreate();
	std::string pieceTypes[] = {
	"Rook", "Knight", "Bishop", "Queen", "King", "Bishop", "Knight", "Rook"
	};

	
	// This is a map used to make naming my pieces easier.
	std::map<std::string, int> pieceCounters;
	
	PieceType typeOfActor;
	for (int i = 0; i < 32; i++) {
		// Tools to index the Unordered Map to more easily index and find things.
		int pieceIndex = i % 16;
		int row = i / 8; 
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
		
		// Based off of which actor it is give them the corresponding mesh.
		switch (typeOfActor)
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
		Vec3 finalPos;

		// Based off of which type of piece it is give it the corresponding texture and update it's position
		if (color == "Black")
		{
			actorNew->AddComponent<MaterialComponent>(blackChessPieces);
			finalPos = Vec3(-4.4f + (actorOffset * col), 4.4f - (actorOffset * row), 0.0f);
		}
		
		if (color == "White")
		{
			actorNew->AddComponent<MaterialComponent>(whiteChessPieces);
			finalPos = Vec3(-4.4f + (actorOffset * col), -6.85f + (actorOffset * row), 0.0f);
		}
		actorNew->AddComponent<TransformComponent>(std::weak_ptr<Component>(), finalPos, Quaternion(), Vec3(0.125f, 0.125f, 0.125f));
		Quaternion rot = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
		actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= rot);

		// Rotate the white knights as they face the opposite direction by default
		actorNew->OnCreate();
		if (type == "Knight" && color == "White") {
			Quaternion extraRot = QMath::angleAxisRotation(180, Vec3(0.0f, 0.0f, 1.0f));
			actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= extraRot);
		}
		// Add the actors into the list assigning the name, move in the pointer, the type of actor it is and the colour of the actor.
		actors.emplace(actorName, ActorData{std::move(actorNew), typeOfActor, color});
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
				lights[i]->GetComponent<TransformComponent>()->setPosition(pos);
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
	float desiredWidth = 200.0f;  
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
	ImGui::End();
}
//					<-PRESET LIGHTING THEMES->					//
void Scene0g::SetupTheme(std::string themeName)
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
void Scene0g::Update(const float deltaTime) {
	camera->Update(deltaTime);
	
	
	float time = SDL_GetTicks() / 1000.0f; // Seconds since start
	float radius = 50.0f;
	float x = cos(time * 0.5f) * radius;
	float z = sin(time * 0.5f) * radius;
	lights[0]->GetComponent<TransformComponent>()->setPosition(Vec3(x, 10.0f, z));

	// Slow transition for the lights to make it look more cinematic
	if (isTransitioning) {
		transitionAlpha += deltaTime * transitionSpeed;
		if (transitionAlpha > 1.0f) transitionAlpha = 1.0f;

		for (int i = 0; i < 5; i++) {
			Vec4 nextDiff = startDiffuse[i] + (targetDiffuse[i] - startDiffuse[i]) * transitionAlpha;
			Vec4 nextSpec = startSpecular[i] + (targetSpecular[i] - startSpecular[i]) * transitionAlpha;
			Vec4 nextAmb  = startAmbient[i]  + (targetAmbient[i]  - startAmbient[i])  * transitionAlpha;
			Vec3 nextPos  = startPos[i]      + (targetPos[i]      - startPos[i])      * transitionAlpha;

			lights[i]->SetDiffuse(nextDiff);
			lights[i]->SetSpecular(nextSpec);
			lights[i]->SetAmbient(nextAmb);
			lights[i]->GetComponent<TransformComponent>()->setPosition(nextPos);
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
	float allPos[15];     // 5 lights * 3 floats (vec3)
	float allDiff[20];    // 5 lights * 4 floats (vec4)
	float allSpec[20];    // 5 lights * 4 floats (vec4)
	float allAmb[20];     // 5 lights * 4 floats (vec4)

	for (int i = 0; i < 5; i++) {
		Vec3 worldPos = lights[i]->GetComponent<TransformComponent>()->GetPosition();
		Vec4 viewPos = camera->GetViewMatrix() * Vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

		allPos[i * 3 + 0] = viewPos.x;
		allPos[i * 3 + 1] = viewPos.y;
		allPos[i * 3 + 2] = viewPos.z;

		Vec4 d = lights[i]->GetDiffuse();
		allDiff[i * 4 + 0] = d.x; allDiff[i * 4 + 1] = d.y; allDiff[i * 4 + 2] = d.z; allDiff[i * 4 + 3] = d.w;

		Vec4 s = lights[i]->GetSpecular();
		allSpec[i * 4 + 0] = s.x; allSpec[i * 4 + 1] = s.y; allSpec[i * 4 + 2] = s.z; allSpec[i * 4 + 3] = s.w;

		Vec4 a = lights[i]->GetAmbient();
		allAmb[i * 4 + 0] = a.x; allAmb[i * 4 + 1] = a.y; allAmb[i * 4 + 2] = a.z; allAmb[i * 4 + 3] = a.w;
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

    	float intensity = (name == selectedActorName) ? 2.5f : 1.0f;
    	glUniform1f(shader->GetUniformID("highlightIntensity"), intensity);
    	
       glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, piece->GetModelMatrix());
       glBindTexture(GL_TEXTURE_2D, piece->GetComponent<MaterialComponent>()->getTextureID());
       piece->GetComponent<MeshComponent>()->Render();
    }
	glUniform1f(shader->GetUniformID("highlightIntensity"), 1.0f);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}


