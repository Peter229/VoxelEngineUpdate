#pragma once
#ifndef CHUNK_H
#define CHUNK_H

#include "PerlinNoise.h"
#include "resource_manager.h"
#include "shader.h"

#include <cmath>

#include <glm/glm.hpp>

#include <vector>

class chunk {
public:
	chunk(int x, int y, int z, unsigned int seed);
	void render(Shader *shader);
	void cleanUp();
	void deleteBlock(glm::vec3 &orig, glm::vec3 &dir);
	void addBlock(glm::vec3 &orig, glm::vec3 &dir);
	glm::vec3 getPosition();
private:
	struct Triangle {
		glm::vec3 v1;
		glm::vec2 t1;
		glm::vec3 n1;
		glm::vec3 v2;
		glm::vec2 t2;
		glm::vec3 n2;
		glm::vec3 v3;
		glm::vec2 t3;
		glm::vec3 n3;
	};
	int px, py, pz;
	const static int chunkSize = 32;
	uint8_t cubes[chunkSize*chunkSize*chunkSize];
	std::vector<Triangle> tris;
	unsigned int VAO, VBO;
	void genVAO();
	void genChunk();
	glm::mat4 model = glm::mat4(1.0f);
	unsigned int seed;
	bool intersect(glm::vec3 &orig, glm::vec3 &dir, glm::vec3 min, glm::vec3 max);
	bool intersectPlane(const glm::vec3 &n, const glm::vec3 &p0, const glm::vec3 &orig, const glm::vec3 &dir, float &t);
	bool pointInCube(glm::vec3 min, glm::vec3 max, glm::vec3 p);
};

#endif