#pragma once
#include <zg/modules/gl/GLEntity.hpp>
#include <zg/modules/gl/GLScene.hpp>
#include <zg/modules/gl/RenderWindow.hpp>
#include "./TextView.hpp"
#include <zg/glm.hpp>
#include <zg/modules/gl/fonts/freetype/Freetype.hpp>

namespace zg::modules::gl::entities
{
	struct Tab;
	struct TabsBar : GLEntity
	{
		std::vector<glm::vec4> colors;
		GLScene& scene;
		glm::vec2 size;
		glm::vec4 color;
		fonts::freetype::FreetypeFont& font;
		float width;
		float height;
		std::shared_ptr<TextView> titleTextView;
		inline static size_t tabBarsCount = 0;
		using TabClickHandler = std::function<void()>;
		TabsBar(RenderWindow& window,
							GLScene& scene,
							glm::vec3 position,
							glm::vec3 rotation,
							glm::vec3 scale,
							glm::vec4 color,
							fonts::freetype::FreetypeFont& font,
							float width,
							float height,
							const shaders::RuntimeConstants& constants = {},
							std::string_view name = "");
		void addTab(std::string_view name, const TabClickHandler &handler, bool active = false);
		void preRender() override;
		void setColor(glm::vec4 color);
		void setSize();
		void markInactive(Tab *activeTab);
	};

	struct Tab : GLEntity
	{
		std::vector<glm::vec4> colors;
		GLScene& scene;
		glm::vec2 size;
		std::string text;
		std::shared_ptr<TextView> textView;
		fonts::freetype::FreetypeFont& font;
		float height;
		float NDCHeight;
		TabsBar::TabClickHandler handler;
		RenderWindow::EventIdentifier mouseHoverID = 0;
		RenderWindow::EventIdentifier mousePressID = 0;
		bool active;
		bool hovered = false;
		glm::vec4 activeColor;
		glm::vec4 inactiveColor;
		TabsBar &tabsBar;
		inline static size_t tabsCount = 0;
		Tab(RenderWindow& window,
							GLScene& scene,
							glm::vec3 position,
							glm::vec3 rotation,
							glm::vec3 scale,
							glm::vec4 color,
							std::string_view text,
							fonts::freetype::FreetypeFont& font,
							float height,
							const TabsBar::TabClickHandler &handler,
							bool active,
							TabsBar &tabsBar,
							const shaders::RuntimeConstants& constants = {},
							std::string_view name = "");
		~Tab() override;
		void preRender() override;
		void setColor(glm::vec4 color);
		void setSize(glm::vec2  size);
		void markInactive();
	};
}
