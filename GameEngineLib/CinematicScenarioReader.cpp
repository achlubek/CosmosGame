#include "stdafx.h"
#include "CinematicScenarioReader.h"


CinematicScenarioReader::CinematicScenarioReader(Interpolator* iinterpolator, CosmosRenderer* icosmos, Media* media, PointerDrivenCameraStrategy* icamera, TimeProvider* itime)
    : tasks({}), cosmos(icosmos), interpolator(iinterpolator), camera(icamera), time(itime), media(media)
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

static void replaceAll(std::string& source, const std::string& from, const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while (std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    newString += source.substr(lastPos);

    source.swap(newString);
}

void CinematicScenarioReader::load(std::string mediakey, double timeOffset)
{
    double frameTime = timeOffset;
    AbsCelestialObject* referenceFrame = &cosmos->getGalaxy()->getClosestCelestialBody();
    auto lines = splitByChar(media->readString(mediakey), '\n');
  //  double* exposurePointer = &cosmos->exposure;
    glm::dvec3 currentPos = glm::dvec3(0.0);
    glm::dvec3* positionPointer = camera->getPositionPointer();
    glm::dquat* orientationPointer = camera->getOrientationPointer();
    double* fovPointer = camera->getFovPointer();
    for (int i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        replaceAll(line, ",", "");
        replaceAll(line, "  ", " ");
        replaceAll(line, "(", "");
        replaceAll(line, ")", "");
        auto words = splitByChar(line, ' ');
        if (words[0] == "FRAME") {
            frameTime = timeOffset + std::stod(words[1]);
        }
        else if (words[0] == "REFERENCE") {
            referenceFrame = cosmos->getGalaxy()->getByPath(std::stoi(words[1]), std::stoi(words[2]), std::stoi(words[3]));
        }
        else if (words[0] == "INTERPOLATE") {
            int w = 1;
            double smoothness = std::stod(words[w++]);
            double duration = std::stod(words[w++]);
            std::string prop = words[w++];

            if (prop == "POS") {
                glm::dvec3 target = glm::dvec3(std::stod(words[w]), std::stod(words[w + 1]), std::stod(words[w + 2]));
                w += 3;
                auto task = new Vec3InterpolatorTask(positionPointer, referenceFrame->getPosition(timeOffset) + target, frameTime, duration, smoothness);
                task->setTag(mediakey);
                animationDuration = max(animationDuration, frameTime - timeOffset + duration);
                tasks.push_back(task);
            }
            else if (prop == "LOOKAT") {
                glm::dvec3 origin = glm::dvec3(std::stod(words[w]), std::stod(words[w + 1]), std::stod(words[w + 2]));
                w += 3;
                glm::dvec3 target = glm::dvec3(std::stod(words[w]), std::stod(words[w + 1]), std::stod(words[w + 2]));
                w += 3;
                glm::dvec3 up =  glm::dvec3(std::stod(words[w]), std::stod(words[w+1]), std::stod(words[w+2]));
                w += 3;
                glm::dquat res = glm::inverse(glm::quat_cast(glm::lookAt(glm::vec3(0.0), glm::vec3(glm::normalize(target - origin)), glm::vec3(up))));
                auto task = new QuatInterpolatorTask(orientationPointer, res, frameTime, duration, smoothness);
                task->setTag(mediakey);
                animationDuration = max(animationDuration, frameTime - timeOffset + duration);
                tasks.push_back(task);
            }
            else if (prop == "EXPOSURE") {
              //    auto task = new DoubleInterpolatorTask(exposurePointer, std::stod(words[4]), frameTime, duration, smoothness);
              //    tasks.push_back(task);
            }
            else if (prop == "FOV") {
                auto task = new DoubleInterpolatorTask(fovPointer, std::stod(words[w++]), frameTime, duration, smoothness);
                task->setTag(mediakey);
                animationDuration = max(animationDuration, frameTime - timeOffset + duration);
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

double CinematicScenarioReader::getAnimationDuration()
{
    return animationDuration;
}
