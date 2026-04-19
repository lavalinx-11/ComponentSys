#pragma once
#include "Component.h"
#include "Matrix.h"
#include "QMath.h"
#include "Euler.h"
#include "Components/TransformComponent.h"
using namespace MATH;
class PhysicsComponent : public Component {
private:
	float mass;
	float drag;
	Vec3 vel;
	Vec3 accel;
	Vec3 force;

protected:
	std::shared_ptr<TransformComponent> transform;
	
public:
	PhysicsComponent(std::weak_ptr<Component> parent_);
	PhysicsComponent(std::weak_ptr<Component> parent_, float mass_ = 1.0f);
	~PhysicsComponent();
	bool OnCreate() override;
	void OnDestroy() override;
	void Update(const float deltaTime_) override;
	void Render() const;

	
	float GetMass() const {return mass;};
	void SetMass(Vec3 mass_);
	Vec3& GetVelocity()  {return vel;}
	void SetVelocity(Vec3 velocity_);
	void ApplyForce(const Vec3& f) { force += f; }
	Vec3 GetPositionUpdate(const float deltaTime);
	void SetTransform(std::shared_ptr<TransformComponent> transform_) {
		transform = transform_;
	}
};