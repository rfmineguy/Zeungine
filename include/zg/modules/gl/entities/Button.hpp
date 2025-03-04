#pragma once
#include <zg/modules/gl/GLEntity.hpp>
#include <zg/modules/gl/GLScene.hpp>
#include <zg/modules/gl/RenderWindow.hpp>
#include "./TextView.hpp"
#include <zg/glm.hpp>
#include <zg/modules/gl/fonts/freetype/Freetype.hpp>

namespace zg::modules::gl::entities
{
	struct Button : GLEntity
	{
		std::vector<glm::vec4> colors;
		GLScene& scene;
		glm::vec2 size;
		std::string text;
		std::shared_ptr<TextView> textView;
		fonts::freetype::FreetypeFont& font;
    using OnClickHandler = std::function<void()>;
    OnClickHandler handler;
		RenderWindow::EventIdentifier mouseHoverID = 0;
		RenderWindow::EventIdentifier mousePressID = 0;
		inline static size_t buttonsCount = 0;
		Button(RenderWindow& window,
							GLScene& scene,
							glm::vec3 position,
							glm::vec3 rotation,
							glm::vec3 scale,
							glm::vec4 color,
              glm::vec2  size,
							std::string_view text,
							fonts::freetype::FreetypeFont& font,
							const OnClickHandler& handler,
							const shaders::RuntimeConstants& constants = {},
							std::string_view name = "");
		~Button() override;
		void preRender() override;
		void setColor(glm::vec4 color);
		void setSize(glm::vec2  size);
	};
}
