#pragma once
#include "Component.h"
#include <glew.h>
#include <vector>
#include <Vector.h>
using namespace MATH;
class SkyboxComponent : public Component
{
	const char* filename;
	
public:
	SkyboxComponent(Component* parent_, const char* filename_);
	~SkyboxComponent();
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime_) override;
	virtual void Render() const override;

	/// Private helper methods
	void LoadModel(const char* filename);
	void StoreMeshData(GLenum drawmode_);

};

