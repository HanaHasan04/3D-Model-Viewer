#include "Light.h"
#include "Utils.h"


Light::Light(const Material& material) :
	material(material)
{
	ambientStrength = 0.5f;
	specularStrength = 0.5f;
	diffuseStrength = 0.5f;
}

const Material& Light::GetMaterial() const
{
	return material;
}

float Light::GetAmbientStrength() const
{
	return ambientStrength;
}

float Light::GetSpecularStrength() const
{
	return specularStrength;
}

float Light::GetDiffuseStrength() const
{
	return diffuseStrength;
}

void Light::SetAmbientStrength(float s)
{
	ambientStrength = s;
}

void Light::SetSpecularStrength(float s)
{
	specularStrength = s;
}

void Light::SetDiffuseStrength(float s)
{
	diffuseStrength = s;
}

void Light::SetMaterial(const Material& material)
{
	this->material = material;
}