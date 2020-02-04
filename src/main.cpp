#include "Cappuccino/Application.h"
#include "MainScene.h"
#include "Cappuccino/FrameBuffer.h"

using Application = Cappuccino::Application;
using FontManager = Cappuccino::FontManager;
using Mesh        = Cappuccino::Mesh;
using Shader      = Cappuccino::Shader;
using SoundSystem = Cappuccino::SoundSystem;
using Texture     = Cappuccino::Texture;
//using LUT		  = Cappuccino::LUT;

#pragma region PROGRAM SETTINGS

constexpr GLuint  SCR_WIDTH = 1600;
constexpr GLuint  SCR_HEIGHT = 1000;
constexpr GLchar* SCR_TITLE = "CGAssignments";

#pragma endregion

// This piece of code was given to us by Shawn Matthews
// Auto-magically changes OpenGL to use the high performance GPU to render rather than the iGPU
// (for dual GPU systems)
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x01;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x01;
}

int main() {

	if(!Application::isInstantiated()) {

		Cappuccino::Viewport view{ glm::vec4(0.1f,0.1f,0.1f,1.0f),glm::vec4(0.0f,0.0f,SCR_WIDTH,SCR_HEIGHT),[]() {} };

		Application* application = new Application(SCR_WIDTH, SCR_HEIGHT, SCR_TITLE, {view});
		application->init();

		// Create your scenes here
		// This is a test scene that you can run to see the capabilities of the engine
		// Cappuccino::TestScene* e = new Cappuccino::TestScene(true);
		// e->init();

		FontManager::setDefaultPath("./Assets/Fonts/");
		Mesh::setDefaultPath("./Assets/Meshes/");
		Shader::setDefaultPath("./Assets/Shaders/");
		SoundSystem::setDefaultPath("./Assets/Sounds/");
		Texture::setDefaultPath("./Assets/Textures/");
		//LUT::setDefaultPath("./Assets/LUTs/");

		//char* frag = R"(#version 420 core
		//out vec4 FragColor;
  		//
		//in vec2 TexCoords;
		//
		//uniform sampler2D screenTexture;
		//
		//void main()
		//{
		//    vec3 col = vec3(texture(screenTexture, TexCoords.st));
		//	//this is HDR
		//	col = vec3(1.0) - exp(-col*1.0);//1 is exposure
		//	//this is HDR
		//    FragColor = vec4(col, 1.0);
		//})";
		//
		//Cappuccino::Framebuffer test(glm::vec2(1600.0f, 1000.0f), 2,
		//	[]()
		//{
		//	CAPP_GL_CALL(glEnable(GL_DEPTH_TEST));
		//	CAPP_GL_CALL(glEnable(GL_CULL_FACE));
		//	CAPP_GL_CALL(glEnable(GL_BLEND));
		//	CAPP_GL_CALL(glEnable(GL_SCISSOR_TEST));
		//	CAPP_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//
		//});

		MainScene* m = new MainScene(true);
		m->init();


		application->run();
		delete application;
	}

	return 0;

}
