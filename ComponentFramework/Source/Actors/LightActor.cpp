#include "Actors/LightActor.h"
LightActor::LightActor(std::weak_ptr<Actor> parent_, Vec4 specular_, Vec4 diffuse_, Vec4 ambient_)
    : Actor(parent_), Specular(specular_), Diffuse(diffuse_), Ambient(ambient_) {
 
}

LightActor::~LightActor() {
}

bool LightActor::OnCreate() {
    // Call the base Actor OnCreate to initialize any components (like Transform)
    if (!Actor::OnCreate()) return false;

    return true;
}

void LightActor::OnDestroy() {
    Actor::OnDestroy();
}

void LightActor::Update(const float deltaTime) {
    Actor::Update(deltaTime);
}

void LightActor::Render() const {
    // Usually, lights don't "render" themselves as a mesh.
    // However, you could call Actor::Render() if you attached a debug sphere component.
    Actor::Render();
}