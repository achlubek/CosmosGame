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
    SubdividedMeshesProvider(ToolkitInterface* vulkan);
    ~SubdividedMeshesProvider();

    std::vector<std::tuple<glm::vec3, Object3dInfoInterface*>>& getPatches(SubdividedMeshQuality quality);
    Object3dInfoInterface* getIcosphere(SubdividedMeshQuality quality);
private:
    ToolkitInterface * toolkit;

    std::vector<std::tuple<glm::vec3, Object3dInfoInterface*>> patchesLowPoly = {};
    std::vector<std::tuple<glm::vec3, Object3dInfoInterface*>> patchesMediumPoly = {};
    std::vector<std::tuple<glm::vec3, Object3dInfoInterface*>> patchesHighPoly = {};

    Object3dInfoInterface* subdivide(Object3dInfoInterface* info);
    std::vector<Object3dInfoInterface*> splitTriangles(Object3dInfoInterface* info);

    Object3dInfoInterface* icosphereLow;
    Object3dInfoInterface* icosphereMedium;
    Object3dInfoInterface* icosphereHigh;
};
