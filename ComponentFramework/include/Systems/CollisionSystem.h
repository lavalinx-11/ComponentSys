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
    /// This function will check the the actor being added is new and has the all proper components 
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

    bool CollisionDetection(const Sphere &s1, const Sphere &s2) const; 
    bool CollisionDetection(const AABB &bb1, const AABB &bb2) const; 
    bool CollisionDetection(const Sphere s1, const Plane p1) const;
    void Update(const float deltaTime);
	
};
