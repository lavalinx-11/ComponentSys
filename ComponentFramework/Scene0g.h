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
	std::unique_ptr<Actor> knightActor;
	std::unique_ptr<Actor> queenActor;
	std::unique_ptr<Actor> pawnActor;
	std::unique_ptr<Actor> kingActor;
	std::unique_ptr<Actor> bishopActor;
	std::unique_ptr<Actor> rookActor;
	std::unique_ptr<Actor> board;
	std::unique_ptr<CameraActor> camera; 

	std::unordered_map<std::string, std::unique_ptr<Actor>> actors; // Map to store actors by name
	std::string actorName; 
	std::string actorColour;
	std::string actorType;
	std::vector<std::unique_ptr<Actor>> allPieces;
	std::vector<std::unique_ptr<Actor>> knightPieces;
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