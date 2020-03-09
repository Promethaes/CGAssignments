#include "MainScene.h"
#include "Cappuccino/ResourceManager.h"
#include "Cappuccino/Random.h"

#define LOAD_MESH Cappuccino::MeshLibrary::loadMesh
#define LOAD_TEXTURE Cappuccino::TextureLibrary::loadTexture

MainScene::MainScene(bool first)
	:Scene(first), _in(true, std::nullopt)
{
	_c.setPosition(_c.getPosition() + glm::vec3(0.0f, 1.0f, 0.0f));
}

bool MainScene::init()
{
	_mainShader = new Cappuccino::Shader(std::string("main"), "PBRAnimate.vert", "PBR.frag");
	Cappuccino::Application::_lightingPassShader = _mainShader;
	_crawler = new Empty(_mainShader, {
		LOAD_TEXTURE("Crawler Albedo",		"Crawler/CrawlerDiffuse.png",Cappuccino::TextureType::PBRAlbedo),
		LOAD_TEXTURE("Crawler Roughness",	"Crawler/Crawler-Roughness.png",Cappuccino::TextureType::PBRRoughness),
		LOAD_TEXTURE("Crawler AO",			"Crawler/Crawler-AO.png",Cappuccino::TextureType::PBRAmbientOcc),
		LOAD_TEXTURE("Crawler Normal",		"Crawler/CrawlerNorm.png",Cappuccino::TextureType::PBRNormal) },
		{
			LOAD_MESH("Crawler Mesh","Crawler.obj")
		});
	_crawler->_rigidBody.setGrav(false);
	_crawler->_rigidBody._position = glm::vec3(0.0f, 0.0f, 0.0f);

	_crawler->setActive(true);

	_bot = new Empty(_mainShader, {
		LOAD_TEXTURE("Bot Albedo",		"Bot/Bot-Diffuse.png",Cappuccino::TextureType::PBRAlbedo),
		LOAD_TEXTURE("Bot Roughness",	"Bot/Bot-Roughness.png",Cappuccino::TextureType::PBRRoughness),
		LOAD_TEXTURE("Bot AO",			"Bot/Bot-AO.png",Cappuccino::TextureType::PBRAmbientOcc),
		LOAD_TEXTURE("Bot Normal",		"Bot/Bot-Normal.png",Cappuccino::TextureType::PBRNormal),
		LOAD_TEXTURE("Bot Emission",	"Bot/Bot-Emission.png",Cappuccino::TextureType::PBREmission),
		LOAD_TEXTURE("Bot Metallic",	"Bot/Bot-Metallic.png",Cappuccino::TextureType::PBRMetallic),
		},
		{
			LOAD_MESH("Crawler Mesh22","Bot.obj")
		});
	_bot->_rigidBody.setGrav(false);
	_bot->_rigidBody._position = glm::vec3(2.0f, 0.0f, 2.0f);

	_bot->setActive(true);

	//_bot = new Empty(_mainShader,{})

	_lights.reserve(25);
	_lights.push_back(new PointLightBody(_mainShader, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)));

	sendLights();
	sendGBufferShaderUniforms();

	return true;
}

bool MainScene::exit()
{
	return true;
}

void MainScene::childUpdate(float dt)
{
	static float eTime = 0.0f;
	eTime += dt;

	//calculate camera movement
	auto moveForce = glm::vec3(0.0f, 0.0f, 0.0f);
	if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::W))
		moveForce += glm::vec3(_c.getFront().x, 0.0f, _c.getFront().z);
	if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::S))
		moveForce -= glm::vec3(_c.getFront().x, 0.0f, _c.getFront().z);

	if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::A))
		moveForce -= glm::vec3(_c.getRight().x, 0.0f, _c.getRight().z);
	if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::D))
		moveForce += glm::vec3(_c.getRight().x, 0.0f, _c.getRight().z);

	float speed = 1.5f;
	moveForce *= speed;

	_c.setPosition(_c.getPosition() + moveForce * dt);

	if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::K) && !_crawler->_animator.isPlaying(AnimationType::Jump)) {
		static bool useLookupTable = false;
		useLookupTable ^= 1;
		_crawler->_animator.playAnimation(AnimationType::Jump);
		Cappuccino::Application::_ppShader->use();
		Cappuccino::Application::_ppShader->setUniform("useLookupTable", useLookupTable);
	}

	{
		static bool lightMove = false;
		static bool done = false;
		if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::B) && !done) {
			lightMove ^= true;
			done = true;
		}
		else if (_in.keyboard->keyReleased(Cappuccino::KeyEvent::B))
			done = false;

		if (lightMove) {

			if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::UP_ARROW)) {
				_lights.front()->setPosition(_lights.front()->getPosition() - glm::vec3(0.0f, 0.0f, 5.0f) * dt);
			}
			else if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::DOWN_ARROW)) {
				_lights.front()->setPosition(_lights.front()->getPosition() + glm::vec3(0.0f, 0.0f, 5.0f) * dt);
			}

			if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::LEFT_ARROW)) {
				_lights.front()->setPosition(_lights.front()->getPosition() - glm::vec3(5.0f, 0.0f, 0.0f) * dt);
			}
			else if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::RIGHT_ARROW)) {
				_lights.front()->setPosition(_lights.front()->getPosition() + glm::vec3(5.0f, 0.0f, 0.0f) * dt);
			}
		}
		else
			_lights.front()->setPosition(_lights.front()->getPosition() + glm::vec3(0.0f, sinf(eTime) / 10.0f, 0.0f));

	}

	{
		static bool done = false;
		if (_in.keyboard->keyPressed(Cappuccino::KeyEvent::L) && !done && _lights.size() < 25) {
			done = true;
			using namespace Cappuccino;
			_lights.push_back(new PointLightBody(_mainShader, glm::vec3(randomFloat(-5.0f, 5.0f), randomFloat(-5.0f, 5.0f), randomFloat(-5.0f, 5.0f)),
				glm::vec3(randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f))));
		}
		else if (_in.keyboard->keyReleased(Cappuccino::KeyEvent::L))
			done = false;
	}



	//_mainShader->loadViewMatrix(_c);
	_mainShader->use();
	_mainShader->setUniform("camPos", _c.getPosition());
	using namespace Cappuccino;
	Application::_gBufferShader->use();
	Application::_gBufferShader->loadViewMatrix(_c);

	for (auto x : _lights)
		x->update(dt);

	sendLights();

}

void MainScene::mouseFunction(double xpos, double ypos)
{
	static bool firstMouse = true;
	static float lastX = 400, lastY = 300;
	static float yaw = -90.0f;
	static float pitch = 0.0f;
	if (firstMouse)
	{
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	_c.doMouseMovement(xOffset, yOffset);
}

void MainScene::sendLights()
{
	static bool first = true;
	_mainShader->use();
	if (first) {
		first = false;
		//_mainShader->loadProjectionMatrix(1600.0f, 1000.0f);
		_mainShader->setUniform("gBuffer.gPos", 0);
		_mainShader->setUniform("gBuffer.gNormal", 1);
		_mainShader->setUniform("gBuffer.gAlbedo", 2);
		_mainShader->setUniform("gBuffer.gMetalRoughnessAO", 3);
		_mainShader->setUniform("gBuffer.gEmissive", 4);
	}
	unsigned inUseLightNum = 0;
	for (unsigned i = 0; i < _lights.size(); i++) {
		if (_lights[i]->_p._isActive)
			continue;
		_mainShader->setUniform("lights[" + std::to_string(i) + "].position", _lights[i]->getPosition());
		_mainShader->setUniform("lights[" + std::to_string(i) + "].colour", _lights[i]->_p._col);

		inUseLightNum++;
		_mainShader->setUniform("numLights", (int)inUseLightNum);

		//_mainShader->setUniform("lights[" + std::to_string(i) + "].active", _lights[i]._isActive);

	}
}

void MainScene::sendGBufferShaderUniforms()
{
	using namespace Cappuccino;
	Application::_gBufferShader->use();
	static bool first = true;

	if (first) {
		first = false;
		Application::_gBufferShader->loadProjectionMatrix(1600.0f, 1000.0f);
		Application::_gBufferShader->setUniform("material.albedo", (int)Cappuccino::TextureType::PBRAlbedo);
		Application::_gBufferShader->setUniform("material.normalMap", (int)Cappuccino::TextureType::PBRNormal);
		Application::_gBufferShader->setUniform("material.metallic", (int)Cappuccino::TextureType::PBRMetallic);
		Application::_gBufferShader->setUniform("material.roughness", (int)Cappuccino::TextureType::PBRRoughness);
		Application::_gBufferShader->setUniform("material.ambientOcc", (int)Cappuccino::TextureType::PBRAmbientOcc);
		Application::_gBufferShader->setUniform("material.emission", (int)Cappuccino::TextureType::PBREmission);
	}


}

Empty::Empty(Cappuccino::Shader* SHADER, const std::vector<Cappuccino::Texture*>& textures, const std::vector<Cappuccino::Mesh*>& meshes)
	:GameObject(*SHADER, textures, meshes), first(*_meshes.back()), last(*_meshes.back()), frame1("e", "Animations/Crawler/Crawler_kf1.obj"), frame2("ee", "Animations/Crawler/Crawler_kf2.obj"), frame3("eee", "Animations/Crawler/Crawler_kf3.obj")
{
	_meshes.back() = &first;
	_meshes.back()->loadFromData();


	frame1.loadMesh();
	frame2.loadMesh();
	frame3.loadMesh();
	last.loadFromData();

	_animator.addAnimation(new Cappuccino::Animation({
		&first,
		&frame2,
		&frame3,
		&frame3,
		&frame2,
		&frame1,
		&last }, AnimationType::Jump));
	_animator.setLoop(AnimationType::Jump, false);
	_animator.setSpeed(AnimationType::Jump, 5.0f);

	_animator.setAnimationShader(AnimationType::Jump, Cappuccino::Application::_gBufferShader);
}

void Empty::childUpdate(float dt)
{
}

PointLightBody::PointLightBody(Cappuccino::Shader* shader, const glm::vec3& position, const glm::vec3& colour)
	:_p(position, colour)
{
	_e = new Empty(shader,
		{ LOAD_TEXTURE("Bot Albedo", "Bot/Bot-Diffuse.png", Cappuccino::TextureType::PBRAlbedo) },
		{ Cappuccino::MeshLibrary::loadMesh("Sphere","Sphere.obj") });
	_e->setActive(true);
	_e->_rigidBody._position = position;
	_e->_transform.scale(glm::vec3(1.0f, 1.0f, 1.0f), 0.5f);
}

PointLightBody::~PointLightBody()
{
	delete _e;
}

void PointLightBody::update(float dt)
{
	_e->_transform.rotate(glm::vec3(0.0f, 1.0f, 1.0f), 90.0f * dt);
}

void PointLightBody::setPosition(const glm::vec3& pos)
{
	_p._pos = pos;
	_e->_rigidBody._position = pos;
}
