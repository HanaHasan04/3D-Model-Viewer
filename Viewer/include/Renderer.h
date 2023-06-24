#pragma once
#include "Scene.h"
#include "ShaderProgram.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <memory>
#include "Texture2D.h"

class Renderer
{
private:
	ShaderProgram lightShader;
	ShaderProgram colorShader;
	ShaderProgram skyBoxShader;
	ShaderProgram reflectionShader;
	Texture2D texture1;
	Texture2D texture2;
	Texture2D normalTexture;
	std::shared_ptr<MeshModel> skyBoxModel;

public:
	Renderer();
	~Renderer();

	void Render(const Scene& scene);
	void LoadShaders();
	void LoadTextures();

	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void DrawCircle(glm::ivec2& center, int r);
	void Draw(const MeshModel& m);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void SetViewport(int width, int height);
	void DrawWorldFrame(const Scene& scene);
	void DrawModelFrame(Scene& scene, const MeshModel& model);
	void DrawLongLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	glm::vec2 min_max_in_2D_arr(float** mat);
	void RecursiveCircle(glm::ivec2& center, int r, int a, glm::vec3& color);
	void PutPixel(int i, int j, const glm::vec3& color, float z = 0);
};
