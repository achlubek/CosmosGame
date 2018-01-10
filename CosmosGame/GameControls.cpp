#include "stdafx.h"
#include "GameControls.h"
#include "INIReader.h"
using namespace std;

vector<string> splitByChar(string src, unsigned char splitter)
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

GameControls::GameControls(Keyboard* ikeyboard, std::string inifile)
    : keyboard(ikeyboard)
{
    INIReader reader = INIReader(inifile);
    INIReader keynamemap = INIReader("keys_name_value_map.ini");
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
 