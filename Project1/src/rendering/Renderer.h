#pragma once
#include "../model/Vao.h"

namespace glp {
	class Renderer {

	public:
		Renderer();

		void render(Vao* vao);
	};
}