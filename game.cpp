#include "game.h"

Game::Game() {

}
 
Game::~Game() {

}

void Game::start() {

	if (WindowGL::start() == -1) {
		std::cout << "Failed to create window\n";
	}

	deltaTime = 0.0f;
	lastFrame = 0.0f;

	deltaTimeT = 0;
	deltaTime2 = 0;
	lastTimeA = glfwGetTime();
	lastTimeB = glfwGetTime();
	nbFrames = 0;

	this->windowWidth = SCR_WIDTH;
	this->windowHeight = SCR_HEIGHT;

	dig = canDig = place = canPlace = false;

	endMouse = startMouse = glm::vec3(0.0f, 0.0f, 0.0f);

	//Load Shaders
	terrainShader = new Shader("Shaders/Vertex/vertexShader.txt", "Shaders/Fragment/fragmentShader.txt");
	hudShader = new Shader("Shaders/Vertex/hudVertexShader.txt", "Shaders/Fragment/hudFragmentShader.txt");

	//Load Textures
	Resource_Manager::loadTexture("Textures/textures.png", "tex");
	Resource_Manager::loadTexture("Textures/crosshair.png", "crosshair");

	//Load Level
	for (int x = 0; x < 6; x++) {
		for (int z = 0; z < 6; z++) {
			for (int y = 0; y < 2; y++) {
				chunks.push_back(chunk(x * 32, y * 32, z * 32, 14)); //321 //4124124
			}
		}
	}

	//Create Hud
	huddy = new hud(glm::mat4(1.0f));

	//Create Camera
	camera = new Camera(glm::vec3(0.0f, 50.0f, 0.0f));

	//Create Frustum
	frust = { camera->Front,
		camera->Front * glm::vec3(-1.0f),
		camera->Up * glm::vec3(-1.0f),
		camera->Up,
		camera->Right * glm::vec3(-1.0f),
		camera->Right,
		camera->Position - (camera->Front * glm::vec3(32)) - (camera->Up * glm::vec3(100)) - (camera->Right * glm::vec3(150)),
		camera->Position + (camera->Front * glm::vec3(400)) + (camera->Up * glm::vec3(100)) + (camera->Right * glm::vec3(150)) };
}

void Game::loop() {

	while (!glfwWindowShouldClose(WindowGL::getWindow())) {

		double currentTimeA = glfwGetTime();
		nbFrames++;
		if (currentTimeA - lastTimeA >= 1.0) {
			printf("%f ms/frame\n%d Frames Per Second\n", 1000.0 / double(nbFrames), nbFrames);
			nbFrames = 0;
			lastTimeA += 1.0;
		}

		//Delta Time To Get How Fast The Game Should Run
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Get Inputs
		WindowGL::update();

		update(WindowGL::getKeys(), WindowGL::getMousePos(), deltaTime);

		if (currentTimeA - lastTimeB >= 0.0166) { //Call physics 60 times a second

			lastTimeB += 0.0166;

			//tick();
		}

		render();

		//Go To Next Frame
		glfwSwapBuffers(WindowGL::getWindow());
	}
}

void Game::update(GLboolean* Keys, double* mousePos, float deltaTime) {

	//Mouse Movement
	double xpos = mousePos[0];
	double ypos = mousePos[1];
	
	if (firstMouse) {

		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);

	if (Keys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS) {
		if (canDig) {
			dig = true;
			canDig = false;
		}
	}

	if (Keys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_RELEASE) {
		canDig = true;
	}
	if (Keys[GLFW_MOUSE_BUTTON_RIGHT] == GLFW_PRESS) {
		if (canPlace) {
			place = true;
			canPlace = false;
		}
	}
	if (Keys[GLFW_MOUSE_BUTTON_RIGHT] == GLFW_RELEASE) {
		canPlace = true;
	}
	
	if (Keys[GLFW_KEY_W] == GLFW_PRESS) {
		camera->ProcessKeyboard(FORWARD, deltaTime);
	}
	if (Keys[GLFW_KEY_S] == GLFW_PRESS) {
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (Keys[GLFW_KEY_A] == GLFW_PRESS) {
		camera->ProcessKeyboard(LEFT, deltaTime);
	}
	if (Keys[GLFW_KEY_D] == GLFW_PRESS) {
		camera->ProcessKeyboard(RIGHT, deltaTime);
	}
}

void Game::logic() {

}

void Game::tick() {

}

void Game::render() {

	frust = { camera->Front,
		camera->Front * glm::vec3(-1.0f),
		camera->Up * glm::vec3(-1.0f),
		camera->Up,
		camera->Right * glm::vec3(-1.0f),
		camera->Right,
		camera->Position - (camera->Front * glm::vec3(32)) - (camera->Up * glm::vec3(100)) - (camera->Right * glm::vec3(200)),
		camera->Position + (camera->Front * glm::vec3(400)) + (camera->Up * glm::vec3(100)) + (camera->Right * glm::vec3(200)) };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.9f, 1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 400.0f);
	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	//Render With Basic Shader
	terrainShader->use();
	//Set Projection Matrix
	terrainShader->setMat4("projection", projection);
	//Set View Matrix
	terrainShader->setMat4("view", view);

	for (int x = 0; x < chunks.size(); x++) {
		if ((glm::dot(frust.farTopRight - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.topPlaneNormal) < 0.0f) && (glm::dot(frust.farTopRight - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.rightPlaneNormal) < 0.0f) && (glm::dot(frust.farTopRight - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.farPlaneNormal) < 0.0f) && (glm::dot(frust.nearBottomLeft - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.bottomPlaneNormal) < 0.0f) && (glm::dot(frust.nearBottomLeft - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.leftPlaneNormal) < 0.0f) && (glm::dot(frust.nearBottomLeft - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.nearPlaneNormal) < 0.0f)) {
			chunks[x].render(terrainShader);
			if (camera->Position.x > chunks[x].getPosition().x && camera->Position.x < chunks[x].getPosition().x + 32 &&
				camera->Position.y > chunks[x].getPosition().y && camera->Position.y < chunks[x].getPosition().y + 32 &&
				camera->Position.z > chunks[x].getPosition().z && camera->Position.z < chunks[x].getPosition().z + 32) {
				if (dig) {
					chunks[x].deleteBlock(camera->Position, camera->Front);
					dig = false;
				}
				if (place) {
					chunks[x].addBlock(camera->Position, camera->Front);
					place = false;
				}
			}
		}
	}

	//Render Hud
	hudShader->use();
	huddy->render(hudShader);

}

void Game::cleanUp() {

	Resource_Manager::clearData();

	for (int x = 0; x < chunks.size(); x++) {
		chunks[x].cleanUp();
	}

	huddy->cleanUp();

	WindowGL::end();
}