#pragma once
#include <vector>
#include "Components/CollisionComponent.h"
#include "Components/PhysicsComponent.h"
#include "Actors/Actor.h"
#include "Engine/Debug.h"
#include <Sphere.h>
using namespace MATH;
using namespace MATHEX;

class CollisionSystem {
private:
    
    std::vector<Ref<Actor>> collidingActors;
public:
    void AddActor(Ref<Actor> actor_) {
        if(actor_->GetComponent<CollisionComponent>().get() == nullptr){
            Debug::Error("The Actor must have a CollisionComponent - ignored ", __FILE__, __LINE__);
            return;
        }

        if(actor_->GetComponent<PhysicsComponent>().get() == nullptr){
            Debug::Error("The Actor must have a PhysicsComponent - ignored ", __FILE__, __LINE__);
            return;
        }
        collidingActors.push_back(actor_);
    }

    static void ApplyImpulse(Ref<PhysicsComponent> pc1, Ref<PhysicsComponent> pc2, Vec3 normal);
    bool CollisionDetection(const Sphere &s1, const Sphere &s2) const; 
    bool CollisionDetection(const AABB &bb1, const AABB &bb2) const; 
    bool CollisionDetection(const Sphere s1, const Plane p1) const;
    void SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2);
    void AABBCollisionResponse(Ref<CollisionComponent> col1, Ref<PhysicsComponent> pc1, Ref<CollisionComponent> col2, Ref<PhysicsComponent> pc2);
    void Update(const float deltaTime);
	
};
