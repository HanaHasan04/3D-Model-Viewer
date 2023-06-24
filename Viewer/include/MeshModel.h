#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Face.h"

struct ModelMaterial
{
	glm::vec3 ambient;
	glm::vec3 specular;
	glm::vec3 diffuse;
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
	glm::vec3 tangent;
};

//enum TextureMapping {
//    PLANAR,
//    CYLINDRICAL,
//    SPHERICAL,
//    COLOR
//};

class MeshModel
{
protected:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> textureCoords;

	std::vector<Vertex> modelVertices;

	glm::mat4x4 modelTransform;
	glm::mat4x4 worldTransform;

	std::vector<glm::vec3> scale;
	std::vector<glm::vec3> rotation;
	std::vector<glm::vec3> translation;

	std::string modelName;

	ModelMaterial material;
	float shininess;
	int renderMode;
	int textureMode;
	bool normalMapping;

	bool toonShading;

	GLuint vbo;
	GLuint vao;

public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName = "");
	MeshModel(const MeshModel& model);
	virtual ~MeshModel();

	const glm::mat4x4& GetWorldTransformation() const;
	const glm::mat4x4& GetModelTransformation() const;

	void SetWorldTransformation(const glm::mat4x4& worldTransform);
	void SetModelTransformation(const glm::mat4x4& modelTransform);

	void UpdateTransform();

	const ModelMaterial& GetMaterial() const;
	void SetMaterial(const ModelMaterial& material);

	const std::string& GetModelName() const;

	const std::vector<Vertex>& GetModelVertices() const;

	const std::vector<glm::vec3>& GetTranslation() const;
	const std::vector<glm::vec3>& GetRotation() const;
	const std::vector<glm::vec3>& GetScale() const;

	void ScaleModel(float factor);

	void SetShininess(float shininess);
	float GetShininess() const;

	void SwitchToTexture();
	void SwitchToMaterial();
	void SwitchToReflection();
	bool IsTextureActivated() const;
	bool IsMaterialActivated() const;
	bool IsReflectionActivated() const;

	bool IsToonShadingActivated() const;
	void ToggleToonShading();

	void SwitchToTextureCoords();
	void SwitchToPlane();
	void SwitchToCylindrical();
	void SwitchToSpherical();
	bool IsPlaneActivated() const;
	bool IsCylindricalActivated() const;
	bool IsSphericalActivated() const;
	bool IsTextureCoordsActivated() const;

	int GetTextureMode() const;
	bool IsNormalMappingActivated() const;
	void ToggleNormalMapping();

	void ApplyTransform(const std::vector<glm::vec3>& translation, const std::vector<glm::vec3>& rotation, const std::vector<glm::vec3>& scale);

	GLuint GetVAO() const;
};