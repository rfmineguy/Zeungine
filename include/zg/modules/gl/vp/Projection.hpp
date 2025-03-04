#pragma once
#include <zg/glm.hpp>
namespace zg::modules::gl
{
	struct RenderWindow;
}
namespace zg::modules::gl::vp
{
	struct Projection
  {
    RenderWindow &window;
    float fov = 0;
    glm::vec2 orthoSize = glm::vec2(0.0f);
    bool isOrthographic = false;
		float nearPlane = 0;
		float farPlane = 0;
    glm::mat4 matrix = glm::mat4(1.0f);
    Projection(RenderWindow &window, float fov);
		Projection(RenderWindow &window, glm::vec2 orthoSize);
    void update();
  };
}