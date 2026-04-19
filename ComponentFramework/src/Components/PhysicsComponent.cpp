#include <iostream>
#include "Components/PhysicsComponent.h"
#include "Actors/Actor.h"
#include "QMath.h"
#include "MMath.h"
using namespace MATH;

PhysicsComponent::PhysicsComponent(std::weak_ptr<Component> parent_, float mass_) :
Component(parent_), mass(mass_)
{
    if (mass <= 0.0f) mass = 1.0f;
    vel = Vec3(0.0f, 0.0f, 0.0f);
    accel = Vec3(0.0f, 0.0f, 0.0f);
    force = Vec3(0.0f, 0.0f, 0.0f);
}

PhysicsComponent::~PhysicsComponent()
{
    
}

bool PhysicsComponent::OnCreate()
{
    std::shared_ptr<Component> parentPtr = parent.lock(); 

    if (parentPtr) {
        // 2. Cast the parent to an Actor so we can look for other components
        std::shared_ptr<Actor> actor = std::dynamic_pointer_cast<Actor>(parentPtr);
        
        if (actor) {
            // 3. Cache the transform so we don't have to look it up in Update()
            transform = actor->GetComponent<TransformComponent>();
        }
    }

    // Return true if we successfully found our transform
    return transform != nullptr;
}

void PhysicsComponent::OnDestroy()
{
}

void PhysicsComponent::Update(const float deltaTime_)
{
    accel = force / mass;
    vel += accel * deltaTime_;
    vel *= 0.98f; 
    if (transform) {
        Vec3 currentPos = transform->GetPosition();
        transform->setPosition(currentPos + (vel * deltaTime_));
    }

    force = Vec3(0.0f, 0.0f, 0.0f);
}

void PhysicsComponent::Render() const
{
}
