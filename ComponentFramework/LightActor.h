#pragma once
#include <VMath.h>
#include "Actor.h"
using namespace MATH;

class LightActor : public Actor
{
private:
    Vec4 Specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    Vec4 Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    Vec4 Ambient  = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
public:
    LightActor(std::weak_ptr<Actor>parent_, Vec4 specular_, Vec4 diffuse_, Vec4 ambient_);
    ~LightActor();
    bool OnCreate();
    void OnDestroy() override;
    void Update(const float deltaTime) override;
    void Render() const override;
};
