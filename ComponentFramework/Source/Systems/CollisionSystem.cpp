#include "Systems/CollisionSystem.h"
#include "VMath.h"

using namespace  MATH;

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

    if(v1p - v2p > 0.0f) { /// The colliding objects are not yet free from one and another, come back next cycle
        return;
    }
    float v1p_new = (((m1 - e * m2) * v1p) + ((1.0f + e) * m2 * v2p)) / (m1 + m2);
    float v2p_new = (((m2 - e * m1) * v2p) + ((1.0f + e) * m1 * v1p)) / (m1 + m2);

    pc1->GetVelocity() = v1 + (v1p_new - v1p) * n;
    pc2->GetVelocity() = v2 + (v2p_new - v2p) * n;
}