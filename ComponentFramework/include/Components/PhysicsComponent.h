#pragma once
#include "Component.h"
#include "Matrix.h"
#include "QMath.h"
#include "Euler.h"
using namespace MATH;
class PhysicsComponent : public Component {
private:
	Vec3 pos;
	Vec3 scale;
	Quaternion orientation;

public:
	PhysicsComponent(std::weak_ptr<Component> parent_);
	PhysicsComponent(std::weak_ptr<Component> parent_, Vec3 pos_, Quaternion orientation_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f));
	~PhysicsComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render() const;
	
};