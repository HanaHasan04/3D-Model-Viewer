#pragma once
#include <istream>
#include "Face.h"
#include <glm/glm.hpp>

Face::Face(std::istream& issLine)
{
	vertex_indices_ = { 0,0,0 };
	normal_indices_ = { 0,0,0 };
	texture_indices_ = { 0,0,0 };

	char c;
	for (int i = 0; i < 3; i++)
	{
		issLine >> std::ws >> vertex_indices_.at(i) >> std::ws;
		if (issLine.peek() != '/')
		{
			continue;
		}

		issLine >> c >> std::ws;
		if (issLine.peek() == '/')
		{
			issLine >> c >> std::ws >> normal_indices_.at(i);
			continue;
		}

		issLine >> texture_indices_.at(i);
		if (issLine.peek() != '/')
		{
			continue;
		}

		issLine >> c >> normal_indices_.at(i);
	}
}

int Face::GetVertexIndex(int internal_index) const
{
	return vertex_indices_[internal_index];
}

int Face::GetNormalIndex(int internal_index) const
{
	return normal_indices_[internal_index];
}

int Face::GetTextureIndex(int internal_index) const
{
	return texture_indices_[internal_index];
}

void Face::SetColor(const glm::vec3& color)
{
	this->color = color;
}

const glm::vec3& Face::GetColor() const
{
	return color;
}