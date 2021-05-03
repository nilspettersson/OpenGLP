#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace glp {
	class Camera2d {
		float width;
		float height;
		glm::mat4 projection;
		float x;
		float y;
	public:
		Camera2d(int width, int height);
		glm::mat4 getProjection();
		int getWidth();
		int getHeight();

		int getX();
		int getY();
		void setX(float x);
		void setY(float y);

	};
}