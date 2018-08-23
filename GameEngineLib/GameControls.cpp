#include "stdafx.h"
#include "GameControls.h"
#include "INIReader.h"
using namespace std;


GameControls::GameControls(Keyboard* ikeyboard, Mouse* imouse, Media* media, std::string inifile)
    : keyboard(ikeyboard), mouse(imouse), media(media)
{
    INIReader reader = INIReader(media, inifile);
    INIReader keynamemap = INIReader(media, "keys_name_value_map.ini");
    auto allkeys = reader.getAllKeys();
    for (auto key : allkeys) {
        auto val = reader.gets(key);
        auto semicolonSplit = splitByChar(val, ';');
        if (semicolonSplit.size() < 2) continue;
        if (semicolonSplit[0] == "key") {
            int keycode = keynamemap.geti(semicolonSplit[1]);
            simpleKeyBinds[key] = keycode;
        }
        else if (semicolonSplit[0] == "keyaxis") {
            bool keepval = semicolonSplit[1] == "keep";
            auto valuesstr = semicolonSplit[2];
            auto valuesmap = splitByChar(valuesstr, ',');
            std::vector<KeyValuePair> binds = {};
            for (auto valuekeyvalstr : valuesmap) {
                auto valueskeyvalsplit = splitByChar(valuekeyvalstr, ':');
                int keyval = keynamemap.geti(valueskeyvalsplit[0]);
                double value = std::stod(valueskeyvalsplit[1]);
                binds.push_back(KeyValuePair(keyval, value));
            }
            keysAxisBinds[key] = ControlKeyAxis(binds, keepval);
        }
    }
    onKeyDown = EventHandler<std::string>();
    onKeyUp = EventHandler<std::string>();    
    
    keyboard->onKeyPress.add([&](int key) {
        for (const auto &p : simpleKeyBinds)
        {
            if (p.second == key) {
                onKeyDown.invoke(p.first);
            }
        }
        for (auto &p : keysAxisBinds) {
            p.second.onKeyDown(key);
        }
    });

    keyboard->onKeyRelease.add([&](int key) {
        for (const auto &p : simpleKeyBinds)
        {
            if (p.second == key) {
                onKeyUp.invoke(p.first);
            }
        }
        for (auto &p : keysAxisBinds) {
            p.second.onKeyUp(key);
        }
    });
}


GameControls::~GameControls()
{
}

bool GameControls::isKeyDown(std::string name)
{
    return keyboard->getKeyStatus(simpleKeyBinds[name]) == GLFW_PRESS;
}

double GameControls::readAxisValue(std::string name)
{
    return keysAxisBinds[name].currentValue;
}

glm::ivec2 GameControls::getCursorPosition()
{
    auto tup = mouse->getCursorPosition();
    return glm::ivec2(get<0>(tup), get<1>(tup));
}

Keyboard * GameControls::getRawKeyboard()
{
    return keyboard;
}

Mouse * GameControls::getRawMouse()
{
    return mouse;
}
 