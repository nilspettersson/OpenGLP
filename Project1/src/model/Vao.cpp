#include "Vao.h"
#include <iostream>

Vao::Vao(std::vector<int> vertexLayout, bool dynamicDraw) {
	this->dynamicDraw = dynamicDraw;
	this->vertexLayout = vertexLayout;

	this->vertexSize = 0;

	float positions[] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
	};

	unsigned int indices[]{
		0, 1, 2,
		2, 3, 0
	};

	/*for (int i = 0; i < this->vertexLayout.size(); i++) {
		std::cout << "array: " << this->vertexLayout[i] << std::endl;
	}*/

	glGenBuffers(1, &this->buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

	for (int i = 0; i < this->vertexLayout.size(); i++) {
		this->vertexSize += this->vertexLayout[i] * sizeof(float);
	}

	for (int i = 0; i < this->vertexLayout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, this->vertexLayout[i], GL_FLOAT, GL_FALSE, this->vertexSize, 0);
	}

	/*glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);*/

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}
