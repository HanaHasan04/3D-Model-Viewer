#pragma once

#include <glm/glm.hpp>
#include "Light.h"

class AmbientLight : public Light
{
public:
	AmbientLight(const Material& material);
	virtual ~AmbientLight();
};