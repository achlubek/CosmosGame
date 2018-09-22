#include "stdafx.h"
#include "SubdividedMeshesProvider.h"

SubdividedMeshesProvider::SubdividedMeshesProvider(VulkanToolkit* vulkan)
    : vulkan(vulkan)
{
    auto wholeIcoMesh = vulkan->getObject3dInfoFactory()->build("icosphere_to_separate.raw");
    auto splitMesh = splitTriangles(wholeIcoMesh);
    for (int i = 0; i < splitMesh.size(); i++) {
        auto vbo = splitMesh[i]->getVBO();
        int g = 0;
        glm::vec3 v1 = glm::normalize(glm::vec3(vbo[g], vbo[g + 1], vbo[g + 2]));
        g += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(vbo[g], vbo[g + 1], vbo[g + 2]));
        g += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(vbo[g], vbo[g + 1], vbo[g + 2]));

        glm::vec3 dir = glm::normalize((v1 + v2 + v3) / glm::vec3(3.0));
        auto low = subdivide(splitMesh[i]);
        auto medium = subdivide(low);
        auto high = subdivide(subdivide(medium));
        patchesLowPoly.push_back({ dir, low });
        patchesMediumPoly.push_back({ dir, medium });
        patchesHighPoly.push_back({ dir, high });
    }


    icosphereLow = vulkan->getObject3dInfoFactory()->build("icosphere_mediumpoly_1unit.raw");

    icosphereMedium = vulkan->getObject3dInfoFactory()->build("icosphere_mediumpoly_1unit.raw");

    icosphereHigh = subdivide(icosphereMedium);// vulkan->getObject3dInfoFactory->build("icosphere_highpoly_1unit.raw");
}

SubdividedMeshesProvider::~SubdividedMeshesProvider()
{
}

std::vector<std::tuple<glm::vec3, Object3dInfo*>>& SubdividedMeshesProvider::getPatches(SubdividedMeshQuality quality)
{
    if (quality == SubdividedMeshQuality::Low)
    {
        return patchesLowPoly;
    }
    if(quality == SubdividedMeshQuality::Medium)
    {
        return patchesMediumPoly;
    }
    if(quality == SubdividedMeshQuality::High)
    {
        return patchesHighPoly;
    }
}

Object3dInfo * SubdividedMeshesProvider::getIcosphere(SubdividedMeshQuality quality)
{
    if (quality == SubdividedMeshQuality::Low)
    {
        return icosphereLow;
    }
    if (quality == SubdividedMeshQuality::Medium)
    {
        return icosphereMedium;
    }
    if (quality == SubdividedMeshQuality::High)
    {
        return icosphereHigh;
    }
}

VEngine::Renderer::Object3dInfo* SubdividedMeshesProvider::subdivide(VEngine::Renderer::Object3dInfo* info)
{
    std::vector<float> floats = {};
    auto vbo = info->getVBO();
    for (int i = 0; i < vbo.size();) {
        glm::vec3 v1 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 tricenter = glm::normalize((v1 + v2 + v3) * glm::vec3(0.33333333));
        glm::vec3 v1tov2 = glm::normalize((v1 + v2) * glm::vec3(0.5));
        glm::vec3 v2tov3 = glm::normalize((v2 + v3) * glm::vec3(0.5));
        glm::vec3 v3tov1 = glm::normalize((v1 + v3) * glm::vec3(0.5));

        std::vector<glm::vec3> positions = {};
        positions.push_back(v3tov1);
        positions.push_back(v1);
        positions.push_back(v1tov2);

        positions.push_back(v1tov2);
        positions.push_back(v2);
        positions.push_back(v2tov3);

        positions.push_back(v2tov3);
        positions.push_back(v3);
        positions.push_back(v3tov1);

        positions.push_back(v1tov2);
        positions.push_back(v2tov3);
        positions.push_back(v3tov1);

        for (int g = 0; g < positions.size(); g++) {
            glm::vec3 v = positions[g];
            // px py pz ux uy nx ny nz tx ty tz tw | px
            // 0  1  2  3  4  5  6  7  8  9  10 11 | 12
            floats.push_back(v.x);
            floats.push_back(v.y);
            floats.push_back(v.z);

            floats.push_back(v.x);
            floats.push_back(v.y);

            floats.push_back(v.x);
            floats.push_back(v.y);
            floats.push_back(v.z);

            floats.push_back(v.x);
            floats.push_back(v.y);
            floats.push_back(v.z);
            floats.push_back(v.x);
        }
    }
    return vulkan->getObject3dInfoFactory()->build(floats);
}

std::vector<VEngine::Renderer::Object3dInfo*> SubdividedMeshesProvider::splitTriangles(
    VEngine::Renderer::Object3dInfo* info)
{
    std::vector<Object3dInfo*> objs = {};

    auto vbo = info->getVBO();
    for (int i = 0; i < vbo.size();) {
        glm::vec3 v1 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;

        auto buffer = std::vector<float>{
            v1.x,
            v1.y,
            v1.z,
            v1.x,
            v1.y,
            v1.x,
            v1.y,
            v1.z,
            v1.x,
            v1.y,
            v1.z,
            v1.x,

            v2.x,
            v2.y,
            v2.z,
            v2.x,
            v2.y,
            v2.x,
            v2.y,
            v2.z,
            v2.x,
            v2.y,
            v2.z,
            v2.x,

            v3.x,
            v3.y,
            v3.z,
            v3.x,
            v3.y,
            v3.x,
            v3.y,
            v3.z,
            v3.x,
            v3.y,
            v3.z,
            v3.x
        };
        objs.push_back(vulkan->getObject3dInfoFactory()->build(buffer));
    }
    return objs;
}
