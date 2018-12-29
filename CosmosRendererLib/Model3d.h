#pragma once

class Model3d
{
public: 
    Model3d(ToolkitInterface* toolkit,
        DescriptorSetLayoutInterface* descriptorSetLayout,
        std::string info3d_file,
        std::string albedo_image, 
        std::string normal_image, 
        std::string roughness_image, 
        std::string metalness_image, 
        std::string emission_idle_image, 
        std::string emission_powered_image,
        glm::dquat orientationCorrection,
        double modelScale);
    ~Model3d();
    void draw(RenderStageInterface* stage, DescriptorSetInterface* celestialSet, glm::dvec3 position, glm::dquat orientation, double scale, int id, double emission);
private:
    ToolkitInterface* toolkit;
    Object3dInfoInterface* info3d;
    std::string info3d_file;

    std::string albedo_image;
    std::string normal_image;
    std::string roughness_image;
    std::string metalness_image;
    std::string emission_idle_image;
    std::string emission_powered_image;
    double modelScale;

    GenericBufferInterface* dataBuffer;
    ImageInterface* albedoImage;
    ImageInterface* normalImage;
    ImageInterface* roughnessImage;
    ImageInterface* metalnessImage;
    ImageInterface* emissionIdleImage;
    ImageInterface* emissionPoweredImage;
    DescriptorSetInterface* descriptorSet;
    glm::dquat orientationCorrection;
};

