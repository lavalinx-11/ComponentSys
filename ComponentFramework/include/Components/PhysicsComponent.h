#pragma once
#include "Component.h"
#include "Matrix.h"
#include "QMath.h"
#include "Euler.h"
using namespace MATH;
class PhysicsComponent : public Component {
private:
	float mass;
	Vec3 vel;
	
public:
	PhysicsComponent(std::weak_ptr<Component> parent_);
	PhysicsComponent(std::weak_ptr<Component> parent_, Vec3 pos_, Quaternion orientation_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f));
	~PhysicsComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render() const;
	
	float GetMass() const {return mass;};
	void SetMass(Vec3 mass_);
	Vec3 GetVelocity() const {return vel;};
	void SetVelocity(Vec3 velocity_);
};