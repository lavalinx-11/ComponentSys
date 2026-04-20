#include <iostream>
#include "Components/CollisionComponent.h"
#include "QMath.h"
#include "MMath.h"
using namespace MATH;

CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Ref<TransformComponent> transform_, float radius_)
    : Component(parent_), transform(transform_), radius(radius_), colliderType(ColliderType::SPHERE) 
{
    sphere.center = transform->GetPosition();
    sphere.r = radius;
}

CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Ref<TransformComponent> transform_, Vec3 halfExtents_, Vec3 offset_)
    : Component(parent_), transform(transform_), halfExtents(halfExtents_), offset(offset_), colliderType(ColliderType::AABB) {}


CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Plane plane_) :
Component(parent_), plane(plane_)
{
    colliderType = ColliderType::PLANE;
}
