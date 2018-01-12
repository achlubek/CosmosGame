#pragma once
class GameObject;
class AbsCameraViewStrategy;
class CameraController
{
public:
    CameraController();
    ~CameraController();
    float getFov();
    void setFov(float fov);
    GameObject* getTarget();
    void setTarget(GameObject* obj);
    glm::dvec3 getPosition();
    void setPosition(glm::dvec3 pos);
    void lookAt(glm::dvec3 point);
    void update(double elapsed);
    Camera* getInternalCamera();
private:
    GameObject * target{ nullptr };
    glm::dvec3 position;
    Camera* internalCamera; // to save some calculations
    AbsCameraViewStrategy* activeViewStrategy;
};

