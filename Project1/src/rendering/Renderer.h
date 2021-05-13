#pragma once
#include <vector>
#include "../shader/Shader.h"
#include "../model/Vao.h"
#include "../entity/Entity.h"

namespace glp {
	class Renderer {

	public:
		Renderer();

		void render(Entity* entity);

	};
}