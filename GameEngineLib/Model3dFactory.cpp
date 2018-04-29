#include "stdafx.h"
#include "Model3dFactory.h"
#include "Model3d.h"
#include "AbsGameContainer.h"
#include "ModelsRenderer.h"


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
    auto info3d = reader.gets("info3d_file");
    auto albedoImage = reader.gets("albedo_image");
    auto normalImage = reader.gets("normal_image");
    auto roughnessImage = reader.gets("roughness_image");
    auto metalnessImage = reader.gets("metalness_image");
    auto emissionIdleImage = reader.gets("emission_idle_image");
    auto emissionPoweredImage = reader.gets("emission_powered_image");

    auto vulkan = AbsGameContainer::getInstance()->getVulkanToolkit();
    auto modeldset = AbsGameContainer::getInstance()->getModelsRenderer()->getModelMRTLayout();

    auto orientationCorrection = axes_vec3toquat(glm::dvec3(reader.getf("rotx"), reader.getf("roty"), reader.getf("rotz")));
    return new Model3d(vulkan, modeldset, info3d, albedoImage, normalImage, roughnessImage, metalnessImage, emissionIdleImage, emissionPoweredImage, orientationCorrection);
}
