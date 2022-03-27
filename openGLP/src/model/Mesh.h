#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"

namespace glp {
	class Mesh {

	public:
		std::vector<int> vertexLayout;
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		Mesh(std::vector<int> vertexLayout, std::vector<float> vertices, std::vector<unsigned int> indices);
		int getVerticesCount();

		static Mesh loadModelFromObj(const std::string& filepath);
	};
}