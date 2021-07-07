#pragma once
#include "../shader/Shader.h";
#include "../model/Vao.h";
#include "../texture/Texture.h"
#include "Material.h"

namespace glp {
	class Entity {
		float x;
		float y;
		float z;
		float scale;
		Material material;
		Vao* model;
		Texture* texture;

	public:
		Entity(Vao* model, Shader* shader, float scale);

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

		void setScale(float scale);
		float getScale();

		Material* getMaterial();
		void setMaterial(Material material);
	};
}