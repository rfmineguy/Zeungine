#include <anex/modules/gl/entities/Panel.hpp>
#include <anex/utilities.hpp>
#include <iostream>
using namespace anex::modules::gl::entities;
PanelMenu::PanelMenu(anex::modules::gl::GLWindow &window,
				             anex::modules::gl::GLScene &scene,
				             const glm::vec3 &position,
				             const glm::vec3 &rotation,
				             const glm::vec3 &scale,
				             const glm::vec4 &color,
				             fonts::freetype::FreetypeFont &font,
										 const std::string &title,
										 const float &width,
				             const anex::modules::gl::shaders::RuntimeConstants &constants,
                     const std::string &name):
	anex::modules::gl::GLEntity(
		window,
		anex::mergeVectors<std::string_view>({
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
	size({ 0, 0 }),
	font(font),
	title(title),
	width(width)
{
	updateIndices(indices);
	setColor(color);
	updateElements("Position", positions);
  float titleLineHeight = 0;
  auto titleSize = font.stringSize(title, 20.f, titleLineHeight, glm::vec2(0));
	titleTextView = std::make_shared<TextView>(window, scene, glm::vec3(titleSize.x / 2, -titleSize.y / 2, 0.5), glm::vec3(0), glm::vec3(1), title, titleSize, font, 20.f);
  titleTextView->addToBVH = false;
  addChild(titleTextView);
  setSize();
  addToBVH = false;
};
void PanelMenu::addItem(const std::string &name, GLEntity &entity)
{
	auto &vao = (VAO &)*this;
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
  auto panelItem = std::make_shared<PanelItem>(vao.window, scene, glm::vec3(0, -sizeYTotal, 0.5), glm::vec3(0), glm::vec3(1), glm::vec4(0.1, 0.1, 0.1, 1), name, font, entity);
  addChild(panelItem);
  scene.postAddEntity(panelItem, {ID, panelItem->ID});
  sizeYTotal += panelItem->size.y;
	float sizeXMax = 0;
	for (auto &child : children)
	{
		auto textViewPointer = std::dynamic_pointer_cast<TextView>(child.second);
		if (textViewPointer)
		{
			sizeXMax = std::max(textViewPointer->size.x, sizeXMax);
			continue;
		}
		auto &childItem = *std::dynamic_pointer_cast<PanelItem>(child.second);
		sizeXMax = std::max(childItem.size.x, sizeXMax);
	}
	for (auto &child : children)
	{
		auto childItemPointer = std::dynamic_pointer_cast<PanelItem>(child.second);
    if (!childItemPointer)
      continue;
    auto &childItem = *childItemPointer;
		childItem.setSize({sizeXMax, childItem.size.y});
    scene.bvh.updateEntity(childItem);
	}
	setSize();
};
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
void PanelMenu::setColor(const glm::vec4 &color)
{
	colors = {color, color, color, color};
	updateElements("Color", colors);
};
void PanelMenu::setSize()
{
	glm::vec2 size(width, 0);
  size.y = 0;
  size.y += titleTextView->size.y;
  for (auto &child : children)
  {
    auto panelItemPointer = std::dynamic_pointer_cast<PanelItem>(child.second);
    if (!panelItemPointer)
      continue;
    size.y += panelItemPointer->size.y;
  }
	positions = {
		{ 0, -size.y, 0 }, { size.x, -size.y, 0 }, { size.x, 0, 0 }, { 0, 0, 0 }
	};
	updateElements("Position", positions);
	this->size = size;
};
PanelItem::PanelItem(GLWindow &window,
										 GLScene &scene,
										 const glm::vec3 &position,
										 const glm::vec3 &rotation,
										 const glm::vec3 &scale,
										 const glm::vec4 &color,
										 const std::string &text,
										 fonts::freetype::FreetypeFont &font,
                     GLEntity &entity,
										 const shaders::RuntimeConstants &constants,
                     const std::string &name):
	anex::modules::gl::GLEntity(
		window,
		anex::mergeVectors<std::string_view>({
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
	size({ 0, 0 }),
	text(text),
	font(font),
	entity(entity)
{
	updateIndices(indices);
  colors.resize(4);
	setColor(color);
	float FontSize = window.windowHeight / 40.f;
	float LineHeight = 0;
	auto TextSize = font.stringSize(text, FontSize, LineHeight, {0, 0});
	textView = std::make_shared<TextView>(window, scene, glm::vec3(TextSize.x / 2, -TextSize.y / 2, 0.5f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), text, TextSize, font, FontSize);
	textView->addToBVH = false;
  addChild(textView);
  setSize({ TextSize.x, TextSize.y });
	mouseHoverID = addMouseHoverHandler([&, color](const auto &hovered)
	{
		if (hovered)
		{
			setColor({ 0.5, 0.5, 0.5, 1 });
		}
		else
		{
			setColor(color);
		}
	});
	mousePressID = addMousePressHandler(0, [&](auto &pressed)
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
void PanelItem::setColor(const glm::vec4 &color)
{
  auto colorsData = colors.data();
	colorsData[0] = color;
  colorsData[1] = color;
  colorsData[2] = color;
  colorsData[3] = color;
	updateElements("Color", colors);
};
void PanelItem::setSize(const glm::vec2 &size)
{
	positions = {
		{ 0, -size.y, 0 }, { size.x, -size.y, 0 }, { size.x, 0, 0 }, { 0, 0, 0 }
	};
	updateElements("Position", positions);
	this->size = size;
};