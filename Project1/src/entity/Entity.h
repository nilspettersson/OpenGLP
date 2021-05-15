#pragma once
#include "../shader/Shader.h";
#include "../model/Vao.h";
#include "../texture/Texture.h"

namespace glp {
	class Entity {
		float x;
		float y;
		float z;
		Shader* shader;
		Vao* model;
		Texture* texture;

	public:
		Entity(Vao* model, Shader* shader);

		Shader* getShader();
		void setShader(Shader* shader);

		Vao* getModel();
		void setModel(Vao* texture);

		Texture* getTexture();
		void setTexture(Texture* texture);

		void setX(float x);
		float getX();

		void setY(float y);
		float getY();

		void setZ(float z);
		float getZ();
	};
}