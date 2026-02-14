#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include "Actor.h"
#include <unordered_map>
#include "CameraActor.h"
#include "Window.h"
/// Forward declarations 
union SDL_Event;


class Scene0g : public Scene {
private:
	Actor* actor;
	Actor* board;
	CameraActor* camera;
	std::unordered_map<std::string, Actor*> actors; // Map to store actors by name
	std::string actorName; 
	std::string actorColour;
	std::string actorType;
	std::vector<Actor*> allPieces;   
	std::vector<Actor*> knightPieces; 
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
};


#endif // SCENE0_H