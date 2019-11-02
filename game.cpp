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

	firstMouse = false;

	this->windowWidth = SCR_WIDTH;
	this->windowHeight = SCR_HEIGHT;

	dig = canDig = place = canPlace = false;

	endMouse = startMouse = glm::vec3(0.0f, 0.0f, 0.0f);

	//Load Shaders
	terrainShader = new Shader("Shaders/Vertex/vertexShader.txt", "Shaders/Fragment/fragmentShader.txt");
	hudShader = new Shader("Shaders/Vertex/hudVertexShader.txt", "Shaders/Fragment/hudFragmentShader.txt");

	//Load Textures
	Resource_Manager::loadTexture("Textures/textures.png", "tex");
	Resource_Manager::loadTexture("Textures/font.png", "font");

	chunkSize = 32;
	maxChunks = 6;
	seed = 136;

	//Load Level
	for (int x = 0; x < maxChunks; x++) {
		for (int z = 0; z < maxChunks; z++) {
			//chunks.push_back(chunk(x * chunkSize, 0, z * chunkSize, 136)); //321 //4124124 //136
			chunks[maxChunks * x + z] = new chunk(x * chunkSize, 0, z * chunkSize, seed);
		}
	}

	//Create Hud
	huddy = new hud(glm::mat4(1.0f), windowWidth, windowHeight);

	huddy->drawCrosshair();
	huddy->drawString("FPS " + std::to_string(0), -38.0f, -8.5f);

	//Create Camera
	camera = new Camera(glm::vec3(100.0f, 50.0f, 100.0f));

	//Create Frustum
	frust = { camera->Front,
		camera->Front * glm::vec3(-1.0f),
		camera->Up * glm::vec3(-1.0f),
		camera->Up,
		camera->Right * glm::vec3(-1.0f),
		camera->Right,
		camera->Position - (camera->Front * glm::vec3(32)) - (camera->Up * glm::vec3(100)) - (camera->Right * glm::vec3(150)),
		camera->Position + (camera->Front * glm::vec3(400)) + (camera->Up * glm::vec3(100)) + (camera->Right * glm::vec3(150)) };

	cModel.genVAO();
	canGen = switchGen = dynGen = false;

	lastChunkX = lastChunkZ = cu = 0;

	updateFPS = false;
	firstFPS = true;
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

	if (Keys[GLFW_KEY_1] == GLFW_PRESS) {
		cPlayer.setEquipState(0);
	}
	if (Keys[GLFW_KEY_2] == GLFW_PRESS) {
		cPlayer.setEquipState(1);
	}

	if (Keys[GLFW_KEY_0] == GLFW_PRESS) {
		if (canGen == true) {
			for (auto iter : chunks) {
				iter.second->cleanUp();
			}
			chunks.clear();
			for (int x = 0; x < maxChunks; x++) {
				for (int z = 0; z < maxChunks; z++) {
					//chunks.push_back(chunk(x * chunkSize, 0, z * chunkSize, (int)(camera->Position.x + camera->Position.z + camera->Position.y))); //321 //4124124 //136
					chunks[32 * x + z] = new chunk(x * chunkSize, 0, z * chunkSize, (int)(camera->Position.x + camera->Position.z + camera->Position.y));
				}
			}
			std::cout << (int)(camera->Position.x + camera->Position.z + camera->Position.y) << "\n";
			canGen = false;
		}
	}
	if (Keys[GLFW_KEY_0] == GLFW_RELEASE) {
		canGen = true;
	}

	if (Keys[GLFW_KEY_9] == GLFW_PRESS) {
		if (switchGen) {
			dynGen = !dynGen;
		}
		if (dynGen) {
			cIt = chunks.begin();
		}
		switchGen = false;
	}
	if (Keys[GLFW_KEY_9] == GLFW_RELEASE) {
		switchGen = true;
	}

	if (dynGen) {
		
		/*if (!(camera->Position.x > chunks[x].getPosition().x && camera->Position.x < chunks[x].getPosition().x + 32 &&
			camera->Position.z > chunks[x].getPosition().z && camera->Position.z < chunks[x].getPosition().z + 32)) {
			//if (chunks[0].getPosition().x == (int)(camera->Position.x / 32) * 32 && chunks[0].getPosition().z == (int)(camera->Position.z / 32) * 32) {

			//}
			chunks[0].reGen((int)(camera->Position.x / 32) * 32, 0, (int)(camera->Position.z / 32) * 32);
		}*/
		//std::cout << (chunks[0].getPosition().x - (int)(camera->Position.x / 32) * 32)*(chunks[0].getPosition().x - (int)(camera->Position.x / 32) * 32) + (chunks[0].getPosition().z - (int)(camera->Position.z / 32) * 32)*(chunks[0].getPosition().z - (int)(camera->Position.z / 32) * 32) << "\n";
		
		/*int chunkX = (int)(camera->Position.x / 32) * 32;
		int chunkZ = (int)(camera->Position.z / 32) * 32;
		//if (lastChunkX != chunkX || lastChunkZ != chunkZ) {
			lastChunkX = chunkX;
			lastChunkZ = chunkZ;
			bool foundNewChunk = false;
			if ((chunks[cu].getPosition().x - chunkX)*(chunks[cu].getPosition().x - chunkX) + (chunks[cu].getPosition().z - chunkZ)*(chunks[cu].getPosition().z - chunkZ) >= 10000) {
				int x = chunkX - 3;
				int z = chunkZ - 3;
				int ci = 0;
				while (!foundNewChunk) {
					bool check = false;
					for (int c = 0; c < chunks.size(); c++) {
						if (chunks[c].getPosition().x == x && chunks[c].getPosition().z == z) {
							check = true;
							if (ci % 3 == 0) {
								x += 32;
								z -= (32 * 3);
							}
							else {
								x += 32;
							}
							if (ci == 9) {
								foundNewChunk = true;
							}
						}
					}
					ci += 1;
					if (check == false) {
						chunks[cu].reGen(x, 0, z);
						std::cout << x << " " << z << "\n";
						foundNewChunk = true;
					}
				}
			}
			cu = (cu + 1) % 36;
		//}
		//dynGen = false;
		*/

		//int pChunkX = (int)(((camera->Position.x / chunkSize) * chunkSize) / chunkSize);
		//int pChunkZ = (int)(((camera->Position.z / chunkSize) * chunkSize) / chunkSize);

		//std::cout << pChunkX << " " << pChunkZ << "\n";

		//int spawnR = 15000;
		//int despawnR = 20000;
		
		/*cIt++;
		if (cIt == chunks.end()) {
			cIt = chunks.begin();
		}

		if ((cIt->second->getPosition().x + chunkSize - camera->Position.x)*(cIt->second->getPosition().x - camera->Position.x) > despawnR || (cIt->second->getPosition().z - camera->Position.z)*(cIt->second->getPosition().z - camera->Position.z) > despawnR) {

			cIt->second->cleanUp();

			//auto it = chunks.find(maxChunks * chunks.at(cu).getPosition().x + chunks.at(cu).getPosition().z);

			cIt = chunks.erase(cIt);
			//std::cout << "Del\n";

			for (int x = pChunkX - 3; x < pChunkX + 3; x++) {
				for (int z = pChunkX - 3; z < pChunkX + 3; z++) {
					if (chunks.find(maxChunks * x + z) == chunks.end()) {
						if (chunks.size() < maxChunks*maxChunks) {
							chunks[maxChunks * x + z] = new chunk(x * chunkSize, 0, z * chunkSize, seed);
							//std::cout << "create\n";
						}
					}
				}
			}
			
		}

		/*if (chunks.size() < maxChunks*maxChunks) {
			for (int x = pChunkX - 3; x < pChunkX + 3; x++) {
				for (int z = pChunkX - 3; z < pChunkX + 3; z++) {
					if (chunks.find(maxChunks * x + z) == chunks.end()) {
						if (chunks.size() < maxChunks*maxChunks) {
							chunks[maxChunks * x + z] = chunk(x * chunkSize, 0, z * chunkSize, seed);
							//std::cout << "create\n";
						}
					}
				}
			}
		}*/

		//for (int x = 0; x < 6; x += 1) {

			//for (int z = 0; z < 6; z += 1) {
				//std::cout << 6 * x + z << "\n";
				//chunks[6 * x + z].reGen(x * 32, 0, z * 32);
			//}
		//}
		
		int cX = camera->Position.x / 32;
		int cZ = camera->Position.z / 32;

		int loadDistance = 5;

		bool chunkLoaded = false;

		for (int i = 0; i < loadDistance; i++) {

			int minX = std::max(cX - i, 0);
			int minZ = std::max(cZ - i, 0);
			int maxX = cX + i;
			int maxZ = cZ + i;

			for (int x = minX; x < maxX; ++x) {

				for (int z = minZ; z < maxZ; ++z) {

					/*if (chunks.count(32 * x + z) != 0 && !chunkLoaded) {
						std::cout << "CREATED CHUNK\n";
						chunks[maxChunks * x + z] = new chunk(x * chunkSize, 0, z * chunkSize, seed);
						chunkLoaded = true;
					}*/

					if (chunks.find(32 * x + z) == chunks.end()) {
						chunks[32 * x + z] = new chunk(x * chunkSize, 0, z * chunkSize, seed);
						chunkLoaded = true;
						//std::cout << "create\n";
					}
				}

				if (chunkLoaded) {
					break;
				}
			}
		}

		std::unordered_map<int, chunk*>::iterator it;

		for (it = chunks.begin(); it != chunks.end(); ++it) {
			glm::vec3 temp = it->second->getPosition();
			if ((abs(temp.x - (int)camera->Position.x) > 200) || (abs(temp.z - (int)camera->Position.z) > 200)) {
				it->second->cleanUp();
				delete it->second;
				it = chunks.erase(it);
			}
		}

		//cManager->removeChunks((int)camera->Position.x, (int)camera->Position.z);
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

	for (auto iter : chunks) {
		//if ((glm::dot(frust.farTopRight - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.topPlaneNormal) < 0.0f) && (glm::dot(frust.farTopRight - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.rightPlaneNormal) < 0.0f) && (glm::dot(frust.farTopRight - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.farPlaneNormal) < 0.0f) && (glm::dot(frust.nearBottomLeft - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.bottomPlaneNormal) < 0.0f) && (glm::dot(frust.nearBottomLeft - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.leftPlaneNormal) < 0.0f) && (glm::dot(frust.nearBottomLeft - (chunks[x].getPosition() + glm::vec3(16.0f)), frust.nearPlaneNormal) < 0.0f)) {
		iter.second->render(terrainShader);
		if (camera->Position.x > iter.second->getPosition().x && camera->Position.x < iter.second->getPosition().x + 32 &&
			camera->Position.z > iter.second->getPosition().z && camera->Position.z < iter.second->getPosition().z + 32) {
			if (dig) {
				iter.second->deleteBlock(camera->Position, camera->Front);
				dig = false;
			}
			if (place) {
				iter.second->addBlock(camera->Position, camera->Front);
				place = false;
			}
			glm::vec3 temp = iter.second->getBlockPos(camera->Position, camera->Front);
			model = glm::translate(model, temp);
		}
		//}
	}

	//std::cout << chunks.size() << "\n";

	if (cPlayer.getEquipState() == 1) {

		cModel.renderCube(terrainShader, model);
	}

	//Render Hud
	hudShader->use();
	huddy->drawString("X " + std::to_string((int)camera->Position.x) + " Y " + std::to_string((int)camera->Position.y) + " Z " + std::to_string((int)camera->Position.z), -38.0f, -10.5f);
	huddy->render(hudShader);
	huddy->removeHudPart(1);
}

void Game::cleanUp() {

	Resource_Manager::clearData();

	for (auto iter : chunks) {
		iter.second->cleanUp();
	}

	huddy->cleanUp();

	WindowGL::end();
}