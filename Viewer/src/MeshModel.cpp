#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <glm/gtc/matrix_transform.hpp>

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName) :
	modelTransform(1),
	worldTransform(1),
	renderMode(0),
	textureMode(0),
	normalMapping(false),
	toonShading(false),
	modelName(modelName),
	shininess(30.f)
{
	material = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
	scale = { glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f) };
	rotation = { glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) };
	translation = { glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) };

	modelVertices.reserve(3 * faces.size());
	for (int i = 0; i < faces.size(); i++)
	{
		Face currentFace = faces.at(i);
		glm::vec3 tangent(0.f);

		//Calculations for Normal Mapping
		int p1Index = currentFace.GetVertexIndex(0) - 1;
		int p2Index = currentFace.GetVertexIndex(1) - 1;
		int p3Index = currentFace.GetVertexIndex(2) - 1;

		glm::vec3 p1 = vertices[p1Index];
		glm::vec3 p2 = vertices[p2Index];
		glm::vec3 p3 = vertices[p3Index];

		glm::vec3 edge1 = p2 - p1;
		glm::vec3 edge2 = p3 - p1;
		if (textureCoords.size() > 0)
		{
			glm::vec2 uv1 = textureCoords[p1Index];
			glm::vec2 uv2 = textureCoords[p2Index];
			glm::vec2 uv3 = textureCoords[p3Index];

			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		}

		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = currentFace.GetVertexIndex(j) - 1;

			Vertex vertex;
			vertex.position = vertices[vertexIndex];
			vertex.normal = normals[vertexIndex];
			vertex.tangent = tangent;


			if (textureCoords.size() > 0)
			{
				int textureCoordsIndex = currentFace.GetTextureIndex(j) - 1;
				vertex.textureCoords = textureCoords[textureCoordsIndex];
			}

			modelVertices.push_back(vertex);
		}
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Tangents
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
}

MeshModel::MeshModel(const MeshModel& model)
{
	modelName = model.modelName;
	modelTransform = model.modelTransform;
	worldTransform = model.worldTransform;
	material = model.material;
	scale = model.scale;
	rotation = model.rotation;
	translation = model.translation;
	shininess = model.shininess;
	renderMode = model.renderMode;
	toonShading = model.toonShading;
	textureMode = model.textureMode;
	normalMapping = model.normalMapping;

	modelVertices = model.modelVertices;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Tangents
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
}

MeshModel::~MeshModel()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void MeshModel::SwitchToTextureCoords()
{
	textureMode = 0;
}

void MeshModel::SwitchToPlane()
{
	textureMode = 1;
}

void MeshModel::SwitchToCylindrical()
{
	textureMode = 2;
}

void MeshModel::SwitchToSpherical()
{
	textureMode = 3;
}

int MeshModel::GetTextureMode() const
{
	return textureMode;
}

bool MeshModel::IsNormalMappingActivated() const
{
	return normalMapping;
}

void MeshModel::ToggleNormalMapping()
{
	normalMapping = !normalMapping;
}

bool MeshModel::IsTextureCoordsActivated() const
{
	return textureMode == 0;
}

bool MeshModel::IsPlaneActivated() const
{
	return textureMode == 1;
}

bool MeshModel::IsCylindricalActivated() const
{
	return textureMode == 2;
}

bool MeshModel::IsSphericalActivated() const
{
	return textureMode == 3;
}

bool MeshModel::IsToonShadingActivated() const
{
	return toonShading;
}

bool MeshModel::IsMaterialActivated() const
{
	return renderMode == 0;
}

bool MeshModel::IsTextureActivated() const
{
	return renderMode == 1;
}

bool MeshModel::IsReflectionActivated() const
{
	return renderMode == 2;
}

void MeshModel::SwitchToMaterial()
{
	renderMode = 0;
}

void MeshModel::SwitchToTexture()
{
	renderMode = 1;
}


void MeshModel::SwitchToReflection()
{
	renderMode = 2;
}

void MeshModel::ToggleToonShading()
{
	toonShading = !toonShading;
}

void MeshModel::ScaleModel(float factor)
{
	scale[1] *= glm::vec3(factor, factor, factor);
	UpdateTransform();
}

void MeshModel::UpdateTransform()
{
	std::vector<glm::mat4> transformations = { glm::mat4(1.0f), glm::mat4(1.0f) };

	for (int i = 0; i < translation.size(); i++)
	{
		transformations[i] *= glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			translation[i].x, translation[i].y, translation[i].z, 1
		);
	}

	for (int i = 0; i < rotation.size(); i++)
	{
		float xRadians = glm::radians(rotation[i].x);
		float yRadians = glm::radians(rotation[i].y);
		float zRadians = glm::radians(rotation[i].z);

		transformations[i] *= Utils::ZRotationMatrix(zRadians);
		transformations[i] *= Utils::XRotationMatrix(xRadians);
		transformations[i] *= Utils::YRotationMatrix(yRadians);
	}

	for (int i = 0; i < scale.size(); i++)
	{
		transformations[i] *= glm::mat4(
			scale[i].x, 0.0f, 0.0f, 0.0f,
			0.0f, scale[i].y, 0.0f, 0.0f,
			0.0f, 0.0f, scale[i].z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}


	worldTransform = transformations[0];
	modelTransform = transformations[1];
}

void MeshModel::SetMaterial(const ModelMaterial& material)
{
	this->material = material;
}

const ModelMaterial& MeshModel::GetMaterial() const
{
	return material;
}

void MeshModel::SetShininess(float shininess)
{
	this->shininess = shininess;
}

float MeshModel::GetShininess() const
{
	return shininess;
}

const std::string& MeshModel::GetModelName() const
{
	return modelName;
}

void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	this->worldTransform = worldTransform;
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	return worldTransform;
}

void MeshModel::SetModelTransformation(const glm::mat4x4& modelTransform)
{
	this->modelTransform = modelTransform;
}

const glm::mat4x4& MeshModel::GetModelTransformation() const
{
	return modelTransform;
}

void MeshModel::ApplyTransform(const std::vector<glm::vec3>& translation, const std::vector<glm::vec3>& rotation, const std::vector<glm::vec3>& scale)
{
	this->translation = translation;
	this->rotation = rotation;
	this->scale = scale;

	UpdateTransform();
}

const std::vector<glm::vec3>& MeshModel::GetTranslation() const
{
	return translation;
}

const std::vector<glm::vec3>& MeshModel::GetRotation() const
{
	return rotation;
}

const std::vector<glm::vec3>& MeshModel::GetScale() const
{
	return scale;
}

GLuint MeshModel::GetVAO() const
{
	return vao;
}

const std::vector<Vertex>& MeshModel::GetModelVertices() const
{
	return modelVertices;
}