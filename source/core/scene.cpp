#include "core/scene.h"
#include "textures/color-texture.h"
#include "tools/constants.h"

#include <limits>
#include <glm/gtx/norm.hpp>

Scene::Scene(Skybox* skybox) : m_skybox(skybox), m_nextID(0) { }

void Scene::addEntity(const TriangleMesh* mesh, const Material* material) {
	m_objectIndices.push_back(m_entities.size());
	const std::unique_ptr<Triangle>* triangles = mesh->getTriangles();
	for (size_t i = 0; i < mesh->getNumberOfTriangles(); ++i) {
		if (!triangles[i]->isDegenerate()) {
			m_entities.push_back(std::make_unique<Entity>(triangles[i].get(), material, m_nextID));
			m_nextID++;
		}
	}
}

void Scene::addLight(const TriangleMesh* mesh, const EmissionMaterial* emissionMat) {
	m_objectIndices.push_back(m_entities.size());
	const std::unique_ptr<Triangle>* triangles = mesh->getTriangles();
	for (size_t i = 0; i < mesh->getNumberOfTriangles(); ++i) {
		if (!triangles[i]->isDegenerate()) {
			auto le = std::make_unique<LightEntity>(triangles[i].get(), emissionMat, m_nextID);
			m_lights.push_back(le.get());
			m_entities.push_back(std::move(le));
			m_nextID++;
		}
	}
}

void Scene::initializeAccelerator() {
	m_accelerator.initialize(m_entities.data(), m_entities.size(), m_objectIndices);
}

void Scene::fillIntersectionFromEntity(const EntityIntersection& entityIntersection,
	Intersection* result) const {
		entityIntersection.entity->fillMeshIntersection(entityIntersection.triangleWeights.w0,
			entityIntersection.triangleWeights.w1, entityIntersection.triangleWeights.w2, result);
		result->hit = true;
		result->light = entityIntersection.entity->getLight();
		result->intersectionPoint.meshID = entityIntersection.entity->getID();
		result->material = entityIntersection.entity->getMaterial();
}

void Scene::intersects(Ray* ray, Intersection* result) const {
	EntityIntersection entityIntersection;
	entityIntersection.t = std::numeric_limits<float>::max();
	result->wo = -ray->direction;
	ray->createRaySpace();
	if (m_accelerator.intersects(*ray, &entityIntersection)) {
		fillIntersectionFromEntity(entityIntersection, result);
		result->calculateScreenDifferentials(*ray);
		return;
	}
	result->light = nullptr;
	result->hit = false;
}

void Scene::intersects(const SurfacePoint& surface, const Vector3& direction, Intersection* result) const {
	Ray ray(surface.point, direction);
	EntityIntersection entityIntersection;
	entityIntersection.t = std::numeric_limits<float>::max();
	result->wo = -ray.direction;
	ray.createRaySpace();
	if (m_accelerator.intersects(ray, surface, &entityIntersection)) {
		fillIntersectionFromEntity(entityIntersection, result);
		result->calculateScreenDifferentials(ray);
		return;
	}
	result->light = nullptr;
	result->hit = false;
}
