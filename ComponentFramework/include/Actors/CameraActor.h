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
	Quaternion orientation;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 translate;
	Vec3 target = Vec3(0.0f, 0.0f, 0.0f);
	float yaw = 0.0f;
	float pitch = 0.0f;
	float pitchAngle = 0.0f;
	float sensitivity = 0.1f;
	bool mouseHeld = false;
	bool ignoreNextMouseDelta = false;
	bool canCamMove = false;
	bool m1Override = false;
	Vec3 position;
	std::unique_ptr<SkyboxComponent> skybox;
	SDL_Window* window;
public:
	CameraActor( std::weak_ptr<Actor>parent_, float fovy, float aspectRatio, float near, float far, SDL_Window* window_);
	~CameraActor();
	bool OnCreate();
	Matrix4 GetProjectionMatrix() { return projectionMatrix; }
	Matrix4 GetViewMatrix() { return viewMatrix; }
	Quaternion GetOrientation() const { return orientation; }
	void SetView(const Quaternion& orientation_, const Vec3& position_) {orientation = orientation_;position = position_;}
	Vec3 GetPosition() const { return position; }
	void SetPosition(Vec3 newPos) { position = newPos; }
	void setOrientaion(Quaternion newOri) { orientation = newOri; }
	void setCamMovement(bool canMove) { canCamMove = canMove; }
	void setM1Override(bool override) { m1Override = override; }
	void setTarget(Vec3 target_) { target = target_; }
	void setCamSensitivity(float sensitivity_) { sensitivity = sensitivity_; }
	Vec3 GetCameraForward();
	Vec3 GetCameraRight();
	void HandleEvents(const SDL_Event& event);
	void UpdateViewMatrix();
	void Update(const float deltaTime);
	bool SkyboxSetup(const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char*
		negYFileName_, const char* negZFileName_);
	int GetSkyTexID() { return skybox->GetTexture(); }
	void  RenderSkybox() const;
};
