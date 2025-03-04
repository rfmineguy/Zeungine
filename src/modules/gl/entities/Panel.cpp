#include <zg/modules/gl/entities/Panel.hpp>
#include <zg/utilities.hpp>
#include <iostream>
using namespace zg::modules::gl::entities;
PanelMenu::PanelMenu(zg::modules::gl::RenderWindow &window,
				             zg::modules::gl::GLScene &scene,
				             glm::vec3 position,
				             glm::vec3 rotation,
				             glm::vec3 scale,
				             glm::vec4 color,
				             fonts::freetype::FreetypeFont &font,
										 const std::string_view title,
										 float width,
										 float height,
				             const zg::modules::gl::shaders::RuntimeConstants &constants,
                     std::string_view name):
	zg::modules::gl::GLEntity(
		window,
		zg::mergeVectors<std::string_view>({
			{
				"Color", "Position",
				"View", "Projection", "Model", "CameraPosition"
			}
		}, constants),
		6,
		{0, 1, 2,  2, 3, 0},
    4,
		{
			{ 0, -0, 0 }, { 0, -0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }
		},
		position,
		rotation,
		scale,
		name.empty() ? "PanelMenu " + std::to_string(++panelMenusCount) : name
	),
	scene(scene),
  color(color),
	font(font),
	title(title),
	width(width),
	height(height)
{
	updateIndices(indices);
	setColor(color);
	updateElements("Position", positions);
	PanelMenu::setSize(glm::vec3(0));
	addToBVH = false;
	if (title.empty())
		return;
  float titleLineHeight = 0;
	auto titleSize = font.stringSize(title, window.windowHeight / 30.f, titleLineHeight, glm::vec2(0));
	titleSize.y /= window.windowHeight * 0.5f;
	titleSize.x /= window.windowWidth * 0.5f;
	static auto indent = 8 / window.windowWidth / 0.5;
	titleTextView = std::make_shared<TextView>(
		window,
		scene,
		glm::vec3(titleSize.x / 2 + indent, -titleSize.y / 2, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(1, 1, 1, 1),
		title,
		titleSize,
		font,
		window.windowHeight / 36.f,
		true,
		[](auto titleSize)
		{
			return glm::vec3(titleSize.x / 2 + indent, -titleSize.y / 2, 0.1);
		},
		TextView::ResizeHandler(),
		[&]
		{
			return this->window.windowHeight / 36.f;
		});
  titleTextView->addToBVH = false;
  addChild(titleTextView);
};
void PanelMenu::addPanelEntity(const std::shared_ptr<GLEntity> &entity, bool alignSizeX)
{
  addChild(entity);
  scene.postAddEntity(entity, {ID, entity->ID});
	if (alignSizeX)
	{
		float sizeXMax = 0;
		for (auto &child : children)
		{
			auto sizablePointer = std::dynamic_pointer_cast<ISizable>(child.second);
			if (sizablePointer)
			{
				sizeXMax = std::max(sizablePointer->size.x, sizeXMax);
			}
		}
		for (auto &child : children)
		{
			auto sizablePointer = std::dynamic_pointer_cast<ISizable>(child.second);
	    if (!sizablePointer)
	      continue;
	    auto &childItem = *sizablePointer;
			childItem.setSize({sizeXMax, childItem.size.y, childItem.size.z});
			auto glEntityPointer = std::dynamic_pointer_cast<GLEntity>(sizablePointer);
	    scene.bvh.updateEntity(*glEntityPointer);
		}
	}
	setSize(glm::vec3(0));
}
void PanelMenu::removePanelEntity(const std::shared_ptr<GLEntity> &entity)
{
	scene.preRemoveEntity(entity, {ID, entity->ID});
	removeChild(entity->ID);
}
float PanelMenu::getSizeYTotal()
{
	float sizeYTotal = 0;
	for (auto &child : children)
	{
		auto textViewPointer = std::dynamic_pointer_cast<TextView>(child.second);
		if (textViewPointer)
		{
			sizeYTotal += textViewPointer->size.y;
			continue;
		}
		auto &childItem = *std::dynamic_pointer_cast<PanelItem>(child.second);
		sizeYTotal += childItem.size.y;
	}
	return sizeYTotal;
}
void PanelMenu::preRender()
{
	const auto &model = getModelMatrix();
	shader.bind();
	scene.entityPreRender(*this);
	shader.setBlock("Model", model);
	shader.setBlock("View", scene.view.matrix);
	shader.setBlock("Projection", scene.projection.matrix);
	shader.setBlock("CameraPosition", scene.view.position, 16);
	shader.unbind();
};
void PanelMenu::setColor(glm::vec4 color)
{
	colors = {color, color, color, color};
	updateElements("Color", colors);
};
void PanelMenu::setSize(glm::vec3 newSize)
{
	glm::vec3 actualNewSize(width / window.windowWidth / 0.5, height / window.windowHeight / 0.5, 0);
	positions = {
		{ 0, -actualNewSize.y, 0 }, { actualNewSize.x, -actualNewSize.y, 0 }, { actualNewSize.x, 0, 0 }, { 0, 0, 0 }
	};
	updateElements("Position", positions);
	size = actualNewSize;
};
PanelItem::PanelItem(RenderWindow &window,
										 GLScene &scene,
										 glm::vec3 position,
										 glm::vec3 rotation,
										 glm::vec3 scale,
										 glm::vec4 color,
										 const std::string_view text,
										 fonts::freetype::FreetypeFont &font,
                     GLEntity &entity,
                     float panelWidth,
                     float indent,
										 const shaders::RuntimeConstants &constants,
                     std::string_view name):
	zg::modules::gl::GLEntity(
		window,
		zg::mergeVectors<std::string_view>({
			{
				"Color", "Position",
				"View", "Projection", "Model", "CameraPosition"
			}
		}, constants),
		6,
		{0, 1, 2,  2, 3, 0},
		4,
		{
			{ 0, -0, 0 }, { 0, -0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }
		},
		position,
		rotation,
		scale,
    name.empty() ? "PanelItem " + std::to_string(++panelItemsCount) : name
	),
	scene(scene),
	text(text),
	font(font),
	entity(entity),
  panelWidth(panelWidth),
  indent(indent)
{
	updateIndices(indices);
  colors.resize(4);
	setColor(color);
	float FontSize = window.windowHeight / 30.f;
	float LineHeight = 0;
	auto TextSize = font.stringSize(text, FontSize, LineHeight, {panelWidth - indent, 0});
	TextSize.y /= window.windowHeight * 0.5f;
	TextSize.x /= window.windowWidth * 0.5f;
	textView = std::make_shared<TextView>(
		window,
		scene,
		glm::vec3(TextSize.x / 2, -TextSize.y / 2, 0.1f),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(1, 1, 1, 1),
		text,
		TextSize,
		font,
		FontSize,
		true,
		[](auto TextSize)
		{
			return glm::vec3(TextSize.x / 2, -TextSize.y / 2, 0.1f);
		},
		TextView::ResizeHandler(),
		[&]
		{
			return this->window.windowHeight / 30.f;
		});
	textView->addToBVH = false;
  addChild(textView);
	PanelItem::setSize({ (panelWidth - indent) / (window.windowWidth / 2), TextSize.y, 0 });
	mouseHoverID = addMouseHoverHandler([&, color](const auto &hovered)
	{
		if (hovered)
		{
			setColor({ 0.4, 0.4, 0.4, 1 });
		}
		else
		{
			setColor(color);
		}
	});
	mousePressID = addMousePressHandler(0, [&](auto pressed)
	{
		if (!pressed)
			return;
	});
};
PanelItem::~PanelItem()
{
	removeMouseHoverHandler(mouseHoverID);
	removeMousePressHandler(0, mousePressID);
};
void PanelItem::preRender()
{
	const auto &model = getModelMatrix();
	shader.bind();
	scene.entityPreRender(*this);
	shader.setBlock("Model", model);
	shader.setBlock("View", scene.view.matrix);
	shader.setBlock("Projection", scene.projection.matrix);
	shader.setBlock("CameraPosition", scene.view.position, 16);
	shader.unbind();
};
void PanelItem::setColor(glm::vec4 color)
{
  auto colorsData = colors.data();
	colorsData[0] = color;
  colorsData[1] = color;
  colorsData[2] = color;
  colorsData[3] = color;
	updateElements("Color", colors);
};
void PanelItem::setSize(glm::vec3 newSize)
{
	positions = {
		{ 0, -newSize.y, 0 }, { newSize.x, -newSize.y, 0 }, { newSize.x, 0, 0 }, { 0, 0, 0 }
	};
	updateElements("Position", positions);
	this->size = newSize;
};