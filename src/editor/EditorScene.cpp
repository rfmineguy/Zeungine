#include <editor/EditorScene.hpp>
#include <editor/CodeScene.hpp>
#include <zg/filesystem/Directory.hpp>
using namespace zg::editor;
EditorScene::EditorScene(RenderWindow& window):
	GLScene(window, {0, 0, 50}, {0, 0, -1}, {2, 2}),
	toolbarHeight(window.windowHeight / 14),
	bottomTabsHeight(window.windowHeight / 18),
	robotoRegularFile("fonts/Roboto/Roboto-Regular.ttf", enums::EFileLocation::Relative, "r"),
	robotoRegularFont(window, robotoRegularFile),
	gameWindowBorderWidth(4.f),
	gameWindowWidth(window.windowWidth / 1.75f),
	gameWindowHeight(window.windowHeight / 1.75f),//- toolbarHeight - bottomTabsHeight),
	gameWindowX(window.windowWidth / 2.f - gameWindowWidth / 2.f),
	gameWindowY(toolbarHeight + gameWindowBorderWidth),
	codeWindowWidth(gameWindowWidth + gameWindowBorderWidth * 2),
	codeWindowHeight(gameWindowHeight + gameWindowBorderWidth * 2),
	codeWindowX(gameWindowX - gameWindowBorderWidth),
	codeWindowY(gameWindowY - gameWindowBorderWidth),
	toolbarColor(35 / 255.f, 33 / 255.f, 36 / 255.f, 1),
	toolbar(std::make_shared<entities::Toolbar>(
		window,
		*this,
		glm::vec3(-1, 1, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(1, 1, 1),
		toolbarColor,
		toolbarHeight,
		robotoRegularFont
	)),
	bottomTabsBar(std::make_shared<entities::TabsBar>(
		window,
		*this,
		glm::vec3(-1 + ((gameWindowX - gameWindowBorderWidth) / window.windowWidth / 0.5), ((bottomTabsHeight - gameWindowBorderWidth * 2) / window.windowHeight / 0.5) - 1, 0),
		glm::vec3(0),
		glm::vec3(1),
		toolbarColor,
		robotoRegularFont,
		gameWindowWidth + gameWindowBorderWidth * 2,
		bottomTabsHeight - gameWindowBorderWidth * 2)),
	status(std::make_shared<entities::Status>(
		window,
		*this,
		glm::vec3(-1, ((bottomTabsHeight - gameWindowBorderWidth * 2) / window.windowHeight / 0.5) - 1, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		toolbarColor,
		robotoRegularFont,
		(window.windowWidth - gameWindowWidth) / 2,
		bottomTabsHeight - gameWindowBorderWidth * 2,
		"Idle"
	)),
	gameWindowBorder(std::make_shared<entities::Plane>(
		window, *this,
		glm::vec3(-1 + ((gameWindowX + (gameWindowWidth / 2)) / window.windowWidth / 0.5),
							1 - ((gameWindowY + (gameWindowHeight / 2)) / window.windowHeight / 0.5), 0.1), glm::vec3(0),
		glm::vec3(1),
		glm::vec2((gameWindowWidth + gameWindowBorderWidth * 2) / (window.windowWidth / 2),
							(gameWindowHeight + gameWindowBorderWidth * 2) / (window.windowHeight / 2)),
		gameWindowBorderColor)),
	sceneGraphPanelMenu(std::make_shared<entities::PanelMenu>(
		window, *this,
		glm::vec3(-1, 1.f - (toolbarHeight / window.windowHeight / 0.5), 0),
		glm::vec3(0), glm::vec3(1), glm::vec4(0.5, 0.5, 0.5, 1),
		robotoRegularFont, "Scene Graph",
		gameWindowX - gameWindowBorderWidth,
		gameWindowHeight + gameWindowBorderWidth * 2.f)),
	resourcePanelMenuHeight(window.windowHeight - ((toolbarHeight + gameWindowHeight + bottomTabsHeight))),
	resourcePanelMenu(std::make_shared<entities::PanelMenu>(
		window,
		*this,
		glm::vec3(-1, 1 - ((toolbarHeight + gameWindowHeight + gameWindowBorderWidth * 2) / window.windowHeight / 0.5), 0.1),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(0.5, 0.5, 0.5, 1),
		robotoRegularFont,
		"",
		window.windowWidth,
		resourcePanelMenuHeight
	)),
	resourceConsole(std::make_shared<entities::Console>(
		window,
		*this,
		glm::vec3(0 , 0, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(0.2, 0.2, 0.2, 1),
		robotoRegularFont,
		window.windowWidth,
		resourcePanelMenuHeight - bottomTabsHeight + gameWindowBorderWidth * 2
	)),
	resourcePanelTabs(std::make_shared<entities::TabsBar>(
		window,
		*this,
		glm::vec3(0, (-resourcePanelMenuHeight + bottomTabsHeight - gameWindowBorderWidth * 2) / window.windowHeight / 0.5, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		toolbarColor,
		robotoRegularFont,
		window.windowWidth,
		bottomTabsHeight - gameWindowBorderWidth * 2)),
	dialogWidth(window.windowWidth / 3),
	dialogHeight(window.windowHeight / 5),
	closeDialogButtonWidth(window.windowWidth / 44.f),
	closeDialogButtonHeight(window.windowWidth / 44.f),
	okayDialogButtonWidth(dialogWidth / 6.f),
	okayDialogButtonHeight(dialogHeight / 4.5f),
	closeDialogButton(std::make_shared<entities::Button>(
		window,
		*this,
		glm::vec3(
			(dialogWidth - closeDialogButtonWidth) / window.windowWidth / 0.5,
			0,
			0.1),
		glm::vec3(0),
    	glm::vec3(1),
		glm::vec4(1, 0, 0, 1),
		glm::vec2(closeDialogButtonWidth, closeDialogButtonHeight),
		"x",
		robotoRegularFont,
    	[](){}
	)),
	okayDialogButton(std::make_shared<entities::Button>(
		window,
		*this,
		glm::vec3(
			(dialogWidth - okayDialogButtonWidth) / window.windowWidth / 0.5,
			(-dialogHeight + okayDialogButtonHeight) / window.windowHeight / 0.5,
			0.1),
		glm::vec3(0),
    	glm::vec3(1),
		glm::vec4(0.2f, 0, 0.8f, 1),
		glm::vec2(okayDialogButtonWidth, okayDialogButtonHeight),
		"Okay",
		robotoRegularFont,
    	[](){}
	)),
	projectNameInput(std::make_shared<entities::Input>(
		window,
		*this,
		glm::vec3(16 / window.windowWidth / 0.5, -30 / window.windowHeight / 0.5, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(0.5, 0.5, 0.5, 1),
		robotoRegularFont,
		dialogWidth - 32,
	    dialogHeight / 4.5f,
	    "Project Name",
		128.f
	)),
	projectDirectoryInput(std::make_shared<entities::Input>(
		window,
		*this,
		glm::vec3(16 / window.windowWidth / 0.5, -((dialogHeight / 4.5f) * 2) / window.windowHeight / 0.5, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(0.5, 0.5, 0.5, 1),
		robotoRegularFont,
		dialogWidth - 32,
	    dialogHeight / 4.5f,
	    "Project Directory",
		128.f
	)),
	newProjectDialog(std::make_shared<entities::Dialog>(
		window,
		*this,
		glm::vec3(0, 0, 5.0),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(0.6, 0.6, 0.6, 1),
		robotoRegularFont,
		"New Project",
		dialogWidth,
    	dialogHeight,
		std::vector<std::shared_ptr<GLEntity>>({closeDialogButton, okayDialogButton, projectNameInput, projectDirectoryInput})
  	))
{
	(*projectNameInput->textPointer) = "EditorGame";
	(*projectDirectoryInput->textPointer) = "C:/Users/Steven/Projects/EditorGame";
	projectNameInput->handleKey(0, true);
	projectDirectoryInput->handleKey(0, true);
	closeDialogButton->handler = [&]()
	{
		removeEntity(activeDialog->ID);
		projectNameInput->clear();
		projectDirectoryInput->clear();
	};
	okayDialogButton->handler = [&]()
	{
		removeEntity(activeDialog->ID);
		newProject(*projectNameInput->textPointer, *projectDirectoryInput->textPointer);
		projectNameInput->clear();
		projectDirectoryInput->clear();
	};
	clearColor = editorClearColor;
	sceneGraphPanelMenu->addToBVH = false;
	addEntity(sceneGraphPanelMenu);
	addEntity(resourcePanelMenu);
	resourcePanelTabs->addTab("Console", [&]
	{
		removeActiveResourceEntity();
		resourcePanelMenu->addPanelEntity(resourceConsole, false);
		activeResourcePanelEntity = std::dynamic_pointer_cast<GLEntity>(resourceConsole);
	}, true);
	resourcePanelMenu->addPanelEntity(resourceConsole, false);
	activeResourcePanelEntity = std::dynamic_pointer_cast<GLEntity>(resourceConsole);
	resourcePanelMenu->addPanelEntity(resourcePanelTabs, false);
	setupToolbarOptions();
	addEntity(toolbar);
	bottomTabsBar->addToBVH = false;
	addEntity(bottomTabsBar);
	addEntity(status);
	addEntity(gameWindowBorder);
	setupGameWindow();
	setupCodeWindow();
	resizeID = view.addResizeHandler([&](auto newSize)mutable
	{
		toolbarHeight = newSize.y / 14;
		bottomTabsHeight = newSize.y / 18;
		gameWindowWidth = window.windowWidth / 1.75f;
		gameWindowHeight = window.windowHeight - toolbarHeight - bottomTabsHeight;
		gameWindowX = window.windowWidth / 2.f - gameWindowWidth / 2.f;
		gameWindowY = toolbarHeight + gameWindowBorderWidth;
		gameWindowPointer->setWidthHeight(gameWindowWidth, gameWindowHeight);
		gameWindowPointer->setXY(gameWindowX, gameWindowY);
		codeWindowWidth = gameWindowWidth + gameWindowBorderWidth * 2;
		codeWindowHeight = gameWindowHeight + gameWindowBorderWidth * 2;
		codeWindowX = gameWindowX - gameWindowBorderWidth;
		codeWindowY = gameWindowY - gameWindowBorderWidth;
		codeWindowPointer->setWidthHeight(codeWindowWidth, codeWindowHeight);
		codeWindowPointer->setXY(codeWindowX, codeWindowY);
	});
	bottomTabsBar->addTab("Scene", [&]()
	{
		minimizeWindows();
		addEntity(gameWindowBorder);
		gameWindowPointer->restore();
	}, true);
	bottomTabsBar->addTab("Code Editor", [&]()
	{
		minimizeWindows();
		codeWindowPointer->restore();
	});
	std::cout << "Opened ZG Editor" << std::endl;
};
EditorScene::~EditorScene()
{
	view.removeResizeHandler(resizeID);
	gameWindowBorder->removeMouseHoverHandler(gameWindowBorderHoverID);
	gameWindowBorder->removeMousePressHandler(0, gameWindowBorderPressID);
};
void EditorScene::onEntityAdded(const std::shared_ptr<IEntity>& entity)
{
	auto& glEntity = *std::dynamic_pointer_cast<GLEntity>(entity);
	auto sizeYTotal = sceneGraphPanelMenu->getSizeYTotal();
	static const auto indent = 16.f;
	auto panelItem = std::make_shared<entities::PanelItem>(
		dynamic_cast<RenderWindow&>(window),
		*this,
		glm::vec3(indent / window.windowWidth / 0.5, -sizeYTotal, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		sceneGraphPanelMenu->color,
		glEntity.name,
		robotoRegularFont,
		glEntity,
		sceneGraphPanelMenu->width,
		indent);
	sceneGraphPanelMenu->addPanelEntity(panelItem);
};
void EditorScene::setupGameWindow()
{
	auto& gameWindow = window.createChildWindow(
		"EditorChild",
		*this,
		gameWindowWidth,
		gameWindowHeight,
		gameWindowX,
		gameWindowY,
		true);
  	gameWindowPointer = (RenderWindow *)&gameWindow;
	gameWindow.minimize();
	std::function<void(const std::shared_ptr<IEntity>&)> entityAddedFunction = std::bind(
		&EditorScene::onEntityAdded, this, std::placeholders::_1);
	gameWindow.registerOnEntityAddedFunction(entityAddedFunction);
	gameWindowBorderHoverID = gameWindowBorder->addMouseHoverHandler([&](const auto& hovered)
	{
		if (hovered)
			gameWindowBorder->setColor(gameWindowHoveredBorderColor);
		else
			gameWindowBorder->setColor(gameWindowBorderColor);
	});
	gameWindowBorderPressID = gameWindowBorder->addMousePressHandler(0, [&](const auto& pressed)
	{
		if (pressed)
		{
			gameWindowPointer->focused = true;
			if (currentHoveredEntity)
			{
				currentHoveredEntity = 0;
			}
			gameWindowBorder->setColor(gameWindowActiveBorderColor);
		}
	});
	gameWindowESCPressID = gameWindowPointer->addKeyPressHandler(27, [&](const auto& pressed)
	{
		if (!pressed)
		{
			gameWindowPointer->focused = false;
			auto &glWindow = (RenderWindow&)window;
			window.callMouseMoveHandler(glWindow.mouseCoords);
		}
	});
};
void EditorScene::setupCodeWindow()
{
	auto& codeWindow = window.createChildWindow(
		"Code Editor",
		*this,
		codeWindowWidth,
		codeWindowHeight,
		codeWindowX,
		codeWindowY,
		true);
  codeWindowPointer = (RenderWindow *)&codeWindow;
	codeWindow.minimize();
	codeWindow.setIScene(std::make_shared<CodeScene>((RenderWindow &)codeWindow));
};
void EditorScene::minimizeWindows()
{
  auto& glWindow = (RenderWindow&)window;
	for (auto &windowPointer : glWindow.childWindows)
	{
		if (!windowPointer->minimized)
		{
			windowPointer->minimize();
			if (windowPointer == gameWindowPointer)
			{
				removeEntity(gameWindowBorder->ID);
			}
		}
	}
}
void EditorScene::removeActiveResourceEntity() const
{
	if (activeResourcePanelEntity && activeResourcePanelEntity->ID)
	{
		resourcePanelMenu->removePanelEntity(activeResourcePanelEntity);
	}
}
void EditorScene::setupToolbarOptions()
{
	auto &fileDropdown = *toolbar->fileDropdown;
	fileDropdown.addOption("New Project", [&]()
	{
		addEntity(newProjectDialog);
		activeDialog = newProjectDialog;
		bvh.removeEntity(*this, *toolbar->fileDropdown);
		toolbar->file->removeChild(toolbar->fileDropdownID);
	});
	fileDropdown.addOption("Open Project", []()
	{

	});
	fileDropdown.addOption("Save", []()
	{

	});
	fileDropdown.addOption("Save As", []()
	{

	});
	fileDropdown.addOption("Import Assets", []()
	{

	});
	fileDropdown.addOption("Export", []()
	{

	});
	fileDropdown.addOption("Recent Files", []()
	{

	});
	fileDropdown.addOption("Settings", []()
	{

	});
	fileDropdown.addOption("Exit", [&]()
	{
		window.close();
	});
}
void EditorScene::newProject(std::string_view projectName, std::string_view projectDirectory)
{
	{
		project = {projectName, projectDirectory};
		filesystem::Directory::ensureExists(project.directory);
		std::string includePath = filesystem::File::toPlatformPath(std::string(project.directory) + "/include");
		filesystem::Directory::ensureExists(includePath);
		std::string srcPath = filesystem::File::toPlatformPath(std::string(project.directory) + "/src");
		filesystem::Directory::ensureExists(srcPath);
		filesystem::File mainIncludeFile(filesystem::File::toPlatformPath(std::string(includePath) + "/main.hpp"), enums::EFileLocation::Relative, "w+");
		std::string mainIncludeFileString(R"(#pragma once
#include <Runtime.hpp>
#include <zg/modules/gl/GLScene.hpp>
#include <zg/modules/gl/RenderWindow.hpp>
using namespace zg;
using namespace zg::modules::gl;
struct MainScene : GLScene
{
	explicit MainScene(RenderWindow &window);
};
ZG_API void OnLoad(RenderWindow &window);
ZG_API void OnHotswapLoad(RenderWindow &window, hscpp::AllocationResolver &allocationResolver);
ZG_API void OnUnLoad(RenderWindow &window);)");
		mainIncludeFile.writeBytes(0, mainIncludeFileString.size(), mainIncludeFileString.data());
		filesystem::File mainSrcFile(filesystem::File::toPlatformPath(std::string(srcPath) + "/main.cpp"), enums::EFileLocation::Relative, "w+");
		std::string mainSrcFileString(R"(#include <main.hpp>
MainScene::MainScene(RenderWindow &window):
	GLScene(window, {0, 50, 50}, {0, -1, -1}, 80.f)
{
	clearColor = {0.5, 0, 0.5, 1};
};
ZG_API void OnLoad(RenderWindow &window)
{
	window.setIScene(std::make_shared<MainScene>(window));
};
ZG_API void OnHotswapLoad(RenderWindow &window, hscpp::AllocationResolver &allocationResolver)
{
	window.setIScene(std::shared_ptr<MainScene>(allocationResolver.Allocate<MainScene>(window)));
};
ZG_API void OnUnLoad(RenderWindow &window)
{
	window.scene.reset();
	window.close();
})");
		mainSrcFile.writeBytes(0, mainSrcFileString.size(), mainSrcFileString.data());
	}
	loadProject(projectDirectory);
};
void EditorScene::loadProject(std::string_view projectDirectory)
{
	hotswapper = std::make_shared<hs::Hotswapper>(projectDirectory, *this);
	resourceAssetBrowser = std::make_shared<entities::AssetBrowser>(
		dynamic_cast<RenderWindow &>(window),
		*this,
		glm::vec3(0, 0, 0.1),
		glm::vec3(0),
		glm::vec3(1),
		glm::vec4(0.1, 0.1, 0.1, 1),
		robotoRegularFont,
		window.windowWidth,
		resourcePanelMenuHeight - bottomTabsHeight,
		projectDirectory);
	resourcePanelTabs->addTab("Asset Browser", [&]
	{
		removeActiveResourceEntity();
		if (resourceAssetBrowser)
		{
			resourcePanelMenu->addPanelEntity(resourceAssetBrowser, false);
			activeResourcePanelEntity = std::dynamic_pointer_cast<GLEntity>(resourceAssetBrowser);
		}
	});
	resourcePanelTabs->addTab("Performance", [&]
	{
		removeActiveResourceEntity();
	});
};