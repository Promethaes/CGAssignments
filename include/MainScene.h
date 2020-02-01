#pragma once
#include "Cappuccino/SceneManager.h"
#include "Cappuccino/GameObject.h"
#include "Cappuccino/CappInput.h"

//empty game object, dont add any super fancy behaviour here
class Empty : public Cappuccino::GameObject {
public:
	Empty(const Cappuccino::Shader& SHADER, const std::vector<Cappuccino::Texture*>& textures, const std::vector<Cappuccino::Mesh*>& meshes);

	void childUpdate(float dt) override;

	//only works if the shader has a "colour" uniform
	void setColour(const glm::vec3& colour);
};

class MainScene : public Cappuccino::Scene {
public:
	MainScene(bool yn);

	void childUpdate(float dt) override;
	bool init() override;
	bool exit() override;

	void mouseFunction(double xpos, double ypos) override;
private:
	Cappuccino::CappInput _in;

	Cappuccino::Camera _c;
	Cappuccino::Shader* _mainShader = nullptr;
	Empty* _ghoul = nullptr;
	Empty* _bullet = nullptr;

};