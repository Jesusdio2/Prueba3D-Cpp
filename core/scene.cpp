#include "scene.h"
#include <iostream>

Scene::Scene() {}
Scene::~Scene() {}

void Scene::LoadTestScene() {
    std::cout << "Escena de prueba cargada." << std::endl;
}

void Scene::Update(float deltaTime) {
    std::cout << "Actualizando escena. DeltaTime: " << deltaTime << std::endl;
}