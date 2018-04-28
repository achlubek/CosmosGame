#pragma once
class Model3d;
class Model3dFactory
{
public:
    Model3dFactory();
    ~Model3dFactory();

    Model3d* build(std::string mediakey);
};

