#pragma once
#include "../shader/Shader.h";
#include "../model/Vao.h";
#include "../texture/Texture.h"
#include "Material.h"
#include <vector>

namespace glp {
	class Entity {
		float x;
		float y;
		float z;
		float scale;
		Material material;
		std::vector<Entity> children;
		std::vector<Texture*> textures;

	public:
		Entity(Vao* model, Shader* shader, float scale);

		Vao* model;

		Shader& getShader();
		void setShader(Shader& shader);

		Vao& getModel();
		void setModel(Vao* model);

		std::vector<Texture*> getTextures();
		void addTexture(Texture* texture);

		void setX(float x);
		float getX();

		void setY(float y);
		float getY();

		void setZ(float z);
		float getZ();

		void setScale(float scale);
		float getScale();

		Material getMaterial();
		void setMaterial(Material material);

		std::vector<Entity> getChildren();
		void addChild(Entity entity);
	};
}