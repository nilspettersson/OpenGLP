#include "Renderer.h"

using namespace glp;
Renderer::Renderer(Camera2d* camera) {
	this->camera = camera;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void Renderer::render(Entity* entity) {
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(entity->getX(), entity->getY(), entity->getZ()));
	model = glm::scale(model, glm::vec3(entity->getScale()));
	entity->getShader()->setUniformMat4f("u_mvp", this->camera->getProjection() * model);
	entity->getTexture()->bind();
	entity->getShader()->bind();
	entity->getModel()->bind();
	entity->getModel()->render();
}

