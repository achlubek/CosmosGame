#pragma once
class FrustumCone;
class Camera
{
public:
    Camera();
    ~Camera();

    void createProjectionPerspective(float fov, float aspectRatio, float nearpl, float farpl);
    void createProjectionOrthogonal(float minx, float miny, float minz, float maxx, float maxy, float maxz);

    float getFarPlane();
    float getFocalLength();
    float getFov();
    FrustumCone* getFrustumCone();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getRotationProjectionMatrix();
    glm::quat getOrientation();
    glm::dvec3 getPosition();

    void setOrientation(glm::quat orient);
    void setPosition(glm::dvec3 pos);

    void updateFrustumCone();

private:

    float farPlane;
    float focalLength;
    float fov;
    FrustumCone *cone;
    glm::mat4 projectionMatrix;
    glm::quat orientation;
    glm::dvec3 position;
    void updateFocalLength();
};
