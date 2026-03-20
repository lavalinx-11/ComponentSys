#include "SkyboxComponent.h"
#include "Component.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Debug.h"
SkyboxComponent::SkyboxComponent(std::weak_ptr<Component> parent_,
	const char* posXFileName_, const char* posYFileName_,
	const char* posZFileName_, const char* negXFileName_,
	const char* negYFileName_, const char* negZFileName_) :
	Component(parent_)
{

	posXFileName = posXFileName_;
	posYFileName = posYFileName_;
	posZFileName = posZFileName_;
	negXFileName = negXFileName_;
	negYFileName = negYFileName_;
	negZFileName = negZFileName_;
}

SkyboxComponent::~SkyboxComponent() {}

bool SkyboxComponent::LoadImages()
{

	const char* fileNames[] = {posXFileName, negXFileName, posYFileName, negYFileName, posZFileName, negZFileName};

	GLenum targets[] = { 
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 
	};
	
    glGenTextures(1, &textureID); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); 

	for (int i = 0; i < 6; i++) {
		SDL_Surface* surface = IMG_Load(fileNames[i]);
    
		if (surface == nullptr) {
			Debug::Error("Skybox face failed to load: " + std::string(fileNames[i]), __FILE__, __LINE__);
			return false;
		}
		
		GLint internalFormat = SDL_ISPIXELFORMAT_ALPHA(surface->format) ? GL_RGBA : GL_RGB;
		GLenum format = internalFormat; 
		glTexImage2D(targets[i], 0, internalFormat, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
		SDL_DestroySurface(surface);
	}
	
	/// Wrapping and filtering options
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

bool SkyboxComponent::OnCreate()
{
	skyShader = std::make_unique<ShaderComponent>(std::weak_ptr<Component>(),"shaders/skyboxVert.glsl", "shaders/skyboxFrag.glsl");
	if (skyShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	
	skyMesh = std::make_unique<MeshComponent>(std::weak_ptr<Component>(),"meshes/Cube.obj");
	skyMesh->OnCreate();
	
	LoadImages();
	return true;
}

void SkyboxComponent::Render() const 
{
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	skyMesh->Render();
	glDepthMask(GL_TRUE);
}

void SkyboxComponent::OnDestroy()
{
	glDeleteTextures(1, &textureID);

	if (skyShader) skyShader->OnDestroy();
	if (skyMesh) skyMesh->OnDestroy();
}

void SkyboxComponent::Update(const float deltaTime_)
{
}
