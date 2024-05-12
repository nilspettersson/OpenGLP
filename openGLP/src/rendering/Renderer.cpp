#include "Renderer.h"
#include <GLFW/glfw3.h>

using namespace glp;
Renderer::Renderer(Camera& camera): camera(camera) {
	this->currentShaderId = 0;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void Renderer::render(Entity& entity) {
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(entity.getX(), entity.getY(), entity.getZ()));
	model = glm::scale(model, glm::vec3(entity.getScale()));
	entity.getShader().setUniformMat4f("u_p", this->camera.getProjection());
	entity.getShader().setUniformMat4f("u_model", model);
	entity.getShader().setUniformMat4f("u_mv", this->camera.getViewMatrix() * model);

	entity.getMaterial().updateUniforms();

	for(int i = 0; i < entity.getTextures().size(); i++) {
		entity.getTextures()[i]->bind(i + 1);
	}
	
	if (this->currentShaderId != entity.getShader().getShaderId()) {
		entity.getShader().bind();
		this->currentShaderId = entity.getShader().getShaderId();
	}
	entity.getModel().bind();
	entity.getModel().render();

	for (int i = 0; i < entity.getChildren().size(); i++) {
		renderChild(entity.getChildren()[i], glm::vec3(entity.getX(), entity.getY(), entity.getZ()), entity.getScale());
	}
}

void glp::Renderer::renderChild(Entity& entity, glm::vec3 parentPosition, float parentScale) {
	if (&entity == nullptr) return;
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(entity.getX(), entity.getY(), entity.getZ()) + parentPosition);
	model = glm::scale(model, glm::vec3(entity.getScale()) * parentScale);

	entity.getShader().setUniformMat4f("u_p", this->camera.getProjection());
	entity.getShader().setUniformMat4f("u_model", model);
	entity.getShader().setUniformMat4f("u_mv", this->camera.getViewMatrix() * model);

	entity.getMaterial().updateUniforms();

	for (int i = 0; i < entity.getTextures().size(); i++) {
		entity.getTextures()[i]->bind(i + 1);
	}

	if (this->currentShaderId != entity.getShader().getShaderId()) {
		entity.getShader().bind();
		this->currentShaderId = entity.getShader().getShaderId();
	}
	entity.getModel().bind();
	entity.getModel().render();

	for (int i = 0; i < entity.getChildren().size(); i++) {
		renderChild(entity.getChildren()[i], glm::vec3(entity.getX(), entity.getY(), entity.getZ()), entity.getScale());
	}
}
