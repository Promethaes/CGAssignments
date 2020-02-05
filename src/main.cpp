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

		char* frag = R"(#version 420 core
		out vec4 FragColor;
  		
		in vec2 TexCoords;
		
		uniform sampler2D screenTexture;
		uniform sampler2D bloom;
		uniform sampler3D LUT1;
		uniform sampler3D LUT2;
		uniform sampler3D LUT3;
		uniform sampler3D LUT4;

		uniform int use1;
		uniform int use2;
		uniform int use3;
		uniform int useBloom;

		const float offset = 1.0 / 500.0; 

		//kernel code is from learnopengl, as for bloom we used a combination of our own findings and learnopengl's tutorial
		//https://learnopengl.com/Advanced-OpenGL/Framebuffers
		//https://learnopengl.com/Advanced-Lighting/Bloom
		//https://learnopengl.com/Advanced-Lighting/HDR
		void main()
		{
		    vec3 col = vec3(texture(screenTexture, TexCoords.st));

				 vec2 offsets[9] = vec2[](
			     vec2(-offset,  offset), // top-left
			     vec2( 0.0f,    offset), // top-center
			     vec2( offset,  offset), // top-right
			     vec2(-offset,  0.0f),   // center-left
			     vec2( 0.0f,    0.0f),   // center-center
			     vec2( offset,  0.0f),   // center-right
			     vec2(-offset, -offset), // bottom-left
			     vec2( 0.0f,   -offset), // bottom-center
			     vec2( offset, -offset)  // bottom-right    
			 );

			 float kernel[9] = float[](
					1,	2,	1,
					2,	4,	2,
					1,	2,	1
			 );
			 
			 vec3 sampleTex[9];
				   for(int i = 0; i < 9; i++)
					{
						sampleTex[i] = vec3(texture(bloom,TexCoords.st + offsets[i]));
					}
				vec3 fBloom = vec3(0.0f);

			// for(int k = 0; k < 2; k++)
				 for(int i = 0; i < 9; i++)
					fBloom += sampleTex[i] * kernel[i]/16.0f;

			//this is HDR
			fBloom = vec3(1.0) - exp(-fBloom*1.0f);//1 is exposure

			col += fBloom*useBloom;    

			//this is HDR
			col = vec3(1.0) - exp(-col*1.0f);//1 is exposure
			vec4 Lcol1 = texture(LUT1,col)*use1;
			vec4 Lcol2 = texture(LUT2,col)*use2;
			vec4 Lcol3 = texture(LUT3,col)*use3;
			vec4 fColour = Lcol1 + Lcol2 + Lcol3;	
			if(fColour == vec4(0.0f,0.0f,0.0f,0.0f))
				fColour = texture(LUT4,col);

		    FragColor = fColour;

		})";
		
		Cappuccino::Framebuffer test(glm::vec2(1600.0f, 1000.0f), 2,
			[]()
		{
			CAPP_GL_CALL(glEnable(GL_DEPTH_TEST));
			CAPP_GL_CALL(glEnable(GL_CULL_FACE));
			CAPP_GL_CALL(glEnable(GL_BLEND));
			CAPP_GL_CALL(glEnable(GL_SCISSOR_TEST));
			CAPP_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		},std::nullopt,frag);

		Cappuccino::Framebuffer::_fbShader->use();
		Cappuccino::Framebuffer::_fbShader->setUniform("screenTexture", 0);
		Cappuccino::Framebuffer::_fbShader->setUniform("LUT", 1);
		MainScene* m = new MainScene(true);
		m->init();


		application->run();
		delete application;
	}

	return 0;

}
