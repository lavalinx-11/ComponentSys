#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <unordered_map>
#include "Actors/Actor.h"
#include "Actors/CameraActor.h"
#include "Actors/LightActor.h"
#include "Engine/Window.h"
#include "Components/ShaderComponent.h"
#include "Components/MeshComponent.h"
#include "Components/CollSystem.h"
#include "Actors/AssetManager.h"

/// Forward declarations 
union SDL_Event;


enum PieceType
{
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING
};


enum class MouseState {
	SELECTING_PIECE,
	SELECTING_DESTINATION
};



struct ActorData
{
	std::unique_ptr<Actor> actor;
	PieceType actorType;
	std::string colour;
	int lastValidCol = 0; 
	int lastValidRow = 0;
	int homeCol;
	int homeRow;
	bool isReturning = false;
};

class Scene0g : public Scene {
private:
	// Shared Pointers
	Ref<ShaderComponent> shader;
	Ref<Actor> board;
	Ref<CameraActor> camera;
	Ref<MeshComponent> debugSphere;
	Ref<MeshComponent> debugCube;
	MouseState currentState;

	std::unordered_map<std::string, ActorData> actors; 
	std::string actorName; 
	std::string actorColour;
	std::string actorType;
	std::string selectedActorName;
	std::vector<std::unique_ptr<Actor>> knightPieces;
	std::unique_ptr<LightActor> lights[5];
	std::unique_ptr<CollisionSystem> collisionSystem;

	// Vectors 
	Vec4 startDiffuse[5], startSpecular[5], startAmbient[5];
	Vec3 startPos[5];
	Vec4 targetDiffuse[5], targetSpecular[5], targetAmbient[5];
	Vec3 targetPos[5];
	Vec3 debugRayOrigin;
	Vec3 debugRayDir;
	Vec3 debugHitPos;
	
	
	
	// Default Variables
	bool showDebugRay = false;
	bool showHitboxes = true;
	bool canBoardSpin = false;
	bool isTransitioning = false;
	bool drawInWireMode;
	float transitionAlpha = 0.0f; // 0.0 to 1.0
	float transitionSpeed = 0.5f; // 1.0 = 1 second, 0.5 = 2 seconds
	float dynamicGridSize;
	float gridOriginX;
	float gridOriginY;
	
	
	// SDL Stuff
	Window* window;
	SDL_GLContext context;
public:
	explicit Scene0g();
	~Scene0g() override;

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
	virtual void RenderGUI() override;
	void SetupTheme(const std::string& themeName);
	void SphereCollisions();
	void AABBCollisions();
	void SnapToGrid();
	bool IsSquareOccupied(int targetCol, int targetRow, Actor* movingPiece) const;
	void HandleMouseClick(int mouseX, int mouseY);
	Vec3 GetBoardIntersect(int mouseX, int mouseY);
};


#endif // SCENE0_H