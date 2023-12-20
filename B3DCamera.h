#pragma once

//STD
#include <cassert>
#include <limits>

//GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class B3DCamera
{
	public:

		void setOrthoGraphicProjection(float left, float right, float top, float bottom, float near, float far);
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		const glm::mat4& getProjection() const { return projectionMatrix; }

	private:

		glm::mat4 projectionMatrix{1.f};
};