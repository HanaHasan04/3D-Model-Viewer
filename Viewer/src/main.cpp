#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>

#include <nfd.h>

#include <stdio.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Utils.h"

double zoomFactor = 1;
int windowWidth = 1600;
int windowHeight = 900;
float lightCubeFactor = 0.25f;
char* windowTitle = "MeshViewer";
glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);
bool zoomChanged = false;
std::shared_ptr<Scene> scene;

ImGuiIO* imgui;
GLFWwindow* window;

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupImgui(GLFWwindow* window);
bool Setup(int windowWidth, int windowHeight, const char* windowName);
void Cleanup();

static void GlfwErrorCallback(int error, const char* description);

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height);

float GetAspectRatio();
void DrawImguiMenus();
void HandleImguiInput();

int main(int argc, char** argv)
{

	if (!Setup(windowWidth, windowHeight, windowTitle))
	{
		std::cerr << "Setup failed" << std::endl;
		return -1;
	}

	scene = std::make_shared<Scene>();
	glm::vec3 eye = glm::vec3(0, 0, 10);
	glm::vec3 at = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	Camera camera = Camera(eye, at, up, GetAspectRatio());
	scene->AddCamera(std::make_shared<Camera>(camera));

	std::shared_ptr<MeshModel> cubeModel = Utils::LoadMeshModel("..\\Data\\box.obj");
	cubeModel->ScaleModel(lightCubeFactor);
	scene->AddLight(std::make_shared<PointLight>(PointLight(*cubeModel, { glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1) })));

	Renderer renderer;
	renderer.LoadShaders();
	renderer.LoadTextures();

	while (!glfwWindowShouldClose(window))
	{
		// Poll and process events
		glfwPollEvents();

		// Imgui stuff
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawImguiMenus();
		ImGui::Render();
		HandleImguiInput();

		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene
		renderer.Render(*scene);

		// Imgui stuff
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool Setup(int windowWidth, int windowHeight, const char* windowName)
{
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, windowName);
	if (!window)
	{
		std::cerr << "Window setup failed" << std::endl;
		return false;
	}

	imgui = &SetupImgui(window);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glEnable(GL_DEPTH_TEST);

	return true;
}

GLFWwindow* SetupGlfwWindow(int windowWidth, int windowHeight, const char* windowName)
{
	// Intialize GLFW
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create an OpenGL 3.3 core, forward compatible context window
	window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(window);

	// Setup window events callbacks
	glfwSetFramebufferSizeCallback(window, glfw_OnFramebufferSize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// An error occured
		std::cerr << "GLAD initialization failed" << std::endl;
		return false;
	}

	return window;
}

ImGuiIO& SetupImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();

	glfwSetScrollCallback(window, glfw_OnMouseScroll);

	return io;
}

void HandleImguiInput()
{

	if (!imgui->WantCaptureKeyboard && scene->GetModelCount() > 0)
	{
		if (imgui->KeysDown[45]) // -
		{
			scene->GetActiveModel().ScaleModel(1.f / 1.05f);
		}
		if (imgui->KeysDown[61]) // +
		{
			scene->GetActiveModel().ScaleModel(1.01);
		}
	}

	if (!imgui->WantCaptureMouse)
	{
		if (zoomChanged)
		{
			scene->GetActiveCamera().Zoom(zoomFactor);
			zoomChanged = false;
		}
	}
}

void Cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
	scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
}

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	zoomFactor = glm::pow(1.1, -yoffset);
	zoomChanged = true;
}

float GetAspectRatio()
{
	return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}

void DrawImguiMenus()
{
	// ******************************** Main Menu Bar ********************************
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					scene->AddModel(Utils::LoadMeshModel(outPath));
					free(outPath);
				}
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
				}

			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}


	// ******************************** Background Control ********************************
	ImGui::Begin("Background");
	bool skyBoxActivated = scene->IsSkyBoxActivated();
	if (ImGui::Checkbox("Skybox", &skyBoxActivated))
	{
		scene->ToggleSkyBoxActivated();
	}
	ImGui::End();


	// ******************************** Cameras Control ********************************
	if (scene->GetCameraCount() > 0)
	{
		ImGui::Begin("Cameras", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PushItemWidth(400);

		int current_camera = scene->GetActiveCameraIndex();

		if (ImGui::Button("Add"))
		{
			glm::vec3 eye = glm::vec3(0, 0, 10);
			glm::vec3 at = glm::vec3(0, 0, 0);
			glm::vec3 up = glm::vec3(0, 1, 0);
			scene->AddCamera(std::make_shared<Camera>(Camera(eye, at, up, GetAspectRatio())));
		}

		static const char* projection_type = "Perspective";
		std::string types[2] = { "Perspective", "Orthographic" };

		if (ImGui::BeginCombo("Projection Type", projection_type))
		{
			for (int i = 0; i < 2; i++)
			{
				bool is_selected = (projection_type == types[i].c_str());
				if (ImGui::Selectable(types[i].c_str(), is_selected))
				{
					projection_type = types[i].c_str();
					if (projection_type == types[0])
						scene->GetActiveCamera().SwitchToPrespective();
					else
						scene->GetActiveCamera().SwitchToOrthographic();
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		const glm::vec3& cameraEyeLocation = scene->GetActiveCamera().GetTranslationVector();
		float cameraEye[3] = { cameraEyeLocation.x, cameraEyeLocation.y, cameraEyeLocation.z };
		const glm::vec3& cameraRotation = scene->GetActiveCamera().GetRotationVector();
		float localCameraRotation[3] = { cameraRotation.x, cameraRotation.y, cameraRotation.z };
		float fov = scene->GetActiveCamera().GetFovy();
		float zNear = scene->GetActiveCamera().GetNear();
		float zFar = scene->GetActiveCamera().GetFar();

		const char** cameraCStrings = new const char* [scene->GetCameraCount()];
		std::vector<std::string> cameraStrings;

		for (int i = 0; i < scene->GetCameraCount(); i++)
		{
			std::ostringstream s;
			s << "Camera " << i;
			std::string mystring = s.str();
			cameraStrings.push_back(mystring);
		}

		for (int i = 0; i < scene->GetCameraCount(); i++)
		{
			cameraCStrings[i] = cameraStrings[i].c_str();
		}

		ImGui::Combo("Active Camera", &current_camera, cameraCStrings, scene->GetCameraCount());

		if (current_camera != scene->GetActiveCameraIndex())
		{
			scene->SetActiveCameraIndex(current_camera);
			scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
		}

		delete[] cameraCStrings;

		if (ImGui::SliderFloat3("Translate", cameraEye, 0.0f, 100.0f, "%.3f"))
			scene->GetActiveCamera().ApplyTransform(glm::vec3(cameraEye[0], cameraEye[1], cameraEye[2]), glm::vec3(localCameraRotation[0], localCameraRotation[1], localCameraRotation[2]));
		if (ImGui::SliderFloat3("Rotate", localCameraRotation, 0.0f, 180.0f, "%.3f"))
			scene->GetActiveCamera().ApplyTransform(glm::vec3(cameraEye[0], cameraEye[1], cameraEye[2]), glm::vec3(localCameraRotation[0], localCameraRotation[1], localCameraRotation[2]));
		ImGui::SetNextItemWidth(173);
		if (ImGui::SliderFloat("zNear", &zNear, 0.f, 1.f, "%.3f"))
			scene->GetActiveCamera().SetNear(zNear);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(173);
		if (ImGui::SliderFloat("zFar", &zFar, 30.0f, 800.0f, "%.3f"))
			scene->GetActiveCamera().SetFar(zFar);

		if (scene->GetActiveCamera().IsPrespective())
		{
			if (ImGui::SliderFloat("FOV", &fov, 0.0f, 2.5f, "%.3f"))
				scene->GetActiveCamera().SetFovy(fov);
		}
	}

	ImGui::End();


	// ******************************** Models Control ********************************
	ImGui::Begin("Models", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::PushItemWidth(400);

	static const char* current_item = nullptr;
	if (scene->GetModelCount() > 0)
		current_item = scene->GetActiveModel().GetModelName().c_str();

	if (ImGui::Button("Add Model"))
	{
		nfdchar_t* outPath = NULL;
		nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
		if (result == NFD_OKAY)
		{
			scene->AddModel(Utils::LoadMeshModel(outPath));
			free(outPath);
		}
		else if (result == NFD_CANCEL)
		{
		}
		else
		{
		}
	}

	int currentModel = scene->GetModelCount() == 0 ? -1 : scene->GetActiveModelIndex();

	const char** modelCStrings = new const char* [scene->GetModelCount()];
	std::vector<std::string> modelStrings;

	for (int i = 0; i < scene->GetModelCount(); i++)
	{
		std::ostringstream s;
		s << scene->GetModel(i).GetModelName();
		std::string mystring = s.str();
		modelStrings.push_back(mystring);
		modelCStrings[i] = modelStrings[i].c_str();
	}

	ImGui::Combo("Active Model", &currentModel, modelCStrings, scene->GetModelCount());

	if (scene->GetModelCount() > 0 && currentModel != scene->GetActiveModelIndex())
	{
		scene->SetActiveModelIndex(currentModel);
	}

	delete[] modelCStrings;

	static const char* transformation_type = "Local";
	std::string types[2] = { "Local", "World" };

	if (ImGui::BeginCombo("Transformation Type", transformation_type))
	{
		for (int i = 0; i < 2; i++)
		{
			bool is_selected = (transformation_type == types[i].c_str());
			if (ImGui::Selectable(types[i].c_str(), is_selected))
			{
				transformation_type = types[i].c_str();
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (scene->GetModelCount() != 0 && current_item)
	{
		MeshModel& activeModel = scene->GetActiveModel();

		const std::vector<glm::vec3>& modelScale = activeModel.GetScale();
		const std::vector<glm::vec3>& modelTranslation = activeModel.GetTranslation();
		const std::vector<glm::vec3>& modelRotation = activeModel.GetRotation();

		float localScale[3] = { modelScale[1].x, modelScale[1].y, modelScale[1].z };
		float localTranslation[3] = { modelTranslation[1].x, modelTranslation[1].y, modelTranslation[1].z };
		float localRotation[3] = { modelRotation[1].x, modelRotation[1].y, modelRotation[1].z };

		float worldScale[3] = { modelScale[0].x, modelScale[0].y, modelScale[0].z };
		float worldTranslation[3] = { modelTranslation[0].x, modelTranslation[0].y, modelTranslation[0].z };
		float worldRotation[3] = { modelRotation[0].x, modelRotation[0].y, modelRotation[0].z };

		std::vector<glm::vec3> newTranslation = { glm::vec3(worldTranslation[0], worldTranslation[1], worldTranslation[2]), glm::vec3(localTranslation[0], localTranslation[1], localTranslation[2]) };
		std::vector<glm::vec3> newScale = { glm::vec3(worldScale[0], worldScale[1], worldScale[2]), glm::vec3(localScale[0], localScale[1], localScale[2]) };
		std::vector<glm::vec3> newRotation = { glm::vec3(worldRotation[0], worldRotation[1], worldRotation[2]), glm::vec3(localRotation[0], localRotation[1], localRotation[2]) };

		if (transformation_type == types[0])
		{
			if (ImGui::SliderFloat3("Scale", localScale, 0.0f, 100.0f, "%.3f"))
			{
				newScale = { glm::vec3(worldScale[0], worldScale[1], worldScale[2]), glm::vec3(localScale[0], localScale[1], localScale[2]) };
				activeModel.ApplyTransform(newTranslation, newRotation, newScale);
			}

			if (ImGui::SliderFloat3("Rotate", localRotation, 0.0f, 360.0f, "%.3f"))
			{
				newRotation = { glm::vec3(worldRotation[0], worldRotation[1], worldRotation[2]), glm::vec3(localRotation[0], localRotation[1], localRotation[2]) };
				activeModel.ApplyTransform(newTranslation, newRotation, newScale);
			}

			if (ImGui::SliderFloat3("Translation", localTranslation, 0.0f, 100.0f, "%.3f"))
			{
				newTranslation = { glm::vec3(worldTranslation[0], worldTranslation[1], worldTranslation[2]), glm::vec3(localTranslation[0], localTranslation[1], localTranslation[2]) };
				activeModel.ApplyTransform(newTranslation, newRotation, newScale);
			}

		}
		else
		{
			if (ImGui::SliderFloat3("Scale", worldScale, 0.0f, 100.0f, "%.3f"))
			{
				newScale = { glm::vec3(worldScale[0], worldScale[1], worldScale[2]), glm::vec3(localScale[0], localScale[1], localScale[2]) };
				activeModel.ApplyTransform(newTranslation, newRotation, newScale);
			}

			if (ImGui::SliderFloat3("Rotate", worldRotation, 0.0f, 360.0f, "%.3f"))
			{
				newRotation = { glm::vec3(worldRotation[0], worldRotation[1], worldRotation[2]), glm::vec3(localRotation[0], localRotation[1], localRotation[2]) };
				activeModel.ApplyTransform(newTranslation, newRotation, newScale);
			}

			if (ImGui::SliderFloat3("Translation", worldTranslation, 0.0f, 100.f, "%.3f"))
			{
				newTranslation = { glm::vec3(worldTranslation[0], worldTranslation[1], worldTranslation[2]), glm::vec3(localTranslation[0], localTranslation[1], localTranslation[2]) };
				activeModel.ApplyTransform(newTranslation, newRotation, newScale);
			}
		}


		// ******************************** Textures Control ********************************
		ImGui::Begin("Textures", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		bool modelTextureActivated = activeModel.IsTextureActivated();
		bool modelMaterialActivated = activeModel.IsMaterialActivated();
		bool modelReflectionActivated = activeModel.IsReflectionActivated();
		bool modelToonShadingActivated = activeModel.IsToonShadingActivated();


		if (ImGui::RadioButton("Material", modelMaterialActivated))
			activeModel.SwitchToMaterial();
		ImGui::SameLine();
		if (ImGui::RadioButton("Texture", modelTextureActivated))
			activeModel.SwitchToTexture();
		ImGui::SameLine();
		if (ImGui::RadioButton("Reflection", modelReflectionActivated))
			activeModel.SwitchToReflection();

		if (activeModel.IsTextureActivated())
		{
			bool textureCoordsActivated = activeModel.IsTextureCoordsActivated();
			bool planeTextureActivated = activeModel.IsPlaneActivated();
			bool cylindricalTextureActivated = activeModel.IsCylindricalActivated();
			bool sphericalTextureActivated = activeModel.IsSphericalActivated();
			bool normalMappingActivated = activeModel.IsNormalMappingActivated();

			if (ImGui::RadioButton("Texture Coords", textureCoordsActivated))
				activeModel.SwitchToTextureCoords();
			ImGui::SameLine();
			if (ImGui::RadioButton("Plane", planeTextureActivated))
				activeModel.SwitchToPlane();
			ImGui::SameLine();
			if (ImGui::RadioButton("Cylinder", cylindricalTextureActivated))
				activeModel.SwitchToCylindrical();
			ImGui::SameLine();
			if (ImGui::RadioButton("Sphere", sphericalTextureActivated))
				activeModel.SwitchToSpherical();

			if (ImGui::Checkbox("Normal Mapping", &normalMappingActivated))
				activeModel.ToggleNormalMapping();
			ImGui::SameLine();
		}


		if (!activeModel.IsReflectionActivated())
		{
			if (ImGui::Checkbox("Toon Shading", &modelToonShadingActivated))
				activeModel.ToggleToonShading();
		}


		if (!activeModel.IsTextureActivated() && !activeModel.IsReflectionActivated())
		{
			const ModelMaterial& modelMaterial = activeModel.GetMaterial();
			float model_amb[3] = { modelMaterial.ambient.x, modelMaterial.ambient.y, modelMaterial.ambient.z };
			float model_diff[3] = { modelMaterial.diffuse.x, modelMaterial.diffuse.y, modelMaterial.diffuse.z };
			float model_spec[3] = { modelMaterial.specular.x, modelMaterial.specular.y, modelMaterial.specular.z };
			float shininess = activeModel.GetShininess();


			ImGui::Text("Material Properties:");
			ModelMaterial newMaterial = { glm::vec3(model_amb[0], model_amb[1], model_amb[2]), glm::vec3(model_spec[0], model_spec[1], model_spec[2]), glm::vec3(model_diff[0], model_diff[1], model_diff[2]) };

			if (ImGui::ColorEdit3("Ambient", model_amb))
			{
				newMaterial.ambient = glm::vec3(model_amb[0], model_amb[1], model_amb[2]);
				activeModel.SetMaterial(newMaterial);
			}

			if (ImGui::ColorEdit3("Diffuse", model_diff))
			{
				newMaterial.diffuse = glm::vec3(model_diff[0], model_diff[1], model_diff[2]);
				activeModel.SetMaterial(newMaterial);
			}

			if (ImGui::ColorEdit3("Specular", model_spec))
			{
				newMaterial.specular = glm::vec3(model_spec[0], model_spec[1], model_spec[2]);
				activeModel.SetMaterial(newMaterial);
			}

			if (ImGui::SliderFloat("Shininess", &shininess, 10.f, 100.0f))
				activeModel.SetShininess(shininess);
		}
		ImGui::End();
	}

	ImGui::End();


	// ******************************** Lights Control ********************************
	ImGui::Begin("Lights", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	static int current_light = 0;

	ImGui::Text("Lights:");

	const char** lightCStrings = new const char* [scene->GetLightCount()];
	std::vector<std::string> lightStrings;

	for (int i = 0; i < scene->GetLightCount(); i++)
	{
		std::ostringstream s;
		s << "Light " << i;
		std::string mystring = s.str();
		lightStrings.push_back(mystring);
	}

	for (int i = 0; i < scene->GetLightCount(); i++)
	{
		lightCStrings[i] = lightStrings[i].c_str();
	}

	ImGui::Combo("Active Light", &current_light, lightCStrings, scene->GetLightCount());

	delete[] lightCStrings;


	if (scene->GetLightCount() > 0)
	{
		std::shared_ptr<Light> light = scene->GetLight(current_light);

		const Material& lightMaterial = light->GetMaterial();
		float amb[3] = { lightMaterial.ambient.x, lightMaterial.ambient.y, lightMaterial.ambient.z };
		float diff[3] = { lightMaterial.diffuse.x, lightMaterial.diffuse.y, lightMaterial.diffuse.z };
		float spec[3] = { lightMaterial.specular.x, lightMaterial.specular.y, lightMaterial.specular.z };
		float Astrength = light->GetAmbientStrength(), Dstrength = light->GetDiffuseStrength(), Sstrength = light->GetSpecularStrength();

		std::shared_ptr<PointLight> pointLight;

		if (pointLight = std::dynamic_pointer_cast<PointLight>(light)) 
		{
			static const char* light_transformation_type = "Local";
			std::string light_types[2] = { "Local", "World" };

			if (ImGui::BeginCombo("Transformation Type", light_transformation_type))
			{
				for (int i = 0; i < 2; i++)
				{
					bool is_selected = (light_transformation_type == light_types[i].c_str());
					if (ImGui::Selectable(light_types[i].c_str(), is_selected))
					{
						light_transformation_type = light_types[i].c_str();
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (scene->GetLightCount() > 0)
			{
				const std::vector<glm::vec3>& lightScale = pointLight->GetScale();
				const std::vector<glm::vec3>& lightTranslation = pointLight->GetTranslation();
				const std::vector<glm::vec3>& lightRotation = pointLight->GetRotation();

				float lightLocalScale[3] = { lightScale[1].x, lightScale[1].y, lightScale[1].z };
				float lightLocalTranslation[3] = { lightTranslation[1].x, lightTranslation[1].y, lightTranslation[1].z };
				float lightLocalRotation[3] = { lightRotation[1].x, lightRotation[1].y, lightRotation[1].z };

				float lightWorldScale[3] = { lightScale[0].x, lightScale[0].y, lightScale[0].z };
				float lightWorldTranslation[3] = { lightTranslation[0].x, lightTranslation[0].y, lightTranslation[0].z };
				float lightWorldRotation[3] = { lightRotation[0].x, lightRotation[0].y, lightRotation[0].z };

				std::vector<glm::vec3> newTranslation = { glm::vec3(lightWorldTranslation[0], lightWorldTranslation[1], lightWorldTranslation[2]), glm::vec3(lightLocalTranslation[0], lightLocalTranslation[1], lightLocalTranslation[2]) };
				std::vector<glm::vec3> newScale = { glm::vec3(lightWorldScale[0], lightWorldScale[1], lightWorldScale[2]), glm::vec3(lightLocalScale[0], lightLocalScale[1], lightLocalScale[2]) };
				std::vector<glm::vec3> newRotation = { glm::vec3(lightWorldRotation[0], lightWorldRotation[1], lightWorldRotation[2]), glm::vec3(lightLocalRotation[0], lightLocalRotation[1], lightLocalRotation[2]) };

				if (light_transformation_type == light_types[0])
				{
					if (ImGui::SliderFloat3("Scale", lightLocalScale, 0.0f, 10.f, "%.3f"))
					{
						newScale = { glm::vec3(lightLocalScale[0], lightLocalScale[1], lightLocalScale[2]), glm::vec3(lightLocalScale[0], lightLocalScale[1], lightLocalScale[2]) };
						pointLight->ApplyTransform(newTranslation, newRotation, newScale);
					}

					if (ImGui::SliderFloat3("Rotate", lightLocalRotation, 0.0f, 6.283f, "%.3f"))
					{
						newRotation = { glm::vec3(lightLocalRotation[0], lightLocalRotation[1], lightLocalRotation[2]), glm::vec3(lightLocalRotation[0], lightLocalRotation[1], lightLocalRotation[2]) };
						pointLight->ApplyTransform(newTranslation, newRotation, newScale);
					}

					if (ImGui::SliderFloat3("Translate", lightLocalTranslation, 0.0f, 10.f, "%.3f"))
					{
						newTranslation = { glm::vec3(lightLocalTranslation[0], lightLocalTranslation[1], lightLocalTranslation[2]), glm::vec3(lightLocalTranslation[0], lightLocalTranslation[1], lightLocalTranslation[2]) };
						pointLight->ApplyTransform(newTranslation, newRotation, newScale);
					}
				}
				else
				{
					if (ImGui::SliderFloat3("Scale", lightWorldScale, 0.0f, 10.f, "%.3f"))
					{
						newScale = { glm::vec3(lightWorldScale[0], lightWorldScale[1], lightWorldScale[2]), glm::vec3(lightLocalScale[0], lightLocalScale[1], lightLocalScale[2]) };
						pointLight->ApplyTransform(newTranslation, newRotation, newScale);
					}

					if (ImGui::SliderFloat3("Rotate", lightWorldRotation, 0.0f, 6.283f, "%.3f"))
					{
						newRotation = { glm::vec3(lightWorldRotation[0], lightWorldRotation[1], lightWorldRotation[2]), glm::vec3(lightLocalRotation[0], lightLocalRotation[1], lightLocalRotation[2]) };
						pointLight->ApplyTransform(newTranslation, newRotation, newScale);
					}

					if (ImGui::SliderFloat3("Translate", lightWorldTranslation, 0.0f, 10.f, "%.3f"))
					{
						newTranslation = { glm::vec3(lightWorldTranslation[0], lightWorldTranslation[1], lightWorldTranslation[2]), glm::vec3(lightLocalTranslation[0], lightLocalTranslation[1], lightLocalTranslation[2]) };
						pointLight->ApplyTransform(newTranslation, newRotation, newScale);
					}
				}
			}
		}
		else
		{
			static float direction[3] = { 1.f, 1.f, 1.f };
			ImGui::SliderFloat3("Direction", direction, -50.f, 50.0f, "%.3f");
		}

		ImGui::Text("Light Source Colors: ");

		Material newMaterial = { glm::vec3(amb[0], amb[1], amb[2]), glm::vec3(spec[0], spec[1], spec[2]), glm::vec3(diff[0], diff[1], diff[2]) };

		if (ImGui::ColorEdit3("Ambient Reflection", amb))
		{
			newMaterial.ambient = glm::vec3(amb[0], amb[1], amb[2]);
			light->SetMaterial(newMaterial);
		}

		if (ImGui::SliderFloat("Ambient Strength", &Astrength, 0.0f, 1.0f))
			light->SetAmbientStrength(Astrength);

		if (ImGui::ColorEdit3("Diffuse Reflection", diff))
		{
			newMaterial.diffuse = glm::vec3(diff[0], diff[1], diff[2]);
			light->SetMaterial(newMaterial);
		}

		if (ImGui::SliderFloat("Diffuse Strength", &Dstrength, 0.0f, 1.0f))
			light->SetDiffuseStrength(Dstrength);

		if (ImGui::ColorEdit3("Specular Reflection", spec))
		{
			newMaterial.specular = glm::vec3(spec[0], spec[1], spec[2]);
			light->SetMaterial(newMaterial);
		}

		if (ImGui::SliderFloat("Specular Strength", &Sstrength, 0.0f, 1.0f))
			light->SetSpecularStrength(Sstrength);
	}


	ImGui::End();
}