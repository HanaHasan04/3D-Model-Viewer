#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Light.h"

class PointLight : public Light, public MeshModel
{
public:
	PointLight(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName, const Material& material);
	PointLight(const MeshModel& model, const Material material);
	virtual ~PointLight();
};