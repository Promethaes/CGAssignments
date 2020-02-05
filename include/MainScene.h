#pragma once
#include "Cappuccino/SceneManager.h"
#include "Cappuccino/GameObject.h"
#include "Cappuccino/CappInput.h"
#include "LUTLoader.h"

class PointLight {
public:
	PointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float spec);


	float _constant = 1.0f;
	float _linear = 0.0001f;
	float _quadratic = 0.001f;

	glm::vec3 _position;
	glm::vec3 _diffuse;
	glm::vec3 _specular;
	glm::vec3 _ambient;
	float _spec;
private:
};

//empty game object, dont add any super fancy behaviour here
class Empty : public Cappuccino::GameObject {
public:
	Empty(const Cappuccino::Shader& SHADER, const std::vector<Cappuccino::Texture*>& textures, const std::vector<Cappuccino::Mesh*>& meshes);

	void childUpdate(float dt) override;

};

class MainScene : public Cappuccino::Scene {
public:
	MainScene(bool yn);

	void childUpdate(float dt) override;
	bool init() override;
	bool exit() override;

	void sendLights();

	void mouseFunction(double xpos, double ypos) override;
private:
	std::vector<PointLight> _lights;
	Cappuccino::CappInput _in;

	Cappuccino::Camera _c;
	Cappuccino::Shader* _mainShader = nullptr;
	Empty* _ghoul = nullptr;
	Empty* _gun = nullptr;
	std::vector<Empty*> _lamps;



	/*
	Booleans
	*/
	int _ambient = false;
	int _specular = false;
	float _diffuseMyTimer = 0.0f;
	int _diffuse = false;
	int _rim = false;
	int _diffuseRampShading = false;
	float _diffuseTimer = 0.0f;
	int _specularRampShading = false;
	float _specularTimer = 0.0f;
	int _warm = false;
	float _warmTimer = 0.0f;
	int _cool = false;
	float _coolTimer = 0.0f;
	int _custom = false;
	float _customTimer = 0.0f;

	Cappuccino::LUT lut = Cappuccino::LUT("Warm.CUBE");
	Cappuccino::LUT lut1 = Cappuccino::LUT("Cool.CUBE");
	Cappuccino::LUT lut2 = Cappuccino::LUT("Custom.CUBE");
	Cappuccino::LUT lut3 = Cappuccino::LUT("Blank.CUBE");
};