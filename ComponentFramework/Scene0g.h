#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Actor.h"
#include <unordered_map>
#include "CameraActor.h"
#include "Window.h"
#include "ShaderComponent.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "LightActor.h"
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
	Ref<ShaderComponent> shader;
	Ref<Actor> board;
	Ref<CameraActor> camera;
	std::unordered_map<std::string, ActorData> actors; 
	std::string actorName; 
	std::string actorColour;
	std::string actorType;
	std::vector<std::unique_ptr<Actor>> knightPieces;
	//std::vector<std::shared_ptr<LightActor>> lights;
	std::unique_ptr<LightActor> lights[5];

	bool isTransitioning = false;
	float transitionAlpha = 0.0f; // 0.0 to 1.0
	float transitionSpeed = 0.5f; // 1.0 = 1 second, 0.5 = 2 seconds
	Vec4 startDiffuse[5], startSpecular[5], startAmbient[5];
	Vec3 startPos[5];
	Vec4 targetDiffuse[5], targetSpecular[5], targetAmbient[5];
	Vec3 targetPos[5];
	
	bool drawInWireMode;
	bool showImGuiDemoWindow = true; // optional for testing
	char textBuffer[256] = "";       // input text buffer
	int buttonClicks = 0;            // button counter
	Window* window;
	SDL_GLContext context;
public:
	explicit Scene0g();
	virtual ~Scene0g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
	virtual void RenderGUI() override;
	void RenderAll();
	void RotateKnights(float angleDegrees);
	void SetupMood(std::string moodName);
	
};


#endif // SCENE0_H