#include "stdafx.h"
#include "CinematicScenarioReader.h"


CinematicScenarioReader::CinematicScenarioReader(Interpolator* iinterpolator, CosmosRenderer* icosmos, PointerDrivenCameraStrategy* icamera, TimeProvider* itime)
    : tasks({}), cosmos(icosmos), interpolator(iinterpolator), camera(icamera), time(itime)
{

}


CinematicScenarioReader::~CinematicScenarioReader()
{
}

static vector<string> splitByChar(string src, unsigned char splitter)
{
    vector<string> output = {};
    int i = 0, d = 0;
    while (i < src.size()) {
        if (src[i] == splitter) {
            output.push_back(src.substr(d, i - d));
            d = i;
            while (src[i++] == splitter)  d++;
        }
        else {
            i++;
        }
    }
    if (i == src.size() && d < i) {
        output.push_back(src.substr(d, i));
    }
    return output;
}
void CinematicScenarioReader::load(std::string mediakey, double timeOffset)
{
    double frameTime = timeOffset;
    AbsCelestialObject* referenceFrame = &cosmos->galaxy->getClosestCelestialBody();
    auto lines = splitByChar(Media::readString(mediakey), '\n');
    double* exposurePointer = &cosmos->exposure;
    glm::dvec3 currentPos = glm::dvec3(0.0);
    glm::dvec3* positionPointer = camera->getPositionPointer();
    glm::dquat* orientationPointer = camera->getOrientationPointer();
    double* fovPointer = camera->getFovPointer();
    for (int i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        auto words = splitByChar(line, ' ');
        if (words[0] == "FRAME") {
            frameTime = timeOffset + std::stod(words[1]);
        }
        else if (words[0] == "REFERENCE") {
            referenceFrame = cosmos->galaxy->getByPath(std::stoi(words[1]), std::stoi(words[2]), std::stoi(words[3]));
        }
        else if (words[0] == "INTERPOLATE") {
            double smoothness = std::stod(words[1]);
            double duration = std::stod(words[2]);
            std::string prop = words[3];
            if (prop == "POS") {
                glm::dvec3 target = referenceFrame->getPosition(timeOffset) + glm::dvec3(std::stod(words[4]), std::stod(words[5]), std::stod(words[6]));
                auto task = new Vec3InterpolatorTask(positionPointer, target, 0.0, duration, smoothness);
                tasks.push_back(task);
            }
            else if (prop == "LOOKAT") {
              //  glm::dvec3 target = glm::dvec3(std::stod(words[4]), std::stod(words[5]), std::stod(words[6]));
              //  auto task = new Vec3InterpolatorTask(positionPointer, target, 0.0, duration, smoothness);
              //  tasks.push_back(task);
            }
            else if (prop == "EXPOSURE") {
                auto task = new DoubleInterpolatorTask(exposurePointer, std::stod(words[4]), 0.0, duration, smoothness);
                tasks.push_back(task);
            }
            else if (prop == "FOV") {
                auto task = new DoubleInterpolatorTask(fovPointer, std::stod(words[4]), 0.0, duration, smoothness);
                tasks.push_back(task);
            }
        }
    }
}

void CinematicScenarioReader::execute()
{
    for (int i = 0; i < tasks.size(); i++) {
        interpolator->addPremadeInterpolator(tasks[i]);
    }
}
