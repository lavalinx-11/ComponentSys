#pragma once
#include "TransformComponent.h"
#include <Plane.h>
#include <Sphere.h>
using namespace MATHEX;

enum class ColliderType {
	SPHERE,
	AABB,
	PLANE,
	
};

struct DebugBounds {
	Vec3 center;
	Vec3 scale;
};


/// For an Axis Aligned Bounding Box, there are many ways you could define the box.
/// My favorite way, the easiest to understand way is pick the center location of 
/// the box, then specify the radius from that center in 
/// the x, y,and z dimensions. Umer calls the radius the halfExtent
struct AABB {
	Vec3 center;
	Vec3 halfExtents;
};

class CollisionComponent: public Component {
	friend class CollisionSystem;
	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&) = delete;
	CollisionComponent& operator = (const CollisionComponent&) = delete;
	CollisionComponent& operator = (CollisionComponent&&) = delete;
protected:
	Ref<TransformComponent> transform;
	ColliderType colliderType;  
	Vec3 halfExtents; /// AABB
	Vec3 offset;
	Plane plane; /// Plane
	Sphere sphere;
	float radius; /// Sphere collision
public:
	
	/*											<-DEFAULT FUNCTIONS->														*/
	CollisionComponent(std::weak_ptr<Component> parent_, Ref<TransformComponent> transform_, float radius_ );
	CollisionComponent(std::weak_ptr<Component> parent_, Ref<TransformComponent> transform_, Vec3 halfExtents_, Vec3 offset_);
	CollisionComponent(std::weak_ptr<Component> parent_, Plane plane_);
	bool OnCreate(){return true;}
	void OnDestroy(){}
	void Update(const float deltaTime_){}
	void Render()const{}
	ColliderType GetColliderType() const {return colliderType;}


	/*			<-SPHERE COLLISIONS->			*/
	Sphere GetSphere() const {
		Sphere s;
		if (transform) {
			s.center = transform->GetPosition() + offset;
			s.r = radius;
		}
		return s;
	}
	void SetSphere(const Vec3& center, const float& r) { sphere.center = center; sphere.r = r; }
	

	/*				<-AABB COLLISIONS->			*/
	AABB GetAABB() const {
		AABB box;
		if (transform) {
			box.center = transform->GetPosition() + offset; 
			box.halfExtents = halfExtents; 
		}
		return box;
	}
	Vec3 GetOffset() const {return offset;}




	/*											<-HITBOX->														*/
	DebugBounds GetDebugBounds() const {
		DebugBounds db;
		if (colliderType == ColliderType::SPHERE) {
			db.center = transform->GetPosition(); 
			db.scale = Vec3(radius, radius, radius);
		} else {
			db.center = transform->GetPosition() + offset;
			db.scale = halfExtents * 2.0f;
		}
		return db;
	}

};
