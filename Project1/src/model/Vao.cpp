#include "Vao.h"
#include <iostream>

Vao::Vao(std::vector<int> vertexLayout) {
	this->vertexLayout = vertexLayout;

	Vao::setVertexSize();

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


	glGenBuffers(1, &this->vaoArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);


	for (int i = 0; i < this->vertexLayout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, this->vertexLayout[i], GL_FLOAT, GL_FALSE, this->vertexSize, 0);
	}

	glGenBuffers(1, &this->vaoElementId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

void Vao::setVertexSize() {
	this->vertexSize = 0;
	for (int i = 0; i < this->vertexLayout.size(); i++) {
		this->vertexSize += this->vertexLayout[i] * sizeof(float);
	}
}
