#pragma once
#ifndef HUD_H
#define HUD_H

#include "resource_manager.h"
#include "shader.h"

class hud {
public:
	hud(glm::mat4 model);
	void render(Shader *shader);
	void cleanUp();
private:
	float verticesPlane[16] = {
		1.0f,  1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f,  1.0f, 0.0f, 1.0f
	};
	unsigned int indicesPlane[6] = {
		0, 2, 1,
		0, 3, 2
	};
	unsigned int VAO, VBO, EBO;
	glm::mat4 model;
	void genVAO();
};

#endif