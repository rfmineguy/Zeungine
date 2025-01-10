#include <anex/modules/gl/textures/Texture.hpp>
#include <anex/modules/gl/textures/TextureFactory.hpp>
using namespace anex::modules::gl::textures;
Texture::Texture(const glm::ivec4 &size, const void *data, const Format &format, const Type &type, const FilterType &filterType):
	size(size),
  format(format),
  type(type),
  filterType(filterType)
{
  TextureFactory::initTexture(*this, data);
};
Texture::Texture(const glm::ivec4 &size, const std::string_view &path, const Format &format, const Type &type, const FilterType &filterType):
  size(size),
  format(format),
  type(type),
  filterType(filterType)
{
  TextureFactory::initTexture(*this, path);
};
Texture::Texture(const glm::ivec4 &size, const std::vector<std::string_view> &paths, const Format &format, const Type &type, const FilterType &filterType):
  size(size),
  format(format),
  type(type),
  filterType(filterType)
{
  TextureFactory::initTexture(*this, paths);
};
void Texture::bind() const
{
  glBindTexture(target, id);
  GLcheck("glBindTexture");
};
void Texture::unbind() const
{
  glBindTexture(target, 0);
  GLcheck("glBindTexture");
};