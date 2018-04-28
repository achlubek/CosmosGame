#include "stdafx.h"
#include "Model3dFactory.h"
#include "Model3d.h"
#include "GameContainer.h"
#include "CosmosRenderer.h"


Model3dFactory::Model3dFactory()
{
}


Model3dFactory::~Model3dFactory()
{
}

glm::dquat axes_vec3toquat(glm::dvec3 rot) {
    glm::mat3 xrot = glm::mat3_cast(glm::angleAxis(deg2rad(rot.x), glm::dvec3(1.0, 0.0, 0.0)));
    glm::mat3 zrot = glm::mat3_cast(glm::angleAxis(deg2rad(rot.y), glm::dvec3(0.0, 1.0, 0.0)));
    glm::mat3 yrot = glm::mat3_cast(glm::angleAxis(deg2rad(rot.z), glm::dvec3(0.0, 0.0, 1.0)));
    return glm::dquat(xrot * yrot * zrot);
}

Model3d* Model3dFactory::build(std::string mediakey)
{
    INIReader reader = INIReader(mediakey);
    auto info3d = reader.gets("model");
    auto albedoImage = reader.gets("albedo");
    auto normalImage = reader.gets("normal");
    auto roughnessImage = reader.gets("roughness");
    auto metalnessImage = reader.gets("metalness");
    auto emissionIdleImage = reader.gets("emission_idle");
    auto emissionPoweredImage = reader.gets("emission_powered");

    auto vulkan = GameContainer::getInstance()->getVulkanToolkit();
    auto modeldset = GameContainer::getInstance()->getCosmosRenderer()->modelMRTLayout;

    auto orientationCorrection = axes_vec3toquat(glm::dvec3(reader.getf("rotx"), reader.getf("roty"), reader.getf("rotz")));
    return new Model3d(vulkan, modeldset, info3d, albedoImage, normalImage, roughnessImage, metalnessImage, emissionIdleImage, emissionPoweredImage, orientationCorrection);
}
