#pragma once
#include <vector>
#include "../shader/Shader.h"
#include "../model/Vao.h"
#include "../entity/Entity.h"
#include "../camera/Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace glp {
	class Renderer {
		Camera& camera;
		unsigned int currentShaderId;

		void renderChild(Entity& entity, glm::vec3 parentPosition, float parentScale);
	public:
		Renderer(Camera& camera);
		void render(Entity& entity);
	};
}