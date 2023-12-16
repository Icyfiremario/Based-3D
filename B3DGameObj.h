#pragma once

//local
#include "B3DModel.h"

//std
#include <memory>

struct Transform2dComponent
{
	glm::vec2 translation{};
	glm::vec2 scale{1.f, 1.f};
	float rotation;

	glm::mat2 mat2()
	{
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);
		glm::mat2 rotMatrix{ {c, s}, {-s, c} };

		glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
		return rotMatrix * scaleMat;
	}
};

class B3DGameObj
{
	public:
		
		using id_t = unsigned int;

		std::shared_ptr<B3DModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d;

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