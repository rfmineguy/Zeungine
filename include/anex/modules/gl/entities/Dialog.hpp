#pragma once
#include <anex/modules/gl/GLEntity.hpp>
#include <anex/modules/gl/GLScene.hpp>
#include <anex/modules/gl/GLWindow.hpp>
#include "./TextView.hpp"
#include <anex/glm.hpp>
#include <anex/modules/gl/fonts/freetype/Freetype.hpp>

namespace anex::modules::gl::entities
{
	struct Dialog : GLEntity
	{
		std::vector<glm::vec4> colors;
		GLScene& scene;
		glm::vec2 size;
		glm::vec4 color;
		fonts::freetype::FreetypeFont& font;
		std::string title;
		float width;
		float height;
		float fontSize;
		std::shared_ptr<TextView> titleTextView;
		inline static size_t dialogsCount = 0;
		Dialog(GLWindow& window,
							GLScene& scene,
							const glm::vec3& position,
							const glm::vec3& rotation,
							const glm::vec3& scale,
							const glm::vec4& color,
							fonts::freetype::FreetypeFont& font,
							std::string_view title,
							float width,
							float height,
              const std::vector<std::shared_ptr<GLEntity>> &children = {},
							const shaders::RuntimeConstants& constants = {},
							std::string_view name = "");
		void preRender() override;
		void setColor(const glm::vec4& color);
		void setSize(const glm::vec2 &newSize);
	};
}
