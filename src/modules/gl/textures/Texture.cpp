#include <zg/modules/gl/renderers/GLRenderer.hpp>
#include <zg/modules/gl/textures/Texture.hpp>
#include <zg/modules/gl/textures/TextureFactory.hpp>
using namespace zg::modules::gl::textures;
Texture::Texture(RenderWindow &window, const glm::ivec4 &size, const void *data, const Format &format, const Type &type, const FilterType &filterType):
  window(window),
	size(size),
  format(format),
  type(type),
  filterType(filterType)
{
  TextureFactory::initTexture(*this, data);
};
Texture::Texture(RenderWindow &window, const glm::ivec4 &size, const std::string_view path, const Format &format, const Type &type, const FilterType &filterType):
  window(window),
  size(size),
  format(format),
  type(type),
  filterType(filterType)
{
  TextureFactory::initTexture(*this, path);
};
Texture::Texture(RenderWindow &window, const glm::ivec4 &size, const std::vector<std::string_view> &paths, const Format &format, const Type &type, const FilterType &filterType):
  window(window),
  size(size),
  format(format),
  type(type),
  filterType(filterType)
{
  TextureFactory::initTexture(*this, paths);
};
Texture::~Texture()
{
  TextureFactory::destroyTexture(*this);
};
void Texture::bind() const
{
	auto &glRenderer = *std::dynamic_pointer_cast<GLRenderer>(window.iVendorRenderer);
  glRenderer.glContext->BindTexture(target, id);
  GLcheck(glRenderer, "glBindTexture");
};
void Texture::unbind() const
{
	auto &glRenderer = *std::dynamic_pointer_cast<GLRenderer>(window.iVendorRenderer);
  glRenderer.glContext->BindTexture(target, 0);
  GLcheck(glRenderer, "glBindTexture");
};