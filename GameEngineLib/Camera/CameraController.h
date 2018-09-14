#pragma once
class GameObject;
class AbsCameraViewStrategy;
class Camera;
class CameraController
{
public:
    CameraController();
    ~CameraController();
    float getFov();
    void setFov(float fov);
    GameObject* getTarget();
    void setTarget(GameObject* obj);
    void lookAt(glm::dvec3 point);
    void lookAtDirection(glm::dvec3 dir);
    void update(double elapsed);
    Camera* getCamera();
    void setStrategy(AbsCameraViewStrategy* strategy);
private:
    GameObject * target{ nullptr };
    Camera* camera;
    AbsCameraViewStrategy* activeViewStrategy;
};

