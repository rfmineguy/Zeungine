#pragma once
#ifdef LINUX
#include "../RenderWindow.hpp"
namespace zg::modules::gl
{
	struct X11Window : IPlatformWindow
	{
		float dpiScale = 1.0f;
		void init(RenderWindow& window) override;
		void createContext() override;
		void renderInit();
		void postInit() override;
		bool pollMessages() override;
		void swapBuffers() override;
		void destroy() override;
		void close() override;
		void minimize() override;
		void maximize() override;
		void restore() override;
		void warpPointer(glm::vec2 coords) override;
		void setXY() override;
		void setWidthHeight() override;
		void mouseCapture(bool capture) override;
	};
}
#endif