#pragma once
class INIReader
{
public:
    INIReader(VEngine::FileSystem::MediaInterface* media, std::string mediakey);
    INIReader(std::string inistring);
    float getf(std::string key);
    double getd(std::string key);
    glm::vec2 getv2(std::string key);
    glm::vec3 getv3(std::string key);
    glm::quat getquat(std::string key);

    glm::dvec2 getdv2(std::string key);
    glm::dvec3 getdv3(std::string key);
    glm::dquat getdquat(std::string key);

    void splitBySpaces(std::vector<std::string>& output, std::string src);
    int geti(std::string key);
    int64_t geti64(std::string key);
    uint64_t getu64(std::string key);
    std::string gets(std::string key);
    std::vector<std::string> getAllKeys();
    ~INIReader();
private:
    std::vector<std::string> allKeys;
    std::unordered_map<std::string, std::string> data;
    void readString(std::string str);
};
