#pragma once
#include <glew.h>
#include "Component.h"
class TextureComponent: public Component {
private:
	GLuint textureID;
public:
	TextureComponent();
	virtual ~TextureComponent();
	bool LoadImage(const char* filename);
	inline GLuint getTextureID() const { return textureID; }

};

