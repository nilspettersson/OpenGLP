#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../window/Window.h"

namespace glp {
	class Camera {
	protected:
		float width;
		float height;
		glm::mat4 projection;
		float x;
		float y;
		float z;
		float fov;

	public:
		Camera(int width, int height);
		virtual glm::mat4 getProjection();
		float getWidth();
		float getHeight();
		void setWidth(float width);
		void setHeight(float height);

		float getX();
		float getY();
		void setX(float x);
		void setY(float y);
	};


	class Camera2d : public Camera {
	public:
		Camera2d(Window& window);
		Camera2d(int width, int height);
	};

	class Camera3d : public Camera {
	
	protected:
		void updateProjection();
	public:
		glm::vec3 rotation;
		Camera3d(Window& window);
		Camera3d(int width, int height);
		glm::mat4 getProjection();
		float getFov();
		void setFov(float fovInDegrees);

		float getZ();
		void setZ(float z);

		void rotateX(float);
		void rotateY(float);

		void moveForward(float amount);
		void moveBackward(float amount);
		void moveLeft(float amount);
		void moveRight(float amount);
		void moveUp(float amount);
		void moveDown(float amount);
	};
}