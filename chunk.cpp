#include "chunk.h"

chunk::chunk() {
	this->px = 0;
	this->py = 0;
	this->pz = 0;
	this->seed = 0;
	model = glm::translate(model, glm::vec3(0, 0, 0));
}

chunk::chunk(int x, int y, int z, unsigned int seed) {
	this->px = x;
	this->py = y;
	this->pz = z;
	this->seed = seed;
	model = glm::translate(model, glm::vec3(px, py, pz));
	genChunk();
}

void chunk::render(Shader *shader) {
	glBindTexture(GL_TEXTURE_2D, Resource_Manager::getTexture("tex"));
	glBindVertexArray(VAO);
	shader->setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, tris.size() * 3);
	//std::cout << "BOI\n";
}

glm::vec3 chunk::getPosition() {
	return glm::vec3(px, py, pz);
}

void chunk::reGen(int x, int y, int z) {
	this->px = x;
	this->py = y;
	this->pz = z;
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(px, py, pz));
	cleanUp();
	genChunk();
}

void chunk::deleteBlock(glm::vec3 &orig, glm::vec3 &dir) {

	glm::ivec3 tempPos(10000000.0f);

	float dist = 0;
	float dist2 = 0;

	for (int x = 0; x < chunkSize; x++) {
		for (int y = 0; y < chunkSize; y++) {
			for (int z = 0; z < chunkSize; z++) {
				if (glm::dot(orig - glm::vec3(px+x+0.5, py + y+0.5, pz+z+0.5), dir) < 0.0f) {
					if (intersect(orig, dir, glm::vec3(x + px, y + py, z + pz), glm::vec3(x + px + 1, y + py + 1, z + pz + 1))) {
						if (cubes[x + chunkSize * (y + chunkSize * z)] != 0) {
							//std::cout << glm::dot(glm::vec3(x + 0.5, y + 0.5, z + 0.5) - orig, dir) << "\n";
							dist = pow(orig.x - (px + x + 0.5), 2) + pow(orig.y - (py + y + 0.5), 2) + pow(orig.z - (pz + z + 0.5), 2);
							dist2 = pow(orig.x - (px + tempPos.x + 0.5), 2) + pow(orig.y - (py + tempPos.y + 0.5), 2) + pow(orig.z - (pz + tempPos.z + 0.5), 2);
							if (dist < dist2) {
								tempPos = glm::ivec3(x, y, z);
							}
						}
					}
				}
			}
		}
	}

	if (tempPos.x < chunkSize) {
		cubes[tempPos.x + chunkSize * (tempPos.y + chunkSize * tempPos.z)] = 0;
		tris.clear();
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		genVAO();
	}
}

void chunk::addBlock(glm::vec3 &orig, glm::vec3 &dir) {

	glm::ivec3 tempPos(10000000.0f);
	glm::ivec3 finalPos(0.0f);

	float dist = 0;
	float dist2 = 0;
	float finalDist = 0; //Distance to closest block
	float finalPosDist = 0; //Distance to air block

	std::vector<glm::ivec3> airBlocks;

	for (int x = 0; x < chunkSize; x++) {
		for (int y = 0; y < chunkSize; y++) {
			for (int z = 0; z < chunkSize; z++) {
				if (glm::dot(orig - glm::vec3(px + x + 0.5, py + y + 0.5, pz + z + 0.5), dir) < 0.0f) { //If block is infront
					if (intersect(orig, dir, glm::vec3(x + px, y + py, z + pz), glm::vec3(x + px + 1, y + py + 1, z + pz + 1))) { //If the ray intersects the block
						if (cubes[x + chunkSize * (y + chunkSize * z)] != 0) {
							dist = pow(orig.x - (px + x + 0.5), 2) + pow(orig.y - (py + y + 0.5), 2) + pow(orig.z - (pz + z + 0.5), 2); //Current Block distance
							dist2 = pow(orig.x - (px + tempPos.x + 0.5), 2) + pow(orig.y - (py + tempPos.y + 0.5), 2) + pow(orig.z - (pz + tempPos.z + 0.5), 2); //Current shortest block distance
							if (dist < dist2) {
								finalDist = dist;
								tempPos = glm::ivec3(x, y, z);
							}
						}
						else {
							airBlocks.push_back(glm::ivec3(x, y, z));
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < airBlocks.size(); i++) {
		dist = pow(orig.x - (px + airBlocks[i].x + 0.5), 2) + pow(orig.y - (py + airBlocks[i].y + 0.5), 2) + pow(orig.z - (pz + airBlocks[i].z + 0.5), 2);
		if (dist < finalDist) {
			if (dist > finalPosDist) {
				finalPosDist = dist;
				finalPos = airBlocks[i];
			}
		}
	}

	airBlocks.clear();

	if (finalPos.x < chunkSize) {
		cubes[finalPos.x + chunkSize * (finalPos.y + chunkSize * (finalPos.z))] = 7;
		tris.clear();
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		genVAO();
	}
}

glm::ivec3 chunk::getBlockPos(glm::vec3 &orig, glm::vec3 &dir) {

	glm::ivec3 tempPos(10000000.0f);
	glm::ivec3 finalPos(0.0f);

	float dist = 0;
	float dist2 = 0;
	float finalDist = 0; //Distance to closest block
	float finalPosDist = 0; //Distance to air block

	std::vector<glm::ivec3> airBlocks;

	for (int x = 0; x < chunkSize; x++) {
		for (int y = 0; y < chunkSize; y++) {
			for (int z = 0; z < chunkSize; z++) {
				if (glm::dot(orig - glm::vec3(px + x + 0.5, py + y + 0.5, pz + z + 0.5), dir) < 0.0f) { //If block is infront
					if (intersect(orig, dir, glm::vec3(x + px, y + py, z + pz), glm::vec3(x + px + 1, y + py + 1, z + pz + 1))) { //If the ray intersects the block
						if (cubes[x + chunkSize * (y + chunkSize * z)] != 0) {
							dist = pow(orig.x - (px + x + 0.5), 2) + pow(orig.y - (py + y + 0.5), 2) + pow(orig.z - (pz + z + 0.5), 2); //Current Block distance
							dist2 = pow(orig.x - (px + tempPos.x + 0.5), 2) + pow(orig.y - (py + tempPos.y + 0.5), 2) + pow(orig.z - (pz + tempPos.z + 0.5), 2); //Current shortest block distance
							if (dist < dist2) {
								finalDist = dist;
								tempPos = glm::ivec3(x, y, z);
							}
						}
						else {
							airBlocks.push_back(glm::ivec3(x, y, z));
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < airBlocks.size(); i++) {
		dist = pow(orig.x - (px + airBlocks[i].x + 0.5), 2) + pow(orig.y - (py + airBlocks[i].y + 0.5), 2) + pow(orig.z - (pz + airBlocks[i].z + 0.5), 2);
		if (dist < finalDist) {
			if (dist > finalPosDist) {
				finalPosDist = dist;
				finalPos = airBlocks[i];
			}
		}
	}

	airBlocks.clear();

	if (finalPos.x < chunkSize) {
		return finalPos + glm::ivec3(px, py, pz);
	}
	else {
		return glm::ivec3(666, 666, 666);
	}
}

bool chunk::intersect(glm::vec3 &orig, glm::vec3 &dir, glm::vec3 min, glm::vec3 max) {

	glm::vec3 invdir = glm::vec3(1.0f) / dir;
	int sign[3] = { invdir.x < 0, invdir.y < 0, invdir.z < 0 };

	//std::cout << sign[0] << " " << sign[1] << " " << sign[2] << "\n";

	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	
	glm::vec3 bounds[2] = { min, max };

	tmin = (bounds[sign[0]].x - orig.x) * invdir.x;
	tmax = (bounds[1-sign[0]].x - orig.x) * invdir.x;
	tymin = (bounds[sign[1]].y - orig.y) * invdir.y;
	tymax = (bounds[1-sign[1]].y - orig.y) * invdir.y;

	if ((tmin > tymax) || tymin > tmax) {
		return false;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}

	tzmin = (bounds[sign[2]].z - orig.z) * invdir.z;
	tzmax = (bounds[1-sign[2]].z - orig.z) * invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}

	return true;
}

bool chunk::intersectPlane(const glm::vec3 &n, const glm::vec3 &p0, const glm::vec3 &orig, const glm::vec3 &dir, float &t) {

	float denom = glm::dot(n, dir);
	if (denom > 1e-6) {
		glm::vec3 p0l0 = p0 - orig;
		t = glm::dot(p0l0, n) / denom;
		return (t >= 0);
	}

	return false;
}

bool chunk::pointInCube(glm::vec3 min, glm::vec3 max, glm::vec3 p) {
	return (p.x < max.x && p.x > min.x && p.y < max.y && p.y > min.y && p.z < max.z && p.z > min.z);
}

void chunk::genChunk() {

	//unsigned int seed = 237;
	//unsigned int seed = 2323167;
	//unsigned int seed = 213;
	PerlinNoise sn(seed);

	PerlinNoise biome(seed + 53 * 213 / 3);

	PerlinNoise tree(seed + 7 * 23 / 5);
	
	int scale = 3;

	std::vector<glm::ivec2> treePos;

	int lift = 0;

	for (int x = 0; x < chunkSize; x++) {
		for (int y = chunkSize-1; y > -1; y--) {
			for (int z = 0; z < chunkSize; z++) {

				float b = biome.noise(0.011 * (x + px), 0.011 * (z + pz), 0.011 /** (y + py)*/);
				if (b > 0.7) { //Sand Mountains
					b = biome.noise(0.011 * (x + px), 0.011 * (z + pz), 0.011 * (y + py));
					if (b > 0.75) {
						b = 4;
					}
					else {
						b = 1;
					}
				}
				else if (b > 0.5) { //Light forest
					b = 1; 
					if (pow(tree.noise(0.55 * (px + x), 0.55 * (pz + z), 1.0), 1) > 0.8) {
						treePos.push_back(glm::vec2(x, z));
					}
				}
				else if (b > 0.3) { //Heavy forest
					b = 1;
					if (pow(tree.noise(0.55 * (px + x), 0.55 * (pz + z), 1.0), 1) > 0.7) {
						treePos.push_back(glm::vec2(x, z));
					}
				}
				else { //Snow
					b = 5;
				}
				
				float h = 0.0f;
				//b = 1;
				if (b == 1) {
					h = 100 * ((0.11 * sn.noise(0.011 * (x + px), 0.011 * (z + pz), 0.25)));
					h = pow(h, 1.3);
					h = lift + h;
				}else if (b == 4) {
					h = 100 * ((0.11 * sn.noise(0.11 * (x + px), 0.11 * (z + pz), 0.25 * (y + py))));
					h = pow(h, 1.0);
					h = lift + h;
				}
				else {
					h = 50 * ((0.4 * sn.noise(0.25 * (x + px), 0.25 * (z + pz), 0.25)));
					h = pow(h, 1.0);
					h = lift + h;
				}

				int worldHeight = y + py;

				if (b != 4) {
					if (worldHeight == (int)h) {

						cubes[x + chunkSize * (y + chunkSize * z)] = (int)b;
					}
					else if (worldHeight < h && worldHeight > h - 3) {

						cubes[x + chunkSize * (y + chunkSize * z)] = 2;
					}
					else if (worldHeight < h - 2) {

						cubes[x + chunkSize * (y + chunkSize * z)] = 3;
					}
					else {
						cubes[x + chunkSize * (y + chunkSize * z)] = 0;
					}
				}
				else {
					//std::cout << h << "\n";
					if (4 < h) {

						cubes[x + chunkSize * (y + chunkSize * z)] = (int)b;
					}
					else {
						cubes[x + chunkSize * (y + chunkSize * z)] = 0;
					}
				}
			}
		}
	}
	for (int i = 0; i < treePos.size(); i++) {
		int h = 100 * ((0.11 * sn.noise(0.011 * (treePos[i].x + px), 0.011 * (treePos[i].y + pz), 0.25)));
		h = pow(h, 1.3);
		h = lift + h + 1;
		for (int treeHeight = 0; treeHeight < 7; treeHeight++) {
			if ((treeHeight + h) - py < chunkSize) {
				cubes[treePos[i].x + chunkSize * (((treeHeight + h) - py) + chunkSize * treePos[i].y)] = 8;
				if (treeHeight > 4) {
					for (int x = 0; x < 2; x++) {
						if (treePos[i].x + x < chunkSize) {
							cubes[(treePos[i].x + x) + chunkSize * (((treeHeight + h) - py) + chunkSize * treePos[i].y)] = 6;
						}
						if (treePos[i].x - x > -1) {
							cubes[(treePos[i].x - x) + chunkSize * (((treeHeight + h) - py) + chunkSize * treePos[i].y)] = 6;
						}
						for (int z = 0; z < 2; z++) {
							if (treePos[i].y + z < chunkSize) {
								cubes[treePos[i].x + chunkSize * (((treeHeight + h) - py) + chunkSize * (treePos[i].y+z))] = 6;
							}
							if (treePos[i].y - z > -1) {
								cubes[treePos[i].x + chunkSize * (((treeHeight + h) - py) + chunkSize * (treePos[i].y-z))] = 6;
							}
						}
					}
				}
			}
		}
	}

	genVAO();
}

void chunk::cleanUp() {

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//Resource_Manager::clearData();
	tris.clear();
	
	//delete cubes;
	//delete[]cubes;
}

void chunk::genVAO() {

	for (int x = 0; x < chunkSize; x++) {
		for (int y = 0; y < chunkSize; y++) {
			for (int z = 0; z < chunkSize; z++) {
				if (cubes[x + chunkSize * (y + chunkSize * z)] != 0) {
					int type = cubes[x + chunkSize * (y + chunkSize * z)] - 1;
					int typeb = floor(type / 3.0f);
					/*float texy = 1.0f;
					float texx = 1.0f;
					if (type < 4) {
						texy = (temp.y / 3.0f) + ((1.0f / 3.0f) * 2);
					}
					else if (type > 3) {
						texy = (temp.y / 3.0f) + ((1.0f / 3.0f));
					}
					if (type == 1 || type == 4) {
						texx = temp.x / 3.0f;
					}
					else if (type == 2 || type == 5) {
						texx = (temp.x / 3.0f) + (1.0f / 3.0f);
					}
					else if (type == 3 || type == 6) {
						ttexx = (temp.x / 3.0f) + ((1.0f / 3.0f) * 2);
					}*/
					if (y < chunkSize - 1) { //Check top and bottom
						if (cubes[x + chunkSize * ((y + 1) + chunkSize * z)] == 0) {
							tris.push_back({ glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f) });

							tris.push_back({ glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f) });
						}
					}
					else {
						tris.push_back({ glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f) });

						tris.push_back({ glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
							glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f),
							glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 1.0f, 0.0f) });
						
					}
					if (y > 0) {
						if (cubes[x + chunkSize * ((y - 1) + chunkSize * z)] == 0) {
							tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
								glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f) });

							tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f) });
						}
					}
					else {
						tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
								glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f) });

						tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
							glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f),
							glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, -1.0f, 0.0f) });
					}
					if (x < chunkSize - 1) { //Check Right and Left
						if (cubes[((x + 1) + chunkSize * (y + chunkSize * z))] == 0) {
							tris.push_back({ glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f) });

							tris.push_back({ glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f) });
						}
					}
					else {
						tris.push_back({ glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f) });

						tris.push_back({ glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
							glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f),
							glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(1.0f, 0.0f, 0.0f) });
					}
					if (x > 0) {
						if (cubes[(x - 1) + chunkSize * (y + chunkSize * z)] == 0) {
							tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
								glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f) });

							tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f) });
						}
					}
					else {
						tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
								glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f) });

						tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
							glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f),
							glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(-1.0f, 0.0f, 0.0f) });
					}
					if (z < chunkSize - 1) { //Check Front and back
						if (cubes[(x + chunkSize * (y + chunkSize * (z + 1)))] == 0) {
							tris.push_back({ glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f), });

							tris.push_back({ glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f), });
						}
					}
					else {
						tris.push_back({ glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f), });

						tris.push_back({ glm::vec3((float)x, (float)y, (float)z + 1.0f), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
							glm::vec3((float)x + 1.0f, (float)y, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f),
							glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, 1.0f), });
					}
					if (z > 0) {
						if (cubes[x + chunkSize * (y + chunkSize * (z - 1))] == 0) {
							tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f), });

							tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
								glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f) });
						}
					}
					else {
						tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
								glm::vec3((float)x, (float)y + 1.0f, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
								glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f), });

						tris.push_back({ glm::vec3((float)x, (float)y, (float)z), glm::vec2((0.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
							glm::vec3((float)x + 1.0f, (float)y + 1.0f, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (1.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f),
							glm::vec3((float)x + 1.0f, (float)y, (float)z), glm::vec2((1.0f / 3.0f) + ((float)type * (1.0f / 3.0f)), (0.0f / 3.0f) + ((1.0f / 3.0f) * (2 - typeb))), glm::vec3(0.0f, 0.0f, -1.0f) });
					}
				}
			}
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * tris.size(), &tris[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
}