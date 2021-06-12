#include "Vao.h"
#include <iostream>

using namespace glp;

Vao::Vao(std::vector<VertexLayout> vertexLayout, const unsigned int* indices, int indicesCount, const float* vertices, int verticesCount) {
	this->vertexLayout = vertexLayout;

	this->indicesCount = indicesCount;
	this->verticesCount = verticesCount;

	Vao::setVertexSize();

	glGenBuffers(1, &this->vaoArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * Vao::vertexSize, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &this->vaoElementId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);


	int offset = 0;
	for (int i = 0; i < this->vertexLayout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, this->vertexLayout[i].attribCount, (GLenum)this->vertexLayout[i].dataType, GL_FALSE, this->vertexSize, (const void*) offset);
		if (this->vertexLayout[i].dataType == DataType::BYTE) {
			offset += this->vertexLayout[i].attribCount * 1;
		}
		else if (this->vertexLayout[i].dataType == DataType::SHORT) {
			offset += this->vertexLayout[i].attribCount * 2;
		}
		else {
			offset += this->vertexLayout[i].attribCount * 4;
		}
	}
}

void Vao::setVertexSize() {
	this->vertexSize = 0;
	for (int i = 0; i < this->vertexLayout.size(); i++) {
		this->vertexSize += this->vertexLayout[i].attribCount * sizeof(float);
	}
}

void Vao::render() {
	glDrawElements(GL_TRIANGLES, this->indicesCount, GL_UNSIGNED_INT, nullptr);
}

void glp::Vao::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
}

unsigned int Vao::getVaoArrayId() {
	return this->vaoArrayId;
}

unsigned int Vao::getVaoElementId() {
	return this->vaoElementId;
}

int Vao::getVertexSize() {
	return this->vertexSize;
}



glp::VertexLayout::VertexLayout(unsigned int attribCount, DataType dataType) {
	this->attribCount = attribCount;
	this->dataType = dataType;
}
