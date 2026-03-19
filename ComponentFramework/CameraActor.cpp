#include "CameraActor.h"
#include "TransformComponent.h"
#include "imgui.h"

CameraActor::CameraActor(std::weak_ptr<Actor>parent_, float fovy, float aspectRatio, float near, float far, SDL_Window* window_) :
	Actor(parent_), window(window_)
{

	projectionMatrix = MMath::perspective(fovy, aspectRatio, near, far);
	viewMatrix.loadIdentity();
}


Vec3 CameraActor::GetCameraForward() {
	Matrix4 camToWorld = MMath::inverse(GetViewMatrix());
	Vec4 forwardCamSpace(0.0f, 0.0f, -1.0f, 0.0f);
	Vec4 forwardWorld = camToWorld * forwardCamSpace;
	return VMath::normalize(Vec3(forwardWorld.x, forwardWorld.y, forwardWorld.z));
}

Vec3 CameraActor::GetCameraRight() {
	Matrix4 camToWorld = MMath::inverse(GetViewMatrix());
	Vec4 rightCamSpace(1.0f, 0.0f, 0.0f, 0.0f);
	Vec4 rightWorld = camToWorld * rightCamSpace;
	return VMath::normalize(Vec3(rightWorld.x, rightWorld.y, rightWorld.z));
}

void CameraActor::HandleEvents(const SDL_Event& event)
{

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		mouseHeld = false; // Ensure we don't get stuck rotating
		return; 
	}

	// Handle Mouse Events for Camera Rotation. Checks if the left mouse button is held down
	if (!m1Override) {
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
			event.button.button == SDL_BUTTON_LEFT) {
			mouseHeld = true;
			ignoreNextMouseDelta = true;
			SDL_SetWindowRelativeMouseMode(window, true);
		}
	}
	// Handle Mouse Release
	if (!m1Override) {
		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
			event.button.button == SDL_BUTTON_LEFT) {
			mouseHeld = false;
			SDL_SetWindowRelativeMouseMode(window, true);
		}
	}
	// Handle Mouse Motion if mouse 1 is held down or if the mouse1 override is active
	if (event.type == SDL_EVENT_MOUSE_MOTION && mouseHeld || event.type == SDL_EVENT_MOUSE_MOTION && m1Override) {
		if (ignoreNextMouseDelta) {
			ignoreNextMouseDelta = false;
			return;
		}
		// Get Mouse Deltas
		float dx = (float)event.motion.xrel;
		float dy = (float)event.motion.yrel;
		// Rotation Sensitivity
		// Update yaw and pitch angles
		yaw -= dx * sensitivity;
		pitchAngle -= dy * sensitivity;
		// Clamp the pitch angle to avoid looking too far up or down 
		if (pitchAngle > 89.0f) pitchAngle = 89.0f;
		if (pitchAngle < -89.0f) pitchAngle = -89.0f;
		/// Create the orientation quaternion from yaw and pitch
		Quaternion yawQ = QMath::angleAxisRotation(yaw, Vec3(0.0f, 1.0f, 0.0f));
		Quaternion pitchQ = QMath::angleAxisRotation(pitchAngle, Vec3(1.0f, 0.0f, 0.0f));
		// Combine yaw and pitch and normalize the orientation
		orientation = yawQ * pitchQ;
		orientation = QMath::normalize(orientation);
	}
}


CameraActor::~CameraActor() {}

bool CameraActor::OnCreate() {
	Ref<TransformComponent> tc = GetComponent<TransformComponent>();
	if (tc != nullptr) {
		viewMatrix = tc->GetTransformMatrix();
		viewMatrix.print("view matrix");
	}
	return true;
}




	
	void CameraActor::UpdateViewMatrix() {
		// A view matrix is the inverse of the camera's world transform
		Matrix4 rotation = MMath::toMatrix4(orientation);
		Matrix4 translation = MMath::translate(position);

		// View = Inverse(Model)
		viewMatrix = MMath::inverse(translation * rotation);
	}


	void CameraActor::Update(const float deltaTime)
	{
		if (!canCamMove) return;

		const bool* keys = SDL_GetKeyboardState(nullptr);
		float moveSpeed = 9.0f * deltaTime;

		// 1. Create a direction vector to accumulate movement
		Vec3 moveDir(0.0f, 0.0f, 0.0f);

		// 2. Accumulate all inputs into the direction vector
		if (keys[SDL_SCANCODE_W]) moveDir += GetCameraForward();
		if (keys[SDL_SCANCODE_S]) moveDir -= GetCameraForward();
		if (keys[SDL_SCANCODE_A]) moveDir -= GetCameraRight();
		if (keys[SDL_SCANCODE_D]) moveDir += GetCameraRight();

		// Vertical movement is usually independent of looking direction, 
		// but you can include it in normalization if you want "3D flight" 
		// to be consistent speed-wise.
		if (keys[SDL_SCANCODE_E]) moveDir += Vec3(0.0f, 1.0f, 0.0f);
		if (keys[SDL_SCANCODE_Q]) moveDir -= Vec3(0.0f, 1.0f, 0.0f);

		// 3. Normalize and Apply
		// We check magnitude to avoid division by zero if no keys are pressed
		if (VMath::mag(moveDir) > 0.001f) {
			moveDir = VMath::normalize(moveDir);
			SetPosition(GetPosition() + moveDir * moveSpeed);
		}

		UpdateViewMatrix();
	}
