#pragma once
#include <glew.h>
#include "Component.h"
#include "ShaderComponent.h"
#include "MeshComponent.h"
#include <unordered_map>
using namespace MATH;
class SkyboxComponent : public Component
{
	std::unique_ptr<ShaderComponent> skyShader;
	std::unique_ptr<MeshComponent> skyMesh;
	unsigned int textureID;
	const char* posXFileName;
	const char* posYFileName;
	const char* posZFileName;
	const char* negXFileName;
	const char* negYFileName;
	const char* negZFileName;
	
public:

	SkyboxComponent(std::weak_ptr<Component>parent_, const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char*
		negYFileName_, const char* negZFileName_);
	~SkyboxComponent();
	virtual bool LoadImages();
	virtual bool OnCreate() override;
	virtual void Render() const override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime_) override;
	ShaderComponent* GetShader() const { return skyShader.get(); }
	virtual int GetTexture() const { return textureID; }

};

