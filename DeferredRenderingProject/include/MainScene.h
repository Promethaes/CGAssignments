#pragma once
#include "Cappuccino/SceneManager.h"
#include "Cappuccino/GameObject.h"
#include "Cappuccino/PointLight.h"
#include "Cappuccino/CappInput.h"
#include "Cappuccino/Application.h"

class Empty : public Cappuccino::GameObject {
public:
	Empty(Cappuccino::Shader* SHADER, const std::vector<Cappuccino::Texture*>& textures, const std::vector<Cappuccino::Mesh*>& meshes);

	void childUpdate(float dt) override;

	Cappuccino::Mesh first;
	Cappuccino::Mesh frame1;
	Cappuccino::Mesh frame2;
	Cappuccino::Mesh frame3;
	Cappuccino::Mesh last;
};

class PointLightBody {
public:
	PointLightBody(Cappuccino::Shader* shader, const glm::vec3& position, const glm::vec3& colour);
	~PointLightBody();

	void update(float dt);


	void setBodyVisible(bool yn) { _e->setVisible(yn); }
	void setPosition(const glm::vec3& pos);
	glm::vec3 getPosition() const { return _p._pos; }

	Cappuccino::PointLight _p;
	Empty* _e;

};

class MainScene : public Cappuccino::Scene {
public:
	MainScene(bool first);

	bool init() override;
	bool exit() override;

	void childUpdate(float dt) override;

	void mouseFunction(double xpos, double ypos);

	void sendLights();
	void sendGBufferShaderUniforms();

	std::vector<PointLightBody*> _lights;
	Cappuccino::Shader* _mainShader;

	Empty* _crawler;
	Empty* _bot;
	Empty* _cap;

	std::vector<Empty*> _empties;

	Cappuccino::CappInput _in;
	Cappuccino::Camera _c;
	bool lightsVisible = true;
	float lightsVisibleTimer = 0.0f;

	bool oneLight = false;
	float oneLightTimer = 0.0f;

	bool positionDepth = true;
	float positionDepthTimer = 0.0f;

	bool normal = true;
	float normalTimer = 0.0f;

	bool colourMaterial = true;
	float colourMaterialTimer = 0.0f;

	bool accumulation = true;
	float accumulationTimer = 0.0f;

	unsigned currentLight = 0;
	float currentLightTimer = 0.0f;

	bool warm = false;
	float warmTimer = 0.0f;

	bool cool = false;
	float coolTimer = 0.0f;

	bool custom = false;
	float customTimer = 0.0f;

};