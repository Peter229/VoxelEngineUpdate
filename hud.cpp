#include "hud.h"

hud::hud(glm::mat4 model) {
	model = glm::scale(model, glm::vec3(0.002f * 9, 0.002f * 16, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0));
	this->model = model;
	genVAO();
}

void hud::render(Shader *shader) {
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, Resource_Manager::getTexture("crosshair"));
	shader->setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void hud::cleanUp() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void hud::genVAO() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlane), verticesPlane, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPlane), indicesPlane, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}