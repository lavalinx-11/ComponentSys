#include "Components/CollSystem.h"
#include "VMath.h"

using namespace  MATH;



void CollisionSystem::ApplyImpulse(Ref<PhysicsComponent> pc1, Ref<PhysicsComponent> pc2, Vec3 normal)
{

    float e = 1.0f; // Elasticity / Bounciness
    Vec3 v1 = pc1->GetVelocity(); // Initial Velocity
    Vec3 v2 = pc2->GetVelocity();
    float m1 = pc1->GetMass();
    float m2 = pc2->GetMass();

    // Get dot prod to find direction of velocity
    float v1p = VMath::dot(v1, normal);
    float v2p = VMath::dot(v2, normal);

    // if they're moving apart do nothing
    if(v1p - v2p > 0.0f) return;

    // Final Velocity Direction
    float v1p_new = (((m1 - e * m2) * v1p) + ((1.0f + e) * m2 * v2p)) / (m1 + m2);
    float v2p_new = (((m2 - e * m1) * v2p) + ((1.0f + e) * m1 * v1p)) / (m1 + m2);

    pc1->SetVelocity(v1 + (v1p_new - v1p) * normal);
    pc2->SetVelocity(v2 + (v2p_new - v2p) * normal);
}

bool CollisionSystem::CollisionDetection(const Sphere& s1, const Sphere& s2) const
{
    float distanceX = s1.center.x - s2.center.x;
    float distanceY = s1.center.y - s2.center.y;
    float distanceZ = s1.center.z - s2.center.z;
    
    float squaredDistance = (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ);
    float radiusTotal = s1.r + s2.r;
    
    return squaredDistance <= radiusTotal * radiusTotal;
    
}

bool CollisionSystem::CollisionDetection(const AABB& bb1, const AABB& bb2) const
{
    if (std::abs(bb1.center.x - bb2.center.x) > (bb1.halfExtents.x + bb2.halfExtents.x)) return false;
    if (std::abs(bb1.center.y - bb2.center.y) > (bb1.halfExtents.y + bb2.halfExtents.y)) return false;
    if (std::abs(bb1.center.z - bb2.center.z) > (bb1.halfExtents.z + bb2.halfExtents.z)) return false;

    return true;
}

/*bool CollisionSystem::CollisionDetection(const Sphere s1, const Plane p1) const
{
    
}*/


void CollisionSystem::SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2)
{
    Vec3 L = s1.center - s2.center;
    Vec3 n = VMath::normalize(L);

   ApplyImpulse(pc1, pc2, n);
}


void CollisionSystem::AABBCollisionResponse(Ref<CollisionComponent> col1, Ref<PhysicsComponent> pc1, Ref<CollisionComponent> col2, Ref<PhysicsComponent> pc2)
{
// Find normal of piece then apply impulse 
    AABB a = col1->GetAABB();
    AABB b = col2->GetAABB();
    Vec3 distance = a.center - b.center;
    float xOverlap = (a.halfExtents.x + b.halfExtents.x) - std::abs(distance.x);    
    float yOverlap = (a.halfExtents.y + b.halfExtents.y) - std::abs(distance.y);
    
  

    Vec3 normal;
    
    float overlapDiff = std::abs(xOverlap - yOverlap);
   
    float cornerThreshold = 0.2f; 

    if (overlapDiff < cornerThreshold) {
        normal = VMath::normalize(Vec3(distance.x, distance.y, 0.0f));
    }
    else if (xOverlap < yOverlap) {
        // Hit the Left or Right flat wall
        normal = Vec3(distance.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
    } 
    else {
        // Hit the Top or Bottom flat wall
        normal = Vec3(0.0f, distance.y > 0 ? 1.0f : -1.0f, 0.0f);
    }
    

    ApplyImpulse(pc1,pc2, normal);
}

void CollisionSystem::Update(const float deltaTime)
{
    
}
