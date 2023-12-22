#pragma once

//local
#include "B3DModel.h"

//std
#include <memory>

//GLM
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent
{
	glm::vec3 translation{};
	glm::vec3 scale{1.f, 1.f, 1.f};
	glm::vec3 rotation{};

	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

class B3DGameObj
{
	public:
		
		using id_t = unsigned int;

		std::shared_ptr<B3DModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

		static B3DGameObj createGameObject()
		{
			static id_t currentId = 0;
			
			return B3DGameObj{ currentId++ };
		}

		B3DGameObj(const B3DGameObj&) = delete;
		B3DGameObj& operator=(const B3DGameObj&) = delete;
		B3DGameObj(B3DGameObj&&) = default;
		B3DGameObj& operator=(B3DGameObj&&) = default;

		id_t getId() { return id; }

	private:

		id_t id;

		B3DGameObj(id_t objId) : id{ objId } {}

};