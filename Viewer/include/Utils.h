#pragma once
#include <glm/glm.hpp>
#include <string>
#include "MeshModel.h"

struct Material
{
	glm::vec3 ambient;
	glm::vec3 specular;
	glm::vec3 diffuse;
};

class Utils
{
public:
	static glm::mat4x4 XRotationMatrix(double angle);
	static glm::mat4x4 YRotationMatrix(double angle);
	static glm::mat4x4 ZRotationMatrix(double angle);

	static glm::vec3 Vec3fFromStream(std::istream& issLine);
	static glm::vec2 Vec2fFromStream(std::istream& issLine);

	static std::shared_ptr<MeshModel> LoadMeshModel(const std::string& filePath);

	static std::vector<glm::vec3> CalculateNormals(std::vector<glm::vec3> vertices, std::vector<Face> faces);

	static std::string GetFileName(const std::string& filePath);
};