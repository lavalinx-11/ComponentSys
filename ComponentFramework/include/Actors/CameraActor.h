#pragma once
#include "Actor.h"
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>
#include <MMath.h>
#include <VMath.h>
#include <QMath.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Components/SkyboxComponent.h"

using namespace MATH;
class CameraActor : public Actor {
private:
	// Math Variables
	Quaternion orientation;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 translate;

	// Basic Variables
	float yaw = 0.0f;
	float pitch = 0.0f;
	float pitchAngle = 0.0f;
	float sensitivity = 0.1f;
	bool mouseHeld = false;
	bool ignoreNextMouseDelta = false;
	bool canCamMove = false;
	bool m1Override = false;
	float camSpeed = 9.0f;

	// Vectors And Skybox
	Vec3 target = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 position;
	std::unique_ptr<SkyboxComponent> skybox;
	SDL_Window* window;
public:
	CameraActor( std::weak_ptr<Actor>parent_, float fovy, float aspectRatio, float near, float far, SDL_Window* window_);
	~CameraActor();
	bool OnCreate();
	void Update(const float deltaTime);
	void HandleEvents(const SDL_Event& event);
	void UpdateViewMatrix();
	bool SkyboxSetup(const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char*
		negYFileName_, const char* negZFileName_);
	void  RenderSkybox() const;
	
	/*											<-SETTERS->														*/
	void SetView(const Quaternion& orientation_, const Vec3& position_) {orientation = orientation_;position = position_;}
	void SetPosition(Vec3 newPos) { position = newPos; }
	void SetOrientaion(Quaternion newOri) { orientation = newOri; }
	void SetCamMovement(bool canMove) { canCamMove = canMove; }
	void SetM1Override(bool override) { m1Override = override; }
	void SetTarget(Vec3 target_) { target = target_; }
	void SetCamSensitivity(float sensitivity_) { sensitivity = sensitivity_; }
	void SetCameraSpeed(float speed_) {camSpeed = speed_;}

	/*											<-GETTERS->														*/
	float GetSensitivity() const { return sensitivity; }
	float GetCameraSpeed() const {return camSpeed;}
	int GetSkyTexID() { return skybox->GetTexture(); }
	Vec3 GetCameraRight();
	Vec3 GetCameraForward();
	Vec3 GetPosition() const { return position; }
	Quaternion GetOrientation() const { return orientation; }
	Matrix4 GetProjectionMatrix() { return projectionMatrix; }
	Matrix4 GetViewMatrix() { return viewMatrix; }

};
