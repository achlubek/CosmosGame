#pragma once

enum class SubdividedMeshQuality
{
    Low,
    Medium,
    High
};

class SubdividedMeshesProvider
{
public:
    SubdividedMeshesProvider(VulkanToolkit* vulkan);
    ~SubdividedMeshesProvider();

    std::vector<std::tuple<glm::vec3, Object3dInfo*>>& getPatches(SubdividedMeshQuality quality);
    Object3dInfo* getIcosphere(SubdividedMeshQuality quality);
private:
    VulkanToolkit * vulkan;

    std::vector<std::tuple<glm::vec3, Object3dInfo*>> patchesLowPoly = {};
    std::vector<std::tuple<glm::vec3, Object3dInfo*>> patchesMediumPoly = {};
    std::vector<std::tuple<glm::vec3, Object3dInfo*>> patchesHighPoly = {};

    Object3dInfo* subdivide(Object3dInfo* info);
    std::vector<Object3dInfo*> splitTriangles(Object3dInfo* info);

    Object3dInfo* icosphereLow;
    Object3dInfo* icosphereMedium;
    Object3dInfo* icosphereHigh;
};
