#pragma once
#include <VMath.h>
#include "Actor.h"
#include "Components/TransformComponent.h"
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

    Vec4 GetSpecular() const { return Specular; }
    Vec4 GetDiffuse()  const { return Diffuse; }
    Vec4 GetAmbient()  const { return Ambient; }

    void SetSpecular(Vec4 specular) { Specular = specular; }
    void SetDiffuse(Vec4 diffuse) { Diffuse = diffuse; }
    void SetAmbient(Vec4 ambient) { Ambient = ambient; }
    
};
