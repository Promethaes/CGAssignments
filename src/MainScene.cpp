#include "MainScene.h"

MainScene::MainScene(bool yn)
	:Cappuccino::Scene(yn), _in(true, std::nullopt)
{
	_c.setPosition(_c.getPosition() + glm::vec3(0.0f, 1.0f, 0.0f));
}


void MainScene::childUpdate(float dt)
{
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

	//load view matrix
	_mainShader->use();
	_mainShader->loadViewMatrix(_c);


	static float elapsedTime = 0.0f;
	elapsedTime += dt;

	//_lights.back()._position.x = sinf(elapsedTime);
	//sendLights();
	////
	//_ghoul->_rigidBody._position.y = sinf(elapsedTime);
	//_bullet->_rigidBody._position.x = cosf(elapsedTime);


}

bool MainScene::init()
{
	if (_lights.empty())
		_lights.push_back(PointLight(glm::vec3(-1.0f, 2.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 64.0f));
	_lamps.clear();
	
	//make the shader
	if (_mainShader == nullptr) {
		_mainShader = new Cappuccino::Shader("mainVert.vert", "mainFrag.frag");
		_mainShader->use();
		_mainShader->loadProjectionMatrix(1600.0f, 1000.0f);
		_mainShader->setUniform("material.diffuse", 0);
		_mainShader->setUniform("material.specular", 1);
		_mainShader->setUniform("material.normalMap", 2);
		_mainShader->setUniform("material.emissionMap", 3);
		_mainShader->setUniform("material.heightMap", 4);
	}

	for (unsigned i = 0; i < _lights.size(); i++) {
		_lamps.push_back(new Empty(*_mainShader, { new Cappuccino::Texture("red.png", Cappuccino::TextureType::DiffuseMap) }, { new Cappuccino::Mesh("FromPrimordial/bullet.obj") }));
		_lamps.back()->_rigidBody._position = _lights[i]._position;
		_lamps.back()->setActive(true);
	}

	sendLights();

	//set the cursor so that its locked to the window
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//initialize the ghoul and the bullet
	if (_ghoul == nullptr) {
		_ghoul = new Empty(*_mainShader, { new Cappuccino::Texture("CrawlerDiffuse.png",Cappuccino::TextureType::DiffuseMap),
			new Cappuccino::Texture("CrawlerNorm.png",Cappuccino::TextureType::NormalMap) }, { new Cappuccino::Mesh("FromPrimordial/Crawler.obj") });
	}
	if (_gun == nullptr) {
		_gun = new Empty(*_mainShader, { new Cappuccino::Texture("autoRifleDiffuse.png",Cappuccino::TextureType::DiffuseMap),
			new Cappuccino::Texture("autoRifleNormal.png",Cappuccino::TextureType::NormalMap),new Cappuccino::Texture("autoRifleEmission.png",Cappuccino::TextureType::EmissionMap)
			}, { new Cappuccino::Mesh("FromPrimordial/autoRifle.obj") });
		_gun->_rigidBody._position += glm::vec3(1.0f, 1.0f, 1.0f);
		_gun->_transform.scale(glm::vec3(1.0f, 1.0f, 1.0f), 1.5f);
	}

	_ghoul->setActive(true);
	_gun->setActive(true);

	_initialized = true;
	_shouldExit = false;
	return true;
}

bool MainScene::exit()
{
	_ghoul->setActive(false);
	_gun->setActive(false);

	_initialized = false;
	_shouldExit = true;
	return false;
}

void MainScene::sendLights()
{
	_mainShader->use();
	_mainShader->setUniform("numLights", (int)_lights.size());

	for (unsigned i = 0; i < _lights.size(); i++) {
		_mainShader->setUniform("pointLight[" + std::to_string(i) + "].position", _lights[i]._position);

		_mainShader->setUniform("pointLight[" + std::to_string(i) + "].ambient", _lights[i]._ambient);
		_mainShader->setUniform("pointLight[" + std::to_string(i) + "].specular", _lights[i]._specular);
		_mainShader->setUniform("pointLight[" + std::to_string(i) + "].diffuse", _lights[i]._diffuse);

		_mainShader->setUniform("pointLight[" + std::to_string(i) + "].constant", _lights[i]._constant);
		_mainShader->setUniform("pointLight[" + std::to_string(i) + "].linear", _lights[i]._linear);
		_mainShader->setUniform("pointLight[" + std::to_string(i) + "].quadratic", _lights[i]._quadratic);

		_mainShader->setUniform("material.shininess", _lights[i]._spec);
	}
}

void MainScene::mouseFunction(double xpos, double ypos)
{
	static bool firstMouse = true;
	static float lastX = 400, lastY = 300;
	static float yaw = -90.0f;
	static float pitch = 0.0f;
	if (firstMouse)
	{
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

Empty::Empty(const Cappuccino::Shader& SHADER, const std::vector<Cappuccino::Texture*>& textures, const std::vector<Cappuccino::Mesh*>& meshes)
	:Cappuccino::GameObject(SHADER, textures, meshes)
{
	_rigidBody.setGrav(false);
}

void Empty::childUpdate(float dt)
{
	//_transform.rotate(glm::vec3(0.0f, 1.0f, 0.0f), dt * 90.0f);
}

PointLight::PointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float spec)
	:_position(position), _diffuse(diffuse), _specular(specular), _spec(spec), _ambient(ambient)
{
}
