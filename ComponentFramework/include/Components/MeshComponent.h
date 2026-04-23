#pragma once
#include "Component.h"
#include <glew.h>
#include <Matrix.h>
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
	GLuint vao, vbo, instanceVBO;
	Vec3 rawExtents;
	Vec3 rawOffset;
	int maxInstances;
	
	public:
		MeshComponent(std::weak_ptr<Component> parent_, const char* filename_);
		~MeshComponent();
		bool OnCreate() override;
		 void OnDestroy() override;
		 void Update(const float deltaTime_) override;
		 void Render() const override;

		/// Private helper methods
		void LoadModel(const char* filename);
		void StoreMeshData(GLenum drawmode_);
		std::vector<Vec3> GetVertices() {return vertices;}
		void DumpVertices() {vertices.clear();}
	inline Vec3 GetRawExtents() const { return rawExtents; }
	inline Vec3 GetRawOffset() const { return rawOffset; }
	void UpdateInstanceData(const std::vector<Matrix4>& transforms);
	void RenderInstanced(int instanceCount) const;
};

