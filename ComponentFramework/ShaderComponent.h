#pragma once
#include "Component.h"
#include "glew.h"
#include <unordered_map>
#include <string>
class ShaderComponent : public Component
{
	const char* vertFilename;
	const char* fragFilename;
	const char* tessCtrlFilename;
	const char* tessEvalFilename;
	const char* geomFilename;

	GLuint  shaderID;
	GLuint vertShaderID;
	GLuint fragShaderID;
	GLuint tessCtrlShaderID;
	GLuint tessEvalShaderID;
	GLuint geomShaderID;
	std::unordered_map<std::string, GLuint > uniformMap;

	/// Private helper methods
	char* ReadTextFile(const char* fileName);
	bool CompileAttach();
	bool Link();
	void SetUniformLocations();

public:

	ShaderComponent(std::weak_ptr<Component>parent_, const char* vertFilename_, const char* fragFilename_,
		const char* tessCtrlFilename_ = nullptr, const char* tessEvalFilename = nullptr,
		const char* geomFilename_ = nullptr);
	virtual ~ShaderComponent();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime_) override;
	virtual void Render() const override;

	inline GLuint GetProgram() const { return shaderID; }
	GLuint GetUniformID(std::string name);
};

