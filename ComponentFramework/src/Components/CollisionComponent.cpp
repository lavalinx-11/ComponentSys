#include <algorithm>
#include <iostream>
#include "Components/CollisionComponent.h"
#include "QMath.h"
#include "MMath.h"
#include "Engine/Debug.h"
using namespace MATH;

CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Ref<TransformComponent> transform_, float radius_)
    : Component(parent_), transform(transform_), radius(radius_), colliderType(ColliderType::SPHERE) 
{
    sphere.center = transform->GetPosition();
    sphere.r = radius;
}

CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Ref<TransformComponent> transform_, Ref<MeshComponent> mesh_)
    : Component(parent_), transform(transform_), mesh(mesh_), colliderType(ColliderType::AABB) {}


CollisionComponent::CollisionComponent(std::weak_ptr<Component> parent_, Plane plane_) :
Component(parent_), plane(plane_)
{
    colliderType = ColliderType::PLANE;
}

bool CollisionComponent::OnCreate()
{
    GenerateBoundingBox(mesh);
    return true;
}

void CollisionComponent::GenerateBoundingBox(Ref<MeshComponent> meshComp)
{
    // Confirming if there is a mesh
    if (!meshComp) {
        std::cerr << "Error: meshComp is NULL! (GetComponent failed to find the mesh)" << std::endl;
        halfExtents = Vec3(0.5f, 0.5f, 0.5f); 
        offset = Vec3(0.0f, 0.0f, 0.0f);
        return;
    }

    // If there's a mesh make sure that there are vertices in the mesh
    if (meshComp->GetVertices().empty()) {
        std::cerr << "Error: Mesh found, but it has 0 vertices" << std::endl;
        halfExtents = Vec3(0.5f, 0.5f, 0.5f); 
        offset = Vec3(0.0f, 0.0f, 0.0f);
        return;
    }


    // grab the vertices from the mesh and initialize the highest and lowest points of the mesh
    std::vector<Vec3> vertices = meshComp->GetVertices();
    float maxX = vertices[0].x;
    float maxY = vertices[0].y;
    float maxZ = vertices[0].z;
    float minX = vertices[0].x;
    float minY = vertices[0].y;
    float minZ = vertices[0].z;


    // Loop through each vertex and set the highest and lowest axis
    for (int i = 1; i < vertices.size(); i++)
    {
        maxX = std::max(vertices[i].x, maxX);
        maxY = std::max(vertices[i].y, maxY);
        maxZ = std::max(vertices[i].z, maxZ);
        minX = std::min(vertices[i].x, minX);
        minY = std::min(vertices[i].y, minY);
        minZ = std::min(vertices[i].z, minZ);
    }

    
    Vec3 maxVertices(maxX, maxY, maxZ);
    Vec3 minVertices(minX, minY, minZ);

    // Create the basic half extents
    Vec3 rawExtents = (maxVertices - minVertices) / 2.0f;
    Vec3 rawOffset = (minVertices + maxVertices) / 2.0f;

    // Adjust the extents to have the correct rotation by multiplying each axis by the rotation matrix 
    Matrix3 rotMat = MMath::toMatrix3(transform->GetOrientation());
    Vec3 rotatedExtents;
    rotatedExtents.x = std::abs(rotMat[0]) * rawExtents.x + std::abs(rotMat[3]) * rawExtents.y + std::abs(rotMat[6]) * rawExtents.z;
    rotatedExtents.y = std::abs(rotMat[1]) * rawExtents.x + std::abs(rotMat[4]) * rawExtents.y + std::abs(rotMat[7]) * rawExtents.z;
    rotatedExtents.z = std::abs(rotMat[2]) * rawExtents.x + std::abs(rotMat[5]) * rawExtents.y + std::abs(rotMat[8]) * rawExtents.z;
    Vec3 rotatedOffset = rotMat * rawOffset;
    
    // Grab the scale and adjust the offset and half extents to match the piece in world space
    Vec3 scale = transform->GetScale();
    halfExtents = Vec3(rotatedExtents.x * scale.x, rotatedExtents.y * scale.y, rotatedExtents.z * scale.z);
    offset = Vec3(rotatedOffset.x * scale.x, rotatedOffset.y * scale.y, rotatedOffset.z * scale.z);
}


