#include <zg/modules/gl/vp/View.hpp>
using namespace zg::modules::gl::vp;
View::View(glm::vec3 _position, glm::vec3 _direction):
	position(_position),
	direction(glm::normalize(_direction))
{
	phi = atan2(direction.z, direction.x);
	theta = acos(glm::clamp(direction.y, -1.0f, 1.0f));
  update();
};
void View::update()
{
	matrix = glm::lookAt(position, position + direction, glm::vec3{0, 1, 0});
};
void View::addPhiTheta(float addPhi, float addTheta)
{
	phi += addPhi;
	theta += addTheta;
	theta = glm::clamp(theta, 0.001f, glm::pi<float>() - 0.001f);
	glm::dvec3 newDirection;
	newDirection.x = sin(theta) * cos(phi);
	newDirection.y = cos(theta);
	newDirection.z = sin(theta) * sin(phi);
	direction = newDirection + 1e-6;
  update();
};
zg::IWindow::EventIdentifier View::addResizeHandler(const IWindow::ViewResizeHandler &callback)
{
	auto id = ++viewResizeHandlers.first;
	viewResizeHandlers.second[id] = callback;
	return id;
};
void View::removeResizeHandler(zg::IWindow::EventIdentifier &id)
{
	auto &handlers = viewResizeHandlers.second;
	auto handlerIter = handlers.find(id);
	if (handlerIter == handlers.end())
	{
		return;
	}
	handlers.erase(handlerIter);
	id = 0;
};
void View::callResizeHandler(glm::vec2 newSize)
{
	auto& handlersMap = viewResizeHandlers.second;
	std::vector<IWindow::ViewResizeHandler> handlersCopy;
	for (const auto& pair : handlersMap)
		handlersCopy.push_back(pair.second);
	for (auto& handler : handlersCopy)
	{
		handler(newSize);
	}
};