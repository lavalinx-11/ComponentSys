#pragma once
#include "Component.h"
#include <glew.h>
#include <vector>
#include <Vector.h>
using namespace MATH;
class MeshComponent : public Component
{
	const char* filename;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvCoords;
	std::vector<Vec3> vertices;
	size_t dateLength;
	GLenum drawmode;
	GLuint vao, vbo;
	public:
		MeshComponent(Component* parent_, const char* filename_);
		~MeshComponent();
		virtual bool OnCreate() override;
		virtual void OnDestroy() override;
		virtual void Update(const float deltaTime_) override;
		virtual void Render() const override;

		/// Private helper methods
		void LoadModel(const char* filename);
		void StoreMeshData(GLenum drawmode_); 
};

