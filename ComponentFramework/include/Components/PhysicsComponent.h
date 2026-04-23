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

	/*											<-DEFAULT FUNCTIONS->														*/
	PhysicsComponent(std::weak_ptr<Component> parent_);
	PhysicsComponent(std::weak_ptr<Component> parent_, float mass_ = 1.0f);
	~PhysicsComponent();
	bool OnCreate() override;
	void OnDestroy() override;
	void Update(const float deltaTime_) override;
	void Render() const;
	void ApplyForce(const Vec3& f) { force += f; }

	
	/*											<-GETTERS->														*/
	float GetMass() const {return mass;};
	Vec3& GetVelocity()  {return vel;}
	Vec3 GetPositionUpdate(const float deltaTime);

	
	/*											<-SETTERS->														*/
	void SetMass(float mass_) {mass = mass_;}
	void SetVelocity(Vec3 velocity_) {vel = velocity_;}
	void SetAcceleration(Vec3 acceleration_) {accel = acceleration_;}
	void SetTransform(std::shared_ptr<TransformComponent> transform_) {
		transform = transform_;
	}
};