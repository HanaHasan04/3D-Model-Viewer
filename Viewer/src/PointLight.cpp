#include "PointLight.h"
#include <algorithm>

PointLight::PointLight(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName, const Material& material) :
	Light(material),
	MeshModel(faces, vertices, normals, textureCoords, modelName)
{
}

PointLight::PointLight(const MeshModel& model, const Material material) : Light(material), MeshModel(model) {}

PointLight::~PointLight()
{
}