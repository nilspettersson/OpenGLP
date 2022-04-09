#include "Vao.h"
#include <iostream>
#include <GL/glew.h>
#include <vector>

using namespace glp;

Vao::Vao(std::vector<int> vertexLayout, const unsigned int* indices, int indicesCount, const float* vertices, int verticesCount, bool dynamic) {
	this->vertexLayout = vertexLayout;

	this->indicesCount = indicesCount;
	this->verticesCount = verticesCount;
	this->dynamic = dynamic;

	Vao::setVertexSize();

	GLenum drawType;
	if (this->dynamic) {
		drawType = GL_DYNAMIC_DRAW;
	}
	else {
		drawType = GL_STATIC_DRAW;
	}

	glGenBuffers(1, &this->vaoArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * this->vertexSize, vertices, drawType);

	glGenBuffers(1, &this->vaoElementId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned int), indices, drawType);


	int offset = 0;
	for (int i = 0; i < this->vertexLayout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, this->vertexLayout[i], GL_FLOAT, GL_FALSE, this->vertexSize, (const void*) offset);
		offset += this->vertexLayout[i] * 4;
	}
}

glp::Vao::Vao(Mesh mesh, bool dynamic) {
	this->vertexLayout = mesh.vertexLayout;

	float* vertices = &mesh.vertices[0];
	unsigned int* indices = &mesh.indices[0];

	this->indicesCount = mesh.indices.size();
	this->verticesCount = mesh.getVerticesCount();
	this->dynamic = dynamic;


	Vao::setVertexSize();

	GLenum drawType;
	if (this->dynamic) {
		drawType = GL_DYNAMIC_DRAW;
	}
	else {
		drawType = GL_STATIC_DRAW;
	}

	glGenBuffers(1, &this->vaoArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * this->vertexSize, vertices, drawType);

	glGenBuffers(1, &this->vaoElementId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned int), indices, drawType);


	int offset = 0;
	for (int i = 0; i < this->vertexLayout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, this->vertexLayout[i], GL_FLOAT, GL_FALSE, this->vertexSize, (const void*)offset);
		offset += this->vertexLayout[i] * 4;
	}
}

void Vao::setVertexSize() {
	this->vertexSize = 0;
	for (int i = 0; i < this->vertexLayout.size(); i++) {
		this->vertexSize += this->vertexLayout[i] * 4;
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

void glp::Vao::updateVertices(int offset, int size, const float* data) {
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void glp::Vao::setVertices(int size, const float* data) {
	glBindBuffer(GL_ARRAY_BUFFER, this->vaoArrayId);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void glp::Vao::updateIndices(int offset, int size, const float* data) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void glp::Vao::setIndices(int size, const float* data) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vaoElementId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}
