#pragma once
#include <anex/IWindow.hpp>
#include "./common.hpp"
#include "./textures/Framebuffer.hpp"
#include <mutex>
namespace anex::modules::gl
{
	namespace shaders
	{
		struct Shader;
	}
	namespace entities
	{
		struct Plane;
	}
	struct ShaderContext
	{
		std::unordered_map<uint32_t, std::shared_ptr<shaders::Shader>> shaders;
		std::unordered_map<std::size_t, std::pair<uint32_t, std::shared_ptr<shaders::Shader>>> shadersByHash;
		uint32_t shaderCount = 0;
	};
	struct GLScene;
#define KEYCODE_HOME 0x80
#define KEYCODE_END 0x81
	struct GLWindow : IWindow
	{
		const char *title;
#ifdef _WIN32
		HINSTANCE hInstance;
		HWND hwnd;
		HDC hDeviceContext;
		HGLRC hRenderingContext;
#endif
		int windowKeys[256];
		int windowButtons[7];
		bool mouseMoved = false;
		glm::vec2 mouseCoords;
		int mod = 0;
		bool isChildWindow = false;
		GLWindow *parentWindow = 0;
		GLScene *parentScene = 0;
		std::vector<GLWindow*> childWindows;
		GladGLContext *glContext = 0;
		ShaderContext *shaderContext = 0;
		bool NDCFramebufferPlane;
		std::shared_ptr<textures::Texture> framebufferTexture;
		std::shared_ptr<textures::Texture> framebufferDepthTexture;
		std::shared_ptr<textures::Framebuffer> framebuffer;
		std::shared_ptr<entities::Plane> framebufferPlane;
		static constexpr unsigned int MinMouseButton = 0;
		static constexpr unsigned int MaxMouseButton = 6;
		float dpiScale = 1.0f;
		glm::vec2 oldXY;
		GLWindow(const char *title,
						 float windowWidth,
						 float windowHeight,
						 float windowX,
						 float windowY,
						 bool borderless = false,
						 uint32_t framerate = 60);
		GLWindow(GLWindow &parentWindow,
						 GLScene &parentScene,
						 const char *childTitle,
						 float childWindowWidth,
						 float childWindowHeight,
						 float childWindowX,
						 float childWindowY,
						 bool NDCFramebufferPlane = false,
						 uint32_t framerate = 60);
		~GLWindow() override;
		void startWindow() override;
		void renderInit();
		void updateKeyboard() override;
		void updateMouse() override;
		void close() override;
		void minimize() override;
		void maximize() override;
		void restore() override;
		void preRender() override;
		void postRender() override;
		void drawLine(int x0, int y0, int x1, int y1, uint32_t color) override;
		void drawRectangle(int x, int y, int w, int h, uint32_t color) override;
		void drawCircle(int x, int y, int radius, uint32_t color) override;
		void drawText(int x, int y, const char* text, int scale, uint32_t color) override;
		void warpPointer(glm::vec2 coords) override;
		void setXY(float x, float y) override;
		void setWidthHeight(float width, float height) override;
		IWindow &createChildWindow(const char *title,
															 IScene &scene,
															 float windowWidth,
															 float windowHeight,
															 float windowX,
															 float windowY,
															 bool NDCFramebufferPlane) override;
	};
	void computeNormals(const std::vector<uint32_t> &indices, const std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals);
}
