#pragma once

#include <stdio.h>
#include <tchar.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <functional>
#include <thread>
#include <algorithm>
#include <queue>
#include <regex>
#include <map>
#include <unordered_map>
#include <array>
#include <fstream>
#include <iomanip>
using namespace std;

#include <VEngine.h>
#include <VEUserInterface.h>

using namespace VEngine::Input;
using namespace VEngine::Renderer;
using namespace VEngine::FileSystem;
using namespace VEngine::Utilities;
using namespace VEngine::UserInterface;


//#include "glad.h"
//#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

#include <GLFW/glfw3.h>
#include <memory>
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE 1
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "SQLiteDatabase.h"
#include "CosmosRendererLib.h"
#include "GameEngineLib.h"

#define PI 3.141592f
#define rad2deg(a) (a * (180.0f / PI))
#define deg2rad(a) (a * (PI / 180.0f))
#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}

std::string to_string_with_precision(double a_value, const int n = 6)
{
    std::ostringstream out;
    out << std::setprecision(n) << a_value;
    return out.str();
}

vector<string> splitByChar(string src, unsigned char splitter);