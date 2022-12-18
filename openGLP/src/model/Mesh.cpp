#include "Mesh.h"

using namespace glp;

const std::vector<int>glp::Mesh::DefaultVertexLayout({ 3, 2, 3 });

glp::Mesh::Mesh(std::vector<int> vertexLayout, std::vector<float> vertices, std::vector<unsigned int> indices) {
	this->vertices = vertices;
	this->indices = indices;
	this->vertexLayout = vertexLayout;
}

int glp::Mesh::getVerticesCount() {
	int vertexSize = 0;
	for (int i = 0; i < vertexLayout.size(); i++) {
		vertexSize += vertexLayout[i];
	}
	return vertices.size() / vertexSize;
}

Mesh glp::Mesh::loadModelFromObj(const std::string& filepath) {
	std::vector<float> positions = std::vector<float>();
	std::vector<float> texCoords = std::vector<float>();
	std::vector<float> normals = std::vector<float>();

	std::vector<float> vertices = std::vector<float>();

	std::ifstream stream(filepath);
	std::string line;
	while (std::getline(stream, line)) {
		if (line[0] == 'v' && line[1] == ' ') {
			int startIndex = 2;
			for (int i = 0; i < 3; i++) {
				int endIndex = line.find_first_of(' ', startIndex);
				float position = std::stof(line.substr(startIndex, endIndex));
				positions.push_back(position);

				startIndex = endIndex + 1;
			}
			
		}
		else if (line[0] == 'v' && line[1] == 't') {
			int startIndex = 3;
			for (int i = 0; i < 2; i++) {
				int endIndex = line.find_first_of(' ', startIndex) - 1;
				float texCoord = std::stof(line.substr(startIndex, endIndex));
				texCoords.push_back(texCoord);

				startIndex = endIndex + 1;
			}
		}
		else if (line[0] == 'v' && line[1] == 'n') {
			int startIndex = 3;
			for (int i = 0; i < 3; i++) {
				int endIndex = line.find_first_of(' ', startIndex);
				float normal = std::stof(line.substr(startIndex, endIndex));
				normals.push_back(normal);

				startIndex = endIndex + 1;
			}
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			int startIndex = 2;
			for (int i = 0; i < 3; i++) {
				int endIndex = line.find_first_of(' ', startIndex);
				std::string triangle = line.substr(startIndex, endIndex);

				int triangleStartIndex = 0;
				for (int ii = 0; ii < 3; ii++) {
					int triangleEndIndex = 0;
					if (ii != 2) {
						triangleEndIndex = triangle.find_first_of('/', triangleStartIndex);
					}
					else {
						triangleEndIndex = triangle.size() - 1;
					}
					
					if (ii == 0) {
						int index = std::stoi(triangle.substr(triangleStartIndex, triangleEndIndex)) * 3 - 3;
						vertices.push_back(positions[index]);
						vertices.push_back(positions[index + 1]);
						vertices.push_back(positions[index + 2]);
					}
					else if (ii == 1) {
						int index = std::stoi(triangle.substr(triangleStartIndex, triangleEndIndex)) * 2 - 2;
						vertices.push_back(texCoords[index]);
						vertices.push_back(texCoords[index + 1]);
					}
					else {
						int index = std::stoi(triangle.substr(triangleStartIndex, triangleEndIndex)) * 3 - 3;
						vertices.push_back(normals[index]);
						vertices.push_back(normals[index + 1]);
						vertices.push_back(normals[index + 2]);
					}

					triangleStartIndex = triangleEndIndex + 1;
				}

				startIndex = endIndex + 1;
			}
		}
	}
	std::vector<unsigned int> indices = std::vector<unsigned int>();
	for (int i = 0; i < vertices.size() / 3; i++) {
		indices.push_back(i);
	}

	return Mesh(DefaultVertexLayout, vertices, indices);
}

Mesh glp::Mesh::Primitives::Plane(float width, float height) {
	std::vector<float> vertices = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

	};
	std::vector<unsigned int> indices {
		0, 1, 2,
		2, 3, 0,
	};
	return glp::Mesh(DefaultVertexLayout, vertices, indices);
}
