#include "stdafx.h"
#include "GameControls.h"

GameControls::GameControls(KeyboardInterface* keyboard, MouseInterface* mouse, MediaInterface* media, EventBus* ieventBus, std::string inifile)
    : keyboard(keyboard), mouse(mouse), media(media), eventBus(ieventBus)
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
    
    keyboard->setOnKeyPressHandler([&](int key) {
        for (const auto &p : simpleKeyBinds)
        {
            if (p.second == key) {
                eventBus->enqueue(new OnKeyPressEvent(p.first));
            }
        }
        for (auto &p : keysAxisBinds) {
            p.second.onKeyDown(key);
        }
    });

    keyboard->setOnKeyReleaseHandler([&](int key) {
        for (const auto &p : simpleKeyBinds)
        {
            if (p.second == key) {
                eventBus->enqueue(new OnKeyReleaseEvent(p.first));
            }
        }
        for (auto &p : keysAxisBinds) {
            p.second.onKeyUp(key);
        }
    });

    keyboard->setOnKeyRepeatHandler([&](int key) {
    });

    keyboard->setOnCharHandler([&](unsigned int key) {
    });


    mouse->setOnMouseDownHandler([&](int key) {
    });

    mouse->setOnMouseUpHandler([&](int key) {
    });

    mouse->setOnMouseScrollHandler([&](double x, double y) {
    });
}


GameControls::~GameControls()
{
}

bool GameControls::isKeyDown(std::string name)
{
    return keyboard->isKeyDown(simpleKeyBinds[name]);
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
