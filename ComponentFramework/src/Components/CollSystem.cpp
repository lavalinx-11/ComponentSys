#include "Components/CollSystem.h"
#include "VMath.h"

using namespace  MATH;

bool CollisionSystem::CollisionDetection(const Sphere& s1, const Sphere& s2) const
{
    float distanceX = s1.center.x - s2.center.x;
    float distanceY = s1.center.y - s2.center.y;
    float distanceZ = s1.center.z - s2.center.z;
    
    float squaredDistance = (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ);
    float radiusTotal = s1.r + s2.r;
    
    return squaredDistance <= radiusTotal * radiusTotal;
    
}

/*bool CollisionSystem::CollisionDetection(const AABB& bb1, const AABB& bb2) const
{
    
}*/

/*bool CollisionSystem::CollisionDetection(const Sphere s1, const Plane p1) const
{
    
}*/


void CollisionSystem::SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2) {
    float e = 1.0f; /// coefficient of restitution
    Vec3 L = s1.center - s2.center;
    Vec3 n = VMath::normalize(L);
    Vec3 v1 = pc1->GetVelocity();
    Vec3 v2 = pc2->GetVelocity();
    float m1 = pc1->GetMass();
    float m2 = pc2->GetMass();

    float v1p = VMath::dot(v1, n);
    float v2p = VMath::dot(v2, n);

    /*if(v1p - v2p > 0.0f) { 
        return;
    }*/
    float v1p_new = (((m1 - e * m2) * v1p) + ((1.0f + e) * m2 * v2p)) / (m1 + m2);
    float v2p_new = (((m2 - e * m1) * v2p) + ((1.0f + e) * m1 * v1p)) / (m1 + m2);

    pc1->GetVelocity() = v1 + (v1p_new - v1p) * n;
    pc2->GetVelocity() = v2 + (v2p_new - v2p) * n;
}

void CollisionSystem::Update(const float deltaTime)
{
    
}
