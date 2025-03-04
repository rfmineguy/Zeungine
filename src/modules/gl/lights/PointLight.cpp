#include <zg/modules/gl/lights/PointLight.hpp>
#include <zg/modules/gl/shaders/Shader.hpp>
#include <zg/modules/gl/shaders/ShaderManager.hpp>
#include <iostream>
using namespace zg::modules::gl::lights;
PointLightShadow::PointLightShadow(RenderWindow &window, PointLight &pointLight):
	window(window),
	shader(
    *shaders::ShaderManager::getShaderByConstants(window, {
      "Color",
			"Position",
			"Normal",
			"Model",
			"PointLightSpaceMatrix"
      }, {
        shaders::Shader::ShaderType::Vertex,
			 	shaders::Shader::ShaderType::Geometry,
			 	shaders::Shader::ShaderType::Fragment
      }
 		).second
	),
	pointLight(pointLight),
  texture(window, glm::ivec4(2048, 2048, 1, 1), 0, textures::Texture::Depth, textures::Texture::Float),
	framebuffer(window, texture)
{
	updateShadowTransforms();
};
void PointLightShadow::updateShadowTransforms()
{
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, pointLight.nearPlane, pointLight.farPlane);
	shadowTransforms[0] = projection * glm::lookAt(pointLight.position, pointLight.position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	shadowTransforms[1] = projection * glm::lookAt(pointLight.position, pointLight.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	shadowTransforms[2] = projection * glm::lookAt(pointLight.position, pointLight.position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0,  0.0, 1.0));
	shadowTransforms[3] = projection * glm::lookAt(pointLight.position, pointLight.position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0,  0.0, -1.0));
	shadowTransforms[4] = projection * glm::lookAt(pointLight.position, pointLight.position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
	shadowTransforms[5] = projection * glm::lookAt(pointLight.position, pointLight.position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0, -1.0, 0.0));
};