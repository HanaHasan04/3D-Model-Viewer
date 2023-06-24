#include "Scene.h"
#include "MeshModel.h"
#include <string>

Scene::Scene() :
	active_camera_index_(0),
	active_model_index_(0)
{

}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	mesh_models_.push_back(mesh_model);
}

void Scene::RemoveActiveModel()
{
	std::shared_ptr<MeshModel>& temp = mesh_models_.back();
	mesh_models_[active_model_index_] = temp;
	mesh_models_.pop_back();
}

int Scene::GetModelCount() const
{
	return mesh_models_.size();
}

MeshModel& Scene::GetModel(int index) const
{
	return *mesh_models_[index];
}

MeshModel& Scene::GetActiveModel() const
{
	return *mesh_models_[active_model_index_];
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
	cameras_.push_back(camera);
}

void Scene::RemoveActiveCamera()
{
	if (cameras_.size() < 2)
		return;
	std::shared_ptr<Camera>& temp = cameras_.back();
	cameras_[active_camera_index_] = temp;
	cameras_.pop_back();
}

int Scene::GetCameraCount() const
{
	return cameras_.size();
}

Camera& Scene::GetCamera(int index)
{
	return *cameras_[index];
}

const Camera& Scene::GetActiveCamera() const
{
	return *cameras_[active_camera_index_];
}

Camera& Scene::GetActiveCamera()
{
	return *cameras_[active_camera_index_];
}

void Scene::AddLight(const std::shared_ptr<Light>& light)
{
	lights_.push_back(light);
}

int Scene::GetLightCount() const
{
	return lights_.size();
}

std::shared_ptr<Light> Scene::GetLight(int index)
{
	return lights_[index];
}

const std::shared_ptr<Light> Scene::GetLight(int index) const
{
	return lights_[index];
}

void Scene::SetActiveCameraIndex(int index)
{
	active_camera_index_ = index;
}

int Scene::GetActiveCameraIndex() const
{
	return active_camera_index_;
}

void Scene::SetActiveModelIndex(int index)
{
	active_model_index_ = index;
}

int Scene::GetActiveModelIndex() const
{
	return active_model_index_;
}

bool Scene::IsSkyBoxActivated() const
{
	return skyBoxActivated;
}

void Scene::ToggleSkyBoxActivated()
{
	skyBoxActivated = !skyBoxActivated;
}