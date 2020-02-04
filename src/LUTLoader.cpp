#include <glad/glad.h>
#include <glm/glm.hpp>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "LUTLoader.h"
#include "Cappuccino/CappMacros.h"
#include "Cappuccino/ResourceManager.h"

namespace Cappuccino {

	std::string LUT::_lutDirectory = "./Assets/LUTs/";

	LUT::LUT(const std::string& path)
	{
		this->_path = path;
	}
	bool LUT::loadLUT()
	{
		if (loaded)
			return true;
		char inputString[128];

		std::string titleData;
		unsigned int sizeData;
		glm::vec3 domainMinData;
		glm::vec3 domainMaxData;
		std::vector<glm::vec3> lookupData;
		//loading file
		std::ifstream input{};
		input.open(_lutDirectory + _path);

		if (!input.good()) {
			std::cout << "Problem loading file: " << _path << "\n";
			return false;
		}
		//import data
		while (!input.eof()) {
			input.getline(inputString, 128);

			//LUT title
			if (inputString[0] == 'T') {
				std::sscanf(inputString, "TITLE \"%s\"", titleData);
			}
			else if (inputString[0] == 'L') {
				std::sscanf(inputString, "LUT_3D_SIZE %i", sizeData);
			}
			else if (inputString[7] == 'M' && inputString[8] == 'I' && inputString[9] == 'N') {
				std::sscanf(inputString, "DOMAIN_MIN %f %f %f", domainMinData.x, domainMinData.y, domainMinData.z);
			}
			else if (inputString[7] == 'M' && inputString[8] == 'A' && inputString[9] == 'X') {
				std::sscanf(inputString, "DOMAIN_MAX %f %f %f", domainMaxData.x, domainMaxData.y, domainMaxData.z);
			}
			else if (inputString[0] == '0' || inputString[0] == '1') {
				glm::vec3 lutData{ 0 };
				std::sscanf(inputString, "%f %f %f", lutData.x, lutData.y, lutData.z);
				lookupData.push_back(lutData);
			}
			else
				continue;
		}

		_lutName = titleData;
		_lutSize = sizeData;
		_domainMin.push_back( domainMinData);
		_domainMax.push_back( domainMaxData);

		for (unsigned int i = 0; i < lookupData.size(); i++)
			_rgbValues.push_back(lookupData[i]);

		input.close();
		return loaded = true;
	}

	void LUT::setDefaultPath(const std::string& directory)
	{
		std::string dir = directory;
		std::transform(dir.begin(), dir.end(), dir.begin(), ::tolower);

		if (dir == "default")
			_lutDirectory = "./Assets/LUTs/";
		else
			_lutDirectory = directory;
	}
}