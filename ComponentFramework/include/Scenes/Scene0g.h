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
struct ActorData
{
	std::unique_ptr<Actor> actor;
	PieceType actorType;
	std::string colour;
};

class Scene0g : public Scene {
private:
	// Shared Pointers
	Ref<ShaderComponent> shader;
	Ref<Actor> board;
	Ref<CameraActor> camera;
	Ref<MeshComponent> debugSphere;
	Ref<MeshComponent> debugCube;

	
	std::unordered_map<std::string, ActorData> actors; 
	std::string actorName; 
	std::string actorColour;
	std::string actorType;
	std::string selectedActorName;
	std::vector<std::unique_ptr<Actor>> knightPieces;
	std::unique_ptr<LightActor> lights[5];
	std::unique_ptr<CollisionSystem> collisionSystem;
	std::unique_ptr<AssetManager> assetManager;

	
	// Default Variables
	bool showHitboxes = true;
	bool canBoardSpin = false;
	bool isTransitioning = false;
	bool drawInWireMode;
	float transitionAlpha = 0.0f; // 0.0 to 1.0
	float transitionSpeed = 0.5f; // 1.0 = 1 second, 0.5 = 2 seconds

	// Vectors 
	Vec4 startDiffuse[5], startSpecular[5], startAmbient[5];
	Vec3 startPos[5];
	Vec4 targetDiffuse[5], targetSpecular[5], targetAmbient[5];
	Vec3 targetPos[5];

	
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
	
};


#endif // SCENE0_H