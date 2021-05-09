#include "Renderer.h"

using namespace glp;
Renderer::Renderer() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void Renderer::render(Vao* vao) {
	vao->bind();
	vao->render();
}

