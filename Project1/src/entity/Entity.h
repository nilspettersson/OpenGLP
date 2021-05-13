#pragma once
#include "../shader/Shader.h";
#include "../model/Vao.h";

namespace glp {
	class Entity {
		Shader* shader;
		Vao* model;
	public:
		Entity(Vao* model, Shader* shader);

		Shader* getShader();
		void setShader(Shader* shader);

		Vao* getModel();
		void setModel(Vao* model);
	};
}