#pragma once
class Model3d;
class Model3dFactory
{
public:
    Model3dFactory(VEngine::FileSystem::Media * media);
    ~Model3dFactory();

    Model3d* build(std::string mediakey);
private:
    VEngine::FileSystem::Media * media;
};

