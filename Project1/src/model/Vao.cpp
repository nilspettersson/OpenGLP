#include "Vao.h"
#include <iostream>

Vao::Vao(std::vector<int> vertexLayout, unsigned int indices[], float vertices[]) {
	this->vertexLayout = vertexLayout;
	Vao::setVertexSize();

	glGenBuffers(1, &this->vaoArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &this->vaoElementId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


	for (int i = 0; i < this->vertexLayout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, this->vertexLayout[i], GL_FLOAT, GL_FALSE, this->vertexSize, 0);
	}
}

void Vao::setVertexSize() {
	this->vertexSize = 0;
	for (int i = 0; i < this->vertexLayout.size(); i++) {
		this->vertexSize += this->vertexLayout[i] * sizeof(float);
	}
}