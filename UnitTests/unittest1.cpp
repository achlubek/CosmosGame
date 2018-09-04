#include "stdafx.h"
#include "CppUnitTest.h"

#include "glm\glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "AbsCelestialObject.h"
#include "Star.h"
#include "CelestialBody.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(UnitTest1)
    {
    public:

        TEST_METHOD(TestGravityStrength)
        {
            Star star = {};
            star.radius = 12345.0;
            star.x = 0;
            star.y = 0;
            star.z = 0;
            CelestialBody body = {};
            body.hostDistance = 63700000.1;
            body.orbitSpeed = 112.0;
            body.radius = 6371.0;
            body.orbitPlane = glm::normalize(glm::vec3(0.01, 1.2, 0.2));
            body.host = &star;

            auto bodypos = body.getPosition(1234.0);

            Logger::WriteMessage(("Body position (X: "
                + std::to_string(bodypos.x)
                + " , Y: "
                + std::to_string(bodypos.y)
                + " , Z: "
                + std::to_string(bodypos.z)
                + " )\n").c_str());

            glm::dvec3 cosmosPositionZero = bodypos + glm::dvec3(0.0, 0.0000001, 0.0);
            glm::dvec3 cosmosPositionSurface = bodypos + glm::dvec3(0.0, body.radius, 0.0);
            glm::dvec3 cosmosPosition100KM = bodypos + glm::dvec3(0.0, body.radius + 100.0, 0.0);
            glm::dvec3 cosmosPosition1000KM = bodypos + glm::dvec3(0.0, body.radius + 1000.0, 0.0);

            double expectedAtZero = 9.81;
            double expectedAtSurface = 9.81;
            double expectedAt100KM = 9.50;
            double expectedAt1000KM = 7.32;

            double resultAtZero = glm::length(body.getGravity(cosmosPositionZero, 1234.0));
            double resultAtSurface = glm::length(body.getGravity(cosmosPositionSurface, 1234.0));
            double resultAt100KM = glm::length(body.getGravity(cosmosPosition100KM, 1234.0));
            double resultAt1000KM = glm::length(body.getGravity(cosmosPosition1000KM, 1234.0));

            Logger::WriteMessage(("resultAtZero " + std::to_string(resultAtZero) + "\n").c_str());
            Logger::WriteMessage(("resultAtSurface " + std::to_string(resultAtSurface) + "\n").c_str());
            Logger::WriteMessage(("resultAt100KM " + std::to_string(resultAt100KM) + "\n").c_str());
            Logger::WriteMessage(("resultAt1000KM " + std::to_string(resultAt1000KM) + "\n").c_str());

            Assert::IsTrue(glm::abs(resultAtZero - expectedAtZero) < 0.01);
            Assert::IsTrue(glm::abs(resultAtSurface - expectedAtSurface) < 0.01);
            Assert::IsTrue(glm::abs(expectedAt100KM - expectedAt100KM) < 0.01);
            Assert::IsTrue(glm::abs(expectedAt1000KM - expectedAt1000KM) < 0.01);

        }

        TEST_METHOD(TestOrbitalVelocity)
        {
            Star star = {};
            star.radius = 12345.0;
            star.x = 0;
            star.y = 0;
            star.z = 0;
            CelestialBody body = {};
            body.hostDistance = 63700000.1;
            body.orbitSpeed = 112.0;
            body.radius = 6371.0;
            body.orbitPlane = glm::normalize(glm::vec3(0.01, 1.2, 0.2));
            body.host = &star;

            auto bodypos = body.getPosition(1234.0);

            Logger::WriteMessage(("Body position (X: "
                + std::to_string(bodypos.x)
                + " , Y: "
                + std::to_string(bodypos.y)
                + " , Z: "
                + std::to_string(bodypos.z)
                + " )\n").c_str());

            double expectedAtZero = 7.905664;
            double expectedAtSurface = 7.905664;
            double expectedAt100KM = 7.844341;
            double expectedAt1000KM = 7.349858;

            double resultAtZero = body.calculateOrbitVelocity(0.0);
            double resultAtSurface = body.calculateOrbitVelocity(0.0);
            double resultAt100KM = body.calculateOrbitVelocity(100.0);
            double resultAt1000KM = body.calculateOrbitVelocity(1000.0);

            Logger::WriteMessage(("resultAtZero " + std::to_string(resultAtZero) + "\n").c_str());
            Logger::WriteMessage(("resultAtSurface " + std::to_string(resultAtSurface) + "\n").c_str());
            Logger::WriteMessage(("resultAt100KM " + std::to_string(resultAt100KM) + "\n").c_str());
            Logger::WriteMessage(("resultAt1000KM " + std::to_string(resultAt1000KM) + "\n").c_str());

            Assert::IsTrue(glm::abs(resultAtZero - expectedAtZero) < 0.01);
            Assert::IsTrue(glm::abs(resultAtSurface - expectedAtSurface) < 0.01);
            Assert::IsTrue(glm::abs(expectedAt100KM - expectedAt100KM) < 0.01);
            Assert::IsTrue(glm::abs(expectedAt1000KM - expectedAt1000KM) < 0.01);

        }

    };
}