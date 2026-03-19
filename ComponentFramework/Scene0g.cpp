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
#include <map>
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

	/*						<-SHADER AND CAMERA SETUP->										*/
	shader = std::make_shared<ShaderComponent>(std::weak_ptr<Component>(),"shaders/texturePhongVert.glsl","shaders/texturePhongFrag.glsl");
	shader->OnCreate();
	window = new Window();
	camera = std::make_shared<CameraActor>(std::weak_ptr<Actor>(), 45.0f, 16.0f / 9.0f, 0.1f, 100.0f, window->getWindow());
	camera->AddComponent<TransformComponent>(std::weak_ptr<Component>(), Vec3(0.0f, 0.0f, -15.0f), Quaternion());
	camera->OnCreate();
	camera->setCamMovement(true);

	
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
		
		if (color == "Black")
		{
			actorNew->AddComponent<MaterialComponent>(blackChessPieces);
			finalPos = Vec3(-4.4f + (actorOffset * col), 4.4f - (actorOffset * row), 0.1f);
		}
		
		if (color == "White")
		{
			actorNew->AddComponent<MaterialComponent>(whiteChessPieces);
			finalPos = Vec3(-4.4f + (actorOffset * col), -6.85f + (actorOffset * row), 0.1f);
		}
		
		actorNew->AddComponent<TransformComponent>(std::weak_ptr<Component>(), finalPos, Quaternion(), Vec3(0.125f, 0.125f, 0.125f));
		Quaternion rot = QMath::angleAxisRotation(90.0f, Vec3(1.0f, 0.0f, 0.0f));
		actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= rot);
		
		actorNew->OnCreate();
		if (type == "Knight" && color == "White") {
			Quaternion extraRot = QMath::angleAxisRotation(180, Vec3(0.0f, 0.0f, 1.0f));
			actorNew->GetComponent<TransformComponent>()->SetOrientation(actorNew->GetComponent<TransformComponent>()->GetOrientation() *= extraRot);
		}
		
		actors.emplace(actorName, ActorData{std::move(actorNew), typeOfActor, color});

	}
	return true;
}

void Scene0g::OnDestroy() {
	actors.clear();
	//lights.clear();

	
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
void RenderActorRow(const std::string& name, const ActorData& data) {
	// 1. Give this specific row a unique ID based on the piece name
	ImGui::PushID(name.c_str()); 

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - 100.0f);

	ImVec4 textColor = (data.colour == "White") ? ImVec4(1, 1, 1, 1) : ImVec4(0.5f, 0.5f, 0.5f, 1);
	ImGui::TextColored(textColor, "%s", name.c_str());
    
	ImGui::NextColumn();
    
	// 2. Now this button will actually trigger because its ID is unique
	if (ImGui::SmallButton("Edit")) {
		Debug::Info("Button clicked for: " + name, __FILE__, __LINE__);
		// Place selection logic here
	}
    
	ImGui::Columns(1);
    
	// 3. Pop the ID to clean up for the next piece
	ImGui::PopID();
}

void Scene0g::RenderGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 450), ImGuiCond_Always);
	ImGui::Begin("Lighting Inspector");
    
    if (ImGui::CollapsingHeader("Global Themes")) {
        if (ImGui::Button("Bright"))   SetupMood("Bright");
        ImGui::SameLine();
        if (ImGui::Button("Midnight")) SetupMood("Midnight");
        ImGui::SameLine();
        if (ImGui::Button("Sunset"))   SetupMood("Sunset");
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

    // --- ACTOR OUTLINER SECTION ---
	ImGui::SetNextWindowPos(ImVec2(window->getWidth() - 320.0f, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(310, 450), ImGuiCond_Always);
	ImGui::Begin("Scene Outliner");
	if (ImGui::BeginTabBar("Tabs")) {
		if (ImGui::BeginTabItem("Chess Pieces")) {
          
			if (ImGui::TreeNodeEx("White Team", ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto const& pair : actors) {
					if (pair.second.colour == "White") RenderActorRow(pair.first, pair.second);
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
          
			if (ImGui::TreeNodeEx("Black Team", ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto const& pair : actors) {
					if (pair.second.colour == "Black") RenderActorRow(pair.first, pair.second);
				}
				ImGui::TreePop();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void Scene0g::SetupMood(std::string moodName)
{
	isTransitioning = true;
	transitionAlpha = 0.0f;

	// 1. Capture current state as the starting point
	for (int i = 0; i < 5; i++) {
		startDiffuse[i] = lights[i]->GetDiffuse();
		startSpecular[i] = lights[i]->GetSpecular();
		startAmbient[i] = lights[i]->GetAmbient();
		startPos[i] = lights[i]->GetComponent<TransformComponent>()->GetPosition();
	}

	// 2. Define the Target values based on the name
	if (moodName == "Midnight") {
		targetDiffuse[0] = Vec4(0.2f, 0.2f, 0.5f, 1.0f);
		targetSpecular[0] = Vec4(0.9f, 0.9f, 1.0f, 1.0f);
		targetAmbient[0] = Vec4(0.02f, 0.02f, 0.05f, 1.0f);
		targetPos[0] = Vec3(0.0f, 15.0f, -10.0f);
		for(int i=1; i<5; i++) targetDiffuse[i] = Vec4(0.01f, 0.01f, 0.03f, 1.0f);
	} 
	else if (moodName == "Bright") {
		// Over-the-top stadium lighting
		targetDiffuse[0] = Vec4(1.5f, 1.5f, 1.3f, 1.0f); 
		targetSpecular[0] = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		targetAmbient[0] = Vec4(0.4f, 0.4f, 0.4f, 1.0f);
		targetPos[0] = Vec3(0.0f, 20.0f, 0.0f); // Directly overhead
		// Fill from corners
		targetDiffuse[1] = Vec4(0.5f, 0.5f, 0.5f, 1.0f); targetPos[1] = Vec3(10, 10, 10);
		targetDiffuse[2] = Vec4(0.5f, 0.5f, 0.5f, 1.0f); targetPos[2] = Vec3(-10, 10, 10);
	} 
	else if (moodName == "Sunset") {
		// Warm oranges and long shadows
		targetDiffuse[0] = Vec4(1.0f, 0.5f, 0.2f, 1.0f); // Orange Sun
		targetSpecular[0] = Vec4(1.0f, 0.8f, 0.4f, 1.0f);
		targetAmbient[0] = Vec4(0.1f, 0.05f, 0.05f, 1.0f);
		targetPos[0] = Vec3(15.0f, 2.0f, 0.0f); // Low angle for long shadows
		for(int i=1; i<5; i++) targetDiffuse[i] = Vec4(0.1f, 0.02f, 0.0f, 1.0f);
	}
}


void Scene0g::Update(const float deltaTime) {
	camera->Update(deltaTime);
	
	
	float time = SDL_GetTicks() / 1000.0f; // Seconds since start
	float radius = 15.0f;
	float x = cos(time * 0.5f) * radius;
	float z = sin(time * 0.5f) * radius;
	//lights[0]->GetComponent<TransformComponent>()->setPosition(Vec3(x, 10.0f, z));
	
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
	for (int i = 0; i < 5; i++) {
		// Get view space position
		Vec3 worldPos = lights[i]->GetComponent<TransformComponent>()->GetPosition();
		Vec4 viewPos = camera->GetViewMatrix() * Vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
		Vec3 lightPosVS = Vec3(viewPos.x, viewPos.y, viewPos.z);

		// Explicitly check for the ID before uploading
		std::string name = "lightPos[" + std::to_string(i) + "]";
		GLint loc = shader->GetUniformID(name);
    
		if (loc != -1) {
			glUniform3fv(loc, 1, &lightPosVS[0]);
			glUniform4fv(shader->GetUniformID("Diffuse[" + std::to_string(i) + "]"), 1, &lights[i]->GetDiffuse()[0]);
			glUniform4fv(shader->GetUniformID("Specular[" + std::to_string(i) + "]"), 1, &lights[i]->GetSpecular()[0]);
			glUniform4fv(shader->GetUniformID("Ambient[" + std::to_string(i) + "]"), 1, &lights[i]->GetAmbient()[0]);
		} else if (i == 1) {
			// This will tell us if it's still missing even with unique_ptrs
			static bool warned = false;
			if(!warned) { std::cout << "Warning: lightPos[1] still not found!" << std::endl; warned = true; }
		}
	}
	
    glUniformMatrix4fv(static_cast<GLint>(shader->GetUniformID("modelMatrix")), 1, GL_FALSE, board->GetComponent<TransformComponent>()->GetTransformMatrix());
    glBindTexture(GL_TEXTURE_2D, board->GetComponent<MaterialComponent>()->getTextureID());
    board->GetComponent<MeshComponent>()->Render();

    for (const auto& pair : actors) {
       Actor* piece = pair.second.actor.get();
       glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, piece->GetModelMatrix());
       glBindTexture(GL_TEXTURE_2D, piece->GetComponent<MaterialComponent>()->getTextureID());
       piece->GetComponent<MeshComponent>()->Render();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
