#include <iostream>
#include "Components/CollisionComponent.h"
#include "QMath.h"
#include "MMath.h"
using namespace MATH;

CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, float radius_) :
Component(parent_), radius(radius_)
{
    colliderType = ColliderType::SPHERE;
}

CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Vec3 halfExtents_) :
Component(parent_), halfExtents(halfExtents_)
{
    colliderType = ColliderType::AABB;
}

CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Plane plane_) :
Component(parent_), plane(plane_)
{
    colliderType = ColliderType::PLANE;
}
