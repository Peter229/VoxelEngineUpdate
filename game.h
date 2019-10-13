#pragma once

#include "windowGL.h"

#include "shader.h"
#include "camera.h"
#include "hud.h"
#include "chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <memory>
#include <fstream>
#include <strstream>

struct frustum {
	glm::vec3 nearPlaneNormal;
	glm::vec3 farPlaneNormal;
	glm::vec3 topPlaneNormal;
	glm::vec3 bottomPlaneNormal;
	glm::vec3 rightPlaneNormal;
	glm::vec3 leftPlaneNormal;
	glm::vec3 nearBottomLeft;
	glm::vec3 farTopRight;
};

class Game {

public:
	Game();
	~Game();

	void start();

	void loop();

	void update(GLboolean* Keys, double* mousePos, float deltaTime);
	void logic();

	void tick();

	void render();

	void cleanUp();
private:
	hud* huddy;
	Camera* camera;
	Shader* terrainShader;
	Shader* hudShader;
	frustum frust;
	std::vector<chunk> chunks;
	bool firstMouse;
	double lastX, lastY;
	int windowWidth, windowHeight;
	bool dig, place, canPlace, canDig;
	glm::vec3 startMouse, endMouse;

	float deltaTime;
	float lastFrame;

	float deltaTimeT;
	float deltaTime2;
	double lastTimeA;
	double lastTimeB;
	int nbFrames;

};