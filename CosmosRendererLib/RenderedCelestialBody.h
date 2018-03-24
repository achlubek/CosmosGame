#pragma once
class CelestialBody;
/*
Idea here:
fields:
CelestialBody the subject
some VulkanImages to hold the data
updateSet - set used by compute shader to update the data in textures using imageStore
renderSet - a set containing the images and some data that will be used in rendering the object
2 functions:
    update Body Data :
        - takes computeStage, binsd the set, deploys the computation, done
        - should be split into 2 functions - hi frequency and low frequency
    render:
        - takes a render stage, a object (a cube probably! can be a sphere ico but not smaller than atmo radius)
        - binds the set and deploys the draw
WARNING:
The constructor MUST initialzie the images
The destructor MUST destroy the images!!!

The images:
    low freq:
        heightMap (R_16F) - Only update if body is of type No or Light atmosphere
        baseColor (RGBA)8) A channel is roughness - this color is used for sufrace in No and Light atmo bodies, and in thick atmo its used for atmosphere color (very important)
    hi freq:
        clouds (RG_8) - Density of High and Low clouds - ONLY used in Light atmosphere bodies!

The
*/
class RenderedCelestialBody
{
public:
    RenderedCelestialBody(VulkanToolkit* toolkit, CelestialBody body, 
		VulkanDescriptorSetLayout* dataSetLayout, VulkanDescriptorSetLayout* shadowMapSetLayout, VulkanDescriptorSetLayout* renderSetLayout,
		VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout,
		VulkanDescriptorSetLayout* celestialBodyWaterSetLayout);
    ~RenderedCelestialBody();
	void updateData(VulkanComputeStage* stage);
	void updateShadows(VulkanComputeStage* stage, VulkanDescriptorSet* rendererDataSet);
    void draw(VulkanRenderStage* stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo* info3d);
    void updateBuffer(glm::dvec3 observerPosition, double scale, double time);
    double getDistance(glm::dvec3 position, double at_time);
	void resizeDataImages(int lowFreqWidth, int lowFreqHeight, int hiFreqWidth, int hiFreqHeight);
	bool needsDataUpdate();
private:

	int lowFreqWidth = 0;
	int lowFreqHeight = 0;

	int hiFreqWidth = 0;
	int hiFreqHeight = 0;

	bool initialized = false;
	bool needsUpdate = true;

	int shadowMapWidthOffset = 0;

    VulkanToolkit * toolkit;

    CelestialBody body;

    VulkanImage* heightMapImage = nullptr;
    VulkanImage* baseColorImage = nullptr;
    VulkanImage* cloudsImage = nullptr;

	VulkanImage* shadowMapImage = nullptr;

    VulkanGenericBuffer* dataBuffer;

	VulkanDescriptorSet* dataSet;

	VulkanDescriptorSet* shadowMapSet;

	VulkanDescriptorSet* renderSet;

	VulkanDescriptorSet* renderSurfaceSet;

	VulkanDescriptorSet* renderWaterSet;

};

