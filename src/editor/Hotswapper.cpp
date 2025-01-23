#include <editor/Hotswapper.hpp>
#include <editor/EditorScene.hpp>
#include <anex/filesystem/Directory.hpp>
#include <anex/filesystem/File.hpp>
using namespace anex::editor::hs;
Hotswapper::Hotswapper(std::string_view directory, EditorScene &editorScene):
		running(true),
		directory(directory),
		editorScene(editorScene),
		swapper(std::make_shared<hscpp::Hotswapper>()),
		updateThread(std::make_shared<std::thread>(&Hotswapper::update, this))
{}
Hotswapper::~Hotswapper()
{
	running = false;
	updateThread->join();
}
void Hotswapper::update()
{
	editorScene.status->setText("Initializing Compiler...");
	editorScene.status->setTextColor({1, 1, 0, 1});
	idle = false;
	auto &swapperRef = *swapper;
	auto srcDirectory = directory + "/src";
	filesystem::Directory srcDirectoryActual(srcDirectory);
	auto files = srcDirectoryActual.getRecursiveFileMap();
	for (auto &file : files)
	{
		swapperRef.AddForceCompiledSourceFile(file.second);
	}
	swapperRef.AddSourceDirectory(srcDirectory);
	swapperRef.AddIncludeDirectory(directory + "/include");
	auto programDirectoryPath = filesystem::File::getProgramDirectoryPath();
	swapperRef.AddIncludeDirectory(programDirectoryPath + "/../include");
	swapperRef.AddIncludeDirectory(programDirectoryPath + "/../vendor/glm");
	swapperRef.AddIncludeDirectory(programDirectoryPath + "/../vendor/stb");
	swapperRef.AddIncludeDirectory(programDirectoryPath + "/../vendor/bvh/src");
	swapperRef.AddIncludeDirectory(programDirectoryPath + "/../vendor/freetype/include");
	swapperRef.LocateAndAddLibrary(programDirectoryPath, "abstractnexus.lib");
	swapperRef.LinkLibrary("opengl32.lib");
	swapperRef.LinkLibrary("gdi32.lib");
	swapperRef.LinkLibrary("user32.lib");
	swapperRef.AddCompileOption("-DHSCPP_CXX_STANDARD=" + std::to_string(HSCPP_CXX_STANDARD));
	swapperRef.AddCompileOption("-DHSCPP_PLATFORM_WIN32");
	swapperRef.SetBuildDirectory(directory + "/build");
	while (!swapperRef.IsCompilerInitialized())
	{
		swapperRef.Update();
	}
	editorScene.status->setText("Compiling...");
	editorScene.status->setTextColor({1, 1, 0, 1});
	compiling = true;
	compiled = false;
	swapperRef.TriggerManualBuild(false);
	while (running)
	{
		auto updateResult = swapperRef.Update(false);
		switch (updateResult)
		{
		case hscpp::Hotswapper::UpdateResult::BeforeCompile:
		{
			if (editorScene.loaded)
			{
				if (editorScene.OnUnLoad)
				{
					editorScene.OnUnLoad(*editorScene.gameWindowPointer);
				}
				editorScene.loaded = false;
				editorScene.OnHotswapLoad = std::function<void(GLWindow&, hscpp::AllocationResolver &)>();
				editorScene.OnUnLoad = std::function<void(GLWindow&)>();
				swapperRef.UnLoadModule();
			}
			compiled = false;
			break;
		}
		case hscpp::Hotswapper::UpdateResult::Compiling:
		{
			editorScene.status->setText("Compiling...");
			editorScene.status->setTextColor({1, 1, 0, 1});
			compiling = true;
			break;
		}
		case hscpp::Hotswapper::UpdateResult::Compiled:
		{
			editorScene.status->setText("Compiled.");
			editorScene.status->setTextColor({0, 1, 0, 1});
			editorScene.OnHotswapLoad = swapperRef.GetFunction<void(GLWindow &, hscpp::AllocationResolver &)>("OnHotswapLoad");
			editorScene.OnUnLoad = swapperRef.GetFunction<void(GLWindow &)>("OnUnLoad");
			if (editorScene.OnHotswapLoad)
			{
				auto &allocationResolver = *swapper->GetAllocationResolver();
				editorScene.OnHotswapLoad(*editorScene.gameWindowPointer, allocationResolver);
				editorScene.loaded = true;
			}
			compiledTime = std::chrono::system_clock::now();
			compiling = false;
			compiled = true;
			break;
		}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (compiled && !idle)
		{
			auto now = std::chrono::system_clock::now();
			std::chrono::duration<float> duration = now - compiledTime;
			auto durationSeconds = duration.count();
			if (durationSeconds > 5)
			{
				editorScene.status->setText("Idle");
				editorScene.status->setTextColor({1, 1, 1, 1});
				idle = true;
			}
		}
	}
}